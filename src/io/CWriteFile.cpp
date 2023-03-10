// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CWriteFile.h"
#include <stdio.h>

namespace irr
{
namespace io
{


CWriteFile::CWriteFile(const io::path& fileName, bool append)
: FileSize(0)
{

	setDebugName("CWriteFile");


	Filename = fileName;
	openFile(append);
}



CWriteFile::~CWriteFile()
{
	if (File) SDL_FreeRW(File);
}



//! returns if file is open
inline bool CWriteFile::isOpen() const
{
	return File != 0;
}



//! returns how much was read
size_t CWriteFile::write(const void* buffer, size_t sizeToWrite)
{
	if (!isOpen())
		return 0;


		return (s32)SDL_RWwrite(File,buffer,1,sizeToWrite);
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CWriteFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	return SDL_RWseek(File,finalPos,relativeMovement ? RW_SEEK_CUR : RW_SEEK_SET) == 0;
}



//! returns where in the file we are.
long CWriteFile::getPos() const
{
		return (long)SDL_RWtell(File);
}



//! opens the file
void CWriteFile::openFile(bool append)
{
	if (Filename.size() == 0)
	{
		File = 0;
		return;
	}


//	File = fopen(Filename.c_str(), append ? "ab" : "wb");
	File=SDL_RWFromFile(Filename.c_str(), append ? "ab" : "wb");

	if (File)
	{
		// get FileSize

		// get FileSize
		SDL_RWseek(File,0,RW_SEEK_END);

		//fseek(File, 0, SEEK_END);
		FileSize = getPos();

		SDL_RWseek(File,0,RW_SEEK_SET);
		//fseek(File, 0, SEEK_SET);
	}
}



//! returns name of file
const io::path& CWriteFile::getFileName() const
{
	return Filename;
}

//! Flush the content of the buffer in the file
bool CWriteFile::flush()
{
	if (!isOpen())
		return false;

	//return fflush(File) == 0; // 0 indicates success, otherwise EOF and errno is set
	return true;
}

IWriteFile* CWriteFile::createWriteFile(const io::path& fileName, bool append)
{
	CWriteFile* file = new CWriteFile(fileName, append);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace irr

