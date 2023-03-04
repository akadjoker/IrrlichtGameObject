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
core::vector3df cameraPos   = core::vector3df(0,5,20);
core::vector3df cameraFront = core::vector3df(0,0,1);


//core::vector3df cameraPos   = core::vector3df(0,4,10);
//core::vector3df cameraFront = core::vector3df(0,-4,0);

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



void processInput()
{

    float cameraSpeed = 400 * engine->GetFrameTime();
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
/*

scene::IMesh* cube_mesh= engine->getGeometryCreator()->createCubeMesh();
cube_mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/marble.jpg"));
mainScene->createGameObject("cube")->addComponent<IMeshRenderer>(cube_mesh);
*/




scene::IAnimatedMesh* nijaMesh= engine->getGeometryCreator()->importMesh("media/ninja.ms3d");
scene::IGameObject* ninja=mainScene->createGameObject("ninja",3);
ninja->addComponent<IMeshRenderer>(nijaMesh);

ninja->addComponent<IAnimator>(nijaMesh);
ninja->Transform->setPosition(0,0,0);

scene::IShadowMesh* md2Shadow= new scene::CShadowMesh(nijaMesh->getMesh(0));

//ninja->Transform->setPosition(200,400,200);

scene::IMesh* cube_mesh= engine->getGeometryCreator()->createCubeMesh();
cube_mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/marble.jpg"));
scene::IGameObject* cube=mainScene->createGameObject("cube");
cube->addComponent<IMeshRenderer>(cube_mesh);
cube->Transform->setScale(100,0.5f,100);


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





    engine->BeginScene();

    engine->ClearScene(video::ECBF_COLOR | video::ECBF_DEPTH,RAYWHITE );






IShader* shader = engine->getShader("LightDetailShader");
detailScene->OnRender(shader);

shader = engine->getShader("LightShader");
mainScene->OnRender(shader);


IShader* shader_points = engine->getShader("Points");

    core::matrix4 model;
if (shader_points)
{

	engine->startStencilTest();
    engine->setShader(shader_points);
    shader_points->setMat4("view",engine->getViewMatrix());
    shader_points->setMat4("projection",engine->getProjectionMatrix());
    model = model.makeIdentity();
    model.setTranslation(core::vector3df(0.0f,0.0f,0.0f));
    shader_points->setMat4("model",model);
    md2Shadow->render(model);
    engine->endStencilTest();
}
//mainScene->OnRenderStatic();

IShader* shader_stancil = engine->getShader("Stencil");
if (shader_stancil)
{
engine->useShader(shader_stancil);
engine->drawStencilShadow(true);
}



BeginModeBatch3D();

Draw3DGrid(20,1);
EndRenderBatch();

//RenderBatch();

//engine->setDepthTest(false);

//engine->render3DBatch();

//engine->setDepthTest(true);





engine->EndScene();


engine->Swap();


     // SDL_SetWindowTitle(window,TextFormat("fps[ %d ] %f %f ",GetFPS(),GetFrameTime(),GetTime()));
   engine->setWindowCaption(TextFormat("%s   fps[ %d ] %d %d %d %d",TITLE,engine->GetFPS(),engine->GetKeyPressed(),engine->GetCharPressed(),engine->GetTouchX(),engine->GetTouchY()));


}




engine->drop();


return EXIT_SUCCESS;
}
