#include "ICameraObject.h"
#include "Engine.h"
#include "SViewFrustum.h"

namespace irr
{
namespace scene
{
ICameraObject::ICameraObject()
:IGameObject(),Target(0.0f,0.0f,1.0f), UpVector(0.0f, 1.0f, 0.0f), ZNear(1.0f), ZFar(3000.0f),TargetAndRotationAreBound(false)
{
	setDebugName("CCameraSceneNode");

	// set default projection
	Fovy = core::PI / 2.5f;	// Field of view, in radians.


	Aspect = (f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Width /	(f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Height;


	ViewArea.setFarNearDistance(ZFar - ZNear);
	recalculateProjectionMatrix();
	recalculateViewArea();
}
ICameraObject::ICameraObject(const core::vector3df& position, const core::vector3df& lookat)
:IGameObject(),Target(lookat), UpVector(0.0f, 1.0f, 0.0f), ZNear(1.0f), ZFar(3000.0f),TargetAndRotationAreBound(false)
{
	setDebugName("CCameraSceneNode");

	// set default projection
	Fovy = core::PI / 2.5f;	// Field of view, in radians.


	Aspect = (f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Width /	(f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Height;


	ViewArea.setFarNearDistance(ZFar - ZNear);
	recalculateProjectionMatrix();
	recalculateViewArea();
}
ICameraObject::ICameraObject(const core::vector3df& position, const core::vector3df& lookat, IGameObject* parent,  s32 id)
:IGameObject(parent,id),Target(lookat), UpVector(0.0f, 1.0f, 0.0f), ZNear(1.0f), ZFar(3000.0f),TargetAndRotationAreBound(false)
{
	setDebugName("CCameraSceneNode");

	// set default projection
	Fovy = core::PI / 2.5f;	// Field of view, in radians.


	Aspect = (f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Width /	(f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Height;


	ViewArea.setFarNearDistance(ZFar - ZNear);
	recalculateProjectionMatrix();
	recalculateViewArea();
}

ICameraObject::~ICameraObject()
{
	setDebugName("CCameraSceneNode");

	// set default projection
	Fovy = core::PI / 2.5f;	// Field of view, in radians.


	Aspect = (f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Width /	(f32)scene::Engine::Instance()->getCurrentRenderTargetSize().Height;


	ViewArea.setFarNearDistance(ZFar - ZNear);
	recalculateProjectionMatrix();
	recalculateViewArea();
}






//! Sets the projection matrix of the camera.
/** The core::matrix4 class has some methods
to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
\param projection: The new projection matrix of the camera. */
void ICameraObject::setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal)
{
	IsOrthogonal = isOrthogonal;
	ViewArea.getTransform ( video::ETS_PROJECTION ) = projection;
}


//! Gets the current projection matrix of the camera
//! \return Returns the current projection matrix of the camera.
const core::matrix4& ICameraObject::getProjectionMatrix() const
{
	return ViewArea.getTransform ( video::ETS_PROJECTION );
}


//! Gets the current view matrix of the camera
//! \return Returns the current view matrix of the camera.
const core::matrix4& ICameraObject::getViewMatrix() const
{
	return ViewArea.getTransform ( video::ETS_VIEW );
}


//! Sets a custom view matrix affector. The matrix passed here, will be
//! multiplied with the view matrix when it gets updated.
//! This allows for custom camera setups like, for example, a reflection camera.
/** \param affector: The affector matrix. */
void ICameraObject::setViewMatrixAffector(const core::matrix4& affector)
{
	Affector = affector;
}


//! Gets the custom view matrix affector.
const core::matrix4& ICameraObject::getViewMatrixAffector() const
{
	return Affector;
}


//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void ICameraObject::setTarget(const core::vector3df& pos)
{
	Target = pos;

	if(TargetAndRotationAreBound)
	{
		const core::vector3df toTarget = Target - Transform->getAbsolutePosition();
		Transform->setRotation(toTarget.getHorizontalAngle());
	}
}


//! Sets the rotation of the node.
/** This only modifies the relative rotation of the node.
If the camera's target and rotation are bound ( @see bindTargetAndRotation() )
then calling this will also change the camera's target to match the rotation.
\param rotation New rotation of the node in degrees. */
void ICameraObject::setRotation(const core::vector3df& rotation)
{
	if(TargetAndRotationAreBound)
		Target = Transform->getAbsolutePosition() + rotation.rotationToDirection();

	Transform->setRotation(rotation);
}


//! Gets the current look at target of the camera
//! \return Returns the current look at target of the camera
const core::vector3df& ICameraObject::getTarget() const
{
	return Target;
}


//! sets the up vector of the camera
//! \param pos: New upvector of the camera.
void ICameraObject::setUpVector(const core::vector3df& pos)
{
	UpVector = pos;
}


//! Gets the up vector of the camera.
//! \return Returns the up vector of the camera.
const core::vector3df& ICameraObject::getUpVector() const
{
	return UpVector;
}


f32 ICameraObject::getNearValue() const
{
	return ZNear;
}


f32 ICameraObject::getFarValue() const
{
	return ZFar;
}


f32 ICameraObject::getAspectRatio() const
{
	return Aspect;
}


f32 ICameraObject::getFOV() const
{
	return Fovy;
}


void ICameraObject::setNearValue(f32 f)
{
	ZNear = f;
	recalculateProjectionMatrix();
	ViewArea.setFarNearDistance(ZFar - ZNear);
}


void ICameraObject::setFarValue(f32 f)
{
	ZFar = f;
	recalculateProjectionMatrix();
	ViewArea.setFarNearDistance(ZFar - ZNear);
}


void ICameraObject::setAspectRatio(f32 f)
{
	Aspect = f;
	recalculateProjectionMatrix();
}


void ICameraObject::setFOV(f32 f)
{
	Fovy = f;
	recalculateProjectionMatrix();
}


void ICameraObject::recalculateProjectionMatrix()
{
	ViewArea.getTransform ( video::ETS_PROJECTION ).buildProjectionMatrixPerspectiveFovLH(Fovy, Aspect, ZNear, ZFar, HasD3DStyleProjectionMatrix);
	IsOrthogonal = false;
}




//! render
void ICameraObject::OnRender(IShader* shader)
{
	IGameObject::OnRender(shader);

}
void ICameraObject::OnUpdate(u32 timeMs)
{
  IGameObject::OnUpdate(timeMs);
  updateMatrices();
}


//! update
void ICameraObject::updateMatrices()
{
	core::vector3df pos = Transform->getAbsolutePosition();
	core::vector3df tgtv = Target - pos;
	tgtv.normalize();

	// if upvector and vector to the target are the same, we have a
	// problem. so solve this problem:
	core::vector3df up = UpVector;
	up.normalize();

	f32 dp = tgtv.dotProduct(up);

	if ( core::equals(core::abs_<f32>(dp), 1.f) )
	{
		up.X += 0.5f;
	}

	ViewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(pos, Target, up);
	ViewArea.getTransform(video::ETS_VIEW) *= Affector;
	recalculateViewArea();
}



//! returns the view frustum.
const SViewFrustum* ICameraObject::getViewFrustum() const
{
	return &ViewArea;
}


void ICameraObject::recalculateViewArea()
{
	ViewArea.cameraPosition = Transform->getAbsolutePosition();

	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
	m.setbyproduct_nocheck(ViewArea.getTransform(video::ETS_PROJECTION),
						ViewArea.getTransform(video::ETS_VIEW));
	ViewArea.setFrom(m, HasD3DStyleProjectionMatrix);
}


//! Set the binding between the camera's rotation adn target.
void ICameraObject::bindTargetAndRotation(bool bound)
{
	TargetAndRotationAreBound = bound;
}


//! Gets the binding between the camera's rotation and target.
bool ICameraObject::getTargetAndRotationBinding(void) const
{
	return TargetAndRotationAreBound;
}

} // end namespace scene
} // end namespace irr



