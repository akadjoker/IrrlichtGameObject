#ifndef __I_SHADOWMESH_H_INCLUDED
#define __I_SHADOWMESH_H_INCLUDED

#include <functional>

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

	enum ESHADOWVOLUME_OPTIMIZATION
	{
		//! Create volumes around every triangle
		ESV_NONE,

		//! Create volumes only around the silhouette of the mesh
		/** This can reduce the number of volumes drastically,
		but will have an upfront-cost where it calculates adjacency of
		triangles. Also it will not work with all models. Basically
		if you see strange black shadow lines then you have a model
		for which it won't work.
		We get that information about adjacency by comparing the positions of
		all edges in the mesh (even if they are in different meshbuffers). */
		ESV_SILHOUETTE_BY_POS
	};

class IShadowMesh :public virtual IReferenceCounted
{
public:

        IShadowMesh(){setDebugName("IShadowMesh");};
       virtual ~IShadowMesh(){};
      	//! Sets the mesh from which the shadow volume should be generated.
		/** To optimize shadow rendering, use a simpler mesh for shadows.
		*/
		virtual void setShadowMesh(const scene::IMesh* mesh) =0;

		//! Updates the shadow volumes for current light positions.
		virtual void updateShadowVolumes(core::matrix4 matrix) =0;

		//! Set optimization used to create shadow volumes
		/** Default is ESV_SILHOUETTE_BY_POS. If the shadow
		looks bad then give ESV_NONE a try (which will be slower).
		Alternatively you can try to fix the model, it's often
		because it's not closed (aka if you'd put water in it then
		that would leak out). */
		virtual void setOptimization(ESHADOWVOLUME_OPTIMIZATION optimization)=0 ;

		//! Get currently active optimization used to create shadow volumes
		virtual ESHADOWVOLUME_OPTIMIZATION getOptimization() const =0;

		virtual void render(core::matrix4 matrix)=0;


};

} // end namespace video
} // end namespace irr

#endif
