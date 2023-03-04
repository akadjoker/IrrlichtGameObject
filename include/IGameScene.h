#ifndef __I_GAMESCENE_H_INCLUDED
#define __I_GAMESCENE_H_INCLUDED


#include "IGameObject.h"
#include "IGameStatic.h"
#include "IReferenceCounted.h"
#include "ITransform.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "quaternion.h"
#include "matrix4.h"
#include "irrList.h"

namespace irr
{
namespace scene
{
class IGameScene;

typedef core::list<IGameScene*> ISceneList;

class IGameScene : public virtual IReferenceCounted
{
public:

    IGameScene(core::stringc name,s32 id):Name(name),ID(id)
    {
     setDebugName("IGameScene");
    // SDL_Log("Create IScene %s %d",Name.c_str(),ID);
    };
   virtual~IGameScene(){};
   virtual void OnUpdate(u32 timeMs)=0;
   virtual void OnRender(IShader* shader)=0;
   virtual void OnRenderStatic()=0;

   virtual IGameObject* createGameObject()=0;
   virtual IGameObject* createGameObject(core::stringc name)=0;
   virtual IGameObject* createGameObject(core::stringc name,s32 id)=0;

   virtual IGameObject* createGameObject(IGameObject* parent, s32 id,core::stringc name)=0;
   virtual IGameObject* createGameObject(IGameObject* parent, s32 id)=0;
   virtual IGameObject* createGameObject(IGameObject* parent)=0;

   virtual IGameObject* addGameObject(IGameObject* gameObject)=0;
   virtual IGameObject* removeGameObjectById(s32 id)=0;
   virtual IGameObject* removeGameObjectByName(const core::stringc name)=0;
   virtual IGameObject* getGameObjectById(s32 id)=0;
   virtual IGameObject* getGameObjectByName(const core::stringc name)=0;


   virtual IGameStatic* addGameStatic(IGameStatic* gameObject)=0;
   virtual IGameStatic* removeGameStatic(IGameStatic* gameObject)=0;
   virtual IGameStatic* removeGameStaticById(s32 id)=0;
   virtual IGameStatic* removeGameStaticByName(const core::stringc name)=0;
   virtual IGameStatic* getGameStaticById(s32 id)=0;
   virtual IGameStatic* getGameStaticByName(const core::stringc name)=0;




   virtual void removeAll()=0;
   virtual void removeAllStatic()=0;

    s32 getID() const {return ID;}
    void setID(s32 id)   {id=id;}
    const c8* getName() const    {        return Name.c_str();    }
    void setName(const c8* name)    {        Name = name;    }

private:

protected:
		core::stringc Name;
		s32 ID;

};

} // end namespace scene
} // end namespace irr

#endif
