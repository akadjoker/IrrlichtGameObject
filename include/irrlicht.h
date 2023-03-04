/* irrlicht.h -- interface of the 'Irrlicht Engine'

  Copyright (C) 2002-2012 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Please note that the Irrlicht Engine is based in part on the work of the
  Independent JPEG Group, the zlib and the libPng. This means that if you use
  the Irrlicht Engine in your product, you must acknowledge somewhere in your
  documentation that you've used the IJG code. It would also be nice to mention
  that you use the Irrlicht Engine, the zlib and libPng. See the README files
  in the jpeglib, the zlib and libPng for further information.
*/

#ifndef __IRRLICHT_H_INCLUDED__
#define __IRRLICHT_H_INCLUDED__

#include "stb_image.h"
#include "stb_image_write.h"

#include "ITransform.h"
#include "IGameObject.h"
#include "IGameScene.h"


#include "CImage.h"
#include "COpenGLTexture.h"
#include "CGeometryCreator.h"

#include "IMeshAnimator.h"
#include "CMeshAnimator.h"

#include "IShadowMesh.h"
#include "CShadowMesh.h"
#include "os.h"
#include "IrrCompileConfig.h"
#include "aabbox3d.h"
#include "CMeshBuffer.h"
#include "coreutil.h"


#include "dimension2d.h"
#include "IEngineEnums.h"
#include "CShader.h"

#include "fast_atof.h"
#include "heapsort.h"
#include "IAnimatedMesh.h"
#include "IFileList.h"
#include "IFileSystem.h"
#include "CFileSystem.h"
#include "IGeometryCreator.h"
#include "IImage.h"
#include "IIndexBuffer.h"
#include "ILogger.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "IMeshManipulator.h"
#include "IMeshWriter.h"

#include "IReadFile.h"
#include "IReferenceCounted.h"
#include "irrArray.h"
#include "IRandomizer.h"
#include "irrList.h"
#include "irrMap.h"
#include "irrMath.h"
#include "irrString.h"
#include "irrTypes.h"
#include "path.h"
#include "irrXML.h"
#include "ISkinnedMesh.h"
#include "ITexture.h"
#include "ITimer.h"
#include "IVertexBuffer.h"
#include "IWriteFile.h"
#include "IXMLReader.h"
#include "IXMLWriter.h"
#include "line2d.h"
#include "line3d.h"
#include "matrix4.h"
#include "plane3d.h"
#include "position2d.h"
#include "quaternion.h"
#include "rect.h"
#include "S3DVertex.h"
#include "SAnimatedMesh.h"
#include "SColor.h"
#include "SMaterial.h"
#include "SMesh.h"
#include "SSharedMeshBuffer.h"
#include "SSkinMeshBuffer.h"
#include "SViewFrustum.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "CWriteFile.h"
#include "Engine.h"


#define CLITERAL(type)      type

#define LIGHTGRAY  CLITERAL(SColor){255, 200, 200, 200 }   // Light Gray
#define GRAY       CLITERAL(SColor){255, 130, 130, 130 }   // Gray
#define DARKGRAY   CLITERAL(SColor){255, 80, 80, 80 }      // Dark Gray
#define YELLOW     CLITERAL(SColor){255, 253, 249, 0 }     // Yellow
#define GOLD       CLITERAL(SColor){255, 255, 203, 0 }     // Gold
#define ORANGE     CLITERAL(SColor){255, 255, 161, 0 }     // Orange
#define PINK       CLITERAL(SColor){255, 255, 109, 194 }   // Pink
#define RED        CLITERAL(SColor){255, 230, 41, 55 }     // Red
#define MAROON     CLITERAL(SColor){255, 190, 33, 55 }     // Maroon
#define GREEN      CLITERAL(SColor){255, 0, 228, 48 }      // Green
#define LIME       CLITERAL(SColor){255, 0, 158, 47 }      // Lime
#define DARKGREEN  CLITERAL(SColor){255, 0, 117, 44 }      // Dark Green
#define SKYBLUE    CLITERAL(SColor){255, 102, 191, 255 }   // Sky Blue
#define BLUE       CLITERAL(SColor){255, 0, 121, 241 }     // Blue
#define DARKBLUE   CLITERAL(SColor){255, 0, 82, 172 }      // Dark Blue
#define PURPLE     CLITERAL(SColor){255, 200, 122, 255 }   // Purple
#define VIOLET     CLITERAL(SColor){255, 135, 60, 190 }    // Violet
#define DARKPURPLE CLITERAL(SColor){255, 112, 31, 126 }    // Dark Purple
#define BEIGE      CLITERAL(SColor){255, 211, 176, 131 }   // Beige
#define BROWN      CLITERAL(SColor){255, 127, 106, 79 }    // Brown
#define DARKBROWN  CLITERAL(SColor){255, 76, 63, 47 }      // Dark Brown

#define WHITE      CLITERAL(SColor){255, 255, 255, 255 }   // White
#define BLACK      CLITERAL(SColor){255, 0, 0, 0 }         // Black
#define BLANK      CLITERAL(SColor){255, 0, 0, 0, }           // Blank (Transparent)
#define MAGENTA    CLITERAL(SColor){255, 255, 0, 255 }     // Magenta
#define RAYWHITE   CLITERAL(SColor){255, 245, 245, 245 }   // My own White (raylib logo)



/*! \mainpage Irrlicht Engine 1.9 API documentation
 *
 * <div align="center"><img src="logobig.png" ></div>
 *
 * \section intro Introduction
 *
 * Welcome to the Irrlicht Engine API documentation.
 * Here you'll find any information you'll need to develop applications with
 * the Irrlicht Engine. If you are looking for a tutorial on how to start, you'll
 * find some on the homepage of the Irrlicht Engine at
 * <A HREF="http://irrlicht.sourceforge.net" >irrlicht.sourceforge.net</A>
 * or inside the SDK in the examples directory.
 *
 * The Irrlicht Engine is intended to be an easy-to-use 3d engine, so
 * this documentation is an important part of it. If you have any questions or
 * suggestions, just send a email to the author of the engine, Nikolaus Gebhardt
 * (niko (at) irrlicht3d.org).
 *
 *

 *
 * Irrlicht can load a lot of file formats automatically, see irr::scene::ISceneManager::getMesh()
 * for a detailed list. So if you would like to replace the simple blue screen background by
 * a cool Quake 3 Map, optimized by an octree, just insert this code
 * somewhere before the while loop:
 *
 * \code
 *	// add .pk3 archive to the file system
 *	device->getFileSystem()->addZipFileArchive("quake3map.pk3");
 *
 *	// load .bsp file and show it using an octree
 *	scenemgr->addOctreeSceneNode(
 *		scenemgr->getMesh("quake3map.bsp"));
 * \endcode
 *
 * As you can see, the engine uses namespaces. Everything in the engine is
 * placed into the namespace 'irr', but there are also 5 sub namespaces.
 * You can find a list of all namespaces with descriptions at the
 * <A HREF="namespaces.html"> namespaces page</A>.
 * This is also a good place to start reading the documentation. If you
 * don't want to write the namespace names all the time, just use all namespaces like
 * this:
 * \code
 * using namespace core;
 * using namespace scene;
 * using namespace video;
 * using namespace io;
 * using namespace gui;
 * \endcode
 *
 * There is a lot more the engine can do, but I hope this gave a short
 * overview over the basic features of the engine. For more examples, please take
 * a look into the examples directory of the SDK.
 */



//! Everything in the Irrlicht Engine can be found in this namespace.
namespace irr
{

	//! Basic classes such as vectors, planes, arrays, lists, and so on can be found in this namespace.
	namespace core
	{
	}

	//! The gui namespace contains useful classes for easy creation of a graphical user interface.
	namespace gui
	{
	}

	//! This namespace provides interfaces for input/output: Reading and writing files, accessing zip archives, xml files, ...
	namespace io
	{

	}

	//! All scene management can be found in this namespace: Mesh loading, special scene nodes like octrees and billboards, ...
	namespace scene
	{


	}

	//! The video namespace contains classes for accessing the video driver. All 2d and 3d rendering is done here.
	namespace video
	{


	}
}

/*! \file irrlicht.h
	\brief Main header file of the irrlicht, the only file needed to include.
*/

#endif

