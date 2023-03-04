// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GEOMETRY_CREATOR_H_INCLUDED__
#define __C_GEOMETRY_CREATOR_H_INCLUDED__

#include "IGeometryCreator.h"
#include "IMeshBuffer.h"
#include "CMeshBuffer.h"
#include "S3DVertex.h"
#include "SMesh.h"
#include "irrMap.h"
#include "IFileSystem.h"
#include "CSkinnedMesh.h"
#include "CAnimatedMeshMD2.h"
#include "IAnimatedMeshMD2.h"
#include "CSkinnedMesh.h"
#include "ISkinnedMesh.h"

namespace irr
{

namespace scene
{
class IAnimatedMesh;

//! class for creating geometry on the fly
class CGeometryCreator : public IGeometryCreator
{
	void addToBuffer(const video::S3DVertex& v, SMeshBuffer* Buffer) const;
public:
	virtual IMesh* createCubeMesh(const core::vector3df& size, ECUBE_MESH_TYPE type) const _IRR_OVERRIDE_;
	virtual IMesh* createCubeMesh(ECUBE_MESH_TYPE type) const _IRR_OVERRIDE_;

	virtual IMesh* createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize, const core::dimension2d<u32>& tileCount,
		video::SMaterial* material, f32 hillHeight, const core::dimension2d<f32>& countHills,
		const core::dimension2d<f32>& textureRepeatCount) const _IRR_OVERRIDE_;

	virtual IMesh* createGeoplaneMesh(f32 radius, u32 rows, u32 columns) const _IRR_OVERRIDE_;

	virtual IMesh* createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight, 	const core::dimension2d<u32>& defaultVertexBlockSize,
		bool debugBorders=false) const _IRR_OVERRIDE_;

	virtual IMesh* createArrowMesh(const u32 tesselationCylinder,
			const u32 tesselationCone, const f32 height,
			const f32 cylinderHeight, const f32 width0,
			const f32 width1, const video::SColor vtxColor0,
			const video::SColor vtxColor1) const _IRR_OVERRIDE_;

	virtual IMesh* createSphereMesh(f32 radius, u32 polyCountX, u32 polyCountY) const _IRR_OVERRIDE_;

	virtual IMesh* createCylinderMesh(f32 radius, f32 length, u32 tesselation,
				const video::SColor& color=0xffffffff,
				bool closeTop=true, f32 oblique=0.f, u32 normalType=0) const _IRR_OVERRIDE_;

	virtual IMesh* createConeMesh(f32 radius, f32 length, u32 tesselation,
				const video::SColor& colorTop=0xffffffff,
				const video::SColor& colorBottom=0xffffffff,
				f32 oblique=0.f) const _IRR_OVERRIDE_;

	virtual IMesh* createVolumeLightMesh(
			const u32 subdivideU=32, const u32 subdivideV=32,
			const video::SColor footColor=0xffffffff,
			const video::SColor tailColor=0xffffffff,
			const f32 lpDistance = 8.f,
			const core::vector3df& lightDim = core::vector3df(1.f,1.2f,1.f)) const _IRR_OVERRIDE_;


	virtual IAnimatedMesh* importMesh(const char* filename)  const _IRR_OVERRIDE_;

};


class IMeshLoader
{
public:

	IMeshLoader()  {}
	virtual ~IMeshLoader(){}
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) = 0;


protected:

};




//! Meshloader capable of loading 3ds meshes.
class C3DSMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	C3DSMeshFileLoader();

	//! destructor
	virtual ~C3DSMeshFileLoader();
	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:

// byte-align structures
#include "irrpack.h"

	struct ChunkHeader
	{
		u16 id;
		s32 length;
	} PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

	struct ChunkData
	{
		ChunkData() : read(0) {}

		ChunkHeader header;
		s32 read;
	};

	struct SCurrentMaterial
	{
		void clear() {
			Material=video::SMaterial();
			Name="";
			Filename[0]="";
			Filename[1]="";
			Filename[2]="";
			Filename[3]="";
			Filename[4]="";
			Strength[0]=0.f;
			Strength[1]=0.f;
			Strength[2]=0.f;
			Strength[3]=0.f;
			Strength[4]=0.f;
		}

		video::SMaterial Material;
		core::stringc Name;
		core::stringc Filename[5];
		f32 Strength[5];
	};

	struct SMaterialGroup
	{
		SMaterialGroup() : faceCount(0), faces(0) {};

		SMaterialGroup(const SMaterialGroup& o)
		{
			*this = o;
		}

		~SMaterialGroup()
		{
			clear();
		}

		void clear()
		{
			delete [] faces;
			faces = 0;
			faceCount = 0;
		}

		void operator =(const SMaterialGroup& o)
		{
			MaterialName = o.MaterialName;
			faceCount = o.faceCount;
			faces = new u16[faceCount];
			for (u16 i=0; i<faceCount; ++i)
				faces[i] = o.faces[i];
		}

		core::stringc MaterialName;
		u16 faceCount;
		u16* faces;
	};

	bool readChunk(io::IReadFile* file, ChunkData* parent);
	bool readMaterialChunk(io::IReadFile* file, ChunkData* parent);
	bool readFrameChunk(io::IReadFile* file, ChunkData* parent);
	bool readTrackChunk(io::IReadFile* file, ChunkData& data,
				IMeshBuffer* mb, const core::vector3df& pivot);
	bool readObjectChunk(io::IReadFile* file, ChunkData* parent);
	bool readPercentageChunk(io::IReadFile* file, ChunkData* chunk, f32& percentage);
	bool readColorChunk(io::IReadFile* file, ChunkData* chunk, video::SColor& out);

	void readChunkData(io::IReadFile* file, ChunkData& data);
	void readString(io::IReadFile* file, ChunkData& data, core::stringc& out);
	void readVertices(io::IReadFile* file, ChunkData& data);
	void readIndices(io::IReadFile* file, ChunkData& data);
	void readMaterialGroup(io::IReadFile* file, ChunkData& data);
	void readTextureCoords(io::IReadFile* file, ChunkData& data);

	void composeObject(io::IReadFile* file, const core::stringc& name);
	void loadMaterials(io::IReadFile* file);
	void cleanUp();


	io::IFileSystem* FileSystem;

	f32* Vertices;
	u16* Indices;
	u32* SmoothingGroups;
	core::array<u16> TempIndices;
	f32* TCoords;
	u16 CountVertices;
	u16 CountFaces; // = CountIndices/4
	u16 CountTCoords;
	core::array<SMaterialGroup> MaterialGroups;

	SCurrentMaterial CurrentMaterial;
	core::array<SCurrentMaterial> Materials;
	core::array<core::stringc> MeshBufferNames;
	core::matrix4 TransformationMatrix;

	SMesh* Mesh;
};



//! Meshloader capable of loading obj meshes.
class COBJMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	COBJMeshFileLoader();

	//! destructor
	virtual ~COBJMeshFileLoader();


	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:

	struct SObjMtl
	{
		SObjMtl() : Meshbuffer(0), Bumpiness (1.0f), Illumination(0),
			RecalculateNormals(false)
		{
			Meshbuffer = new SMeshBuffer();
			Meshbuffer->Material.Shininess = 0.0f;
			Meshbuffer->Material.AmbientColor = video::SColorf(0.2f, 0.2f, 0.2f, 1.0f).toSColor();
			Meshbuffer->Material.DiffuseColor = video::SColorf(0.8f, 0.8f, 0.8f, 1.0f).toSColor();
			Meshbuffer->Material.SpecularColor = video::SColorf(1.0f, 1.0f, 1.0f, 1.0f).toSColor();
		}

		SObjMtl(const SObjMtl& o)
			: Name(o.Name), Group(o.Group),
			Bumpiness(o.Bumpiness), Illumination(o.Illumination),
			RecalculateNormals(false)
		{
			Meshbuffer = new SMeshBuffer();
			Meshbuffer->Material = o.Meshbuffer->Material;
		}

		core::map<video::S3DVertex, int> VertMap;
		scene::SMeshBuffer *Meshbuffer;
		core::stringc Name;
		core::stringc Group;
		f32 Bumpiness;
		c8 Illumination;
		bool RecalculateNormals;
	};

	// helper method for material reading
	const c8* readTextures(const c8* bufPtr, const c8* const bufEnd, SObjMtl* currMaterial, const io::path& relPath);

	// returns a pointer to the first printable character available in the buffer
	const c8* goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines=true);
	// returns a pointer to the first printable character after the first non-printable
	const c8* goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines=true);
	// returns a pointer to the next printable character after the first line break
	const c8* goNextLine(const c8* buf, const c8* const bufEnd);
	// copies the current word from the inBuf to the outBuf
	u32 copyWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);
	// copies the current line from the inBuf to the outBuf
	core::stringc copyLine(const c8* inBuf, const c8* const bufEnd);

	// combination of goNextWord followed by copyWord
	const c8* goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);

	//! Read the material from the given file
	void readMTL(const c8* fileName, const io::path& relPath);

	//! Find and return the material with the given name
	SObjMtl* findMtl(const core::stringc& mtlName, const core::stringc& grpName);

	//! Read RGB color
	const c8* readColor(const c8* bufPtr, video::SColor& color, const c8* const pBufEnd);
	//! Read 3d vector of floats
	const c8* readVec3(const c8* bufPtr, core::vector3df& vec, const c8* const pBufEnd);
	//! Read 2d vector of floats
	const c8* readUV(const c8* bufPtr, core::vector2df& vec, const c8* const pBufEnd);
	//! Read boolean value represented as 'on' or 'off'
	const c8* readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd);

	// reads and convert to integer the vertex indices in a line of obj file's face statement
	// -1 for the index if it doesn't exist
	// indices are changed to 0-based index instead of 1-based from the obj file
	bool retrieveVertexIndices(c8* vertexData, s32* idx, const c8* bufEnd, u32 vbsize, u32 vtsize, u32 vnsize);

	void cleanUp();


	io::IFileSystem* FileSystem;

	core::array<SObjMtl*> Materials;
};


//! Meshloader capable of loading MD2 files
class CMD2MeshFileLoader  : public IMeshLoader
{
public:

	//! Constructor
	CMD2MeshFileLoader();


	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:
	//! Loads the file data into the mesh
	bool loadFile(io::IReadFile* file, CAnimatedMeshMD2* mesh);

};


//! Meshloader capable of loading md3 files.
class CMD3MeshFileLoader : public IMeshLoader
{



public:

	//! Constructor
	CMD3MeshFileLoader( );

	//! destructor
	virtual ~CMD3MeshFileLoader();

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:


};


//! Meshloader capable of loading Milkshape 3D files
class CMS3DMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CMS3DMeshFileLoader();


	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:

	core::stringc stripPathFromString(const core::stringc& inString, bool returnPath) const;

	bool load(io::IReadFile* file);

	CSkinnedMesh* AnimatedMesh;
};

} // end namespace scene
} // end namespace irr

#endif

