#include <exception>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>


#include <SDL2/SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <cstdio>
#include <cstdlib>
#include "irrlicht.h"
#include "CVertexBuffer.h"

#include "Utils.h"
#include "Core.h"
#include "CColorConverter.h"

#include "CSkinnedMesh.h"
#include "ISkinnedMesh.h"
#include "SSkinMeshBuffer.h"

#include "matrix3.h"

using namespace irr;
using namespace scene;
using namespace video;



const unsigned int DISP_WIDTH = 1025;
const unsigned int DISP_HEIGHT = 720;
const char* TITLE=" Stencil test Batch";
const Uint8 *state=NULL;
Uint32 mouse_buttons;
int mouse_x;
int mouse_y;

irr::scene::Engine* engine;
int width, height;
float aspect;


// camera
//core::vector3df cameraPos   = core::vector3df(2700*2,255*2,2600*2);
//core::vector3df cameraFront = core::vector3df(2397*2,343*2,2700*2);
core::vector3df cameraPos   = core::vector3df(0,4,10);
core::vector3df cameraFront = core::vector3df(0,-4,0);

core::vector3df cameraUp    = core::vector3df(0.0f, 1.0f, 0.0f);


/*
camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);
*/

bool firstMouse = true;
bool isMouseDown = false;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;



class Tex2d
{
public:
  Tex2d(){};

  void Load(const char* fileName)
  {

        texture = engine->getTexture("media/font.png");



             		  // Create and compile the vertex shader
		const char* vertexShaderSource = GLSL(

					// Input vertex data, different for all executions of this shader.
					attribute vec2 vertexPosition_screenspace;
					attribute vec2 vertexUV;

					// Output data ; will be interpolated for each fragment.
					varying vec2 UV;

					void main(){


						vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - vec2(400,300);
						vertexPosition_homoneneousspace /= vec2(400,300);
						//gl_Position =  vec4(vertexPosition_screenspace,0,1);
						gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);

						// UV of the vertex. No special space for this one.
						UV = vertexUV;
					}
			);

		const char *fragmentShaderSource = GLSL(
                precision highp float;
				varying vec2 UV;
				uniform sampler2D tex;

				void main(){

                    gl_FragColor = texture2D( tex, UV );


				}


		);

		  shader.FromString(vertexShaderSource,fragmentShaderSource);
		  shader.Use();
		  shader.setInt("tex", 0);
          glGenBuffers(1, &Text2DVertexBufferID);
		  glGenBuffers(1, &Text2DUVBufferID);



	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);



  }
  void Print(const char * text, float x, float y, float size)
  {
	// Bind shader
	shader.Use();
	//engine->setShader(&shader);

	// Bind texture




   	//engine->setTexture(GL_TEXTURE_2D,texture.ID);
   	engine->setTexture(texture);


     unsigned int length = strlen(text);

	// Fill buffers
	std::vector<core::vector2df> vertices;
	std::vector<core::vector2df> UVs;
	for ( unsigned int i=0 ; i<length ; i++ ){

		core::vector2df vertex_up_left    = core::vector2df( x+i*size     , y+size );
		core::vector2df vertex_up_right   = core::vector2df( x+i*size+size, y+size );
		core::vector2df vertex_down_right = core::vector2df( x+i*size+size, y      );
		core::vector2df vertex_down_left  = core::vector2df( x+i*size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		core::vector2df uv_up_left    = core::vector2df( uv_x           , uv_y );
		core::vector2df uv_up_right   = core::vector2df( uv_x+1.0f/16.0f, uv_y );
		core::vector2df uv_down_right = core::vector2df( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		core::vector2df uv_down_left  = core::vector2df( uv_x           , (uv_y + 1.0f/16.0f) );

		UVs.push_back(uv_up_left   );
		UVs.push_back(uv_down_left );
		UVs.push_back(uv_up_right  );

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(core::vector2df), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(core::vector2df), &UVs[0], GL_STATIC_DRAW);




	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	engine->setBlend(true);
	engine->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	engine->setBlend(false);

	///glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);



  }
  void Unload()
  {
	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);


  }

 private:
 IShader shader;
 video::ITexture* texture;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;




};


void processInput()
{

    float cameraSpeed = 20 * engine->GetFrameTime();
    if (state[SDL_SCANCODE_W])
    {
    cameraPos += cameraSpeed * cameraFront;
    }

    if (state[SDL_SCANCODE_S])
    {
     cameraPos -= cameraSpeed * cameraFront;
    }

    if (state[SDL_SCANCODE_A])
    {
    core::vector3df n = cameraFront.crossProduct(cameraUp).normalize();

    cameraPos -= n * cameraSpeed;
    }

    if (state[SDL_SCANCODE_D])
    {
      core::vector3df n = cameraFront.crossProduct(cameraUp).normalize();
      cameraPos += n * cameraSpeed;
    }



}

void window_size_callback(int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glScissor(0, 0, newWidth, newHeight);
    core::matrix4 pMat;
    pMat.buildProjectionMatrixPerspectiveFovRH(core::degToRad(fov),aspect, 0.1f, 42000.0f);
    engine->setProjectionMatrix(pMat);
    engine->setCameraFov(core::degToRad(fov));
    engine->setCameraAspect(aspect);

}
void mouse_callback( double xpos, double ypos)
{
	if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    if ((mouse_buttons & SDL_BUTTON_RMASK) != 0)
    {

    yaw += xoffset;
	pitch += yoffset;
    }



    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
	if (pitch < -89.0f)
        pitch = -89.0f;

	core::vector3df  front;


	front.X = cos(core::degToRad(yaw)) * cos(core::degToRad(pitch));
	front.Y = sin(core::degToRad(pitch));
	front.Z = sin(core::degToRad(yaw)) * cos(core::degToRad(pitch));
	cameraFront = front.normalize();

}






core::matrix4 createTransformation(core::vector3df RelativeTranslation,
core::vector3df RelativeScale =core::vector3df(1.f,1.f,1.f),
core::vector3df RelativeRotation=core::vector3df(0,0,0))
{
    core::matrix4 mat;
    mat.setRotationDegrees(RelativeRotation);
    mat.setTranslation(RelativeTranslation);

    if (RelativeScale != core::vector3df(1.f,1.f,1.f))
    {
        core::matrix4 smat;
        smat.setScale(RelativeScale);
        mat *= smat;
    }

    return mat;
}

int main(int argc, char *args[]) {




engine=irr::scene::Engine::createEngine(DISP_WIDTH,DISP_HEIGHT,false,TITLE);




window_size_callback(DISP_WIDTH,DISP_HEIGHT);





//size_t LengthOfArray = sizeof myArray / sizeof myArray[0];


Tex2d font;
font.Load("media/font.png");










scene::IGameScene* mainScene=engine->createScene("main",0);
scene::IGameScene* detailScene=engine->createScene("detail",1);

/*
        core::array<core::stringc> faces;
        faces.push_back("media/skybox/right.jpg");
        faces.push_back("media/skybox/left.jpg");
        faces.push_back("media/skybox/top.jpg");
        faces.push_back("media/skybox/bottom.jpg");
        faces.push_back("media/skybox/front.jpg");
        faces.push_back("media/skybox/back.jpg");

scene::ISkyBox* skybox= new ISkyBox(faces);

mainScene->addGameStatic(skybox);


*/


scene::IMesh* cube_mesh= engine->getGeometryCreator()->createCubeMesh();
cube_mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/marble.jpg"));
mainScene->createGameObject("cube")->addComponent<IMeshRenderer>(cube_mesh);


const float sunRadius = 4.0f;
const float earthRadius = 0.6f;
const float earthOrbitRadius = 8.0f;
const float moonRadius = 0.16f;
const float moonOrbitRadius = 1.5f;
float rotationSpeed = 0.2f;         // General system rotation speed

float earthRotation = 0.0f;         // Rotation of earth around itself (days) in degrees
float earthOrbitRotation = 0.0f;    // Rotation of earth around the Sun (years) in degrees
float moonRotation = 0.0f;          // Rotation of moon around itself
float moonOrbitRotation = 0.0f;     // Rotation of moon around earth in degrees



int frame=0;

bool quit = false;
while (!quit && !engine->ShouldClose())
{

       SDL_Event event;
       while (SDL_PollEvent(&event))
        {


          if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE )
                quit = true;

         switch (event.type)
            {
            case SDL_QUIT:
            {
             quit = true;
             break;
            }
            case SDL_WINDOWEVENT:
            {

                 switch (event.window.event)
                 {
                    case SDL_WINDOWEVENT_CLOSE:
                     {
                     break;
                     }
                     case SDL_WINDOWEVENT_RESIZED:
                     {

                     window_size_callback(event.window.data1,event.window.data2);

                     break;
                     }
                      case SDL_WINDOWEVENT_SIZE_CHANGED:
                     {
                     break;
                     }


                  }
            break;
            }
             case SDL_MOUSEBUTTONDOWN:
                    {

                     engine->MouseButtonCallback(event.button.button-1,1,0);
                     engine->MouseCursorPosCallback((double)event.button.x,(double)event.button.y);
                    isMouseDown=false;

                    }
                    break;
                    case SDL_MOUSEBUTTONUP:
                    {
                     engine->MouseButtonCallback(event.button.button-1,0,0);
                     engine->MouseCursorPosCallback(event.button.x,event.button.y);
                     isMouseDown=false;

                    }
                    break;
                    case SDL_MOUSEMOTION:
                    {

                     engine->MouseCursorPosCallback(event.button.x,event.button.y);
                     break;
                    }

            case SDL_MOUSEWHEEL:
                {
                        engine->MouseScrollCallback(event.wheel.x,event.wheel.y);

                       if(event.wheel.y > 0) // scroll up
                        {
                           //  MouseWheel-=1;
                           fov -= 1;
                        }
                        else if(event.wheel.y < 0) // scroll down
                        {
                         fov += 1;
                        }


                    if (fov < 1.0f)
                    fov = 1.0f;
                    if (fov > 60.0f)
                    fov = 60.0f;



                    //pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

                        core::matrix4 pMat;
                        pMat.buildProjectionMatrixPerspectiveFovRH(core::degToRad(fov),aspect, 0.1f, 42000.0f);
                        engine->setCameraFov(core::degToRad(fov));
                        engine->setCameraAspect(aspect);
                        engine->setProjectionMatrix(pMat);

                  break;
                }

                  case SDL_KEYDOWN:
                       {
                            engine->KeyCallback(event.key.keysym.scancode,event.key.keysym.sym,1,event.key.keysym.mod);
                       }
                       break;
                       case SDL_KEYUP:
                       {
                       engine->KeyCallback(event.key.keysym.scancode,event.key.keysym.sym,0,event.key.keysym.mod);
                       }
                      break;

     }
    }

    //engine->PollInputEvents();

    mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
    quit=true;
    }
    processInput();
    mouse_callback((double)mouse_x,(double)mouse_y);



    if (engine->IsKeyPressed(SDL_SCANCODE_O))
    {
      SDL_Log("o press");
    }

    if (engine->IsKeyDown(SDL_SCANCODE_O))
    {
      SDL_Log("o down");
    }

    if (engine->IsKeyReleased(SDL_SCANCODE_O))
    {
      SDL_Log("o release");
    }



/*
    if (engine->IsKeyUp(SDL_SCANCODE_O))
    {
      SDL_Log("o up");
    }
*/

    engine->setViewMatrix(cameraPos, cameraPos + cameraFront, cameraUp);

     detailScene->OnUpdate(os::Timer::getTime());
     mainScene->OnUpdate(os::Timer::getTime());


        earthRotation += (5.0f*rotationSpeed);
        earthOrbitRotation += (365/360.0f*(5.0f*rotationSpeed)*rotationSpeed);
        moonRotation += (2.0f*rotationSpeed);
        moonOrbitRotation += (8.0f*rotationSpeed);




    engine->BeginScene();

    engine->ClearScene(video::ECBF_COLOR | video::ECBF_DEPTH ,RAYWHITE);





//     IShader* shader = engine->getShader("LightShader");




     IShader* shader = engine->getShader("LightDetailShader");
   //  detailScene->OnRender(shader);

    shader = engine->getShader("LightShader");
    mainScene->OnRender(shader);



mainScene->OnRenderStatic();


BeginModeBatch3D();




rlPushMatrix();
        rlScalef(sunRadius, sunRadius, sunRadius);          // Scale Sun

        DrawSphereBasic(GOLD);                              // Draw the Sun
rlPopMatrix();

  rlPushMatrix();
                    rlRotatef(earthOrbitRotation, 0.0f, 1.0f, 0.0f);    // Rotation for Earth orbit around Sun
                    rlTranslatef(earthOrbitRadius, 0.0f, 0.0f);         // Translation for Earth orbit
                    rlRotatef(-earthOrbitRotation, 0.0f, 1.0f, 0.0f);   // Rotation for Earth orbit around Sun inverted

                    rlPushMatrix();
                        rlRotatef(earthRotation, 0.25, 1.0, 0.0);       // Rotation for Earth itself
                        rlScalef(earthRadius, earthRadius, earthRadius);// Scale Earth
                        DrawSphereBasic(BLUE);                          // Draw the Earth
                    rlPopMatrix();

                    rlRotatef(moonOrbitRotation, 0.0f, 1.0f, 0.0f);     // Rotation for Moon orbit around Earth
                    rlTranslatef(moonOrbitRadius, 0.0f, 0.0f);          // Translation for Moon orbit
                    rlRotatef(-moonOrbitRotation, 0.0f, 1.0f, 0.0f);    // Rotation for Moon orbit around Earth inverted
                    rlRotatef(moonRotation, 0.0f, 1.0f, 0.0f);          // Rotation for Moon itself
                    rlScalef(moonRadius, moonRadius, moonRadius);       // Scale Moon
                    DrawSphereBasic(LIGHTGRAY);                         // Draw the Moon
             //       cube_mesh->render();
                rlPopMatrix();

                // Some reference elements (not affected by previous matrix transformations)
                DrawCircle3D((core::vector3df){ 0.0f, 0.0f, 0.0f }, earthOrbitRadius, (core::vector3df){ 1, 0, 0 }, 90.0f, RED.Fade(0.5f));

//engine->DrawGrid(20,1);
Draw3DGrid(20,1);

EndRenderBatch();


//engine->setDepthTest(false);

//engine->render3DBatch();

//engine->setDepthTest(true);



/*
char text[256];
sprintf(text,"%f sec %d fps", engine->GetFrameTime(),engine->GetFPS() );
font.Print(text, 20, 20, 10.5f);

*/

engine->EndScene();


engine->Swap();


     // SDL_SetWindowTitle(window,TextFormat("fps[ %d ] %f %f ",GetFPS(),GetFrameTime(),GetTime()));
   engine->setWindowCaption(TextFormat("%s   fps[ %d ] %d %d %d %d",TITLE,engine->GetFPS(),engine->GetKeyPressed(),engine->GetCharPressed(),engine->GetTouchX(),engine->GetTouchY()));


}


//cube->drop();

//quakemd2->drop();


//mainScene->drop();
//mainCamera->drop();

font.Unload();

engine->drop();


return EXIT_SUCCESS;
}
