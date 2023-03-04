// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__

#include "IImage.h"
#include "rect.h"

#include "os.h"
#include "stb_image.h"
#include "stb_image_write.h"



enum eDDSPixelFormat
{
	DDS_PF_ARGB8888,
	DDS_PF_DXT1,
	DDS_PF_DXT2,
	DDS_PF_DXT3,
	DDS_PF_DXT4,
	DDS_PF_DXT5,
	DDS_PF_UNKNOWN
};


namespace irr
{









#include "irrpack.h"

	struct SBMPHeader
	{
		u16	Id;					//	BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
											//	BA - OS/2 Bitmap Array
											//	CI - OS/2 Color Icon
											//	CP - OS/2 Color Pointer
											//	IC - OS/2 Icon
											//	PT - OS/2 Pointer
		u32	FileSize;
		u32	Reserved;
		u32	BitmapDataOffset;
		u32	BitmapHeaderSize;	// should be 28h for windows bitmaps or
											// 0Ch for OS/2 1.x or F0h for OS/2 2.x
		u32 Width;
		u32 Height;
		u16 Planes;
		u16 BPP;					// 1: Monochrome bitmap
											// 4: 16 color bitmap
											// 8: 256 color bitmap
											// 16: 16bit (high color) bitmap
											// 24: 24bit (true color) bitmap
											// 32: 32bit (true color) bitmap

		u32  Compression;			// 0: none (Also identified by BI_RGB)
											// 1: RLE 8-bit / pixel (Also identified by BI_RLE4)
											// 2: RLE 4-bit / pixel (Also identified by BI_RLE8)
											// 3: Bitfields  (Also identified by BI_BITFIELDS)

		u32  BitmapDataSize;		// Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
		u32  PixelPerMeterX;
		u32  PixelPerMeterY;
		u32  Colors;
		u32  ImportantColors;
	} PACK_STRUCT;


	struct SPCXHeader
	{
		u8	Manufacturer;
		u8	Version;
		u8	Encoding;
		u8	BitsPerPixel;
		u16	XMin;
		u16	YMin;
		u16	XMax;
		u16	YMax;
		u16	HorizDPI;
		u16	VertDPI;
		u8	Palette[48];
		u8	Reserved;
		u8	Planes;
		u16	BytesPerLine;
		u16	PaletteType;
		u16	HScrSize;
		u16	VScrSize;
		u8	Filler[54];
	} PACK_STRUCT;



struct ddsPixelFormat
{
	u32	Size;
	u32 Flags;
	u32 FourCC;
	u32 RGBBitCount;
	u32	RBitMask;
	u32 GBitMask;
	u32 BBitMask;
	u32	ABitMask;
} PACK_STRUCT;


struct ddsCaps
{
	u32		caps1;
	u32		caps2;
	u32		caps3;
	u32		caps4;
} PACK_STRUCT;


struct ddsHeader
{
	c8 Magic[4];
	u32 Size;
	u32 Flags;
	u32 Height;
	u32 Width;
	u32 PitchOrLinearSize;
	u32 Depth;
	u32 MipMapCount;
	u32 Reserved1[11];
	ddsPixelFormat PixelFormat;
	ddsCaps Caps;
	u32 Reserved2;
} PACK_STRUCT;



struct ddsColorBlock
{
	u16		colors[ 2 ];
	u8		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlockExplicit
{
	u16		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlock3BitLinear
{
	u8		alpha0;
	u8		alpha1;
	u8		stuff[ 6 ];
} PACK_STRUCT;


struct ddsColor
{
	u8		r, g, b, a;
} PACK_STRUCT;





typedef union
{
	f32	f;
	c8	c[ 4 ];
}
floatSwapUnion;


#include "irrunpack.h"

using namespace io;
namespace video
{

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class CImage : public IImage
{
public:

    CImage();

	//! constructor from raw image data
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size, void* data,bool ownForeignMemory = true, bool deleteMemory = true);
	//! constructor for empty image
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size);

	CImage(const char* FileName,bool invert=false);


	virtual void FlipVertical()   _IRR_OVERRIDE_;
	virtual void FlipHorizontal()   _IRR_OVERRIDE_;


	//! returns a pixel
	virtual SColor getPixel(u32 x, u32 y) const _IRR_OVERRIDE_;

	//! sets a pixel
	virtual void setPixel(u32 x, u32 y, const SColor &color, bool blend = false ) _IRR_OVERRIDE_;

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch=0) _IRR_OVERRIDE_;

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(IImage* target) _IRR_OVERRIDE_;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos=core::position2d<s32>(0,0)) _IRR_OVERRIDE_;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0) _IRR_OVERRIDE_;

	virtual bool Save(const char* FileName);

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	virtual void copyToWithAlpha(IImage* target, const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect, const SColor &color,
			const core::rect<s32>* clipRect = 0, bool combineAlpha=false) _IRR_OVERRIDE_;

	//! copies this surface into another, scaling it to fit, applying a box filter
	virtual void copyToScalingBoxFilter(IImage* target, s32 bias = 0, bool blend = false) _IRR_OVERRIDE_;

	//! fills the surface with given color
	virtual void fill(const SColor &color) _IRR_OVERRIDE_;

private:
	inline SColor getPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias ) const;
	int GetPixelDataSize(int width, int height, ECOLOR_FORMAT format);
};

} // end namespace video

} // end namespace irr


#endif

