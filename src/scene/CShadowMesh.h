#ifndef __C_SHADOWMESH_H_INCLUDED
#define __C_SHADOWMESH_H_INCLUDED

#include "IShadowMesh.h"
#include "IReferenceCounted.h"
#include "SMaterial.h"
#include "aabbox3d.h"
#include "S3DVertex.h"
#include "IEngineEnums.h"
#include "IMesh.h"
#include "CMeshBuffer.h"
#include "IMeshBuffer.h"

#include "os.h"
#include "IrrCompileConfig.h"

#include "dimension2d.h"

#include "IReferenceCounted.h"

#include "ITexture.h"

#include "line2d.h"
#include "line3d.h"
#include "matrix4.h"
#include "plane3d.h"
#include "position2d.h"
#include "quaternion.h"
#include "rect.h"
#include "S3DVertex.h"
#include "SColor.h"
#include "SMaterial.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "aabbox3d.h"
#include "coreutil.h"

namespace irr
{
namespace video
{

class CShadowMesh : public IShadowMesh
{
public:

    CShadowMesh(const scene::IMesh* shadowMesh, bool zfailmethod=true, f32 infinity=10000.0f);
    ~CShadowMesh();

    /** To optimize shadow rendering, use a simpler mesh for shadows.
		*/
		virtual void setShadowMesh(const scene::IMesh* mesh) _IRR_OVERRIDE_;

		//! Updates the shadow volumes for current light positions.
		virtual void updateShadowVolumes(core::matrix4 matrix) _IRR_OVERRIDE_;

		//! Set optimization used to create shadow volumes
		/** Default is ESV_SILHOUETTE_BY_POS. If the shadow
		looks bad then give ESV_NONE a try (which will be slower).
		Alternatively you can try to fix the model, it's often
		because it's not closed (aka if you'd put water in it then
		that would leak out). */
		virtual void setOptimization(ESHADOWVOLUME_OPTIMIZATION optimization)_IRR_OVERRIDE_ ;

		//! Get currently active optimization used to create shadow volumes
		virtual ESHADOWVOLUME_OPTIMIZATION getOptimization() const _IRR_OVERRIDE_;

		virtual void render(core::matrix4 matrix)_IRR_OVERRIDE_;

    private:
    	typedef core::array<core::vector3df> SShadowVolume;

		void createShadowVolume(const core::vector3df& pos, bool isDirectional);
		u32 createEdgesAndCaps(const core::vector3df& light, bool isDirectional, SShadowVolume* svp, core::aabbox3d<f32>* bb);
		void renderVolume();

		//! Generates adjacency information based on mesh indices.
		void calculateAdjacency();

		core::aabbox3d<f32> Box;

		// a shadow volume for every light
		core::array<SShadowVolume> ShadowVolumes;

		// a back cap bounding box for every light
		core::array<core::aabbox3d<f32> > ShadowBBox;

		core::array<core::vector3df> Vertices;
		core::array<u16> Indices;
		core::array<u16> Adjacency;
		core::array<u16> Edges;
		// tells if face is front facing
		core::array<bool> FaceData;
		bool AdjacencyDirtyFlag;

		const scene::IMesh* ShadowMesh;

		u32 IndexCount;
		u32 VertexCount;
		u32 ShadowVolumesUsed;

        unsigned int VertexBufferID;

		f32 Infinity;
		bool UseZFailMethod;
		video::ESHADOWVOLUME_OPTIMIZATION Optimization;
		int midStencilVal;
		GLuint stencil_vbo;
};

} // end namespace video
} // end namespace irr


#endif
