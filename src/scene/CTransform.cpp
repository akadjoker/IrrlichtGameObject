#include "ITransform.h"


namespace irr
{
namespace scene
{

ITransform::ITransform()
: Rotation(core::vector3df(0,0,0)),RelativeTranslation(core::vector3df(0,0,0)), RelativeRotation(core::vector3df(0,0,0)), RelativeScale(core::vector3df(1.0f, 1.0f, 1.0f)),Parent(0)
{
    updateAbsolutePosition();
    setDebugName("ITransform");
};

ITransform::ITransform(scene::ITransform* parent)
: Rotation(core::vector3df(0,0,0)), RelativeTranslation(core::vector3df(0,0,0)), RelativeRotation(core::vector3df(0,0,0)), RelativeScale(core::vector3df(1.0f, 1.0f, 1.0f)),Parent(parent)
{
    updateAbsolutePosition();
    setDebugName("ITransform");
}



ITransform::ITransform(scene::ITransform* parent,const core::vector3df& position)
: Rotation(core::vector3df(0,0,0)), RelativeTranslation(position), RelativeRotation(core::vector3df(0,0,0)), RelativeScale(core::vector3df(1.0f, 1.0f, 1.0f)),Parent(parent)
{
    updateAbsolutePosition();
    setDebugName("ITransform");
}


ITransform::ITransform(scene::ITransform* parent,const core::vector3df& position,const core::vector3df& rotation,const core::vector3df& scale )
: Rotation(core::vector3df(0,0,0)),RelativeTranslation(position), RelativeRotation(rotation), RelativeScale(scale),Parent(parent)
{
    updateAbsolutePosition();
    setDebugName("ITransform");
}

} // end namespace video
} // end namespace irr



