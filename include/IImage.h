// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_IMAGE_H_INCLUDED__
#define __I_IMAGE_H_INCLUDED__

#include "IReferenceCounted.h"
#include "position2d.h"
#include "rect.h"
#include "SColor.h"
#include "os.h"
#include "IFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"

#include "irrAllocator.h"
#include <string.h>

namespace irr
{
namespace video
{

//! Interface for software image data.
/** Image loaders create these images from files. IVideoDrivers convert
these images into their (hardware) textures.
NOTE: Floating point formats are not well supported yet. Basically only getData() works for them.
*/
class IImage : public virtual IReferenceCounted
{
public:

	//! constructor
	IImage():
        Data(0),  BytesPerPixel(0), Pitch(0), DeleteMemory(false)
	{
	setDebugName("IIMage");
	}
	IImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size, bool deleteMemory) :
		Format(format), Size(size), Data(0), DeleteMemory(deleteMemory)

	{
		BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;
		Pitch = BytesPerPixel * Size.Width;
		setDebugName("IIMage");
	}

	IImage(const char* FileName,bool invert=false):
        Data(0), DeleteMemory(false)
	{
	    BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;
		Pitch = BytesPerPixel * Size.Width;
		setDebugName("IIMage");
	}


	//! destructor
	virtual ~IImage()
	{
		if (DeleteMemory)
			delete[] Data;


	}



		 static bool exportToBMP(io::IWriteFile* file, IImage* image);
		 static bool exportToPCX(io::IWriteFile *file, IImage *image);
		 static IImage* importPCX(io::IReadFile* file) ;
		 static IImage* importBMP(io::IReadFile* file) ;
		 static IImage* importDDS(io::IReadFile* file) ;


	virtual bool Save(const char* FileName)=0;

	//! Returns the color format
	ECOLOR_FORMAT getColorFormat() const
	{
		return Format;
	}


	//! Returns width and height of image data.
	const core::dimension2d<u32>& getDimension() const
	{
		return Size;
	}

	//! Returns bits per pixel.
	u32 getBitsPerPixel() const
	{

		return getBitsPerPixelFromFormat(Format);
	}

	//! Returns bytes per pixel
	u32 getBytesPerPixel() const
	{
		return BytesPerPixel;
	}

	//! Returns image data size in bytes
	u32 getImageDataSizeInBytes() const
	{
		return getDataSizeFromFormat(Format, Size.Width, Size.Height);
	}

	//! Returns image data size in pixels
	u32 getImageDataSizeInPixels() const
	{
		return Size.Width * Size.Height;
	}

	//! Returns pitch of image
	u32 getPitch() const
	{
		return Pitch;
	}

	//! Returns mask for red value of a pixel
	u32 getRedMask() const
	{
		switch (Format)
		{
		case ECF_A1R5G5B5:
			return 0x1F << 10;
		case ECF_R5G6B5:
			return 0x1F << 11;
		case ECF_R8G8B8:
			return 0x00FF0000;
		case ECF_A8R8G8B8:
			return 0x00FF0000;
		default:
			return 0x0;
		}
	}

	//! Returns mask for green value of a pixel
	u32 getGreenMask() const
	{
		switch (Format)
		{
		case ECF_A1R5G5B5:
			return 0x1F << 5;
		case ECF_R5G6B5:
			return 0x3F << 5;
		case ECF_R8G8B8:
			return 0x0000FF00;
		case ECF_A8R8G8B8:
			return 0x0000FF00;
		default:
			return 0x0;
		}
	}

	//! Returns mask for blue value of a pixel
	u32 getBlueMask() const
	{
		switch (Format)
		{
		case ECF_A1R5G5B5:
			return 0x1F;
		case ECF_R5G6B5:
			return 0x1F;
		case ECF_R8G8B8:
			return 0x000000FF;
		case ECF_A8R8G8B8:
			return 0x000000FF;
		default:
			return 0x0;
		}
	}

	//! Returns mask for alpha value of a pixel
	u32 getAlphaMask() const
	{
		switch (Format)
		{
		case ECF_A1R5G5B5:
			return 0x1 << 15;
		case ECF_R5G6B5:
			return 0x0;
		case ECF_R8G8B8:
			return 0x0;
		case ECF_A8R8G8B8:
			return 0xFF000000;
		default:
			return 0x0;
		}
	}

	//! Use this to get a pointer to the image data.
	/**
	\return Pointer to the image data. What type of data is pointed to
	depends on the color format of the image. For example if the color
	format is ECF_A8R8G8B8, it is of u32. */
	void* getData() const
	{
		return Data;
	}





	virtual void FlipVertical()   =0 ;
	virtual void FlipHorizontal() =0 ;



	//! Returns a pixel
	virtual SColor getPixel(u32 x, u32 y) const = 0;

	//! Sets a pixel
	virtual void setPixel(u32 x, u32 y, const SColor &color, bool blend = false ) = 0;

	//! Copies the image into the target, scaling the image to fit
	/**	NOTE: mipmaps are ignored */
	virtual void copyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0) =0;

	//! Copies the image into the target, scaling the image to fit
	/**	NOTE: mipmaps are ignored */
	virtual void copyToScaling(IImage* target) =0;

	//! copies this surface into another
	/**	NOTE: mipmaps are ignored */
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos=core::position2d<s32>(0,0)) =0;

	//! copies this surface into another
	/**	NOTE: mipmaps are ignored */
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0) =0;

	//! copies this surface into another, using the alpha mask and cliprect and a color to add with
	/**	NOTE: mipmaps are ignored
	\param combineAlpha - When true then combine alpha channels. When false replace target image alpha with source image alpha.
	*/
	virtual void copyToWithAlpha(IImage* target, const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect, const SColor &color,
			const core::rect<s32>* clipRect = 0,
			bool combineAlpha=false) =0;

	//! copies this surface into another, scaling it to fit, applying a box filter
	/**	NOTE: mipmaps are ignored */
	virtual void copyToScalingBoxFilter(IImage* target, s32 bias = 0, bool blend = false) = 0;

	//! fills the surface with given color
	virtual void fill(const SColor &color) =0;



	//! get the amount of Bits per Pixel of the given color format
	static u32 getBitsPerPixelFromFormat(const ECOLOR_FORMAT format)
	{
		switch(format)
		{
		case ECF_A1R5G5B5:
			return 16;
		case ECF_R5G6B5:
			return 16;
		case ECF_R8G8B8:
			return 24;
		case ECF_A8R8G8B8:
			return 32;

		default:
			return 0;
		}
	}

	//! calculate image data size in bytes for selected format, width and height.
	static u32 getDataSizeFromFormat(ECOLOR_FORMAT format, u32 width, u32 height)
	{
		u32 imageSize = 0;

	 // uncompressed formats
			imageSize = getBitsPerPixelFromFormat(format) / 8 * width;
			imageSize *= height;

		return imageSize;
	}


	//! check if the color format is only viable for depth/stencil textures
	static bool isDepthFormat(const ECOLOR_FORMAT format)
	{
		switch(format)
		{
			case ECF_D16:
			case ECF_D32:
			case ECF_D24S8:
				return true;
			default:
				return false;
		}
	}

	//! Check if the color format uses floating point values for pixels
	static bool isFloatingPointFormat(const ECOLOR_FORMAT format)
	{

		switch(format)
		{
		case ECF_R16F:
		case ECF_G16R16F:
		case ECF_A16B16G16R16F:
		case ECF_R32F:
		case ECF_G32R32F:
		case ECF_A32B32G32R32F:
			return true;
		default:
			break;
		}
		return false;
	}


protected:
	ECOLOR_FORMAT Format;
	core::dimension2d<u32> Size;

	u8* Data;


	u32 BytesPerPixel;
	u32 Pitch;

	bool DeleteMemory;


};


} // end namespace video
} // end namespace irr

#endif

