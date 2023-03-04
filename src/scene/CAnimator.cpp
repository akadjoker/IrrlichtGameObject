
#include "CSkinnedMesh.h"
#include "IGameObject.h"
#include "Engine.h"



namespace irr
{
namespace scene
{


IBoneValue::IBoneValue(u32 boneIndex, const c8* boneName)
: BoneIndex(boneIndex),AnimationMode(EBAM_AUTOMATIC), SkinningSpace(EBSS_LOCAL)
{

}

//! Returns the index of the bone
u32 IBoneValue::getBoneIndex() const
{
	return BoneIndex;
}
//! How the relative transformation of the bone is used
void IBoneValue::setSkinningSpace(E_BONE_SKINNING_SPACE space)
{
    SkinningSpace=space;
}

E_BONE_SKINNING_SPACE IBoneValue::getSkinningSpace() const
{
    return SkinningSpace;
}

//! Sets the animation mode of the bone. Returns true if successful.
bool IBoneValue::setAnimationMode(E_BONE_ANIMATION_MODE mode)
{
	AnimationMode = mode;
	return true;
}


//! Gets the current animation mode of the bone
E_BONE_ANIMATION_MODE IBoneValue::getAnimationMode() const
{
	return AnimationMode;
}





void IBoneValue::OnUpdate(u32 timeMs)
{
updateAbsolutePositionOfAllChildren();

}


void IBoneValue::helper_updateAbsolutePositionOfAllChildren(IGameObject *Node)
{
/*	Node->Transform->updateAbsolutePosition();

	IGameObjectList::ConstIterator it = Node->getChildren().begin();
	for (; it != Node->getChildren().end(); ++it)
	{
		helper_updateAbsolutePositionOfAllChildren( (*it) );
	}*/
}


void IBoneValue::updateAbsolutePositionOfAllChildren()
{
	helper_updateAbsolutePositionOfAllChildren( m_parent );
}
//************************************************************************
IAnimator::IAnimator(scene::IAnimatedMesh* mesh)  :IComponent(),Mesh(0),StartFrame(0), EndFrame(10), FramesPerSecond(0.025f),
	CurrentFrameNr(0.f), LastTimeMs(0),
	TransitionTime(0), Transiting(0.f), TransitingBlend(0.f),
	JointMode(EJUOR_NONE), JointsUsed(false),
	Looping(true), ReadOnlyMaterials(false), RenderFromIdentity(false),MD3Special(0)
{
setMesh(mesh);


};



void IAnimator::setCurrentFrame(f32 frame)
{

// if you pass an out of range value, we just clamp it
	CurrentFrameNr = core::clamp ( frame, (f32)StartFrame, (f32)EndFrame );

	beginTransition(); //transit to this frame if enabled



}

void IAnimator::OnUpdate(u32 timeMs)
{
   updateAbsolutePosition();

   if (LastTimeMs==0)	// first frame
	{
		LastTimeMs = timeMs;
	}

	// set CurrentFrameNr
	buildFrameNr(timeMs-LastTimeMs);


	// update bbox
	if (Mesh)
	{
		scene::IMesh * mesh = getMeshForCurrentFrame();

		if (mesh)
			Box = mesh->getBoundingBox();
	}
	LastTimeMs = timeMs;



}



bool IAnimator::setFrameLoop(s32 begin, s32 end)
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

void IAnimator::setLoopMode(bool playAnimationLooped)
{
Looping = playAnimationLooped;
}

bool IAnimator::getLoopMode() const
{
return Looping;
}

void IAnimator::setAnimationSpeed(f32 framesPerSecond)
{
FramesPerSecond = framesPerSecond * 0.001f;
}

f32 IAnimator::getAnimationSpeed() const
{
return FramesPerSecond * 1000.f;
}

u32 IAnimator::getCurrentFrame() const
{
}

bool IAnimator::setMD2Animation(EMD2_ANIMATION_TYPE anim)
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

bool IAnimator::setMD2Animation(const c8* animationName)
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
//! updates the absolute position based on the relative and the parents position
void IAnimator::updateAbsolutePosition()
{


	if (!Mesh || Mesh->getMeshType() != EAMT_MD3)
		return;

	SMD3QuaternionTagList *taglist;
	taglist = ( (IAnimatedMeshMD3*) Mesh )->getTagList ( (s32)getFrameNr(),255,getStartFrame (),getEndFrame () );
	if (taglist)
	{
		if (!MD3Special)
		{
			MD3Special = new SMD3Special();
		}

		SMD3QuaternionTag parent ( MD3Special->Tagname );

			const SMD3QuaternionTag * p = getMD3TagTransformation( MD3Special->Tagname );

			if (p)
				parent = *p;


		SMD3QuaternionTag relative(m_parent->Transform->getPosition(), m_parent->Transform->getRotation() );

		MD3Special->AbsoluteTagList.set_used ( taglist->size () );
		for ( u32 i=0; i!= taglist->size (); ++i )
		{
			MD3Special->AbsoluteTagList[i].position = parent.position + (*taglist)[i].position + relative.position;
			MD3Special->AbsoluteTagList[i].rotation = parent.rotation * (*taglist)[i].rotation * relative.rotation;
		}
	}
}
f32 IAnimator::getFrameNr() const
{
return CurrentFrameNr;
}

s32 IAnimator::getStartFrame() const
{
	return StartFrame;
}

s32 IAnimator::getEndFrame() const
{
return EndFrame;
}

void IAnimator::setMesh(IAnimatedMesh* mesh)
{
	if (!mesh)
		return; // won't set null mesh

	if (Mesh != mesh)
	{
		Mesh = mesh;
	}

	// get materials and bounding box
	Box = Mesh->getBoundingBox();

	IMesh* m = Mesh->getMesh(0,0);
	if (m)
	{
		Materials.clear();
		Materials.reallocate(m->getMeshBufferCount());

		for (u32 i=0; i<m->getMeshBufferCount(); ++i)
		{
			IMeshBuffer* mb = m->getMeshBuffer(i);
			if (mb)
				Materials.push_back(mb->getMaterial());
			else
				Materials.push_back(video::SMaterial());
		}
	}

	// clean up joint nodes
	if (JointsUsed)
	{
		JointsUsed=false;
		checkJoints();
	}

	// get start and begin time
	setAnimationSpeed(Mesh->getAnimationSpeed());	// NOTE: This had been commented out (but not removed!) in r3526. Which caused meshloader-values for speed to be ignored unless users specified explicitly. Missing a test-case where this could go wrong so I put the code back in.
	setFrameLoop(0, Mesh->getFrameCount()-1);

	SDL_Log("speed %f ",Mesh->getAnimationSpeed());
	SDL_Log("frames %d ",Mesh->getFrameCount());
}



void IAnimator::setTransitionTime(f32 Time)
{
     const u32 ttime = (u32)core::floor32(Time*1000.0f);
	if (TransitionTime==ttime)
		return;
	TransitionTime = ttime;
	if (ttime != 0)
		setJointMode(EJUOR_CONTROL);
	else
		setJointMode(EJUOR_NONE);
}

void IAnimator::setJointMode(E_JOINT_UPDATE_ON_RENDER mode)
{
    checkJoints();
	JointMode=mode;
}



void IAnimator::animateJoints(bool CalculateAbsolutePositions)
{
return;

if (Mesh && Mesh->getMeshType() == EAMT_SKINNED )
	{
		checkJoints();
		const f32 frame = getFrameNr(); //old?

		CSkinnedMesh* skinnedMesh=reinterpret_cast<CSkinnedMesh*>(Mesh);

		skinnedMesh->transferOnlyJointsHintsToMesh( JointChildSceneNodes );
		skinnedMesh->animateMesh(frame, 1.0f);
		skinnedMesh->recoverJointsFromMesh( JointChildSceneNodes);

		//-----------------------------------------
		//		Transition
		//-----------------------------------------

		if (Transiting != 0.f)
		{
			// Init additional matrices
			if (PretransitingSave.size()<JointChildSceneNodes.size())
			{
				for(u32 n=PretransitingSave.size(); n<JointChildSceneNodes.size(); ++n)
					PretransitingSave.push_back(core::matrix4());
			}

			for (u32 n=0; n<JointChildSceneNodes.size(); ++n)
			{
				//------Position------

				JointChildSceneNodes[n]->Transform->setPosition(
						core::lerp(
							PretransitingSave[n].getTranslation(),
							JointChildSceneNodes[n]->Transform->getPosition(),
							TransitingBlend));

				//------Rotation------

				//Code is slow, needs to be fixed up

				const core::quaternion RotationStart(PretransitingSave[n].getRotationDegrees()*core::DEGTORAD);
				const core::quaternion RotationEnd(JointChildSceneNodes[n]->Transform->getRotation()*core::DEGTORAD);

				core::quaternion QRotation;
				QRotation.slerp(RotationStart, RotationEnd, TransitingBlend);

				core::vector3df tmpVector;
				QRotation.toEuler(tmpVector);
				tmpVector*=core::RADTODEG; //convert from radians back to degrees
				JointChildSceneNodes[n]->Transform->setRotation( tmpVector );

				//------Scale------

				//JointChildSceneNodes[n]->setScale(
				//		core::lerp(
				//			PretransitingSave[n].getScale(),
				//			JointChildSceneNodes[n]->getScale(),
				//			TransitingBlend));
			}
		}

		if (CalculateAbsolutePositions)
		{
			//---slow---
			/*for (u32 n=0;n<JointChildSceneNodes.size();++n)
			{
				if (JointChildSceneNodes[n]->getParent()==m_parent)
				{
				//	JointChildSceneNodes[n]->updateAbsolutePositionOfAllChildren(); //temp, should be an option
				}
			}*/
		}
	}
}

void IAnimator::setRenderFromIdentity( bool On )
{
RenderFromIdentity=On;
}

const SMD3QuaternionTag* IAnimator::getMD3TagTransformation( const core::stringc & tagname)
{
return MD3Special ? MD3Special->AbsoluteTagList.get(tagname) : 0;
}

const SMD3QuaternionTag* IAnimator::getMD3Tag(const core::stringc& tagname)
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

const SMD3QuaternionTag* IAnimator::getMD3Tag(u32 index)
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


IGameObject* IAnimator::getJointNode(const c8* jointName)
{
if (!Mesh || Mesh->getMeshType() != EAMT_SKINNED)
	{
		os::Printer::log("No mesh, or mesh not of skinned mesh type", ELL_WARNING);
		return 0;
	}

	checkJoints();

	ISkinnedMesh *skinnedMesh=(ISkinnedMesh*)Mesh;

	const s32 number = skinnedMesh->getJointNumber(jointName);

	if (number == -1)
	{
		os::Printer::log("Joint with specified name not found in skinned mesh", jointName, ELL_DEBUG);
		return 0;
	}

	if ((s32)JointChildSceneNodes.size() <= number)
	{
		os::Printer::log("Joint was found in mesh, but is not loaded into node", jointName, ELL_WARNING);
		return 0;
	}

	return JointChildSceneNodes[number];
}


IGameObject* IAnimator::getJointNode(u32 jointID)
{
	if (!Mesh || Mesh->getMeshType() != EAMT_SKINNED)
	{
		os::Printer::log("No mesh, or mesh not of skinned mesh type", ELL_WARNING);
		return 0;
	}

	checkJoints();

	if (JointChildSceneNodes.size() <= jointID)
	{
		os::Printer::log("Joint not loaded into node", ELL_WARNING);
		return 0;
	}



	return JointChildSceneNodes[jointID];
}

//! Gets joint count.
u32 IAnimator::getJointCount() const
{
	if (!Mesh || Mesh->getMeshType() != EAMT_SKINNED)
		return 0;

	ISkinnedMesh *skinnedMesh=(ISkinnedMesh*)Mesh;

	return skinnedMesh->getJointCount();
}

IMesh* IAnimator::getMeshForCurrentFrame()
{
    if(Mesh->getMeshType() != EAMT_SKINNED)
	{

		s32 frameNr = (s32) getFrameNr();
		s32 frameBlend = (s32) (core::fract ( getFrameNr() ) * 1000.f);
		return Mesh->getMesh(frameNr, frameBlend, StartFrame, EndFrame);
	}
	else
	{


		// As multiple scene nodes may be sharing the same skinned mesh, we have to
		// re-animate it every frame to ensure that this node gets the mesh that it needs.

		CSkinnedMesh* skinnedMesh = reinterpret_cast<CSkinnedMesh*>(Mesh);

		if (JointMode == EJUOR_CONTROL)//write to mesh
			skinnedMesh->transferJointsToMesh(JointChildSceneNodes);
		else
			skinnedMesh->animateMesh(getFrameNr(), 1.0f);



		// Update the skinned mesh for the current joint transforms.
		skinnedMesh->skinMesh();

		if (JointMode == EJUOR_READ)//read from mesh
		{
			skinnedMesh->recoverJointsFromMesh(JointChildSceneNodes);

			//---slow---
			for (u32 n=0;n<JointChildSceneNodes.size();++n)
				if (JointChildSceneNodes[n]->getParent()==m_parent)
				{
				     std::shared_ptr<IBoneValue> value = m_parent->getComponent<IBoneValue>();
                    if (value.get())
                    {
                    }

/*
					    JointChildSceneNodes[n]->updateAbsolutePositionOfAllChildren(); //temp, should be an option
						if (value.get())
                            {
                            value.get()->updateAbsolutePositionOfAllChildren();
                            }*/
				}
		}

		if(JointMode == EJUOR_CONTROL)
		{
			// For meshes other than EJUOR_CONTROL, this is done by calling animateMesh()
			skinnedMesh->updateBoundingBox();
		}

		return skinnedMesh;

	}
}

void IAnimator::buildFrameNr(u32 timeMs)
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
			//		LoopCallBack->OnAnimationEnd(this);
			}
		}
	}
}
void IAnimator::checkJoints()
{
	if (!Mesh || Mesh->getMeshType() != EAMT_SKINNED)
		return;

	if (!JointsUsed)
	{
		for (u32 i=0; i<JointChildSceneNodes.size(); ++i)
		{
		 m_parent->removeChild(JointChildSceneNodes[i]);
		}
		JointChildSceneNodes.clear();

		//Create joints for SkinnedMesh
		((CSkinnedMesh*)Mesh)->addJoints(JointChildSceneNodes, m_parent);
		((CSkinnedMesh*)Mesh)->recoverJointsFromMesh(JointChildSceneNodes);

		JointsUsed=true;
		JointMode=EJUOR_READ;
	}
}

void IAnimator::beginTransition()
{
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
			PretransitingSave[n]=JointChildSceneNodes[n]->Transform->getRelativeTransformation();

		Transiting = core::reciprocal((f32)TransitionTime);
	}
	TransitingBlend = 0.f;
}



}
}
