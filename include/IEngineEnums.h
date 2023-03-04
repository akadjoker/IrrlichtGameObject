// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_MESH_WRITER_ENUMS_H_INCLUDED__
#define __E_MESH_WRITER_ENUMS_H_INCLUDED__

#include "irrTypes.h"

//! Everything in the Irrlicht Engine can be found in this namespace.
namespace irr
{
//! enumeration for key actions. Used for example in the FPS Camera.
	enum EKEY_ACTION
	{
		EKA_MOVE_FORWARD = 0,
		EKA_MOVE_BACKWARD,
		EKA_STRAFE_LEFT,
		EKA_STRAFE_RIGHT,
		EKA_JUMP_UP,
		EKA_CROUCH,
		EKA_ROTATE_LEFT,
		EKA_ROTATE_RIGHT,
		EKA_COUNT,

		//! This value is not used. It only forces this enumeration to compile in 32 bit.
		EKA_FORCE_32BIT = 0x7fffffff
	};

	//! Struct storing which key belongs to which action.
	struct SKeyMap
	{
		SKeyMap() {}
		SKeyMap(EKEY_ACTION action, int keyCode) : Action(action), KeyCode(keyCode) {}

		EKEY_ACTION Action;
		int KeyCode;
	};


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

	//! An enumeration for different class types implementing IReadFile
	enum EREAD_FILE_TYPE
	{
		//! CReadFile
		ERFT_READ_FILE  = MAKE_IRR_ID('r','e','a','d'),

		//! CMemoryReadFile
		ERFT_MEMORY_READ_FILE = MAKE_IRR_ID('r','m','e','m'),

		//! CLimitReadFile
		ERFT_LIMIT_READ_FILE = MAKE_IRR_ID('r','l','i','m'),

		//! Unknown type
		EFIT_UNKNOWN        = MAKE_IRR_ID('u','n','k','n')
	};
	}

	//! All scene management can be found in this namespace: Mesh loading, special scene nodes like octrees and billboards, ...
	namespace scene
	{

		enum E_INTERPOLATION_MODE
	{
		// constant does use the current key-values without interpolation
		EIM_CONSTANT = 0,

		// linear interpolation
		EIM_LINEAR,

		//! count of all available interpolation modes
		EIM_COUNT
	};


	//! Enumeration for all primitive types there are.
	enum E_PRIMITIVE_TYPE
	{
		//! All vertices are non-connected points.
		EPT_POINTS=0,

		//! All vertices form a single connected line.
		EPT_LINE_STRIP,

		//! Just as LINE_STRIP, but the last and the first vertex is also connected.
		EPT_LINE_LOOP,

		//! Every two vertices are connected creating n/2 lines.
		EPT_LINES,

		//! After the first two vertices each vertex defines a new triangle.
		//! Always the two last and the new one form a new triangle.
		EPT_TRIANGLE_STRIP,

		//! After the first two vertices each vertex defines a new triangle.
		//! All around the common first vertex.
		EPT_TRIANGLE_FAN,

		//! Explicitly set all vertices for each triangle.
		EPT_TRIANGLES
	};
/*A scene mesh writers is represented by a four character code
	such as 'irrm' or 'coll' instead of simple numbers, to avoid
	name clashes with external mesh writers.*/
	enum EMESH_WRITER_TYPE
	{
		//! Irrlicht native mesh writer, for static .irrmesh files.
		EMWT_IRR_MESH     = MAKE_IRR_ID('i','r','r','m'),

		//! COLLADA mesh writer for .dae and .xml files
		EMWT_COLLADA      = MAKE_IRR_ID('c','o','l','l'),

		//! STL mesh writer for .stl files
		EMWT_STL          = MAKE_IRR_ID('s','t','l',0),

		//! OBJ mesh writer for .obj files
		EMWT_OBJ          = MAKE_IRR_ID('o','b','j',0),

		//! PLY mesh writer for .ply files
		EMWT_PLY          = MAKE_IRR_ID('p','l','y',0),

		//! B3D mesh writer, for static .b3d files
		EMWT_B3D          = MAKE_IRR_ID('b', '3', 'd', 0)
	};


	//! flags configuring mesh writing
	enum E_MESH_WRITER_FLAGS
	{
		//! no writer flags
		EMWF_NONE = 0,

		//! write lightmap textures out if possible
		//! Currently not used by any Irrlicht mesh-writer
		// (Note: User meshwriters can still use it)
		EMWF_WRITE_LIGHTMAPS = 0x1,

		//! write in a way that consumes less disk space
		// (Note: Mainly there for user meshwriters)
		EMWF_WRITE_COMPRESSED = 0x2,

		//! write in binary format rather than text
		EMWF_WRITE_BINARY = 0x4
	};
	enum E_HARDWARE_MAPPING
	{
		//! Don't store on the hardware
		EHM_NEVER=0,

		//! Rarely changed, usually stored completely on the hardware
		EHM_STATIC,

		//! Sometimes changed, driver optimized placement
		EHM_DYNAMIC,

		//! Always changed, cache optimizing on the GPU
		EHM_STREAM
	};

	enum E_BUFFER_TYPE
	{
		//! Does not change anything
		EBT_NONE=0,
		//! Change the vertex mapping
		EBT_VERTEX,
		//! Change the index mapping
		EBT_INDEX,
		//! Change both vertex and index mapping to the same value
		EBT_VERTEX_AND_INDEX
	};

//! An enumeration for all types of debug data for built-in scene nodes (flags)
	enum E_DEBUG_SCENE_TYPE
	{
		//! No Debug Data ( Default )
		EDS_OFF = 0,

		//! Show Bounding Boxes of SceneNode
		EDS_BBOX = 1,

		//! Show Vertex Normals
		EDS_NORMALS = 2,

		//! Shows Skeleton/Tags
		EDS_SKELETON = 4,

		//! Overlays Mesh Wireframe
		EDS_MESH_WIRE_OVERLAY = 8,

		//! Temporary use transparency Material Type
		EDS_HALF_TRANSPARENCY = 16,

		//! Show Bounding Boxes of all MeshBuffers
		EDS_BBOX_BUFFERS = 32,

		//! EDS_BBOX | EDS_BBOX_BUFFERS
		EDS_BBOX_ALL = EDS_BBOX | EDS_BBOX_BUFFERS,

		//! Show all debug infos
		EDS_FULL = 0xffffffff
	};
	//! An enumeration for all types of automatic culling for built-in scene nodes
	enum E_CULLING_TYPE
	{
		EAC_OFF = 0,
		EAC_BOX = 1,
		EAC_FRUSTUM_BOX = 2,
		EAC_FRUSTUM_SPHERE = 4,
		EAC_OCC_QUERY = 8
	};

	//! Names for culling type
	const c8* const AutomaticCullingNames[] =
	{
		"false",
		"box",			// camera box against node box
		"frustum_box",		// camera frustum against node box
		"frustum_sphere",	// camera frustum against node sphere
		"occ_query",		// occlusion query
		0
	};


	//! An enumeration for all types of built-in scene node animators
	enum ESCENE_NODE_ANIMATOR_TYPE
	{
		//! Fly circle scene node animator
		ESNAT_FLY_CIRCLE = 0,

		//! Fly straight scene node animator
		ESNAT_FLY_STRAIGHT,

		//! Follow spline scene node animator
		ESNAT_FOLLOW_SPLINE,

		//! Rotation scene node animator
		ESNAT_ROTATION,

		//! Texture scene node animator
		ESNAT_TEXTURE,

		//! Deletion scene node animator
		ESNAT_DELETION,

		//! Collision response scene node animator
		ESNAT_COLLISION_RESPONSE,

		//! FPS camera animator
		ESNAT_CAMERA_FPS,

		//! Maya camera animator
		ESNAT_CAMERA_MAYA,

		//! Amount of built-in scene node animators
		ESNAT_COUNT,

		//! Unknown scene node animator
		ESNAT_UNKNOWN,

		//! This enum is never used, it only forces the compiler to compile this enumeration to 32 bit.
		ESNAT_FORCE_32_BIT = 0x7fffffff
	};
		//! An enumeration for all types of built-in scene nodes
	/** A scene node type is represented by a four character code
	such as 'cube' or 'mesh' instead of simple numbers, to avoid
	name clashes with external scene nodes.*/
	enum ESCENE_NODE_TYPE
	{
		//! of type CSceneManager (note that ISceneManager is not(!) an ISceneNode)
		ESNT_SCENE_MANAGER  = MAKE_IRR_ID('s','m','n','g'),

		//! simple cube scene node
		ESNT_CUBE           = MAKE_IRR_ID('c','u','b','e'),

		//! Sphere scene node
		ESNT_SPHERE         = MAKE_IRR_ID('s','p','h','r'),

		//! Text Scene Node
		ESNT_TEXT           = MAKE_IRR_ID('t','e','x','t'),

		//! Billboard text scene node
		ESNT_BILLBOARD_TEXT = MAKE_IRR_ID('b','t','x','t'),

		//! Water Surface Scene Node
		ESNT_WATER_SURFACE  = MAKE_IRR_ID('w','a','t','r'),

		//! Terrain Scene Node
		ESNT_TERRAIN        = MAKE_IRR_ID('t','e','r','r'),

		//! Sky Box Scene Node
		ESNT_SKY_BOX        = MAKE_IRR_ID('s','k','y','_'),

		//! Sky Dome Scene Node
		ESNT_SKY_DOME       = MAKE_IRR_ID('s','k','y','d'),

		//! Shadow Volume Scene Node
		ESNT_SHADOW_VOLUME  = MAKE_IRR_ID('s','h','d','w'),

		//! Octree Scene Node
		ESNT_OCTREE         = MAKE_IRR_ID('o','c','t','r'),

		//! Mesh Scene Node
		ESNT_MESH           = MAKE_IRR_ID('m','e','s','h'),

		//! Light Scene Node
		ESNT_LIGHT          = MAKE_IRR_ID('l','g','h','t'),

		//! Empty Scene Node
		ESNT_EMPTY          = MAKE_IRR_ID('e','m','t','y'),

		//! Dummy Transformation Scene Node
		ESNT_DUMMY_TRANSFORMATION = MAKE_IRR_ID('d','m','m','y'),

		//! Camera Scene Node
		ESNT_CAMERA         = MAKE_IRR_ID('c','a','m','_'),

		//! Billboard Scene Node
		ESNT_BILLBOARD      = MAKE_IRR_ID('b','i','l','l'),

		//! Animated Mesh Scene Node
		ESNT_ANIMATED_MESH  = MAKE_IRR_ID('a','m','s','h'),

		//! Particle System Scene Node
		ESNT_PARTICLE_SYSTEM = MAKE_IRR_ID('p','t','c','l'),

		//! Quake3 Shader Scene Node
		ESNT_Q3SHADER_SCENE_NODE  = MAKE_IRR_ID('q','3','s','h'),

		//! Quake3 Model Scene Node ( has tag to link to )
		ESNT_MD3_SCENE_NODE  = MAKE_IRR_ID('m','d','3','_'),

		//! Volume Light Scene Node
		ESNT_VOLUME_LIGHT  = MAKE_IRR_ID('v','o','l','l'),

		//! Maya Camera Scene Node
		/** Legacy, for loading version <= 1.4.x .irr files */
		ESNT_CAMERA_MAYA    = MAKE_IRR_ID('c','a','m','M'),

		//! First Person Shooter Camera
		/** Legacy, for loading version <= 1.4.x .irr files */
		ESNT_CAMERA_FPS     = MAKE_IRR_ID('c','a','m','F'),

		//! Unknown scene node
		ESNT_UNKNOWN        = MAKE_IRR_ID('u','n','k','n'),

		//! Will match with any scene node when checking types
		ESNT_ANY            = MAKE_IRR_ID('a','n','y','_')
	};
    //! enumeration for patch sizes specifying the size of patches in the TerrainSceneNode
	enum E_TERRAIN_PATCH_SIZE
	{
		//! patch size of 9, at most, use 4 levels of detail with this patch size.
		ETPS_9 = 9,

		//! patch size of 17, at most, use 5 levels of detail with this patch size.
		ETPS_17 = 17,

		//! patch size of 33, at most, use 6 levels of detail with this patch size.
		ETPS_33 = 33,

		//! patch size of 65, at most, use 7 levels of detail with this patch size.
		ETPS_65 = 65,

		//! patch size of 129, at most, use 8 levels of detail with this patch size.
		ETPS_129 = 129
	};

	//! Name of the parameter for changing how Irrlicht handles the ZWrite flag for transparent (blending) materials
	/** The default behavior in Irrlicht is to disable writing to the
	z-buffer for all really transparent, i.e. blending materials. This
	avoids problems with intersecting faces, but can also break renderings.
	If transparent materials should use the SMaterial flag for ZWriteEnable
	just as other material types use this attribute.
	Use it like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
	\endcode
	**/
	const c8* const ALLOW_ZWRITE_ON_TRANSPARENT = "Allow_ZWrite_On_Transparent";

	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** Was used for changing the texture path of the built-in csm loader like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::CSM_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const CSM_TEXTURE_PATH = "CSM_TexturePath";

	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** Was used for changing the texture path of the built-in lmts loader like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::LMTS_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const LMTS_TEXTURE_PATH = "LMTS_TexturePath";

	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** Was used for changing the texture path of the built-in MY3D loader like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::MY3D_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const MY3D_TEXTURE_PATH = "MY3D_TexturePath";

	//! Name of the parameter specifying the COLLADA mesh loading mode
	/**
	Specifies if the COLLADA loader should create instances of the models, lights and
	cameras when loading COLLADA meshes. By default, this is set to false. If this is
	set to true, the ISceneManager::getMesh() method will only return a pointer to a
	dummy mesh and create instances of all meshes and lights and cameras in the collada
	file by itself. Example:
	\code
	SceneManager->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);
	\endcode
	*/
	const c8* const COLLADA_CREATE_SCENE_INSTANCES = "COLLADA_CreateSceneInstances";

	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** This path is prefixed to the file names defined in the Deled file when loading
	textures. This allows to alter the paths for a specific project setting.
	Use it like this:
	\code
	SceneManager->getStringParameters()->setAttribute(scene::DMF_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const DMF_TEXTURE_PATH = "DMF_TexturePath";

	//! Name of the parameter for preserving DMF textures dir structure with built-in DMF loader.
	/** If this parameter is set to true, the texture directory defined in the Deled file
	is ignored, and only the texture name is used to find the proper file. Otherwise, the
	texture path is also used, which allows to use a nicer media layout.
	Use it like this:
	\code
	//this way you won't use this setting (default)
	SceneManager->getParameters()->setAttribute(scene::DMF_IGNORE_MATERIALS_DIRS, false);
	\endcode
	\code
	//this way you'll use this setting
	SceneManager->getParameters()->setAttribute(scene::DMF_IGNORE_MATERIALS_DIRS, true);
	\endcode
	**/
	const c8* const DMF_IGNORE_MATERIALS_DIRS = "DMF_IgnoreMaterialsDir";

	//! Name of the parameter for setting reference value of alpha in transparent materials.
	/** Use it like this:
	\code
	//this way you'll set alpha ref to 0.1
	SceneManager->getParameters()->setAttribute(scene::DMF_ALPHA_CHANNEL_REF, 0.1);
	\endcode
	**/
	const c8* const DMF_ALPHA_CHANNEL_REF = "DMF_AlphaRef";

	//! Name of the parameter for choose to flip or not tga files.
	/** Use it like this:
	\code
	//this way you'll choose to flip alpha textures
	SceneManager->getParameters()->setAttribute(scene::DMF_FLIP_ALPHA_TEXTURES, true);
	\endcode
	**/
	const c8* const DMF_FLIP_ALPHA_TEXTURES = "DMF_FlipAlpha";


	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** Was used for changing the texture path of the built-in obj loader like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::OBJ_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const OBJ_TEXTURE_PATH = "OBJ_TexturePath";

	//! Flag to avoid loading group structures in .obj files
	/** Use it like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::OBJ_LOADER_IGNORE_GROUPS, true);
	\endcode
	**/
	const c8* const OBJ_LOADER_IGNORE_GROUPS = "OBJ_IgnoreGroups";


	//! Flag to avoid loading material .mtl file for .obj files
	/** Use it like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::OBJ_LOADER_IGNORE_MATERIAL_FILES, true);
	\endcode
	**/
	const c8* const OBJ_LOADER_IGNORE_MATERIAL_FILES = "OBJ_IgnoreMaterialFiles";


	//! Flag to ignore the b3d file's mipmapping flag
	/** Instead Irrlicht's texture creation flag is used. Use it like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::B3D_LOADER_IGNORE_MIPMAP_FLAG, true);
	\endcode
	**/
	const c8* const B3D_LOADER_IGNORE_MIPMAP_FLAG = "B3D_IgnoreMipmapFlag";

	//! Deprecated, use IMeshLoader::getMeshTextureLoader()->setTexturePath instead.
	/** Was used for changing the texture path of the built-in b3d loader like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::B3D_TEXTURE_PATH, "path/to/your/textures");
	\endcode
	**/
	const c8* const B3D_TEXTURE_PATH = "B3D_TexturePath";

	//! Flag set as parameter when the scene manager is used as editor
	/** In this way special animators like deletion animators can be stopped from
	deleting scene nodes for example */
	const c8* const IRR_SCENE_MANAGER_IS_EDITOR = "IRR_Editor";

	//! Name of the parameter for setting the length of debug normals.
	/** Use it like this:
	\code
	SceneManager->getParameters()->setAttribute(scene::DEBUG_NORMAL_LENGTH, 1.5f);
	\endcode
	**/
	const c8* const DEBUG_NORMAL_LENGTH = "DEBUG_Normal_Length";

	//! Name of the parameter for setting the color of debug normals.
	/** Use it like this:
	\code
	SceneManager->getParameters()->setAttributeAsColor(scene::DEBUG_NORMAL_COLOR, video::SColor(255, 255, 255, 255));
	\endcode
	**/
	const c8* const DEBUG_NORMAL_COLOR = "DEBUG_Normal_Color";

	}


	//! The video namespace contains classes for accessing the video driver. All 2d and 3d rendering is done here.
	namespace video
	{
enum E_TRANSFORMATION_STATE
	{
		//! View transformation
		ETS_VIEW = 0,
		//! World transformation
		ETS_WORLD,
		//! Projection transformation
		ETS_PROJECTION,
		//! Texture transformation
		ETS_TEXTURE_0,
		//! Texture transformation
		ETS_TEXTURE_1,
		//! Texture transformation
		ETS_TEXTURE_2,
		//! Texture transformation
		ETS_TEXTURE_3,
#if _IRR_MATERIAL_MAX_TEXTURES_>4
		//! Texture transformation
		ETS_TEXTURE_4,
#if _IRR_MATERIAL_MAX_TEXTURES_>5
		//! Texture transformation
		ETS_TEXTURE_5,
#if _IRR_MATERIAL_MAX_TEXTURES_>6
		//! Texture transformation
		ETS_TEXTURE_6,
#if _IRR_MATERIAL_MAX_TEXTURES_>7
		//! Texture transformation
		ETS_TEXTURE_7,
#endif
#endif
#endif
#endif
		//! Only used internally
		ETS_COUNT = ETS_TEXTURE_0 + _IRR_MATERIAL_MAX_TEXTURES_
	};

	//! Special render targets, which usually map to dedicated hardware
	/** These render targets (besides 0 and 1) need not be supported by gfx cards */
	enum E_RENDER_TARGET
	{
		//! Render target is the main color frame buffer
		ERT_FRAME_BUFFER=0,
		//! Render target is a render texture
		ERT_RENDER_TEXTURE,
		//! Multi-Render target textures
		ERT_MULTI_RENDER_TEXTURES,
		//! Render target is the main color frame buffer
		ERT_STEREO_LEFT_BUFFER,
		//! Render target is the right color buffer (left is the main buffer)
		ERT_STEREO_RIGHT_BUFFER,
		//! Render to both stereo buffers at once
		ERT_STEREO_BOTH_BUFFERS,
		//! Auxiliary buffer 0
		ERT_AUX_BUFFER0,
		//! Auxiliary buffer 1
		ERT_AUX_BUFFER1,
		//! Auxiliary buffer 2
		ERT_AUX_BUFFER2,
		//! Auxiliary buffer 3
		ERT_AUX_BUFFER3,
		//! Auxiliary buffer 4
		ERT_AUX_BUFFER4
	};

	//! Enum for the flags of clear buffer
	enum E_CLEAR_BUFFER_FLAG
	{
		ECBF_NONE = 0,
		ECBF_COLOR = 1,
		ECBF_DEPTH = 2,
		ECBF_STENCIL = 4,
		ECBF_ALL = ECBF_COLOR|ECBF_DEPTH|ECBF_STENCIL
	};

	//! Enum for the types of fog distributions to choose from
	enum E_FOG_TYPE
	{
		EFT_FOG_EXP=0,
		EFT_FOG_LINEAR,
		EFT_FOG_EXP2
	};

	const c8* const FogTypeNames[] =
	{
		"FogExp",
		"FogLinear",
		"FogExp2",
		0
	};
	//! Material flags
	enum E_MATERIAL_FLAG
	{
		//! Draw as wireframe or filled triangles? Default: false
		EMF_WIREFRAME = 0x1,

		//! Draw as point cloud or filled triangles? Default: false
		EMF_POINTCLOUD = 0x2,

		//! Flat or Gouraud shading? Default: true
		EMF_GOURAUD_SHADING = 0x4,

		//! Will this material be lighted? Default: true
		EMF_LIGHTING = 0x8,

		//! Is the ZBuffer enabled? Default: true
		EMF_ZBUFFER = 0x10,

		//! May be written to the zbuffer or is it readonly. Default: true
		/** This flag is ignored, if the material type is a transparent type. */
		EMF_ZWRITE_ENABLE = 0x20,

		//! Is backface culling enabled? Default: true
		EMF_BACK_FACE_CULLING = 0x40,

		//! Is frontface culling enabled? Default: false
		/** Overrides EMF_BACK_FACE_CULLING if both are enabled. */
		EMF_FRONT_FACE_CULLING = 0x80,

		//! Is bilinear filtering enabled? Default: true
		EMF_BILINEAR_FILTER = 0x100,

		//! Is trilinear filtering enabled? Default: false
		/** If the trilinear filter flag is enabled,
		the bilinear filtering flag is ignored. */
		EMF_TRILINEAR_FILTER = 0x200,

		//! Is anisotropic filtering? Default: false
		/** In Irrlicht you can use anisotropic texture filtering in
		conjunction with bilinear or trilinear texture filtering
		to improve rendering results. Primitives will look less
		blurry with this flag switched on. */
		EMF_ANISOTROPIC_FILTER = 0x400,

		//! Is fog enabled? Default: false
		EMF_FOG_ENABLE = 0x800,

		//! Normalizes normals. Default: false
		/** You can enable this if you need to scale a dynamic lighted
		model. Usually, its normals will get scaled too then and it
		will get darker. If you enable the EMF_NORMALIZE_NORMALS flag,
		the normals will be normalized again, and the model will look
		as bright as it should. */
		EMF_NORMALIZE_NORMALS = 0x1000,

		//! Access to all layers texture wrap settings. Overwrites separate layer settings.
		/** Note that if you want to change TextureWrapU, TextureWrapV, TextureWrapW
		independently, then you can't work with this flag, but will have to set the variables
		directly. */
		EMF_TEXTURE_WRAP = 0x2000,

		//! AntiAliasing mode
		EMF_ANTI_ALIASING = 0x4000,

		//! ColorMask bits, for enabling the color planes
		EMF_COLOR_MASK = 0x8000,

		//! ColorMaterial enum for vertex color interpretation
		EMF_COLOR_MATERIAL = 0x10000,

		//! Flag for enabling/disabling mipmap usage
		EMF_USE_MIP_MAPS = 0x20000,

		//! Flag for blend operation
		EMF_BLEND_OPERATION = 0x40000,

		//! Flag for polygon offset
		EMF_POLYGON_OFFSET = 0x80000,

        //! Flag for blend factor
		EMF_BLEND_FACTOR = 0x160000
	};
	//! Abstracted and easy to use fixed function/programmable pipeline material modes.
	enum E_MATERIAL_TYPE
	{
		//! Standard solid material.
		/** Only first texture is used, which is supposed to be the
		diffuse material. */
		EMT_SOLID = 0,

		//! Solid material with 2 texture layers.
		/** The second is blended onto the first using the alpha value
		of the vertex colors. This material is currently not implemented in OpenGL.
		*/
		EMT_SOLID_2_LAYER,

		//! Material type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is
		ignored. */
		EMT_LIGHTMAP,

		//! Material type with lightmap technique like EMT_LIGHTMAP.
		/** But lightmap and diffuse texture are added instead of modulated. */
		EMT_LIGHTMAP_ADD,

		//! Material type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is
		ignored. The texture colors are effectively multiplied by 2
		for brightening. Like known in DirectX as D3DTOP_MODULATE2X. */
		EMT_LIGHTMAP_M2,

		//! Material type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is
		ignored. The texture colors are effectively multiplied by 4
		for brightening. Like known in DirectX as D3DTOP_MODULATE4X. */
		EMT_LIGHTMAP_M4,

		//! Like EMT_LIGHTMAP, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING,

		//! Like EMT_LIGHTMAP_M2, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING_M2,

		//! Like EMT_LIGHTMAP_M4, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING_M4,

		//! Detail mapped material.
		/** The first texture is diffuse color map, the second is added
		to this and usually displayed with a bigger scale value so that
		it adds more detail. The detail map is added to the diffuse map
		using ADD_SIGNED, so that it is possible to add and subtract
		color from the diffuse map. For example a value of
		(127,127,127) will not change the appearance of the diffuse map
		at all. Often used for terrain rendering. */
		EMT_DETAIL_MAP,

		//! Look like a reflection of the environment around it.
		/** To make this possible, a texture called 'sphere map' is
		used, which must be set as the first texture. */
		EMT_SPHERE_MAP,

		//! A reflecting material with an optional non reflecting texture layer.
		/** The reflection map should be set as first texture. */
		EMT_REFLECTION_2_LAYER,

		//! A transparent material.
		/** Only the first texture is used. The new color is calculated
		by simply adding the source color and the dest color. This
		means if for example a billboard using a texture with black
		background and a red circle on it is drawn with this material,
		the result is that only the red circle will be drawn a little
		bit transparent, and everything which was black is 100%
		transparent and not visible. This material type is useful for
		particle effects. */
		EMT_TRANSPARENT_ADD_COLOR,

		//! Makes the material transparent based on the texture alpha channel.
		/** The final color is blended together from the destination
		color and the texture color, using the alpha channel value as
		blend factor. Only first texture is used. If you are using
		this material with small textures, it is a good idea to load
		the texture in 32 bit mode
		(video::IVideoDriver::setTextureCreationFlag()). Also, an alpha
		ref is used, which can be manipulated using
		SMaterial::MaterialTypeParam. This value controls how sharp the
		edges become when going from a transparent to a solid spot on
		the texture. */
		EMT_TRANSPARENT_ALPHA_CHANNEL,

		//! Makes the material transparent based on the texture alpha channel.
		/** If the alpha channel value is greater than 127, a
		pixel is written to the target, otherwise not. This
		material does not use alpha blending and is a lot faster
		than EMT_TRANSPARENT_ALPHA_CHANNEL. It is ideal for drawing
		stuff like leaves of plants, because the borders are not
		blurry but sharp. Only first texture is used. If you are
		using this material with small textures and 3d object, it
		is a good idea to load the texture in 32 bit mode
		(video::IVideoDriver::setTextureCreationFlag()). */
		EMT_TRANSPARENT_ALPHA_CHANNEL_REF,

		//! Makes the material transparent based on the vertex alpha value.
		EMT_TRANSPARENT_VERTEX_ALPHA,

		//! A transparent reflecting material with an optional additional non reflecting texture layer.
		/** The reflection map should be set as first texture. The
		transparency depends on the alpha value in the vertex colors. A
		texture which will not reflect can be set as second texture.*/
		EMT_TRANSPARENT_REFLECTION_2_LAYER,

		//! A solid normal map renderer.
		/** First texture is the color map, the second should be the
		normal map. Note that you should use this material only when
		drawing geometry consisting of vertices of type
		S3DVertexTangents (EVT_TANGENTS). You can convert any mesh into
		this format using IMeshManipulator::createMeshWithTangents()
		(See SpecialFX2 Tutorial). This shader runs on vertex shader
		1.1 and pixel shader 1.1 capable hardware and falls back to a
		fixed function lighted material if this hardware is not
		available. Only two lights are supported by this shader, if
		there are more, the nearest two are chosen. */
		EMT_NORMAL_MAP_SOLID,

		//! A transparent normal map renderer.
		/** First texture is the color map, the second should be the
		normal map. Note that you should use this material only when
		drawing geometry consisting of vertices of type
		S3DVertexTangents (EVT_TANGENTS). You can convert any mesh into
		this format using IMeshManipulator::createMeshWithTangents()
		(See SpecialFX2 Tutorial). This shader runs on vertex shader
		1.1 and pixel shader 1.1 capable hardware and falls back to a
		fixed function lighted material if this hardware is not
		available. Only two lights are supported by this shader, if
		there are more, the nearest two are chosen. */
		EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR,

		//! A transparent (based on the vertex alpha value) normal map renderer.
		/** First texture is the color map, the second should be the
		normal map. Note that you should use this material only when
		drawing geometry consisting of vertices of type
		S3DVertexTangents (EVT_TANGENTS). You can convert any mesh into
		this format using IMeshManipulator::createMeshWithTangents()
		(See SpecialFX2 Tutorial). This shader runs on vertex shader
		1.1 and pixel shader 1.1 capable hardware and falls back to a
		fixed function lighted material if this hardware is not
		available.  Only two lights are supported by this shader, if
		there are more, the nearest two are chosen. */
		EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,

		//! Just like EMT_NORMAL_MAP_SOLID, but uses parallax mapping.
		/** Looks a lot more realistic. This only works when the
		hardware supports at least vertex shader 1.1 and pixel shader
		1.4. First texture is the color map, the second should be the
		normal map. The normal map texture should contain the height
		value in the alpha component. The
		IVideoDriver::makeNormalMapTexture() method writes this value
		automatically when creating normal maps from a heightmap when
		using a 32 bit texture. The height scale of the material
		(affecting the bumpiness) is being controlled by the
		SMaterial::MaterialTypeParam member. If set to zero, the
		default value (0.02f) will be applied. Otherwise the value set
		in SMaterial::MaterialTypeParam is taken. This value depends on
		with which scale the texture is mapped on the material. Too
		high or low values of MaterialTypeParam can result in strange
		artifacts. */
		EMT_PARALLAX_MAP_SOLID,

		//! A material like EMT_PARALLAX_MAP_SOLID, but transparent.
		/** Using EMT_TRANSPARENT_ADD_COLOR as base material. */
		EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR,

		//! A material like EMT_PARALLAX_MAP_SOLID, but transparent.
		/** Using EMT_TRANSPARENT_VERTEX_ALPHA as base material. */
		EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA,

		//! BlendFunc = source * sourceFactor + dest * destFactor ( E_BLEND_FUNC )
		/** Using only first texture. Generic blending method.
		The blend function is set to SMaterial::MaterialTypeParam with
		pack_textureBlendFunc (for 2D) or pack_textureBlendFuncSeparate (for 3D). */
		EMT_ONETEXTURE_BLEND,

		//! This value is not used. It only forces this enumeration to compile to 32 bit.
		EMT_FORCE_32BIT = 0x7fffffff
	};

	//! Array holding the built in material type names
	const char* const sBuiltInMaterialTypeNames[] =
	{
		"solid",
		"solid_2layer",
		"lightmap",
		"lightmap_add",
		"lightmap_m2",
		"lightmap_m4",
		"lightmap_light",
		"lightmap_light_m2",
		"lightmap_light_m4",
		"detail_map",
		"sphere_map",
		"reflection_2layer",
		"trans_add",
		"trans_alphach",
		"trans_alphach_ref",
		"trans_vertex_alpha",
		"trans_reflection_2layer",
		"normalmap_solid",
		"normalmap_trans_add",
		"normalmap_trans_vertexalpha",
		"parallaxmap_solid",
		"parallaxmap_trans_add",
		"parallaxmap_trans_vertexalpha",
		"onetexture_blend",
		0
	};
	//! Enumeration of cube texture surfaces
	enum E_CUBE_SURFACE
	{
		ECS_POSX  = 0,
		ECS_NEGX,
		ECS_POSY,
		ECS_NEGY,
		ECS_POSZ,
		ECS_NEGZ
	};


	}
}


#endif // __E_MESH_WRITER_ENUMS_H_INCLUDED__

