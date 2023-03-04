// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_TEXTURE_H_INCLUDED__
#define __I_TEXTURE_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IImage.h"
#include "dimension2d.h"

#include "path.h"
#include "matrix4.h"

namespace irr
{
namespace video
{


//! Enumeration flags used to tell the video driver with setTextureCreationFlag in which format textures should be created.
enum E_TEXTURE_CREATION_FLAG
{
	/** Forces the driver to create 16 bit textures always, independent of
	which format the file on disk has. When choosing this you may lose
	some color detail, but gain much speed and memory. 16 bit textures can
	be transferred twice as fast as 32 bit textures and only use half of
	the space in memory.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	ETCF_OPTIMIZED_FOR_SPEED at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_ALWAYS_16_BIT = 0x00000001,

	/** Forces the driver to create 32 bit textures always, independent of
	which format the file on disk has. Please note that some drivers (like
	the software device) will ignore this, because they are only able to
	create and use 16 bit textures.
	Default is true.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	ETCF_OPTIMIZED_FOR_SPEED at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_ALWAYS_32_BIT = 0x00000002,

	/** Lets the driver decide in which format the textures are created and
	tries to make the textures look as good as possible. Usually it simply
	chooses the format in which the texture was stored on disk.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_SPEED at
	the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_OPTIMIZED_FOR_QUALITY = 0x00000004,

	/** Lets the driver decide in which format the textures are created and
	tries to create them maximizing render speed.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_QUALITY,
	at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_OPTIMIZED_FOR_SPEED = 0x00000008,

	/** Creates textures with mipmap levels.
	If disabled textures can not have mipmaps.
	Default is true. */
	ETCF_CREATE_MIP_MAPS = 0x00000010,

	/** Discard any alpha layer and use non-alpha color format.
	Warning: This may lead to getting 24-bit texture formats which
	         are often badly supported by drivers. So it's generally
			 not recommended to enable this flag.	*/
	ETCF_NO_ALPHA_CHANNEL = 0x00000020,

	//! Allow the Driver to use Non-Power-2-Textures
	/** BurningVideo can handle Non-Power-2 Textures in 2D (GUI), but not in 3D. */
	ETCF_ALLOW_NON_POWER_2 = 0x00000040,

	//! Allow the driver to keep a copy of the texture in memory
	/** Enabling this makes calls to ITexture::lock a lot faster, but costs main memory.
	Currently only used in combination with OpenGL drivers.
	NOTE: Disabling this does not yet work correctly with alpha-textures.
	So the default is on for now (but might change with Irrlicht 1.9 if we get the alpha-troubles fixed).
	*/
	ETCF_ALLOW_MEMORY_COPY = 0x00000080,

	//! Enable automatic updating mip maps when the base texture changes.
	/** Default is true.
	This flag is only used when ETCF_CREATE_MIP_MAPS is also enabled and if the driver supports it.
	Please note:
	- On D3D (and maybe older OGL?) you can no longer manually set mipmap data when enabled
	 (for example mips from image loading will be ignored).
	- On D3D (and maybe older OGL?) texture locking for mipmap levels usually won't work anymore.
	- On new OGL this flag is ignored.
	- When disabled you do _not_ get hardware mipmaps on D3D, so mipmap generation can be slower.
	- When disabled you can still update your mipmaps when the texture changed by manually calling regenerateMipMapLevels.
	- You can still call regenerateMipMapLevels when this flag is enabled (it will be a hint on d3d to update mips immediately)
	  */
	ETCF_AUTO_GENERATE_MIP_MAPS = 0x00000100,

	/** This flag is never used, it only forces the compiler to compile
	these enumeration values to 32 bit. */
	ETCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//! Enum for the mode for texture locking. Read-Only, write-only or read/write.
enum E_TEXTURE_LOCK_MODE
{
	//! The default mode. Texture can be read and written to.
	ETLM_READ_WRITE = 0,

	//! Read only. The texture is downloaded, but not uploaded again.
	/** Often used to read back shader generated textures. */
	ETLM_READ_ONLY,

	//! Write only. The texture is not downloaded and might be uninitialized.
	/** The updated texture is uploaded to the GPU.
	Used for initializing the shader from the CPU. */
	ETLM_WRITE_ONLY
};

//! Additional bitflags for ITexture::lock() call
enum E_TEXTURE_LOCK_FLAGS
{
	ETLF_NONE = 0,

	//! Flip left-bottom origin rendertarget textures upside-down
	/** Irrlicht usually has all textures with left-top as origin.
	And for drivers with a left-bottom origin coordinate system (OpenGL)
	Irrlicht modifies the texture-matrix in the fixed function pipeline to make
	the textures show up correctly (shader coders have to handle upside down
	textures themselves).
	But rendertarget textures (RTT's) are written by drivers the way the
	coordinate system of that driver works. So on OpenGL images tend to look
	upside down (aka Y coordinate going up) on lock() when this flag isn't set.
	When the flag is set it will flip such textures on lock() to make them look
	like non-rtt textures (origin left-top). Note that this also means the texture
	will be uploaded flipped on unlock. So mostly you want to have this flag set
	when you want to look at the texture or save it, but unset if you want to
	upload it again to the card.
	If you disable this flag you get the memory just as it is on the graphic card.
	For backward compatibility reasons this flag is enabled by default. */
	ETLF_FLIP_Y_UP_RTT = 1
};





//! Interface of a Video Driver dependent Texture.
/** An ITexture is created by an IVideoDriver by using IVideoDriver::addTexture
or IVideoDriver::getTexture. After that, the texture may only be used by this
VideoDriver. As you can imagine, textures of the DirectX and the OpenGL device
will, e.g., not be compatible. An exception is the Software device and the
NULL device, their textures are compatible. If you try to use a texture
created by one device with an other device, the device will refuse to do that
and write a warning or an error message to the output buffer.
*/
class ITexture : public virtual IReferenceCounted
{
public:

	//! constructor
	ITexture()
	: NamedPath(""),OriginalColorFormat(ECF_UNKNOWN),ColorFormat(ECF_UNKNOWN),  HasMipMaps(false), IsRenderTarget(false)
	{
	}
	ITexture(const io::path& name)
	: NamedPath(name),OriginalColorFormat(ECF_UNKNOWN),
		ColorFormat(ECF_UNKNOWN),  HasMipMaps(false), IsRenderTarget(false)
	{
	}
	ITexture(const char* Filename,const io::path& name) : NamedPath(name),OriginalColorFormat(ECF_UNKNOWN),
		ColorFormat(ECF_UNKNOWN),  HasMipMaps(false), IsRenderTarget(false)
	{
	}


	virtual void* getPixels() = 0;


	virtual void uploadTexture(bool newTexture)=0;



	//! Get original size of the texture.
	/** The texture is usually scaled, if it was created with an unoptimal
	size. For example if the size was not a power of two. This method
	returns the size of the texture it had before it was scaled. Can be
	useful when drawing 2d images on the screen, which should have the
	exact size of the original texture. Use ITexture::getSize() if you want
	to know the real size it has now stored in the system.
	\return The original size of the texture. */
	const core::dimension2d<u32>& getOriginalSize() const { return OriginalSize; };

	//! Get dimension (=size) of the texture.
	/** \return The size of the texture. */
	const core::dimension2d<u32>& getSize() const { return Size; };



	//! Get the color format of texture.
	/** \return The color format of texture. */
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; };


	//! Get the original color format
	/** When create textures from image data we will often use different color formats.
	For example depending on driver TextureCreationFlag's.
	This can give you the original format which the image used to create the texture had	*/
	ECOLOR_FORMAT getOriginalColorFormat() const { return OriginalColorFormat; };



	//! Check whether the texture has MipMaps
	/** \return True if texture has MipMaps, else false. */
	bool hasMipMaps() const { return HasMipMaps; }

	//! Check whether the texture is a render target
	/** Render targets can be set as such in the video driver, in order to
	render a scene into the texture. Once unbound as render target, they can
	be used just as usual textures again.
	\return True if this is a render target, otherwise false. */
	bool isRenderTarget() const { return IsRenderTarget; }

	//! Get name of texture (in most cases this is the filename)
	const io::SNamedPath& getName() const { return NamedPath; }


	//! Returns if the texture has an alpha channel
	bool hasAlpha() const
	{
		bool status = false;

		switch (ColorFormat)
		{
		case ECF_A8R8G8B8:
		case ECF_A1R5G5B5:
		case ECF_A16B16G16R16F:
		case ECF_A32B32G32R32F:
			status = true;
			break;
		default:
			break;
		}

		return status;
	}



protected:


	io::SNamedPath NamedPath;
	core::dimension2d<u32> OriginalSize;
	core::dimension2d<u32> Size;
	ECOLOR_FORMAT OriginalColorFormat;
	ECOLOR_FORMAT ColorFormat;

	bool HasMipMaps;
	bool IsRenderTarget;
};


} // end namespace video
} // end namespace irr

#endif

