// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"
#include <SDL2/SDL_opengles2.h>
#include "irrList.h"

namespace irr
{
namespace video
{


//! OpenGL texture.
class COpenGLTexture : public ITexture
{
public:

	//! constructor
	COpenGLTexture()
	{};
	COpenGLTexture(const io::path& name);

	COpenGLTexture(ECOLOR_FORMAT format, int w, int h, void* data,const io::path name);
	COpenGLTexture(IImage* surface, const io::path& name);
	COpenGLTexture(const char* Filename, const io::path& name);

	//! destructor
	virtual ~COpenGLTexture();

	//! lock function
	virtual void* getPixels();




	//! Returns original size of the texture (image).
	virtual const core::dimension2d<u32>& getOriginalSize() const;

	//! Returns size of the texture.
	virtual const core::dimension2d<u32>& getSize() const;


	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;



	//! return open gl texture name
	GLuint getOpenGLTextureId() const;

	virtual GLuint getOpenGLTextureType() const
	{
	 return GL_TEXTURE_2D;
	}

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps() const;


	//! Is it a render target?
	virtual bool isRenderTarget() const;

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget(bool isTarget);

protected:




	//! get the desired color format based on texture creation flags and the input format.
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);



	//! get important numbers of the image and hw texture
	void getImageValues(IImage* image);

	//! copies the texture into an OpenGL texture.
	/** \param newTexture True if method is called for a newly created texture for the first time. Otherwise call with false to improve memory handling.
	\param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image.
	\param mipLevel If set to non-zero, only that specific miplevel is updated, using the MipImage member. */
	void uploadTexture(bool newTexture=false);

	core::dimension2d<u32> ImageSize;
	core::dimension2d<u32> TextureSize;
	ECOLOR_FORMAT ColorFormat;

	IImage* Image;


	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	bool IsRenderTarget;



	bool ReadOnlyLock;
	bool KeepImage;
};

//! OpenGL FBO texture.
class COpenGLFBOTexture : public COpenGLTexture
{
public:

	//! FrameBufferObject constructor
	COpenGLFBOTexture(const core::dimension2d<u32>& size, const io::path& name, ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! destructor
	virtual ~COpenGLFBOTexture();

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	ITexture* DepthTexture;
protected:
	GLuint ColorFrameBuffer;
};


//! OpenGL FBO depth texture.
class COpenGLFBODepthTexture : public COpenGLTexture
{
public:
	//! FrameBufferObject depth constructor
	COpenGLFBODepthTexture(const core::dimension2d<u32>& size, const io::path& name, bool useStencil=false);

	//! destructor
	virtual ~COpenGLFBODepthTexture();

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	bool attach(ITexture*);

protected:
	GLuint DepthRenderBuffer;
	GLuint StencilRenderBuffer;
	bool UseStencil;
};


//! OpenGL FBO depth texture.
class COpenGLCubeMapTexture : public COpenGLTexture
{
public:
	//! FrameBufferObject depth constructor
	COpenGLCubeMapTexture(const core::array<core::stringc> faces, const io::path& name);

	~COpenGLCubeMapTexture();

	virtual GLuint getOpenGLTextureType() const _IRR_OVERRIDE_
	{
	 return GL_TEXTURE_CUBE_MAP;
	}


protected:

};



} // end namespace video
} // end namespace irr

#endif


