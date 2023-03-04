// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SKIN_MESH_BUFFER_H_INCLUDED__
#define __I_SKIN_MESH_BUFFER_H_INCLUDED__

#include "IMeshBuffer.h"
#include "S3DVertex.h"

inline irr::u8* bufferoffset(const long offset)
{
    return ((irr::u8*)0 + offset);
}

namespace irr
{
namespace scene
{

using namespace video;

//! A mesh buffer able to choose between S3DVertex2TCoords, S3DVertex and S3DVertexTangents at runtime
struct SSkinMeshBuffer : public IMeshBuffer
{
	//! Default constructor
	SSkinMeshBuffer(video::E_VERTEX_TYPE vt=video::EVT_STANDARD) :
		ChangedID_Vertex(0), ChangedID_Index(0), VertexType(vt),ChangedID_array(0),usage_vertex(GL_DYNAMIC_DRAW),
		usage_index(GL_STATIC_DRAW),isBufferCreated(false),
		PrimitiveType(EPT_TRIANGLES),indices_type(GL_UNSIGNED_SHORT),bufferUpdate(EBT_NONE),
		MappingHint_Vertex(EHM_NEVER), MappingHint_Index(EHM_NEVER),
		BoundingBoxNeedsRecalculated(true)
	{


		setDebugName("SSkinMeshBuffer");
		Material=IdentityMaterial;

	}
	virtual ~SSkinMeshBuffer()
    {

      SDL_Log("[SSkinMeshBuffer]  Delete");
   if (ChangedID_array!=-1)
      {

        SDL_Log("[SKINMESHBUFFER]  Free VAO[%d]",ChangedID_array);
        glDeleteVertexArrays(1,&ChangedID_array);


      }
      if (ChangedID_Index!=-1)
      {
         SDL_Log("[SKINMESHBUFFER]  Free EBO[%d]",ChangedID_Index);
         glDeleteBuffers(1,&ChangedID_Index);

      }
      if (ChangedID_Vertex!=-1)
      {
         SDL_Log("[SKINMESHBUFFER]  Free VBO[%d]",ChangedID_Vertex);
        glDeleteBuffers(1,&ChangedID_Vertex);

      }

    Indices.clear();
    Vertices_2TCoords.clear();
    Vertices_Tangents.clear();
    Vertices_Standard.clear();

    }


	//! Get Material of this buffer.
	virtual const video::SMaterial& getMaterial() const _IRR_OVERRIDE_
	{
		return Material;
	}

	//! Get Material of this buffer.
	virtual video::SMaterial& getMaterial() _IRR_OVERRIDE_
	{
		return Material;
	}

			//virtual void setType(video::E_INDEX_TYPE IndexType);
		virtual void setType(video::E_INDEX_TYPE IndexType) _IRR_OVERRIDE_
		{

			indexType=IndexType;
			if (IndexType==EIT_16BIT)
			  indices_type=GL_UNSIGNED_SHORT;
            else
              indices_type=GL_UNSIGNED_INT;


		}


	//! Get standard vertex at given index
	virtual video::S3DVertex *getVertex(u32 index)
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return (video::S3DVertex*)&Vertices_2TCoords[index];
			case video::EVT_TANGENTS:
				return (video::S3DVertex*)&Vertices_Tangents[index];
			default:
				return &Vertices_Standard[index];
		}
	}

	//! Get pointer to vertex array
	virtual const void* getVertices() const _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords.const_pointer();
			case video::EVT_TANGENTS:
				return Vertices_Tangents.const_pointer();
			default:
				return Vertices_Standard.const_pointer();
		}
	}

	//! Get pointer to vertex array
	virtual void* getVertices() _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords.pointer();
			case video::EVT_TANGENTS:
				return Vertices_Tangents.pointer();
			default:
				return Vertices_Standard.pointer();
		}
	}

	//! Get vertex count
	virtual u32 getVertexCount() const _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords.size();
			case video::EVT_TANGENTS:
				return Vertices_Tangents.size();
			default:
				return Vertices_Standard.size();
		}
	}

	//! Get type of index data which is stored in this meshbuffer.
	/** \return Index type of this buffer. */
	virtual video::E_INDEX_TYPE getIndexType() const _IRR_OVERRIDE_
	{
		return video::EIT_16BIT;
	}

	//! Get pointer to index array
	virtual const u16* getIndices() const _IRR_OVERRIDE_
	{
		return Indices.const_pointer();
	}

	//! Get pointer to index array
	virtual u16* getIndices() _IRR_OVERRIDE_
	{
		return Indices.pointer();
	}

	//! Get index count
	virtual u32 getIndexCount() const _IRR_OVERRIDE_
	{
		return Indices.size();
	}

	//! Get bounding box
	virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_
	{
		return BoundingBox;
	}

	//! Set bounding box
	virtual void setBoundingBox( const core::aabbox3df& box) _IRR_OVERRIDE_
	{
		BoundingBox = box;
	}

	//! Recalculate bounding box
	virtual void recalculateBoundingBox() _IRR_OVERRIDE_
	{
		if(!BoundingBoxNeedsRecalculated)
			return;

		BoundingBoxNeedsRecalculated = false;

		switch (VertexType)
		{
			case video::EVT_STANDARD:
			{
				if (Vertices_Standard.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_Standard[0].Pos);
					for (u32 i=1; i<Vertices_Standard.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_Standard[i].Pos);
				}
				break;
			}
			case video::EVT_2TCOORDS:
			{
				if (Vertices_2TCoords.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_2TCoords[0].Pos);
					for (u32 i=1; i<Vertices_2TCoords.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_2TCoords[i].Pos);
				}
				break;
			}
			case video::EVT_TANGENTS:
			{
				if (Vertices_Tangents.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_Tangents[0].Pos);
					for (u32 i=1; i<Vertices_Tangents.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_Tangents[i].Pos);
				}
				break;
			}
		}
	}

	//! Get vertex type
	virtual video::E_VERTEX_TYPE getVertexType() const _IRR_OVERRIDE_
	{
		return VertexType;
	}

	//! Convert to 2tcoords vertex type
	void convertTo2TCoords()
	{
		if (VertexType==video::EVT_STANDARD)
		{
			for(u32 n=0;n<Vertices_Standard.size();++n)
			{
				video::S3DVertex2TCoords Vertex;
				Vertex.Color=Vertices_Standard[n].Color;
				Vertex.Pos=Vertices_Standard[n].Pos;
				Vertex.Normal=Vertices_Standard[n].Normal;
				Vertex.TCoords=Vertices_Standard[n].TCoords;
				Vertices_2TCoords.push_back(Vertex);
			}
			Vertices_Standard.clear();
			VertexType=video::EVT_2TCOORDS;
		}
		 setDirty(EBT_VERTEX);
	}

	//! Convert to tangents vertex type
	void convertToTangents()
	{
		if (VertexType==video::EVT_STANDARD)
		{
			for(u32 n=0;n<Vertices_Standard.size();++n)
			{
				video::S3DVertexTangents Vertex;
				Vertex.Color=Vertices_Standard[n].Color;
				Vertex.Pos=Vertices_Standard[n].Pos;
				Vertex.Normal=Vertices_Standard[n].Normal;
				Vertex.TCoords=Vertices_Standard[n].TCoords;
				Vertices_Tangents.push_back(Vertex);
			}
			Vertices_Standard.clear();
			VertexType=video::EVT_TANGENTS;
		}
		else if (VertexType==video::EVT_2TCOORDS)
		{
			for(u32 n=0;n<Vertices_2TCoords.size();++n)
			{
				video::S3DVertexTangents Vertex;
				Vertex.Color=Vertices_2TCoords[n].Color;
				Vertex.Pos=Vertices_2TCoords[n].Pos;
				Vertex.Normal=Vertices_2TCoords[n].Normal;
				Vertex.TCoords=Vertices_2TCoords[n].TCoords;
				Vertices_Tangents.push_back(Vertex);
			}
			Vertices_2TCoords.clear();
			VertexType=video::EVT_TANGENTS;
		}
      setDirty(EBT_VERTEX);
	}

	//! returns position of vertex i
	virtual const core::vector3df& getPosition(u32 i) const _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].Pos;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].Pos;
			default:
				return Vertices_Standard[i].Pos;
		}
	}

	//! returns position of vertex i
	virtual core::vector3df& getPosition(u32 i) _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].Pos;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].Pos;
			default:
				return Vertices_Standard[i].Pos;
		}
	}

	//! returns normal of vertex i
	virtual const core::vector3df& getNormal(u32 i) const _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].Normal;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].Normal;
			default:
				return Vertices_Standard[i].Normal;
		}
	}

	//! returns normal of vertex i
	virtual core::vector3df& getNormal(u32 i) _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].Normal;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].Normal;
			default:
				return Vertices_Standard[i].Normal;
		}
	}

	//! returns texture coords of vertex i
	virtual const core::vector2df& getTCoords(u32 i) const _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].TCoords;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].TCoords;
			default:
				return Vertices_Standard[i].TCoords;
		}
	}

	//! returns texture coords of vertex i
	virtual core::vector2df& getTCoords(u32 i) _IRR_OVERRIDE_
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:
				return Vertices_2TCoords[i].TCoords;
			case video::EVT_TANGENTS:
				return Vertices_Tangents[i].TCoords;
			default:
				return Vertices_Standard[i].TCoords;
		}
	}

	//! append the vertices and indices to the current buffer
	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) _IRR_OVERRIDE_ {}

	//! append the meshbuffer to the current buffer
	virtual void append(const IMeshBuffer* const other) _IRR_OVERRIDE_ {}


		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const _IRR_OVERRIDE_
		{
			return MappingHint_Vertex;
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const _IRR_OVERRIDE_
		{
			return MappingHint_Index;
		}

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint, E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX ) _IRR_OVERRIDE_
		{
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_VERTEX)
			{
				MappingHint_Vertex=NewMappingHint;


				switch (MappingHint_Vertex)
				{

                    //! Rarely changed, usually stored completely on the hardware
                   case EHM_STATIC:
                    {
                    usage_vertex=GL_STATIC_DRAW;
                    break;
                    }

                    //! Sometimes changed, driver optimized placement
                   case EHM_DYNAMIC:
                    {
                    usage_vertex=GL_DYNAMIC_DRAW;
                    break;
                    }

                    //! Always changed, cache optimizing on the GPU
                  case  EHM_STREAM:
                    {
                    usage_vertex=GL_STREAM_DRAW;
                    break;
                    }
				}



            }
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_INDEX)
			{
				MappingHint_Index=NewMappingHint;
                switch (MappingHint_Vertex)
				{

                    //! Rarely changed, usually stored completely on the hardware
                  case  EHM_STATIC:
                    {
                    usage_index=GL_STATIC_DRAW;
                    break;
                    }

                    //! Sometimes changed, driver optimized placement
                    case EHM_DYNAMIC:
                    {
                    usage_index=GL_DYNAMIC_DRAW;
                    break;
                    }

                    //! Always changed, cache optimizing on the GPU
                    case EHM_STREAM:
                    {
                    usage_index=GL_STREAM_DRAW;
                    break;
                    }
				}
            }

		}

		//! Describe what kind of primitive geometry is used by the meshbuffer
		virtual void setPrimitiveType(E_PRIMITIVE_TYPE type) _IRR_OVERRIDE_
		{
			PrimitiveType = type;
		}

		//! Get the kind of primitive geometry which is used by the meshbuffer
		virtual E_PRIMITIVE_TYPE getPrimitiveType() const _IRR_OVERRIDE_
		{
			return PrimitiveType;
		}

	//! flags the mesh as changed, reloads hardware buffers
	virtual void setDirty(E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX) _IRR_OVERRIDE_
	{
    	      bufferUpdate=Buffer;

           if (!isBufferCreated)
           {
             createVertexBuffer();
             return;
           }
	}

		//! Get the currently used ID for identification of changes.

		virtual u32 getChangedID_Vertex() const _IRR_OVERRIDE_ {return ChangedID_Vertex;}

		//! Get the currently used ID for identification of changes.

		virtual u32 getChangedID_Index() const _IRR_OVERRIDE_ {return ChangedID_Index;}

	//! Call this after changing the positions of any vertex.
	void boundingBoxNeedsRecalculated(void) { BoundingBoxNeedsRecalculated = true; }

        //! create opengl vertex buffer/index buffer/array buffer
    virtual void createVertexBuffer() override
    {
        if(isBufferCreated)
        {
             SDL_Log("[MESHBUFFER]   VAO[%d] exists, exit",ChangedID_array);
             SDL_Log("[MESHBUFFER]   VBO[%d] exists, exit",ChangedID_Vertex);
             SDL_Log("[MESHBUFFER]   EBO[%d] exists, exit",ChangedID_Index);
             return;
        }



       glGenVertexArrays(1, &ChangedID_array);
       glBindVertexArray(ChangedID_array);
      // SDL_LogWarn(0,"[MESHBUFFER] Create vertex buffer  %d",ChangedID_array);






        glGenBuffers(1, &ChangedID_Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, ChangedID_Vertex);
        glBufferData(GL_ARRAY_BUFFER, getVertexCount()*getVertexPitchFromType(getVertexType()), getVertices(), usage_vertex);



        glEnableVertexAttribArray(video::EVA_POSITION);
        glEnableVertexAttribArray(video::EVA_COLOR);
        glEnableVertexAttribArray(video::EVA_NORMAL);
        glEnableVertexAttribArray(video::EVA_TCOORD0);



      switch (getVertexType())
       {
            case video::EVT_STANDARD:
            {


            if (Vertices_Standard.pointer())
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Pos)));
				glVertexAttribPointer(EVA_NORMAL,   3, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Normal)));
				glVertexAttribPointer(EVA_COLOR,    4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Color)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,TCoords)));
			}
			else
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), 0);
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex), bufferoffset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), bufferoffset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex), bufferoffset(28));
			}



            break;
            }
            case video::EVT_2TCOORDS:
            {

                glEnableVertexAttribArray(video::EVA_TCOORD1);

           if (Vertices_2TCoords.pointer())
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Pos)));
				glVertexAttribPointer(EVA_NORMAL,   3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Normal)));
				glVertexAttribPointer(EVA_COLOR,    4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Color)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,TCoords)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,TCoords2)));
			}
			else
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), 0);
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), bufferoffset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), bufferoffset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), bufferoffset(28));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), bufferoffset(36));
			}


            break;
            }
            case video::EVT_TANGENTS:
            {

                glEnableVertexAttribArray(EVA_TANGENT);
                glEnableVertexAttribArray(EVA_BINORMAL);

         if (Vertices_Standard.pointer())
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,Pos)));
				glVertexAttribPointer(EVA_NORMAL,   3, GL_FLOAT, false, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,Normal)));
				glVertexAttribPointer(EVA_COLOR,    4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,Color)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,TCoords)));
				glVertexAttribPointer(EVA_TANGENT, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,Tangent)));
				glVertexAttribPointer(EVA_BINORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), reinterpret_cast<void*>(offsetof(video::S3DVertexTangents,Binormal)));

			}
			else
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), 0);
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), bufferoffset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), bufferoffset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), bufferoffset(28));
                glVertexAttribPointer(EVA_TANGENT, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), bufferoffset(36));
				glVertexAttribPointer(EVA_BINORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), bufferoffset(48));
			}


            break;
            }


        }

        GLenum indexSize = sizeof(u16);
        glGenBuffers(1, &ChangedID_Index);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChangedID_Index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexCount()*indexSize, Indices.pointer(), usage_index);


    isBufferCreated=true;
    glBindVertexArray(0);
    }

    virtual void render( ) _IRR_OVERRIDE_
    {
        int PrimitiveCount            =getPrimitiveCount();
       E_PRIMITIVE_TYPE primitiveType=getPrimitiveType();
       render(PrimitiveCount,primitiveType);
    }
    virtual void render( int primitiveCount,E_PRIMITIVE_TYPE pType) _IRR_OVERRIDE_
    {
            if (!isBufferCreated)
           {
             createVertexBuffer();
           }


      glBindVertexArray(ChangedID_array);
   //   glBindBuffer(GL_ARRAY_BUFFER, ChangedID_array);




    if (bufferUpdate==EBT_VERTEX_AND_INDEX || bufferUpdate==EBT_VERTEX)
    {

        glBindBuffer(GL_ARRAY_BUFFER, ChangedID_Vertex);
        //  glBufferData(GL_ARRAY_BUFFER, getVertexCount()*getVertexPitchFromType(getVertexType()), Vertices.pointer(), usage_vertex);
        glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexCount()*getVertexPitchFromType(getVertexType()), getVertices());


    }






    if (bufferUpdate==EBT_VERTEX_AND_INDEX || bufferUpdate==EBT_INDEX)
    {

        GLenum indexSize = sizeof(u16);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChangedID_Index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexCount()*indexSize, Indices.pointer(), usage_index);
        //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, getIndexCount()*indexSize, Indices.pointer());

    }



            switch (pType)
            {
                case EPT_POINTS:
                    glDrawArrays(GL_POINTS, 0, primitiveCount);
                    break;
                case EPT_LINE_STRIP:
                    glDrawElements(GL_LINE_STRIP, primitiveCount+1, indices_type, nullptr);
                    break;
                case EPT_LINE_LOOP:
                    glDrawElements(GL_LINE_LOOP, primitiveCount, indices_type, nullptr);
                    break;
                case EPT_LINES:
                    glDrawElements(GL_LINES, primitiveCount*2, indices_type, nullptr);
                    break;
                case EPT_TRIANGLE_STRIP:
                    glDrawElements(GL_TRIANGLE_STRIP, primitiveCount+2, indices_type, nullptr);
                    break;
                case EPT_TRIANGLE_FAN:
                    glDrawElements(GL_TRIANGLE_FAN, primitiveCount+2, indices_type, nullptr);
                    break;
                case EPT_TRIANGLES:
                  {
                      bool PointCloud=false;
                      bool Wireframe=false;


                     glDrawElements((Material.Wireframe)?GL_LINES:(Material.PointCloud)?GL_POINTS:GL_TRIANGLES, primitiveCount*3, indices_type, nullptr);

                   break;
                    }

            }
      glBindVertexArray(0);
      bufferUpdate=EBT_NONE;

    };

	core::array<video::S3DVertexTangents> Vertices_Tangents;
	core::array<video::S3DVertex2TCoords> Vertices_2TCoords;
	core::array<video::S3DVertex> Vertices_Standard;
	core::array<u16> Indices;

	u32 ChangedID_Vertex;
	u32 ChangedID_Index;

        u32 ChangedID_array;
		GLenum usage_vertex;
		GLenum usage_index;
		GLenum indices_type;

    	E_BUFFER_TYPE bufferUpdate;
		bool isBufferCreated;


	core::matrix4 Transformation;

	video::SMaterial Material;
	video::E_VERTEX_TYPE VertexType;

	core::aabbox3d<f32> BoundingBox;

	//! Primitive type used for rendering (triangles, lines, ...)
	E_PRIMITIVE_TYPE PrimitiveType;
	E_INDEX_TYPE indexType;

	// hardware mapping hint
	E_HARDWARE_MAPPING MappingHint_Vertex;
	E_HARDWARE_MAPPING MappingHint_Index;

	bool BoundingBoxNeedsRecalculated:1;
};


} // end namespace scene
} // end namespace irr

#endif

