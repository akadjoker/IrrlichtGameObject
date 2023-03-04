// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 9
#define IRRLICHT_VERSION_REVISION 0
// This flag will be defined only in SVN, the official release code will have
// it undefined
#define IRRLICHT_VERSION_SVN alpha
#define IRRLICHT_SDK_VERSION "1.0.1"

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

//! The defines for different operating system are:
//! _IRR_WINDOWS_ for all irrlicht supported Windows versions
//! _IRR_WINDOWS_API_ for Windows or XBox
//! _IRR_LINUX_PLATFORM_ for Linux (it is defined here if no other os is defined)
//! _IRR_SOLARIS_PLATFORM_ for Solaris
//! _IRR_OSX_PLATFORM_ for Apple systems running OSX
//! _IRR_IOS_PLATFORM_ for Apple devices running iOS
//! _IRR_ANDROID_PLATFORM_ for devices running Android
//! _IRR_POSIX_API_ for Posix compatible systems
//! Note: PLATFORM defines the OS specific layer, API can group several platforms

//! DEVICE is the windowing system used, several PLATFORMs support more than one DEVICE
//! Irrlicht can be compiled with more than one device
//! _IRR_COMPILE_WITH_WINDOWS_DEVICE_ for Windows API based device
//! _IRR_COMPILE_WITH_OSX_DEVICE_ for Cocoa native windowing on OSX
//! _IRR_COMPILE_WITH_X11_DEVICE_ for Linux X11 based device
//! _IRR_COMPILE_WITH_SDL_DEVICE_ for platform independent SDL framework
//! _IRR_COMPILE_WITH_CONSOLE_DEVICE_ for no windowing system, used as a fallback
//! _IRR_COMPILE_WITH_FB_DEVICE_ for framebuffer systems

//! Passing defines to the compiler which have NO in front of the _IRR definename is an alternative
//! way which can be used to disable defines (instead of outcommenting them in this header).
//! So defines can be controlled from Makefiles or Projectfiles which allows building
//! different library versions without having to change the sources.
//! Example: NO_IRR_COMPILE_WITH_X11_ would disable X11


//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_
#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1500)
#  error "Only Microsoft Visual Studio 9.0 and later are supported."
#endif





#if !defined(_IRR_WINDOWS_API_) && !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_IOS_PLATFORM_) && !defined(_IRR_ANDROID_PLATFORM_) && !defined(_IRR_EMSCRIPTEN_PLATFORM_)
#ifndef _IRR_SOLARIS_PLATFORM_
#define _IRR_LINUX_PLATFORM_
#endif
#define _IRR_POSIX_API_
#define _IRR_COMPILE_WITH_X11_DEVICE_
#endif



//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 4

//! Whether to support XML and XML-based formats (irrmesh, collada...)
#define _IRR_COMPILE_WITH_XML_
#ifdef NO_IRR_COMPILE_WITH_XML_
#undef _IRR_COMPILE_WITH_XML_
#endif

//! Add a leak-hunter to Irrlicht which helps finding unreleased reference counted objects.
//! NOTE: This is slow and should only be used for debugging
//#define _IRR_COMPILE_WITH_LEAK_HUNTER_
#ifdef NO_IRR_COMPILE_WITH_LEAK_HUNTER_
#undef _IRR_COMPILE_WITH_LEAK_HUNTER_
#endif





//! Define _IRR_WCHAR_FILESYSTEM to enable unicode filesystem support for the engine.
/** This enables the engine to read/write from unicode filesystem. If you
disable this feature, the engine behave as before (ansi). This is currently only supported
for Windows based systems. You also have to set #define UNICODE for this to compile.
*/
//#define _IRR_WCHAR_FILESYSTEM
#ifdef NO_IRR_WCHAR_FILESYSTEM
#undef _IRR_WCHAR_FILESYSTEM
#endif


//! Define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_ if you want to open ZIP and GZIP archives
/** ZIP reading has several more options below to configure. */
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#endif
#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
//! Define _IRR_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
/** This enables the engine to read from compressed .zip archives. If you
disable this feature, the engine can still read archives, but only uncompressed
ones. */
#define _IRR_COMPILE_WITH_ZLIB_
#ifdef NO_IRR_COMPILE_WITH_ZLIB_
#undef _IRR_COMPILE_WITH_ZLIB_
#endif
//! Define _IRR_USE_NON_SYSTEM_ZLIB_ to let irrlicht use the zlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the zlib
	installed on the system. This is only used when _IRR_COMPILE_WITH_ZLIB_ is
	defined.
	NOTE: You will also have to modify the Makefile or project files when changing this default.
 */
#define _IRR_USE_NON_SYSTEM_ZLIB_
#ifdef NO_IRR_USE_NON_SYSTEM_ZLIB_
#undef _IRR_USE_NON_SYSTEM_ZLIB_
#endif

//! Define _IRR_COMPILE_WITH_BZIP2_ if you want to support bzip2 compressed zip archives
/** bzip2 is superior to the original zip file compression modes, but requires
a certain amount of memory for decompression and adds several files to the
library. */
//#define _IRR_COMPILE_WITH_BZIP2_
#ifdef NO_IRR_COMPILE_WITH_BZIP2_
#undef _IRR_COMPILE_WITH_BZIP2_
#endif
//! Define _IRR_USE_NON_SYSTEM_BZLIB_ to let irrlicht use the bzlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the bzlib
installed on the system. This is only used when _IRR_COMPILE_WITH_BZLIB_ is
defined.
NOTE: You will also have to modify the Makefile or project files when changing this default.
*/
#define _IRR_USE_NON_SYSTEM_BZLIB_
#ifdef NO_IRR_USE_NON_SYSTEM_BZLIB_
#undef _IRR_USE_NON_SYSTEM_BZLIB_
#endif
//! Define _IRR_COMPILE_WITH_LZMA_ if you want to use LZMA compressed zip files.
/** LZMA is a very efficient compression code, known from 7zip. Irrlicht
currently only supports zip archives, though. */
//#define _IRR_COMPILE_WITH_LZMA_
#ifdef NO_IRR_COMPILE_WITH_LZMA_
#undef _IRR_COMPILE_WITH_LZMA_
#endif
#endif

//! Define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_ if you want to mount folders as archives
#define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#endif
//! Define __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_ if you want to open ID software PAK archives
#define __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#endif


//! Set FPU settings
/** Irrlicht should use approximate float and integer fpu techniques
precision will be lower but speed higher. currently X86 only
*/
#if !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_SOLARIS_PLATFORM_)
	//#define IRRLICHT_FAST_MATH
	#ifdef NO_IRRLICHT_FAST_MATH
	#undef IRRLICHT_FAST_MATH
	#endif
#endif

// Some cleanup and standard stuff

#ifdef _IRR_WINDOWS_API_

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _IRR_STATIC_LIB_
#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define IRRLICHT_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_

#ifndef _IRR_WINDOWS_API_
	#undef _IRR_WCHAR_FILESYSTEM
#endif



//! Define __IRR_HAS_S64 if the irr::s64 type should be enable (needs long long, available on most platforms, but not part of ISO C++ 98)
#define __IRR_HAS_S64
#ifdef NO__IRR_HAS_S64
#undef __IRR_HAS_S64
#endif

// These depend on XML
#ifndef _IRR_COMPILE_WITH_XML_
	#undef _IRR_COMPILE_WITH_IRR_MESH_LOADER_
	#undef _IRR_COMPILE_WITH_IRR_WRITER_
	#undef _IRR_COMPILE_WITH_COLLADA_WRITER_
	#undef _IRR_COMPILE_WITH_COLLADA_LOADER_
#endif

#if defined(__BORLANDC__)
	#include <tchar.h>

	// Borland 5.5.1
	#if __BORLANDC__ == 0x551
		#undef _tfinddata_t
		#undef _tfindfirst
		#undef _tfindnext

		#define _tfinddata_t __tfinddata_t
		#define _tfindfirst  __tfindfirst
		#define _tfindnext   __tfindnext
		typedef long intptr_t;
	#endif
#endif

#ifndef __has_feature
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif



#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__

