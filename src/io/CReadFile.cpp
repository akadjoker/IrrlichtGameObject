// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CReadFile.h"

namespace irr
{
namespace io
{


CReadFile::CReadFile(const io::path& fileName)
: File(0), FileSize(0), Filename(fileName)
{

	setDebugName("CReadFile");


	openFile();
}


CReadFile::~CReadFile()
{
	if (File) SDL_FreeRW(File);
}


//! returns how much was read
size_t CReadFile::read(void* buffer, size_t sizeToRead)
{
	if (!isOpen())
		return 0;

    return (s32)SDL_RWread(File,buffer,1,sizeToRead);


}


//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CReadFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	//return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
	return SDL_RWseek(File,finalPos,relativeMovement ? RW_SEEK_CUR : RW_SEEK_SET) == 0;
}


//! returns size of file
long CReadFile::getSize() const
{
	return FileSize;
}


//! returns where in the file we are.
long CReadFile::getPos() const
{
    return (long)SDL_RWtell(File);
}


//! opens the file
void CReadFile::openFile()
{
	if (Filename.size() == 0) // bugfix posted by rt
	{
		File = 0;
		return;
	}



	File=SDL_RWFromFile(Filename.c_str(), "rb");


	if (File)
	{
		SDL_RWseek(File,0,RW_SEEK_END);
		FileSize = getPos();
	//	SDL_Log("[FILE] Load %s , size %d", Filename.c_str(),FileSize);
		SDL_RWseek(File,0,RW_SEEK_SET);

	} else{
			SDL_LogError(0,"[FILE] %s don't exists ", Filename.c_str());
	}
}


//! returns name of file
const io::path& CReadFile::getFileName() const
{
	return Filename;
}


IReadFile* CReadFile::createReadFile(const io::path& fileName)
{
	CReadFile* file = new CReadFile(fileName);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace irr

