

#include "IGameObject.h"
#include "Engine.h"



namespace irr
{
namespace scene
{

//************************************************************************
ICamera::ICamera()
{
}
core::matrix4 ICamera::getViewMatrix(void) const
{
 core::matrix4 model = m_parent->Transform->getAbsoluteTransformation();
 core::matrix4 view=view.makeIdentity();
 model.getInverse(view);
 return view;
}
core::matrix4 ICamera::getProjMatrix(void) const
{
   scene::Engine* engine = scene::Engine::Instance();


   float aspect =1025/720;// (float)engine->getScreenSize().Width / (float)engine->getScreenSize().Height;
   float fov   =  45.0f;
   core::matrix4 pMat;
   pMat.buildProjectionMatrixPerspectiveFovRH(core::degToRad(fov),aspect, 0.1f, 1000.0f);

}
void ICamera::OnRender(IShader* shader)
{
  scene::Engine* engine = scene::Engine::Instance();
  //engine->setProjectionMatrix(getProjMatrix());
  engine->setViewMatrix(getViewMatrix());
}


}
}
