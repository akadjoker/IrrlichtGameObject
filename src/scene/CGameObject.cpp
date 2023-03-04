
#include "CSkinnedMesh.h"
#include "IGameObject.h"
#include "Engine.h"



namespace irr
{
namespace scene
{


IGameObject::IGameObject(s32 id,const c8* name):Parent(0), ID(id),IsVisible(true),Name(name)
{
 Transform= new scene::ITransform();
 setDebugName("IGameObject");
}

IGameObject::IGameObject(IGameObject* parent, s32 id):Parent(parent), ID(id),IsVisible(true),Name("GameObject")
{
 Transform= new scene::ITransform();
 setDebugName("IGameObject");
}
IGameObject::IGameObject(s32 id):Parent(0), ID(id),IsVisible(true),Name("GameObject")
{
 Transform= new scene::ITransform();
 setDebugName("IGameObject");
}

IGameObject::IGameObject():Parent(0), ID(-1),IsVisible(true),Name("GameObject")
{
 Transform= new scene::ITransform();
 setDebugName("IGameObject");
}

IGameObject::~IGameObject()
{

  int index=0;
  for (auto component : components)
  {
   SDL_Log("[OBJECT] Free %s component",component->getType());

    index++;
    components.erase (components.begin()+index);


    component->free();

  }

 SDL_Log("[GAMEOBJECT] Delete %s %d",Name.c_str(),ID);
 removeAll();

}

bool sortByName(const std::shared_ptr<IComponent>& d1, const std::shared_ptr<IComponent>& d2)
{
}
bool sortById(const std::shared_ptr<IComponent>& d1, const std::shared_ptr<IComponent>& d2)
{
}


static bool objSort (std::shared_ptr<IComponent> a,std::shared_ptr<IComponent> b) { return (a->getPriority()<b->getPriority()); }

void IGameObject::sort()
{
  std::sort (components.begin(), components.end(), objSort);
  for (auto component : components)
  {
    // TRACELOG(0,"id %d",component->getPriority());
  }

}


void IGameObject::OnUpdate(u32 timeMs)
{

    if (SortCmp)
    {
        sort();
        SortCmp=false;
        return;
    }


    for (auto component : components)
    {
     component->OnUpdate(timeMs);
    }


    Transform->OnUpdate(timeMs);
    IGameObjectList::Iterator it = Children.begin();
    for (; it != Children.end(); ++it)
    {
            (*it)->OnUpdate(timeMs);

    }
}

void IGameObject::OnRender(IShader* shader)
{

  for (auto component : components)
  {
    component->OnRender(shader);
  }

    IGameObjectList::Iterator it = Children.begin();
    for (; it != Children.end(); ++it)
    {
            (*it)->OnRender(shader);

    }



}
void IGameObject::setParent(IGameObject* newParent)
{
  //  grab();
  //  remove();

    Parent = newParent;
    Transform->setParent(newParent->Transform);

    if (Parent)
        Parent->addChild(this);

 //   drop();
}

const core::list<IGameObject*>& IGameObject::getChildren() const
{
    return Children;
}


void IGameObject::setVisible(bool isVisible)
{
    IsVisible = isVisible;
}


 s32 IGameObject::getID() const
{
    return ID;
}



void IGameObject::setID(s32 id)
{
    ID = id;
}


void IGameObject::remove()
{
    if (Parent)
        Parent->removeChild(this);
}


 void IGameObject::addChild(IGameObject* child)
{
    if (child && (child != this))
    {

      //  child->grab();
      //  child->remove(); // remove from old parent
        Children.push_back(child);
        child->Parent = this;
    }
}

IGameObject* IGameObject::getChildById(s32 id)
{
    IGameObject* node = 0;
	const IGameObjectList& list = Children;
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
IGameObject* IGameObject::getChildByName(const core::stringc name)
{
IGameObject* node = 0;


	const IGameObjectList& list = Children;
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


bool IGameObject::removeChild(IGameObject* child)
{
    IGameObjectList::Iterator it = Children.begin();
    for (; it != Children.end(); ++it)
        if ((*it) == child)
        {
            (*it)->Parent = 0;
            (*it)->drop();
            Children.erase(it);
            return true;
        }

    return false;
}

 void IGameObject::removeAll()
{
    IGameObjectList::Iterator it = Children.begin();
    for (; it != Children.end(); ++it)
    {
        (*it)->Parent = 0;
        (*it)->drop();
    }

    Children.clear();
}

} // end namespace scene
} // end namespace irr



