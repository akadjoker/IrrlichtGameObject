#include "IMeshAnimator.h"
#include "CMeshAnimator.h"

#include "S3DVertex.h"
#include "os.h"
#include "IAnimatedMeshMD3.h"
#include "CSkinnedMesh.h"
#include "IMesh.h"
#include "IAnimatedMesh.h"
#include "quaternion.h"


namespace irr
{
namespace scene
{


CMeshAnimator::CMeshAnimator(IAnimatedMesh* mesh):StartFrame(0), EndFrame(0), FramesPerSecond(0.025f),
	CurrentFrameNr(0.f), LastTimeMs(0),
	TransitionTime(0), Transiting(0.f), TransitingBlend(0.f), MD3Special(0),
	Looping(true), ReadOnlyMaterials(false), RenderFromIdentity(false), PassCount(0)
{
setMesh(mesh);
setDebugName("MeshAnimator");
}

CMeshAnimator::~CMeshAnimator()
{
if (MD3Special)
		delete MD3Special;

if (Mesh)
		Mesh->drop();


}

void CMeshAnimator::setCurrentFrame(f32 frame)
{
// if you pass an out of range value, we just clamp it
	CurrentFrameNr = core::clamp ( frame, (f32)StartFrame, (f32)EndFrame );

	beginTransition(); //transit to this frame if enabled
}

void CMeshAnimator::OnAnimate(u32 timeMs)
{
if (LastTimeMs==0)	// first frame
	{
		LastTimeMs = timeMs;
	}

	// set CurrentFrameNr
	buildFrameNr(timeMs-LastTimeMs);

	// update bbox
	if (Mesh)
	{
		scene::IMesh * mesh = getMeshFrame();

		if (mesh)
			Box = mesh->getBoundingBox();
	}
	LastTimeMs = timeMs;

//	IMeshAnimator::OnAnimate(timeMs);

/*
	if (!Mesh || Mesh->getMeshType() == EAMT_MD3)
	{


        SMD3QuaternionTagList *taglist;
        taglist = ( (IAnimatedMeshMD3*) Mesh )->getTagList ( (s32)getFrameNr(),255,getStartFrame (),getEndFrame () );
        if (taglist)
        {

            if (!MD3Special)
            {
                MD3Special = new SMD3Special();
            }

            SMD3QuaternionTag parent ( MD3Special->Tagname );/*
            if (Parent && Parent->getType() == ESNT_ANIMATED_MESH)
            {
                const SMD3QuaternionTag * p = ((IMeshAnimator*) Parent)->getMD3TagTransformation
                                        ( MD3Special->Tagname );

                if (p)
                    parent = *p;
            }

           / SMD3QuaternionTag relative( RelativeTranslation, RelativeRotation );

            MD3Special->AbsoluteTagList.set_used ( taglist->size () );
            for ( u32 i=0; i!= taglist->size (); ++i )
            {
                MD3Special->AbsoluteTagList[i].position = parent.position + (*taglist)[i].position + relative.position;
                MD3Special->AbsoluteTagList[i].rotation = parent.rotation * (*taglist)[i].rotation * relative.rotation;
            }
        }*/

}



const core::aabbox3d<f32>& CMeshAnimator::getBoundingBox() const
{
return Box;
}

bool CMeshAnimator::setFrameLoop(s32 begin, s32 end)
{
const s32 maxFrameCount = Mesh->getFrameCount() - 1;
	if (end < begin)
	{
		StartFrame = core::s32_clamp(end, 0, maxFrameCount);
		EndFrame = core::s32_clamp(begin, StartFrame, maxFrameCount);
	}
	else
	{
		StartFrame = core::s32_clamp(begin, 0, maxFrameCount);
		EndFrame = core::s32_clamp(end, StartFrame, maxFrameCount);
	}
	if (FramesPerSecond < 0)
		setCurrentFrame((f32)EndFrame);
	else
		setCurrentFrame((f32)StartFrame);

	return true;
}

void CMeshAnimator::setLoopMode(bool playAnimationLooped)
{
Looping = playAnimationLooped;
}

bool CMeshAnimator::getLoopMode() const
{
return Looping;
}

void CMeshAnimator::setAnimationSpeed(f32 framesPerSecond)
{
FramesPerSecond = framesPerSecond * 0.001f;
}

f32 CMeshAnimator::getAnimationSpeed() const
{
return FramesPerSecond * 1000.f;
}


bool CMeshAnimator::setMD2Animation(EMD2_ANIMATION_TYPE anim)
{
if (!Mesh || Mesh->getMeshType() != EAMT_MD2)
		return false;

	IAnimatedMeshMD2* md = (IAnimatedMeshMD2*)Mesh;

	s32 begin, end, speed;
	md->getFrameLoop(anim, begin, end, speed);

	setAnimationSpeed( f32(speed) );
	setFrameLoop(begin, end);
	return true;
}

bool CMeshAnimator::setMD2Animation(const c8* animationName)
{
if (!Mesh || Mesh->getMeshType() != EAMT_MD2)
		return false;

	IAnimatedMeshMD2* md = (IAnimatedMeshMD2*)Mesh;

	s32 begin, end, speed;
	if (!md->getFrameLoop(animationName, begin, end, speed))
		return false;

	setAnimationSpeed( (f32)speed );
	setFrameLoop(begin, end);
	return true;
}

f32 CMeshAnimator::getFrameNr() const
{
	return CurrentFrameNr;
}
u32 CMeshAnimator::getCurrentFrame() const
{
return CurrentFrame;
}
s32 CMeshAnimator::getStartFrame() const
{
return StartFrame;
}

s32 CMeshAnimator::getEndFrame() const
{
return EndFrame;
}



void CMeshAnimator::setMesh(IAnimatedMesh* mesh)
{
if (!mesh)
		return; // won't set null mesh

	if (Mesh != mesh)
	{
		//if (Mesh)
		//	Mesh->drop();

		Mesh = mesh;

		// grab the mesh (it's non-null!)
		//Mesh->grab();
	}

	// get materials and bounding box
	Box = Mesh->getBoundingBox();

	// get start and begin time
	setAnimationSpeed(Mesh->getAnimationSpeed());	// NOTE: This had been commented out (but not removed!) in r3526. Which caused meshloader-values for speed to be ignored unless users specified explicitly. Missing a test-case where this could go wrong so I put the code back in.
	setFrameLoop(0, Mesh->getFrameCount()-1);
}


void CMeshAnimator::setTransitionTime(f32 time)
{

	const u32 ttime = (u32)core::floor32(time*1000.0f);
	if (TransitionTime==ttime)
		return;
	TransitionTime = ttime;
	/*if (ttime != 0)
		setJointMode(EJUOR_CONTROL);
	else
		setJointMode(EJUOR_NONE);*/

}

IMesh* CMeshAnimator::getMeshFrame()
{
        s32 frameNr = (s32) getFrameNr();
		s32 frameBlend = (s32) (core::fract ( getFrameNr() ) * 1000.f);

		CurrentFrame=(u32)frameNr;

		return Mesh->getMesh(frameNr, frameBlend, StartFrame, EndFrame);
}

void CMeshAnimator::buildFrameNr(u32 timeMs)
{
if (Transiting!=0.f)
	{
		TransitingBlend += (f32)(timeMs) * Transiting;
		if (TransitingBlend > 1.f)
		{
			Transiting=0.f;
			TransitingBlend=0.f;
		}
	}

	if (StartFrame==EndFrame)
	{
		CurrentFrameNr = (f32)StartFrame; //Support for non animated meshes
	}
	else if (Looping)
	{
		// play animation looped
		CurrentFrameNr += timeMs * FramesPerSecond;

		// We have no interpolation between EndFrame and StartFrame,
		// the last frame must be identical to first one with our current solution.
		if (FramesPerSecond > 0.f) //forwards...
		{
			if (CurrentFrameNr > EndFrame)
				CurrentFrameNr = StartFrame + fmodf(CurrentFrameNr - StartFrame, (f32)(EndFrame-StartFrame));
		}
		else //backwards...
		{
			if (CurrentFrameNr < StartFrame)
				CurrentFrameNr = EndFrame - fmodf(EndFrame - CurrentFrameNr, (f32)(EndFrame-StartFrame));
		}
	}
	else
	{
		// play animation non looped

		CurrentFrameNr += timeMs * FramesPerSecond;
		if (FramesPerSecond > 0.f) //forwards...
		{
			if (CurrentFrameNr > (f32)EndFrame)
			{
				CurrentFrameNr = (f32)EndFrame;
				//if (LoopCallBack)
				//	LoopCallBack->OnAnimationEnd(this);
			}
		}
		else //backwards...
		{
			if (CurrentFrameNr < (f32)StartFrame)
			{
				CurrentFrameNr = (f32)StartFrame;
				//if (LoopCallBack)
				//	LoopCallBack->OnAnimationEnd(this);
			}
		}
	}
}

void CMeshAnimator::checkJoints()
{

}


// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
// or the absolutetransformation if it's a normal scenenode
const SMD3QuaternionTag* CMeshAnimator::getMD3TagTransformation(const core::stringc& tagname)
{
	return MD3Special ? MD3Special->AbsoluteTagList.get(tagname) : 0;
}


// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
// or the absolutetransformation if it's a normal scenenode
const SMD3QuaternionTag* CMeshAnimator::getMD3Tag(const core::stringc& tagname)
{

SMD3QuaternionTag* tag=nullptr;

	if (!Mesh || Mesh->getMeshType() == EAMT_MD3)
	{


        SMD3QuaternionTagList *taglist;
        taglist = ( (IAnimatedMeshMD3*) Mesh )->getTagList ( (s32)getFrameNr(),255,getStartFrame (),getEndFrame () );
        if (taglist)
        {

           taglist->get(tagname);


            for ( u32 i=0; i!= taglist->size (); ++i )
            {
                SDL_Log("%d %s ",i,(*taglist)[i].Name.c_str());

            }

        }
    }

return tag;
}


// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
// or the absolutetransformation if it's a normal scenenode
 const SMD3QuaternionTag* CMeshAnimator::getMD3Tag(u32 index)
{

SMD3QuaternionTag* tag=nullptr;

	if (!Mesh || Mesh->getMeshType() == EAMT_MD3)
	{


        SMD3QuaternionTagList *taglist;
        taglist = ( (IAnimatedMeshMD3*) Mesh )->getTagList ( (s32)getFrameNr(),255,getStartFrame (),getEndFrame () );
        if (taglist)
        {
          tag   =  taglist->get(index);

            //SDL_Log("%s %f %f %f",tag->Name.c_str(),tag->position.X,tag->position.Y,tag->position.Z);

          return tag;
        }
    }

return tag;
}


void CMeshAnimator::beginTransition()
{
/*
if (!JointsUsed)
		return;

	if (TransitionTime != 0)
	{
		//Check the array is big enough
		if (PretransitingSave.size()<JointChildSceneNodes.size())
		{
			for(u32 n=PretransitingSave.size(); n<JointChildSceneNodes.size(); ++n)
				PretransitingSave.push_back(core::matrix4());
		}

		//Copy the position of joints
		for (u32 n=0;n<JointChildSceneNodes.size();++n)
			PretransitingSave[n]=JointChildSceneNodes[n]->getRelativeTransformation();

		Transiting = core::reciprocal((f32)TransitionTime);
	}
	TransitingBlend = 0.f;
*/
}

}
}
