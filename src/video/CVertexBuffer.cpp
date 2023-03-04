#include "CVertexBuffer.h"

#include "irrString.h"
#include "os.h"

namespace irr
{
namespace video
{


size_t getTypeFormat(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_FLOAT1:
			return GL_FLOAT;
		case VET_FLOAT2:
			return GL_FLOAT;
		case VET_FLOAT3:
			return GL_FLOAT;
		case VET_FLOAT4:
			return GL_FLOAT;
		case VET_SHORT1:
			return GL_SHORT;
		case VET_SHORT2:
			return GL_SHORT;
		case VET_SHORT3:
			return GL_SHORT;
		case VET_SHORT4:
			return GL_SHORT;
        case VET_BYTE3:
            return GL_UNSIGNED_BYTE;
        case VET_BYTE4:
            return GL_UNSIGNED_BYTE;
		}
		return 0;
	}
	size_t getTypeSize(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float)*2;
		case VET_FLOAT3:
			return sizeof(float)*3;
		case VET_FLOAT4:
			return sizeof(float)*4;
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short)*2;
		case VET_SHORT3:
			return sizeof(short)*3;
		case VET_SHORT4:
			return sizeof(short)*4;
        case VET_BYTE3:
            return sizeof(unsigned char)*3+1;
        case VET_BYTE4:
            return sizeof(unsigned char)*4+1;
		}
		return 0;
	}


	unsigned short getTypeCount(VertexElementType etype)
	{
		switch (etype)
		{
		case VET_FLOAT1:
			return 1;
		case VET_FLOAT2:
			return 2;
		case VET_FLOAT3:
			return 3;
		case VET_FLOAT4:
			return 4;
		case VET_SHORT1:
			return 1;
		case VET_SHORT2:
			return 2;
		case VET_SHORT3:
			return 3;
		case VET_SHORT4:
			return 4;
        case VET_BYTE3:
            return 3;
        case VET_BYTE4:
            return 4;
		}
      return 0;
	}



CVertexBuffer::CVertexBuffer( std::vector<VertexDeclaration> fvf )
{

m_VertexDeclaration = fvf;
m_iCountVertexDeclaration=fvf.size();

VAO, EBO,VBO=-1;

m_iVertexOffSetSize=0;
for (int i=0; i<m_iCountVertexDeclaration;i++)
{

m_iVertexOffSetSize += getTypeSize(fvf[i].type) ;

//SDL_Log("layer %d %d  %d",i,getTypeSize(fvf[i].type) ,m_iVertexOffSetSize);
}

//m_iVertexOffSetSize = 3 *sizeof(float) + 3 * sizeof(unsigned char);
//SDL_Log(" %d  ",m_iVertexOffSetSize);

//m_iVertexOffSetSize = sizeof(Vertex);

//SDL_Log(" %d  ",m_iVertexOffSetSize);





}

void CVertexBuffer::createVBO(void* data , int vtx_cout, void* indices, int indices_count, bool staticBuffer)
{
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

m_iVertexCount =vtx_cout;
m_iIndicesCount=indices_count;

IsEBO=false;

if (indices)
{
    if (indices_count>0)
    {
    IsEBO=true;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count*sizeof(unsigned int), indices, GL_STATIC_DRAW);
    }
}



glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, vtx_cout*m_iVertexOffSetSize, data, (staticBuffer==true)?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
int offSet = 0;



for (int i=0; i<m_iCountVertexDeclaration;i++)
{

int attribute = i;
glEnableVertexAttribArray(attribute);
void* pointer = reinterpret_cast<void*>(offSet);
if( m_VertexDeclaration[i].element & VF_POSITION )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize, pointer);
}

if( m_VertexDeclaration[i].element & VF_COLOR )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize,  pointer);
}

if( m_VertexDeclaration[i].element & VF_FLOATCOLOR )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), true,m_iVertexOffSetSize,  pointer);
}

if( m_VertexDeclaration[i].element & VF_NORMAL )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize,  pointer);
}

if( m_VertexDeclaration[i].element & VF_TANGENT )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize,  pointer);
}

if( m_VertexDeclaration[i].element & VF_TEXCOORD )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize, pointer);
}

if( m_VertexDeclaration[i].element & VF_TEXCOORD2 )
{

glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize,  pointer);
}

if( m_VertexDeclaration[i].element & VF_TEXCOORD3 )
{
glVertexAttribPointer(attribute, getTypeCount(m_VertexDeclaration[i].type), getTypeFormat(m_VertexDeclaration[i].type), false,m_iVertexOffSetSize, pointer);
}


offSet += getTypeSize(m_VertexDeclaration[i].type);
}


}

void CVertexBuffer::createVBO(void* data , int vtx_cout, bool staticBuffer)
{
createVBO(data,vtx_cout,nullptr,0,staticBuffer);
}

void CVertexBuffer::UpdateVertex( void *data, uint size)
{
if (!VAO) return ;
glBindVertexArray(VAO);
if (!VBO) return ;
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferSubData(GL_ARRAY_BUFFER,0, size*m_iVertexOffSetSize, data);
glBindVertexArray(0);
}

void CVertexBuffer::UpdateIndices(void* indices, uint size )
{
if (!VAO) return ;
glBindVertexArray(VAO);

if (!IsEBO || !EBO) return;
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, size*sizeof(unsigned int), indices);
glBindVertexArray(0);
}


void CVertexBuffer::Unload()
{
    if (VAO)    glDeleteVertexArrays(1, &VAO);
    if (VBO)    glDeleteBuffers(1, &VBO);
    if (IsEBO)  glDeleteBuffers(1, &EBO);
}

void CVertexBuffer::Render(int mode, int count,bool useEBO)
{

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);


if (useEBO)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

} else
{
glDrawArrays(mode,0,count);
}

glBindVertexArray(0);


}


void CVertexBuffer::Render(int mode, int count)
{

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

if (IsEBO)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} else
{
glDrawArrays(mode,0,count);
}

glBindVertexArray(0);


}

}
}
