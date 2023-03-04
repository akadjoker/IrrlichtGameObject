#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <set>
#include <stack>
#include <map>
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;

typedef unsigned char		u8;
typedef signed char		    s8;
typedef char			    c8;
typedef unsigned short		u16;
typedef signed short		s16;
typedef unsigned int		u32;
typedef signed int		    s32;
typedef unsigned long long	u64;
typedef long long			s64;
typedef float				f32;
typedef double				f64;

#include <sys/stat.h>               // Required for: stat() [Used in GetFileModTime()]
#include <dirent.h>             // Required for: DIR, opendir(), closedir() [Used in GetDirectoryFiles()]
#include <unistd.h>             // Required for: getch(), chdir() (POSIX), access()
#define GETCWD getcwd
#define CHDIR chdir


#if (defined(__linux__) || defined(PLATFORM_WEB)) && _POSIX_C_SOURCE < 199309L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif

#include <algorithm>    // std::sort



#include <stdlib.h>                 // Required for: srand(), rand(), atexit()
#include <stdio.h>                  // Required for: sprintf() [Used in OpenURL()]
#include <string.h>                 // Required for: strrchr(), strcmp(), strlen()
#include <time.h>                   // Required for: time() [Used in InitTimer()]
#include <math.h>                   // Required for: tan() sinf(), cosf(), sqrtf(), tan(), fabs()

#if defined(__linux__)
    #define MAX_FILEPATH_LENGTH     4096        // Maximum length for filepaths (Linux PATH_MAX default value)
#else
    #define MAX_FILEPATH_LENGTH      512        // Maximum length supported for filepaths
#endif

#define MAX_TEXT_BUFFER_LENGTH              1024        // Size of internal static buffers used on some functions:
                                                        // TextFormat(), TextSubtext(), TextToUpper(), TextToLower(), TextToPascal(), TextSplit()
#define MAX_TEXT_UNICODE_CHARS               512        // Maximum number of unicode codepoints: GetCodepoints()
#define MAX_TEXTSPLIT_COUNT                  128        // Maximum number of substrings to split: TextSplit()
#define RGBA(r,g,b,a)   ((DWORD(a)<<24) + (DWORD(b)<<16) + (DWORD(g)<<8) + DWORD(r))
#define GETR(col)       ((col) & 0xFF)
#define GETG(col)       (((col)>>8) & 0xFF)
#define GETB(col)       (((col)>>16) & 0xFF)
#define GETA(col)       ((col)>>24)
#define SETR(col,r)     (((col) & 0xFFFFFF00) + DWORD(r))
#define SETG(col,g)     (((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)     (((col) & 0xFF00FFFF) + (DWORD(b)<<16))
#define SETA(col,a)     (((col) & 0x00FFFFFF) + (DWORD(a)<<24))


#define MAX_TOUCH_POINTS              10
#define MAX_KEY_PRESSED_QUEUE         16
#define MAX_CHAR_PRESSED_QUEUE        16




typedef struct ByteArray {
    Uint32 size;
    Uint32 pos;
    bool write;
    SDL_RWops *rw;
} ByteArray;




void ShowSimpleMessageBox(Uint32 flags, const char *title, const char *message);



 unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead);     // Load file data as byte array (read)
 void UnloadFileData(unsigned char *data);                   // Unload file data allocated by LoadFileData()
 bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite); // Save data to file from byte array (write), returns true on success
 char *LoadFileText(const char *fileName);                   // Load text data from file (read), returns a '\0' terminated string
 void UnloadFileText(unsigned char *text);                   // Unload file text data allocated by LoadFileText()
 bool SaveFileText(const char *fileName, char *text);        // Save text data to file (write), string must be '\0' terminated, returns true on success
 bool FileExists(const char *fileName);                      // Check if file exists
 bool DirectoryExists(const char *dirPath);                  // Check if a directory path exists
 bool IsFileExtension(const char *fileName, const char *ext);// Check file extension (including point: .png, .wav)
 const char *GetFileExtension(const char *fileName);         // Get pointer to extension for a filename string (including point: ".png")
 const char *GetFileName(const char *filePath);              // Get pointer to filename for a path string
 const char *GetFileNameWithoutExt(const char *filePath);    // Get filename string without extension (uses static string)
 const char *GetDirectoryPath(const char *filePath);         // Get full path for a given fileName with path (uses static string)
 const char *GetPrevDirectoryPath(const char *dirPath);      // Get previous directory path for a given path (uses static string)
 const char *GetWorkingDirectory(void);                      // Get current working directory (uses static string)
 char **GetDirectoryFiles(const char *dirPath, int *count);  // Get filenames in a directory path (memory should be freed)
 void ClearDirectoryFiles(void);                             // Clear directory files paths buffers (free memory)
 bool ChangeDirectory(const char *dir);                      // Change working directory, return true on success
 long GetFileModTime(const char *fileName);                  // Get file modification time (last write time)


// Text strings management functions (no utf8 strings, only byte chars)
// NOTE: Some strings allocate memory internally for returned strings, just be careful!
 int TextCopy(char *dst, const char *src);                                             // Copy one string to another, returns bytes copied
 bool TextIsEqual(const char *text1, const char *text2);                               // Check if two text string are equal
 unsigned int TextLength(const char *text);                                            // Get text length, checks for '\0' ending
 const char *TextFormat(const char *text, ...);                                        // Text formatting with variables (sprintf style)
 const char *TextSubtext(const char *text, int position, int length);                  // Get a piece of a text string
 char *TextReplace(char *text, const char *replace, const char *by);                   // Replace text string (memory must be freed!)
 char *TextInsert(const char *text, const char *insert, int position);                 // Insert text in a position (memory must be freed!)
 const char *TextJoin(const char **textList, int count, const char *delimiter);        // Join text strings with delimiter
 const char **TextSplit(const char *text, char delimiter, int *count);                 // Split text into multiple strings
 void TextAppend(char *text, const char *append, int *position);                       // Append text at specific position and move cursor!
 int TextFindIndex(const char *text, const char *find);                                // Find first text occurrence within a string
 const char *TextToUpper(const char *text);                      // Get upper case version of provided string
 const char *TextToLower(const char *text);                      // Get lower case version of provided string
 const char *TextToPascal(const char *text);                     // Get Pascal case notation version of provided string
 int TextToInteger(const char *text);                            // Get integer value from text (negative values not supported)
char *TextToUtf8(int *codepoints, int length);                  // Encode text codepoint into utf8 text (memory must be freed!)
// UTF8 text strings management functions
 int *GetCodepoints(const char *text, int *count);               // Get all codepoints in a string, codepoints count returned by parameters
 int GetCodepointsCount(const char *text);                       // Get total number of characters (codepoints) in a UTF8 encoded string
 int GetNextCodepoint(const char *text, int *bytesProcessed);    // Returns next codepoint in a UTF8 encoded string; 0x3f('?') is returned on failure
 const char *CodepointToUtf8(int codepoint, int *byteLength);    // Encode codepoint into utf8 text (char array length returned as parameter)




int GetRandomValue(int min, int max);
void Random_Seed(int seed) ;
int Random_Int(int min, int max);
float Random_Float(float min, float max);


unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead);
void UnloadFileData(unsigned char *data);
bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite);
char *LoadFileText(const char *fileName);
void UnloadFileText(unsigned char *text);
bool SaveFileText(const char *fileName, char *text);


 ByteArray* NewByteArrayFromFile(const char* filename,const char* mod);

 ByteArray* CreateByteArrayFromFile(const char* filename,bool append);
 ByteArray* OpenByteArrayFromFile(const char* filename);

 ByteArray* CreateByteArrayFromMem(void *mem, int size);
 ByteArray* CreateByteArrayFromConstMem(const void *mem,int size);

 void UnloadByteArray(ByteArray* f);

 Uint32 GetByteArrayPosition(ByteArray* file);
 bool ByteArrayIsOpen(ByteArray* f);
 Uint32 GetByteArraySize(ByteArray* f);

 int  EofByteArray(ByteArray* f);

 Uint32 SeekByteArray(ByteArray* f,Uint32 finalPos, bool relativeMovement);
 Uint32 WriteByteArray(ByteArray* f,void* buffer, Uint32 sizeToRead);
 Uint32 ReadByteArray(ByteArray* f,void* buffer, Uint32 sizeToRead);

char* ReadByteArrayBytes(ByteArray* f,int count);
int WriteByteArrayBytes(ByteArray* f,const char* c,int count);


 int WriteByteArrayByte(ByteArray* f,char c);
 int WriteByteArrayShort(ByteArray* f,short c);
 int WriteByteArrayInt(ByteArray* f,int c);
 int WriteByteArrayLong(ByteArray* f,long c);
 int WriteByteArrayFloat(ByteArray* f,float c);
 int WriteByteArrayString(ByteArray* f,const char* c);
 int WriteByteArrayLine(ByteArray* f,const char* c);

 char ReadByteArrayByte(ByteArray* f);
 short ReadByteArrayShort(ByteArray* f);
 int ReadByteArrayInt(ByteArray* f);
 float ReadByteArrayFloat(ByteArray* f);
 long ReadByteArrayLong(ByteArray* f);
 char* ReadByteArrayString(ByteArray* f);
 char* ReadByteArrayLine(ByteArray* f);


void GLCheckErrors();
void UnloadTexture(unsigned int id);
GLuint LoadVertexBuffer(void *buffer, int size, bool dynamic);
void UnloadVertexBuffer(GLuint vboId);
GLuint LoadVertexBufferElement(void *buffer, int size, bool dynamic);
void EnableVertexBuffer(unsigned int id);
void DisableVertexBuffer(void);
void EnableVertexBufferElement(GLuint id);
void DisableVertexBufferElement(void);
void UpdateVertexBuffer(int bufferId, void *data, int dataSize, int offset);
bool EnableVertexArray(GLuint vaoId);
void DisableVertexArray(void);
void EnableVertexAttribute(GLuint index);
void DisableVertexAttribute(GLuint index);
void DrawVertexArray(GLenum mode,int offset, int count);
void DrawVertexArrayElements(GLenum mode,int offset, int count, void *buffer);
GLuint LoadVertexArray(void);
void SetVertexAttribute(GLuint index, int compSize, int type, bool normalized, int stride, void *pointer);
void UnloadVertexArray(GLuint vaoId);


// Textures management
GLuint  LoadTexture(const char* filename,int filter=0,bool invert=true);
unsigned int CreateDataTexture(void* data , int width, int height, int channels);

unsigned int CreateTexture(void *data, int width, int height, int format);
unsigned int CreateTextureMipmaps(void *data, int width, int height, int format,int mipmapCount);
unsigned int LoadTextureDepth(int width, int height, bool useRenderBuffer);
void UnloadTexture(unsigned int id);

// Textures state
void ActiveTextureSlot(int slot);               // Select and active a texture slot
void EnableTexture(unsigned int id);            // Enable texture
void DisableTexture(void);                      // Disable texture
void EnableTextureCubemap(unsigned int id);     // Enable texture cubemap
void DisableTextureCubemap(void);               // Disable texture cubemap
void TextureParameters(unsigned int id, int param, int value); // Set texture parameters (filter, wrap)
void UpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void *data);


// Textures state
 void ActiveTextureSlot(int slot);               // Select and active a texture slot
 void EnableTexture(unsigned int id);            // Enable texture
 void DisableTexture(void);                      // Disable texture
 void EnableTextureCubemap(unsigned int id);     // Enable texture cubemap
 void DisableTextureCubemap(void);               // Disable texture cubemap
 void TextureParameters(unsigned int id, int param, int value); // Set texture parameters (filter, wrap)
void SetTextureFilter(GLuint texture, int filter);
void SetTextureWrap(GLuint texture, int wrap);

 // Enable depth test

void EnableDepthTest(void) ;
void DisableDepthTest(void) ;
void EnableDepthMask(void) ;
void DisableDepthMask(void) ;
void EnableBackfaceCulling(void) ;
void DisableBackfaceCulling(void) ;
void EnableScissorTest(void) ;
void DisableScissorTest(void) ;
void Scissor(int x, int y, int width, int height) ;
void Viewport(int x, int y, int width, int height);


GLuint LoadShaderFromMemory(const char *vsCode, const char *fsCode);
GLuint LoadShader(const char *vsFileName, const char *fsFileName);

GLuint LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);
void UnloadShaderProgram(GLuint id);
GLuint CompileShader(const char *shaderCode, int type);
GLuint LoadShaderCode(const char *vsCode, const char *fsCode);

int GetLocationUniform(GLuint shaderId, const char *uniformName);
int GetLocationAttrib(GLuint shaderId, const char *attribName);
void SetUniform(int locIndex, const void *value, int uniformType, int count);
void SetVertexAttributeDefault(int locIndex, const void *value, int attribType, int count);
void SetUniformMatrix(int locIndex, const GLfloat* mat);
void SetUniformSampler(int locIndex, GLuint textureId);
void SetShader(GLuint id);



#endif // UTILS_H
