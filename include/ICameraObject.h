#ifndef __I_CAMERAOBJECT_H_INCLUDED
#define __I_CAMERAOBJECT_H_INCLUDED

#include "IReferenceCounted.h"
#include "ITransform.h"
#include "IGameObject.h"
#include "SViewFrustum.h"

namespace irr
{
namespace scene
{
struct SViewFrustum;
class ICameraObject : public IGameObject
{
public:


    ICameraObject();
    ICameraObject(IGameObject* parent,  s32 id);
    ICameraObject(const core::vector3df& position, const core::vector3df& lookat);
    ICameraObject(const core::vector3df& position, const core::vector3df& lookat, IGameObject* parent,  s32 id);

    ~ICameraObject();

    void OnUpdate(u32 timeMs) override;
    void OnRender(IShader* shader) override;


		//! Sets the look at target of the camera
		/** If the camera's target and rotation are bound ( @see
		bindTargetAndRotation() ) then calling this will also change
		the camera's scene node rotation to match the target.
		Note that setTarget uses the current absolute position
		internally, so if you changed setPosition since last rendering you must
		call updateAbsolutePosition before using this function.
		\param pos Look at target of the camera, in world co-ordinates. */
		 void setTarget(const core::vector3df& pos) ;

		//! Sets the rotation of the node.
		/** This only modifies the relative rotation of the node.
		If the camera's target and rotation are bound ( @see
		bindTargetAndRotation() ) then calling this will also change
		the camera's target to match the rotation.
		\param rotation New rotation of the node in degrees. */
		 void setRotation(const core::vector3df& rotation) ;

		//! Gets the current look at target of the camera
		/** \return The current look at target of the camera, in world co-ordinates */
		 const core::vector3df& getTarget() const ;

		//! Sets the up vector of the camera.
		/** \param pos: New upvector of the camera. */
		 void setUpVector(const core::vector3df& pos) ;

		//! Gets the up vector of the camera.
		/** \return The up vector of the camera, in world space. */
		 const core::vector3df& getUpVector() const ;

		//! Gets the value of the near plane of the camera.
		/** \return The value of the near plane of the camera. */
		f32 getNearValue() const ;

		//! Gets the value of the far plane of the camera.
		/** \return The value of the far plane of the camera. */
		 f32 getFarValue() const ;

		//! Gets the aspect ratio of the camera.
		/** \return The aspect ratio of the camera. */
		 f32 getAspectRatio() const;

		//! Gets the field of view of the camera.
		/** \return The field of view of the camera in radians. */
		 f32 getFOV() const ;

		//! Sets the value of the near clipping plane. (default: 1.0f)
		/** \param zn: New z near value. */
		 void setNearValue(f32 zn) ;

		//! Sets the value of the far clipping plane (default: 2000.0f)
		/** \param zf: New z far value. */
		 void setFarValue(f32 zf) ;

		//! Sets the aspect ratio (default: 4.0f / 3.0f)
		/** \param aspect: New aspect ratio. */
		 void setAspectRatio(f32 aspect) ;

		//! Sets the field of view (Default: PI / 2.5f)
		/** \param fovy: New field of view in radians. */
		 void setFOV(f32 fovy) ;

		//! Get the view frustum.
		/** \return The current view frustum. */
		 const SViewFrustum* getViewFrustum() const ;


		//! Checks if a camera is orthogonal.
		 bool isOrthogonal() const
		{
			return IsOrthogonal;
		}

		//! Binds the camera scene node's rotation to its target position and vice versa, or unbinds them.
		/** When bound, calling setRotation() will update the camera's
		target position to be along its +Z axis, and likewise calling
		setTarget() will update its rotation so that its +Z axis will
		point at the target point. FPS camera use this binding by
		default; other cameras do not.
		\param bound True to bind the camera's scene node rotation
		and targeting, false to unbind them.
		@see getTargetAndRotationBinding() */
		 void bindTargetAndRotation(bool bound) ;
		 bool getTargetAndRotationBinding(void) const;

		//! Updates the matrices without uploading them to the driver
		 void updateMatrices() ;

		 void setViewMatrixAffector(const core::matrix4& affector);
		 const core::matrix4& getViewMatrixAffector() const;

			//! Sets the projection matrix of the camera.
		/** The core::matrix4 class has some methods to build a
		projection matrix. e.g:
		core::matrix4::buildProjectionMatrixPerspectiveFovLH.
		Note that the matrix will only stay as set by this method until
		one of the following Methods are called: setNearValue,
		setFarValue, setAspectRatio, setFOV.
		NOTE: The frustum is not updated before render() is called
		unless you explicitly call updateMatrices()
		\param projection The new projection matrix of the camera.
		\param isOrthogonal Set this to true if the matrix is an
		orthogonal one (e.g. from matrix4::buildProjectionMatrixOrtho).
		*/
		 void setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal=false) ;

		//! Gets the current projection matrix of the camera.
		/** \return The current projection matrix of the camera. */
		 const core::matrix4& getProjectionMatrix() const ;

		//! Gets the current view matrix of the camera.
		/** \return The current view matrix of the camera. */
		 const core::matrix4& getViewMatrix() const ;




protected:
		bool IsOrthogonal;
		void recalculateProjectionMatrix();
		void recalculateViewArea();

		core::aabbox3d<f32> BoundingBox;

		core::vector3df Target;
		core::vector3df UpVector;

		f32 Fovy;	// Field of view, in radians.
		f32 Aspect;	// Aspect ratio.
		f32 ZNear;	// value of the near view-plane.
		f32 ZFar;	// Z-value of the far view-plane.

		SViewFrustum ViewArea;
		core::matrix4 Affector;

        bool TargetAndRotationAreBound;
		bool HasD3DStyleProjectionMatrix;	// true: projection from 0 to w; false: -w to w
};


} // end namespace scene
} // end namespace irr

#endif
