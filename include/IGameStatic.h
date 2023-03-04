#ifndef __I_GAMESTATIC_H_INCLUDED
#define __I_GAMESTATIC_H_INCLUDED

#include "IReferenceCounted.h"

#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "quaternion.h"
#include "matrix4.h"
#include "irrList.h"
#include "CShader.h"
#include "SMesh.h"

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
class IGameStatic;
typedef core::list<IGameStatic*> IGameStaticList;



//********************************************************************
class IGameStatic : public virtual IReferenceCounted
{
public:

    IGameStatic();
    IGameStatic(s32 id);
    IGameStatic(s32 id,const c8* name);
    ~IGameStatic();

    virtual    void OnUpdate(u32 timeMs) ;
    virtual    void OnRender() ;



//! Returns the name of the node.
/** \return Name as character string. */
 const c8* getName() const
{
    return Name.c_str();
}


//! Sets the name of the node.
/** \param name New name of the scene node. */
 void setName(const c8* name)
{
    Name = name;
}

//! Sets if the node should be visible or not.
/** All children of this node won't be visible either, when set
to false. Invisible nodes are not valid candidates for selection by
collision manager bounding box methods.
\param isVisible If the node shall be visible. */
void setVisible(bool isVisible);

//! Get the id of the scene node.
/** This id can be used to identify the node.
\return The id. */
 s32 getID() const;

//! Sets the id of the scene node.
/** This id can be used to identify the node.
\param id The new id. */
void setID(s32 id);


protected:
		//! Name of the scene node.
		core::stringc Name;
		//! ID of the node.
		s32 ID;
        //! Is the node visible?
		bool IsVisible;


};



class ISkyBox : public IGameStatic
{
  public:
    ISkyBox(core::array<core::stringc> faces);
    ~ISkyBox();
   virtual void OnUpdate(u32 timeMs) _IRR_OVERRIDE_;
   virtual void OnRender() _IRR_OVERRIDE_;
private:
unsigned int textureID;
unsigned int skyboxVAO, skyboxVBO;
video::ITexture* texture;

void loadCubemap(const core::array<core::stringc> faces);

};



} // end namespace scene
} // end namespace irr

#endif
