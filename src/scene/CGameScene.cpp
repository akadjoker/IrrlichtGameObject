
#include "Engine.h"
#include "CGameScene.h"


namespace irr
{
namespace scene
{

CGameScene::CGameScene(core::stringc name,s32 id):IGameScene(name,id),ShadowColor(150,0,0,0), AmbientLight(0,0,0,0)
{
  setDebugName("CGameScene");
 }

CGameScene::~CGameScene()
{
   removeAll();
   removeAllStatic();
}

void CGameScene::OnUpdate(u32 timeMs)
{

IGameStaticList::Iterator its = GameStaticList.begin();

for (; its != GameStaticList.end(); ++its)
    (*its)->OnUpdate(timeMs);

        IGameObjectList::Iterator it = GameObjectList.begin();
for (; it != GameObjectList.end(); ++it)
    (*it)->OnUpdate(timeMs);


}

void CGameScene::OnRender(IShader* shader)
{
scene::Engine::Instance()->setMaterial(video::SMaterial());
scene::Engine::Instance()->setShader(shader);


IGameObjectList::Iterator it = GameObjectList.begin();
for (; it != GameObjectList.end(); ++it)
    (*it)->OnRender(shader);



}


IGameObject* CGameScene::createGameObject(core::stringc name)
{
   IGameObject* gameObject = new IGameObject();
   gameObject->setName(name.c_str());
   GameObjectList.push_back(gameObject);
   return gameObject;
}
IGameObject* CGameScene::createGameObject(core::stringc name,s32 id)
{
   IGameObject* gameObject = new IGameObject();
   gameObject->setName(name.c_str());
   gameObject->setID(id);

   GameObjectList.push_back(gameObject);
   return gameObject;
}
IGameObject* CGameScene::createGameObject(IGameObject* parent, s32 id,core::stringc name)
{
  IGameObject* gameObject = new IGameObject();
   gameObject->setName(name.c_str());
   gameObject->setID(id);
 if(parent)  gameObject->setParent(parent);
//   GameObjectList.push_back(gameObject);
   return gameObject;
}


IGameObject* CGameScene::createGameObject(IGameObject* parent, s32 id)
{
  IGameObject* gameObject = new IGameObject();
   gameObject->setID(id);
 if(parent)  gameObject->setParent(parent);
//   GameObjectList.push_back(gameObject);
   return gameObject;
}

IGameObject* CGameScene::createGameObject(IGameObject* parent)
{
  IGameObject* gameObject = new IGameObject();
  if(parent) gameObject->setParent(parent);
 //  GameObjectList.push_back(gameObject);
   return gameObject;
}

IGameObject* CGameScene::createGameObject()
{
   IGameObject* gameObject = new IGameObject();
   GameObjectList.push_back(gameObject);
   return gameObject;
}

IGameObject* CGameScene::addGameObject(IGameObject* gameObject)
{
   GameObjectList.push_back(gameObject);
   return gameObject;
}


IGameObject* CGameScene::removeGameObjectById(s32 id)
{
IGameObject* node = 0;


	IGameObjectList& list = GameObjectList;
	IGameObjectList::Iterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		node = (IGameObject*)*it;
		if (node)
		{
            if (node->getID()==id)
            {
            GameObjectList.erase(it);
            return node;
            }
		}

	}

	return node;
}

IGameObject* CGameScene::removeGameObjectByName(const core::stringc name)
{
IGameObject* node = 0;


	IGameObjectList& list = GameObjectList;
	IGameObjectList::Iterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		node = (IGameObject*)*it;
		if (node)
		{
            if (strcmp(node->getName(),name.c_str())==0)
            {
            GameObjectList.erase(it);
            return node;
            }
		}

	}

	return node;

}
IGameObject* CGameScene::getGameObjectById(s32 id)
{
	IGameObject* node = 0;
	const IGameObjectList& list = GameObjectList;
	IGameObjectList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		IGameObject* n = (IGameObject*)*it;
		if (n)
		{
			if (n->getID()==id)
            {
            return n;
            }
		}

	}

return node;
}

IGameObject* CGameScene::getGameObjectByName(const core::stringc name)
{
	IGameObject* node = 0;


	const IGameObjectList& list = GameObjectList;
	IGameObjectList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		node = (IGameObject*)*it;
		if (node)
		{
			    //SDL_Log("%d %s %s",strcmp(node->getName(),name.c_str()),node->getName(),name.c_str());
                if (strcmp(node->getName(),name.c_str())==0)
                {
                 return node;
                }
		}

	}

	return node;
}
void CGameScene::removeAll()
{

      //  SDL_Log("Delete Objects");
        IGameObjectList::Iterator it = GameObjectList.begin();
		for (; it != GameObjectList.end(); ++it)
				(*it)->drop();
			GameObjectList.clear();


}
void CGameScene::removeAllStatic()
{
 //  SDL_Log("Delete Objects");
        IGameStaticList::Iterator it = GameStaticList.begin();
		for (; it != GameStaticList.end(); ++it)
				(*it)->drop();
			GameStaticList.clear();

}
void CGameScene::OnRenderStatic()
{
scene::Engine::Instance()->setMaterial(video::SMaterial());

IGameStaticList::Iterator it = GameStaticList.begin();
for (; it != GameStaticList.end(); ++it)
    (*it)->OnRender();



}


IGameStatic* CGameScene::addGameStatic(IGameStatic* gameStatic)
{
   GameStaticList.push_back(gameStatic);
   return gameStatic;
}

IGameStatic* CGameScene::removeGameStatic(IGameStatic* gameStatic)
{
    IGameStatic* node = 0;
	IGameStaticList& list = GameStaticList;
	IGameStaticList::Iterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		IGameStatic* n = (IGameStatic*)*it;
		if (n)
		{
			if (n->getID()==gameStatic->getID() && n->getName()==gameStatic->getName())
            {

              GameStaticList.erase(it);
              return n;
            }
		}

	}
}

IGameStatic* CGameScene::removeGameStaticById(s32 id)
{
  IGameStatic* node = 0;
	IGameStaticList& list = GameStaticList;
	IGameStaticList::Iterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		IGameStatic* n = (IGameStatic*)*it;
		if (n)
		{
			if (n->getID()==id)
            {

              GameStaticList.erase(it);
              return n;
            }
		}

	}
}

IGameStatic* CGameScene::removeGameStaticByName(const core::stringc name)
{
  IGameStatic* node = 0;
	IGameStaticList& list = GameStaticList;
	IGameStaticList::Iterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		IGameStatic* n = (IGameStatic*)*it;
		if (n)
		{
			if (n->getName()==name.c_str())
            {

              GameStaticList.erase(it);
              return n;
            }
		}

	}
}

IGameStatic* CGameScene::getGameStaticById(s32 id)
{
 IGameStatic* node = 0;


	const IGameStaticList& list = GameStaticList;
	IGameStaticList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		IGameStatic* n = (IGameStatic*)*it;
		if (n)
		{
			if (n->getID()==id)
            {
            return n;
            }
		}

	}


return node;

}

IGameStatic* CGameScene::getGameStaticByName(const core::stringc name)
{
    IGameStatic* node = 0;


	const IGameStaticList& list = GameStaticList;
	IGameStaticList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{

		node = (IGameStatic*)*it;
		if (node)
		{
			    //SDL_Log("%d %s %s",strcmp(node->getName(),name.c_str()),node->getName(),name.c_str());
                if (strcmp(node->getName(),name.c_str())==0)
                {
                 return node;
                }
		}

	}

	return node;
}





} // end namespace scene
} // end namespace irr



