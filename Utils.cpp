#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


extern SDL_Window *window;





// Check if the file exists
bool FileExists(const char *fileName)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1) result = true;
#else
    if (access(fileName, F_OK) != -1) result = true;
#endif

    return result;
}

// Check file extension
// NOTE: Extensions checking is not case-sensitive
bool IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;
    const char *fileExt = GetFileExtension(fileName);

    if (fileExt != NULL)
    {

        int extCount = 0;
        const char **checkExts = TextSplit(ext, ';', &extCount);

        char fileExtLower[16] = { 0 };
        strcpy(fileExtLower, TextToLower(fileExt));

        for (int i = 0; i < extCount; i++)
        {
            if (TextIsEqual(fileExtLower, TextToLower(checkExts[i] + 1)))
            {
                result = true;
                break;
            }
        }
    }

    return result;
}

// Check if a directory path exists
bool DirectoryExists(const char *dirPath)
{
    bool result = false;
    DIR *dir = opendir(dirPath);

    if (dir != NULL)
    {
        result = true;
        closedir(dir);
    }

    return result;
}

// Get pointer to extension for a filename string
const char *GetFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return (dot + 1);
}

// String pointer reverse break: returns right-most occurrence of charset in s
static const char *strprbrk(const char *s, const char *charset)
{
    const char *latestMatch = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) { }
    return latestMatch;
}

// Get pointer to filename for a path string
const char *GetFileName(const char *filePath)
{
    const char *fileName = NULL;
    if (filePath != NULL) fileName = strprbrk(filePath, "\\/");

    if (!fileName) return filePath;

    return fileName + 1;
}

// Get filename string without extension (uses static string)
const char *GetFileNameWithoutExt(const char *filePath)
{
    #define MAX_FILENAMEWITHOUTEXT_LENGTH   128

    static char fileName[MAX_FILENAMEWITHOUTEXT_LENGTH];
    memset(fileName, 0, MAX_FILENAMEWITHOUTEXT_LENGTH);

    if (filePath != NULL) strcpy(fileName, GetFileName(filePath));   // Get filename with extension

    int len = (int)strlen(fileName);

    for (int i = 0; (i < len) && (i < MAX_FILENAMEWITHOUTEXT_LENGTH); i++)
    {
        if (fileName[i] == '.')
        {
            // NOTE: We break on first '.' found
            fileName[i] = '\0';
            break;
        }
    }

    return fileName;
}

// Get directory for a given filePath
const char *GetDirectoryPath(const char *filePath)
{
    // NOTE: Directory separator is different in Windows and other platforms,
    // fortunately, Windows also support the '/' separator, that's the one should be used
    #if defined(_WIN32)
        char separator = '\\';
    #else
        char separator = '/';
    #endif

    const char *lastSlash = NULL;
    static char dirPath[MAX_FILEPATH_LENGTH];
    memset(dirPath, 0, MAX_FILEPATH_LENGTH);

    // In case provided path does not contain a root drive letter (C:\, D:\) nor leading path separator (\, /),
    // we add the current directory path to dirPath
    if (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/')
    {
        // For security, we set starting path to current directory,
        // obtained path will be concated to this
        dirPath[0] = '.';
        dirPath[1] = '/';
    }

    lastSlash = strprbrk(filePath, "\\/");
    if (lastSlash)
    {
        if (lastSlash == filePath)
        {
            // The last and only slash is the leading one: path is in a root directory
            dirPath[0] = filePath[0];
            dirPath[1] = '\0';
        }
        else
        {
            // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
            memcpy(dirPath + (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/' ? 2 : 0), filePath, strlen(filePath) - (strlen(lastSlash) - 1));
            dirPath[strlen(filePath) - strlen(lastSlash) + (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/' ? 2 : 0)] = '\0';  // Add '\0' manually
        }
    }

    return dirPath;
}

// Get previous directory path for a given path
const char *GetPrevDirectoryPath(const char *dirPath)
{
    static char prevDirPath[MAX_FILEPATH_LENGTH];
    memset(prevDirPath, 0, MAX_FILEPATH_LENGTH);
    int pathLen = (int)strlen(dirPath);

    if (pathLen <= 3) strcpy(prevDirPath, dirPath);

    for (int i = (pathLen - 1); (i >= 0) && (pathLen > 3); i--)
    {
        if ((dirPath[i] == '\\') || (dirPath[i] == '/'))
        {
            // Check for root: "C:\" or "/"
            if (((i == 2) && (dirPath[1] ==':')) || (i == 0)) i++;

            strncpy(prevDirPath, dirPath, i);
            break;
        }
    }

    return prevDirPath;
}

// Get current working directory
const char *GetWorkingDirectory(void)
{
    static char currentDir[MAX_FILEPATH_LENGTH];
    memset(currentDir, 0, MAX_FILEPATH_LENGTH);

    char *ptr = GETCWD(currentDir, MAX_FILEPATH_LENGTH - 1);

    return ptr;
}

static char **dirFilesPath = NULL;          // Store directory files paths as strings
static int dirFilesCount = 0;               // Count directory files strings


// Get filenames in a directory path (max 512 files)
// NOTE: Files count is returned by parameters pointer
char **GetDirectoryFiles(const char *dirPath, int *fileCount)
{
    #define MAX_DIRECTORY_FILES     512

    ClearDirectoryFiles();

    // Memory allocation for MAX_DIRECTORY_FILES
    dirFilesPath = (char **)SDL_malloc(sizeof(char *)*MAX_DIRECTORY_FILES);

    for (int i = 0; i < MAX_DIRECTORY_FILES; i++) dirFilesPath[i] = (char *)SDL_malloc(sizeof(char)*MAX_FILEPATH_LENGTH);

    int counter = 0;
    struct dirent *entity;
    DIR *dir = opendir(dirPath);

    if (dir != NULL)  // It's a directory
    {
        // TODO: Reading could be done in two passes,
        // first one to count files and second one to read names
        // That way we can allocate required memory, instead of a limited pool

        while ((entity = readdir(dir)) != NULL)
        {
            strcpy(dirFilesPath[counter], entity->d_name);
            counter++;
        }

        closedir(dir);
    }
    else SDL_Log( "FILEIO: Failed to open requested directory");  // Maybe it's a file...

    dirFilesCount = counter;
    *fileCount = dirFilesCount;

    return dirFilesPath;
}

// Clear directory files paths buffers
void ClearDirectoryFiles(void)
{
    if (dirFilesCount > 0)
    {
        for (int i = 0; i < MAX_DIRECTORY_FILES; i++) SDL_free(dirFilesPath[i]);

        SDL_free(dirFilesPath);
    }

    dirFilesCount = 0;
}

// Change working directory, returns true on success
bool ChangeDirectory(const char *dir)
{
    bool result = CHDIR(dir);

    if (result != 0) SDL_Log( "SYSTEM: Failed to change to directory: %s", dir);

    return (result == 0);
}
// Get file modification time (last write time)
long GetFileModTime(const char *fileName)
{
    struct stat result = { 0 };

    if (stat(fileName, &result) == 0)
    {
        time_t mod = result.st_mtime;

        return (long)mod;
    }

    return 0;
}


// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given.
// A user could craft a malicious string performing another action.
// Only call this function yourself not with user input or make sure to check the string yourself.
// Ref: https://github.com/raysan5/raylib/issues/686
void OpenURL(const char *url)
{
    // Small security check trying to avoid (partially) malicious code...
    // sorry for the inconvenience when you hit this point...
    if (strchr(url, '\'') != NULL)
    {
        SDL_Log( "SYSTEM: Provided URL is not valid");
    }
    else
    {
#if defined(PLATFORM_DESKTOP)
        char *cmd = (char *)SCALLOC(strlen(url) + 10, sizeof(char));
    #if defined(_WIN32)
        sprintf(cmd, "explorer %s", url);
    #elif defined(__linux__)
        sprintf(cmd, "xdg-open '%s'", url); // Alternatives: firefox, x-www-browser
    #elif defined(__APPLE__)
        sprintf(cmd, "open '%s'", url);
    #endif
        system(cmd);
        SDL_free(cmd);
#endif
#if defined(PLATFORM_WEB)
        emscripten_run_script(TextFormat("window.open('%s', '_blank')", url));
#endif
    }
}


//----------------------------------------------------------------------------------
// Text strings management functions
//----------------------------------------------------------------------------------
// Get text length in bytes, check for \0 character
unsigned int TextLength(const char *text)
{
    unsigned int length = 0; //strlen(text)

    if (text != NULL)
    {
        while (*text++) length++;
    }

    return length;
}

// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
const char *TextFormat(const char *text, ...)
{

    #define MAX_TEXTFORMAT_BUFFERS 4        // Maximum number of static buffers for text formatting

    // We create an array of buffers so strings don't expire until MAX_TEXTFORMAT_BUFFERS invocations
    static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
    static int  index = 0;

    char *currentBuffer = buffers[index];
    memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH);   // Clear buffer before using

    va_list args;
    va_start(args, text);
    vsprintf(currentBuffer, text, args);
    va_end(args);

    index += 1;     // Move to next buffer for next function call
    if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;

    return currentBuffer;
}

// Get integer value from text
// NOTE: This function replaces atoi() [stdlib.h]
int TextToInteger(const char *text)
{
    int value = 0;
    int sign = 1;

    if ((text[0] == '+') || (text[0] == '-'))
    {
        if (text[0] == '-') sign = -1;
        text++;
    }

    for (int i = 0; ((text[i] >= '0') && (text[i] <= '9')); ++i) value = value*10 + (int)(text[i] - '0');

    return value*sign;
}


// Encode codepoint into utf8 text (char array length returned as parameter)
 const char *CodepointToUtf8(int codepoint, int *byteLength)
{
    static char utf8[6] = { 0 };
    int length = 0;

    if (codepoint <= 0x7f)
    {
        utf8[0] = (char)codepoint;
        length = 1;
    }
    else if (codepoint <= 0x7ff)
    {
        utf8[0] = (char)(((codepoint >> 6) & 0x1f) | 0xc0);
        utf8[1] = (char)((codepoint & 0x3f) | 0x80);
        length = 2;
    }
    else if (codepoint <= 0xffff)
    {
        utf8[0] = (char)(((codepoint >> 12) & 0x0f) | 0xe0);
        utf8[1] = (char)(((codepoint >>  6) & 0x3f) | 0x80);
        utf8[2] = (char)((codepoint & 0x3f) | 0x80);
        length = 3;
    }
    else if (codepoint <= 0x10ffff)
    {
        utf8[0] = (char)(((codepoint >> 18) & 0x07) | 0xf0);
        utf8[1] = (char)(((codepoint >> 12) & 0x3f) | 0x80);
        utf8[2] = (char)(((codepoint >>  6) & 0x3f) | 0x80);
        utf8[3] = (char)((codepoint & 0x3f) | 0x80);
        length = 4;
    }

    *byteLength = length;

    return utf8;
}

// Get all codepoints in a string, codepoints count returned by parameters
int *GetCodepoints(const char *text, int *count)
{
    static int codepoints[MAX_TEXT_UNICODE_CHARS] = { 0 };
    memset(codepoints, 0, MAX_TEXT_UNICODE_CHARS*sizeof(int));

    int bytesProcessed = 0;
    int textLength = TextLength(text);
    int codepointsCount = 0;

    for (int i = 0; i < textLength; codepointsCount++)
    {
        codepoints[codepointsCount] = GetNextCodepoint(text + i, &bytesProcessed);
        i += bytesProcessed;
    }

    *count = codepointsCount;

    return codepoints;
}

// Returns total number of characters(codepoints) in a UTF8 encoded text, until '\0' is found
// NOTE: If an invalid UTF8 sequence is encountered a '?'(0x3f) codepoint is counted instead
int GetCodepointsCount(const char *text)
{
    unsigned int len = 0;
    char *ptr = (char *)&text[0];

    while (*ptr != '\0')
    {
        int next = 0;
        int letter = GetNextCodepoint(ptr, &next);

        if (letter == 0x3f) ptr += 1;
        else ptr += next;

        len++;
    }

    return len;
}


// Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found
// When a invalid UTF8 byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
// Total number of bytes processed are returned as a parameter
// NOTE: the standard says U+FFFD should be returned in case of errors
// but that character is not supported by the default font in raylib
// TODO: optimize this code for speed!!
int GetNextCodepoint(const char *text, int *bytesProcessed)
{
/*
    UTF8 specs from https://www.ietf.org/rfc/rfc3629.txt

    Char. number range  |        UTF-8 octet sequence
      (hexadecimal)    |              (binary)
    --------------------+---------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
    // NOTE: on decode errors we return as soon as possible

    int code = 0x3f;   // Codepoint (defaults to '?')
    int octet = (unsigned char)(text[0]); // The first UTF8 octet
    *bytesProcessed = 1;

    if (octet <= 0x7f)
    {
        // Only one octet (ASCII range x00-7F)
        code = text[0];
    }
    else if ((octet & 0xe0) == 0xc0)
    {
        // Two octets
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        unsigned char octet1 = text[1];

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytesProcessed = 2; return code; } // Unexpected sequence

        if ((octet >= 0xc2) && (octet <= 0xdf))
        {
            code = ((octet & 0x1f) << 6) | (octet1 & 0x3f);
            *bytesProcessed = 2;
        }
    }
    else if ((octet & 0xf0) == 0xe0)
    {
        // Three octets
        unsigned char octet1 = text[1];
        unsigned char octet2 = '\0';

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytesProcessed = 2; return code; } // Unexpected sequence

        octet2 = text[2];

        if ((octet2 == '\0') || ((octet2 >> 6) != 2)) { *bytesProcessed = 3; return code; } // Unexpected sequence

        /*
            [0]xE0    [1]xA0-BF       [2]UTF8-tail(x80-BF)
            [0]xE1-EC [1]UTF8-tail    [2]UTF8-tail(x80-BF)
            [0]xED    [1]x80-9F       [2]UTF8-tail(x80-BF)
            [0]xEE-EF [1]UTF8-tail    [2]UTF8-tail(x80-BF)
        */

        if (((octet == 0xe0) && !((octet1 >= 0xa0) && (octet1 <= 0xbf))) ||
            ((octet == 0xed) && !((octet1 >= 0x80) && (octet1 <= 0x9f)))) { *bytesProcessed = 2; return code; }

        if ((octet >= 0xe0) && (0 <= 0xef))
        {
            code = ((octet & 0xf) << 12) | ((octet1 & 0x3f) << 6) | (octet2 & 0x3f);
            *bytesProcessed = 3;
        }
    }
    else if ((octet & 0xf8) == 0xf0)
    {
        // Four octets
        if (octet > 0xf4) return code;

        unsigned char octet1 = text[1];
        unsigned char octet2 = '\0';
        unsigned char octet3 = '\0';

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytesProcessed = 2; return code; }  // Unexpected sequence

        octet2 = text[2];

        if ((octet2 == '\0') || ((octet2 >> 6) != 2)) { *bytesProcessed = 3; return code; }  // Unexpected sequence

        octet3 = text[3];

        if ((octet3 == '\0') || ((octet3 >> 6) != 2)) { *bytesProcessed = 4; return code; }  // Unexpected sequence

        /*
            [0]xF0       [1]x90-BF       [2]UTF8-tail  [3]UTF8-tail
            [0]xF1-F3    [1]UTF8-tail    [2]UTF8-tail  [3]UTF8-tail
            [0]xF4       [1]x80-8F       [2]UTF8-tail  [3]UTF8-tail
        */

        if (((octet == 0xf0) && !((octet1 >= 0x90) && (octet1 <= 0xbf))) ||
            ((octet == 0xf4) && !((octet1 >= 0x80) && (octet1 <= 0x8f)))) { *bytesProcessed = 2; return code; } // Unexpected sequence

        if (octet >= 0xf0)
        {
            code = ((octet & 0x7) << 18) | ((octet1 & 0x3f) << 12) | ((octet2 & 0x3f) << 6) | (octet3 & 0x3f);
            *bytesProcessed = 4;
        }
    }

    if (code > 0x10ffff) code = 0x3f;     // Codepoints after U+10ffff are invalid

    return code;
}

// Copy one string to another, returns bytes copied
int TextCopy(char *dst, const char *src)
{
    int bytes = 0;

    if (dst != NULL)
    {
        while (*src != '\0')
        {
            *dst = *src;
            dst++;
            src++;

            bytes++;
        }

        *dst = '\0';
    }

    return bytes;
}

// Check if two text string are equal
// REQUIRES: strcmp()
bool TextIsEqual(const char *text1, const char *text2)
{
    bool result = false;

    if (strcmp(text1, text2) == 0) result = true;

    return result;
}

// Get a piece of a text string
const char *TextSubtext(const char *text, int position, int length)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };

    int textLength = TextLength(text);

    if (position >= textLength)
    {
        position = textLength - 1;
        length = 0;
    }

    if (length >= textLength) length = textLength;

    for (int c = 0 ; c < length ; c++)
    {
        *(buffer + c) = *(text + position);
        text++;
    }

    *(buffer + length) = '\0';

    return buffer;
}

// Replace text string
// REQUIRES: strstr(), strncpy(), strcpy()
// WARNING: Internally allocated memory must be freed by the user (if return != NULL)
char *TextReplace(char *text, const char *replace, const char *by)
{
    // Sanity checks and initialization
    if (!text || !replace || !by) return NULL;

    char *result;

    char *insertPoint;      // Next insert point
    char *temp;             // Temp pointer
    int replaceLen;         // Replace string length of (the string to remove)
    int byLen;              // Replacement length (the string to replace replace by)
    int lastReplacePos;     // Distance between replace and end of last replace
    int count;              // Number of replacements

    replaceLen = TextLength(replace);
    if (replaceLen == 0) return NULL;  // Empty replace causes infinite loop during count

    byLen = TextLength(by);

    // Count the number of replacements needed
    insertPoint = text;
    for (count = 0; (temp = strstr(insertPoint, replace)); count++) insertPoint = temp + replaceLen;

    // Allocate returning string and point temp to it
    temp = result =(char*) SDL_malloc(TextLength(text) + (byLen - replaceLen)*count + 1);

    if (!result) return NULL;   // Memory could not be allocated

    // First time through the loop, all the variable are set correctly from here on,
    //    temp points to the end of the result string
    //    insertPoint points to the next occurrence of replace in text
    //    text points to the remainder of text after "end of replace"
    while (count--)
    {
        insertPoint = strstr(text, replace);
        lastReplacePos = (int)(insertPoint - text);
        temp = strncpy(temp, text, lastReplacePos) + lastReplacePos;
        temp = strcpy(temp, by) + byLen;
        text += lastReplacePos + replaceLen; // Move to next "end of replace"
    }

    // Copy remaind text part after replacement to result (pointed by moving temp)
    strcpy(temp, text);

    return result;
}

// Insert text in a specific position, moves all text forward
// WARNING: Allocated memory should be manually freed
char *TextInsert(const char *text, const char *insert, int position)
{
    int textLen = TextLength(text);
    int insertLen =  TextLength(insert);

    char *result = (char *)SDL_malloc(textLen + insertLen + 1);

    for (int i = 0; i < position; i++) result[i] = text[i];
    for (int i = position; i < insertLen + position; i++) result[i] = insert[i];
    for (int i = (insertLen + position); i < (textLen + insertLen); i++) result[i] = text[i];

    result[textLen + insertLen] = '\0';     // Make sure text string is valid!

    return result;
}

// Join text strings with delimiter
// REQUIRES: memset(), memcpy()
const char *TextJoin(const char **textList, int count, const char *delimiter)
{
    static char text[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(text, 0, MAX_TEXT_BUFFER_LENGTH);
    char *textPtr = text;

    int totalLength = 0;
    int delimiterLen = TextLength(delimiter);

    for (int i = 0; i < count; i++)
    {
        int textLength = TextLength(textList[i]);

        // Make sure joined text could fit inside MAX_TEXT_BUFFER_LENGTH
        if ((totalLength + textLength) < MAX_TEXT_BUFFER_LENGTH)
        {
            memcpy(textPtr, textList[i], textLength);
            totalLength += textLength;
            textPtr += textLength;

            if ((delimiterLen > 0) && (i < (count - 1)))
            {
                memcpy(textPtr, delimiter, delimiterLen);
                totalLength += delimiterLen;
                textPtr += delimiterLen;
            }
        }
    }

    return text;
}

// Split string into multiple strings
const char **TextSplit(const char *text, char delimiter, int *count)
{
    // NOTE: Current implementation returns a copy of the provided string with '\0' (string end delimiter)
    // inserted between strings defined by "delimiter" parameter. No memory is dynamically allocated,
    // all used memory is static... it has some limitations:
    //      1. Maximum number of possible split strings is set by MAX_TEXTSPLIT_COUNT
    //      2. Maximum size of text to split is MAX_TEXT_BUFFER_LENGTH

    static const char *result[MAX_TEXTSPLIT_COUNT] = { NULL };
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    result[0] = buffer;
    int counter = 0;

    if (text != NULL)
    {
        counter = 1;

        // Count how many substrings we have on text and point to every one
        for (int i = 0; i < MAX_TEXT_BUFFER_LENGTH; i++)
        {
            buffer[i] = text[i];
            if (buffer[i] == '\0') break;
            else if (buffer[i] == delimiter)
            {
                buffer[i] = '\0';   // Set an end of string at this point
                result[counter] = buffer + i + 1;
                counter++;

                if (counter == MAX_TEXTSPLIT_COUNT) break;
            }
        }
    }

    *count = counter;
    return result;
}

// Append text at specific position and move cursor!
// REQUIRES: strcpy()
void TextAppend(char *text, const char *append, int *position)
{
    strcpy(text + *position, append);
    *position += TextLength(append);
}

// Find first text occurrence within a string
// REQUIRES: strstr()
int TextFindIndex(const char *text, const char *find)
{
    int position = -1;

    char *ptr = (char*)strstr(text, find);

    if (ptr != NULL) position = (int)(ptr - text);

    return position;
}

// Get upper case version of provided string
const char *TextToUpper(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };

    for (int i = 0; i < MAX_TEXT_BUFFER_LENGTH; i++)
    {
        if (text[i] != '\0')
        {
            buffer[i] = (char)toupper(text[i]);
            //if ((text[i] >= 'a') && (text[i] <= 'z')) buffer[i] = text[i] - 32;

            // TODO: Support Utf8 diacritics!
            //if ((text[i] >= 'à') && (text[i] <= 'ý')) buffer[i] = text[i] - 32;
        }
        else { buffer[i] = '\0'; break; }
    }

    return buffer;
}

// Get lower case version of provided string
const char *TextToLower(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };

    for (int i = 0; i < MAX_TEXT_BUFFER_LENGTH; i++)
    {
        if (text[i] != '\0')
        {
            buffer[i] = (char)tolower(text[i]);
            //if ((text[i] >= 'A') && (text[i] <= 'Z')) buffer[i] = text[i] + 32;
        }
        else { buffer[i] = '\0'; break; }
    }

    return buffer;
}

// Get Pascal case notation version of provided string
const char *TextToPascal(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };

    buffer[0] = (char)toupper(text[0]);

    for (int i = 1, j = 1; i < MAX_TEXT_BUFFER_LENGTH; i++, j++)
    {
        if (text[j] != '\0')
        {
            if (text[j] != '_') buffer[i] = text[j];
            else
            {
                j++;
                buffer[i] = (char)toupper(text[j]);
            }
        }
        else { buffer[i] = '\0'; break; }
    }

    return buffer;
}




void UnloadByteArray(ByteArray* f)
{
 if (f)
  {
  if (f->rw) SDL_FreeRW(f->rw);
  }
  SDL_free(f);
}

 ByteArray* NewByteArrayFromFile(const char* filename,const char* mod)
{
ByteArray *file = (ByteArray *)SDL_malloc(sizeof(ByteArray));
file->rw=SDL_RWFromFile(filename, mod);
file->write=true;
if (file->rw)
	{
        SDL_RWseek(file->rw,0,RW_SEEK_END);
		file->size = ByteArrayIsOpen(file);
		SDL_RWseek(file->rw,0,RW_SEEK_SET);
	}else
	{
	 SDL_Log( "Failed to load file [%s] ", filename);
	 return 0;
	}

 SDL_Log( "Load file [%s] size [%d]", filename,file->size);
return file;
}


ByteArray* CreateByteArrayFromMem(void *mem, int size)
{
ByteArray *file = (ByteArray *)SDL_malloc(sizeof(ByteArray));
file->rw=SDL_RWFromMem(mem, size);
file->write=true;
file->size = size;
if (!file->rw)
{
	 SDL_Log( "Failed to create bytearray ");
	 return 0;
	}
return file;
}

ByteArray* CreateByteArrayFromConstMem(const void *mem,int size)
{
ByteArray *file = (ByteArray *)SDL_malloc(sizeof(ByteArray));
file->rw=SDL_RWFromConstMem(mem, size);
file->write=true;
file->size = size;
if (!file->rw)
{
	SDL_Log( "Failed to create bytearray ");
	 return 0;
	}
return file;
}


ByteArray* CreateByteArrayFromFile(const char* filename,bool append)
{
ByteArray *file = (ByteArray *)SDL_malloc(sizeof(ByteArray));

file->rw=SDL_RWFromFile(filename, append ? "ab" : "wb");
file->write=true;
if (file->rw)
	{
        SDL_RWseek(file->rw,0,RW_SEEK_END);
		file->size = ByteArrayIsOpen(file);
		SDL_RWseek(file->rw,0,RW_SEEK_SET);
	}else
	{
	 SDL_Log( "Failed to load file [%s] ", filename);
	 return 0;
	}

return file;
}

ByteArray* OpenByteArrayFromFile(const char* filename)
{
ByteArray *file = (ByteArray *)SDL_malloc(sizeof(ByteArray));
file->rw=SDL_RWFromFile(filename, "rb");
file->write=false;

	if (file->rw)
	{
    	SDL_RWseek(file->rw,0,RW_SEEK_END);
		file->size = ByteArrayIsOpen(file);
		SDL_RWseek(file->rw,0,RW_SEEK_SET);
	} else
	{
	 SDL_Log( "Failed to load file [%s] ", filename);
	 return 0;
	}
return file;
}

Uint32 GetByteArrayPosition(ByteArray* f)
{
      if (!ByteArrayIsOpen(f))
		return 0;
		f->pos = (Uint32)SDL_RWtell(f->rw);
		return f->pos;
}
Uint32 GetByteArraySize(ByteArray* f)
{
      if (!ByteArrayIsOpen(f))
		return 0;

		return f->size;
}



bool ByteArrayIsOpen(ByteArray* f)
{
   if (!f) return false;
	return f->rw != 0;
}

Uint32 ReadByteArray(ByteArray* f,void* buffer, Uint32 sizeToRead)
{

	if (!ByteArrayIsOpen(f))
		return 0;
        f->pos += sizeToRead;
		return (Uint32)SDL_RWread(f->rw,buffer,1,sizeToRead);
}


//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
Uint32 SeekByteArray(ByteArray* f,Uint32 finalPos, bool relativeMovement)
{
	if (!ByteArrayIsOpen(f))
		return 0;

		f->pos += finalPos;
		return SDL_RWseek(f->rw,finalPos,relativeMovement ? RW_SEEK_CUR : RW_SEEK_SET) == 0;
}

Uint32 WriteByteArray(ByteArray* f,void* buffer, Uint32 sizeToRead)
{

	if (!ByteArrayIsOpen(f))
		return 0;

		f->pos += sizeToRead;
		return (Uint32)SDL_RWwrite(f->rw,buffer,1,sizeToRead);
}


int  EofByteArray(ByteArray* f)
{
if (!ByteArrayIsOpen(f))
		return -1;


	int pos=GetByteArrayPosition(f);
	int size = f->size;



	return (pos <= size);

}


int WriteByteArrayByte(ByteArray* f,char c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	return WriteByteArray(f,&c,1);
}

int WriteByteArrayShort(ByteArray* f,short c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	return WriteByteArray(f,&c,2);
}


int WriteByteArrayInt(ByteArray* f,int c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	return WriteByteArray(f,&c,4);
}

int WriteByteArrayLong(ByteArray* f,long c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	return WriteByteArray(f,&c,8);
}

int WriteByteArrayFloat(ByteArray* f,float c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	return WriteByteArray(f,&c,4);
}



int WriteByteArrayString(ByteArray* f,const char* c)
{
	if (!ByteArrayIsOpen(f))
		return 0;
	int strLen =(int) SDL_strlen( c );
	int w = WriteByteArrayInt(f,strLen);
	for(int i=0;i<strLen;i++)
	{
	w += WriteByteArrayByte(f,c[i]);
	}

	return w;
}

int WriteByteArrayLine(ByteArray* f,const char* c)
{

	if (!ByteArrayIsOpen(f))
		return 0;
	size_t strLen = SDL_strlen( c );
	int size=0;
	for(int i=0;i<strLen;i++)
	{
	size += WriteByteArrayByte(f,c[i]);
	}
	char c13=13;
	char c10=10;
	size += WriteByteArrayByte(f,c13);
	size += WriteByteArrayByte(f,c10);
	return size;
}

int WriteByteArrayBytes(ByteArray* f,const char* c,int count)
{

	if (!ByteArrayIsOpen(f))
		return 0;
	int size	= 0;
	for(int i=0;i<count;i++)
	{
	size += WriteByteArrayByte(f,c[i]);
	}

	return size;
}

char ReadByteArrayByte(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    char c;
	ReadByteArray(f,&c,1);
	return c;
}

short ReadByteArrayShort(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    short c;
	ReadByteArray(f,&c,2);
	return c;
}

int ReadByteArrayInt(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    int c;
	ReadByteArray(f,&c,4);
	return c;
}


float ReadByteArrayFloat(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    float c;
	ReadByteArray(f,&c,4);
	return c;
}


long ReadByteArrayLong(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    long c;
	ReadByteArray(f,&c,8);
	return c;
}

char* ReadByteArrayString(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    int length=ReadByteArrayInt(f);
   // printf(" str size %d \n",length);
    char* c=(char *)SDL_malloc((length+1)*sizeof(char));

    ReadByteArray(f,c,length);

  //  printf ( " %s \n",c);

    c[length]='\0';
	return c;
}

char* ReadByteArrayBytes(ByteArray* f,int count)
{

	if (!ByteArrayIsOpen(f))
		return 0;

	char *text =  (char *)SDL_malloc((count+1)*sizeof(char));
	for(int i=0;i<count;i++)
	{
	  text[i] = ReadByteArrayByte(f);
	}

	return text;
}

char* ReadByteArrayLine(ByteArray* f)
{
	if (!ByteArrayIsOpen(f))
		return 0;

    char *text = NULL;
	char c= ReadByteArrayByte(f);
	text = (char *)SDL_malloc((1024)*sizeof(char));

    int index  =0;

	    while(c!=13 && c!=10 && EofByteArray(f)!=1){
		if(c!=0)
		{
			text[index++]=c;
		}
		c= ReadByteArrayByte(f);
	}
    SDL_realloc(text,(index)*sizeof(char));
	text[index] = '\0';

	int pos=-1;

	// pass possible remaining new line character
	if(EofByteArray(f)!=1)
	{
		pos=GetByteArrayPosition(f);
		c=ReadByteArrayByte(f);
		if(c!=13 && c!=10) SeekByteArray(f,pos,false);
	}

	return text;

}



// Load data from file into a buffer
unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead)
{
   unsigned char *data = NULL;
    *bytesRead = 0;

    if (fileName != NULL)
    {
        SDL_RWops* file= SDL_RWFromFile(fileName, "rb");



        if (file != NULL)
        {
            int size =(int) SDL_RWsize(file);

            if (size > 0)
            {
                data = (unsigned char *)malloc(size*sizeof(unsigned char));

                unsigned int count = (unsigned int) SDL_RWread(file, data, sizeof(unsigned char), size);
                *bytesRead = count;

                if (count != size) SDL_Log( "FILEIO: [%s] File partially loaded", fileName);
                else SDL_Log( "FILEIO: [%s] File loaded successfully", fileName);
            }
            else SDL_Log( "FILEIO: [%s] Failed to read file", fileName);
            SDL_RWclose(file);
        }
        else SDL_Log( "FILEIO: [%s] Failed to open file", fileName);
    }
    else SDL_Log( "FILEIO: File name provided is not valid");
    return data;
}




// Unload file data allocated by LoadFileData()
void UnloadFileData(unsigned char *data)
{
    SDL_free(data);
}

// Save data to file from buffer
bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite)
{
    bool success = false;

    if (fileName != NULL)
    {
        SDL_RWops* file= SDL_RWFromFile(fileName, "wb");


        if (file != NULL)
        {
            //unsigned int count = (unsigned int)fwrite(data, sizeof(unsigned char), bytesToWrite, file);

            unsigned int count = (unsigned int) SDL_RWwrite(file, data, sizeof(unsigned char), bytesToWrite);


            if (count == 0) SDL_Log( "FILEIO: [%s] Failed to write file", fileName);
            else if (count != bytesToWrite) SDL_Log( "FILEIO: [%s] File partially written", fileName);
            else SDL_Log( "FILEIO: [%s] File saved successfully", fileName);

            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else SDL_Log( "FILEIO: [%s] Failed to open file", fileName);
    }
    else SDL_Log( "FILEIO: File name provided is not valid");

    return success;
}

// Load text data from file, returns a '\0' terminated string
// NOTE: text chars array should be freed manually
char *LoadFileText(const char *fileName)
{
    char *text = NULL;

    if (fileName != NULL)
    {

        SDL_RWops* textFile= SDL_RWFromFile(fileName, "rt");



        if (textFile != NULL)
        {
            // WARNING: When reading a file as 'text' file,
            // text mode causes carriage return-linefeed translation...
            // ...but using fseek() should return correct byte-offset

            int size =(int) SDL_RWsize(textFile);


            if (size > 0)
            {
                text = (char *)SDL_malloc((size + 1)*sizeof(char));
            //    unsigned int count = (unsigned int)fread(text, sizeof(char), size, textFile);

                unsigned int count = (unsigned int) SDL_RWread(textFile, text, sizeof( char), size);

                // WARNING: \r\n is converted to \n on reading, so,
                // read bytes count gets reduced by the number of lines
                if (count < size) text = (char*)SDL_realloc(text, count + 1);

                // Zero-terminate the string
                text[count] = '\0';

                SDL_Log( "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else SDL_Log( "FILEIO: [%s] Failed to read text file", fileName);

            SDL_RWclose(textFile);
        }
        else SDL_Log( "FILEIO: [%s] Failed to open text file", fileName);
    }
    else SDL_Log( "FILEIO: File name provided is not valid");

    return text;
}

// Unload file text data allocated by LoadFileText()
void UnloadFileText(unsigned char *text)
{
    SDL_free(text);
}

// Save text data to file (write), string must be '\0' terminated
bool SaveFileText(const char *fileName, char *text)
{
    bool success = false;

    if (fileName != NULL)
    {

         SDL_RWops* file= SDL_RWFromFile(fileName, "wt");


        if (file != NULL)
        {
             size_t strLen = SDL_strlen( text );
	         int count = SDL_RWwrite( file, text, 1, strLen );


            if (count < 0) SDL_Log( "FILEIO: [%s] Failed to write text file", fileName);
            else SDL_Log( "FILEIO: [%s] Text file saved successfully", fileName);

            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else SDL_Log( "FILEIO: [%s] Failed to open text file", fileName);
    }
    else SDL_Log( "FILEIO: File name provided is not valid");

    return success;
}


// Check and log OpenGL error codes
void GLCheckErrors()
{
    int check = 1;
    while (check)
    {
        const GLenum err = glGetError();
        switch (err)
        {
            case GL_NO_ERROR: check = 0; break;
            case 0x0500: SDL_Log( "GL: Error detected: GL_INVALID_ENUM"); break;
            case 0x0501: SDL_Log( "GL: Error detected: GL_INVALID_VALUE"); break;
            case 0x0502: SDL_Log( "GL: Error detected: GL_INVALID_OPERATION"); break;
            case 0x0503: SDL_Log( "GL: Error detected: GL_STACK_OVERFLOW"); break;
            case 0x0504: SDL_Log( "GL: Error detected: GL_STACK_UNDERFLOW"); break;
            case 0x0505: SDL_Log( "GL: Error detected: GL_OUT_OF_MEMORY"); break;
            case 0x0506: SDL_Log( "GL: Error detected: GL_INVALID_FRAMEBUFFER_OPERATION"); break;
            default: SDL_Log( "GL: Error detected: Unknown error code: %x", err); break;
        }
    }

}



void UnloadTexture(unsigned int id)
{
    glDeleteTextures(1, &id);
}

// Load a new attributes buffer
GLuint LoadVertexBuffer(void *buffer, int size, bool dynamic)
{
    GLuint id = 0;

    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, buffer, dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    return id;
}


void UnloadVertexBuffer(GLuint vboId)
{
    glDeleteBuffers(1, &vboId);

}

// Load a new attributes element buffer
GLuint LoadVertexBufferElement(void *buffer, int size, bool dynamic)
{
    GLuint id = 0;

    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    return id;
}

void EnableVertexBuffer(unsigned int id)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void DisableVertexBuffer(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EnableVertexBufferElement(GLuint id)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void DisableVertexBufferElement(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


// Update GPU buffer with new data
// NOTE: dataSize and offset must be provided in bytes
void UpdateVertexBuffer(int bufferId, void *data, int dataSize, int offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, data);
}

bool EnableVertexArray(GLuint vaoId)
{
    bool result = false;
    glBindVertexArray(vaoId);

    return result;
}

void DisableVertexArray(void)
{
    glBindVertexArray(0);
}

void EnableVertexAttribute(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void DisableVertexAttribute(GLuint index)
{
    glDisableVertexAttribArray(index);
}

void DrawVertexArray(GLenum mode,int offset, int count)
{
    glDrawArrays(mode, offset, count);
}

void DrawVertexArrayElements(GLenum mode,int offset, int count, void *buffer)
{
    glDrawElements(mode, count, GL_UNSIGNED_SHORT, (unsigned short*)buffer + offset);
}



GLuint LoadVertexArray(void)
{
    unsigned int vaoId = 0;
    glGenVertexArrays(1, &vaoId);
    return vaoId;
}

void SetVertexAttribute(GLuint index, int compSize, int type, bool normalized, int stride, void *pointer)
{
    glVertexAttribPointer(index, compSize, type, normalized, stride, pointer);

}


void UnloadVertexArray(GLuint vaoId)
{
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vaoId);

}

// Shaders management
//-----------------------------------------------------------------------------------------------

// Load custom shader strings and return program id
GLuint LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)
{
    GLuint program = 0;

    GLint success = 0;
    program = glCreateProgram();

    glAttachShader(program, vShaderId);
    glAttachShader(program, fShaderId);

    // NOTE: Default attribute shader locations must be binded before linking
   /* glBindAttribLocation(program, 0, DEFAULT_SHADER_ATTRIB_NAME_POSITION);
    glBindAttribLocation(program, 1, DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
    glBindAttribLocation(program, 2, DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
    glBindAttribLocation(program, 3, DEFAULT_SHADER_ATTRIB_NAME_COLOR);
    glBindAttribLocation(program, 4, DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
    glBindAttribLocation(program, 5, DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
*/
    // NOTE: If some attrib name is no found on the shader, it locations becomes -1

    glLinkProgram(program);

    // NOTE: All uniform variables are intitialised to 0 when a program links

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        SDL_Log( "SHADER: [ID %i] Failed to link shader program", program);

        int maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0)
        {
            int length = 0;
            char *log = (char*)SDL_calloc(maxLength, sizeof(char));
            glGetProgramInfoLog(program, maxLength, &length, log);
            SDL_Log( "SHADER: [ID %i] Link error: %s", program, log);
            SDL_free(log);
        }

        glDeleteProgram(program);

        program = 0;
    }
    else SDL_Log( "SHADER: [ID %i] Program shader loaded successfully", program);

    return program;
}

// Unload shader program
void UnloadShaderProgram(GLuint id)
{
    glDeleteProgram(id);
    SDL_Log( "SHADER: [ID %i] Unloaded shader program data from VRAM (GPU)", id);

}
// Compile custom shader and return shader id
GLuint CompileShader(const char *shaderCode, int type)
{
    GLuint shader = 0;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);

    GLint success = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        switch (type)
        {
            case GL_VERTEX_SHADER: SDL_Log( "SHADER: [ID %i] Failed to compile vertex shader code", shader); break;
            case GL_FRAGMENT_SHADER: SDL_Log( "SHADER: [ID %i] Failed to compile fragment shader code", shader); break;
//            case GL_GEOMETRY_SHADER: SDL_Log( "SHADER: [ID %i] Failed to compile geometry shader code", shader); break;
            //case GL_COMPUTE_SHADER:
            default: break;
        }

        int maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0)
        {
            int length = 0;
            char *log = (char*)SDL_calloc(maxLength, sizeof(char));
            glGetShaderInfoLog(shader, maxLength, &length, log);
            SDL_Log( "SHADER: [ID %i] Compile error: %s", shader, log);
            SDL_free(log);
        }
    }
    else
    {
        switch (type)
        {
            case GL_VERTEX_SHADER: SDL_Log( "SHADER: [ID %i] Vertex shader compiled successfully", shader); break;
            case GL_FRAGMENT_SHADER: SDL_Log( "SHADER: [ID %i] Fragment shader compiled successfully", shader); break;
//            case GL_GEOMETRY_SHADER: SDL_Log( "SHADER: [ID %i] Geometry shader compiled successfully", shader); break;
            //case GL_COMPUTE_SHADER:
            default: break;
        }
    }

    return shader;
}
// Load shader from code strings
// NOTE: If shader string is NULL, using default vertex/fragment shaders
GLuint LoadShaderCode(const char *vsCode, const char *fsCode)
{
   GLuint id = 0;



   GLuint vertexShaderId ;
   GLuint fragmentShaderId ;

    if (vsCode != NULL) vertexShaderId =    CompileShader(vsCode, GL_VERTEX_SHADER);
    if (fsCode != NULL) fragmentShaderId =  CompileShader(fsCode, GL_FRAGMENT_SHADER);


       id = LoadShaderProgram(vertexShaderId, fragmentShaderId);



        if (id == 0)
        {
            SDL_Log( "SHADER: Failed to load custom shader code");
            return id;
        }


    // Get available shader uniforms
    // NOTE: This information is useful for debug...
    int uniformCount = -1;

    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (int i = 0; i < uniformCount; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;

        // Get the name of the uniforms
        glGetActiveUniform(id, i, sizeof(name) - 1, &namelen, &num, &type, name);

        name[namelen] = 0;

        SDL_Log("SHADER: [ID %i] Active uniform (%s) set at location: %i", id, name, glGetUniformLocation(id, name));
    }


    return id;
}


// Load shader from files and bind default locations
// NOTE: If shader string is NULL, using default vertex/fragment shaders
GLuint LoadShader(const char *vsFileName, const char *fsFileName)
{


    char *vShaderStr = NULL;
    char *fShaderStr = NULL;

    if (vsFileName != NULL) vShaderStr = LoadFileText(vsFileName);
    if (fsFileName != NULL) fShaderStr = LoadFileText(fsFileName);

    GLuint id = LoadShaderCode(vShaderStr, fShaderStr);

    if (vShaderStr != NULL) SDL_free(vShaderStr);
    if (fShaderStr != NULL) SDL_free(fShaderStr);

    // After shader loading, we TRY to set default location names
    if (id > 0)
    {
        // Default shader attrib locations have been fixed before linking:
        //          vertex position location    = 0
        //          vertex texcoord location    = 1
        //          vertex normal location      = 2
        //          vertex color location       = 3
        //          vertex tangent location     = 4
        //          vertex texcoord2 location   = 5

        // NOTE: If any location is not found, loc point becomes -1

    }

    return id;
}

// Load shader from code strings and bind default locations
GLuint LoadShaderFromMemory(const char *vsCode, const char *fsCode)
{

GLuint    id = LoadShaderCode(vsCode, fsCode);

    // After shader loading, we TRY to set default location names
    if (id > 0)
    {
        // Default shader attrib locations have been fixed before linking:
        //          vertex position location    = 0
        //          vertex texcoord location    = 1
        //          vertex normal location      = 2
        //          vertex color location       = 3
        //          vertex tangent location     = 4
        //          vertex texcoord2 location   = 5

        // NOTE: If any location is not found, loc point becomes -1

    }

    return id;
}
// Get shader location uniform
int GetLocationUniform(GLuint shaderId, const char *uniformName)
{
    int location = -1;
    location = glGetUniformLocation(shaderId, uniformName);

    if (location == -1) SDL_Log( "SHADER: [ID %i] Failed to find shader uniform: %s", shaderId, uniformName);
    else SDL_Log( "SHADER: [ID %i] Shader uniform (%s) set at location: %i", shaderId, uniformName, location);
    return location;
}

// Get shader location attribute
int GetLocationAttrib(GLuint shaderId, const char *attribName)
{
    int location = -1;
    location = glGetAttribLocation(shaderId, attribName);

    if (location == -1) SDL_Log( "SHADER: [ID %i] Failed to find shader attribute: %s", shaderId, attribName);
    else SDL_Log( "SHADER: [ID %i] Shader attribute (%s) set at location: %i", shaderId, attribName, location);
    return location;
}



// Set shader value uniform matrix
void SetUniformMatrix(int locIndex, const GLfloat* mat)
{
    glUniformMatrix4fv(locIndex, 1, false, mat);
}

// Set shader value uniform sampler
void SetUniformSampler(int locIndex, GLuint textureId)
{
         //   glUniform1i(locIndex, 1 + i);              // Activate new texture unit

}

// Set shader currently active
void SetShader(GLuint id)
{


}

// Enable depth test
void EnableDepthTest(void) { glEnable(GL_DEPTH_TEST); }

// Disable depth test
void DisableDepthTest(void) { glDisable(GL_DEPTH_TEST); }

// Enable depth write
void EnableDepthMask(void) { glDepthMask(GL_TRUE); }

// Disable depth write
void DisableDepthMask(void) { glDepthMask(GL_FALSE); }

// Enable backface culling
void EnableBackfaceCulling(void) { glEnable(GL_CULL_FACE); }

// Disable backface culling
void DisableBackfaceCulling(void) { glDisable(GL_CULL_FACE); }

// Enable scissor test
void EnableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }

// Disable scissor test
void DisableScissorTest(void) { glDisable(GL_SCISSOR_TEST); }

// Scissor test
void Scissor(int x, int y, int width, int height) { glScissor(x, y, width, height); }

void UpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void *data)
{
    glBindTexture(GL_TEXTURE_2D, id);


    if (format==1)
    {
    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, (unsigned char *)data);
    }


    if (format==2)
    {

    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (unsigned char *)data);
    }


    if (format==3)
    {

    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *)data);
    }

    if (format==4)
    {

    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)data);
    }




}

unsigned int CreateTexture(void *data, int width, int height, int format)
{
    glBindTexture(GL_TEXTURE_2D, 0);    // Free any old binding
    unsigned int id = 0;



    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);              // Generate texture id

    glBindTexture(GL_TEXTURE_2D, id);

    if (format==1)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    }


    if (format==2)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
    }


    if (format==3)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    if (format==4)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR

    glBindTexture(GL_TEXTURE_2D, 0);
    glGenerateMipmap(id);

    if (id > 0) SDL_Log( "TEXTURE: [ID %i] Texture loaded successfully (%ix%i )", id, width, height);
    else SDL_Log( "TEXTURE: Failed to load texture");

    return id;
}


unsigned int CreateTextureMipmaps(void *data, int width, int height, int format,int mipmapCount)
{
    glBindTexture(GL_TEXTURE_2D, 0);    // Free any old binding
    unsigned int id = 0;



    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);              // Generate texture id

    glBindTexture(GL_TEXTURE_2D, id);

    int mipWidth = width;
    int mipHeight = height;
    int mipOffset = 0;          // Mipmap data offset

    // Load the different mipmap levels
    for (int i = 0; i < mipmapCount; i++)
    {
        unsigned int mipSize = 3;

         SDL_Log("TEXTURE: Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mipWidth, mipHeight, mipSize, mipOffset);


            if (format==1)
            {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,mipWidth, mipHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (unsigned char *)data + mipOffset);
            }

            if (format==2)
            {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, mipWidth, mipHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (unsigned char *)data + mipOffset);
            }

            if (format==3)
            {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,mipWidth, mipHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *)data + mipOffset);
            }

            if (format==4)
            {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mipWidth, mipHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)data + mipOffset);
            }
            glGenerateMipmap(GL_TEXTURE_2D);



        mipWidth /= 2;
        mipHeight /= 2;
        mipOffset += mipSize;

        // Security check for NPOT textures
        if (mipWidth < 1) mipWidth = 1;
        if (mipHeight < 1) mipHeight = 1;
    }





    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    if (id > 0) SDL_Log( "TEXTURE: [ID %i] Texture loaded successfully (%ix%i - %i mipmaps)", id, width, height, mipmapCount);
    else SDL_Log("TEXTURE: Failed to load texture");

    return id;
}
// Load depth texture/renderbuffer (to be attached to fbo)
// WARNING: OpenGL ES 2.0 requires GL_OES_depth_texture/WEBGL_depth_texture extensions
unsigned int LoadTextureDepth(int width, int height, bool useRenderBuffer)
{
    unsigned int id = 0;

    bool texDepth=true;
    int maxDepthBits=24;

    // In case depth textures not supported, we force renderbuffer usage
    if (!texDepth) useRenderBuffer = true;

    // NOTE: We let the implementation to choose the best bit-depth
    // Possible formats: GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32 and GL_DEPTH_COMPONENT32F
    unsigned int glInternalFormat = GL_DEPTH_COMPONENT;


    if (maxDepthBits == 32) glInternalFormat =GL_DEPTH_COMPONENT; //GL_DEPTH_COMPONENT32_OES;
    else if (maxDepthBits == 24) glInternalFormat = GL_DEPTH_COMPONENT;//GL_DEPTH_COMPONENT24_OES;
    else glInternalFormat =GL_DEPTH_COMPONENT;// GL_DEPTH_COMPONENT16;


    if (!useRenderBuffer && texDepth)
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);
SDL_Log("TEXTURE: Depth texture loaded successfully");
    }
    else
    {
        // Create the renderbuffer that will serve as the depth attachment for the framebuffer
        // NOTE: A renderbuffer is simpler than a texture and could offer better performance on embedded devices
        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glRenderbufferStorage(GL_RENDERBUFFER, glInternalFormat, width, height);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
SDL_Log( "TEXTURE: [ID %i] Depth renderbuffer loaded successfully (%i bits)", id, (maxDepthBits >= 24)? maxDepthBits : 16);
    }


    return id;
}



// Set texture parameters (wrap mode/filter mode)
void setTextureParameters(unsigned int id, int param, int value)
{
    glBindTexture(GL_TEXTURE_2D, id);
/*
    switch (param)
    {
        case TEXTURE_WRAP_S:
        case TEXTURE_WRAP_T:
        {
            if (value == TEXTURE_WRAP_MIRROR_CLAMP)
            {
                if (CORE.STATE.texMirrorClamp) glTexParameteri(GL_TEXTURE_2D, param, value);
                else TRACELOG(LOG_WARNING, "GL: Clamp mirror wrap mode not supported (GL_MIRROR_CLAMP_EXT)");
            }
            else glTexParameteri(GL_TEXTURE_2D, param, value);

        } break;
        case TEXTURE_MAG_FILTER:
        case TEXTURE_MIN_FILTER: glTexParameteri(GL_TEXTURE_2D, param, value); break;
        case TEXTURE_FILTER_ANISOTROPIC:
        {
            if (value <= CORE.STATE.maxAnisotropyLevel) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            else if (CORE.STATE.maxAnisotropyLevel > 0.0f)
            {
                TRACELOG(LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, CORE.STATE.maxAnisotropyLevel);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            }
            else TRACELOG(LOG_WARNING, "GL: Anisotropic filtering not supported");
        } break;
        default: break;
    }
*/
    glBindTexture(GL_TEXTURE_2D, 0);
}


// Set texture scaling filter mode
void SetTextureFilter(GLuint texture, int filter)
{
/*
    switch (filter)
    {
        case TEXTURE_FILTER_POINT:
        {
            if (texture.mipmaps > 1)
            {
                // RL_TEXTURE_FILTER_MIP_NEAREST - tex filter: POINT, mipmaps filter: POINT (sharp switching between mipmaps)
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER,TEXTURE_FILTER_MIP_NEAREST);

                // RL_TEXTURE_FILTER_NEAREST - tex filter: POINT (no filter), no mipmaps
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_NEAREST);
            }
            else
            {
                // RL_TEXTURE_FILTER_NEAREST - tex filter: POINT (no filter), no mipmaps
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER,TEXTURE_FILTER_NEAREST);
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_NEAREST);
            }
        } break;
        case TEXTURE_FILTER_BILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST - tex filter: BILINEAR, mipmaps filter: POINT (sharp switching between mipmaps)
                // Alternative: RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR - tex filter: POINT, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER, TEXTURE_FILTER_LINEAR_MIP_NEAREST);

                // RL_TEXTURE_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_LINEAR);
            }
            else
            {
                // RL_TEXTURE_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER, TEXTURE_FILTER_LINEAR);
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_LINEAR);
            }
        } break;
        case TEXTURE_FILTER_TRILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // RL_TEXTURE_FILTER_MIP_LINEAR - tex filter: BILINEAR, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER, TEXTURE_FILTER_MIP_LINEAR);

                // RL_TEXTURE_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_LINEAR);
            }
            else
            {
                TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] No mipmaps available for TRILINEAR texture filtering", texture.id);

                // RL_TEXTURE_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                setTextureParameters(texture.id, TEXTURE_MIN_FILTER, TEXTURE_FILTER_LINEAR);
                setTextureParameters(texture.id, TEXTURE_MAG_FILTER, TEXTURE_FILTER_LINEAR);
            }
        } break;
        case TEXTURE_FILTER_ANISOTROPIC_4X: setTextureParameters(texture.id, TEXTURE_FILTER_ANISOTROPIC, 4); break;
        case TEXTURE_FILTER_ANISOTROPIC_8X: setTextureParameters(texture.id, TEXTURE_FILTER_ANISOTROPIC, 8); break;
        case TEXTURE_FILTER_ANISOTROPIC_16X: setTextureParameters(texture.id,TEXTURE_FILTER_ANISOTROPIC, 16); break;
        default: break;
    }
    */
}


// Set texture wrapping mode
void SetTextureWrap(GLuint texture, int wrap)
{
  /*  switch (wrap)
    {
        case TEXTURE_WRAP_REPEAT:
        {
            setTextureParameters(texture.id, TEXTURE_WRAP_S, TEXTURE_WRAP_REPEAT);
            setTextureParameters(texture.id, TEXTURE_WRAP_T, TEXTURE_WRAP_REPEAT);
        } break;
        case TEXTURE_WRAP_CLAMP:
        {
            setTextureParameters(texture.id, TEXTURE_WRAP_S, TEXTURE_WRAP_CLAMP);
            setTextureParameters(texture.id, TEXTURE_WRAP_T, TEXTURE_WRAP_CLAMP);
        } break;
        case TEXTURE_WRAP_MIRROR_REPEAT:
        {
            setTextureParameters(texture.id, TEXTURE_WRAP_S, TEXTURE_WRAP_MIRROR_REPEAT);
            setTextureParameters(texture.id, TEXTURE_WRAP_T, TEXTURE_WRAP_MIRROR_REPEAT);
        } break;
        case TEXTURE_WRAP_MIRROR_CLAMP:
        {
            setTextureParameters(texture.id, TEXTURE_WRAP_S, TEXTURE_WRAP_MIRROR_CLAMP);
            setTextureParameters(texture.id, TEXTURE_WRAP_T, TEXTURE_WRAP_MIRROR_CLAMP);
        } break;
        default: break;
    }*/
}


// Select and active a texture slot
void ActiveTextureSlot(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
}

// Enable texture
void EnableTexture(unsigned int id)
{
    glBindTexture(GL_TEXTURE_2D, id);
}

// Disable texture
void DisableTexture(void)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Enable texture cubemap
void EnableTextureCubemap(unsigned int id)
{
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

// Disable texture cubemap
void DisableTextureCubemap(void)
{
    glDisable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
/*
// Set texture parameters (wrap mode/filter mode)
void TextureParameters(unsigned int id, int param, int value)
{
    glBindTexture(GL_TEXTURE_2D, id);

    switch (param)
    {
        case TEXTURE_WRAP_S:
        case TEXTURE_WRAP_T:
        {
            if (value == TEXTURE_WRAP_MIRROR_CLAMP)
            {
                if (CORE.STATE.texMirrorClamp) glTexParameteri(GL_TEXTURE_2D, param, value);
                else TRACELOG(LOG_WARNING, "GL: Clamp mirror wrap mode not supported (GL_MIRROR_CLAMP_EXT)");
            }
            else glTexParameteri(GL_TEXTURE_2D, param, value);

        } break;
        case TEXTURE_MAG_FILTER:
        case TEXTURE_MIN_FILTER: glTexParameteri(GL_TEXTURE_2D, param, value); break;
        case TEXTURE_FILTER_ANISOTROPIC:
        {
            if (value <=CORE.STATE.maxAnisotropyLevel) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            else if (CORE.STATE.maxAnisotropyLevel > 0.0f)
            {
                TRACELOG(LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, CORE.STATE.maxAnisotropyLevel);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            }
            else SDL_Log("GL: Anisotropic filtering not supported");
        } break;
        default: break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
*/




unsigned int CreateDataTexture(void* data , int width, int height, int channels)
{
   unsigned int  original_texture_format = 0;

	switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			break;
		}
    GLuint ID;
	glGenTextures(1, &ID);              // Generate texture id
	glBindTexture(GL_TEXTURE_2D, ID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		original_texture_format, width, height, 0,
		original_texture_format, GL_UNSIGNED_BYTE, data );
	SDL_free(data);
   // set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

	if (ID > 0) SDL_Log( "TEXTURE: [ID %i] Texture create successfully (%ix%i ) channels(%d) . \n", ID,  width, height,channels);
	return ID;

}
GLuint  LoadTexture(const char* filename,int filter,bool invert)
{
 unsigned int  original_texture_format = 0;
unsigned char* data;
unsigned int fileSize = 0;
unsigned char *fileData = LoadFileData(filename, &fileSize);
int width;
int height;
int channels;


if (fileData != NULL)
{
stbi_set_flip_vertically_on_load(invert);
data= stbi_load_from_memory(fileData, fileSize, &width,  &height,  &channels,0);
if (data)
{

	SDL_free(fileData);
	bool CONVERT_TO_PO=true;

    if (CONVERT_TO_PO)
    {
        int new_width = 1;
        int new_height = 1;
        while( new_width < width )
        {
            new_width *= 2;
        }
        while( new_height < height )
        {
            new_height *= 2;
        }
        if( (new_width != width) || (new_height != height) )
        {
			unsigned char *resampled = (unsigned char*)malloc( channels*new_width*new_height );
			up_scale_image(data, width, height, channels, resampled, new_width, new_height );
			SDL_free( data );
			data = resampled;
			width = new_width;
			height = new_height;
		}
	}


}else{
	   SDL_Log("IMAGE: [%s] Failed to load image. \n", filename);
	   return -1;
}
}else{
	   SDL_Log("IMAGE: [%s] Failed to load data . \n", filename);
       return -1;
}

	switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			break;
		}



    GLuint ID;
	glGenTextures(1, &ID);              // Generate texture id
	glBindTexture(GL_TEXTURE_2D, ID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters

	if (filter==0)
	{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}	else
	if (filter==1)
	{

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else

	if (filter==2)
	{

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	} else
   	{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}




	glTexImage2D(
		GL_TEXTURE_2D, 0,
		original_texture_format, width, height, 0,
		original_texture_format, GL_UNSIGNED_BYTE, data );



	stbi_image_free(data);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
	if (ID > 0)
	{
	SDL_Log( "TEXTURE: [ID %i] Texture(%s) loaded successfully (%ix%i ) channels(%d)", ID, filename, width, height,channels);
	return ID;
	}




	else
	{
	SDL_Log( "TEXTURE: Failed to load texture(%s)",filename);
	return -1;
	}

	return ID;
}




