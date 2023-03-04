// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"


#include "CImage.h"
#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "os.h"
#include "Engine.h"
#include "CColorConverter.h"
#include "irrString.h"
#include "stb_image.h"
#include "stb_image_write.h"

namespace irr
{
namespace video
{


void GetGlTextureFormats(int format, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType)
{
    *glInternalFormat = -1;
    *glFormat = -1;
    *glType = -1;

    switch (format)
    {
        case ECF_R8:   *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_UNSIGNED_BYTE; break;
        case ECF_R8G8: *glInternalFormat = GL_LUMINANCE_ALPHA; *glFormat = GL_LUMINANCE_ALPHA; *glType = GL_UNSIGNED_BYTE; break;
        case ECF_R5G6B5: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case ECF_R8G8B8: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case ECF_A1R5G5B5: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
//        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case ECF_A8R8G8B8: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case ECF_R32F:  *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_FLOAT; break;   // NOTE: Requires extension OES_texture_float
        case ECF_G32R32F:  *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_FLOAT; break;         // NOTE: Requires extension OES_texture_float
        case ECF_A32B32G32R32F:  *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;    // NOTE: Requires extension OES_texture_float

   /* #elif defined(GRAPHICS_API_OPENGL_33)
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_R8; *glFormat = GL_RED; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_RG8; *glFormat = GL_RG; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB565; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB8; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGB5_A1; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA4; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_R32F; *glFormat = GL_RED; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGB32F; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGBA32F; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
    #endif
*/
}
}

//! constructor for usual textures
COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name)
	: ITexture(name), ColorFormat(ECF_A8R8G8B8),  Image(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE),
	IsRenderTarget(false),
	ReadOnlyLock(false), KeepImage(true)
{

	setDebugName("COpenGLTexture");



	ColorFormat=origImage->getColorFormat();
	getImageValues(origImage);
	Image = new video::CImage(ColorFormat,TextureSize);

    SDL_Log("image %d   ",ColorFormat);


	glGenTextures(1, &TextureName);

	if (ImageSize==TextureSize)
	{

		origImage->copyTo(Image);
	}
	else
	{

		origImage->copyToScaling(Image);
	}

/*

   unsigned int  original_texture_format = 0;



	GLint pixelFormat = GL_RGBA;
    GLint  pixelType = GL_UNSIGNED_BYTE;

   // GLint internalformat = GL_RGBA;
  //  GLint type = GL_UNSIGNED_BYTE;


   int format =(int)ColorFormat;
*/



unsigned int glInternalFormat, glFormat, glType;
GetGlTextureFormats(ColorFormat, &glInternalFormat, &glFormat, &glType);

   int channels=0;

   if (Image->getColorFormat()==ECF_R8G8B8)
   {
     channels=3;
   } else
   if (Image->getColorFormat()==ECF_A8R8G8B8)
   {
     channels=4;
   }else
  if (Image->getColorFormat()==ECF_A1R5G5B5)
  //ECF_A1R5G5B5
   {
     channels=4;
     int format=(int)Image->getColorFormat();
     SDL_LogError(0,"Error: Unsupported texture color format (%d ).",format);
   }else
    if (Image->getColorFormat()==ECF_R5G6B5)
   {
     channels=3;
   }else
    if (Image->getColorFormat()==ECF_R8G8)
   {
     channels=2;

   }else
    if (Image->getColorFormat()==ECF_R8)
   {
     channels=1;
   }
    else
   {
      int format=(int)Image->getColorFormat();
      SDL_LogError(0,"Error: Unsupported texture color format (%d ).",format);
       return;

   }






	             // Generate texture id
	glBindTexture(GL_TEXTURE_2D, TextureName); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexImage2D(
		//GL_TEXTURE_2D, 0,		pixelFormat,  TextureSize.Width, TextureSize.Height, 0,		pixelFormat, pixelType, Image->getData() );

    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, TextureSize.Width, TextureSize.Height, 0, glFormat, glType, (unsigned char *)Image->getData() );


		//glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);


	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoset = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

	if (TextureName > 0) SDL_Log( "TEXTURE: [ID %i] Texture create successfully (%ix%i ) channels(%d) . \n", TextureName,  TextureSize.Width, TextureSize.Height,channels);

	if (!KeepImage)
	{

		Image->drop();
		Image=0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);


}
COpenGLTexture::COpenGLTexture(ECOLOR_FORMAT format,  int w, int h, void* data,const io::path name)
: ITexture(name), ColorFormat(format),  Image(0)
{
setDebugName("COpenGLTexture");



	ColorFormat=format;
	glGenTextures(1, &TextureName);


	TextureSize.set(w,h);
	ImageSize.set(w,h);



   unsigned int  original_texture_format = 0;





   int channels=0;

   if (ColorFormat==ECF_R8G8B8)
   {
     channels=3;
   } else
   if (ColorFormat==ECF_A8R8G8B8)
   {
     channels=4;
   } else
   {
   int format=(int)Image->getColorFormat();
   SDL_LogError(0,"Error: Unsupported texture color format (%d )for making normal map.",format);
   }




	switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			break;
		}



	             // Generate texture id
	glBindTexture(GL_TEXTURE_2D, TextureName); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		original_texture_format,  TextureSize.Width, TextureSize.Height, 0,
		original_texture_format, GL_UNSIGNED_BYTE, data );



	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoset = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

	if (TextureName > 0) SDL_Log( "TEXTURE: [ID %i] Texture create successfully (%ix%i ) channels(%d) . \n", TextureName,  TextureSize.Width, TextureSize.Height,channels);


	glBindTexture(GL_TEXTURE_2D, 0);
}
//! constructor for usual textures
COpenGLTexture::COpenGLTexture(const char* FileName, const io::path& name)
	: ITexture(name), ColorFormat(ECF_A8R8G8B8),  Image(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE),
	IsRenderTarget(false),
	ReadOnlyLock(false), KeepImage(false)
{

	setDebugName("COpenGLTexture");




unsigned int  original_texture_format = 0;
unsigned char* data;
unsigned int fileSize = 0;
unsigned char *fileData = irr::core::LoadFileData(FileName, &fileSize);
int Width,Height,channels;



if (fileData != NULL)
{
    stbi_set_flip_vertically_on_load(true);
    data= stbi_load_from_memory(fileData, fileSize, &Width,  &Height,  &channels,0);



if (data)
{


         SDL_Log( "IMAGE: Filename(%s) loaded successfully (%ix%i ) channels(%d) . \n",  FileName, Width, Height,channels);

        ImageSize.set(Width,Height);
        if ( !ImageSize.Width || !ImageSize.Height)
        {
            os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
            return;
        }
        TextureSize.set(Width,Height);






	bool CONVERT_TO_PO=true;

    if (CONVERT_TO_PO)
    {
        int new_width = 1;
        int new_height = 1;
        while( new_width < Width )
        {
            new_width *= 2;
        }
        while( new_height < Height )
        {
            new_height *= 2;
        }
        if( (new_width != Width) || (new_height != Height) )
        {
			unsigned char *resampled = (unsigned char*)malloc( channels*new_width*new_height );
			up_scale_image(data, Width, Height, channels, resampled, new_width, new_height );
			free( data );
			data = resampled;
			TextureSize.set(new_width,new_height);
		}
	}


}else{
	   SDL_Log("IMAGE: [%s] Failed to load image. \n", FileName);
	   return ;
}
}else{
	   SDL_Log("IMAGE: [%s] Failed to load data . \n", FileName);
       return ;
}

	switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			ColorFormat=ECF_R8G8B8;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			ColorFormat=ECF_A8R8G8B8;
			break;
		}




	glGenTextures(1, &TextureName);              // Generate texture id
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexImage2D(
		GL_TEXTURE_2D, 0,
		original_texture_format,  Width, Height, 0,
		original_texture_format, GL_UNSIGNED_BYTE, data );


	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoset = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

	stbi_image_free(data);
    free(fileData);




	if (TextureName > 0)
	{
	SDL_Log( "TEXTURE: [ID %i] Texture(%s) loaded successfully (%ix%i ) channels(%d) . \n", TextureName, FileName, Width, Height,channels);
	}
	else
	{
	SDL_Log( "TEXTURE: Failed to load texture(%s). \n",FileName);
	}





	glBindTexture(GL_TEXTURE_2D, 0);

}

//! constructor for basic setup (only for derived classes)
COpenGLTexture::COpenGLTexture(const io::path& name)
	: ITexture(name), ColorFormat(ECF_A8R8G8B8),  Image(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE),
	 IsRenderTarget(false),
	ReadOnlyLock(false), KeepImage(true)
{

	setDebugName("COpenGLTexture");


}

//! destructor
COpenGLTexture::~COpenGLTexture()
{


	if (TextureName)
	{
	 os::Printer::print(ELL_INFORMATION,"[TEXTURE]  Delete [%d] texture.",TextureName);
	 glDeleteTextures(1, &TextureName);
	}

	if (Image)
		Image->drop();
}






// prepare values ImageSize, TextureSize, and ColorFormat based on image
void COpenGLTexture::getImageValues(IImage* image)
{
	if (!image)
	{
		os::Printer::log("[TEXTURE] No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("[TEXTURE]Invalid size of image for OpenGL Texture.", ELL_ERROR);
		return;
	}

	const f32 ratio = (f32)ImageSize.Width/(f32)ImageSize.Height;

	TextureSize=ImageSize.getOptimalSize(true);


}

//! copies the the texture into an open gl texture.
void COpenGLTexture::uploadTexture(bool newTexture)
{
/*

	// check which image needs to be uploaded
	IImage* image = Image;
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
		return;
	}

	if (newTexture)
	{
	     glDeleteTextures(1, &TextureName);
		 glGenTextures(1, &TextureName);

	}




   unsigned int  original_texture_format = 0;


   int format =(int)Image->getColorFormat();


   int channels=0;

   if (Image->getColorFormat()==ECF_R8G8B8)
   {
     channels=3;
   } else
   if (Image->getColorFormat()==ECF_A8R8G8B8)
   {
     channels=4;
   }




	switch( channels )
		{
		case 1:
			original_texture_format = GL_LUMINANCE;
			break;
		case 2:
			original_texture_format = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			original_texture_format = GL_RGB;
			break;
		case 4:
			original_texture_format = GL_RGBA;
			break;
		}



	             // Generate texture id
	glBindTexture(GL_TEXTURE_2D, TextureName); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		original_texture_format,  TextureSize.Width, TextureSize.Height, 0,
		original_texture_format, GL_UNSIGNED_BYTE, Image->getData() );




	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoset = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

	if (TextureName > 0) SDL_Log( "TEXTURE: [ID %i] Texture update successfully (%ix%i ) channels(%d) . \n", TextureName,  TextureSize.Width, TextureSize.Height,channels);


    Image->Save("media/texture_update.png");
*/
}

ECOLOR_FORMAT COpenGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
{
	ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;

	return destFormat;
}

//! lock function
void* COpenGLTexture::getPixels()
{


return nullptr;
}




//! Returns size of the original image.
const core::dimension2d<u32>& COpenGLTexture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<u32>& COpenGLTexture::getSize() const
{
	return TextureSize;
}





//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	return ColorFormat;
}




//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureId() const
{
	return TextureName;
}


//! Returns whether this texture has mipmaps
bool COpenGLTexture::hasMipMaps() const
{
	return HasMipMaps;
}


bool COpenGLTexture::isRenderTarget() const
{
	return IsRenderTarget;
}


void COpenGLTexture::setIsRenderTarget(bool isTarget)
{
	IsRenderTarget = isTarget;
}


bool COpenGLTexture::isFrameBufferObject() const
{
	return false;
}


//! Bind Render Target Texture
void COpenGLTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLTexture::unbindRTT()
{
	//Driver->setActiveTexture(0, this);
	glBindTexture(GL_TEXTURE_2D, TextureName);

	// Copy Our ViewPort To The Texture
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
}


/* FBO Textures */

// helper function for render to texture
static bool checkFBOStatus();

//! RTT ColorFrameBuffer constructor
COpenGLFBOTexture::COpenGLFBOTexture(const core::dimension2d<u32>& size,
					const io::path& name,
					ECOLOR_FORMAT format)
	: COpenGLTexture(name), DepthTexture(0), ColorFrameBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_FBO");
	#endif

	ImageSize = size;
	TextureSize = size;

	//if (ECF_UNKNOWN == format)
	//	format = getBestColorFormat(driver->getColorFormat());

	ColorFormat = format;

	GLint FilteringType;

	HasMipMaps = false;
	IsRenderTarget = true;


	// generate frame buffer
	//Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &TextureName);
	//Driver->setActiveTexture(0, this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
		ImageSize.Height, 0, PixelFormat, PixelType, 0);
#ifdef _DEBUG
	//driver->testGLError();
#endif

	// attach color texture to frame buffer
	/*Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT,
						GL_TEXTURE_2D,
						TextureName,
						0);*/
#ifdef _DEBUG
	checkFBOStatus();
#endif


	unbindRTT();
}


//! destructor
COpenGLFBOTexture::~COpenGLFBOTexture()
{
//	if (DepthTexture)
	//	if (DepthTexture->drop())
		//	Driver->removeDepthTexture(DepthTexture);
	//if (ColorFrameBuffer)
	//	Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
}


bool COpenGLFBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COpenGLFBOTexture::bindRTT()
{
	//if (ColorFrameBuffer != 0)
		//Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
//	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

}


//! Unbind Render Target Texture
void COpenGLFBOTexture::unbindRTT()
{
	//if (ColorFrameBuffer != 0)
		//Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COpenGLFBODepthTexture::COpenGLFBODepthTexture(
		const core::dimension2d<u32>& size,
		const io::path& name,

		bool useStencil)
	: COpenGLTexture(name), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{
#ifdef _DEBUG
	setDebugName("COpenGLTextureFBO_Depth");
#endif
/*
	ImageSize = size;
	TextureSize = size;
	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

	if (useStencil)
	{
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

			// generate stencil texture
			glGenTextures(1, &StencilRenderBuffer);
			glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
				ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);

	}
	else
	{
		// generate depth buffer
		/*Driver->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, DepthRenderBuffer);
		Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT,
				Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height);
	}
	*/

}


//! destructor
COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
{
/*	if (DepthRenderBuffer && UseStencil)
		glDeleteTextures(1, &DepthRenderBuffer);
	else
		Driver->extGlDeleteRenderbuffers(1, &DepthRenderBuffer);
	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteTextures(1, &StencilRenderBuffer);*/
}


//combine depth texture and rtt
bool COpenGLFBODepthTexture::attach(ITexture* renderTex)
{
/*
	if (!renderTex)
		return false;
	video::COpenGLFBOTexture* rtt = static_cast<video::COpenGLFBOTexture*>(renderTex);
	rtt->bindRTT();
#ifdef GL_EXT_framebuffer_object
	if (UseStencil)
	{
		// attach stencil texture to stencil buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_STENCIL_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						StencilRenderBuffer,
						0);

		// attach depth texture to depth buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						DepthRenderBuffer,
						0);
	}
	else
	{
		// attach depth renderbuffer to depth buffer
		Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_RENDERBUFFER_EXT,
						DepthRenderBuffer);
	}
#endif
	// check the status
	if (!checkFBOStatus(Driver))
	{
		os::Printer::log("FBO incomplete");
		return false;
	}
	rtt->DepthTexture=this;
	grab(); // grab the depth buffer, not the RTT
	rtt->unbindRTT();*/
	return true;
}


//! Bind Render Target Texture
void COpenGLFBODepthTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLFBODepthTexture::unbindRTT()
{
}


bool checkFBOStatus()
{
/*	GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

#ifdef GL_EXT_framebuffer_multisample
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
#endif
	os::Printer::log("FBO error", ELL_ERROR);
//	_IRR_DEBUG_BREAK_IF(true);*/
	return false;
}

COpenGLCubeMapTexture::COpenGLCubeMapTexture(const core::array<core::stringc> faces, const io::path& name)
{

    scene::Engine* engine = scene::Engine::Instance();

    GLint bound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);



    IImage* images[6];
    glGenTextures(1, &TextureName);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureName);




    for (unsigned int i = 0; i < faces.size(); i++)
    {




        images[i] = engine->createImageFromFile(faces[i].c_str());
        if (images[i])
        {


           int width  =images[i]->getDimension().Width;
           int height =images[i]->getDimension().Height;


            ImageSize.set(width,height);
	        TextureSize.set(width,height);
	        ColorFormat=images[i]->getColorFormat();


            GLint internalformat;
            GLenum format;
           if (images[i]->getColorFormat()==ECF_R8G8B8)
           {
           internalformat =GL_RGB;
           format         =GL_RGB;
           } else{
           internalformat =GL_RGBA;
           format         =GL_RGBA;
           }


           //GL_APICALL void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);

          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, images[i]->getData());

        }
        else
        {
            SDL_Log("Cubemap texture failed to load at path: %s" , faces[i]);

        }

    }

    if (TextureName > 0) SDL_Log( "CUBETEXTURE: [ID %i] Texture create successfully (%ix%i ). \n", TextureName,  TextureSize.Width, TextureSize.Height);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bound);

      for (unsigned int i = 0; i < faces.size(); i++)
    {
          if(images[i]) images[i]->drop();
    }

}
COpenGLCubeMapTexture::~COpenGLCubeMapTexture()
{
if (TextureName)
	{
	 os::Printer::print(ELL_INFORMATION,"[CUBETEXTURE]  Delete [%d] texture.",TextureName);
	 glDeleteTextures(1, &TextureName);
	}
}

} // end namespace video
} // end namespace irr



