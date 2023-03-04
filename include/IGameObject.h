#ifndef __I_GAMEOBJECT_H_INCLUDED
#define __I_GAMEOBJECT_H_INCLUDED

#include "IReferenceCounted.h"
#include "ITransform.h"
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

#include "IAnimatedMeshMD2.h"
#include "IAnimatedMeshMD3.h"

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
class IGameObject;
typedef core::list<IGameObject*> IGameObjectList;



class IComponent
{
public:
IComponent(){
};
virtual ~IComponent(void){m_parent=nullptr;DebugDataVisible=0;};
virtual void OnUpdate(u32 timeMs){};
virtual void OnRender(IShader* shader){};
virtual void free(){};
virtual void init(){};

        //! Sets if debug data like bounding boxes should be drawn.
		/** A bitwise OR of the types from @ref irr::scene::E_DEBUG_SCENE_TYPE.
		Please note that not all scene nodes support all debug data types.
		\param state The debug data visibility state to be used. */
		 void setDebugDataVisible(u32 state)
		{
			DebugDataVisible = state;
		}

		//! Returns if debug data like bounding boxes are drawn.
		/** \return A bitwise OR of the debug data values from
		@ref irr::scene::E_DEBUG_SCENE_TYPE that are currently visible. */
		u32 isDebugDataVisible() const
		{
			return DebugDataVisible;
		}

const virtual  char *getType(void) = 0;
void setParent(IGameObject *parent){  m_parent = parent;};
IGameObject *getParent(void) const {  return m_parent;};
int    getPriority() const { return m_priority; }
void   setPriority(int priority) {  m_priority=priority; }
private:
protected:
  IGameObject *m_parent;
  int m_priority;
  u32 DebugDataVisible;
};

//********************************************************************
class IMeshRenderer : public IComponent
{
public:
  IMeshRenderer(scene::IMesh* mesh);
  virtual void OnRender(IShader* shader);
  const virtual char *getType(void) { return "MESHRENDERER"; }




    virtual const core::aabbox3d<f32>& getBoundingBox() const ;
    virtual video::SMaterial& getMaterial(u32 i) ;
    virtual u32 getMaterialCount() const ;
    virtual void free()
    {
      if(Mesh)
      {
        Mesh->drop();
        Mesh=0;
      }
    };

private:
  IMesh* Mesh;

        core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		video::SMaterial ReadOnlyMaterial;
};
//********************************************************************
class ICamera : public IComponent
{
public:
  ICamera();
  virtual void OnRender(IShader* shader);
  const virtual char *getType(void) { return "CAMERA"; }
  core::matrix4 getViewMatrix(void) const;
  core::matrix4 getProjMatrix(void) const;
private:

};

//********************************************************************
	//! Enumeration for different bone animation modes
	enum E_BONE_ANIMATION_MODE
	{
		//! The bone is usually animated, unless it's parent is not animated
		EBAM_AUTOMATIC=0,

		//! The bone is animated by the skin, if it's parent is not animated then animation will resume from this bone onward
		EBAM_ANIMATED,

		//! The bone is not animated by the skin
		EBAM_UNANIMATED,

		//! Not an animation mode, just here to count the available modes
		EBAM_COUNT

	};

	enum E_BONE_SKINNING_SPACE
	{
		//! local skinning, standard
		EBSS_LOCAL=0,

		//! global skinning
		EBSS_GLOBAL,

		EBSS_COUNT
	};

	//! Names for bone animation modes
	const c8* const BoneAnimationModeNames[] =
	{
		"automatic",
		"animated",
		"unanimated",
		0,
	};


	enum E_JOINT_UPDATE_ON_RENDER
	{
		//! do nothing
		EJUOR_NONE = 0,

		//! get joints positions from the mesh (for attached nodes, etc)
		EJUOR_READ,

		//! control joint positions in the mesh (eg. ragdolls, or set the animation from animateJoints() )
		EJUOR_CONTROL
	};


class IBoneValue : public IComponent
{
public:
  IBoneValue(u32 boneIndex, const c8* boneName);
  //! Get the index of the bone
		 u32 getBoneIndex() const ;

		//! Sets the animation mode of the bone.
		/** \return True if successful. (Unused) */
		 bool setAnimationMode(E_BONE_ANIMATION_MODE mode) ;

		//! Gets the current animation mode of the bone
		 E_BONE_ANIMATION_MODE getAnimationMode() const ;

		//! The animation method.
		 virtual void OnUpdate(u32 timeMs) _IRR_OVERRIDE_;

//! How the relative transformation of the bone is used
		 void setSkinningSpace(E_BONE_SKINNING_SPACE space) ;

		 E_BONE_SKINNING_SPACE getSkinningSpace() const ;



		//! Updates the absolute position based on the relative and the parents position
		 void updateAbsolutePositionOfAllChildren();

        const virtual char *getType(void) { return "IBoneValue"; }
public:

		s32 positionHint;
		s32 scaleHint;
		s32 rotationHint;


private:

void helper_updateAbsolutePositionOfAllChildren(IGameObject *Node);

	u32 BoneIndex;
		E_BONE_ANIMATION_MODE AnimationMode;
		E_BONE_SKINNING_SPACE SkinningSpace;
};
//********************************************************************
class IAnimator : public IComponent
{
public:
  IAnimator(scene::IAnimatedMesh* mesh);






  const virtual char *getType(void) { return "IANIMATOR"; }
  		//! sets the current frame. from now on the animation is played from this frame.
		 void setCurrentFrame(f32 frame) ;

		//! OnAnimate() is called just before rendering the whole scene.
		 virtual void OnUpdate(u32 timeMs) override;



		 		//! Returns a pointer to a child node, which has the same transformation as
		//! the corresponding joint, if the mesh in this scene node is a skinned mesh.
		 IGameObject* getJointNode(const c8* jointName) ;

		//! same as getJointNode(const c8* jointName), but based on id
		 IGameObject* getJointNode(u32 jointID) ;

		//! Gets joint count.
		 u32 getJointCount() const ;


		//! sets the frames between the animation is looped.
		//! the default is 0 - MaximalFrameCount of the mesh.
		//! NOTE: setMesh will also change this value and set it to the full range of animations of the mesh
		 bool setFrameLoop(s32 begin, s32 end) ;

		//! Sets looping mode which is on by default. If set to false,
		//! animations will not be looped.
		 void setLoopMode(bool playAnimationLooped) ;

		//! returns the current loop mode
		 bool getLoopMode() const ;


		//! sets the speed with which the animation is played
		//! NOTE: setMesh will also change this value and set it to the default speed of the mesh
		 void setAnimationSpeed(f32 framesPerSecond) ;

		//! gets the speed with which the animation is played
		 f32 getAnimationSpeed() const ;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hierarchy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.


		 u32 getCurrentFrame() const ;


		//! Starts a MD2 animation.
		 bool setMD2Animation(EMD2_ANIMATION_TYPE anim) ;

		//! Starts a special MD2 animation.
		 bool setMD2Animation(const c8* animationName) ;

		//! Returns the current displayed frame number.
		 f32 getFrameNr() const ;
		//! Returns the current start frame number.
		 s32 getStartFrame() const ;
		//! Returns the current end frame number.
		 s32 getEndFrame() const ;


		//! Sets a new mesh
		 void setMesh(IAnimatedMesh* mesh) ;

		//! Returns the current mesh
		 IAnimatedMesh* getMesh(void)  { return Mesh; };



		 //! Set how the joints should be updated on render
		 void setJointMode(E_JOINT_UPDATE_ON_RENDER mode);

		//! Sets the transition time in seconds
		/** Note: This needs to enable joints, and setJointmode set to
		EJUOR_CONTROL. You must call animateJoints(), or the mesh will
		not animate.*/
		 void setTransitionTime(f32 Time) ;

		//! animates the joints in the mesh based on the current frame.
		/** Also takes in to account transitions.*/
		 void animateJoints(bool CalculateAbsolutePositions=true) ;

		//! render mesh ignoring its transformation.
		/** Culling is unaffected.*/
		 void setRenderFromIdentity( bool On );

		 bool getRenderFromIdentity( ) {return RenderFromIdentity;};


		 //! Get a static mesh for the current frame of this animated mesh
		IMesh* getMeshForCurrentFrame();



		// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
		// or the absolutetransformation if it's a normal scenenode
		const SMD3QuaternionTag* getMD3TagTransformation( const core::stringc & tagname) ;
		const SMD3QuaternionTag* getMD3Tag(const core::stringc& tagname);
		const SMD3QuaternionTag* getMD3Tag(u32 index);

;

private:


		void updateAbsolutePosition();

		void buildFrameNr(u32 timeMs);
		void checkJoints();
		void beginTransition();

		core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		IAnimatedMesh* Mesh;

		s32 StartFrame;
		s32 EndFrame;
		f32 FramesPerSecond;
		f32 CurrentFrameNr;

		u32 LastTimeMs;
		u32 TransitionTime; //Transition time in millisecs
		f32 Transiting; //is mesh transiting (plus cache of TransitionTime)
		f32 TransitingBlend; //0-1, calculated on buildFrameNr

		//0-unused, 1-get joints only, 2-set joints only, 3-move and set
		E_JOINT_UPDATE_ON_RENDER JointMode;
		bool JointsUsed;

		bool Looping;
		bool ReadOnlyMaterials;
		bool RenderFromIdentity;


		s32 PassCount;



		core::array<IGameObject* > JointChildSceneNodes;
		core::array<core::matrix4> PretransitingSave;

		// Quake3 Model
		struct SMD3Special : public virtual IReferenceCounted
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
//********************************************************************


class IGeoTerrain : public IComponent
{
public:
  IGeoTerrain(core::vector3df size,	s32 maxLOD = 4, E_TERRAIN_PATCH_SIZE patchSize = ETPS_17, float smoothFactor=0);

    virtual void OnUpdate(u32 timeMs) _IRR_OVERRIDE_;
    virtual void OnRender(IShader* shader) _IRR_OVERRIDE_;
    virtual void free()_IRR_OVERRIDE_;
    virtual void init()_IRR_OVERRIDE_;
    const virtual  char *getType(void) _IRR_OVERRIDE_
    { return "IGEOTERRAIN";};



video::SMaterial& getMaterial(){return RenderBuffer->getMaterial();};

u32 getMaterialCount() const {return 1;};

 //! Initializes the terrain data.  Loads the vertices from the heightMapFile.
/** The file must contain a loadable image of the heightmap. The heightmap
must be square.
\param file The file to read the image from. File is not rewinded.
\param vertexColor Color of all vertices.
\param smoothFactor Number of smoothing passes. */
bool loadHeightMap(const io::path& heightMapFileName,  video::SColor vertexColor=video::SColor(255,255,255,255)) ;



//! Initializes the terrain data.  Loads the vertices from the heightMapFile.
/** The data is interpreted as (signed) integers of the given bit size or
floats (with 32bits, signed). Allowed bitsizes for integers are
8, 16, and 32. The heightmap must be square.
\param file The file to read the RAW data from. File is not rewinded.
\param bitsPerPixel Size of data if integers used, for floats always use 32.
\param signedData Whether we use signed or unsigned ints, ignored for floats.
\param floatVals Whether the data is float or int.
\param width Width (and also Height, as it must be square) of the heightmap. Use 0 for autocalculating from the filesize.
\param vertexColor Color of all vertices.
\param smoothFactor Number of smoothing passes. */
 bool loadHeightMapRAW(const io::path& heightMapFileName, s32 bitsPerPixel=16,
    bool signedData=false, bool floatVals=false, s32 width=0,
    video::SColor vertexColor=video::SColor(255,255,255,255),
    s32 smoothFactor=0) ;



 //! Gets the meshbuffer data based on a specified level of detail.
/** \param mb A reference to an IMeshBuffer object
\param LOD The level of detail you want the indices from. */
 void getMeshBufferForLOD(IMeshBuffer& mb, s32 LOD=0) const ;

//! Gets the indices for a specified patch at a specified Level of Detail.
/** \param indices A reference to an array of u32 indices.
\param patchX Patch x coordinate.
\param patchZ Patch z coordinate.
\param LOD The level of detail to get for that patch. If -1,
then get the CurrentLOD. If the CurrentLOD is set to -1,
meaning it's not shown, then it will retrieve the triangles at
the highest LOD (0).
\return Number of indices put into the buffer. */
 s32 getIndicesForPatch(core::array<u32>& indices,s32 patchX, s32 patchZ, s32 LOD=0) ;

//! Populates an array with the CurrentLOD of each patch.
/** \param LODs A reference to a core::array<s32> to hold the
values
\return Number of elements in the array */
 s32 getCurrentLODOfPatches(core::array<s32>& LODs) const ;

//! Manually sets the LOD of a patch
/** NOTE: Any values set here are overwritten again in the automatic
recalculations when the camera changes.
\param patchX Patch x coordinate.
\param patchZ Patch z coordinate.
\param LOD The level of detail to set the patch to. */
 void setLODOfPatch(s32 patchX, s32 patchZ, s32 LOD=0) ;

//! Get center of terrain.
 const core::vector3df& getTerrainCenter() const ;

//! Get height of a point of the terrain.
 f32 getHeight(f32 x, f32 y) const ;

//! Sets the movement camera threshold.
/** It is used to determine when to recalculate
indices for the scene node. The default value is 10.0f. */
 void setCameraMovementDelta(f32 delta) ;

//! Sets the rotation camera threshold.
/** It is used to determine when to recalculate
indices for the scene node. The default value is 1.0f. */
 void setCameraRotationDelta(f32 delta) ;

//! Sets whether or not the node should dynamically update its associated selector when the geomipmap data changes.
/** \param bVal: Boolean value representing whether or not to update selector dynamically. */
 void setDynamicSelectorUpdate(bool bVal) ;

//! Override the default generation of distance thresholds.
/** For determining the LOD a patch is rendered at. If any LOD
is overridden, then the scene node will no longer apply scaling
factors to these values. If you override these distances, and
then apply a scale to the scene node, it is your responsibility
to update the new distances to work best with your new terrain
size. */
bool overrideLODDistance(s32 LOD, f64 newDistance) ;

//! Scales the base texture, similar to makePlanarTextureMapping.
/** \param scale The scaling amount. Values above 1.0
increase the number of time the texture is drawn on the
terrain. Values below 0 will decrease the number of times the
texture is drawn on the terrain. Using negative values will
flip the texture, as well as still scaling it.
\param scale2 If set to 0 (default value), this will set the
second texture coordinate set to the same values as in the
first set. If this is another value than zero, it will scale
the second texture coordinate set by this value. */
 void scaleTexture(f32 scale = 1.0f, f32 scale2=0.0f) ;



//! Force node to use a fixed LOD level at the borders of the terrain.
/** This can be useful when several TerrainSceneNodes are connected.
\param borderLOD When >= 0 all patches at the 4 borders will use the
given LOD. When < 0 borders are just regular patches (that's default).	*/
void setFixedBorderLOD(irr::s32 borderLOD=0) ;


 //! Gets the last scaling factor applied to the scene node.  This value only represents the
//! last scaling factor presented to the node.  For instance, if you make create the node
//! with a scale factor of ( 1.0f, 1.0f, 1.0f ) then call setScale ( 50.0f, 5.0f, 50.0f ),
//! then make another call to setScale with the values ( 2.0f, 2.0f, 2.0f ), this will return
//! core::vector3df ( 2.0f, 2.0f, 2.0f ), although the total scaling of the scene node is
//! core::vector3df ( 100.0f, 10.0f, 100.0f ).
//! \return Returns the last scaling factor passed to the scene node.
 const core::vector3df& getScale() const
{
    return TerrainData.Scale;
}

//! Scales the scene nodes vertices by the vector specified.
//! \param scale: Scaling factor to apply to the node.
 void setScale(const core::vector3df& scale) ;

//! Gets the last rotation factor applied to the scene node.
//! \return Returns the last rotation factor applied to the scene node.
 const core::vector3df& getRotation() const
{
    return TerrainData.Rotation;
}

//! Rotates the node. This only modifies the relative rotation of the node.
//! \param rotation: New rotation of the node in degrees.
 void setRotation(const core::vector3df& rotation) ;

//! Sets the pivot point for rotation of this node.
//! NOTE: The default for the RotationPivot will be the center of the individual tile.
 void setRotationPivot( const core::vector3df& pivot );

//! Gets the last positioning vector applied to the scene node.
//! \return Returns the last position vector applied to the scene node.
 const core::vector3df& getPosition() const
{
    return TerrainData.Position;
}

//! Moves the scene nodes vertices by the vector specified.
//! \param newpos: Vector specifying how much to move each vertex of the scene node.
 void setPosition(const core::vector3df& newpos) ;


//! smooth the terrain
void smoothTerrain( s32 smoothFactor);


private:
        struct SPatch
{
    SPatch()
    : Top(0), Bottom(0), Right(0), Left(0), CurrentLOD(-1)
    {
    }

    SPatch* Top;
    SPatch* Bottom;
    SPatch* Right;
    SPatch* Left;
    s32 CurrentLOD;
    core::aabbox3df BoundingBox;
    core::vector3df Center;
};

struct STerrainData
{
    STerrainData(s32 patchSize, s32 maxLOD, const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
    : Patches(0), Size(0), Position(position), Rotation(rotation),
        Scale(scale), PatchSize(patchSize), CalcPatchSize(patchSize-1),
        PatchCount(0), MaxLOD(maxLOD)
    {
    }

    SPatch*		Patches;
    s32		Size;
    core::vector3df	Position;
    core::vector3df	Rotation;
    core::vector3df RotationPivot;
    core::vector3df	Scale;
    core::vector3df Center;
    s32		PatchSize;
    s32		CalcPatchSize;
    s32		PatchCount;
    s32		MaxLOD;
    core::aabbox3df	BoundingBox;
    core::array<f64> LODDistanceThreshold;
};

		//! calculate or recalculate the distance thresholds
		void calculateDistanceThresholds(bool scalechanged = false);
				//! create patches, stuff that needs to only be done once for patches goes here.
		void createPatches();

		//! calculate the internal STerrainData structure
		void calculatePatchData();

        bool preRenderCalculationsIfNeeded();
		bool preRenderLODCalculations();
		void preRenderIndicesCalculations();

		//! get indices when generating index data for patches at varying levels of detail.
		u32 getIndex(const s32 PatchX, const s32 PatchZ, const s32 PatchIndex, u32 vX, u32 vZ) const;





        const core::aabbox3d<f32>& getBoundingBox() const;
        const core::aabbox3d<f32>& getBoundingBox(s32 patchX, s32 patchZ) const;


		//! sets the CurrentLOD of all patches to the specified LOD
		void setCurrentLODOfPatches(s32 i);

		//! sets the CurrentLOD of TerrainData patches to the LODs specified in the array
		void setCurrentLODOfPatches(const core::array<s32>& lodarray);

		//! Apply transformation changes( scale, position, rotation )
		void applyTransformation();

		void getMeshBufferForLOD(SMeshBufferLightMap& mb, s32 LOD ) const;
	//	void getMeshBufferForLOD(SMeshBuffer& mb, s32 LOD ) const;

				//! calculate smooth normals
		void calculateNormals(SMeshBufferLightMap* mb);
		//void calculateNormals(SMeshBuffer* mb);


        //! smooth the terrain
        void smoothTerrain(SMeshBufferLightMap* mb, s32 smoothFactor);
        //void smoothTerrain(SMeshBuffer* mb, s32 smoothFactor);

private:

	    STerrainData TerrainData;
		SMesh* Mesh;

		SMeshBufferLightMap *RenderBuffer;
	//	SMeshBuffer *RenderBuffer;

		u32 VerticesToRender;
		u32 IndicesToRender;

		bool DynamicSelectorUpdate;
		bool OverrideDistanceThreshold;
		bool UseDefaultRotationPivot;
		bool ForceRecalculation;
		s32 FixedBorderLOD;



		core::vector3df	OldCameraPosition;
		core::vector3df	OldCameraRotation;
		core::vector3df	OldCameraUp;
		f32             OldCameraFOV;
		f32 CameraMovementDelta;
		f32 CameraRotationDelta;
		f32 CameraFOVDelta;

		// needed for (de)serialization
		f32 TCoordScale1;
		f32 TCoordScale2;
		s32 SmoothFactor;

		io::IFileSystem* FileSystem;

};

//********************************************************************
class IGameObject : public virtual IReferenceCounted
{
public:

    IGameObject();
    IGameObject(s32 id);
    IGameObject(s32 id,const c8* name);
    IGameObject(IGameObject* parent, s32 id);


    ~IGameObject();


    virtual void OnUpdate(u32 timeMs);
    virtual void OnRender(IShader* shader);



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
		IGameObject* getChildById(s32 id);
        IGameObject* getChildByName(const core::stringc name);



    		//! Changes the parent of the scene node.
		/** \param newParent The new parent to be used. */
        void setParent(IGameObject* newParent);
        IGameObject* getParent(){ return Parent;};

		//! Returns a const reference to the list of all children.
		/** \return The list of all children of this node. */

		const core::list<IGameObject*>& getChildren() const;

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

		//! Adds a child to this scene node.
		/** If the scene node already has a parent it is first removed
		from the other parent.
		\param child A pointer to the new child. */
		 void addChild(IGameObject* child);

		//! Removes a child from this scene node.
		/** If found in the children list, the child pointer is also
		dropped and might be deleted if no other grab exists.
		\param child A pointer to the child which shall be removed.
		\return True if the child was removed, and false if not,
		e.g. because it couldn't be found in the children list. */
		bool removeChild(IGameObject* child);

		//! Removes all children of this scene node
		/** The scene nodes found in the children list are also dropped
		and might be deleted if no other grab exists on them.
		*/
		void removeAll();

		//! Removes this scene node from the scene
        /** If no other grab exists for this node, it will be deleted.
        */
        void remove();



   template <class T>
  inline T addComponent(std::shared_ptr<T> component)
  {
    component->setParent(this);
    componentsByTypeid[typeid(T)].push_back(component);
    components.push_back(component);
    component->init();
    SortCmp=true;
    return  component.get();
  }

  template <class T, class... _Types>
  inline void addComponent(_Types &&... _Args)
  {
    auto component = std::make_shared<T>(_Args...);

    component->setParent(this);
    componentsByTypeid[typeid(T)].push_back(std::dynamic_pointer_cast<IComponent>(component));
    components.push_back(component);
    component->init();
    SortCmp=true;

  }
 std::vector<std::shared_ptr<IComponent>> getComponents(void);

   template <class T>
  inline std::vector<std::shared_ptr<T>> getComponentsByType(void)
  {
    auto i = componentsByTypeid.find(typeid(T));
    if (i == componentsByTypeid.end())
    {
      return std::vector<std::shared_ptr<T>>();
    }
    else
    {
      auto vec = i->second;

      std::vector<std::shared_ptr<T>> target(vec.size());
      std::transform(vec.begin(), vec.end(), target.begin(), [](std::shared_ptr<IComponent> t) { return std::dynamic_pointer_cast<T>(t); });
      return target;
    }
  }

  template <class T>
  inline std::shared_ptr<T> getComponent(void)
  {
    auto i = componentsByTypeid.find(typeid(T));
    if (i == componentsByTypeid.end())
    {
      return nullptr;
    }
    else
    {
      auto vec = i->second;
      if (vec.size() > 0)
      {
        return std::dynamic_pointer_cast<T>(vec[0]);
      }
      else
      {
        return nullptr;
      }
    }
  }

  template <class T>
  inline const std::shared_ptr<T> getConstComponent(void)
  {
    auto i = componentsByTypeid.find(typeid(T));
    if (i == componentsByTypeid.end())
    {
      return nullptr;
    }
    else
    {
      auto vec = i->second;
      if (vec.size() > 0)
      {
        return std::dynamic_pointer_cast<T>(vec[0]);
      }
      else
      {
        return nullptr;
      }
    }
  }

public:
    scene::ITransform *Transform;


private:

    std::vector<std::shared_ptr<IComponent>> components;
    std::map<std::type_index, std::vector<std::shared_ptr<IComponent>>> componentsByTypeid;
    bool SortCmp;
    void sort();

protected:
		//! Name of the scene node.
		core::stringc Name;
		//! Pointer to the parent
		IGameObject* Parent;
		//! List of all children of this node
		core::list<IGameObject*> Children;
		//! ID of the node.
		s32 ID;
        //! Is the node visible?
		bool IsVisible;


};


} // end namespace scene
} // end namespace irr

#endif
