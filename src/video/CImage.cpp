// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImage.h"
#include "irrString.h"
#include "CColorConverter.h"
#include "CBlit.h"
#include "os.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



// Header flag values
#define DDSD_CAPS			0x00000001
#define DDSD_HEIGHT			0x00000002
#define DDSD_WIDTH			0x00000004
#define DDSD_PITCH			0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE		0x00080000
#define DDSD_DEPTH			0x00800000

// Pixel format flag values
#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_ALPHA			0x00000002
#define DDPF_FOURCC			0x00000004
#define DDPF_RGB			0x00000040
#define DDPF_COMPRESSED		0x00000080
#define DDPF_LUMINANCE		0x00020000

// Caps1 values
#define DDSCAPS1_COMPLEX	0x00000008
#define DDSCAPS1_TEXTURE	0x00001000
#define DDSCAPS1_MIPMAP		0x00400000

// Caps2 values
#define DDSCAPS2_CUBEMAP            0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME             0x00200000



namespace irr
{

namespace video
{


CImage::CImage()
{
}
//! Constructor from raw data
CImage::CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size, void* data,bool ownForeignMemory, bool deleteMemory)
	: IImage(format, size, deleteMemory)
{

    setDebugName("CIMage");

    BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;
	Pitch = BytesPerPixel * Size.Width;

	if (!Data)
	{
		DeleteMemory=true;
		Data = new u8[Size.Height * Pitch];
	}

	if (ownForeignMemory)
	{
		Data = (u8*)data;
	}
	else
	{
		const u32 dataSize = getDataSizeFromFormat(Format, Size.Width, Size.Height);
		Data = new u8[align_next(dataSize,16)];
		memcpy(Data, data, dataSize);
		DeleteMemory = true;
	}

}
//! Constructor of empty image
CImage::CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size) : IImage(format, size, true)
{
    BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;
	Pitch = BytesPerPixel * Size.Width;
	Data = new u8[align_next(getDataSizeFromFormat(Format, Size.Width, Size.Height),16)];
	DeleteMemory = true;
	setDebugName("CIMage");
}

int CImage::GetPixelDataSize(int width, int height, ECOLOR_FORMAT format)
{
    int dataSize = 0;       // Size in bytes
    int bpp = 0;            // Bits per pixel

    switch (format)
    {
        //case 1: bpp = 8; break;
        //case 2: bpp = 8; break;
        case video::ECF_R8G8B8  : bpp = 24; break;
        case video::ECF_A8R8G8B8: bpp = 32; break;
        default: break;
    }

    dataSize = width*height*bpp/8;  // Total data size in bytes


    return dataSize;
}
CImage::CImage(const char* FileName,bool invert)
{
int   Width, Height,channels;
unsigned int  original_texture_format = 0;
unsigned int fileSize = 0;
unsigned char *fileData = irr::core::LoadFileData(FileName, &fileSize);
if (fileData != NULL)
{
stbi_set_flip_vertically_on_load(invert);
Data= stbi_load_from_memory(fileData, fileSize, &Width,  &Height,  &channels,0);
} else
{
SDL_Log("IMAGE: [%s] Failed to load image. \n", FileName);
}

if (Data)
{

Size.Width =Width;
Size.Height=Height;



    if (channels==4)
    {
    	Format=video::ECF_A8R8G8B8;
    }
	else if (channels==3)
	{
		Format=video::ECF_R8G8B8;
    }
	else if (channels==2)
	{
		Format=video::ECF_R8G8;
    } else if (channels==1)
	{
		Format=video::ECF_R8;
    }

    /*
    	Format=video::ECF_A8R8G8B8;

           u8 *resampled   = (u8*)malloc( 4 * Width * Height*sizeof(u8) );
           core::quaternion *PIXELS = core::LoadPixelDataNormalized(Data,Width,Height,channels);     // Supports 8 to 32 bit per channel

                    for (int i = 0, k = 0; i < Width*Height*4; i += 4, k++)
                    {
                        ((u8*)resampled)[i + 0] = (u8)(PIXELS[k].X*255.0f);
                        ((u8*)resampled)[i + 1] = (u8)(PIXELS[k].Y*255.0f);
                        ((u8*)resampled)[i + 2] = (u8)(PIXELS[k].Z*255.0f);
                        ((u8*)resampled)[i + 3] = (u8)(PIXELS[k].W*255.0f);



                    }

      free(Data);
      Data=resampled;



    }
*/



    BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;
    Pitch = BytesPerPixel * Size.Width;
    const u32 dataSize = getDataSizeFromFormat(Format, Size.Width, Size.Height);
	DeleteMemory = true;


	 SDL_Log("IMAGE  Load  size(%d)   channels(%d)  BPP(%d) Pitch(%d) ", dataSize, channels,BytesPerPixel,Pitch);








}else{
	   SDL_Log("IMAGE: [%s] Failed to load image. \n", FileName);

}



}

bool CImage::Save(const char* FileName)
{
u32 pixel_size = getBytesPerPixel();
u32 row_stride = (pixel_size * getDimension().Width);


	int channels=0;
	switch(Format)
		{
		case ECF_A1R5G5B5:
			channels=4;break;
		case ECF_R5G6B5:
			channels=3;break;
		case ECF_R8G8:
			channels=2;break;
		case ECF_R8:
			channels=1;break;
        }




if (irr::core::IsFileWithExtension(FileName,"png"))
{
stbi_write_png(FileName,(int)getDimension().Width, (int)getDimension().Height,channels,Data,row_stride);
} else
if (irr::core::IsFileWithExtension(FileName,"jpeg"))
{
 stbi_write_jpg(FileName,(int)getDimension().Width, (int)getDimension().Height,channels,Data,90);
} else
if (irr::core::IsFileWithExtension(FileName,"bmp"))
{

 stbi_write_bmp(FileName,(int)getDimension().Width, (int)getDimension().Height,channels,Data);
}else
if (irr::core::IsFileWithExtension(FileName,"tga"))
{

 stbi_write_tga(FileName,(int)getDimension().Width, (int)getDimension().Height,channels,Data);
}






}




//! sets a pixel
void CImage::setPixel(u32 x, u32 y, const SColor &color, bool blend)
{
	if (x >= Size.Width || y >= Size.Height)
		return;

	switch(Format)
	{
		case ECF_A1R5G5B5:
		{
			u16 * dest = (u16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = video::A8R8G8B8toA1R5G5B5( color.color );
		} break;

		case ECF_R5G6B5:
		{
			u16 * dest = (u16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = video::A8R8G8B8toR5G6B5( color.color );
		} break;

		case ECF_R8G8B8:
		{
			u8* dest = Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (u8)color.getRed();
			dest[1] = (u8)color.getGreen();
			dest[2] = (u8)color.getBlue();
		} break;

		case ECF_A8R8G8B8:
		{
			u32 * dest = (u32*) (Data + ( y * Pitch ) + ( x << 2 ));
			*dest = blend ? PixelBlend32 ( *dest, color.color ) : color.color;
		} break;

		case ECF_UNKNOWN:
			os::Printer::log("IImage::setPixel unknown format.", ELL_WARNING);
			return;

		default:
			break;
	}
}


//! returns a pixel
SColor CImage::getPixel(u32 x, u32 y) const
{
	if (x >= Size.Width || y >= Size.Height)
		return SColor(0);

	switch(Format)
	{
	case ECF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_R5G6B5:
		return R5G6B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((u32*)Data)[y*Size.Width + x];
	case ECF_R8G8B8:
		{
			u8* p = Data+(y*3)*Size.Width + (x*3);
			return SColor(255,p[0],p[1],p[2]);
		}
	case ECF_UNKNOWN:
		os::Printer::log("IImage::getPixel unknown format.", ELL_WARNING);
		break;

	default:
		break;
	}

	return SColor(0);
}


//! copies this surface into another at given position
void CImage::copyTo(IImage* target, const core::position2d<s32>& pos)
{


	if (!Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0)
		&& target && pos.X == 0 && pos.Y == 0 &&
		CColorConverter::canConvertFormat(Format, target->getColorFormat()))
	{
		// No fast blitting, but copyToScaling uses other color conversions and might work
		irr::core::dimension2du dim(target->getDimension());
		copyToScaling(target->getData(), dim.Width, dim.Height, target->getColorFormat(), target->getPitch());
	}
}


//! copies this surface partially into another at given position
void CImage::copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect)
{


	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}


//! copies this surface into another, using the alpha mask, a cliprect and a color to add with
void CImage::copyToWithAlpha(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect, bool combineAlpha)
{
	eBlitter op = combineAlpha ? BLITTER_TEXTURE_COMBINE_ALPHA :
		color.color == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND : BLITTER_TEXTURE_ALPHA_COLOR_BLEND;
	Blit(op,target, clipRect, &pos, this, &sourceRect, color.color);
}


//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void CImage::copyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch)
{


	if (!target || !width || !height)
		return;

	const u32 bpp=getBitsPerPixelFromFormat(format)/8;
	if (0==pitch)
		pitch = width*bpp;

	if (Format==format && Size.Width==width && Size.Height==height)
	{
		if (pitch==Pitch)
		{
			memcpy(target, Data, height*pitch);
			return;
		}
		else
		{
			u8* tgtpos = (u8*) target;
			u8* srcpos = Data;
			const u32 bwidth = width*bpp;
			const u32 rest = pitch-bwidth;
			for (u32 y=0; y<height; ++y)
			{
				// copy scanline
				memcpy(tgtpos, srcpos, bwidth);
				// clear pitch
				memset(tgtpos+bwidth, 0, rest);
				tgtpos += pitch;
				srcpos += Pitch;
			}
			return;
		}
	}

	// NOTE: Scaling is coded to keep the border pixels intact.
	// Alternatively we could for example work with first pixel being taken at half step-size.
	// Then we have one more step here and it would be:
	//     sourceXStep = (f32)(Size.Width-1) / (f32)(width);
	//     And sx would start at 0.5f + sourceXStep / 2.f;
	//     Similar for y.
	// As scaling is done without any antialiasing it doesn't matter too much which outermost pixels we use and keeping
	// border pixels intact is probably mostly better (with AA the other solution would be more correct).
	const f32 sourceXStep = width > 1 ? (f32)(Size.Width-1) / (f32)(width-1) : 0.f;
	const f32 sourceYStep = height > 1 ? (f32)(Size.Height-1) / (f32)(height-1) : 0.f;
	s32 yval=0, syval=0;
	f32 sy = 0.5f;	// for rounding to nearest pixel
	for (u32 y=0; y<height; ++y)
	{
		f32 sx = 0.5f;	// for rounding to nearest pixel
		for (u32 x=0; x<width; ++x)
		{
			CColorConverter::convert_viaFormat(Data+ syval + ((s32)sx)*BytesPerPixel, Format, 1, ((u8*)target)+ yval + (x*bpp), format);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=(s32)(sy)*Pitch;
		yval+=pitch;
	}
}


//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void CImage::copyToScaling(IImage* target)
{

	if (!target)
		return;

	const core::dimension2d<u32>& targetSize = target->getDimension();

	if (targetSize==Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->getData(), targetSize.Width, targetSize.Height, target->getColorFormat());
}


//! copies this surface into another, scaling it to fit it.
void CImage::copyToScalingBoxFilter(IImage* target, s32 bias, bool blend)
{


	const core::dimension2d<u32> destSize = target->getDimension();

	const f32 sourceXStep = (f32) Size.Width / (f32) destSize.Width;
	const f32 sourceYStep = (f32) Size.Height / (f32) destSize.Height;

	target->getData();

	s32 fx = core::ceil32( sourceXStep );
	s32 fy = core::ceil32( sourceYStep );
	f32 sx;
	f32 sy;

	sy = 0.f;
	for ( u32 y = 0; y != destSize.Height; ++y )
	{
		sx = 0.f;
		for ( u32 x = 0; x != destSize.Width; ++x )
		{
			target->setPixel( x, y,
				getPixelBox( core::floor32(sx), core::floor32(sy), fx, fy, bias ), blend );
			sx += sourceXStep;
		}
		sy += sourceYStep;
	}
}


//! fills the surface with given color
void CImage::fill(const SColor &color)
{

	u32 c;

	switch ( Format )
	{
		case ECF_A1R5G5B5:
			c = color.toA1R5G5B5();
			c |= c << 16;
			break;
		case ECF_R5G6B5:
			c = video::A8R8G8B8toR5G6B5( color.color );
			c |= c << 16;
			break;
		case ECF_A8R8G8B8:
			c = color.color;
			break;
		case ECF_R8G8B8:
		{
			u8 rgb[3];
			CColorConverter::convert_A8R8G8B8toR8G8B8(&color, 1, rgb);
			const u32 size = getImageDataSizeInBytes();
			for (u32 i=0; i<size; i+=3)
			{
				memcpy(Data+i, rgb, 3);
			}
			return;
		}
		break;
		default:
		// TODO: Handle other formats
			return;
	}
	memset32( Data, c, getImageDataSizeInBytes() );
}


//! get a filtered pixel
inline SColor CImage::getPixelBox( s32 x, s32 y, s32 fx, s32 fy, s32 bias ) const
{


	SColor c;
	s32 a = 0, r = 0, g = 0, b = 0;

	for ( s32 dx = 0; dx != fx; ++dx )
	{
		for ( s32 dy = 0; dy != fy; ++dy )
		{
			c = getPixel(	core::s32_min ( x + dx, Size.Width - 1 ) ,
							core::s32_min ( y + dy, Size.Height - 1 )
						);

			a += c.getAlpha();
			r += c.getRed();
			g += c.getGreen();
			b += c.getBlue();
		}

	}

	s32 sdiv = s32_log2_s32(fx * fy);

	a = core::s32_clamp( ( a >> sdiv ) + bias, 0, 255 );
	r = core::s32_clamp( ( r >> sdiv ) + bias, 0, 255 );
	g = core::s32_clamp( ( g >> sdiv ) + bias, 0, 255 );
	b = core::s32_clamp( ( b >> sdiv ) + bias, 0, 255 );

	c.set( a, r, g, b );

	return c;
}
void CImage::FlipVertical()
{
u32 pixel_size = getBytesPerPixel();
u32 row_stride = (pixel_size * getDimension().Width);


	int channels=0;
	switch(Format)
		{
		case ECF_A1R5G5B5:
			channels=4;break;
		case ECF_R5G6B5:
			channels=3;break;
		case ECF_R8G8B8:
			channels=3;break;
		case ECF_A8R8G8B8:
			channels=4;break;
        }

        int bytesPerPixel =channels;// GetPixelDataSize(1, 1, image->format);
        unsigned char *flippedData = (u8 *)malloc(Size.Width*Size.Height*bytesPerPixel);

        for (int i = (Size.Height - 1), offsetSize = 0; i >= 0; i--)
        {
            memcpy(flippedData + offsetSize, ((u8 *)Data) + i*Size.Width*bytesPerPixel, Size.Width*bytesPerPixel);
            offsetSize += Size.Width*bytesPerPixel;
        }

        free(Data);
        Data = flippedData;

}

void CImage::FlipHorizontal()
{
        uint32_t *ptr = (uint32_t *)Data;
        for (int y = 0; y < Size.Height; y++)
        {
            for (int x = 0; x < Size.Width/2; x++)
            {
                uint32_t backup = ptr[y*Size.Width + x];
                ptr[y*Size.Width + x] = ptr[y*Size.Width + (Size.Width - 1 - x)];
                ptr[y*Size.Width + (Size.Width - 1 - x)] = backup;
            }
        }

}


#ifndef __BIG_ENDIAN__
#ifdef _SGI_SOURCE
#define	__BIG_ENDIAN__
#endif
#endif


#ifdef __BIG_ENDIAN__

	s32   DDSBigLong( s32 src ) { return src; }
	s16 DDSBigShort( s16 src ) { return src; }
	f32 DDSBigFloat( f32 src ) { return src; }

	s32 DDSLittleLong( s32 src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

	s16 DDSLittleShort( s16 src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

	f32 DDSLittleFloat( f32 src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}

#else

	s32   DDSLittleLong( s32 src ) { return src; }
	s16 DDSLittleShort( s16 src ) { return src; }
	f32 DDSLittleFloat( f32 src ) { return src; }

	s32 DDSBigLong( s32 src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

	s16 DDSBigShort( s16 src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

	f32 DDSBigFloat( f32 src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}

#endif




s32 DDSGetInfo(ddsHeader* dds, s32* width, s32* height, eDDSPixelFormat* pf)
{

	if( dds == NULL )
		return -1;


	if( *((s32*) dds->Magic) != *((s32*) "DDS ") )
		return -1;
	if( DDSLittleLong( dds->Size ) != 124 )
		return -1;
	if( !(DDSLittleLong( dds->Flags ) & DDSD_PIXELFORMAT) )
		return -1;
	if( !(DDSLittleLong( dds->Flags ) & DDSD_CAPS) )
		return -1;

	//* extract width and height
	if( width != NULL )
		*width = DDSLittleLong( dds->Width );
	if( height != NULL )
		*height = DDSLittleLong( dds->Height );

	//* get pixel format


	const u32 fourCC = dds->PixelFormat.FourCC;


	if( fourCC == 0 )
		*pf = DDS_PF_ARGB8888;
	else if( fourCC == *((u32*) "DXT1") )
		*pf = DDS_PF_DXT1;
	else if( fourCC == *((u32*) "DXT2") )
		*pf = DDS_PF_DXT2;
	else if( fourCC == *((u32*) "DXT3") )
		*pf = DDS_PF_DXT3;
	else if( fourCC == *((u32*) "DXT4") )
		*pf = DDS_PF_DXT4;
	else if( fourCC == *((u32*) "DXT5") )
		*pf = DDS_PF_DXT5;
	else
		*pf = DDS_PF_UNKNOWN;

	return 0;
}




s32 DDSDecompressARGB8888(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{

	u8* in = data;
	u8* out = pixels;


	for(s32 y = 0; y < height; y++)
	{

		for(s32 x = 0; x < width; x++)
		{
			*out++ = *in++;
			*out++ = *in++;
			*out++ = *in++;
			*out++ = *in++;
		}
	}


	return 0;
}



void DDSGetColorBlockColors(ddsColorBlock* block, ddsColor colors[4])
{
	u16		word;



	word = DDSLittleShort( block->colors[ 0 ] );
	colors[ 0 ].a = 0xff;


	colors[ 0 ].b = (u8) word;
	colors[ 0 ].b <<= 3;
	colors[ 0 ].b |= (colors[ 0 ].b >> 5);
	word >>= 5;
	colors[ 0 ].g = (u8) word;
	colors[ 0 ].g <<= 2;
	colors[ 0 ].g |= (colors[ 0 ].g >> 5);
	word >>= 6;
	colors[ 0 ].r = (u8) word;
	colors[ 0 ].r <<= 3;
	colors[ 0 ].r |= (colors[ 0 ].r >> 5);


	word = DDSLittleShort( block->colors[ 1 ] );
	colors[ 1 ].a = 0xff;


	colors[ 1 ].b = (u8) word;
	colors[ 1 ].b <<= 3;
	colors[ 1 ].b |= (colors[ 1 ].b >> 5);
	word >>= 5;
	colors[ 1 ].g = (u8) word;
	colors[ 1 ].g <<= 2;
	colors[ 1 ].g |= (colors[ 1 ].g >> 5);
	word >>= 6;
	colors[ 1 ].r = (u8) word;
	colors[ 1 ].r <<= 3;
	colors[ 1 ].r |= (colors[ 1 ].r >> 5);


	if( block->colors[ 0 ] > block->colors[ 1 ] )
	{
		/* four-color block: derive the other two colors.
		00 = color 0, 01 = color 1, 10 = color 2, 11 = color 3
		these two bit codes correspond to the 2-bit fields
		stored in the 64-bit block. */

		word = ((u16) colors[ 0 ].r * 2 + (u16) colors[ 1 ].r ) / 3;
		/* no +1 for rounding */
		/* as bits have been shifted to 888 */
		colors[ 2 ].r = (u8) word;
		word = ((u16) colors[ 0 ].g * 2 + (u16) colors[ 1 ].g) / 3;
		colors[ 2 ].g = (u8) word;
		word = ((u16) colors[ 0 ].b * 2 + (u16) colors[ 1 ].b) / 3;
		colors[ 2 ].b = (u8) word;
		colors[ 2 ].a = 0xff;

		word = ((u16) colors[ 0 ].r + (u16) colors[ 1 ].r * 2) / 3;
		colors[ 3 ].r = (u8) word;
		word = ((u16) colors[ 0 ].g + (u16) colors[ 1 ].g * 2) / 3;
		colors[ 3 ].g = (u8) word;
		word = ((u16) colors[ 0 ].b + (u16) colors[ 1 ].b * 2) / 3;
		colors[ 3 ].b = (u8) word;
		colors[ 3 ].a = 0xff;
	}
	else
	{
		/* three-color block: derive the other color.
		00 = color 0, 01 = color 1, 10 = color 2,
		11 = transparent.
		These two bit codes correspond to the 2-bit fields
		stored in the 64-bit block */

		word = ((u16) colors[ 0 ].r + (u16) colors[ 1 ].r) / 2;
		colors[ 2 ].r = (u8) word;
		word = ((u16) colors[ 0 ].g + (u16) colors[ 1 ].g) / 2;
		colors[ 2 ].g = (u8) word;
		word = ((u16) colors[ 0 ].b + (u16) colors[ 1 ].b) / 2;
		colors[ 2 ].b = (u8) word;
		colors[ 2 ].a = 0xff;

		/* random color to indicate alpha */
		colors[ 3 ].r = 0x00;
		colors[ 3 ].g = 0xff;
		colors[ 3 ].b = 0xff;
		colors[ 3 ].a = 0x00;
	}
}


/*
DDSDecodeColorBlock()
decodes a dds color block
fixme: make endian-safe
*/

void DDSDecodeColorBlock(u32* pixel, ddsColorBlock* block, s32 width, u32 colors[4])
{
	s32				r, n;
	u32	bits;
	u32	masks[] = { 3, 12, 3 << 4, 3 << 6 };	/* bit masks = 00000011, 00001100, 00110000, 11000000 */
	s32				shift[] = { 0, 2, 4, 6 };


	/* r steps through lines in y */
	for( r = 0; r < 4; r++, pixel += (width - 4) )	/* no width * 4 as u32 ptr inc will * 4 */
	{
		/* width * 4 bytes per pixel per line, each j dxtc row is 4 lines of pixels */

		/* n steps through pixels */
		for( n = 0; n < 4; n++ )
		{
			bits = block->row[ r ] & masks[ n ];
			bits >>= shift[ n ];

			switch( bits )
			{
			case 0:
				*pixel = colors[ 0 ];
				pixel++;
				break;

			case 1:
				*pixel = colors[ 1 ];
				pixel++;
				break;

			case 2:
				*pixel = colors[ 2 ];
				pixel++;
				break;

			case 3:
				*pixel = colors[ 3 ];
				pixel++;
				break;

			default:
				/* invalid */
				pixel++;
				break;
			}
		}
	}
}


/*
DDSDecodeAlphaExplicit()
decodes a dds explicit alpha block
*/
void DDSDecodeAlphaExplicit(u32* pixel, ddsAlphaBlockExplicit* alphaBlock, s32 width, u32 alphaZero)
{
	s32				row, pix;
	u16	word;
	ddsColor		color;


	/* clear color */
	color.r = 0;
	color.g = 0;
	color.b = 0;

	/* walk rows */
	for( row = 0; row < 4; row++, pixel += (width - 4) )
	{
		word = DDSLittleShort( alphaBlock->row[ row ] );

		/* walk pixels */
		for( pix = 0; pix < 4; pix++ )
		{
			/* zero the alpha bits of image pixel */
			*pixel &= alphaZero;
			color.a = word & 0x000F;
			color.a = color.a | (color.a << 4);
			*pixel |= *((u32*) &color);
			word >>= 4;		/* move next bits to lowest 4 */
			pixel++;		/* move to next pixel in the row */
		}
	}
}



/*
DDSDecodeAlpha3BitLinear()
decodes interpolated alpha block
*/
void DDSDecodeAlpha3BitLinear(u32* pixel, ddsAlphaBlock3BitLinear* alphaBlock, s32 width, u32 alphaZero)
{

	s32 row, pix;
	u32 stuff;
	u8 bits[ 4 ][ 4 ];
	u16 alphas[ 8 ];
	ddsColor aColors[ 4 ][ 4 ];

	/* get initial alphas */
	alphas[ 0 ] = alphaBlock->alpha0;
	alphas[ 1 ] = alphaBlock->alpha1;

	/* 8-alpha block */
	if( alphas[ 0 ] > alphas[ 1 ] )
	{
		/* 000 = alpha_0, 001 = alpha_1, others are interpolated */
		alphas[ 2 ] = ( 6 * alphas[ 0 ] +     alphas[ 1 ]) / 7;	/* bit code 010 */
		alphas[ 3 ] = ( 5 * alphas[ 0 ] + 2 * alphas[ 1 ]) / 7;	/* bit code 011 */
		alphas[ 4 ] = ( 4 * alphas[ 0 ] + 3 * alphas[ 1 ]) / 7;	/* bit code 100 */
		alphas[ 5 ] = ( 3 * alphas[ 0 ] + 4 * alphas[ 1 ]) / 7;	/* bit code 101 */
		alphas[ 6 ] = ( 2 * alphas[ 0 ] + 5 * alphas[ 1 ]) / 7;	/* bit code 110 */
		alphas[ 7 ] = (     alphas[ 0 ] + 6 * alphas[ 1 ]) / 7;	/* bit code 111 */
	}

	/* 6-alpha block */
	else
	{
		/* 000 = alpha_0, 001 = alpha_1, others are interpolated */
		alphas[ 2 ] = (4 * alphas[ 0 ] +     alphas[ 1 ]) / 5;	/* bit code 010 */
		alphas[ 3 ] = (3 * alphas[ 0 ] + 2 * alphas[ 1 ]) / 5;	/* bit code 011 */
		alphas[ 4 ] = (2 * alphas[ 0 ] + 3 * alphas[ 1 ]) / 5;	/* bit code 100 */
		alphas[ 5 ] = (    alphas[ 0 ] + 4 * alphas[ 1 ]) / 5;	/* bit code 101 */
		alphas[ 6 ] = 0;										/* bit code 110 */
		alphas[ 7 ] = 255;										/* bit code 111 */
	}

	/* decode 3-bit fields into array of 16 bytes with same value */

	/* first two rows of 4 pixels each */
	stuff = *((u32*) &(alphaBlock->stuff[ 0 ]));

	bits[ 0 ][ 0 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 0 ][ 1 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 0 ][ 2 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 0 ][ 3 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 1 ][ 0 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 1 ][ 1 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 1 ][ 2 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 1 ][ 3 ] = (u8) (stuff & 0x00000007);

	/* last two rows */
	stuff = *((u32*) &(alphaBlock->stuff[ 3 ])); /* last 3 bytes */

	bits[ 2 ][ 0 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 2 ][ 1 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 2 ][ 2 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 2 ][ 3 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 3 ][ 0 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 3 ][ 1 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 3 ][ 2 ] = (u8) (stuff & 0x00000007);
	stuff >>= 3;
	bits[ 3 ][ 3 ] = (u8) (stuff & 0x00000007);

	/* decode the codes into alpha values */
	for( row = 0; row < 4; row++ )
	{
		for( pix=0; pix < 4; pix++ )
		{
			aColors[ row ][ pix ].r = 0;
			aColors[ row ][ pix ].g = 0;
			aColors[ row ][ pix ].b = 0;
			aColors[ row ][ pix ].a = (u8) alphas[ bits[ row ][ pix ] ];
		}
	}

	/* write out alpha values to the image bits */
	for( row = 0; row < 4; row++, pixel += width-4 )
	{
		for( pix = 0; pix < 4; pix++ )
		{
			/* zero the alpha bits of image pixel */
			*pixel &= alphaZero;

			/* or the bits into the prev. nulled alpha */
			*pixel |= *((u32*) &(aColors[ row ][ pix ]));
			pixel++;
		}
	}
}


/*
DDSDecompressDXT1()
decompresses a dxt1 format texture
*/
s32 DDSDecompressDXT1(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{
	s32 x, y, xBlocks, yBlocks;
	u32 *pixel;
	ddsColorBlock *block;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block */
		block = (ddsColorBlock*) (data + y * xBlocks * 8);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			DDSGetColorBlockColors( block, colors );
			pixel = (u32*) (pixels + x * 16 + (y * 4) * width * 4);
			DDSDecodeColorBlock( pixel, block, width, (u32*) colors );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT3()
decompresses a dxt3 format texture
*/

s32 DDSDecompressDXT3(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{
	s32 x, y, xBlocks, yBlocks;
	u32 *pixel, alphaZero;
	ddsColorBlock *block;
	ddsAlphaBlockExplicit *alphaBlock;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* create zero alpha */
	colors[ 0 ].a = 0;
	colors[ 0 ].r = 0xFF;
	colors[ 0 ].g = 0xFF;
	colors[ 0 ].b = 0xFF;
	alphaZero = *((u32*) &colors[ 0 ]);

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block, 1 block for alpha, 1 block for color */
		block = (ddsColorBlock*) (data + y * xBlocks * 16);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			/* get alpha block */
			alphaBlock = (ddsAlphaBlockExplicit*) block;

			/* get color block */
			block++;
			DDSGetColorBlockColors( block, colors );

			/* decode color block */
			pixel = (u32*) (pixels + x * 16 + (y * 4) * width * 4);
			DDSDecodeColorBlock( pixel, block, width, (u32*) colors );

			/* overwrite alpha bits with alpha block */
			DDSDecodeAlphaExplicit( pixel, alphaBlock, width, alphaZero );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT5()
decompresses a dxt5 format texture
*/
s32 DDSDecompressDXT5(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{
	s32 x, y, xBlocks, yBlocks;
	u32 *pixel, alphaZero;
	ddsColorBlock *block;
	ddsAlphaBlock3BitLinear *alphaBlock;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* create zero alpha */
	colors[ 0 ].a = 0;
	colors[ 0 ].r = 0xFF;
	colors[ 0 ].g = 0xFF;
	colors[ 0 ].b = 0xFF;
	alphaZero = *((u32*) &colors[ 0 ]);

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block, 1 block for alpha, 1 block for color */
		block = (ddsColorBlock*) (data + y * xBlocks * 16);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			/* get alpha block */
			alphaBlock = (ddsAlphaBlock3BitLinear*) block;

			/* get color block */
			block++;
			DDSGetColorBlockColors( block, colors );

			/* decode color block */
			pixel = (u32*) (pixels + x * 16 + (y * 4) * width * 4);
			DDSDecodeColorBlock( pixel, block, width, (u32*) colors );

			/* overwrite alpha bits with alpha block */
			DDSDecodeAlpha3BitLinear( pixel, alphaBlock, width, alphaZero );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT2()
decompresses a dxt2 format texture (fixme: un-premultiply alpha)
*/
s32 DDSDecompressDXT2(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{
	/* decompress dxt3 first */
	const s32 r = DDSDecompressDXT3( dds, data, width, height, pixels );

	/* return to sender */
	return r;
}


/*
DDSDecompressDXT4()
decompresses a dxt4 format texture (fixme: un-premultiply alpha)
*/
s32 DDSDecompressDXT4(ddsHeader* dds, u8* data, s32 width, s32 height, u8* pixels)
{
	/* decompress dxt5 first */
	const s32 r = DDSDecompressDXT5( dds, data, width, height, pixels );

	/* return to sender */
	return r;
}


/*
DDSDecompress()
decompresses a dds texture into an rgba image buffer, returns 0 on success
*/
s32 DDSDecompress(ddsHeader* dds, u8* data, u8* pixels)
{
	s32 width, height;
	eDDSPixelFormat pf;

	/* get dds info */
	s32 r = DDSGetInfo( dds, &width, &height, &pf );
	if ( r )
		return r;

	/* decompress */
	switch( pf )
	{
	case DDS_PF_ARGB8888:
		/* fixme: support other [a]rgb formats */
		r = DDSDecompressARGB8888( dds, data, width, height, pixels );
		break;

	case DDS_PF_DXT1:
		r = DDSDecompressDXT1( dds, data, width, height, pixels );
		break;

	case DDS_PF_DXT2:
		r = DDSDecompressDXT2( dds, data, width, height, pixels );
		break;

	case DDS_PF_DXT3:
		r = DDSDecompressDXT3( dds, data, width, height, pixels );
		break;

	case DDS_PF_DXT4:
		r = DDSDecompressDXT4( dds, data, width, height, pixels );
		break;

	case DDS_PF_DXT5:
		r = DDSDecompressDXT5( dds, data, width, height, pixels );
		break;

	default: // DDS_PF_UNKNOWN
		r = -1;
		break;
	}

	/* return to sender */
	return r;
}




void decompress8BitRLE(u8*& bmpData, s32 size, s32 width, s32 height, s32 pitch)
{
	u8* p = bmpData;
	u8* newBmp = new u8[(width+pitch)*height];
	u8* d = newBmp;
	u8* destEnd = newBmp + (width+pitch)*height;
	s32 line = 0;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*(width+pitch));
				break;
			case 1: // end of bmp
				delete [] bmpData;
				bmpData = newBmp;
				return;
			case 2:
				++p; d +=(u8)*p;  // delta
				++p; d += ((u8)*p)*(width+pitch);
				++p;
				break;
			default:
				{
					// absolute mode
					s32 count = (u8)*p; ++p;
					s32 readAdditional = ((2-(count%2))%2);
					s32 i;

					for (i=0; i<count; ++i)
					{
						*d = *p;
						++p;
						++d;
					}

					for (i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else
		{
			s32 count = (u8)*p; ++p;
			u8 color = *p; ++p;
			for (s32 i=0; i<count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}

	delete [] bmpData;
	bmpData = newBmp;
}


void decompress4BitRLE(u8*& bmpData, s32 size, s32 width, s32 height, s32 pitch)
{
	s32 lineWidth = (width+1)/2+pitch;
	u8* p = bmpData;
	u8* newBmp = new u8[lineWidth*height];
	u8* d = newBmp;
	u8* destEnd = newBmp + lineWidth*height;
	s32 line = 0;
	s32 shift = 4;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*lineWidth);
				shift = 4;
				break;
			case 1: // end of bmp
				delete [] bmpData;
				bmpData = newBmp;
				return;
			case 2:
				{
					++p;
					s32 x = (u8)*p; ++p;
					s32 y = (u8)*p; ++p;
					d += x/2 + y*lineWidth;
					shift = x%2==0 ? 4 : 0;
				}
				break;
			default:
				{
					// absolute mode
					s32 count = (u8)*p; ++p;
					s32 readAdditional = ((2-((count)%2))%2);
					s32 readShift = 4;
					s32 i;

					for (i=0; i<count; ++i)
					{
						s32 color = (((u8)*p) >> readShift) & 0x0f;
						readShift -= 4;
						if (readShift < 0)
						{
							++*p;
							readShift = 4;
						}

						u8 mask = 0x0f << shift;
						*d = (*d & (~mask)) | ((color << shift) & mask);

						shift -= 4;
						if (shift < 0)
						{
							shift = 4;
							++d;
						}

					}

					for (i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else
		{
			s32 count = (u8)*p; ++p;
			s32 color1 = (u8)*p; color1 = color1 & 0x0f;
			s32 color2 = (u8)*p; color2 = (color2 >> 4) & 0x0f;
			++p;

			for (s32 i=0; i<count; ++i)
			{
				u8 mask = 0x0f << shift;
				u8 toSet = (shift==0 ? color1 : color2) << shift;
				*d = (*d & (~mask)) | (toSet & mask);

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
		}
	}

	delete [] bmpData;
	bmpData = newBmp;
}




IImage*  IImage::importDDS(io::IReadFile* file)
{

	IImage* image = 0;

	return image;
}

bool IImage::exportToBMP(io::IWriteFile* file, IImage* image)
{
// we always write 24-bit color because nothing really reads 32-bit

	SBMPHeader imageHeader;
	imageHeader.Id = 0x4d42;
	imageHeader.Reserved = 0;
	imageHeader.BitmapDataOffset = sizeof(imageHeader);
	imageHeader.BitmapHeaderSize = 0x28;
	imageHeader.Width = image->getDimension().Width;
	imageHeader.Height = image->getDimension().Height;
	imageHeader.Planes = 1;
	imageHeader.BPP = 24;
	imageHeader.Compression = 0;
	imageHeader.PixelPerMeterX = 0;
	imageHeader.PixelPerMeterY = 0;
	imageHeader.Colors = 0;
	imageHeader.ImportantColors = 0;

	// data size is rounded up to next larger 4 bytes boundary
	imageHeader.BitmapDataSize = imageHeader.Width * imageHeader.BPP / 8;
	imageHeader.BitmapDataSize = (imageHeader.BitmapDataSize + 3) & ~3;
	imageHeader.BitmapDataSize *= imageHeader.Height;

	// file size is data size plus offset to data
	imageHeader.FileSize = imageHeader.BitmapDataOffset + imageHeader.BitmapDataSize;

	// bitmaps are stored upside down and padded so we always do this
	void (*CColorConverter_convertFORMATtoFORMAT)(const void*, s32, void*) = 0;
	switch(image->getColorFormat())
	{
	case ECF_R8G8B8:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_R8G8B8toR8G8B8;
		break;
	case ECF_A8R8G8B8:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_A8R8G8B8toB8G8R8;
		break;
	case ECF_A1R5G5B5:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_A1R5G5B5toR8G8B8;
		break;
	case ECF_R5G6B5:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_R5G6B5toR8G8B8;
		break;
	default:
		os::Printer::log("CImageWriterBMP does not support image format", ColorFormatNames[image->getColorFormat()], ELL_WARNING);
		break;
	}

	// couldn't find a color converter
	if (!CColorConverter_convertFORMATtoFORMAT)
		return false;

	// write the bitmap header
	if (file->write(&imageHeader, sizeof(imageHeader)) != sizeof(imageHeader))
		return false;

	u8* scan_lines = (u8*)image->getData();
	if (!scan_lines)
		return false;

	// size of one pixel in bytes
	u32 pixel_size = image->getBytesPerPixel();

	// length of one row of the source image in bytes
	u32 row_stride = (pixel_size * imageHeader.Width);

	// length of one row in bytes, rounded up to nearest 4-byte boundary
	size_t row_size = ((3 * imageHeader.Width) + 3) & ~3;

	// allocate and clear memory for our scan line
	u8* row_pointer = new u8[row_size];
	memset(row_pointer, 0, row_size);

	// convert the image to 24-bit BGR and flip it over
	s32 y;
	for (y = imageHeader.Height - 1; 0 <= y; --y)
	{
		if (image->getColorFormat()==ECF_R8G8B8)
			CColorConverter::convert24BitTo24Bit(&scan_lines[y * row_stride], row_pointer, imageHeader.Width, 1, 0, false, true);
		else
			// source, length [pixels], destination
			CColorConverter_convertFORMATtoFORMAT(&scan_lines[y * row_stride], imageHeader.Width, row_pointer);
		if (file->write(row_pointer, row_size) < row_size)
			break;
	}

	// clean up our scratch area
	delete [] row_pointer;

	return y < 0;
}
bool IImage::exportToPCX(io::IWriteFile *file, IImage *image)
{
	if (!file || !image)
		return false;

	u8 d1;
	u16 d2;
	u32 i;

	d1 = 10; // Manufacturer
	file->write(&d1, 1);
	d1 = 5; // Version
	file->write(&d1, 1);
	d1 = 1; // Encoding
	file->write(&d1, 1);
	d1 = 8; // Bits per Pixel
	file->write(&d1, 1);
	d2 = 0; // pixel origin
	file->write(&d2, 2);
	file->write(&d2, 2);
	d2 = image->getDimension().Width-1; // width
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = image->getDimension().Height-1; // height
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = 300; // dpi
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	file->write(&d2, 2);
	d2 = 0; // palette (not used)
	for (i=0; i<24; ++i)
	{
		file->write(&d2, 2);
	}
	d1 = 0; // reserved
	file->write(&d1, 1);
	d1 = 3; // planes
	file->write(&d1, 1);
	d2 = image->getDimension().Width; // pitch
	if (d2&0x0001) // must be even
		++d2;
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = 1; // color mode
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = 800; // screen width
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = 600; // screen height
#ifdef __BIG_ENDIAN__
	d2 = os::Byteswap::byteswap(d2);
#endif
	file->write(&d2, 2);
	d2 = 0; // filler (not used)
	for (i=0; i<27; ++i)
	{
		file->write(&d2, 2);
	}

	u8 cnt, value;
	for (i=0; i<image->getDimension().Height; ++i)
	{
		cnt = 0;
		value = 0;
		for (u32 j=0; j<3; ++j) // color planes
		{
			for (u32 k=0; k<image->getDimension().Width; ++k)
			{
				const SColor pix = image->getPixel(k,i);
				if ((cnt!=0) && (cnt<63) &&
					(((j==0) && (value==pix.getRed())) ||
					((j==1) && (value==pix.getGreen())) ||
					((j==2) && (value==pix.getBlue()))))
				{
					++cnt;
				}
				else
				{
					if (cnt!=0)
					{
						if ((cnt>1) || ((value&0xc0)==0xc0))
						{
							cnt |= 0xc0;
							file->write(&cnt, 1);
						}
						file->write(&value, 1);
					}
					cnt=1;
					if (j==0)
						value=(u8)pix.getRed();
					else if (j==1)
						value=(u8)pix.getGreen();
					else if (j==2)
						value=(u8)pix.getBlue();
				}
			}
		}
		if ((cnt>1) || ((value&0xc0)==0xc0))
		{
			cnt |= 0xc0;
			file->write(&cnt, 1);
		}
		file->write(&value, 1);
	}

	return true;
	}

IImage* IImage::importPCX(io::IReadFile* file)
{
	SPCXHeader header;
	s32* paletteData = 0;

	file->read(&header, sizeof(header));
	#ifdef __BIG_ENDIAN__
		header.XMin = os::Byteswap::byteswap(header.XMin);
		header.YMin = os::Byteswap::byteswap(header.YMin);
		header.XMax = os::Byteswap::byteswap(header.XMax);
		header.YMax = os::Byteswap::byteswap(header.YMax);
		header.HorizDPI = os::Byteswap::byteswap(header.HorizDPI);
		header.VertDPI = os::Byteswap::byteswap(header.VertDPI);
		header.BytesPerLine = os::Byteswap::byteswap(header.BytesPerLine);
		header.PaletteType = os::Byteswap::byteswap(header.PaletteType);
		header.HScrSize = os::Byteswap::byteswap(header.HScrSize);
		header.VScrSize = os::Byteswap::byteswap(header.VScrSize);
	#endif

	//! return if the header is wrong
	if (header.Manufacturer != 0x0a && header.Encoding != 0x01)
		return 0;

	// return if this isn't a supported type
	if ((header.BitsPerPixel != 8) && (header.BitsPerPixel != 4) && (header.BitsPerPixel != 1))
	{
		os::Printer::log("Unsupported bits per pixel in PCX file.",
			file->getFileName(), irr::ELL_WARNING);
		return 0;
	}

	// read palette
	if( (header.BitsPerPixel == 8) && (header.Planes == 1) )
	{
		// the palette indicator (usually a 0x0c is found infront of the actual palette data)
		// is ignored because some exporters seem to forget to write it. This would result in
		// no image loaded before, now only wrong colors will be set.
		const long pos = file->getPos();
		file->seek( file->getSize()-256*3, false );

		u8 *tempPalette = new u8[768];
		paletteData = new s32[256];
		file->read( tempPalette, 768 );

		for( s32 i=0; i<256; i++ )
		{
			paletteData[i] = (0xff000000 |
					(tempPalette[i*3+0] << 16) |
					(tempPalette[i*3+1] << 8) |
					(tempPalette[i*3+2]));
		}

		delete [] tempPalette;

		file->seek(pos);
	}
	else if( header.BitsPerPixel == 4 )
	{
		paletteData = new s32[16];
		for( s32 i=0; i<16; i++ )
		{
			paletteData[i] = (0xff000000 |
					(header.Palette[i*3+0] << 16) |
					(header.Palette[i*3+1] << 8) |
					(header.Palette[i*3+2]));
		}
	}

	// read image data
	const s32 width = header.XMax - header.XMin + 1;
	const s32 height = header.YMax - header.YMin + 1;
	const s32 imagebytes = header.BytesPerLine * header.Planes * header.BitsPerPixel * height / 8;
	u8* PCXData = new u8[imagebytes];

	u8 cnt, value;
	s32 lineoffset=0, linestart=0, nextmode=1;
	for(s32 offset = 0; offset < imagebytes; offset += cnt)
	{
		file->read(&cnt, 1);
		if( !((cnt & 0xc0) == 0xc0) )
		{
			value = cnt;
			cnt = 1;
		}
		else
		{
			cnt &= 0x3f;
			file->read(&value, 1);
		}
		if (header.Planes==1)
			memset(PCXData+offset, value, cnt);
		else
		{
			for (u8 i=0; i<cnt; ++i)
			{
				PCXData[linestart+lineoffset]=value;
				lineoffset += 3;
				if (lineoffset>=3*header.BytesPerLine)
				{
					lineoffset=nextmode;
					if (++nextmode==3)
						nextmode=0;
					if (lineoffset==0)
						linestart += 3*header.BytesPerLine;
				}
			}
		}
	}

	// create image
	video::IImage* image = 0;
	s32 pad = (header.BytesPerLine - width * header.BitsPerPixel / 8) * header.Planes;

	if (pad < 0)
		pad = -pad;

	if (header.BitsPerPixel==8)
	{
		switch(header.Planes) // TODO: Other formats
		{
		case 1:
			image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(width, height));
			if (image)
				CColorConverter::convert8BitTo16Bit(PCXData, (s16*)image->getData(), width, height, paletteData, pad);
			break;
		case 3:
			image = new CImage(ECF_R8G8B8, core::dimension2d<u32>(width, height));
			if (image)
				CColorConverter::convert24BitTo24Bit(PCXData, (u8*)image->getData(), width, height, pad);
			break;
		}
	}
	else if (header.BitsPerPixel==4)
	{
		if (header.Planes==1)
		{
			image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(width, height));
			if (image)
				CColorConverter::convert4BitTo16Bit(PCXData, (s16*)image->getData(), width, height, paletteData, pad);
		}
	}
	else if (header.BitsPerPixel==1)
	{
		if (header.Planes==4)
		{
			image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(width, height));
			if (image)
				CColorConverter::convert4BitTo16Bit(PCXData, (s16*)image->getData(), width, height, paletteData, pad);
		}
		else if (header.Planes==1)
		{
			image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(width, height));
			if (image)
				CColorConverter::convert1BitTo16Bit(PCXData, (s16*)image->getData(), width, height, pad);
		}
	}

	// clean up

	delete [] paletteData;
	delete [] PCXData;

	return image;
}

IImage* IImage::importBMP(io::IReadFile* file)
{
	SBMPHeader header;

	file->read(&header, sizeof(header));

#ifdef __BIG_ENDIAN__
	header.Id = os::Byteswap::byteswap(header.Id);
	header.FileSize = os::Byteswap::byteswap(header.FileSize);
	header.BitmapDataOffset = os::Byteswap::byteswap(header.BitmapDataOffset);
	header.BitmapHeaderSize = os::Byteswap::byteswap(header.BitmapHeaderSize);
	header.Width = os::Byteswap::byteswap(header.Width);
	header.Height = os::Byteswap::byteswap(header.Height);
	header.Planes = os::Byteswap::byteswap(header.Planes);
	header.BPP = os::Byteswap::byteswap(header.BPP);
	header.Compression = os::Byteswap::byteswap(header.Compression);
	header.BitmapDataSize = os::Byteswap::byteswap(header.BitmapDataSize);
	header.PixelPerMeterX = os::Byteswap::byteswap(header.PixelPerMeterX);
	header.PixelPerMeterY = os::Byteswap::byteswap(header.PixelPerMeterY);
	header.Colors = os::Byteswap::byteswap(header.Colors);
	header.ImportantColors = os::Byteswap::byteswap(header.ImportantColors);
#endif

	s32 pitch = 0;

	//! return if the header is false

	if (header.Id != 0x4d42)
		return 0;

	if (header.Compression > 2) // we'll only handle RLE-Compression
	{
		os::Printer::log("Compression mode not supported.", ELL_ERROR);
		return 0;
	}

	// adjust bitmap data size to dword boundary
	header.BitmapDataSize += (4-(header.BitmapDataSize%4))%4;

	// read palette

	long pos = file->getPos();
	s32 paletteSize = (header.BitmapDataOffset - pos) / 4;

	s32* paletteData = 0;
	if (paletteSize)
	{
		paletteData = new s32[paletteSize];
		file->read(paletteData, paletteSize * sizeof(s32));
#ifdef __BIG_ENDIAN__
		for (s32 i=0; i<paletteSize; ++i)
			paletteData[i] = os::Byteswap::byteswap(paletteData[i]);
#endif
	}

	// read image data

	if (!header.BitmapDataSize)
	{
		// okay, lets guess the size
		// some tools simply don't set it
		header.BitmapDataSize = static_cast<u32>(file->getSize()) - header.BitmapDataOffset;
	}

	file->seek(header.BitmapDataOffset);

	f32 t = (header.Width) * (header.BPP / 8.0f);
	s32 widthInBytes = (s32)t;
	t -= widthInBytes;
	if (t!=0.0f)
		++widthInBytes;

	s32 lineData = widthInBytes + ((4-(widthInBytes%4)))%4;
	pitch = lineData - widthInBytes;

	u8* bmpData = new u8[header.BitmapDataSize];
	file->read(bmpData, header.BitmapDataSize);



	// decompress data if needed
	switch(header.Compression)
	{
	case 1: // 8 bit rle
		decompress8BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	case 2: // 4 bit rle
		decompress4BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	}

	// create surface



	// no default constructor from packed area! ARM problem!
	core::dimension2d<u32> dim;
	dim.Width = header.Width;
	dim.Height = header.Height;




 int new_width = 1;
int new_height = 1;
while( new_width < dim.Width )
{
	new_width *= 2;
}
while( new_height < dim.Height )
{
	new_height *= 2;
}
/*
if( (new_width != dim.Width) || (new_height != dim.Height) )
{
	os::Printer::log("[IMAGE] Resample image ",file->getFileName(), ELL_INFORMATION);

	u8 *resampled = (unsigned char*)malloc( header.BPP*new_width*new_height );
		up_scale_image(bmpData, dim.Width, dim.Height, header.BPP,resampled, new_width, new_height );

		free( (u8*)bmpData );
		bmpData = resampled;
		dim.Width = new_width;
		dim.Height = new_height;
}

*/




	IImage* image = 0;


//	SDL_Log("[IMAGE] *** size %d compress %d   bpp  (%d)",header.BitmapDataSize,header.Compression,header.BPP);

	switch(header.BPP)
	{
	case 1:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert1BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	case 4:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert4BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 8:
        {

        image = new CImage(ECF_R8G8B8, dim);
        if (image)
		{
         CColorConverter::convert8BitToRGB(bmpData,  (u8*)image->getData(), header.Width, header.Height, paletteData, pitch);
         image->FlipVertical();

        }
/*
   //     s16* data = (s16 *)malloc(header.Width*header.Height*4*sizeof(s16));
  //      CColorConverter::convert8BitTo16Bit(bmpData, data, header.Width, header.Height, paletteData, pitch, true);
  //      image = new CImage(ECF_R8G8B8, dim);
		if (image)
		{


		   for (int x=0; x < header.Width; ++x)
		{
			for (int y=0; y < header.Height; ++y)
			{

                u16 original =X8R8G8B8toA1R5G5B5(paletteData[(u8)(*bmpData)]);

		        SColor color =  A1R5G5B5toA8R8G8B8(((u16*)bmpData)[y*header.Width + x]);



		        image->setPixel(x,y,color);

			}
        }

       }




      // s16* data = (s16 *)malloc(header.Width*header.Height*4*sizeof(s16));
      //  CColorConverter::convert8BitTo16Bit(bmpData, data, header.Width, header.Height, paletteData, pitch, true);

           u8 *resampled   = (u8*)malloc( header.Width* header.Height * 4 *sizeof(u8) );
           video::SColorf *PIXELS = core::LoadPixelsDataNormalized(bmpData,header.Width, header.Height,4);     // Supports 8 to 32 bit per channel

                    for (int i = 0, k = 0; i < header.Width* header.Height*4; i += 4, k++)
                    {
                        ((u8*)resampled)[i + 0] = (u8)(PIXELS[k].r*255.0f);
                        ((u8*)resampled)[i + 1] = (u8)(PIXELS[k].g*255.0f);
                        ((u8*)resampled)[i + 2] = (u8)(PIXELS[k].b*255.0f);
                        ((u8*)resampled)[i + 3] = (u8)(PIXELS[k].a*255.0f);
                    }

     // free(data);
      image = new CImage(ECF_A8R8G8B8, dim,(u8*)resampled);

*/



	    //core::ConvertColorFormat(bmpData, header.Width, header.Height, ECF_A1R5G5B5,ECF_R8G8B8);
	   // image = new CImage(ECF_A1R5G5B5, dim,data);
	    }

		//image = new CImage(ECF_A1R5G5B5, dim);
		//if (image)
		//	CColorConverter::convert8BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 16:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert16BitTo16Bit((s16*)bmpData, (s16*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	case 24:
		image = new CImage(ECF_R8G8B8, dim);
		if (image)
			CColorConverter::convert24BitTo24Bit(bmpData, (u8*)image->getData(), header.Width, header.Height, pitch, true, true);
		break;
	case 32: // thx to Reinhard Ostermeier
		image = new CImage(ECF_A8R8G8B8, dim);
		if (image)
			CColorConverter::convert32BitTo32Bit((s32*)bmpData, (s32*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	};

     os::Printer::log("LOAD BMP ",  file->getFileName(),ELL_INFORMATION);
	// clean up

	delete [] paletteData;
	delete [] bmpData;

	return image;
}



} // end namespace video
} // end namespace irr
