// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "Engine.h"
#include "CGameScene.h"
#include "CMeshManipulator.h"
#include "CImage.h"
#include "COpenGLTexture.h"
#include "CGeometryCreator.h"
#include "CFileSystem.h"
#include "CMeshBuffer.h"
#include "CVertexBuffer.h"
#include "CColorConverter.h"
#define PI 3.14159265358979323846f
#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)



static const char* const copyright = "Irrlicht Engine (c) 2002-2017 Nikolaus Gebhardt";	// put string in binary

#ifdef _IRR_WINDOWS_
	#include <windows.h>
	#if defined(_DEBUG) && !defined(__GNUWIN32__) && !defined(_WIN32_WCE)
		#include <crtdbg.h>
	#endif // _DEBUG
#endif

#include "irrlicht.h"





namespace irr
{
static scene::Engine *theInstance;
extern  s32 ReferenceCounter;



namespace core
{
	const matrix4 IdentityMatrix(matrix4::EM4CONST_IDENTITY);
	irr::core::stringc LOCALE_DECIMAL_POINTS(".");

}

namespace video
{
	SMaterial IdentityMaterial;
	u32 MATERIAL_MAX_TEXTURES_USED = MATERIAL_MAX_TEXTURES;
}

using namespace core;
namespace scene
{
#include "core/Shaders.cc"

Engine::Engine(int Width, int Height,bool vsync, const char* title):AlphaMode(GL_ALWAYS), AlphaRef(0.f), AlphaTest(false),FrameBufferCount(0), BlendEquation(0), BlendSourceRGB(0),
		BlendDestinationRGB(0), BlendSourceAlpha(0), BlendDestinationAlpha(0), Blend(0), BlendEquationInvalid(false), BlendFuncInvalid(false), BlendInvalid(false),
		ColorMask(0), ColorMaskInvalid(false), CullFaceMode(GL_BACK), CullFace(false), DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), FrameBufferID(0),
		ProgramID(0), ActiveTexture(GL_TEXTURE0), ViewportX(0), ViewportY(0),StencilTest(false)
{

if (SDL_Init(SDL_INIT_VIDEO) < 0)
{
 SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
 return ;
}

// Request OpenGL ES 3.0
//scene::Engine::Instance()->
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES)!=0)
    {
        SDL_Log( "ERROR loading context profile mask");
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)!=0)
    {
        SDL_Log("ERROR setting context major version");
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0)!=0)
    {
        SDL_Log("ERROR setting context minor version");
    }
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0){
        SDL_Log( " ERROR \n setting double buffer");
    } // I have tried without the dubble buffer and nothing changes
    if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE   , 24) != 0){
        SDL_Log( " ERROR \n setting depth buffer");
    }
    if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32)!=0){
        SDL_Log( "ERROR setting buffer size");
    }
    if(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }
    if(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }if(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }if(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)!=0){
        SDL_Log(" Error setting alpha");
    }

   if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) != 0){
        SDL_Log( " Error  setting stencil buffer");
    }



renderTargetSize.set(Width,Height);
ScreenSize.set(Width,Height);

window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, Width, Height,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
if (!window)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error","Couldn't create the main window.", NULL);
return ;
}
context = SDL_GL_CreateContext(window);
if (!context)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error","Couldn't create an OpenGL context.", NULL);
return ;
}

 SDL_GL_SetSwapInterval( (vsync ? 1 : 0) ); // Enable vsync



SDL_Log("**************************************************************");
SDL_Log("[ENGINE]");
SDL_Log("");
setDebugName("Engine");
theInstance = this;

currentShader=nullptr;

os::Printer::log("[ENGINE] Init", ELL_INFORMATION);


srand((unsigned int)SDL_GetTicks());    // Initialize random seed
previous = GetTime();     // Get time as double
current = GetTime();            // Number of elapsed seconds since InitTimer()
update = current - previous;
previous = current;

currMaterial=IdentityMaterial;
LastMaterial=IdentityMaterial;

FrameBufferCount =8;


os::Printer::log("[ENGINE] Setup defaults  . ", ELL_INFORMATION);
os::Printer::print(ELL_INFORMATION,"GL: OpenGL device information:");
os::Printer::print(ELL_INFORMATION,"    > Vendor:   %s", glGetString(GL_VENDOR));
os::Printer::print(ELL_INFORMATION,"    > Renderer: %s", glGetString(GL_RENDERER));
os::Printer::print(ELL_INFORMATION,"    > Version:  %s", glGetString(GL_VERSION));
os::Printer::print(ELL_INFORMATION,"    > GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));



    // Get supported extensions list
    GLint numExt = 0;
    const char **extList =(const char**) malloc(512*sizeof(const char *)); // Allocate 512 strings pointers (2 KB)
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);  // One big const string

    // NOTE: We have to duplicate string because glGetString() returns a const string
    int len = strlen(extensions) + 1;
    char *extensionsDup = (char *)calloc(len, sizeof(char));
    strcpy(extensionsDup, extensions);
    extList[numExt] = extensionsDup;

    for (int i = 0; i < len; i++)
    {
        if (extensionsDup[i] == ' ')
        {
            extensionsDup[i] = '\0';
            numExt++;
            extList[numExt] = &extensionsDup[i + 1];
        }
    }

    bool texNPOT=false;
    os::Printer::print(ELL_INFORMATION, "GL: Supported extensions count: %i", numExt);
//    os::Printer::print(ELL_INFORMATION, "GL: OpenGL extensions:");
  //  for (int i = 0; i < numExt; i++) os::Printer::print(ELL_INFORMATION, "    %s", extList[i]);

    // Check required extensions
    for (int i = 0; i < numExt; i++)
    {
        // Check VAO support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has VAO support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_vertex_array_object") == 0)
        {
          //extvao = true;
        }

        // Check instanced rendering support
        if (strcmp(extList[i], (const char *)"GL_ANGLE_instanced_arrays") == 0)         // Web ANGLE
        {
          //extinstancing = true;
        }
        else
        {
            if ((strcmp(extList[i], (const char *)"GL_EXT_draw_instanced") == 0) &&     // Standard EXT
                (strcmp(extList[i], (const char *)"GL_EXT_instanced_arrays") == 0))
            {
              // extinstancing = true;
            }
        }


        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_texture_npot") == 0) texNPOT = true;

        // Check texture float support
        //if (strcmp(extList[i], (const char *)"GL_OES_texture_float") == 0) texFloat32 = true;

        // Check depth texture support
       // if ((strcmp(extList[i], (const char *)"GL_OES_depth_texture") == 0) ||
      //      (strcmp(extList[i], (const char *)"GL_WEBGL_depth_texture") == 0)) texDepth = true;

      //  if (strcmp(extList[i], (const char *)"GL_OES_depth24") == 0) maxDepthBits = 24;
      //  if (strcmp(extList[i], (const char *)"GL_OES_depth32") == 0) maxDepthBits = 32;


    }

    // Free extensions pointers
    free(extList);
    free(extensionsDup);    // Duplicated string must be deallocated


		BlendEquation = new GLenum[FrameBufferCount];
		BlendSourceRGB = new GLenum[FrameBufferCount];
		BlendDestinationRGB = new GLenum[FrameBufferCount];
		BlendSourceAlpha = new GLenum[FrameBufferCount];
		BlendDestinationAlpha = new GLenum[FrameBufferCount];
		Blend = new bool[FrameBufferCount];
		ColorMask = new u8[FrameBufferCount];

		// Initial OpenGL values from specification.


			glBlendEquation(GL_FUNC_ADD);


		for (u32 i = 0; i < FrameBufferCount; ++i)
		{
			BlendEquation[i] = GL_FUNC_ADD;

			BlendSourceRGB[i] = GL_ONE;
			BlendDestinationRGB[i] = GL_ZERO;
			BlendSourceAlpha[i] = GL_ONE;
			BlendDestinationAlpha[i] = GL_ZERO;

			Blend[i] = false;
			ColorMask[i] = ECP_ALL;
		}

		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_BLEND);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glCullFace(CullFaceMode);
		glDisable(GL_CULL_FACE);

		glDepthFunc(DepthFunc);
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(ActiveTexture);

		glDisable(GL_TEXTURE_2D);


		ViewportWidth = Width;
		ViewportHeight =Height;
		glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);

glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);

glPixelStorei(GL_PACK_ALIGNMENT, 1);

glClearDepthf(1.0f);

glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
//glFrontFace(GL_CW);

//glAlphaFunc(AlphaMode, AlphaRef);
//glDisable(GL_ALPHA_TEST);

//glViewport(0, 0, screenSize.Width, screenSize.Height); // Reset The Current Viewport
//glShadeModel(GL_SMOOTH);
//setAmbientLight(SColor(0,0,0,0));

//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glDepthFunc(GL_LEQUAL);


GLint mxaf=0;
glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mxaf);
MaxAnisotropicFilter=(u8)mxaf;


   	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepthf(1.0);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_BLEND);


int StencilBits;
glGetIntegerv(GL_STENCIL_BITS, &StencilBits);
midStencilVal = (StencilBits - 1)^2;
glClearStencil(midStencilVal);

stencil_vbo;
glGenBuffers(1, &stencil_vbo);
glBindBuffer(GL_ARRAY_BUFFER, stencil_vbo);
GLfloat q3[] = {1,1,-1,1,-1,-1,1,-1};
glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),q3,GL_STATIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);


os::Printer::log("[ENGINE] Create File System  . ", ELL_INFORMATION);
FileSystem      = new CFileSystem();
//FileSystem->grab();
os::Printer::log("[ENGINE] Create Mesh Manipulator . ", ELL_INFORMATION);
MeshManipulator = new CMeshManipulator();
//MeshManipulator->grab();
os::Printer::log("[ENGINE] Create Geometry Creator . ", ELL_INFORMATION);
GeometryCreator = new CGeometryCreator();
//GeometryCreator->grab();
os::Timer::initTimer();
os::Timer::startTimer();
InputInit();




createSolidShader();
createStencilShader();
createPointsShader();
createScreenShader();
createTextureShader();
createlLightTextureDetailShader();
createlLightTextureShader();
createSkyBoxShader();
createBatchShader();

rlInit();
}

Engine::~Engine()
{
SDL_Log("**************************************************************");
SDL_Log("[ENGINE]");
SDL_Log("");
os::Printer::log("[ENGINE] Free resources . ", ELL_INFORMATION);
//s32 obj_total = ReferenceCounter;

glDeleteBuffers(1, &stencil_vbo);

os::Printer::log("[ENGINE] Free File System  . ", ELL_INFORMATION);
FileSystem->drop();
//delete FileSystem;
os::Printer::log("[ENGINE] Free Mesh Manipulator . ", ELL_INFORMATION);
MeshManipulator->drop();
//delete MeshManipulator;
os::Printer::log("[ENGINE] Free Geometry Creator . ", ELL_INFORMATION);
GeometryCreator->drop();
//delete GeometryCreator;
os::Printer::log("[ENGINE] Free Texture List . ", ELL_INFORMATION);
deleteAllTextures();

os::Printer::log("[ENGINE] Remove Scenes List . ", ELL_INFORMATION);
removeScenes();

rlClose();

os::Printer::log("[ENGINE] Free Shaders List . ", ELL_INFORMATION);
for (auto iter : Shaders)
{

        iter.second->Unload();

 }

/*core::map<const char*, IShader*>::ParentLastIterator i = Shaders.getParentLastIterator();
while(!i.atEnd())
{
    core::map<const char*, IShader*>::Node* p = i.getNode();
    IShader* s=p->getValue();
    if (s)
    {
        s->Unload();
        delete s;
    }
    i++;
}*/


Shaders.clear();
os::Printer::log("[ENGINE] Free ", ELL_INFORMATION);


SDL_GL_DeleteContext(context);
SDL_DestroyWindow(window);
SDL_Quit();
}

Engine* Engine::Instance()
{
return theInstance;
}

void Engine::Swap()
{
 glFinish();
 SDL_GL_SwapWindow(window);
}
void Engine::ShowSimpleMessageBox(Uint32 flags, const char *title, const char *message)
{
SDL_ShowSimpleMessageBox(flags,
                         title,
                         message,
                         window);
}

Engine* Engine::createEngine(int width, int heigth,bool vsync, const char* title)
{
  Engine* eng = new  Engine( width,  heigth, vsync,  title);

  return eng;
}
void Engine::startStencilTest()
{

    setStencilTest(true);
	glClearStencil(midStencilVal);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glEnable( GL_CULL_FACE );

}
void Engine::endStencilTest()
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	setStencilTest(false);
}
void Engine::drawStencilShadow(bool clearStencilBuffer)
{




		setBlend(true);
		setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		setStencilTest(true);



        glCullFace(GL_BACK);
        glStencilFunc(GL_NOTEQUAL, midStencilVal, 0xffffff);
        glStencilOp(GL_KEEP , GL_KEEP , GL_KEEP);



        glBindBuffer(GL_ARRAY_BUFFER, stencil_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);




    	setStencilTest(false);
    	if (clearStencilBuffer)
    	   glClear(GL_STENCIL_BUFFER_BIT);


		/*



        setDepthMask(false);
		setColorMask(ECP_ALL);


		glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);


		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        glBindBuffer(GL_ARRAY_BUFFER, stencil_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_STENCIL_TEST);*/
}

void Engine::ClearScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil)
{
  // glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLbitfield mask = 0;
    u8 colorMask = 0;
    bool depthMask = false;

    getColorMask(colorMask);
    getDepthMask(depthMask);

    if (clearFlag & ECBF_COLOR)
    {
        setColorMask(ECP_ALL);

        const f32 inv = 1.0f / 255.0f;
        glClearColor(clearColor.getRed() * inv, clearColor.getGreen() * inv,
            clearColor.getBlue() * inv, clearColor.getAlpha() * inv);

        mask |= GL_COLOR_BUFFER_BIT;
    }

    if (clearFlag & ECBF_DEPTH)
    {
        setDepthMask(true);
        glClearDepthf(clearDepth);
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (clearFlag & ECBF_STENCIL)
    {
        glClearStencil(clearStencil);
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    if (mask)
        glClear(mask);

    setColorMask(colorMask);
    setDepthMask(depthMask);
}
void Engine::DrawMeshBufferNormals(const scene::IMeshBuffer* mb, f32 length, SColor color)
{
	const u32 count = mb->getVertexCount();
	const bool normalize = mb->getMaterial().NormalizeNormals;

	for (u32 i=0; i < count; ++i)
	{
		core::vector3df normalizedNormal = mb->getNormal(i);
		if (normalize)
			normalizedNormal.normalize();

		const core::vector3df& pos = mb->getPosition(i);
		video::DrawLine3D(pos, pos + (normalizedNormal * length), color);
	}
}
void Engine::DrawMeshBufferNormals(core::matrix4 mat,const scene::IMeshBuffer* mb, f32 length, SColor color)
{
	const u32 count = mb->getVertexCount();
	const bool normalize = mb->getMaterial().NormalizeNormals;

	for (u32 i=0; i < count; ++i)
	{
		core::vector3df normalizedNormal = mb->getNormal(i);
		if (normalize)
			normalizedNormal.normalize();

		 core::vector3df pos = mb->getPosition(i);
		 core::vector3df pos_tras;
		mat.transformVect(pos_tras,pos);
		video::DrawLine3D(pos_tras, pos_tras + (normalizedNormal * length), color);
	}
}

 void Engine::DrawGrid(int slices, float spacing)
{


  int halfSlices = slices / 2;

DrawLine3D(0, 0.1f, 0 ,0, 1.0f, 0 ,video::SColor(255,255,0,0) );
DrawLine3D(0, 0.1f, 0 ,1.0f, 0.1f, 0 ,video::SColor(255,0,0,255) );
DrawLine3D(0, 0.1f, 0 ,0, 0.1f, 1.0f ,video::SColor(255,0,255,0) );




        for(int i = -halfSlices; i <= halfSlices; i++)
        {

            DrawLine3D( core::vector3df((float)i*spacing, 0.0f, (float)-halfSlices*spacing),
                        core::vector3df((float)i*spacing, 0.0f, (float)halfSlices*spacing)  ,video::SColor(255,255,255 ,255) );

            DrawLine3D( core::vector3df( (float)-halfSlices*spacing, 0.0f, (float)i*spacing),
                        core::vector3df( (float)halfSlices*spacing, 0.0f, (float)i*spacing)  ,video::SColor(255,255,255,255 ) );
        }


}

void Engine::Draw3DBox(const core::aabbox3d<f32>& box, SColor color)
{
	core::vector3df edges[8];
	box.getEdges(edges);

	// TODO: optimize into one big drawIndexPrimitive call.

	video::DrawLine3D(edges[5], edges[1], color);
	video::DrawLine3D(edges[1], edges[3], color);
	video::DrawLine3D(edges[3], edges[7], color);
	video::DrawLine3D(edges[7], edges[5], color);
	video::DrawLine3D(edges[0], edges[2], color);
	video::DrawLine3D(edges[2], edges[6], color);
	video::DrawLine3D(edges[6], edges[4], color);
	video::DrawLine3D(edges[4], edges[0], color);
	video::DrawLine3D(edges[1], edges[0], color);
	video::DrawLine3D(edges[3], edges[2], color);
	video::DrawLine3D(edges[7], edges[6], color);
	video::DrawLine3D(edges[5], edges[4], color);
}



int Engine::GetFPS(void)
{
return FPSCounter.getFPS();
}
void Engine::setWindowCaption(const char* title)
{
   SDL_SetWindowTitle(window,title);
}

void Engine::BeginScene()
{


current = GetTime();            // Number of elapsed seconds since InitTimer()
update = current - previous;
previous = current;
os::Timer::tick();



/*
glActiveTexture(GL_TEXTURE3);
glBindTexture(GL_TEXTURE_2D,0);

glActiveTexture(GL_TEXTURE2);
glBindTexture(GL_TEXTURE_2D,0);


glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D,0);


glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,0);
*/

	setActiveTexture(GL_TEXTURE4);
    setTexture(GL_TEXTURE_2D,0);

    setActiveTexture(GL_TEXTURE3);
    setTexture(GL_TEXTURE_2D,0);

    setActiveTexture(GL_TEXTURE2);
    setTexture(GL_TEXTURE_2D,0);

    setActiveTexture(GL_TEXTURE1);
    setTexture(GL_TEXTURE_2D,0);

    setActiveTexture(GL_TEXTURE0);
    setTexture(GL_TEXTURE_2D,0);


setMaterial(video::IdentityMaterial);

}


void Engine::EndScene()
{
    FPSCounter.registerFrame(os::Timer::getRealTime(), PrimitivesDrawn);
    current = GetTime();
	draw = current - previous;
	previous = current;
	frame = update + draw;


    ViewArea.cameraPosition = getCameraPosition();
	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
	m.setbyproduct_nocheck(ViewArea.getTransform(video::ETS_PROJECTION),ViewArea.getTransform(video::ETS_VIEW));
	ViewArea.setFrom(m, true);


/*

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);


	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

//****************************OPENGL


void Engine::setAlphaTest(bool enable)
{
	if (AlphaTest != enable)
	{
		/*if (enable)
			glEnable(GL_ALPHA_TEST);
		else
			glDisable(GL_ALPHA_TEST);*/
		AlphaTest = enable;
	}
}
/*

//! returns if node is culled
bool CSceneManager::isCulled(const ISceneNode* node) const
{
	const ICameraSceneNode* cam = getActiveCamera();
	if (!cam)
	{
		return false;
	}
	bool result = false;

	// has occlusion query information
	if (node->getAutomaticCulling() & scene::EAC_OCC_QUERY)
	{
		result = (Driver->getOcclusionQueryResult(const_cast<ISceneNode*>(node))==0);
	}

	// can be seen by a bounding box ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_BOX))
	{
		core::aabbox3d<f32> tbox = node->getBoundingBox();
		node->getAbsoluteTransformation().transformBoxEx(tbox);
		result = !(tbox.intersectsWithBox(cam->getViewFrustum()->getBoundingBox() ));
	}

	// can be seen by a bounding sphere
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_SPHERE))
	{
		const core::aabbox3df nbox = node->getTransformedBoundingBox();
		const float rad = nbox.getRadius();
		const core::vector3df center = nbox.getCenter();

		const float camrad = cam->getViewFrustum()->getBoundingRadius();
		const core::vector3df camcenter = cam->getViewFrustum()->getBoundingCenter();

		const float dist = (center - camcenter).getLengthSQ();
		const float maxdist = (rad + camrad) * (rad + camrad);

		result = dist > maxdist;
	}

	// can be seen by cam pyramid planes ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_BOX))
	{
		SViewFrustum frust = *cam->getViewFrustum();

		//transform the frustum to the node's current absolute transformation
		core::matrix4 invTrans(node->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
		//invTrans.makeInverse();
		frust.transform(invTrans);

		core::vector3df edges[8];
		node->getBoundingBox().getEdges(edges);

		for (s32 i=0; i<scene::SViewFrustum::VF_PLANE_COUNT; ++i)
		{
			bool boxInFrustum=false;
			for (u32 j=0; j<8; ++j)
			{
				if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
				{
					boxInFrustum=true;
					break;
				}
			}

			if (!boxInFrustum)
			{
				result = true;
				break;
			}
		}
	}

	return result;
}
*/
void Engine::setMaterial(const SMaterial& material)
{
 // if (LastMaterial!=material)
  {
    setBasicRenderStates(material,LastMaterial,false);
    LastMaterial=material;

  }

}

void Engine::renderMesh(IMesh* mesh)
{
    if (!mesh) return;
    for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
    {
       IMeshBuffer* meshBuffer =mesh->getMeshBuffer(i);
       if(meshBuffer)
       {
        renderMeshBuffer(meshBuffer,true);
       }
    }



}

void Engine::renderMesh(IMesh* mesh,IShader* shader)
{

if (!shader) return;
setShader(shader);
if(!mesh) return;
renderMesh(mesh);
}
void Engine::renderMesh(IMesh* mesh,IShader* shader,core::matrix4 model)
{

if (!shader) return;
setShader(shader,model);
if(!mesh) return;
renderMesh(mesh);
}


void Engine::renderMeshBuffer(IMeshBuffer* meshBuffer,bool useMaterial)
{
    int PrimitiveCount            =meshBuffer->getPrimitiveCount();
    E_PRIMITIVE_TYPE primitiveType=meshBuffer->getPrimitiveType();
    if (useMaterial)
    {
        video::SMaterial material=meshBuffer->getMaterial();
        setMaterial(material);
    }
    meshBuffer->render(PrimitiveCount,primitiveType);
}





void Engine::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial, bool resetAllRenderStates)
	{





		// ZBuffer
		switch (material.ZBuffer)
		{
			case ECFN_DISABLED:
				setDepthTest(false);
				break;
			case ECFN_LESSEQUAL:
				setDepthTest(true);
				setDepthFunc(GL_LEQUAL);
				break;
			case ECFN_EQUAL:
				setDepthTest(true);
				setDepthFunc(GL_EQUAL);
				break;
			case ECFN_LESS:
				setDepthTest(true);
				setDepthFunc(GL_LESS);
				break;
			case ECFN_NOTEQUAL:
				setDepthTest(true);
				setDepthFunc(GL_NOTEQUAL);
				break;
			case ECFN_GREATEREQUAL:
				setDepthTest(true);
				setDepthFunc(GL_GEQUAL);
				break;
			case ECFN_GREATER:
				setDepthTest(true);
				setDepthFunc(GL_GREATER);
				break;
			case ECFN_ALWAYS:
				setDepthTest(true);
				setDepthFunc(GL_ALWAYS);
				break;
			case ECFN_NEVER:
				setDepthTest(true);
				setDepthFunc(GL_NEVER);
				break;
			default:
				break;
		}



		// ZWrite
		if (getWriteZBuffer(material))
		{
			setDepthMask(true);
		}
		else
		{
			setDepthMask(false);
		}


		// Back face culling
		if ((material.FrontfaceCulling) && (material.BackfaceCulling))
		{
			setCullFaceFunc(GL_FRONT_AND_BACK);
			setCullFace(true);
		}
		else if (material.BackfaceCulling)
		{
			setCullFaceFunc(GL_BACK);
			setCullFace(true);
		}
		else if (material.FrontfaceCulling)
		{
			setCullFaceFunc(GL_FRONT);
			setCullFace(true);
		}
		else
		{
			setCullFace(false);
		}





		// Color Mask
		setColorMask(material.ColorMask);


		// Blend Equation
		if (material.BlendOperation == EBO_NONE)
			setBlend(false);
		else
		{
			setBlend(true);

			switch (material.BlendOperation)
			{
			case EBO_ADD:
				setBlendEquation(GL_FUNC_ADD);
				break;
			case EBO_SUBTRACT:
				setBlendEquation(GL_FUNC_SUBTRACT);
				break;
			case EBO_REVSUBTRACT:
				setBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				break;
			default:
				break;
			}
		}



		// Blend Factor
		if (IR(material.BlendFactor) & 0xFFFFFFFF	// TODO: why the & 0xFFFFFFFF?
			&& material.MaterialType != EMT_ONETEXTURE_BLEND
		)
		{
		    E_BLEND_FACTOR srcRGBFact = EBF_ZERO;
		    E_BLEND_FACTOR dstRGBFact = EBF_ZERO;
		    E_BLEND_FACTOR srcAlphaFact = EBF_ZERO;
		    E_BLEND_FACTOR dstAlphaFact = EBF_ZERO;
		    E_MODULATE_FUNC modulo = EMFN_MODULATE_1X;
		    u32 alphaSource = 0;

		    unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo, alphaSource, material.BlendFactor);
			setBlendFuncSeparate(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact),getGLBlend(srcAlphaFact), getGLBlend(dstAlphaFact));
		}

		// TODO: Polygon Offset. Not sure if it was left out deliberately or if it won't work with this driver.

		if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedLine[0], DimAliasedLine[1]));

		// Anti aliasing
		if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
		{
			if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}


		// Texture parameters
		setTextureRenderStates(material, resetAllRenderStates);

		currMaterial=material;

	}

    void Engine::setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates)
	{

		// Set textures to TU/TIU and apply filters to them
		int MaxTextureUnits=4;

		/*
		//for (s32 i = 0; i<MaxTextureUnits; i++)
		int i=0;
		{
			const COpenGLTexture* tmpTexture =static_cast<const COpenGLTexture*>(material.TextureLayer[i].Texture);

			if (!tmpTexture) return;
			//	continue;

			GLenum tmpTextureType = tmpTexture->getOpenGLTextureType();
			GLuint tmpTextureName = tmpTexture->getOpenGLTextureName();
//			if (tmpTextureName<=0) continue;



            SDL_Log("%d %d",tmpTextureName,i);

		//	setActiveTexture(GL_TEXTURE1 );
			//setTexture(tmpTextureType,tmpTextureName);

			glActiveTexture(GL_TEXTURE0);
        	glBindTexture(GL_TEXTURE_2D, tmpTextureName);

        }

        i=1;
		{
			const COpenGLTexture* tmpTexture =static_cast<const COpenGLTexture*>(material.TextureLayer[i].Texture);

			if (!tmpTexture) return;
			//	continue;

			GLenum tmpTextureType = tmpTexture->getOpenGLTextureType();
			GLuint tmpTextureName = tmpTexture->getOpenGLTextureName();
//			if (tmpTextureName<=0) continue;



            SDL_Log("%d %d",tmpTextureName,i);

			//setActiveTexture(GL_TEXTURE0 );
		//	setTexture(tmpTextureType,tmpTextureName);
		    glActiveTexture(GL_TEXTURE1);
        	glBindTexture(GL_TEXTURE_2D, tmpTextureName);
        }


        //setActiveTexture(GL_TEXTURE0);
*/
		for (s32 i = MaxTextureUnits - 1; i >= 0; --i)
		{
			const COpenGLTexture* tmpTexture =static_cast<const COpenGLTexture*>(material.TextureLayer[i].Texture);

			if (!tmpTexture)
				continue;

			GLenum tmpTextureType = tmpTexture->getOpenGLTextureType();
			GLuint tmpTextureName = tmpTexture->getOpenGLTextureId();
			if (tmpTextureName<=0) continue;



       //     SDL_Log("%d %d",tmpTextureName,i);

			setActiveTexture(GL_TEXTURE0 + i);
			setTexture(tmpTextureType,tmpTextureName);


			if (! material.TextureLayer[i].BilinearFilter !=currMaterial.TextureLayer[i].BilinearFilter ||
				material.TextureLayer[i].TrilinearFilter != currMaterial.TextureLayer[i].TrilinearFilter)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_MAG_FILTER,(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

				currMaterial.TextureLayer[i].BilinearFilter = material.TextureLayer[i].BilinearFilter;
				currMaterial.TextureLayer[i].TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
			}

			if (material.UseMipMaps )
			{
				if ( material.TextureLayer[i].BilinearFilter != currMaterial.TextureLayer[i].BilinearFilter ||
					material.TextureLayer[i].TrilinearFilter != currMaterial.TextureLayer[i].TrilinearFilter )
				{
					glTexParameteri(tmpTextureType, GL_TEXTURE_MIN_FILTER,
						material.TextureLayer[i].TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
						material.TextureLayer[i].BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
						GL_NEAREST_MIPMAP_NEAREST);

					currMaterial.TextureLayer[i].BilinearFilter = material.TextureLayer[i].BilinearFilter;
					currMaterial.TextureLayer[i].TrilinearFilter = material.TextureLayer[i].TrilinearFilter;

				}
			}
			else
			{
				if ( material.TextureLayer[i].BilinearFilter != currMaterial.TextureLayer[i].BilinearFilter ||
					material.TextureLayer[i].TrilinearFilter != currMaterial.TextureLayer[i].TrilinearFilter )
				{
					glTexParameteri(tmpTextureType, GL_TEXTURE_MIN_FILTER,
						(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

					currMaterial.TextureLayer[i].BilinearFilter = material.TextureLayer[i].BilinearFilter;
					currMaterial.TextureLayer[i].TrilinearFilter = material.TextureLayer[i].TrilinearFilter;

				}
			}





           if( material.TextureLayer[i].AnisotropicFilter !=  currMaterial.TextureLayer[i].AnisotropicFilter)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				material.TextureLayer[i].AnisotropicFilter>1 ? core::min_(MaxAnisotropicFilter, material.TextureLayer[i].AnisotropicFilter) : 1);
    			 currMaterial.TextureLayer[i].AnisotropicFilter = material.TextureLayer[i].AnisotropicFilter;
    			// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
			}


			if ( material.TextureLayer[i].TextureWrapU != currMaterial.TextureLayer[i].TextureWrapU)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
				currMaterial.TextureLayer[i].TextureWrapU = material.TextureLayer[i].TextureWrapU;
			}

			if ( material.TextureLayer[i].TextureWrapV != currMaterial.TextureLayer[i].TextureWrapV)
			{
				glTexParameteri(tmpTextureType, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
				currMaterial.TextureLayer[i].TextureWrapV = material.TextureLayer[i].TextureWrapV;
			}


		}



	}

	//! Removes a texture from the texture cache and deletes it, freeing lot of
//! memory.
void Engine::removeTexture(ITexture* texture)
{
	if (!texture)
		return;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if (Textures[i] == texture)
		{
			texture->drop();
			Textures.erase(i);
		}
	}
}


//! Removes all texture from the texture cache and deletes them, freeing lot of
//! memory.
void Engine::removeAllTextures()
{

	deleteAllTextures();
}


//! Returns a texture by index
ITexture* Engine::getTextureByIndex(u32 i)
{
	if ( i < Textures.size() )
		return Textures[i];

	return 0;
}
//! prints error if an error happened.
bool Engine::testGLError(int code)
{

    GLenum g = glGetError();
    switch (g)
    {
        case GL_NO_ERROR:
            return false;
        case GL_INVALID_ENUM:
            os::Printer::log("GL_INVALID_ENUM", core::stringc(code).c_str(), ELL_ERROR);
            break;
        case GL_INVALID_VALUE:
            os::Printer::log("GL_INVALID_VALUE", core::stringc(code).c_str(), ELL_ERROR);
            break;
        case GL_INVALID_OPERATION:
            os::Printer::log("GL_INVALID_OPERATION", core::stringc(code).c_str(), ELL_ERROR);
            break;
        case GL_OUT_OF_MEMORY:
            os::Printer::log("GL_OUT_OF_MEMORY", core::stringc(code).c_str(), ELL_ERROR);
            break;
    };
    return true;
}

//! deletes all textures
void Engine::deleteAllTextures()
{
	// we need to remove previously set textures which might otherwise be kept in the
	// last set material member. Could be optimized to reduce state changes.
	setMaterial(SMaterial());

	for (u32 i=0; i<Textures.size(); ++i)
	{

	    video::COpenGLTexture* tex=(video::COpenGLTexture*)Textures[i];
	    if(tex)
	    {

	    tex->drop();
	    tex = 0;
	  /*  tex->grab();

	    //SDL_Log("free texture : %d  %s  id[%d]",i,tex->getName().getInternalName().c_str(),tex->getOpenGLTextureName());
	    if (!tex->drop())
		if(tex)
		{
		  delete tex;
          tex=nullptr;

		}
		*/
        }
	}


	Textures.clear();
}

//! Returns amount of textures currently loaded
u32 Engine::getTextureCount() const
{
	return Textures.size();
}


//! Renames a texture
void Engine::renameTexture(ITexture* texture, const io::path& newName)
{
	// we can do a const_cast here safely, the name of the ITexture interface
	// is just readonly to prevent the user changing the texture name without invoking
	// this method, because the textures will need resorting afterwards

	io::SNamedPath& name = const_cast<io::SNamedPath&>(texture->getName());
	name.setPath(newName);

	Textures.sort();
}


//! loads a Texture
ITexture* Engine::getTexture(const io::path& filename)
{
	// Identify textures by their absolute filenames if possible.
	const io::path absolutePath = FileSystem->getAbsolutePath(filename);

	///SDL_Log("getTexture %s ",filename.c_str());
	//SDL_Log("getTexture %s ",absolutePath.c_str());


	ITexture* texture = findTexture(absolutePath);
	if (texture)
		return texture;

	//SDL_Log(" Then try the raw filename, which might be in an Archive");
	texture = findTexture(filename);
	if (texture)
		return texture;


//	SDL_Log("Now try to open the file using the complete path.");
	io::IReadFile* file = FileSystem->createAndOpenFile(absolutePath);

	if (!file)
	{
		//SDL_Log(" Try to open it using the raw filename.");
		file = FileSystem->createAndOpenFile(filename);
	}

	if (file)
	{
		//SDL_Log(" Re-check name for actual archive names");
		texture = findTexture(file->getFileName());
		if (texture)
		{
			file->drop();
			return texture;
		}

		texture = loadTextureFromFile(file);
		file->drop();

		if (texture)
		{
			addTexture(texture);
		}
		else
			os::Printer::log("[ENGINE] Could not load texture", filename, ELL_ERROR);

		return texture;
	}
	else
	{
		os::Printer::log("[ENGINE] Could not open file of texture", filename, ELL_WARNING);
		return 0;
	}
}


//! loads a Texture
ITexture* Engine::getTexture(io::IReadFile* file)
{
	ITexture* texture = 0;

	if (file)
	{
		texture = findTexture(file->getFileName());

		if (texture)
			return texture;

		texture = loadTextureFromFile(file);

		if (texture)
		{
			addTexture(texture);
		}

		if (!texture)
			os::Printer::log("[ENGINE] Could not load texture", file->getFileName(), ELL_WARNING);
	}

	return texture;
}

ITexture* Engine::addTexture(const io::path& name, IImage* image)
{
	if (0 == name.size())
	{
		os::Printer::log("[ENGINE] Could not create ITexture, texture needs to have a non-empty name.", ELL_WARNING);
		return 0;
	}

	if (!image)
		return 0;



    ITexture* texture = 0;
    texture = new video::COpenGLTexture(image, name);

    image->drop();

	return texture;


	if (texture)
	{
		addTexture(texture);

	}

	return texture;
}

//! adds a surface, not loaded or created by the Irrlicht Engine
void Engine::addTexture(video::ITexture* texture)
{
	if (texture)
	{

		Textures.push_back(texture);
		Textures.sort();
	}
}


//! looks if the image is already loaded
video::ITexture* Engine::findTexture(const io::path& filename)
{


for (u32 i=0; i<Textures.size(); ++i)
{

		 video::ITexture* tex = Textures[i];
		 //SDL_Log("TEXTURES [%d]  %s %s %s",i,tex->getDebugName(),tex->getName().getInternalName().c_str(),tex->getName().getPath().c_str());
         if (tex->getName().getInternalName()==filename)
         {
         //SDL_Log("Find texture  %s ",tex->getName().getPath().c_str());
         return tex;
         }

}

    //SDL_Log("dont find texture  %s ",filename.c_str());

	return 0;
}
//! opens the file and loads it into the surface
video::ITexture* Engine::loadTextureFromFile(io::IReadFile* file, const io::path& hashName )
{
	ITexture* texture = 0;

	SDL_Log("[ENGINE] Load texture from fille %s ",hashName.c_str());
	IImage* image = createImageFromFile(file);

	if (image)
	{
		// create texture from surface
        texture = new video::COpenGLTexture(image, hashName.size() ? hashName : file->getFileName() );
      //  texture->grab();

		//os::Printer::log("Loaded texture", file->getFileName());
		image->drop();
	//	image=0;
	}

	return texture;
}

video::ITexture* Engine::createTexture(ECOLOR_FORMAT format, const core::dimension2d<u32>& size, void* data, const io::path& name )
{
	ITexture* texture = 0;
	if (data)
	{
        texture = new video::COpenGLTexture(format,size.Width,size.Height,data,name);
        addTexture(texture);
	}

	return texture;
}

video::ITexture* Engine::createTexture(video::IImage* image, const io::path& hashName )
{
	ITexture* texture = 0;
	if (image)
	{
        texture = new video::COpenGLTexture(image,hashName);
        addTexture(texture);
		image->drop();
	}

	return texture;
}




//! Creates a software image from a file.
IImage* Engine::createImageFromFile(io::IReadFile* file)
{
	if (!file)
		return 0;



    if (fileIsExtension(file->getFileName(),"bmp"))
    {
     return IImage::importBMP(file);
    } else
    if (fileIsExtension(file->getFileName(),"pcx"))
    {
    return IImage::importPCX(file);
    } else
    if (fileIsExtension(file->getFileName(),"dds"))
    {
     return IImage::importDDS(file);
    }




    file->seek(0);
    int len =file->getSize();
    file->seek(0);
    u8* input = new u8[len];
	int total = file->read(input, len);






	//SDL_Log("Load image from memory  size: %d total %d",len,total,file->getFileName().c_str());



	int W;
	int H;
	int Format;

	//u8* pixels =stbi_load(file->getFileName().c_str(),&W, &H, &Format,  0);

  	u8* pixels = stbi_load_from_memory(input, len, &W, &H, &Format,  0);



  	if (pixels)
  	{
        int new_width = 1;
		int new_height = 1;
		while( new_width < W )
		{
			new_width *= 2;
		}
		while( new_height < H )
		{
			new_height *= 2;
		}

		if( (new_width != W) || (new_height != H) )
		{

		    u8 *resampled = (unsigned char*)malloc( Format*new_width*new_height );
			up_scale_image(pixels, W, H, Format,resampled, new_width, new_height );

			free( (u8*)pixels );
			pixels = resampled;
			W = new_width;
			H = new_height;
			SDL_LogWarn(0,"[IMAGE] Resample  w:%i h:%i c:%i \n",W,H,Format);
			//os::Printer::log("Resample image .",file->getFileName(), ELL_INFORMATION);
		}
  }

//SDL_Log("[ENGINE] LOAD :  %s   (%d  %d)  format:%d \n",file->getFileName().c_str(), W,H,Format);
video::IImage* image = 0;



	if (Format==4)
	{
		image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(W, H),(u8*)pixels);
    }else
    if (Format==3)
	{
	image = new CImage(ECF_R8G8B8, core::dimension2d<u32>(W, H),(u8*)pixels);

    } else
    if (Format==2)
	{
	image = new CImage(ECF_R8G8, core::dimension2d<u32>(W, H),(u8*)pixels);

    } else
     if (Format==1)
	{
	image = new CImage(ECF_R8, core::dimension2d<u32>(W, H),(u8*)pixels);

    }

         /*  u8 *resampled   = (u8*)malloc( 4 * W * H*sizeof(u8) );
           core::quaternion *PIXELS = core::LoadPixelDataNormalized(pixels,W,H,Format);     // Supports 8 to 32 bit per channel

                    for (int i = 0, k = 0; i < W*H*4; i += 4, k++)
                    {
                        ((u8*)resampled)[i + 0] = (u8)(PIXELS[k].X*255.0f);
                        ((u8*)resampled)[i + 1] = (u8)(PIXELS[k].Y*255.0f);
                        ((u8*)resampled)[i + 2] = (u8)(PIXELS[k].Z*255.0f);
                        ((u8*)resampled)[i + 3] = (u8)(PIXELS[k].W*255.0f);



                    }

      free(pixels);
      image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(W, H),(u8*)resampled);
*/





//	if (input)	delete [] input;



	if (!image)
	{
		os::Printer::log("[ENGINE]  create image struct failure", file->getFileName(), ELL_ERROR);
		return 0;
	}

	return image;
}


//! Creates a software image from a file.
IImage* Engine::createImageFromFile(const io::path& filename)
{
	if (!filename.size())
		return 0;

	IImage* image = 0;
	//SDL_Log("[ENGINE] Load %s ",filename.c_str());
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{

    	image = createImageFromFile(file);
 		file->drop();

	}
	else
		os::Printer::log("[ENGINE] Could not open file of image", filename, ELL_WARNING);



	return image;
}

void Engine::setLookAt(core::vector3df pos,core::vector3df dir,core::vector3df up)
{
    camera_pos=pos;
    camera_dir=dir;
    camera_up=up;
}
void Engine::setCameraFov(float fov)
{
camera_fov=fov;
}
void Engine::setCameraAspect(float aspect)
{
camera_aspect=aspect;
}
float Engine::getCameraFov() {return camera_fov;}
float Engine::getCameraAspect() {return camera_aspect;}

SViewFrustum   Engine::getViewFrustum()
{
return ViewArea;
}

core::vector3df Engine::getCameraPosition() {return camera_pos;}
core::vector3df Engine::getCameraTarget(){return camera_dir;}
core::vector3df Engine::getCameraUp(){return camera_up;}


//! Writes the provided image to disk file
bool Engine::writeImageToFile(IImage* image, const io::path& filename)
{
	io::IWriteFile* file = FileSystem->createAndWriteFile(filename);
	if(!file)
		return false;

	bool result = writeImageToFile(image, file);
	file->drop();

	return result;
}

//! Writes the provided image to a file.
bool Engine::writeImageToFile(IImage* image, io::IWriteFile * file)
{
	if(!file)
		return false;

//SDL_Log("save %s ",file->getFileName().c_str());

u32 pixel_size = image->getBytesPerPixel();
u32 row_stride = (pixel_size * image->getDimension().Width);
stbi_write_png(file->getFileName().c_str(),image->getDimension().Width,image->getDimension().Height,4,image->getData(),image->getImageDataSizeInBytes());

	return true;
}


//! Creates a software image from a byte array.
IImage* Engine::createImageFromData(ECOLOR_FORMAT format,
					const core::dimension2d<u32>& size,
					void *data, bool ownForeignMemory,
					bool deleteMemory)
{

	return new CImage(format, size, data, ownForeignMemory, deleteMemory);
}


//! Creates an empty software image.
IImage* Engine::createImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size)
{

	return new CImage(format, size);
}





//! Creates a software image from part of a texture.
IImage* Engine::createImage(ITexture* texture, const core::position2d<s32>& pos, const core::dimension2d<u32>& size)
{
/*	if ((pos==core::position2di(0,0)) && (size == texture->getSize()))
	{
		IImage* image = new CImage(texture->getColorFormat(), size, texture->lock(ETLM_READ_ONLY), false);
		texture->unlock();
		return image;
	}
	else
	{
		// make sure to avoid buffer overruns
		// make the vector a separate variable for g++ 3.x
		const core::vector2d<u32> leftUpper(core::clamp(static_cast<u32>(pos.X), 0u, texture->getSize().Width),
					core::clamp(static_cast<u32>(pos.Y), 0u, texture->getSize().Height));
		const core::rect<u32> clamped(leftUpper,
					core::dimension2du(core::clamp(static_cast<u32>(size.Width), 0u, texture->getSize().Width),
					core::clamp(static_cast<u32>(size.Height), 0u, texture->getSize().Height)));
		if (!clamped.isValid())
			return 0;
		u8* src = static_cast<u8*>(texture->lock(ETLM_READ_ONLY));
		if (!src)
			return 0;
		IImage* image = new CImage(texture->getColorFormat(), clamped.getSize());
		u8* dst = static_cast<u8*>(image->lock());
		src += clamped.UpperLeftCorner.Y * texture->getPitch() + image->getBytesPerPixel() * clamped.UpperLeftCorner.X;
		for (u32 i=0; i<clamped.getHeight(); ++i)
		{
			video::CColorConverter::convert_viaFormat(src, texture->getColorFormat(), clamped.getWidth(), dst, image->getColorFormat());
			src += texture->getPitch();
			dst += image->getPitch();
		}
		image->unlock();
		texture->unlock();
		return image;
	}*/
}

void Engine::setAlphaFunc(GLenum mode, GLclampf ref)
{
if (AlphaMode != mode || AlphaRef != ref)
	{
//		glAlphaFunc(mode, ref);

		AlphaMode = mode;
		AlphaRef = ref;
	}

}


IShader* Engine::loadShaderFromFile(const char* vertexPath, const char* fragmentPath,const char* ID)
{
    IShader* shader= new IShader(vertexPath,fragmentPath,true);
    Shaders[ID]=shader;
    return shader;
}

IShader* Engine::loadShader(const char* vertexPath, const char* fragmentPath,const char* ID)
{

    SDL_Log("[ENGINE] Add %s shader ",ID);
    IShader* shader=new IShader(vertexPath,fragmentPath,false);
    Shaders[ID]=shader;

    return shader;
}
void Engine::useShader(IShader* shader)
{
     if(!shader) return;
     shader->Use();
     currentShader=shader;

}
void Engine::setShader(IShader* shader)
{
     if(!shader) return;
     shader->Use();
     shader->setMat4("view",getViewMatrix());
     shader->setMat4("projection",getProjectionMatrix());
  /*   if (shader->findUniform("CameraPosition"))
     {
       shader->setVec3("CameraPosition",getCameraPosition());
     }
*/
     currentShader=shader;

}
void Engine::setShader(IShader* shader,core::matrix4 model)
{
     if(!shader) return;
     shader->Use();
     shader->setMat4("view",getViewMatrix());
     shader->setMat4("projection",getProjectionMatrix());
     shader->setMat4("model",model);

     currentShader=shader;

}
void Engine::setShader(IShader* shader,core::stringc uniform_name,core::matrix4 trasform)
{
 if(!shader) return;
     shader->Use();
     shader->setMat4(uniform_name.c_str(),trasform);
     currentShader=shader;
}
void Engine::setShader(IShader* shader,
        core::stringc proj_name,core::matrix4 proj,
        core::stringc view_name,core::matrix4 view,
        core::stringc model_name,core::matrix4 model)
{
 if(!shader) return;
     shader->Use();
     shader->setMat4(view_name.c_str(),view);
     shader->setMat4(proj_name.c_str(),proj);
     shader->setMat4(model_name.c_str(),model);

     currentShader=shader;
}


IShader* Engine::setShader(const char* ID)
{
   IShader* shader =getShader(ID);

   if(shader)
   {
      shader->Use();
     shader->setMat4("view",getViewMatrix());
     shader->setMat4("projection",getProjectionMatrix());
      currentShader=shader;
   }

   return shader;
}
IShader* Engine::getShader(const char* ID)
{


for (const auto& [key, value] : Shaders)
{
         if (strcmp(key.c_str(),ID)==0)
         {
         return value;
         }
}

/*
core::map<const char*, IShader*>::Node* n= Shaders.find(ID);
if(n)
{
 return n->getValue();
}
*/

SDL_Log("[SHADER] Fail find shader %s ",ID);
return 0;
}


double Engine::GetTime(void)
{
return (double)SDL_GetTicks()/1000;
}

float Engine::GetFrameTime(void)
{
return (float)frame;
}


void Engine::setTexture(const video::ITexture* texture)
{

        const COpenGLTexture* tmpTexture =static_cast<const COpenGLTexture*>(texture);

        if (!tmpTexture)
            return;

        GLenum tmpTextureType = tmpTexture->getOpenGLTextureType();
        GLuint tmpTextureName = tmpTexture->getOpenGLTextureId();


        setTexture(tmpTextureType,tmpTextureName);


}
void Engine::setTexture(const video::ITexture* texture,int layer)
{

        const COpenGLTexture* tmpTexture =static_cast<const COpenGLTexture*>(texture);

        if (!tmpTexture)
            return;

        GLenum tmpTextureType = tmpTexture->getOpenGLTextureType();
        GLuint tmpTextureName = tmpTexture->getOpenGLTextureId();


        setTexture(tmpTextureType,tmpTextureName);

        setActiveTexture(GL_TEXTURE0 + layer);


}


		void Engine::setBlendEquation(GLenum mode)
	{
		if (BlendEquation[0] != mode || BlendEquationInvalid)
		{
             glBlendEquation(mode);

			for (GLuint i = 0; i < FrameBufferCount; ++i)
				BlendEquation[i] = mode;

			BlendEquationInvalid = false;
		}
	}



	void Engine::setBlendFunc(GLenum source, GLenum destination)
	{
		if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
			BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination ||
			BlendFuncInvalid)
		{
			glBlendFunc(source, destination);

			for (GLuint i = 0; i < FrameBufferCount; ++i)
			{
				BlendSourceRGB[i] = source;
				BlendDestinationRGB[i] = destination;
				BlendSourceAlpha[i] = source;
				BlendDestinationAlpha[i] = destination;
			}

			BlendFuncInvalid = false;
		}
	}

	void Engine::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
	{
		if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
		{
			if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
				BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha ||
				BlendFuncInvalid)
			{
                 glBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

				for (GLuint i = 0; i < FrameBufferCount; ++i)
				{
					BlendSourceRGB[i] = sourceRGB;
					BlendDestinationRGB[i] = destinationRGB;
					BlendSourceAlpha[i] = sourceAlpha;
					BlendDestinationAlpha[i] = destinationAlpha;
				}

				BlendFuncInvalid = false;
			}
		}
		else
		{
			setBlendFunc(sourceRGB, destinationRGB);
		}
	}


	void Engine::setBlend(bool enable)
	{
		if (Blend[0] != enable || BlendInvalid)
		{
			if (enable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			for (GLuint i = 0; i < FrameBufferCount; ++i)
				Blend[i] = enable;

			BlendInvalid = false;
		}
	}



	// Color Mask.

	void Engine::getColorMask(u8& mask)
	{
		mask = ColorMask[0];
	}

	void Engine::setColorMask(u8 mask)
	{
		if (ColorMask[0] != mask || ColorMaskInvalid)
		{
			glColorMask((mask & ECP_RED) ? GL_TRUE : GL_FALSE, (mask & ECP_GREEN) ? GL_TRUE : GL_FALSE, (mask & ECP_BLUE) ? GL_TRUE : GL_FALSE, (mask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);

			for (GLuint i = 0; i < FrameBufferCount; ++i)
				ColorMask[i] = mask;

			ColorMaskInvalid = false;
		}
	}



	// Cull face calls.

	void Engine::setCullFaceFunc(GLenum mode)
	{
		if (CullFaceMode != mode)
		{
			glCullFace(mode);
			CullFaceMode = mode;
		}
	}

	void Engine::setCullFace(bool enable)
	{
		if (CullFace != enable)
		{
			if (enable)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			CullFace = enable;
		}
	}

	// Depth calls.

	void Engine::setDepthFunc(GLenum mode)
	{
		if (DepthFunc != mode)
		{
			glDepthFunc(mode);
			DepthFunc = mode;
		}
	}

	void Engine::getDepthMask(bool& depth)
	{
		depth = DepthMask;
	}

	void Engine::setDepthMask(bool enable)
	{
		if (DepthMask != enable)
		{
			if (enable)
				glDepthMask(GL_TRUE);
			else
				glDepthMask(GL_FALSE);

			DepthMask = enable;
		}
	}

    void Engine::getDepthTest(bool& enable)
    {
        enable = DepthTest;
    }

	void Engine::setDepthTest(bool enable)
	{
		if (DepthTest != enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			DepthTest = enable;
		}
	}

    void Engine::setStencilTest(bool enable)
	{
		if (StencilTest != enable)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			StencilTest = enable;
		}
	}
	// FBO calls.

	void Engine::getFBO(GLuint& frameBufferID) const
	{
		frameBufferID = FrameBufferID;
	}

	void Engine::setFBO(GLuint frameBufferID)
	{
		if (FrameBufferID != frameBufferID)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
			FrameBufferID = frameBufferID;
		}
	}

	// Shaders calls.

	void Engine::getProgram(GLuint& programID) const
	{
		programID = ProgramID;
	}

	void Engine::setProgram(GLuint programID)
	{
		if (ProgramID != programID)
		{
			glUseProgram(programID);
			ProgramID = programID;
		}
	}

	// Texture calls.

	void Engine::getActiveTexture(GLenum& texture) const
	{
		texture = ActiveTexture;
	}

	void Engine::setActiveTexture(GLenum texture)
	{
		if (ActiveTexture != texture)
		{
			glActiveTexture(texture);
			ActiveTexture = texture;

		}
	}


	void Engine::setTexture(GLuint type,GLuint texture)
	{
		if (CurrTexture != texture)
		{
			glBindTexture(type, texture);
			CurrTexture = texture;

		}
	}
	// Viewport calls.

	void Engine::getViewport(GLint& viewportX, GLint& viewportY, GLsizei& viewportWidth, GLsizei& viewportHeight) const
	{
		viewportX = ViewportX;
		viewportY = ViewportY;
		viewportWidth = ViewportWidth;
		viewportHeight = ViewportHeight;
	}
    void Engine::setViewMatrix(core::vector3df pos,core::vector3df dir,core::vector3df up)
    {
      core::matrix4 vMat;
      vMat.buildCameraLookAtMatrixRH(pos,dir, up);
      setLookAt(pos,dir,up);
      ViewArea.cameraPosition=pos;
      ViewArea.getTransform(video::ETS_VIEW)= vMat;
    }
	void Engine::setViewMatrix(core::matrix4 mat)
	{
	 ViewArea.getTransform(video::ETS_VIEW)= mat;

	}

    void Engine::setProjectionMatrix(core::matrix4 mat)
    {
    	ViewArea.getTransform ( video::ETS_PROJECTION ) = mat;
    }



    core::matrix4 Engine::getViewMatrix()
    {
    	return ViewArea.getTransform ( video::ETS_VIEW );
    }
    core::matrix4 Engine::getProjectionMatrix()
    {
     return ViewArea.getTransform ( video::ETS_PROJECTION );
    }

	void Engine::setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
	{
		if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
		{
			glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
			ViewportX = viewportX;
			ViewportY = viewportY;
			ViewportWidth = viewportWidth;
			ViewportHeight = viewportHeight;
		}
	}
		GLenum Engine::getZBufferBits() const
	{
		// TODO: never used, so not sure what this was really about (zbuffer used by device? Or for RTT's?)

		GLenum bits = 0;
//				bits = GL_DEPTH_COMPONENT24_OES;
				bits = GL_DEPTH_COMPONENT16;

		return bits;
	}

		GLint Engine::getTextureWrapMode(u8 clamp) const
	{
		switch (clamp)
		{
			case ETC_CLAMP:
			case ETC_CLAMP_TO_EDGE:
			case ETC_CLAMP_TO_BORDER:
				return GL_CLAMP_TO_EDGE;
			case ETC_MIRROR:
				return GL_REPEAT;
			default:
				return GL_REPEAT;
		}
    }

    void Engine::removeScenes()
    {
        ISceneList::Iterator it = SceneList.begin();
		for (; it != SceneList.end(); ++it)
		{
				//(*it)->drop();
				IGameScene* n = (IGameScene*)(*it);
				if(n)
				{
                   n->drop();
				}

        }

        SceneList.clear();
    }
     void Engine::removeScene(scene::IGameScene* scene)
    {
        ISceneList::Iterator it = SceneList.begin();
        for (; it != SceneList.end(); ++it)
        {
            if ((*it) == scene)
            {
                (*it)->drop();
                SceneList.erase(it);
                return;
            }
        }
    }

   scene::IGameScene* Engine::createScene(core::stringc name,s32 id)
  {
      scene::IGameScene* scene = new scene::CGameScene(name,id);
      SceneList.push_back(scene);
      return scene;

  }



#define MAX_KEYBOARD_KEYS            512        // Maximum number of keyboard keys supported
#define MAX_MOUSE_BUTTONS              8        // Maximum number of mouse buttons supported
#define MAX_GAMEPADS                   4        // Maximum number of gamepads supported
#define MAX_GAMEPAD_AXIS               8        // Maximum number of axis supported (per gamepad)
#define MAX_GAMEPAD_BUTTONS           32        // Maximum number of buttons supported (per gamepad)
#define MAX_TOUCH_POINTS               8        // Maximum number of touch points supported
#define MAX_KEY_PRESSED_QUEUE         16        // Maximum number of keys in the key input queue
#define MAX_CHAR_PRESSED_QUEUE        16        // Maximum number of characters in the char input queue


typedef struct InputData
{
    struct {
        int exitKey;                    // Default exit key
        char currentKeyState[512];      // Registers current frame key state
        char previousKeyState[512];     // Registers previous frame key state

        int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
        int keyPressedQueueCount;       // Input keys queue count

        int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue
        int charPressedQueueCount;      // Input characters queue count
    } Keyboard;
    struct {
        core::vector2df position;               // Mouse position on screen
        core::vector2df offset;                 // Mouse offset
        core::vector2df scale;                  // Mouse scaling
            core::vector2df currentPosition;        // Mouse position on screen
            core::vector2df previousPosition;       // Previous mouse position


        char currentButtonState[3];     // Registers current mouse button state
        char previousButtonState[3];    // Registers previous mouse button state
        float currentWheelMove;         // Registers current mouse wheel variation
        float previousWheelMove;        // Registers previous mouse wheel variation
    } Mouse;
    struct {
        core::vector2df position[MAX_TOUCH_POINTS];         // Touch position on screen
        char currentTouchState[MAX_TOUCH_POINTS];   // Registers current touch state
        char previousTouchState[MAX_TOUCH_POINTS];  // Registers previous touch state

           int pointCount;                             // Number of touch points active
          int pointId[MAX_TOUCH_POINTS];              // Point identifiers

    } Touch;


} InputData;


static InputData Input = { 0 };               // Global CORE state context



//----------------------------------------------------------------------------------
// Module Functions Definition - Input (Keyboard, Mouse, Gamepad) Functions
//----------------------------------------------------------------------------------
// Check if a key has been pressed once
bool Engine::IsKeyPressed(int key)
{
    bool pressed = false;

    if ((Input.Keyboard.previousKeyState[key] == 0) && (Input.Keyboard.currentKeyState[key] == 1)) pressed = true;
    else pressed = false;

    return pressed;
}

// Check if a key is being pressed (key held down)
bool Engine::IsKeyDown(int key)
{
    if (Input.Keyboard.currentKeyState[key] == 1) return true;
    else return false;
}

// Check if a key has been released once
bool Engine::IsKeyReleased(int key)
{
    bool released = false;

    if ((Input.Keyboard.previousKeyState[key] == 1) && (Input.Keyboard.currentKeyState[key] == 0)) released = true;
    else released = false;

    return released;
}

// Check if a key is NOT being pressed (key not held down)
bool Engine::IsKeyUp(int key)
{
    if (Input.Keyboard.currentKeyState[key] == 0) return true;
    else return false;
}

// Get the last key pressed
int Engine::GetKeyPressed(void)
{
    int value = 0;

    if (Input.Keyboard.keyPressedQueueCount > 0)
    {
        // Get character from the queue head
        value = Input.Keyboard.keyPressedQueue[0];

        // Shift elements 1 step toward the head.
        for (int i = 0; i < (Input.Keyboard.keyPressedQueueCount - 1); i++)
            Input.Keyboard.keyPressedQueue[i] = Input.Keyboard.keyPressedQueue[i + 1];

        // Reset last character in the queue
        Input.Keyboard.keyPressedQueue[Input.Keyboard.keyPressedQueueCount] = 0;
        Input.Keyboard.keyPressedQueueCount--;
    }

    return value;
}

// Get the last char pressed
int Engine::GetCharPressed(void)
{
    int value = 0;

    if (Input.Keyboard.charPressedQueueCount > 0)
    {
        // Get character from the queue head
        value = Input.Keyboard.charPressedQueue[0];

        // Shift elements 1 step toward the head.
        for (int i = 0; i < (Input.Keyboard.charPressedQueueCount - 1); i++)
            Input.Keyboard.charPressedQueue[i] = Input.Keyboard.charPressedQueue[i + 1];

        // Reset last character in the queue
        Input.Keyboard.charPressedQueue[Input.Keyboard.charPressedQueueCount] = 0;
        Input.Keyboard.charPressedQueueCount--;
    }

    return value;
}

// Set a custom key to exit program
// NOTE: default exitKey is ESCAPE
void Engine::SetExitKey(int key)
{
    Input.Keyboard.exitKey = key;

}



// Check if a mouse button has been pressed once
bool Engine::IsMouseButtonPressed(int button)
{
    bool pressed = false;

    if ((Input.Mouse.currentButtonState[button] == 1) && (Input.Mouse.previousButtonState[button] == 0)) pressed = true;

    // Map touches to mouse buttons checking
    if ((Input.Touch.currentTouchState[button] == 1) && (Input.Touch.previousTouchState[button] == 0)) pressed = true;

    return pressed;
}

// Check if a mouse button is being pressed
bool Engine::IsMouseButtonDown(int button)
{
    bool down = false;

    if (Input.Mouse.currentButtonState[button] == 1) down = true;

    // Map touches to mouse buttons checking
    if (Input.Touch.currentTouchState[button] == 1) down = true;

    return down;
}

// Check if a mouse button has been released once
bool Engine::IsMouseButtonReleased(int button)
{
    bool released = false;

    if ((Input.Mouse.currentButtonState[button] == 0) && (Input.Mouse.previousButtonState[button] == 1)) released = true;

    // Map touches to mouse buttons checking
    if ((Input.Touch.currentTouchState[button] == 0) && (Input.Touch.previousTouchState[button] == 1)) released = true;

    return released;
}

// Check if a mouse button is NOT being pressed
bool Engine::IsMouseButtonUp(int button)
{
    return !IsMouseButtonDown(button);
}

// Get mouse position X
int Engine::GetMouseX(void)
{
    return (int)((Input.Mouse.currentPosition.X + Input.Mouse.offset.X)*Input.Mouse.scale.X);

}

// Get mouse position Y
int Engine::GetMouseY(void)
{
    return (int)((Input.Mouse.currentPosition.Y + Input.Mouse.offset.Y)*Input.Mouse.scale.Y);

}

// Get mouse position XY
core::vector2df Engine::GetMousePosition(void)
{
    core::vector2df position =core::vector2df(0,0);

    position.X = (Input.Mouse.currentPosition.X + Input.Mouse.offset.X)*Input.Mouse.scale.X;
    position.Y = (Input.Mouse.currentPosition.Y + Input.Mouse.offset.Y)*Input.Mouse.scale.Y;


    return position;
}

// Get mouse delta between frames
core::vector2df Engine::GetMouseDelta(void)
{
    core::vector2df delta = core::vector2df(0,0);

    delta.X = Input.Mouse.currentPosition.X - Input.Mouse.previousPosition.X;
    delta.Y = Input.Mouse.currentPosition.Y - Input.Mouse.previousPosition.Y;

    return delta;
}

// Set mouse position XY
void Engine::SetMousePosition(int x, int y)
{
 Input.Mouse.currentPosition = core::vector2df((float)x, (float)y );

}

// Set mouse offset
// NOTE: Useful when rendering to different size targets
void Engine::SetMouseOffset(int offsetX, int offsetY)
{
    Input.Mouse.offset = core::vector2df((float)offsetX, (float)offsetY );
}

// Set mouse scaling
// NOTE: Useful when rendering to different size targets
void Engine::SetMouseScale(float scaleX, float scaleY)
{
    Input.Mouse.scale = core::vector2df( scaleX, scaleY );
}

// Get mouse wheel movement Y
float GetMouseWheelMove(void)
{

    return Input.Mouse.previousWheelMove;
}



// Get touch position X for touch point 0 (relative to screen size)
int Engine::GetTouchX(void)
{
    return (int)Input.Touch.position[0].X;

}

// Get touch position Y for touch point 0 (relative to screen size)
int Engine::GetTouchY(void)
{
    return (int)Input.Touch.position[0].Y;

}

// Get touch position XY for a touch point index (relative to screen size)
// TODO: Touch position should be scaled depending on display size and render size
core::vector2df Engine::GetTouchPosition(int index)
{
    core::vector2df position = core::vector2df( -1.0f, -1.0f );

    if (index < MAX_TOUCH_POINTS) position = Input.Touch.position[index];


   /* if ((CORE.Window.screen.width > CORE.Window.display.width) || (CORE.Window.screen.height > CORE.Window.display.height))
    {
        position.x = position.x*((float)CORE.Window.screen.width/(float)(CORE.Window.display.width - CORE.Window.renderOffset.x)) - CORE.Window.renderOffset.x/2;
        position.y = position.y*((float)CORE.Window.screen.height/(float)(CORE.Window.display.height - CORE.Window.renderOffset.y)) - CORE.Window.renderOffset.y/2;
    }
    else
    {
        position.x = position.x*((float)CORE.Window.render.width/(float)CORE.Window.display.width) - CORE.Window.renderOffset.x/2;
        position.y = position.y*((float)CORE.Window.render.height/(float)CORE.Window.display.height) - CORE.Window.renderOffset.y/2;
    }*/

    return position;
}

// Get touch point identifier for given index
int Engine::GetTouchPointId(int index)
{
    int id = -1;

    if (index < MAX_TOUCH_POINTS) id = Input.Touch.pointId[index];

    return id;
}

// Get number of touch points
int Engine::GetTouchPointCount(void)
{
    return Input.Touch.pointCount;
}



void Engine::InputInit()
{
//    Input.Keyboard.exitKey = KEY_ESCAPE;
    Input.Mouse.scale = core::vector2df(1.0f, 1.0f );
     // Reset keys/chars pressed registered
    Input.Keyboard.keyPressedQueueCount = 0;
    Input.Keyboard.charPressedQueueCount = 0;

       for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
    {
        Input.Touch.position[i].X = -1;
        Input.Touch.position[i].Y = -1;
        }


    // Reset keyboard key state
    for (int i = 0; i < 512; i++)
    {
    Input.Keyboard.currentKeyState[i] = 0;
    Input.Keyboard.previousKeyState[i] = 0;
    Input.Keyboard.keyPressedQueue[i] = 0;


    }
}


 void Engine::CharCallback( int key)
{
   // SDL_Log( "Char Callback: KEY:%i(%c)", key, key);
    if (Input.Keyboard.charPressedQueueCount < MAX_KEY_PRESSED_QUEUE)
    {
        Input.Keyboard.charPressedQueue[Input.Keyboard.charPressedQueueCount] = key;
        Input.Keyboard.charPressedQueueCount++;
    }
}


 void Engine::KeyCallback( int scancode, int key, int action, int mods)
{


//    SDL_Log( "Key Callback: KEY:%i(%c) SDL KEY:%i(%c) - SCANCODE:%i (STATE:%i)", key, key,index,index, scancode, action);
      // int keycode =get_sdl_to_glfw(event->key.keysym.scancode);
       //if(core.key)  core.key(core.window,keycode,event->key.keysym.sym,GLFW_PRESS,event->key.keysym.mod);


        // WARNING: GLFW could return GLFW_REPEAT, we need to consider it as 1
        // to work properly with our implementation (IsKeyDown/IsKeyUp checks)
        if (action == 0) Input.Keyboard.currentKeyState[scancode] = 0;
        else Input.Keyboard.currentKeyState[scancode] = 1;


        // Check if there is space available in the key queue
        if ((Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == 0))
        {
            // Add character to the queue
            Input.Keyboard.keyPressedQueue[Input.Keyboard.keyPressedQueueCount] = scancode;
            Input.Keyboard.keyPressedQueueCount++;
        }

       CharCallback(key);


}





 void Engine::MouseButtonCallback( int button, int action, int mods)
{
    Input.Mouse.currentButtonState[button] = action;

}


 void Engine::MouseCursorPosCallback( double x, double y)
{
    Input.Mouse.position.X = (float)x;
    Input.Mouse.position.Y = (float)y;
    Input.Touch.position[0] = Input.Mouse.position;

}
void Engine::MouseScrollCallback(double xoffset, double yoffset)
{
    Input.Mouse.currentWheelMove += (float)yoffset;
}

   bool Engine::ShouldClose()
   {
    Input.Keyboard.keyPressedQueueCount = 0;
    Input.Keyboard.charPressedQueueCount = 0;

    for (int i = 0; i < 512; i++) Input.Keyboard.previousKeyState[i] = Input.Keyboard.currentKeyState[i];

    // Register previous mouse states
    for (int i = 0; i < 3; i++) Input.Mouse.previousButtonState[i] = Input.Mouse.currentButtonState[i];

    // Register previous mouse wheel state
    Input.Mouse.previousWheelMove = Input.Mouse.currentWheelMove;
    Input.Mouse.currentWheelMove = 0.0f;

     return shouldClose;
   }

  void Engine::PollInputEvents()
  {




    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) Input.Touch.previousTouchState[i] = Input.Touch.currentTouchState[i];


               SDL_Event e;
                while( SDL_PollEvent( &e ) != 0 )
                {


                    switch(e.type)
                    {
                      case SDL_QUIT:
                        {
                          shouldClose=true;
                        }
                        break;

                            case SDL_WINDOWEVENT:
                            {


                             if( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                            {
                                int w = e.window.data1;
                                int h = e.window.data2;



                            }
                              if( e.window.event == SDL_WINDOWEVENT_RESIZED )
                            {
                                int w = e.window.data1;
                                int h = e.window.data2;


                            }

                            }
                            break;
                       case SDL_KEYDOWN:
                       {
                            KeyCallback(e.key.keysym.scancode,e.key.keysym.sym,1,e.key.keysym.mod);



                       }
                       break;
                       case SDL_KEYUP:
                       {
                       KeyCallback(e.key.keysym.scancode,e.key.keysym.sym,0,e.key.keysym.mod);
                       }
                       break;
                       case SDL_TEXTINPUT:
                       {
                       }
                       break;
                        case SDL_MOUSEWHEEL:
                        {

                          MouseScrollCallback(e.wheel.x,e.wheel.y);
                        }
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                        {

                         MouseButtonCallback(e.button.button-1,1,0);
                         MouseCursorPosCallback((double)e.button.x,(double)e.button.y);


                        }
                        break;
                        case SDL_MOUSEBUTTONUP:
                        {
                         MouseButtonCallback(e.button.button-1,0,0);
                         MouseCursorPosCallback(e.button.x,e.button.y);


                        }
                        break;
                        case SDL_MOUSEMOTION:
                        {

                         MouseCursorPosCallback(e.button.x,e.button.y);






                        }
                        break;

                        case SDL_MULTIGESTURE:
                            {
                            float touchLocation_x;
                            float touchLocation_y;

                                 int b=(int) e.tfinger.fingerId;
                                if( fabs( e.mgesture.dTheta ) > 3.14 / 180.0 )
                                {
                                  //  touchLocation_x = e.mgesture.x * CORE.Window.screen.width;
                                 //   touchLocation_y = e.mgesture.y * CORE.Window.screen.height;


//
                                }
                                 //Pinch detected
                                else if( fabs( e.mgesture.dDist ) > 0.002 )
                                {
                                 //   touchLocation_x = e.mgesture.x * CORE.Window.screen.width;
                                 //   touchLocation_y = e.mgesture.y * CORE.Window.screen.height;

                                    //Pinch open
                                    if( e.mgesture.dDist > 0 )
                                    {

                                    }
                                    //Pinch close
                                    else
                                    {


                                    }

                                }

                            }
                        break;
                        case SDL_FINGERDOWN:
                            {
                            int b=(int) e.tfinger.fingerId;
                          //  int x= (int)e.tfinger.x * CORE.Window.screen.width;
                         //   int y= (int)e.tfinger.y * CORE.Window.screen.height;


                            }
                        break;
                        case SDL_FINGERUP:
                            {
                            int b=(int) e.tfinger.fingerId;
                          //   int x= (int)e.tfinger.x * CORE.Window.screen.width;
                           // int y= (int)e.tfinger.y * CORE.Window.screen.height;


                            }
                        break;
                        case SDL_FINGERMOTION:
                            {

                          //  int x= (int)e.tfinger.x * CORE.Window.screen.width;
                          //  int y= (int)e.tfinger.y * CORE.Window.screen.height;

                            }
                        break;
                   }
                }



  }


  int Engine::PollEvent(SDL_Event* e)
  {





    // Reset keys/chars pressed registered
    Input.Keyboard.keyPressedQueueCount = 0;
    Input.Keyboard.charPressedQueueCount = 0;




//#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    // Keyboard/Mouse input polling (automatically managed by GLFW3 through callback)

    // Register previous keys states
    for (int i = 0; i < 512; i++) Input.Keyboard.previousKeyState[i] = Input.Keyboard.currentKeyState[i];

    // Register previous mouse states
    for (int i = 0; i < 3; i++) Input.Mouse.previousButtonState[i] = Input.Mouse.currentButtonState[i];

    // Register previous mouse wheel state
    Input.Mouse.previousWheelMove = Input.Mouse.currentWheelMove;
    Input.Mouse.currentWheelMove = 0.0f;
//#endif

    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) Input.Touch.previousTouchState[i] = Input.Touch.currentTouchState[i];





                    SDL_WaitEvent(e);
                    switch(e->type)
                    {
                      case SDL_QUIT:
                            {
                              shouldClose=true;
                              break;
                            }


                            case SDL_WINDOWEVENT:
                            {


                             if( e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                            {
                                int w = e->window.data1;
                                int h = e->window.data2;



                            }
                              if( e->window.event == SDL_WINDOWEVENT_RESIZED )
                            {
                                int w = e->window.data1;
                                int h = e->window.data2;


                            }

                            }
                           break;
                       case SDL_KEYDOWN:
                       {
                        KeyCallback(e->key.keysym.scancode,e->key.keysym.sym,1,e->key.keysym.mod);
                       }
                    break;;
                       case SDL_KEYUP:
                       {
                       KeyCallback(e->key.keysym.scancode,e->key.keysym.sym,0,e->key.keysym.mod);
                       }
                   break;
                       case SDL_TEXTINPUT:
                       {
                       }
                       break;
                        case SDL_MOUSEWHEEL:
                        {

                          Input.Mouse.previousWheelMove+=e->wheel.y;
                        }
                    break;
                        case SDL_MOUSEBUTTONDOWN:
                        {

                         MouseButtonCallback(e->button.button-1,1,0);
                         MouseCursorPosCallback((double)e->button.x,(double)e->button.y);


                        }
                      break;
                        case SDL_MOUSEBUTTONUP:
                        {
                         MouseButtonCallback(e->button.button-1,0,0);
                         MouseCursorPosCallback(e->button.x,e->button.y);


                        }
                      break;
                        case SDL_MOUSEMOTION:
                        {

                         MouseCursorPosCallback(e->button.x,e->button.y);

                        }
                    break;

                        case SDL_MULTIGESTURE:
                            {
                            float touchLocation_x;
                            float touchLocation_y;

                                 int b=(int) e->tfinger.fingerId;
                                if( fabs( e->mgesture.dTheta ) > 3.14 / 180.0 )
                                {
                                  //  touchLocation_x = e.mgesture.x * CORE.Window.screen.width;
                                 //   touchLocation_y = e.mgesture.y * CORE.Window.screen.height;


//
                                }
                                 //Pinch detected
                                else if( fabs( e->mgesture.dDist ) > 0.002 )
                                {
                                 //   touchLocation_x = e.mgesture.x * CORE.Window.screen.width;
                                 //   touchLocation_y = e.mgesture.y * CORE.Window.screen.height;

                                    //Pinch open
                                    if( e->mgesture.dDist > 0 )
                                    {

                                    }
                                    //Pinch close
                                    else
                                    {


                                    }

                                }

                            }
                    break;
                        case SDL_FINGERDOWN:
                            {
                            int b=(int) e->tfinger.fingerId;
                          //  int x= (int)e.tfinger.x * CORE.Window.screen.width;
                         //   int y= (int)e.tfinger.y * CORE.Window.screen.height;


                            }
                        break;
                        case SDL_FINGERUP:
                            {
                            int b=(int) e->tfinger.fingerId;
                          //   int x= (int)e.tfinger.x * CORE.Window.screen.width;
                           // int y= (int)e.tfinger.y * CORE.Window.screen.height;


                            }
                       break;
                        case SDL_FINGERMOTION:
                            {

                          //  int x= (int)e.tfinger.x * CORE.Window.screen.width;
                          //  int y= (int)e.tfinger.y * CORE.Window.screen.height;

                            }
                        break;
                   }

   return 0;

  }




}


namespace video
{


typedef struct rlglData {
    rlRenderBatch *currentBatch;            // Current render batch
    rlRenderBatch defaultBatch;             // Default internal render batch

    struct {
        int currentMatrixMode;              // Current matrix mode
        core::matrix4 *currentMatrix;            // Current matrix pointer
        core::matrix4 modelview;                 // Default modelview matrix
        core::matrix4 projection;                // Default projection matrix
        core::matrix4 transform;                 // Transform matrix to be used with rlTranslate, rlRotate, rlScale
        bool transformRequired;             // Require transform matrix application to current draw-call vertex (if required)
        core::matrix4 stack[RL_MAX_MATRIX_STACK_SIZE];// Matrix stack for push/pop
        int stackCounter;                   // Matrix stack counter

        unsigned int defaultTextureId;      // Default texture used on shapes/poly drawing (required by shader)
        unsigned int activeTextureId[RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS];    // Active texture ids to be enabled on batch drawing (0 active by default)

        scene::IShader* shader;


        int currentBlendMode;               // Blending mode active
        int glBlendSrcFactor;               // Blending source factor
        int glBlendDstFactor;               // Blending destination factor
        int glBlendEquation;                // Blending equation


    } State;            // Renderer state

} rlglData;
static rlglData RLGL = { 0 };



// Render batch management
//------------------------------------------------------------------------------------------------
// Load render batch
rlRenderBatch rlLoadRenderBatch(int numBuffers, int bufferElements)
{
    rlRenderBatch batch = { 0 };

    // Initialize CPU (RAM) vertex buffers (position, texcoord, color data and indexes)
    //--------------------------------------------------------------------------------------------
    batch.vertexBuffer = (rlVertexBuffer *)malloc(numBuffers*sizeof(rlVertexBuffer));

    for (int i = 0; i < numBuffers; i++)
    {
        batch.vertexBuffer[i].elementCount = bufferElements;

        batch.vertexBuffer[i].vertices = (float *)malloc(bufferElements*3*4*sizeof(float));        // 3 float by vertex, 4 vertex by quad
        batch.vertexBuffer[i].texcoords = (float *)malloc(bufferElements*2*4*sizeof(float));       // 2 float by texcoord, 4 texcoord by quad
        batch.vertexBuffer[i].colors = (unsigned char *)malloc(bufferElements*4*4*sizeof(unsigned char));   // 4 float by color, 4 colors by quad
        batch.vertexBuffer[i].indices = (unsigned short *)malloc(bufferElements*6*sizeof(unsigned short));  // 6 int by quad (indices)

        for (int j = 0; j < (3*4*bufferElements); j++) batch.vertexBuffer[i].vertices[j] = 0.0f;
        for (int j = 0; j < (2*4*bufferElements); j++) batch.vertexBuffer[i].texcoords[j] = 0.0f;
        for (int j = 0; j < (4*4*bufferElements); j++) batch.vertexBuffer[i].colors[j] = 0;

        int k = 0;

        // Indices can be initialized right now
        for (int j = 0; j < (6*bufferElements); j += 6)
        {
            batch.vertexBuffer[i].indices[j] = 4*k;
            batch.vertexBuffer[i].indices[j + 1] = 4*k + 1;
            batch.vertexBuffer[i].indices[j + 2] = 4*k + 2;
            batch.vertexBuffer[i].indices[j + 3] = 4*k;
            batch.vertexBuffer[i].indices[j + 4] = 4*k + 2;
            batch.vertexBuffer[i].indices[j + 5] = 4*k + 3;

            k++;
        }

        batch.vertexBuffer[i].vCounter = 0;
        batch.vertexBuffer[i].tcCounter = 0;
        batch.vertexBuffer[i].cCounter = 0;
    }

    //--------------------------------------------------------------------------------------------

    // Upload to GPU (VRAM) vertex data and initialize VAOs/VBOs
    //--------------------------------------------------------------------------------------------
    for (int i = 0; i < numBuffers; i++)
    {
            // Initialize Quads VAO
            glGenVertexArrays(1, &batch.vertexBuffer[i].vaoId);
            glBindVertexArray(batch.vertexBuffer[i].vaoId);


        // Quads - Vertex buffers binding and attributes enable
        // Vertex position buffer (shader-location = 0)
        glGenBuffers(1, &batch.vertexBuffer[i].vboId[0]);
        glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[0]);
        glBufferData(GL_ARRAY_BUFFER, bufferElements*3*4*sizeof(float), batch.vertexBuffer[i].vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

        // Vertex texcoord buffer (shader-location = 1)
        glGenBuffers(1, &batch.vertexBuffer[i].vboId[1]);
        glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[1]);
        glBufferData(GL_ARRAY_BUFFER, bufferElements*2*4*sizeof(float), batch.vertexBuffer[i].texcoords, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);

        // Vertex color buffer (shader-location = 2)
        glGenBuffers(1, &batch.vertexBuffer[i].vboId[2]);
        glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[2]);
        glBufferData(GL_ARRAY_BUFFER, bufferElements*4*4*sizeof(unsigned char), batch.vertexBuffer[i].colors, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);



        // Fill index buffer
        glGenBuffers(1, &batch.vertexBuffer[i].eboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vertexBuffer[i].eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferElements*6*sizeof(short), batch.vertexBuffer[i].indices, GL_STATIC_DRAW);
    }

    SDL_LogInfo(0, "RLGL: Render batch vertex buffers loaded successfully in VRAM (GPU)");

    // Unbind the current VAO
    glBindVertexArray(0);
    //--------------------------------------------------------------------------------------------

    // Init draw calls tracking system
    //--------------------------------------------------------------------------------------------
    batch.draws = (rlDrawCall *)malloc(RL_DEFAULT_BATCH_DRAWCALLS*sizeof(rlDrawCall));

    for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++)
    {
        batch.draws[i].mode = RL_QUADS;
        batch.draws[i].vertexCount = 0;
        batch.draws[i].vertexAlignment = 0;
        batch.draws[i].textureId = RLGL.State.defaultTextureId;
    }

    batch.bufferCount = numBuffers;    // Record buffer count
    batch.drawCounter = 1;             // Reset draws counter
    batch.currentDepth = -1.0f;         // Reset depth value
    //--------------------------------------------------------------------------------------------

    return batch;
}

// Unload default internal buffers vertex data from CPU and GPU
void rlUnloadRenderBatch(rlRenderBatch batch)
{
    // Unbind everything
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Unload all vertex buffers data
    for (int i = 0; i < batch.bufferCount; i++)
    {
        // Delete VBOs from GPU (VRAM)
        glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[0]);
        glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[1]);
        glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[2]);

        glDeleteBuffers(1, &batch.vertexBuffer[i].eboId);

        // Delete VAOs from GPU (VRAM)
         glDeleteVertexArrays(1, &batch.vertexBuffer[i].vaoId);

        // Free vertex arrays memory from CPU (RAM)
        free(batch.vertexBuffer[i].vertices);
        free(batch.vertexBuffer[i].texcoords);
        free(batch.vertexBuffer[i].colors);
        free(batch.vertexBuffer[i].indices);
    }

    // Unload arrays
    free(batch.vertexBuffer);
    free(batch.draws);

}
static core::matrix4 rlMatrixIdentity(void)
{
    core::matrix4 result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    return result;
}

// Get two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
static core::matrix4 rlMatrixMultiply(core::matrix4 left, core::matrix4 right)
{
    core::matrix4  result;
    result[0]  = left[0]*right[0] + left[1] *right[4]  + left[2] *right[8]  + left[3] *right[12];
    result[1]  = left[0]*right[1]  + left[1] *right[5]  + left[2] *right[9]  + left[3] *right[13];
    result[2]  = left[0]*right[2]  + left[1] *right[6]  + left[2] *right[10]  + left[3] *right[14];
    result[3]  = left[0]*right[3]  + left[1] *right[7]  + left[2] *right[11]  + left[3] *right[15];

    result[4]  = left[4] *right[0] + left[5] *right[4]  + left[6] *right[8]  + left[7] *right[12];
    result[5]  = left[4] *right[1]  + left[5] *right[5]  + left[6] *right[9]  + left[7] *right[13];
    result[6]  = left[4] *right[2]  + left[5] *right[6]  + left[6] *right[10]  + left[7] *right[14];
    result[7]  = left[4] *right[3]  + left[5] *right[7]  + left[6] *right[11]  + left[7] *right[15];

    result[8]  = left[8] *right[0] + left[9] *right[4]  + left[10] *right[8]  + left[11] *right[12];
    result[9]  = left[8] *right[1]  + left[9] *right[5]  + left[10] *right[9]  + left[11] *right[13];
    result[10]  = left[8] *right[2]  + left[9] *right[6]  + left[10] *right[10]  + left[11] *right[14];
    result[11]  = left[8] *right[3]  + left[9] *right[7]  + left[10] *right[11]  + left[11] *right[15];

    result[12]  = left[12] *right[0] + left[13] *right[4]  + left[14] *right[8]  + left[15] *right[12];
    result[13]  = left[12] *right[1]  + left[13] *right[5]  + left[14] *right[9]  + left[15] *right[13];
    result[14]  = left[12] *right[2]  + left[13] *right[6]  + left[14] *right[10]  + left[15] *right[14];
    result[15]  = left[12] *right[3]  + left[13] *right[7]  + left[14] *right[11]  + left[15] *right[15];

    return result;
}

void rlMatrixMode(int mode)
{
    if (mode == RL_PROJECTION) RLGL.State.currentMatrix = &RLGL.State.projection;
    else if (mode == RL_MODELVIEW) RLGL.State.currentMatrix = &RLGL.State.modelview;
    RLGL.State.currentMatrixMode = mode;
}

// Push the current matrix into RLGL.State.stack
void rlPushMatrix(void)
{
    if (RLGL.State.stackCounter >= RL_MAX_MATRIX_STACK_SIZE) SDL_LogError(0, "RLGL: Matrix stack overflow (RL_MAX_MATRIX_STACK_SIZE)");

    if (RLGL.State.currentMatrixMode == RL_MODELVIEW)
    {
        RLGL.State.transformRequired = true;
        RLGL.State.currentMatrix = &RLGL.State.transform;
    }

    RLGL.State.stack[RLGL.State.stackCounter] = *RLGL.State.currentMatrix;
    RLGL.State.stackCounter++;
}

// Pop lattest inserted matrix from RLGL.State.stack
void rlPopMatrix(void)
{
    if (RLGL.State.stackCounter > 0)
    {
        core::matrix4 mat = RLGL.State.stack[RLGL.State.stackCounter - 1];
        *RLGL.State.currentMatrix = mat;
        RLGL.State.stackCounter--;
    }

    if ((RLGL.State.stackCounter == 0) && (RLGL.State.currentMatrixMode == RL_MODELVIEW))
    {
        RLGL.State.currentMatrix = &RLGL.State.modelview;
        RLGL.State.transformRequired = false;
    }
}

// Reset current matrix to identity matrix
void rlLoadIdentity(void)
{

     *RLGL.State.currentMatrix= {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

// Multiply the current matrix by a translation matrix
void rlTranslatef(float x, float y, float z)
{
 core::matrix4 matTranslation= {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
 matTranslation.setTranslation(core::vector3df(x,y,z));


  // NOTE: We transpose matrix with multiplication order
*RLGL.State.currentMatrix = rlMatrixMultiply(matTranslation, *RLGL.State.currentMatrix);

}

// Multiply the current matrix by a rotation matrix
// NOTE: The provided angle must be in degrees
void rlRotatef(float angle, float x, float y, float z)
{

    core::matrix4 matRotation=rlMatrixIdentity();

    // Axis vector (x, y, z) normalization
    float lengthSquared = x*x + y*y + z*z;
    if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f))
    {
        float inverseLength = 1.0f/sqrtf(lengthSquared);
        x *= inverseLength;
        y *= inverseLength;
        z *= inverseLength;
    }

    // Rotation matrix generation
    float sinres = sinf(DEG2RAD*angle);
    float cosres = cosf(DEG2RAD*angle);
    float t = 1.0f - cosres;



    matRotation[0] = x*x*t + cosres;
    matRotation[1] = y*x*t + z*sinres;
    matRotation[2] = z*x*t - y*sinres;
    matRotation[3] = 0.0f;

    matRotation[4] = x*y*t - z*sinres;
    matRotation[5] = y*y*t + cosres;
    matRotation[6] = z*y*t + x*sinres;
    matRotation[7] = 0.0f;

    matRotation[8] = x*z*t + y*sinres;
    matRotation[9] = y*z*t - x*sinres;
    matRotation[10] = z*z*t + cosres;
    matRotation[11] = 0.0f;

    matRotation[12] = 0.0f;
    matRotation[13] = 0.0f;
    matRotation[14] = 0.0f;
    matRotation[15] = 1.0f;



    // NOTE: We transpose matrix with multiplication order
    *RLGL.State.currentMatrix = rlMatrixMultiply(matRotation, *RLGL.State.currentMatrix);

}

// Multiply the current matrix by a scaling matrix
void rlScalef(float x, float y, float z)
{

       core::matrix4 matScale= {
        x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };




    // NOTE: We transpose matrix with multiplication order
      *RLGL.State.currentMatrix = rlMatrixMultiply(matScale, *RLGL.State.currentMatrix);
}

// Multiply the current matrix by another matrix
void rlMultMatrixf(core::matrix4 mat)
{

    *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix, mat);
}

// Multiply the current matrix by a perspective matrix generated by parameters
void rlFrustum(double left, double right, double bottom, double top, double znear, double zfar)
{
//    *RLGL.State.currentMatrix = scene::Engine::Instance()->getProjectionMatrix()  * (*RLGL.State.currentMatrix);
    *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix,  scene::Engine::Instance()->getProjectionMatrix());
}

// Multiply the current matrix by an orthographic matrix generated by parameters
void rlOrtho(double left, double right, double bottom, double top, double znear, double zfar)
{

    *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix,  scene::Engine::Instance()->getProjectionMatrix());
}
void BeginModeBatch2D()
{
    RLGL.State.shader=scene::Engine::Instance()->getShader("Batch");
    scene::Engine* engine = scene::Engine::Instance();
    engine->setDepthTest(false);
    engine->setBlend(true);
    engine->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    engine->useShader(RLGL.State.shader);

    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)


    *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix,  scene::Engine::Instance()->getProjectionMatrix());


    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    // Setup Camera view
   // Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    rlMultMatrixf(scene::Engine::Instance()->getViewMatrix());      // Multiply modelview matrix by view matrix (camera)

    //rlEnableDepthTest();            // Enable DEPTH_TEST for 3D
}
void BeginModeBatch3D()
{

    RLGL.State.shader=scene::Engine::Instance()->getShader("Batch");
    scene::Engine* engine = scene::Engine::Instance();

    video::SMaterial m;
    m.Lighting = false;

    engine->setMaterial(video::SMaterial());

    //engine->setDepthTest(true);
   // engine->setBlend(false);
    //engine->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




    engine->useShader(RLGL.State.shader);

    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)


    *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix,  scene::Engine::Instance()->getProjectionMatrix());


    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    // Setup Camera view
   // Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    rlMultMatrixf(scene::Engine::Instance()->getViewMatrix());      // Multiply modelview matrix by view matrix (camera)

    //rlEnableDepthTest();            // Enable DEPTH_TEST for 3D
}

void EndRenderBatch(void)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPopMatrix();                  // Restore previous matrix (projection) from matrix stack

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

  //  rlMultMatrixf(MatrixToFloat(CORE.Window.screenScale)); // Apply screen scaling if required


}

// NOTE: We require a pointer to reset batch and increase current buffer (multi-buffer)
void rlDrawRenderBatch(rlRenderBatch *batch)
{
    // Update batch vertex buffers
    //------------------------------------------------------------------------------------------------------------
    // NOTE: If there is not vertex data, buffers doesn't need to be updated (vertexCount > 0)
    // TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (change flag required)
    if (batch->vertexBuffer[batch->currentBuffer].vCounter > 0)
    {
        // Activate elements VAO
        glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);

        // Vertex positions buffer
        glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertexBuffer[batch->currentBuffer].vCounter*3*sizeof(float), batch->vertexBuffer[batch->currentBuffer].vertices);

        // Texture coordinates buffer
        glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertexBuffer[batch->currentBuffer].vCounter*2*sizeof(float), batch->vertexBuffer[batch->currentBuffer].texcoords);

        // Colors buffer
        glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertexBuffer[batch->currentBuffer].vCounter*4*sizeof(unsigned char), batch->vertexBuffer[batch->currentBuffer].colors);


         glBindVertexArray(0);
    }
    //------------------------------------------------------------------------------------------------------------



      core::matrix4 matProjection = RLGL.State.projection;
      core::matrix4 matModelView  = RLGL.State.modelview;



        // Draw buffers
        if (batch->vertexBuffer[batch->currentBuffer].vCounter > 0)
        {




            // Create modelview-projection matrix and upload to shader
          //  core::matrix4 matMVP = rlMatrixMultiply(RLGL.State.modelview, RLGL.State.projection);
            core::matrix4 matMVP = rlMatrixMultiply(matModelView, matProjection);


            RLGL.State.shader->setMat4("mvp",matMVP);
/*            RLGL.State.shader->Use();
            RLGL.State.shader->setMat4("view",RLGL.State.modelview);
            RLGL.State.shader->setMat4("projection",RLGL.State.projection);
             RLGL.State.shader->setMat4("model",rlMatrixIdentity());*/


             glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);


            // Activate additional sampler textures
            // Those additional textures will be common for all draw calls of the batch
            for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++)
            {
                if (RLGL.State.activeTextureId[i] > 0)
                {
                    //glActiveTexture(GL_TEXTURE0 + 1 + i);

                    scene::Engine::Instance()->setActiveTexture(GL_TEXTURE0 + 1 + i);

                    scene::Engine::Instance()->setTexture(GL_TEXTURE_2D,RLGL.State.activeTextureId[i]);

                   // glBindTexture(GL_TEXTURE_2D, RLGL.State.activeTextureId[i]);
                    //glBindTexture(GL_TEXTURE_2D, 0);
                }
            }

            // Activate default sampler2D texture0 (one texture is always active for default batch shader)
            // NOTE: Batch system accumulates calls by texture0 changes, additional textures are enabled for all the draw calls
            //glActiveTexture(GL_TEXTURE0);
            scene::Engine::Instance()->setActiveTexture(GL_TEXTURE0 );

            for (int i = 0, vertexOffset = 0; i < batch->drawCounter; i++)
            {
                // Bind current draw call texture, activated as GL_TEXTURE0 and binded to sampler2D texture0 by default
                //glBindTexture(GL_TEXTURE_2D, batch->draws[i].textureId);
                scene::Engine::Instance()->setTexture(GL_TEXTURE_2D,batch->draws[i].textureId);
                //glBindTexture(GL_TEXTURE_2D, 0);

                if ((batch->draws[i].mode == RL_LINES) || (batch->draws[i].mode == RL_TRIANGLES)) glDrawArrays(batch->draws[i].mode, vertexOffset, batch->draws[i].vertexCount);
                else
                {
                    glDrawElements(GL_TRIANGLES, batch->draws[i].vertexCount/4*6, GL_UNSIGNED_SHORT, (GLvoid *)(vertexOffset/4*6*sizeof(GLushort)));
                }

                vertexOffset += (batch->draws[i].vertexCount + batch->draws[i].vertexAlignment);
            }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
              //  glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
        }

     glBindVertexArray(0); // Unbind VAO



    //------------------------------------------------------------------------------------------------------------

    // Reset batch buffers
    //------------------------------------------------------------------------------------------------------------
    // Reset vertex counters for next frame
    batch->vertexBuffer[batch->currentBuffer].vCounter = 0;
    batch->vertexBuffer[batch->currentBuffer].tcCounter = 0;
    batch->vertexBuffer[batch->currentBuffer].cCounter = 0;

    // Reset depth for next draw
    batch->currentDepth = -1.0f;

    // Restore projection/modelview matrices
    RLGL.State.projection = matProjection;
    RLGL.State.modelview = matModelView;

    // Reset RLGL.currentBatch->draws array
    for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++)
    {
        batch->draws[i].mode = RL_QUADS;
        batch->draws[i].vertexCount = 0;
        batch->draws[i].textureId = RLGL.State.defaultTextureId;
    }

    // Reset active texture units for next batch
    for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) RLGL.State.activeTextureId[i] = 0;

    // Reset draws counter to one draw for the batch
    batch->drawCounter = 1;
    //------------------------------------------------------------------------------------------------------------

    // Change to next buffer in the list (in case of multi-buffering)
    batch->currentBuffer++;
    if (batch->currentBuffer >= batch->bufferCount) batch->currentBuffer = 0;

}


// Set the active render batch for rlgl
void rlSetRenderBatchActive(rlRenderBatch *batch)
{
    rlDrawRenderBatch(RLGL.currentBatch);

    if (batch != NULL) RLGL.currentBatch = batch;
    else RLGL.currentBatch = &RLGL.defaultBatch;

}

// Update and draw internal render batch
void rlDrawRenderBatchActive(void)
{


    rlDrawRenderBatch(RLGL.currentBatch);

}



// Check internal buffer overflow for a given number of vertex
// and force a rlRenderBatch draw call if required
bool rlCheckRenderBatchLimit(int vCount)
{
    bool overflow = false;
    if ((RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter + vCount) >=
        (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4))
    {
        overflow = true;
        rlDrawRenderBatch(RLGL.currentBatch);    // NOTE: Stereo rendering is checked inside
    }


    return overflow;
}



// Initialize drawing mode (how to organize vertex)
void rlBegin(int mode)
{
    // Draw mode can be RL_LINES, RL_TRIANGLES and RL_QUADS
    // NOTE: In all three cases, vertex are accumulated over default internal vertex buffer
    if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode != mode)
    {
        if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount > 0)
        {
            // Make sure current RLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_LINES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount : RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4);
            else if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4)));
            else RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

            if (!rlCheckRenderBatchLimit(RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment))
            {
                RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;
                RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;
                RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;

                RLGL.currentBatch->drawCounter++;
            }
        }

        if (RLGL.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLGL.currentBatch);

        RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode = mode;
        RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount = 0;
        RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId = RLGL.State.defaultTextureId;
    }
}

// Finish vertex providing
void rlEnd(void)
{
    // Make sure vertexCount is the same for vertices, texcoords, colors and normals
    // NOTE: In OpenGL 1.1, one glColor call can be made for all the subsequent glVertex calls

    // Make sure colors count match vertex count
    if (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter != RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter)
    {
        int addColors = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter - RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter;

        for (int i = 0; i < addColors; i++)
        {
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter] = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter - 4];
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 1] = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter - 3];
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 2] = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter - 2];
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 3] = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter - 1];
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter++;
        }
    }

    // Make sure texcoords count match vertex count
    if (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter != RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter)
    {
        int addTexCoords = RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter - RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter;

        for (int i = 0; i < addTexCoords; i++)
        {
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter] = 0.0f;
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter + 1] = 0.0f;
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter++;
        }
    }

    // TODO: Make sure normals count match vertex count... if normals support is added in a future... :P

    // NOTE: Depth increment is dependant on rlOrtho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
    RLGL.currentBatch->currentDepth += (1.0f/20000.0f);

    // Verify internal buffers limits
    // NOTE: This check is combined with usage of rlCheckRenderBatchLimit()
    if ((RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter) >=
        (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4 - 4))
    {
        // WARNING: If we are between rlPushMatrix() and rlPopMatrix() and we need to force a rlDrawRenderBatch(),
        // we need to call rlPopMatrix() before to recover *RLGL.State.currentMatrix (RLGL.State.modelview) for the next forced draw call!
        // If we have multiple matrix pushed, it will require "RLGL.State.stackCounter" pops before launching the draw
        for (int i = RLGL.State.stackCounter; i >= 0; i--) rlPopMatrix();
        rlDrawRenderBatch(RLGL.currentBatch);
    }
}

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
void rlVertex3f(float x, float y, float z)
{
    float tx = x;
    float ty = y;
    float tz = z;

    // Transform provided vector if required
    if (RLGL.State.transformRequired)
    {
     // RLGL.State.transform.transformVect(pos,tras);

        tx = RLGL.State.transform[0]*x + RLGL.State.transform[4]*y + RLGL.State.transform[8]*z + RLGL.State.transform[12];
        ty = RLGL.State.transform[1]*x + RLGL.State.transform[5]*y + RLGL.State.transform[9]*z + RLGL.State.transform[13];
        tz = RLGL.State.transform[2]*x + RLGL.State.transform[6]*y + RLGL.State.transform[10]*z + RLGL.State.transform[14];

    }

    // Verify that current vertex buffer elements limit has not been reached
    if (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter < (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4))
    {
        RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter] = tx;
        RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter + 1] = ty;
        RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter + 2] = tz;
        RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter++;

        RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount++;
    }
    else SDL_LogError(0, "RLGL: Batch elements overflow");
}

// Define one vertex (position)
void rlVertex2f(float x, float y)
{
    rlVertex3f(x, y, RLGL.currentBatch->currentDepth);
}

// Define one vertex (position)
void rlVertex2i(int x, int y)
{
    rlVertex3f((float)x, (float)y, RLGL.currentBatch->currentDepth);
}

// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void rlTexCoord2f(float x, float y)
{
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter] = x;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter + 1] = y;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter++;
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
void rlNormal3f(float x, float y, float z)
{
    // TODO: Normals usage...
}

// Define one vertex (color)
void rlColor4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter] = x;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 1] = y;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 2] = z;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter + 3] = w;
    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter++;
}

// Define one vertex (color)
void rlColor4f(float r, float g, float b, float a)
{
    rlColor4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

// Define one vertex (color)
void rlColor3f(float x, float y, float z)
{
    rlColor4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}
void rlSetTexture(unsigned int id)
{
    if (id == 0)
    {
        // NOTE: If quads batch limit is reached, we force a draw call and next batch starts
        if (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter >=
            RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4)
        {
            rlDrawRenderBatch(RLGL.currentBatch);
        }
    }
    else
    {
        if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId != id)
        {
            if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount > 0)
            {
                // Make sure current RLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
                // that way, following QUADS drawing will keep aligned with index processing
                // It implies adding some extra alignment vertex at the end of the draw,
                // those vertex are not processed but they are considered as an additional offset
                // for the next set of vertex to be drawn
                if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_LINES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount : RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4);
                else if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4)));
                else RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

                if (!rlCheckRenderBatchLimit(RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment))
                {
                    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;
                    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].cCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;
                    RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].tcCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;

                    RLGL.currentBatch->drawCounter++;
                }
            }

            if (RLGL.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLGL.currentBatch);

            RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId = id;
            RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount = 0;
        }

    }
}
void rlClose()
{
 rlUnloadRenderBatch(RLGL.defaultBatch);
}
void rlInit()
{

    unsigned char pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)
//    video::IImage* image = new CImage(video::ECF_A8R8G8B8,1,1,pixels);

    video::COpenGLTexture* texture = new COpenGLTexture(video::ECF_A8R8G8B8,1,1,pixels,"white");
    scene::Engine::Instance()->addTexture(texture);

    RLGL.State.defaultTextureId = texture->getOpenGLTextureId();

    if (RLGL.State.defaultTextureId != 0) SDL_LogInfo(0, "TEXTURE: [ID %i] Default texture loaded successfully", RLGL.State.defaultTextureId);
    else  SDL_LogWarn(0, "TEXTURE: Failed to load default texture");


    // Init default vertex arrays buffers
    RLGL.defaultBatch = rlLoadRenderBatch(RL_DEFAULT_BATCH_BUFFERS, RL_DEFAULT_BATCH_BUFFER_ELEMENTS);
    RLGL.currentBatch = &RLGL.defaultBatch;

    RLGL.State.shader=scene::Engine::Instance()->getShader("Batch");
    scene::Engine::Instance()->useShader(RLGL.State.shader);


    // Init stack matrices (emulating OpenGL 1.1)
    for (int i = 0; i < RL_MAX_MATRIX_STACK_SIZE; i++) RLGL.State.stack[i] = rlMatrixIdentity();

    // Init internal matrices
    RLGL.State.transform = rlMatrixIdentity();
    RLGL.State.projection= rlMatrixIdentity();
    RLGL.State.modelview = rlMatrixIdentity();
    RLGL.State.currentMatrix = &RLGL.State.modelview;
}

void DrawLine3D(core::vector3df startPos, core::vector3df endPos, video::SColor color)
{
    // WARNING: Be careful with internal buffer vertex alignment
    // when using RL_LINES or RL_TRIANGLES, data is aligned to fit
    // lines-triangles-quads in the same indexed buffers!!!
    rlCheckRenderBatchLimit(8);

    rlBegin(RL_LINES);
        rlColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
        rlVertex3f(startPos.X, startPos.Y, startPos.Z);
        rlVertex3f(endPos.X, endPos.Y, endPos.Z);
    rlEnd();
}

void DrawLine3D(float x1,float y1,float z1, float x2,float y2,float z2, video::SColor color)
{
    rlCheckRenderBatchLimit(8);

    rlBegin(RL_LINES);
        rlColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
        rlVertex3f(x1,y1,z1);
        rlVertex3f(x2,y2,z2);
    rlEnd();
}

void DrawTriangle3D(core::vector3df  v1, core::vector3df  v2, core::vector3df  v3, video::SColor color)
{
    rlCheckRenderBatchLimit(3);

    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
        rlVertex3f(v1.X, v1.Y, v1.Z);
        rlVertex3f(v2.X, v2.Y, v2.Z);
        rlVertex3f(v3.X, v3.Y, v3.Z);
    rlEnd();
}

void DrawTriangle3DLines(core::vector3df a,core::vector3df b,core::vector3df c, SColor color)
{


    DrawLine3D(a,b,color);
    DrawLine3D(b,c,color);
    DrawLine3D(c,a,color);



}

void DrawSphereBasic(video::SColor color,int rings ,int slices)
{
    // Make sure there is enough space in the internal render batch
    // buffer to store all required vertex, batch is reseted if required
    rlCheckRenderBatchLimit((rings + 2)*slices*6);

    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

        for (int i = 0; i < (rings + 2); i++)
        {
            for (int j = 0; j < slices; j++)
            {
                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*i))*sinf(DEG2RAD*(j*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*i))*cosf(DEG2RAD*(j*360/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(DEG2RAD*((j+1)*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(DEG2RAD*((j+1)*360/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(DEG2RAD*(j*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(DEG2RAD*(j*360/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*i))*sinf(DEG2RAD*(j*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*i))*cosf(DEG2RAD*(j*360/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*(i)))*sinf(DEG2RAD*((j+1)*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*(i))),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*(i)))*cosf(DEG2RAD*((j+1)*360/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(DEG2RAD*((j+1)*360/slices)),
                           sinf(DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(DEG2RAD*((j+1)*360/slices)));
            }
        }
    rlEnd();
}

// Draw a grid centered at (0, 0, 0)
void Draw3DGrid(int slices, float spacing)
{
    int halfSlices = slices/2;


DrawLine3D(0, 0.1f, 0 ,0, 1.0f, 0 ,video::SColor(255,255,0,0) );
DrawLine3D(0, 0.1f, 0 ,1.0f, 0.1f, 0 ,video::SColor(255,0,0,255) );
DrawLine3D(0, 0.1f, 0 ,0, 0.1f, 1.0f ,video::SColor(255,0,255,0) );




    rlCheckRenderBatchLimit((slices + 2)*4);

    rlBegin(RL_LINES);
        for (int i = -halfSlices; i <= halfSlices; i++)
        {
            if (i == 0)
            {
                rlColor3f(0.5f, 0.5f, 0.5f);
                rlColor3f(0.5f, 0.5f, 0.5f);
                rlColor3f(0.5f, 0.5f, 0.5f);
                rlColor3f(0.5f, 0.5f, 0.5f);
            }
            else
            {
                rlColor3f(0.75f, 0.75f, 0.75f);
                rlColor3f(0.75f, 0.75f, 0.75f);
                rlColor3f(0.75f, 0.75f, 0.75f);
                rlColor3f(0.75f, 0.75f, 0.75f);
            }

            rlVertex3f((float)i*spacing, 0.0f, (float)-halfSlices*spacing);
            rlVertex3f((float)i*spacing, 0.0f, (float)halfSlices*spacing);

            rlVertex3f((float)-halfSlices*spacing, 0.0f, (float)i*spacing);
            rlVertex3f((float)halfSlices*spacing, 0.0f, (float)i*spacing);
        }
    rlEnd();
}

// Draw a circle in 3D world space
void DrawCircle3D(core::vector3df center, float radius, core::vector3df rotationAxis, float rotationAngle, SColor color)
{
    rlCheckRenderBatchLimit(2*36);

    rlPushMatrix();
        rlTranslatef(center.X, center.Y, center.Z);
        rlRotatef(rotationAngle, rotationAxis.X, rotationAxis.Y, rotationAxis.Z);

        rlBegin(RL_LINES);
            for (int i = 0; i < 360; i += 10)
            {
                rlColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

                rlVertex3f(sinf(DEG2RAD*i)*radius, cosf(DEG2RAD*i)*radius, 0.0f);
                rlVertex3f(sinf(DEG2RAD*(i + 10))*radius, cosf(DEG2RAD*(i + 10))*radius, 0.0f);
            }
        rlEnd();
    rlPopMatrix();
}



}// end namespace video

} // end namespace irr


#if defined(_IRR_WINDOWS_API_) && !defined(_IRR_STATIC_LIB_)

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
	// _crtBreakAlloc = 139;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			#if defined(_DEBUG) && !defined(__GNUWIN32__) && !defined(__BORLANDC__) && !defined (_WIN32_WCE) && !defined (_IRR_XBOX_PLATFORM_)
				_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
			#endif
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#endif // defined(_IRR_WINDOWS_)

