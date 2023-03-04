#ifndef __C_GAMESCENE_H_INCLUDED
#define __C_GAMESCENE_H_INCLUDED

#include "IGameScene.h"
#include "ITransform.h"
#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "quaternion.h"
#include "matrix4.h"
#include "irrList.h"


#include <vector>
#include <map>
#include <typeindex>
#include <algorithm>
#include <memory>
#include <chrono>
#include <vector>


namespace irr
{
namespace scene
{

class CGameScene : public IGameScene
{
public:

    CGameScene(core::stringc name,s32 id);
   virtual ~CGameScene();

   virtual void OnUpdate(u32 timeMs) _IRR_OVERRIDE_;
   virtual void OnRender(IShader* shader) _IRR_OVERRIDE_;
   virtual void OnRenderStatic() _IRR_OVERRIDE_;
   virtual IGameObject* createGameObject(core::stringc name)_IRR_OVERRIDE_;
   virtual IGameObject* createGameObject(core::stringc name,s32 id)_IRR_OVERRIDE_;
   virtual IGameObject* createGameObject(IGameObject* parent, s32 id,core::stringc name)_IRR_OVERRIDE_;
   virtual IGameObject* createGameObject(IGameObject* parent, s32 id)_IRR_OVERRIDE_;
   virtual IGameObject* createGameObject(IGameObject* parent)_IRR_OVERRIDE_;
   virtual IGameObject* createGameObject()_IRR_OVERRIDE_;
   virtual IGameObject* addGameObject(IGameObject* gameObject)_IRR_OVERRIDE_;
   virtual IGameObject* removeGameObjectById(s32 id)_IRR_OVERRIDE_;
   virtual IGameObject* removeGameObjectByName(const core::stringc name)_IRR_OVERRIDE_;
   virtual IGameObject* getGameObjectById(s32 id)_IRR_OVERRIDE_;
   virtual IGameObject* getGameObjectByName(const core::stringc name)_IRR_OVERRIDE_;
   virtual void removeAll()_IRR_OVERRIDE_;
   virtual void removeAllStatic()_IRR_OVERRIDE_;


   virtual IGameStatic* addGameStatic(IGameStatic* gameStatic)_IRR_OVERRIDE_;
   virtual IGameStatic* removeGameStatic(IGameStatic* gameStatic)_IRR_OVERRIDE_;
   virtual IGameStatic* removeGameStaticById(s32 id)_IRR_OVERRIDE_;
   virtual IGameStatic* removeGameStaticByName(const core::stringc name)_IRR_OVERRIDE_;
   virtual IGameStatic* getGameStaticById(s32 id)_IRR_OVERRIDE_;
   virtual IGameStatic* getGameStaticByName(const core::stringc name)_IRR_OVERRIDE_;




private:

struct DefaultNodeEntry
		{
			DefaultNodeEntry(scene::IGameObject* n) :Node(n), TextureValue(0)
			{
			//	if (n->getMaterialCount())
			//		TextureValue = (n->getMaterial(0).getTexture(0));
			}

			bool operator < (const DefaultNodeEntry& other) const
			{
				return (TextureValue < other.TextureValue);
			}

			IGameObject* Node;
			private:
			void* TextureValue;
		};

		//! sort on distance (center) to camera
		struct TransparentNodeEntry
		{
			TransparentNodeEntry(IGameObject* n, const core::vector3df& camera)
				: Node(n)
			{
				Distance = Node->Transform->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(camera);
			}

			bool operator < (const TransparentNodeEntry& other) const
			{
				return Distance > other.Distance;
			}

			IGameObject* Node;
			private:
				f64 Distance;
		};

		//! sort on distance (sphere) to camera
		struct DistanceNodeEntry
		{
			DistanceNodeEntry(IGameObject* n, const core::vector3df& cameraPos)
				: Node(n)
			{
				setNodeAndDistanceFromPosition(n, cameraPos);
			}

			bool operator < (const DistanceNodeEntry& other) const
			{
				return Distance < other.Distance;
			}

			void setNodeAndDistanceFromPosition(IGameObject* n, const core::vector3df & fromPosition)
			{
				Node = n;
				Distance = Node->Transform->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(fromPosition);
				//Distance -= Node->getBoundingBox().getExtent().getLengthSQ() * 0.5;
			}

			IGameObject* Node;
			private:
			f64 Distance;
		};
		//! render pass lists
		core::array<IGameObject*> CameraList;

		/*core::array<ISceneNode*> LightList;
		core::array<ISceneNode*> ShadowNodeList;
		core::array<ISceneNode*> SkyBoxList;
		core::array<DefaultNodeEntry> SolidNodeList;
		core::array<TransparentNodeEntry> TransparentNodeList;*/



		//core::array<IMeshLoader*> MeshLoaderList;
		core::list<IGameObject*> GameObjectList;
		//core::list<IGameObjectList*> GameObjectList;

		core::array<IGameObject*> DeletionList;


		core::list<IGameStatic*> GameStaticList;

		//! current active camera
		//ICameraSceneNode* ActiveCamera;
		core::vector3df camWorldPos; // Position of camera for transparent nodes.

		video::SColor ShadowColor;
		video::SColorf AmbientLight;



};


} // end namespace scene
} // end namespace irr


#endif
