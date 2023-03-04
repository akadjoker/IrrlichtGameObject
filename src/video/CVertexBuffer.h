#ifndef CVERTEXBUFFER_H
#define CVERTEXBUFFER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <vector>
#include <list>
#include "IVertexBuffer.h"
#include "CVertexBuffer.h"
#include "os.h"

namespace irr
{
namespace video
{





class CVertexBuffer : public  IVertexBuffer
{
public:
 CVertexBuffer( std::vector<video::VertexDeclaration> fvf);
virtual void createVBO(void* data , int vtx_cout, void* indices, int indices_count, bool staticBuffer)  _IRR_OVERRIDE_;
virtual void createVBO(void* data , int vtx_cout, bool staticBuffer)  _IRR_OVERRIDE_;
virtual void UpdateVertex( void *data, uint size )  _IRR_OVERRIDE_;
virtual void UpdateIndices(void * indices, uint size ) _IRR_OVERRIDE_;
virtual void Unload()  _IRR_OVERRIDE_;
virtual void Render(int mode, int count) _IRR_OVERRIDE_;
virtual void Render(int mode, int count,bool useEBO)_IRR_OVERRIDE_;

private:
    uint m_iVertexCount;
    uint m_iIndicesCount;
    uint m_iCountVertexDeclaration;
    uint m_iVertexOffSetSize;// size of Vertex
    std::vector<VertexDeclaration>  m_VertexDeclaration;
    bool IsEBO;

};

}
}

#endif // IVERTEXBUFFER_H
