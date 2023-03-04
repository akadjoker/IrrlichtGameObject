#ifndef IVERTEXBUFFER_H
#define IVERTEXBUFFER_H

#include "IReferenceCounted.h"

namespace irr
{
namespace video
{


enum VertexElementType
{
    VET_FLOAT1,
    VET_FLOAT2,
    VET_FLOAT3,
    VET_FLOAT4,
    VET_SHORT1,
    VET_SHORT2,
    VET_SHORT3,
    VET_SHORT4,
    VET_BYTE3,
    VET_BYTE4,
};

enum VertexElement {
    VF_POSITION      = 1<<0, // vertex position 3 floats x,y,z
    VF_COLOR         = 1<<1, // vertex color 4 bytes r,g,b,a
    VF_FLOATCOLOR    = 1<<2, // vertex color 4 floats r,g,b,a
    VF_TEXCOORD     = 1<<3, // texcoord 1 2 floats u,v
    VF_NORMAL        = 1<<4, // normal 3 floats x,y,z
    VF_TANGENT       = 1<<5, // normal 3 floats x,y,z
    VF_BINORMAL      = 1<<6, // normal 3 floats x,y,z
    VF_TEXCOORD2   = 1<<7, // texcoord 1 2 floats u,v
    VF_TEXCOORD3   = 1<<8, // texcoord 1 2 floats u,v
    VF_TEXCOORD4   = 1<<9, // texcoord 1 2 floats u,v
    VF_TEXCOORD5   = 1<<10, // texcoord 1 2 floats u,v
    VF_TEXCOORD6   = 1<<11, // texcoord 1 2 floats u,v
    VF_TEXCOORD7   = 1<<12, // texcoord 1 2 floats u,v
    VF_TEXCOORD8   = 1<<13, // texcoord 1 2 floats u,v
};


struct 		VertexDeclaration
{
 VertexElement   element;
 VertexElementType type;
};





class IVertexBuffer : public virtual IReferenceCounted
{
public:
  IVertexBuffer(){};
  	//! destructor
	virtual ~IVertexBuffer(){	}

virtual void createVBO(void* data , int vtx_cout, void* indices, int indices_count, bool staticBuffer)=0;
virtual void createVBO(void* data , int vtx_cout, bool staticBuffer)=0;
virtual void UpdateVertex( void *data, uint size )=0;
virtual void UpdateIndices(void * indices, uint size )=0;
virtual void Unload()=0;
virtual void Render(int mode, int count)=0;
virtual void Render(int mode, int count,bool useEBO)=0;

 bool isLoad(){return VBO>0;};
protected:
    unsigned int  VAO, EBO,VBO;
};

}
}

#endif // IVERTEXBUFFER_H
