#ifndef ENGINE_H
#define ENGINE_H


#include "CImage.h"
#include "CShader.h"
#include "COpenGLTexture.h"
#include "IGeometryCreator.h"
#include "IVertexBuffer.h"
#include "CFPSCounter.h"

#include "os.h"
#include "IrrCompileConfig.h"
#include "aabbox3d.h"
#include "IMeshBuffer.h"
#include "coreutil.h"
#include "SColor.h"


#include "dimension2d.h"
#include "IEngineEnums.h"
#include "SViewFrustum.h"
#include "IGameScene.h"
#include "fast_atof.h"
#include "heapsort.h"
#include "IAnimatedMesh.h"
#include "IFileList.h"
#include "IFileSystem.h"
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



namespace irr
{



namespace video
{

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rlVertexBuffer {
    int elementCount;           // Number of elements in the buffer (QUADS)

    int vCounter;               // Vertex position counter to process (and draw) from full buffer
    int nCounter;               // Vertex position counter to process (and draw) from full buffer
    int tcCounter;              // Vertex texcoord counter to process (and draw) from full buffer
    int cCounter;               // Vertex color counter to process (and draw) from full buffer

    float *vertices;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *normals;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;           // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    unsigned char *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    unsigned short *indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
    unsigned int vaoId;         // OpenGL Vertex Array Object id
    unsigned int eboId;
    unsigned int vboId[4];      // OpenGL Vertex Buffer Objects id (4 types of vertex data)
} rlVertexBuffer;


typedef struct rlDrawCall {
    int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
    int vertexCount;            // Number of vertex of the draw
    int vertexAlignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
    unsigned int textureId;     // Texture id to be used on the draw -> Use to create new draw call if changes
} rlDrawCall;

// rlRenderBatch type
typedef struct rlRenderBatch {
    int bufferCount;           // Number of vertex buffers (multi-buffering support)
    int currentBuffer;          // Current buffer tracking in case of multi-buffering
    rlVertexBuffer *vertexBuffer; // Dynamic buffer(s) for vertex data

    rlDrawCall *draws;          // Draw calls array, depends on textureId
    int drawCounter;            // Draw calls counter
    float currentDepth;         // Current depth value for next draw
} rlRenderBatch;
//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Default internal render batch elements limits
#define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   2048
#define RL_DEFAULT_BATCH_BUFFERS            1      // Default number of batch buffers (multi-buffering)
#define RL_DEFAULT_BATCH_DRAWCALLS        256      // Default number of batch draw calls (by state changes: mode, texture)
#define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS  4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
#define RL_MAX_MATRIX_STACK_SIZE           32      // Maximum size of Matrix stack
#define RL_MODELVIEW                            0x1700      // GL_MODELVIEW
#define RL_PROJECTION                           0x1701      // GL_PROJECTION
#define RL_TEXTURE                              0x1702      // GL_TEXTURE

// Primitive assembly draw modes
#define RL_LINES                                0x0001      // GL_LINES
#define RL_TRIANGLES                            0x0004      // GL_TRIANGLES
#define RL_QUADS                                0x0007      // GL_QUADS

void rlClose();
void rlInit();

void BeginModeBatch3D();
void BeginModeBatch2D();
void EndRenderBatch(void);

void RenderBatch(void);//no matrix trasform

 void rlMatrixMode(int mode);                    // Choose the current matrix to be transformed
 void rlPushMatrix(void);                        // Push the current matrix to stack
 void rlPopMatrix(void);                         // Pop lattest inserted matrix from stack
 void rlLoadIdentity(void);                      // Reset current matrix to identity matrix
 void rlTranslatef(float x, float y, float z);   // Multiply the current matrix by a translation matrix
 void rlRotatef(float angle, float x, float y, float z);  // Multiply the current matrix by a rotation matrix
 void rlScalef(float x, float y, float z);       // Multiply the current matrix by a scaling matrix
 void rlMultMatrixf(core::matrix4 mat);               // Multiply the current matrix by another matrix
 void rlFrustum(double left, double right, double bottom, double top, double znear, double zfar);
 void rlOrtho(double left, double right, double bottom, double top, double znear, double zfar);
 void rlViewport(int x, int y, int width, int height); // Set the viewport area
 void rlBegin(int mode);                         // Initialize drawing mode (how to organize vertex)
 void rlEnd(void);                               // Finish vertex providing
 void rlVertex2i(int x, int y);                  // Define one vertex (position) - 2 int
 void rlVertex2f(float x, float y);              // Define one vertex (position) - 2 float
 void rlVertex3f(float x, float y, float z);     // Define one vertex (position) - 3 float
 void rlTexCoord2f(float x, float y);            // Define one vertex (texture coordinate) - 2 float
 void rlNormal3f(float x, float y, float z);     // Define one vertex (normal) - 3 float
 void rlColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);  // Define one vertex (color) - 4 byte
 void rlColor3f(float x, float y, float z);          // Define one vertex (color) - 3 float
 void rlColor4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float


rlRenderBatch rlLoadRenderBatch(int numBuffers, int bufferElements);  // Load a render batch system
 void rlUnloadRenderBatch(rlRenderBatch batch);                        // Unload render batch system
 void rlDrawRenderBatch(rlRenderBatch *batch);                         // Draw render batch data (Update->Draw->Reset)
 void rlSetRenderBatchActive(rlRenderBatch *batch);                    // Set the active render batch for rlgl (NULL for default internal)
 void rlDrawRenderBatchActive(void);                                   // Update and draw internal render batch
 bool rlCheckRenderBatchLimit(int vCount);                             // Check internal buffer overflow for a given number of vertex
 void rlSetTexture(unsigned int id);           // Set current texture for render batch and check buffers limits

 void DrawLine3D(core::vector3df startPos, core::vector3df endPos, video::SColor color);
 void DrawLine3D(float x1,float y1,float z1, float x2,float y2,float z2, video::SColor color);
 void DrawTriangle3D(core::vector3df  v1, core::vector3df  v2, core::vector3df  v3, video::SColor color);
 void DrawTriangle3DLines(core::vector3df a,core::vector3df b,core::vector3df c, SColor color);
 void DrawSphereBasic(video::SColor color,int rings = 16,int slices = 16);
 void Draw3DGrid(int slices, float spacing);
 void DrawCircle3D(core::vector3df center, float radius, core::vector3df rotationAxis, float rotationAngle, SColor color);
}


namespace scene
{

using namespace  video;
using namespace io;

typedef struct Vertex_s
{
 float x;
 float y;
 float z;
 unsigned char r;
 unsigned char g;
 unsigned char b;
} Vertex;

typedef struct
{
 float x;
 float y;
 float z;
 float u;
 float v;

 unsigned char r;
 unsigned char g;
 unsigned char b;
 unsigned char a;
} Vertex2D;



//*********************************************



class Engine : public  IReferenceCounted
{
    public:
        Engine(int width, int heigth,bool vsync, const char* title);
        virtual ~Engine();

    static Engine* Instance();

    static Engine* createEngine(int width, int heigth,bool vsync, const char* title);


    scene::IGameScene* createScene(core::stringc name,s32 id);
    void removeScene(scene::IGameScene* scene);





    void ShowSimpleMessageBox(Uint32 flags, const char *title, const char *message);

    double GetTime(void);
    float GetFrameTime(void);
    void SetTargetFPS(int fps);
    int GetFPS(void);
    void Wait(float ms);

    int PollEvent(SDL_Event * event);
    void PollInputEvents();

    bool ShouldClose();

    void KeyCallback( int scancode, int key, int action, int mods);
    void MouseButtonCallback( int button, int action, int mods);
    void MouseCursorPosCallback( double x, double y);
    void MouseScrollCallback(double xoffset, double yoffset);


    // Input-related functions: keyboard
    bool IsKeyPressed(int key);                             // Detect if a key has been pressed once
    bool IsKeyDown(int key);                                // Detect if a key is being pressed
    bool IsKeyReleased(int key);                            // Detect if a key has been released once
    bool IsKeyUp(int key);                                  // Detect if a key is NOT being pressed
    void SetExitKey(int key);                               // Set a custom key to exit program (default is ESC)
    int GetKeyPressed(void);                                // Get key pressed (keycode), call it multiple times for keys queued
    int GetCharPressed(void);                               // Get char pressed (unicode), call it multiple times for chars queued


    // Input-related functions: mouse
    bool IsMouseButtonPressed(int button);                  // Detect if a mouse button has been pressed once
    bool IsMouseButtonDown(int button);                     // Detect if a mouse button is being pressed
    bool IsMouseButtonReleased(int button);                 // Detect if a mouse button has been released once
    bool IsMouseButtonUp(int button);                       // Detect if a mouse button is NOT being pressed
    int GetMouseX(void);                                    // Returns mouse position X
    int GetMouseY(void);                                    // Returns mouse position Y
    core::vector2df GetMouseDelta(void);
    core::vector2df GetMousePosition(void);                         // Returns mouse position XY
    void SetMousePosition(int x, int y);                    // Set mouse position XY
    void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
    void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
    float GetMouseWheelMove(void);                          // Returns mouse wheel movement Y
    void SetMouseCursor(int cursor);                        // Set mouse cursor

    // Input-related functions: touch
    int GetTouchX(void);                                    // Returns touch position X for touch point 0 (relative to screen size)
    int GetTouchY(void);                                    // Returns touch position Y for touch point 0 (relative to screen size)
    core::vector2df GetTouchPosition(int index);                    // Returns touch position XY for a touch point index (relative to screen size)
    int GetTouchPointId(int index);
    int GetTouchPointCount(void);


    void ClearScene(u16 clearFlag, SColor color = SColor(255,0,0,45), f32 depth = 1.f, u8 stencil = 0);
    void BeginScene();
    void EndScene();
    void Swap();
    bool testGLError(int code=0);

    void renderMesh(IMesh* mesh);
    void renderMesh(IMesh* mesh,IShader* shader);
    void renderMesh(IMesh* mesh,IShader* shader,core::matrix4 model);

    void renderMeshBuffer(IMeshBuffer* meshBuffer,bool useMaterial);


    void setAlphaFunc(GLenum mode, GLclampf ref);
    void setAlphaTest(bool enable);


    void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial, bool resetAllRenderStates);
    void setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates);


	u32 getMaximalPrimitiveCount() const{		return 65535;	}


	void DrawGrid(int slices, float spacing);
	void Draw3DTriangleLines(core::vector3df a,core::vector3df b,core::vector3df c,SColor color);
	void draw3DTriangle(core::vector3df a,core::vector3df b,core::vector3df c,SColor color);
	void draw3DTriangle(const core::triangle3df& triangle, SColor color);
	void Draw3DLine(core::vector3df a,core::vector3df b, SColor color);
	void Draw3DBox(const core::aabbox3d<f32>& box, SColor color);
	void DrawMeshBufferNormals(const scene::IMeshBuffer* mb, f32 length, SColor color);
	void DrawMeshBufferNormals(core::matrix4 mat,const scene::IMeshBuffer* mb, f32 length, SColor color);



    void setViewMatrix(core::matrix4 mat);
    void setViewMatrix(core::vector3df pos,core::vector3df dir,core::vector3df up);
    void setProjectionMatrix(core::matrix4 mat);
    void setLookAt(core::vector3df pos,core::vector3df dir,core::vector3df up);

    core::matrix4 getViewMatrix();
    core::matrix4 getProjectionMatrix();

      SViewFrustum   getViewFrustum();

    core::vector3df getCameraPosition();
    core::vector3df getCameraTarget();
    core::vector3df getCameraUp();
    float getCameraFov() ;
    float getCameraAspect();


    void setCameraFov(float fov);
    void setCameraAspect(float aspect);


    void setBlendEquation(GLenum mode);
	void setBlendFunc(GLenum source, GLenum destination);
	void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);
	void setBlend(bool enable);

	void setStencilTest(bool enable);
	void startStencilTest();
	void endStencilTest();
	// Color Mask.

	void getColorMask(u8& mask);
	void setColorMask(u8 mask);

	// Cull face calls.

	void setCullFaceFunc(GLenum mode);
	void setCullFace(bool enable);
	// Depth calls.

	void setDepthFunc(GLenum mode);
	void getDepthMask(bool& depth);
	void setDepthMask(bool enable);
    void getDepthTest(bool& enable);
	void setDepthTest(bool enable);


	// FBO calls.

	void getFBO(GLuint& frameBufferID) const;

	void setFBO(GLuint frameBufferID);

	// Shaders calls.

	void getProgram(GLuint& programID) const;

	void setProgram(GLuint programID);

	// Texture calls.

	void getActiveTexture(GLenum& texture) const;
	void setActiveTexture(GLenum texture);
	void setTexture(GLuint type,GLuint texture);
	void setTexture(const video::ITexture* texture,int layer);
	void setTexture(const video::ITexture* texture);

	// Viewport calls.

	void getViewport(GLint& viewportX, GLint& viewportY, GLsizei& viewportWidth, GLsizei& viewportHeight) const;
	void setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight);



	IFileSystem*      getFileSystem(){return FileSystem;};
    IMeshManipulator* getMeshManipulator(){return MeshManipulator;};
    IGeometryCreator* getGeometryCreator(){return GeometryCreator;};


    const core::dimension2d<u32>& getScreenSize() {return ScreenSize;}
	const core::dimension2d<u32>& getCurrentRenderTargetSize() {return renderTargetSize;}

		bool getWriteZBuffer(const SMaterial& material) const
		{
		return 	 material.ZWriteEnable && !material.isTransparent();
		}
		GLenum getGLBlend(E_BLEND_FACTOR factor) const
	{
		static GLenum const blendTable[] =
		{
			GL_ZERO,
			GL_ONE,
			GL_DST_COLOR,
			GL_ONE_MINUS_DST_COLOR,
			GL_SRC_COLOR,
			GL_ONE_MINUS_SRC_COLOR,
			GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA,
			GL_DST_ALPHA,
			GL_ONE_MINUS_DST_ALPHA,
			GL_SRC_ALPHA_SATURATE
		};

		return blendTable[factor];
	}

	GLenum getZBufferBits() const;
	GLint getTextureWrapMode(u8 clamp) const;

    void setMaterial(const SMaterial& material);

    //! deletes all textures
		void deleteAllTextures();

		video::ITexture* createTexture(ECOLOR_FORMAT format, const core::dimension2d<u32>& size, void* data, const io::path& hashName );
		video::ITexture* createTexture(video::IImage* image, const io::path& hashName );

		//! opens the file and loads it into the surface
		video::ITexture* loadTextureFromFile(io::IReadFile* file, const io::path& hashName = "");


		//! adds a surface, not loaded or created by the Irrlicht Engine
		void addTexture(video::ITexture* surface);

        ITexture* addTexture(const io::path& name, IImage* image);

		//! looks if the image is already loaded
		 video::ITexture* findTexture(const io::path& filename);
		//! Removes a texture from the texture cache and deletes it, freeing lot of
		//! memory.
		 void removeTexture(ITexture* texture);

		//! Removes all texture from the texture cache and deletes them, freeing lot of
		//! memory.
		 void removeAllTextures();



		 void drawStencilShadow(bool clearStencilBuffer=true);




		//! loads a Texture
		 ITexture* getTexture(const io::path& filename);

		//! loads a Texture
		 ITexture* getTexture(io::IReadFile* file);

		//! Returns a texture by index
		 ITexture* getTextureByIndex(u32 index);

		//! Returns amount of textures currently loaded
		 u32 getTextureCount() const;

		//! Renames a texture
		 void renameTexture(ITexture* texture, const io::path& newName);



		 bool exportImageToBMP(io::IWriteFile* file, IImage* image);
		 bool exportImageToPCX(io::IWriteFile *file, IImage *image);

		 IImage* importPCXImage(io::IReadFile* file) ;
		 IImage* importBMPImage(io::IReadFile* file) ;

		 		//! Creates a software image from a file.
		 IImage* createImageFromFile(const io::path& filename);

		//! Creates a software image from a file.
		 IImage* createImageFromFile(io::IReadFile* file);

		  IImage* createImageFromData(ECOLOR_FORMAT format,
			const core::dimension2d<u32>& size, void *data,
			bool ownForeignMemory=true, bool deleteForeignMemory = true);

		//! Creates an empty software image.
		 IImage* createImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size);

		 IImage* createImage(ITexture* texture, const core::position2d<s32>& pos, const core::dimension2d<u32>& size);


        bool writeImageToFile(IImage* image, const io::path& filename);
        bool writeImageToFile(IImage* image, io::IWriteFile * file);


        IShader* loadShaderFromFile(const char* vertexPath, const char* fragmentPath,const char* ID);
        IShader* loadShader(const char* vertexPath, const char* fragmentPath,const char* ID);
        IShader* getShader(const char* ID);
        IShader* getShader(int ID);

        //! Aply current shader with no transformation matrix
        void useShader(IShader* shader);
        //! Aply current shader with projection e view.
        void setShader(IShader* shader);
        //! Aply current shader with projection e view.
        void setShader(IShader* shader,core::matrix4 model);
        void setShader(IShader* shader,core::stringc uniform_name,core::matrix4 trasform);
        void setShader(IShader* shader,
                       core::stringc proj_name,core::matrix4 proj,
                       core::stringc view_name,core::matrix4 view,
                       core::stringc model_name,core::matrix4 model);

        IShader* setShader(const char* ID);


        void setWindowCaption(const char* title);


    protected:

    void removeScenes();

    void InputInit();
    void InputUpdate();
    void CharCallback( int key);


    private:



    core::list<scene::IGameScene*> SceneList;

    IFileSystem      *FileSystem;
    IMeshManipulator *MeshManipulator;
    IGeometryCreator *GeometryCreator;

    double current;                     // Current time measure
	double previous;                    // Previous time measure
	double update;                      // Time measure for frame update
	double draw;                        // Time measure for frame draw
	double frame;                       // Time measure for one frame


	//gl states
			float DimAliasedLine[2];
			float DimAliasedPoint[2];
    	GLenum AlphaMode;
		GLclampf AlphaRef;
		bool AlphaTest;
		GLenum MatrixMode;
		GLenum ClientActiveTexture;




	GLuint FrameBufferCount;

	GLenum* BlendEquation;
	GLenum* BlendSourceRGB;
	GLenum* BlendDestinationRGB;
	GLenum* BlendSourceAlpha;
	GLenum* BlendDestinationAlpha;
	bool* Blend;
	bool BlendEquationInvalid;
	bool BlendFuncInvalid;
	bool BlendInvalid;


	u8* ColorMask;
	bool ColorMaskInvalid;

	GLenum CullFaceMode;
	bool CullFace;

	GLenum DepthFunc;
	bool DepthMask;
	bool DepthTest;
	bool StencilTest;
    u8 MaxAnisotropicFilter;

	GLuint FrameBufferID;

	GLuint ProgramID;

	GLenum ActiveTexture;
	GLenum CurrTexture;
	SMaterial currMaterial,LastMaterial;

	GLint ViewportX;
	GLint ViewportY;
	GLsizei ViewportWidth;
	GLsizei ViewportHeight;

    core::array<ITexture*> Textures;
    //core::array<IShader*>  Shaders;
    //core::map<const char*,IShader*>  Shaders;
    std::map<std::string,IShader*>  Shaders;
    IShader* currentShader;

    GLuint stencil_vbo;
    int	midStencilVal;


    SDL_Window *window ;
    SDL_GLContext context ;

    core::dimension2d<u32> renderTargetSize;
    core::dimension2d<u32> ScreenSize;


//    scene::ICameraObject mainCamera;
    SViewFrustum ViewArea;
    //core::map<irr::core::stringc, bool> MaterialsWritten;


   core::vector3df camera_pos;
   core::vector3df camera_dir;
   core::vector3df camera_up;
   float camera_fov;
   float camera_aspect;
   CFPSCounter FPSCounter;
   int PrimitivesDrawn;
   bool shouldClose;

};
}
}
#endif // ENGINE_H
