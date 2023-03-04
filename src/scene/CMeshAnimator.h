#ifndef CMESHANIMATOR_H
#define CMESHANIMATOR_H

#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "irrList.h"

#include "IAnimatedMeshMD2.h"
#include "IAnimatedMeshMD3.h"

namespace irr
{
namespace scene
{

class CMeshAnimator : public IMeshAnimator
	{
	public:

		//! constructor
		CMeshAnimator(IAnimatedMesh* mesh);
		//! destructor
		virtual ~CMeshAnimator();

		//! sets the current frame. from now on the animation is played from this frame.
		virtual void setCurrentFrame(f32 frame) _IRR_OVERRIDE_;

		//! OnAnimate() is called just before rendering the whole scene.
		virtual void OnAnimate(u32 timeMs) _IRR_OVERRIDE_;

		//! renders the node.


		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		//! sets the frames between the animation is looped.
		//! the default is 0 - MaximalFrameCount of the mesh.
		//! NOTE: setMesh will also change this value and set it to the full range of animations of the mesh
		virtual bool setFrameLoop(s32 begin, s32 end) _IRR_OVERRIDE_;

		//! Sets looping mode which is on by default. If set to false,
		//! animations will not be looped.
		virtual void setLoopMode(bool playAnimationLooped) _IRR_OVERRIDE_;

		//! returns the current loop mode
		virtual bool getLoopMode() const _IRR_OVERRIDE_;


		//! sets the speed with which the animation is played
		//! NOTE: setMesh will also change this value and set it to the default speed of the mesh
		virtual void setAnimationSpeed(f32 framesPerSecond) _IRR_OVERRIDE_;

		//! gets the speed with which the animation is played
		virtual f32 getAnimationSpeed() const _IRR_OVERRIDE_;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hierarchy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.


		virtual u32 getCurrentFrame() const _IRR_OVERRIDE_;


		//! Starts a MD2 animation.
		virtual bool setMD2Animation(EMD2_ANIMATION_TYPE anim) _IRR_OVERRIDE_;

		//! Starts a special MD2 animation.
		virtual bool setMD2Animation(const c8* animationName) _IRR_OVERRIDE_;

		//! Returns the current displayed frame number.
		virtual f32 getFrameNr() const _IRR_OVERRIDE_;
		//! Returns the current start frame number.
		virtual s32 getStartFrame() const _IRR_OVERRIDE_;
		//! Returns the current end frame number.
		virtual s32 getEndFrame() const _IRR_OVERRIDE_;


		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) _IRR_OVERRIDE_;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) _IRR_OVERRIDE_ { return Mesh; }


		//! Sets the transition time in seconds (note: This needs to enable joints, and setJointmode maybe set to 2)
		//! you must call animateJoints(), or the mesh will not animate
		virtual void setTransitionTime(f32 Time) _IRR_OVERRIDE_;


		// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
		// or the absolutetransformation if it's a normal scenenode
		const SMD3QuaternionTag* getMD3TagTransformation( const core::stringc & tagname) _IRR_OVERRIDE_;
		const SMD3QuaternionTag* getMD3Tag(const core::stringc& tagname)_IRR_OVERRIDE_;
		const SMD3QuaternionTag* getMD3Tag(u32 index)_IRR_OVERRIDE_;



	private:

			//! Get a static mesh for the current frame of this animated mesh
		 IMesh* getMeshFrame();


		void buildFrameNr(u32 timeMs);
		void checkJoints();
		void beginTransition();


		bool Looping;
		bool ReadOnlyMaterials;
		bool RenderFromIdentity;
		s32 PassCount;

		core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		IAnimatedMesh* Mesh;

		s32 StartFrame;
		s32 EndFrame;
		f32 FramesPerSecond;
		f32 CurrentFrameNr;
		u32 CurrentFrame;

		u32 LastTimeMs;
		u32 TransitionTime; //Transition time in millisecs
		f32 Transiting; //is mesh transiting (plus cache of TransitionTime)
		f32 TransitingBlend; //0-1, calculated on buildFrameNr


		struct SMD3Special
		{
			core::stringc Tagname;
			SMD3QuaternionTagList AbsoluteTagList;

			SMD3Special & operator = (const SMD3Special & copyMe)
			{
				Tagname = copyMe.Tagname;
				AbsoluteTagList = copyMe.AbsoluteTagList;
				return *this;
			}
		};
		SMD3Special *MD3Special;


	};

}
}
#endif // IMESHANIMATOR_H
