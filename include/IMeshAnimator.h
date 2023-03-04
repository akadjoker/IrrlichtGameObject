#ifndef IMESHANIMATOR_H
#define IMESHANIMATOR_H

#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "irrList.h"
#include "IReferenceCounted.h"
#include "IAnimatedMeshMD2.h"
#include "IAnimatedMeshMD3.h"

namespace irr
{
namespace scene
{

class IMeshAnimator : public virtual IReferenceCounted
{
    public:


        //! Sets the current frame number.
		/** From now on the animation is played from this frame.
		\param frame: Number of the frame to let the animation be started from.
		The frame number must be a valid frame number of the IMesh used by this
		scene node. Set IAnimatedMesh::getMesh() for details. */
		virtual void setCurrentFrame(f32 frame) = 0;

		//! Sets the frame numbers between the animation is looped.
		/** The default is 0 to getFrameCount()-1 of the mesh.
		Number of played frames is end-start.
		It interpolates toward the last frame but stops when it is reached.
		It does not interpolate back to start even when looping.
		Looping animations should ensure last and first frame-key are identical.
		\param begin: Start frame number of the loop.
		\param end: End frame number of the loop.
		\return True if successful, false if not. */
		virtual bool setFrameLoop(s32 begin, s32 end) = 0;

		//! Sets the speed with which the animation is played.
		/** \param framesPerSecond: Frames per second played. */
		virtual void setAnimationSpeed(f32 framesPerSecond) = 0;

		//! Gets the speed with which the animation is played.
		/** \return Frames per second played. */
		virtual f32 getAnimationSpeed() const =0;




		//! Starts a default MD2 animation.
		/** With this method it is easily possible to start a Run,
		Attack, Die or whatever animation, if the mesh contained in
		this scene node is an md2 mesh. Otherwise, nothing happens.
		\param anim: An MD2 animation type, which should be played, for
		example EMAT_STAND for the standing animation.
		\return True if successful, and false if not, for example if
		the mesh in the scene node is not a md2 mesh. */
		virtual bool setMD2Animation(EMD2_ANIMATION_TYPE anim) = 0;

		//! Starts a special MD2 animation.
		/** With this method it is easily possible to start a Run,
		Attack, Die or whatever animation, if the mesh contained in
		this scene node is an md2 mesh. Otherwise, nothing happens.
		This method uses a character string to identify the animation.
		If the animation is a standard md2 animation, you might want to
		start this animation with the EMD2_ANIMATION_TYPE enumeration
		instead.
		\param animationName: Name of the animation which should be
		played.
		\return Returns true if successful, and false if not, for
		example if the mesh in the scene node is not an md2 mesh, or no
		animation with this name could be found. */
		virtual bool setMD2Animation(const c8* animationName) = 0;

		//! Returns the currently displayed frame number.
		virtual f32 getFrameNr() const = 0;
		//! Returns the current start frame number.
		virtual s32 getStartFrame() const = 0;
		//! Returns the current end frame number.
		virtual s32 getEndFrame() const = 0;

        virtual u32 getCurrentFrame() const = 0;

		//! Sets looping mode which is on by default.
		/** If set to false, animations will not be played looped. */
		virtual void setLoopMode(bool playAnimationLooped) = 0;

		//! returns the current loop mode
		/** When true the animations are played looped */
		virtual bool getLoopMode() const = 0;


		//! Get the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh, or the absolutetransformation if it's a normal scenenode
		virtual const SMD3QuaternionTag* getMD3TagTransformation( const core::stringc & tagname) = 0;
		virtual const SMD3QuaternionTag* getMD3Tag(const core::stringc& tagname)=0;
		virtual const SMD3QuaternionTag* getMD3Tag(u32 index)=0;


		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) = 0;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) = 0;

		//! Sets the transition time in seconds
		/** Note: This needs to enable joints, and setJointmode set to
		EJUOR_CONTROL. You must call animateJoints(), or the mesh will
		not animate. */
		virtual void setTransitionTime(f32 Time) =0;

        virtual void OnAnimate(u32 timeMs)=0;
        virtual const core::aabbox3d<f32>& getBoundingBox() const =0;


		//! Creates a clone of this scene node and its children.
		/** \param newParent An optional new parent.
		\param newManager An optional new scene manager.
		\return The newly created clone of this node. */
		//virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) = 0;


};

}
}
#endif // IMESHANIMATOR_H
