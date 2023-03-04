// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_CORE_UTIL_H_INCLUDED__
#define __IRR_CORE_UTIL_H_INCLUDED__

#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <set>
#include <stack>
#include <map>
#include <sys/stat.h>               // Required for: stat() [Used in GetFileModTime()]
#include <dirent.h>             // Required for: DIR, opendir(), closedir() [Used in GetDirectoryFiles()]
#include <unistd.h>             // Required for: getch(), chdir() (POSIX), access()
#define GETCWD getcwd
#define CHDIR chdir
#include <stdlib.h>                 // Required for: srand(), rand(), atexit()
#include <stdio.h>                  // Required for: sprintf() [Used in OpenURL()]
#include <string.h>                 // Required for: strrchr(), strcmp(), strlen()
#include <time.h>                   // Required for: time() [Used in InitTimer()]
#include <math.h>                   // Required for: tan() sinf(), cosf(), sqrtf(), tan(), fabs()
#include <stdarg.h>

#if (defined(__linux__) || defined(PLATFORM_WEB)) && _POSIX_C_SOURCE < 199309L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif

#include <algorithm>    // std::sort


#define MAX_TEXT_BUFFER_LENGTH              1024        // Size of internal static buffers used on some functions:
                                                        // TextFormat(), TextSubtext(), TextToUpper(), TextToLower(), TextToPascal(), TextSplit()
#define MAX_TEXT_UNICODE_CHARS               512        // Maximum number of unicode codepoints: GetCodepoints()
#define MAX_TEXTSPLIT_COUNT                  128        // Maximum number of substrings to split: TextSplit()
#if defined(__linux__)
    #define MAX_FILEPATH_LENGTH     4096        // Maximum length for filepaths (Linux PATH_MAX default value)
#else
    #define MAX_FILEPATH_LENGTH      512        // Maximum length supported for filepaths
#endif


#include "irrString.h"
#include "SColor.h"
#include "path.h"
#include "os.h"

namespace irr
{
namespace core
{

/*! \file coreutil.h
	\brief File containing useful basic utility functions
*/


// ----------- some basic quite often used string functions -----------------

//! search if a filename has a proper extension
inline s32 isFileExtension (const io::path& filename, const io::path& ext0,		const io::path& ext1, const io::path& ext2)
{
	s32 extPos = filename.findLast ( '.' );
	if ( extPos < 0 )
		return 0;

	extPos += 1;
	if ( filename.equals_substring_ignore_case ( ext0, extPos ) )
		return 1;
	if ( filename.equals_substring_ignore_case ( ext1, extPos ) )
		return 2;
	if ( filename.equals_substring_ignore_case ( ext2, extPos ) )
		return 3;
	return 0;
}

inline bool fileIsExtension (const io::path& filename, const io::path& ext0)
{
	s32 extPos = filename.findLast ( '.' );
	if ( extPos < 0 )
		return false;

	extPos += 1;
	if ( filename.equals_substring_ignore_case ( ext0, extPos ) )
		return true;

	return false;
}

//! search if a filename has a proper extension
inline bool hasFileExtension(const io::path& filename, const io::path& ext0,				const io::path& ext1 = "", const io::path& ext2 = "")
{
	return isFileExtension ( filename, ext0, ext1, ext2 ) > 0;
}


//! cut the filename extension from a source file path and store it in a dest file path
inline io::path& cutFilenameExtension ( io::path &dest, const io::path &source )
{
	s32 endPos = source.findLast ( '.' );
	dest = source.subString ( 0, endPos < 0 ? source.size () : endPos );
	return dest;
}

//! get the filename extension from a file path
inline io::path& getFileNameExtension ( io::path &dest, const io::path &source )
{
	s32 endPos = source.findLast ( '.' );
	if ( endPos < 0 )
		dest = "";
	else
		dest = source.subString ( endPos, source.size () );
	return dest;
}

//! delete path from filename
inline io::path& deletePathFromFilename(io::path& filename)
{
	// delete path from filename
	const fschar_t* s = filename.c_str();
	const fschar_t* p = s + filename.size();

	// search for path separator or beginning
	while ( *p != '/' && *p != '\\' && p != s )
		p--;

	if ( p != s )
	{
		++p;
		filename = p;
	}
	return filename;
}

//! trim paths
inline io::path& deletePathFromPath(io::path& filename, s32 pathCount)
{
	// delete path from filename
	s32 i = filename.size();

	// search for path separator or beginning
	while ( i>=0 )
	{
		if ( filename[i] == '/' || filename[i] == '\\' )
		{
			if ( --pathCount <= 0 )
				break;
		}
		--i;
	}

	if ( i>0 )
	{
		filename [ i + 1 ] = 0;
		filename.validate();
	}
	else
		filename="";
	return filename;
}

//! looks if file is in the same directory of path. returns offset of directory.
//! 0 means in same directory. 1 means file is direct child of path
inline s32 isInSameDirectory ( const io::path& path, const io::path& file )
{
	if ( path.size() && !path.equalsn ( file, path.size() ) )
		return -1;

	s32 subA = 0;
	s32 subB = 0;
	s32 pos = 0;
	while ( (pos = path.findNext ( '/', pos )) >= 0 )
	{
		subA += 1;
		pos += 1;
	}

	pos = 0;
	while ( (pos = file.findNext ( '/', pos )) >= 0 )
	{
		subB += 1;
		pos += 1;
	}

	return subB - subA;
}

//! splits a path into components
static inline void splitFilename(const io::path &name, io::path* path=0,
		io::path* filename=0, io::path* extension=0, bool make_lower=false)
{
	s32 i = name.size();
	s32 extpos = i;

	// search for path separator or beginning
	while ( i >= 0 )
	{
		if ( name[i] == '.' )
		{
			extpos = i;
			if ( extension )
				*extension = name.subString ( extpos + 1, name.size() - (extpos + 1), make_lower );
		}
		else
		if ( name[i] == '/' || name[i] == '\\' )
		{
			if ( filename )
				*filename = name.subString ( i + 1, extpos - (i + 1), make_lower );
			if ( path )
			{
				*path = name.subString ( 0, i + 1, make_lower );
				path->replace ( '\\', '/' );
			}
			return;
		}
		i -= 1;
	}
	if ( filename )
		*filename = name.subString ( 0, extpos, make_lower );
}

//! create a filename from components
static inline io::path mergeFilename(const io::path& path, const io::path& filename, const io::path& extension = "")
{
	io::path result(path);

	if ( !result.empty() )
	{
		fschar_t last = result.lastChar();
		if ( last != _IRR_TEXT('/') && last != _IRR_TEXT('\\') )
			result += _IRR_TEXT('/');
	}
	if ( !filename.empty() )
		result += filename;
	if ( !extension.empty() )
	{
		if ( !result.empty() && extension[0] != _IRR_TEXT('.') )
			result += _IRR_TEXT('.');
		result += extension;
	}

	return result;
}


//! some standard function ( to remove dependencies )
inline s32 isdigit(s32 c) { return c >= '0' && c <= '9'; }
inline s32 isspace(s32 c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
inline s32 isupper(s32 c) { return c >= 'A' && c <= 'Z'; }

inline unsigned int TextLength(const char *text)
{
    unsigned int length = 0; //strlen(text)

    if (text != NULL)
    {
        while (*text++) length++;
    }

    return length;
}

// Get integer value from text
// NOTE: This function replaces atoi() [stdlib.h]
inline   int TextToInteger(const char *text)
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

// Copy one string to another, returns bytes copied
inline int TextCopy(char *dst, const char *src)
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
inline bool TextIsEqual(const char *text1, const char *text2)
{
    bool result = false;

    if (strcmp(text1, text2) == 0) result = true;

    return result;
}

// Get a piece of a text string
inline const char *TextSubtext(const char *text, int position, int length)
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
inline char *TextReplace(char *text, const char *replace, const char *by)
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
    temp = result =(char*) malloc(TextLength(text) + (byLen - replaceLen)*count + 1);

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
inline char *TextInsert(const char *text, const char *insert, int position)
{
    int textLen = TextLength(text);
    int insertLen =  TextLength(insert);

    char *result = (char *)malloc(textLen + insertLen + 1);

    for (int i = 0; i < position; i++) result[i] = text[i];
    for (int i = position; i < insertLen + position; i++) result[i] = insert[i];
    for (int i = (insertLen + position); i < (textLen + insertLen); i++) result[i] = text[i];

    result[textLen + insertLen] = '\0';     // Make sure text string is valid!

    return result;
}

// Join text strings with delimiter
// REQUIRES: memset(), memcpy()
inline const char *TextJoin(const char **textList, int count, const char *delimiter)
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
inline const char **TextSplit(const char *text, char delimiter, int *count)
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
inline void TextAppend(char *text, const char *append, int *position)
{
    strcpy(text + *position, append);
    *position += TextLength(append);
}

// Find first text occurrence within a string
// REQUIRES: strstr()
inline int TextFindIndex(const char *text, const char *find)
{
    int position = -1;

    char *ptr = (char*)strstr(text, find);

    if (ptr != NULL) position = (int)(ptr - text);

    return position;
}

// Get upper case version of provided string
inline const char *TextToUpper(const char *text)
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
inline const char *TextToLower(const char *text)
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
inline const char *TextToPascal(const char *text)
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

// Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found
// When a invalid UTF8 byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
// Total number of bytes processed are returned as a parameter
// NOTE: the standard says U+FFFD should be returned in case of errors
// but that character is not supported by the default font in raylib
// TODO: optimize this code for speed!!
inline int GetNextCodepoint(const char *text, int *bytesProcessed)
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

// Encode codepoint into utf8 text (char array length returned as parameter)
 inline const char *CodepointToUtf8(int codepoint, int *byteLength)
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
inline int *GetCodepoints(const char *text, int *count)
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
inline int GetCodepointsCount(const char *text)
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




// Check if the file exists
inline  bool FileExists(const char *fileName)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1) result = true;
#else
    if (access(fileName, F_OK) != -1) result = true;
#endif

    return result;
}



// Get pointer to extension for a filename string
inline  const char *GetFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return (dot + 1);
}

// Check file extension
// NOTE: Extensions checking is not case-sensitive
inline  bool IsFileExtension(const char *fileName, const char *ext)
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

inline  bool IsFileWithExtension(const char *fileName, const char *ext)
{
    bool result = false;
    const char *fileExt = TextToLower(GetFileExtension(fileName));

    if (fileExt != NULL)
    {

            if (TextIsEqual(fileExt, TextToLower(ext)))
            {
                result = true;

            }

    }

    return result;
}

// String pointer reverse break: returns right-most occurrence of charset in s
static const char *strprbrk(const char *s, const char *charset)
{
    const char *latestMatch = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) { }
    return latestMatch;
}

// Get pointer to filename for a path string
inline  const char *GetFileName(const char *filePath)
{
    const char *fileName = NULL;
    if (filePath != NULL) fileName = strprbrk(filePath, "\\/");

    if (!fileName) return filePath;

    return fileName + 1;
}

// Get filename string without extension (uses static string)
inline  const char *GetFileNameWithoutExt(const char *filePath)
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
inline  const char *GetDirectoryPath(const char *filePath)
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
inline  const char *GetPrevDirectoryPath(const char *dirPath)
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
inline  const char *GetWorkingDirectory(void)
{
    static char currentDir[MAX_FILEPATH_LENGTH];
    memset(currentDir, 0, MAX_FILEPATH_LENGTH);

    char *ptr = GETCWD(currentDir, MAX_FILEPATH_LENGTH - 1);

    return ptr;
}

static char **dirFilesPath = NULL;          // Store directory files paths as strings
static int dirFilesCount = 0;               // Count directory files strings
    #define MAX_DIRECTORY_FILES     512

// Clear directory files paths buffers
inline  void ClearDirectoryFiles(void)
{
    if (dirFilesCount > 0)
    {
        for (int i = 0; i < MAX_DIRECTORY_FILES; i++) free(dirFilesPath[i]);

    free(dirFilesPath);
    }

    dirFilesCount = 0;
}
// Get filenames in a directory path (max 512 files)
// NOTE: Files count is returned by parameters pointer
inline  char **GetDirectoryFiles(const char *dirPath, int *fileCount)
{
    #define MAX_DIRECTORY_FILES     512

    ClearDirectoryFiles();

    // Memory allocation for MAX_DIRECTORY_FILES
    dirFilesPath = (char **)malloc(sizeof(char *)*MAX_DIRECTORY_FILES);

    for (int i = 0; i < MAX_DIRECTORY_FILES; i++) dirFilesPath[i] = (char *)malloc(sizeof(char)*MAX_FILEPATH_LENGTH);

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
    else os::Printer::log("FILEIO: Failed to open requested directory.", ELL_WARNING);  // Maybe it's a file...

    dirFilesCount = counter;
    *fileCount = dirFilesCount;

    return dirFilesPath;
}



// Change working directory, returns true on success
inline bool ChangeDirectory(const char *dir)
{
    bool result = CHDIR(dir);

    if (result != 0) os::Printer::print(irr::ELL_ERROR, "SYSTEM: Failed to change to directory: %s", dir);

    return (result == 0);
}
// Get file modification time (last write time)
inline long GetFileModTime(const char *fileName)
{
    struct stat result = { 0 };

    if (stat(fileName, &result) == 0)
    {
        time_t mod = result.st_mtime;

        return (long)mod;
    }

    return 0;
}


// Check if a directory path exists
inline bool DirectoryExists(const char *dirPath)
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


// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
inline const char *TextFormat(const char *text, ...)
{

    #define MAX_TEXTFORMAT_BUFFERS 4        // Maximum number of static buffers for text formatting
    #define MAX_TEXT_BUFFER_LENGTH              1024        // Size of internal static buffers used on some functions:

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




// Load data from file into a buffer
inline unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead)
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





// Save data to file from buffer
inline bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite)
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
inline char *LoadFileText(const char *fileName)
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



// Save text data to file (write), string must be '\0' terminated
inline bool SaveFileText(const char *fileName, char *text)
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


/*
// Load color data from image as a Color array (RGBA - 32bit)
// NOTE: Memory allocated should be freed using UnloadImageColors();
Color *LoadImageColors(Image image)
{
    if ((image.width == 0) || (image.height == 0)) return NULL;

    Color *pixels = (Color *)RL_MALLOC(image.width*image.height*sizeof(Color));

    if (image.format >= PIXELFORMAT_COMPRESSED_DXT1_RGB) TRACELOG(LOG_WARNING, "IMAGE: Pixel data retrieval not supported for compressed image formats");
    else
    {
        if ((image.format == PIXELFORMAT_UNCOMPRESSED_R32) ||
            (image.format == PIXELFORMAT_UNCOMPRESSED_R32G32B32) ||
            (image.format == PIXELFORMAT_UNCOMPRESSED_R32G32B32A32)) TRACELOG(LOG_WARNING, "IMAGE: Pixel format converted from 32bit to 8bit per channel");

        for (int i = 0, k = 0; i < image.width*image.height; i++)
        {
            switch (image.format)
            {
                case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
                {
                    pixels[i].r = ((unsigned char *)image.data)[i];
                    pixels[i].g = ((unsigned char *)image.data)[i];
                    pixels[i].b = ((unsigned char *)image.data)[i];
                    pixels[i].a = 255;

                } break;
                case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
                {
                    pixels[i].r = ((unsigned char *)image.data)[k];
                    pixels[i].g = ((unsigned char *)image.data)[k];
                    pixels[i].b = ((unsigned char *)image.data)[k];
                    pixels[i].a = ((unsigned char *)image.data)[k + 1];

                    k += 2;
                } break;
                case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6)*(255/31));
                    pixels[i].b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1)*(255/31));
                    pixels[i].a = (unsigned char)((pixel & 0b0000000000000001)*255);

                } break;
                case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5)*(255/63));
                    pixels[i].b = (unsigned char)((float)(pixel & 0b0000000000011111)*(255/31));
                    pixels[i].a = 255;

                } break;
                case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12)*(255/15));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)*(255/15));
                    pixels[i].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)*(255/15));
                    pixels[i].a = (unsigned char)((float)(pixel & 0b0000000000001111)*(255/15));

                } break;
                case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
                {
                    pixels[i].r = ((unsigned char *)image.data)[k];
                    pixels[i].g = ((unsigned char *)image.data)[k + 1];
                    pixels[i].b = ((unsigned char *)image.data)[k + 2];
                    pixels[i].a = ((unsigned char *)image.data)[k + 3];

                    k += 4;
                } break;
                case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
                {
                    pixels[i].r = (unsigned char)((unsigned char *)image.data)[k];
                    pixels[i].g = (unsigned char)((unsigned char *)image.data)[k + 1];
                    pixels[i].b = (unsigned char)((unsigned char *)image.data)[k + 2];
                    pixels[i].a = 255;

                    k += 3;
                } break;
                case PIXELFORMAT_UNCOMPRESSED_R32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = 0;
                    pixels[i].b = 0;
                    pixels[i].a = 255;

                } break;
                case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float *)image.data)[k + 1]*255.0f);
                    pixels[i].b = (unsigned char)(((float *)image.data)[k + 2]*255.0f);
                    pixels[i].a = 255;

                    k += 3;
                } break;
                case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].b = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].a = (unsigned char)(((float *)image.data)[k]*255.0f);

                    k += 4;
                } break;
                default: break;
            }
        }
    }

    return pixels;
}

inline core::SColorf * LoadPixelDataNormalized(u8* data,int width, int height,int format)
{
    core::SColorf *pixels = (core::SColorf *)malloc(width*height*sizeof(core::SColorf));


        for (int i = 0, k = 0; i < width*height; i++)
        {
            switch (format)
            {
                case 1:
                {
                    pixels[i].X = (float)((u8 *)data)[i]/255.0f;
                    pixels[i].Y = (float)((u8 *)data)[i]/255.0f;
                    pixels[i].Z = (float)((u8 *)data)[i]/255.0f;
                    pixels[i].W = 1.0f;

                } break;
                case 2:
                {
                    pixels[i].X = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].Y = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].Z = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].W = (float)((unsigned char *)data)[k + 1]/255.0f;

                    k += 2;
                } break;
               case 3:
                {
                    pixels[i].X = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].Y = (float)((unsigned char *)data)[k + 1]/255.0f;
                    pixels[i].Z = (float)((unsigned char *)data)[k + 2]/255.0f;
                    pixels[i].W = 1.0f;

                    //SDL_Log("%f %f %f",pixels[i].X,pixels[i].Y,pixels[i].Z);

                    k += 3;
                } break;

                case 4:
                {
                    pixels[i].X = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].Y = (float)((unsigned char *)data)[k + 1]/255.0f;
                    pixels[i].Z = (float)((unsigned char *)data)[k + 2]/255.0f;
                    pixels[i].W = (float)((unsigned char *)data)[k + 3]/255.0f;

                    k += 4;
                } break;

                default: break;
            }
        }


    return pixels;
}
*/

/*
// Load color data from image as a Color array (RGBA - 32bit)
// NOTE: Memory allocated should be freed using UnloadImageColors();
inline irr::video::SColorf *LoadImageColors(video::IImage* image)
{

    int width= image->getDimension().Width;
    int height= image->getDimension().Height;
    video::SColorf *pixels = (video::SColorf *)malloc(width*height*sizeof(core::SColorf));

    void* data = image->getData();

    for (int i = 0, k = 0; i < width*height; i++)
        {
            switch (image->getColorFormat())
            {
                case 1://PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
                {
                    pixels[i].r = ((unsigned char *)data)[i];
                    pixels[i].g = ((unsigned char *)data)[i];
                    pixels[i].b = ((unsigned char *)data)[i];
                    pixels[i].a = 255;

                } break;
                case 2://PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
                {
                    pixels[i].r = ((unsigned char *)data)[k];
                    pixels[i].g = ((unsigned char *)data)[k];
                    pixels[i].b = ((unsigned char *)data)[k];
                    pixels[i].a = ((unsigned char *)data)[k + 1];

                    k += 2;
                } break;
                case ECF_A1R5G5B5://PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
                {
                    unsigned short pixel = ((unsigned short *)data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6)*(255/31));
                    pixels[i].b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1)*(255/31));
                    pixels[i].a = (unsigned char)((pixel & 0b0000000000000001)*255);

                } break;
                case ECF_R5G6B5://PIXELFORMAT_UNCOMPRESSED_R5G6B5:
                {
                    unsigned short pixel = ((unsigned short *)data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5)*(255/63));
                    pixels[i].b = (unsigned char)((float)(pixel & 0b0000000000011111)*(255/31));
                    pixels[i].a = 255;

                } break;

                case ECF_A8R8G8B8://PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
                {
                    pixels[i].r = ((unsigned char *)data)[k];
                    pixels[i].g = ((unsigned char *)data)[k + 1];
                    pixels[i].b = ((unsigned char *)data)[k + 2];
                    pixels[i].a = ((unsigned char *)data)[k + 3];

                    k += 4;
                } break;
                case ECF_R8G8B8://PIXELFORMAT_UNCOMPRESSED_R8G8B8:
                {
                    pixels[i].r = (unsigned char)((unsigned char *)data)[k];
                    pixels[i].g = (unsigned char)((unsigned char *)data)[k + 1];
                    pixels[i].b = (unsigned char)((unsigned char *)data)[k + 2];
                    pixels[i].a = 255;

                    k += 3;
                } break;
             /*   case PIXELFORMAT_UNCOMPRESSED_R32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = 0;
                    pixels[i].b = 0;
                    pixels[i].a = 255;

                } break;
                case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float *)image.data)[k + 1]*255.0f);
                    pixels[i].b = (unsigned char)(((float *)image.data)[k + 2]*255.0f);
                    pixels[i].a = 255;

                    k += 3;
                } break;
                case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
                {
                    pixels[i].r = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].b = (unsigned char)(((float *)image.data)[k]*255.0f);
                    pixels[i].a = (unsigned char)(((float *)image.data)[k]*255.0f);

                    k += 4;
                } break;
                default: break;
            }
        }


    return pixels;
}
*/
using namespace video;





// Get pixel data from image as Vector4 array (float normalized)
inline video::SColorf *LoadPixelsDataNormalized(void *data, int width,int height, int format)
{
    video::SColorf *pixels = (video::SColorf *)malloc(width*height*sizeof(video::SColorf));

        for (int i = 0, k = 0; i < width*height; i++)
        {
            switch (format)
            {
                case ECF_R8:
                {
                    pixels[i].r = (float)((unsigned char *)data)[i]/255.0f;
                    pixels[i].g = (float)((unsigned char *)data)[i]/255.0f;
                    pixels[i].b = (float)((unsigned char *)data)[i]/255.0f;
                    pixels[i].a = 1.0f;

                } break;
                case ECF_R8G8:
                {
                    pixels[i].r = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].g = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].b = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].a = (float)((unsigned char *)data)[k + 1]/255.0f;

                    k += 2;
                } break;
                case ECF_A1R5G5B5:
                {
                    unsigned short pixel = ((unsigned short *)data)[i];

                    pixels[i].r = (float)((pixel & 0b1111100000000000) >> 11)*(1.0f/31);
                    pixels[i].g = (float)((pixel & 0b0000011111000000) >> 6)*(1.0f/31);
                    pixels[i].b = (float)((pixel & 0b0000000000111110) >> 1)*(1.0f/31);
                    pixels[i].a = ((pixel & 0b0000000000000001) == 0)? 0.0f : 1.0f;

                } break;
                case ECF_R5G6B5:
                {
                    unsigned short pixel = ((unsigned short *)data)[i];

                    pixels[i].r = (float)((pixel & 0b1111100000000000) >> 11)*(1.0f/31);
                    pixels[i].g = (float)((pixel & 0b0000011111100000) >> 5)*(1.0f/63);
                    pixels[i].b = (float)(pixel & 0b0000000000011111)*(1.0f/31);
                    pixels[i].a = 1.0f;

                } break;
                case ECF_A8R8G8B8:
                {
                    pixels[i].r = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].g = (float)((unsigned char *)data)[k + 1]/255.0f;
                    pixels[i].b = (float)((unsigned char *)data)[k + 2]/255.0f;
                    pixels[i].a = (float)((unsigned char *)data)[k + 3]/255.0f;

                    k += 4;
                } break;
                case ECF_R8G8B8:
                {
                    pixels[i].r = (float)((unsigned char *)data)[k]/255.0f;
                    pixels[i].g = (float)((unsigned char *)data)[k + 1]/255.0f;
                    pixels[i].b = (float)((unsigned char *)data)[k + 2]/255.0f;
                    pixels[i].a = 1.0f;

                    k += 3;
                } break;
                case ECF_R32F:
                {
                    pixels[i].r = ((float *)data)[k];
                    pixels[i].g = 0.0f;
                    pixels[i].b = 0.0f;
                    pixels[i].a = 1.0f;

                } break;
               case ECF_G32R32F:
                {
                    pixels[i].r = ((float *)data)[k];
                    pixels[i].g = ((float *)data)[k + 1];
                    pixels[i].b = ((float *)data)[k + 2];
                    pixels[i].a = 1.0f;

                    k += 3;
                } break;
                case ECF_A32B32G32R32F:
                {
                    pixels[i].r = ((float *)data)[k];
                    pixels[i].g = ((float *)data)[k + 1];
                    pixels[i].b = ((float *)data)[k + 2];
                    pixels[i].a = ((float *)data)[k + 3];

                    k += 4;
                }
                default: break;
            }
        }


    return pixels;
}

// Convert image data to desired format
inline void ConvertColorFormat(void *data, int width,int height, int format,int newFormat)
{
    // Security check to avoid program crash
    if ((data == NULL) || (width == 0) || (height == 0)) return;

    if ((newFormat != 0) && (format != newFormat))
    {

            video::SColorf *pixels = LoadPixelsDataNormalized(data,width,height,format);     // Supports 8 to 32 bit per channel

            free(data);
            data = NULL;


            int k = 0;

            switch (format)
            {
                case ECF_R8:
                {
                    data = (unsigned char *)malloc(width*height*sizeof(unsigned char));

                    for (int i = 0; i < width*height; i++)
                    {
                        ((unsigned char *)data)[i] = (unsigned char)((pixels[i].r*0.299f + pixels[i].g*0.587f + pixels[i].b*0.114f)*255.0f);
                    }

                } break;
                case ECF_R8G8:
                {
                    data = (unsigned char *)malloc(width*height*2*sizeof(unsigned char));

                    for (int i = 0; i < width*height*2; i += 2, k++)
                    {
                        ((unsigned char *)data)[i] = (unsigned char)((pixels[k].r*0.299f + (float)pixels[k].g*0.587f + (float)pixels[k].b*0.114f)*255.0f);
                        ((unsigned char *)data)[i + 1] = (unsigned char)(pixels[k].a*255.0f);
                    }

                } break;
                case ECF_R5G6B5:
                {
                   data = (unsigned short *)malloc(width*height*sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;

                    for (int i = 0; i < width*height; i++)
                    {
                        r = (unsigned char)(round(pixels[i].r*31.0f));
                        g = (unsigned char)(round(pixels[i].g*63.0f));
                        b = (unsigned char)(round(pixels[i].b*31.0f));

                        ((unsigned short *)data)[i] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                    }

                } break;
                case ECF_R8G8B8:
                {
                    data = (unsigned char *)malloc(width*height*3*sizeof(unsigned char));

                    for (int i = 0, k = 0; i < width*height*3; i += 3, k++)
                    {
                        ((unsigned char *)data)[i] = (unsigned char)(pixels[k].r*255.0f);
                        ((unsigned char *)data)[i + 1] = (unsigned char)(pixels[k].g*255.0f);
                        ((unsigned char *)data)[i + 2] = (unsigned char)(pixels[k].b*255.0f);
                    }
                } break;
                case ECF_A1R5G5B5:
                {
                    data = (unsigned short *)malloc(width*height*sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;

                    for (int i = 0; i < width*height; i++)
                    {
                        r = (unsigned char)(round(pixels[i].r*31.0f));
                        g = (unsigned char)(round(pixels[i].g*31.0f));
                        b = (unsigned char)(round(pixels[i].b*31.0f));
                        a = (pixels[i].a > ((float)50/255.0f))? 1 : 0;

                        ((unsigned short *)data)[i] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                    }

                } break;

                case ECF_A8R8G8B8:
                {
                    data = (unsigned char *)malloc(width*height*4*sizeof(unsigned char));

                    for (int i = 0, k = 0; i < width*height*4; i += 4, k++)
                    {
                        ((unsigned char *)data)[i] = (unsigned char)(pixels[k].r*255.0f);
                        ((unsigned char *)data)[i + 1] = (unsigned char)(pixels[k].g*255.0f);
                        ((unsigned char *)data)[i + 2] = (unsigned char)(pixels[k].b*255.0f);
                        ((unsigned char *)data)[i + 3] = (unsigned char)(pixels[k].a*255.0f);
                    }
                } break;
                case ECF_D32:
                {
                    // WARNING: Image is converted to GRAYSCALE eqeuivalent 32bit

                    data = (float *)malloc(width*height*sizeof(float));

                    for (int i = 0; i < width*height; i++)
                    {
                        ((float *)data)[i] = (float)(pixels[i].r*0.299f + pixels[i].g*0.587f + pixels[i].b*0.114f);
                    }
                } break;
                case ECF_G32R32F:
                {
                    data = (float *)malloc(width*height*3*sizeof(float));

                    for (int i = 0, k = 0; i < width*height*3; i += 3, k++)
                    {
                        ((float *)data)[i] = pixels[k].r;
                        ((float *)data)[i + 1] = pixels[k].g;
                        ((float *)data)[i + 2] = pixels[k].b;
                    }
                } break;
                case ECF_A32B32G32R32F:
                {
                    data = (float *)malloc(width*height*4*sizeof(float));

                    for (int i = 0, k = 0; i < width*height*4; i += 4, k++)
                    {
                        ((float *)data)[i] = pixels[k].r;
                        ((float *)data)[i + 1] = pixels[k].g;
                        ((float *)data)[i + 2] = pixels[k].b;
                        ((float *)data)[i + 3] = pixels[k].a;
                    }
                } break;
                default: break;
            }

            free(pixels);
            pixels = NULL;


    }
}


} // end namespace core
} // end namespace irr

#endif
