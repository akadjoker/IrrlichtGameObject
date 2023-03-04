// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __T_MESH_BUFFER_H_INCLUDED__
#define __T_MESH_BUFFER_H_INCLUDED__

#include "irrArray.h"
#include "IMeshBuffer.h"
#include "os.h"


inline irr::u8* buffer_offset(const long offset)
{
    return ((irr::u8*)0 + offset);
}

namespace irr
{



namespace scene
{

using namespace  video;


	//! Template implementation of the IMeshBuffer interface
	template <class T>
	class CMeshBuffer : public IMeshBuffer
	{

	public:
		//! Default constructor for empty meshbuffer
		CMeshBuffer()
			: ChangedID_Vertex(-1), ChangedID_Index(-1),ChangedID_array(-1),isBufferCreated(false),usage_vertex(GL_STATIC_DRAW)
			,usage_index(GL_STATIC_DRAW),bufferUpdate(EBT_NONE),indices_type(GL_UNSIGNED_SHORT)
			,MappingHint_Vertex(EHM_NEVER), MappingHint_Index(EHM_NEVER),indexType(EIT_16BIT)
			, PrimitiveType(EPT_TRIANGLES)
		{

			setDebugName("CMeshBuffer");
			Material=IdentityMaterial;


		}

    virtual ~CMeshBuffer()
    {

      SDL_Log("[MESHBUFFER]  Delete");

      if (ChangedID_array!=-1)
      {

        SDL_Log("[MESHBUFFER]  Free VAO[%d]",ChangedID_array);
        glDeleteVertexArrays(1,&ChangedID_array);


      }
      if (ChangedID_Index!=-1)
      {
         SDL_Log("[MESHBUFFER]  Free EBO[%d]",ChangedID_Index);
         glDeleteBuffers(1,&ChangedID_Index);

      }
      if (ChangedID_Vertex!=-1)
      {
         SDL_Log("[MESHBUFFER]  Free VBO[%d]",ChangedID_Vertex);
        glDeleteBuffers(1,&ChangedID_Vertex);

      }
      Vertices.clear();
      Indices.clear();
    }

		//! Get material of this meshbuffer
		/** \return Material of this buffer */
		virtual const video::SMaterial& getMaterial() const _IRR_OVERRIDE_
		{
			return Material;
		}


		//! Get material of this meshbuffer
		/** \return Material of this buffer */
		virtual video::SMaterial& getMaterial() _IRR_OVERRIDE_
		{
			return Material;
		}


		//! Get pointer to vertices
		/** \return Pointer to vertices. */
		virtual const void* getVertices() const _IRR_OVERRIDE_
		{
			return Vertices.const_pointer();
		}


		//! Get pointer to vertices
		/** \return Pointer to vertices. */
		virtual void* getVertices() _IRR_OVERRIDE_
		{
			return Vertices.pointer();
		}


		//! Get number of vertices
		/** \return Number of vertices. */
		virtual u32 getVertexCount() const _IRR_OVERRIDE_
		{
			return Vertices.size();
		}

		//! Get type of index data which is stored in this meshbuffer.
		/** \return Index type of this buffer. */
		virtual video::E_INDEX_TYPE getIndexType() const _IRR_OVERRIDE_
		{
					return indexType;
		}

		virtual void setType(video::E_INDEX_TYPE IndexType) _IRR_OVERRIDE_
		{

			indexType=IndexType;
			if (IndexType==EIT_16BIT)
			  indices_type=GL_UNSIGNED_SHORT;
            else
              indices_type=GL_UNSIGNED_INT;


		}

		//! Get pointer to indices
		/** \return Pointer to indices. */
		virtual const u16* getIndices() const _IRR_OVERRIDE_
		{
			return Indices.const_pointer();
		}


		//! Get pointer to indices
		/** \return Pointer to indices. */
		virtual u16* getIndices() _IRR_OVERRIDE_
		{
			return Indices.pointer();
		}


		//! Get number of indices
		/** \return Number of indices. */
		virtual u32 getIndexCount() const _IRR_OVERRIDE_
		{
			return Indices.size();
		}


		//! Get the axis aligned bounding box
		/** \return Axis aligned bounding box of this buffer. */
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_
		{
			return BoundingBox;
		}


		//! Set the axis aligned bounding box
		/** \param box New axis aligned bounding box for this buffer. */
		//! set user axis aligned bounding box
		virtual void setBoundingBox(const core::aabbox3df& box) _IRR_OVERRIDE_
		{
			BoundingBox = box;
		}


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
       SDL_LogWarn(0,"[MESHBUFFER] Create vertex buffer  %d",ChangedID_array);








        glGenBuffers(1, &ChangedID_Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, ChangedID_Vertex);
        glBufferData(GL_ARRAY_BUFFER, getVertexCount()*getVertexPitchFromType(getVertexType()), Vertices.pointer(), usage_vertex);




        //SDL_Log("create EBO  %d",ChangedID_Index);
        //SDL_Log("create VBO %d ",ChangedID_Vertex);

        glEnableVertexAttribArray(video::EVA_POSITION);
        glEnableVertexAttribArray(video::EVA_COLOR);
        glEnableVertexAttribArray(video::EVA_NORMAL);
        glEnableVertexAttribArray(video::EVA_TCOORD0);


      switch (getVertexType())
       {
            case video::EVT_STANDARD:
            {



            if (Vertices.pointer())
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Pos)));
				glVertexAttribPointer(EVA_NORMAL,   3, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Normal)));
				glVertexAttribPointer(EVA_COLOR,    4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,Color)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertex), reinterpret_cast<void*>(offsetof(video::S3DVertex,TCoords)));
			}
			else
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), 0);
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex), buffer_offset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), buffer_offset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex), buffer_offset(28));
			}





            break;
            }
            case video::EVT_2TCOORDS:
            {



               glEnableVertexAttribArray(video::EVA_TCOORD1);

            if (Vertices.pointer())
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Pos)));
				glVertexAttribPointer(EVA_NORMAL,   3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Normal)));
				glVertexAttribPointer(EVA_COLOR,    4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,Color)));
				glVertexAttribPointer(EVA_TCOORD0,  2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,TCoords)));
				glVertexAttribPointer(EVA_TCOORD1,  2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), reinterpret_cast<void*>(offsetof(video::S3DVertex2TCoords,TCoords2)));
			}
			else
			{
				glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), 0);
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), buffer_offset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(28));
				glVertexAttribPointer(EVA_TCOORD1, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(36));
			}


            break;
            }
            case video::EVT_TANGENTS:
            {

                glEnableVertexAttribArray(EVA_TANGENT);
                glEnableVertexAttribArray(EVA_BINORMAL);

         if (Vertices.pointer())
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
				glVertexAttribPointer(EVA_NORMAL, 3, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(12));
				glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex2TCoords), buffer_offset(24));
				glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex2TCoords), buffer_offset(28));
                glVertexAttribPointer(EVA_TANGENT, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(36));
				glVertexAttribPointer(EVA_BINORMAL, 3, GL_FLOAT, false, sizeof(S3DVertexTangents), buffer_offset(48));
			}


            break;
            }


        }


        GLenum indexSize=sizeof(u32);
        glGenBuffers(1, &ChangedID_Index);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChangedID_Index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexCount()*indexSize, Indices.pointer(), usage_index);

        glBindVertexArray(0);
        isBufferCreated=true;

    }
    virtual void render( ) override
    {
       int PrimitiveCount            =getPrimitiveCount();
       E_PRIMITIVE_TYPE primitiveType=getPrimitiveType();
       render(PrimitiveCount,primitiveType);
    }
    virtual void render( int primitiveCount,E_PRIMITIVE_TYPE pType) _IRR_OVERRIDE_
    {
         if(!isBufferCreated)
         {
            SDL_LogWarn(0,"[MESHBUFFER] *** Create mesh buffer on render  *** ");
            createVertexBuffer();
         }

      glBindVertexArray(ChangedID_array);
     // glBindBuffer(GL_ARRAY_BUFFER, ChangedID_array);



    if (bufferUpdate==EBT_VERTEX_AND_INDEX || bufferUpdate==EBT_VERTEX)
    {

        glBindBuffer(GL_ARRAY_BUFFER, ChangedID_Vertex);
        //  glBufferData(GL_ARRAY_BUFFER, getVertexCount()*getVertexPitchFromType(getVertexType()), Vertices.pointer(), usage_vertex);
        glBufferSubData(GL_ARRAY_BUFFER, 0, getVertexCount()*getVertexPitchFromType(getVertexType()), Vertices.pointer());


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
                     glDrawElements((Material.Wireframe)?GL_LINES:(Material.PointCloud)?GL_POINTS:GL_TRIANGLES, primitiveCount*3, indices_type, nullptr);
                    break;
                    }

            }
      glBindVertexArray(0);
      bufferUpdate=EBT_NONE;
    }





		//! Recalculate the bounding box.
		/** should be called if the mesh changed. */
		virtual void recalculateBoundingBox() _IRR_OVERRIDE_
		{
		 if (ChangedID_array==-1){createVertexBuffer();}

			if (!Vertices.empty())
			{
				BoundingBox.reset(Vertices[0].Pos);
				const irr::u32 vsize = Vertices.size();
				for (u32 i=1; i<vsize; ++i)
					BoundingBox.addInternalPoint(Vertices[i].Pos);
			}
			else
				BoundingBox.reset(0,0,0);

		}


		//! Get type of vertex data stored in this buffer.
		/** \return Type of vertex data. */
		virtual video::E_VERTEX_TYPE getVertexType() const _IRR_OVERRIDE_
		{
			return T::getType();
		}

		//! returns position of vertex i
		virtual const core::vector3df& getPosition(u32 i) const _IRR_OVERRIDE_
		{
			return Vertices[i].Pos;
		}

		//! returns position of vertex i
		virtual core::vector3df& getPosition(u32 i) _IRR_OVERRIDE_
		{
			return Vertices[i].Pos;
		}

		//! returns normal of vertex i
		virtual const core::vector3df& getNormal(u32 i) const _IRR_OVERRIDE_
		{
			return Vertices[i].Normal;
		}

		//! returns normal of vertex i
		virtual core::vector3df& getNormal(u32 i) _IRR_OVERRIDE_
		{
			return Vertices[i].Normal;
		}

		//! returns texture coord of vertex i
		virtual const core::vector2df& getTCoords(u32 i) const _IRR_OVERRIDE_
		{
			return Vertices[i].TCoords;
		}

		//! returns texture coord of vertex i
		virtual core::vector2df& getTCoords(u32 i) _IRR_OVERRIDE_
		{
			return Vertices[i].TCoords;
		}


		//! Append the vertices and indices to the current buffer
		/** Only works for compatible types, i.e. either the same type
		or the main buffer is of standard type. Otherwise, behavior is
		undefined.
		*/
		virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) _IRR_OVERRIDE_
		{
			if (vertices == getVertices())
				return;

			const u32 vertexCount = getVertexCount();
			u32 i;

			Vertices.reallocate(vertexCount+numVertices);
			for (i=0; i<numVertices; ++i)
			{
				Vertices.push_back(reinterpret_cast<const T*>(vertices)[i]);
				BoundingBox.addInternalPoint(reinterpret_cast<const T*>(vertices)[i].Pos);
			}

			Indices.reallocate(getIndexCount()+numIndices);
			for (i=0; i<numIndices; ++i)
			{
				Indices.push_back(indices[i]+vertexCount);
//				Indices.push_back(reinterpret_cast<const T*>(indices)[i]+vertexCount);

			}

		}


		//! Append the meshbuffer to the current buffer
		/** Only works for compatible types, i.e. either the same type
		or the main buffer is of standard type. Otherwise, behavior is
		undefined.
		\param other Meshbuffer to be appended to this one.
		*/
		virtual void append(const IMeshBuffer* const other) _IRR_OVERRIDE_
		{

			if (this==other)
				return;

			const u32 vertexCount = getVertexCount();
			u32 i;

			Vertices.reallocate(vertexCount+other->getVertexCount());
			for (i=0; i<other->getVertexCount(); ++i)
			{
				Vertices.push_back(reinterpret_cast<const T*>(other->getVertices())[i]);
			}

			Indices.reallocate(getIndexCount()+other->getIndexCount());
			for (i=0; i<other->getIndexCount(); ++i)
			{
				Indices.push_back(other->getIndices()[i]+vertexCount);
			//	Indices.push_back(reinterpret_cast<const T*>(other->getIndices())[i]+vertexCount);
			}
			BoundingBox.addInternalBox(other->getBoundingBox());


		}


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
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Vertex() const _IRR_OVERRIDE_ {return ChangedID_Vertex;}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Index() const _IRR_OVERRIDE_ {return ChangedID_Index;}

		u32 ChangedID_Vertex;
		u32 ChangedID_Index;
		u32 ChangedID_array;
		GLenum usage_vertex;
		GLenum usage_index;
		GLenum indices_type;


    	E_BUFFER_TYPE bufferUpdate;
		bool isBufferCreated;



		//! hardware mapping hint
		E_HARDWARE_MAPPING MappingHint_Vertex;
		E_HARDWARE_MAPPING MappingHint_Index;

		//! Material for this meshbuffer.
		video::SMaterial Material;
		//! Vertices of this buffer
		core::array<T> Vertices;
		//! Indices into the vertices of this buffer.
		core::array<u16> Indices;
		//! Bounding box of this meshbuffer.
		core::aabbox3d<f32> BoundingBox;
		//! Primitive type used for rendering (triangles, lines, ...)
		E_PRIMITIVE_TYPE PrimitiveType;
	    E_INDEX_TYPE indexType;
	};

	//! Standard meshbuffer
	typedef CMeshBuffer<video::S3DVertex> SMeshBuffer;
	//! Meshbuffer with two texture coords per vertex, e.g. for lightmaps
	typedef CMeshBuffer<video::S3DVertex2TCoords> SMeshBufferLightMap;
	//! Meshbuffer with vertices having tangents stored, e.g. for normal mapping
	typedef CMeshBuffer<video::S3DVertexTangents> SMeshBufferTangents;
} // end namespace scene
} // end namespace irr

#endif


