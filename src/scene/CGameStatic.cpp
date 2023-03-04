

#include "IGameStatic.h"
#include "Engine.h"
#include "COpenGLTexture.h"


namespace irr
{
namespace scene
{


IGameStatic::IGameStatic(s32 id,const c8* name): ID(id),IsVisible(true),Name(name)
{

 setDebugName("IGameStatic");
}


IGameStatic::IGameStatic(s32 id): ID(id),IsVisible(true),Name("IGAMESTATIC")
{
setDebugName("IGameStatic");
}

IGameStatic::IGameStatic(): ID(-1),IsVisible(true),Name("IGAMESTATIC")
{
setDebugName("IGameStatic");
}

IGameStatic::~IGameStatic()
{


 SDL_Log("[IGAMESTATIC] Delete %s %d",Name.c_str(),ID);


}


void IGameStatic::OnUpdate(u32 timeMs)
{


}

void IGameStatic::OnRender()
{





}

void IGameStatic::setVisible(bool isVisible)
{
    IsVisible = isVisible;
}


 s32 IGameStatic::getID() const
{
    return ID;
}



void IGameStatic::setID(s32 id)
{
    ID = id;
}



ISkyBox::ISkyBox(core::array<core::stringc> faces)
{
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };




  //  loadCubemap(faces);

    texture = new COpenGLCubeMapTexture(faces,"TextureSkyBox");




    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

}
ISkyBox::~ISkyBox()
{

    texture->drop();
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);

    //SDL_Log("[SKYBOX] free"); //is engine clean this memory ?? must delete :D
}
void ISkyBox::OnUpdate(u32 timeMs)
{
}
void ISkyBox::OnRender()
{

IShader* skyboxShader=scene::Engine::Instance()->getShader("SkyBox");
if(!skyboxShader) return;




  // draw skybox as last

     //   GLint bound;
     //   glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);



        Engine* engine=scene::Engine::Instance();
        engine->setDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content
        engine->useShader(skyboxShader);


        //core::matrix3 view3=engine->getViewMatrix().ToMatrix3();

        //core::matrix4 view =  engine->getViewMatrix();// remove translation from the view matrix
        core::matrix4 view =  engine->getViewMatrix().ToMatrix3();// remove translation from the view matrix
        skyboxShader->setMat4("view", view);
        //skyboxShader->setMat3("view", view3);
        skyboxShader->setMat4("projection", engine->getProjectionMatrix());
        skyboxShader->setInt("skybox",0);
        glBindVertexArray(skyboxVAO);


      //  engine->setActiveTexture(0);
     //   engine->setTexture(GL_TEXTURE_CUBE_MAP, textureID);
         engine->setTexture(texture);

         glDrawArrays(GL_TRIANGLES, 0, 36);
         glBindVertexArray(0);
         engine->setDepthFunc(GL_LESS);



}
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
void ISkyBox::loadCubemap(const core::array<core::stringc> faces)
{
Engine* engine=scene::Engine::Instance();


    IImage* images[6];

    GLint bound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);



    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {




        images[i] = engine->createImageFromFile(faces[i].c_str());
        if (images[i])
        {


           int width  =images[i]->getDimension().Width;
           int height =images[i]->getDimension().Height;
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
            SDL_Log("Cubemap texture failed to load at path: %s" , faces[i].c_str() );

        }

    }

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


} // end namespace scene
} // end namespace irr



