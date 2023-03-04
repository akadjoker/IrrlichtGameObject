// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "CImage.h"
#include "CGeometryCreator.h"
#include "IAnimatedMesh.h"
#include "IMeshBuffer.h"
#include "SMesh.h"
#include "IMesh.h"
#include "Engine.h"
#include "os.h"
#include "CMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "IReadFile.h"
#include "IMeshManipulator.h"
#include "CAnimatedMeshMD2.h"
#include "CAnimatedMeshMD3.h"
#include "CSkinnedMesh.h"

namespace irr
{
namespace scene
{

IMesh* CGeometryCreator::createCubeMesh(const core::vector3df& size, ECUBE_MESH_TYPE type) const
{
	SMesh* mesh = new SMesh;

	const video::SColor clr(255,255,255,255);

	if ( type == ECMT_1BUF_12VTX_NA )
	{
		SMeshBuffer* buffer = new SMeshBuffer();

		// Create indices (pos, neg describes normal direction of front-face)
		const u16 u[36] = {	0,2,1,   0,3,2,		// NEG_Z
							1,5,4,   1,2,5,		// POS_X
							4,6,7,   4,5,6,		// POS_Z
							7,3,0,   7,6,3,		// NEG_X
							9,5,2,   9,8,5,		// POS_Y
							0,11,10, 0,10,7};	// NEG_Y

		buffer->Indices.set_used(36);

		for (u32 i=0; i<36; ++i)
			buffer->Indices[i] = u[i];

		// Create vertices
		buffer->Vertices.reallocate(12);

		buffer->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr, 0, 1));	// 0
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 1, 1));	// 1
		buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 1,-1, clr, 1, 0));	// 2
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 0, 0));	// 3
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 0, 1));	// 4
		buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 1, 1, clr, 0, 0));	// 5
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 1, 0));	// 6
		buffer->Vertices.push_back(video::S3DVertex(0,0,1, -1,-1, 1, clr, 1, 1));	// 7
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 0, 1));	// 8
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 1, 1));	// 9
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 1, 0));	// 10
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 0, 0));	// 11

		// Recalculate bounding box and set cube size
		buffer->BoundingBox.reset(0,0,0);

		for (u32 i=0; i<12; ++i)
		{
			//buffer->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
		//	buffer->Vertices[i].Pos *= size;
			buffer->Vertices[i].Normal.normalize();
			buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
		}

		buffer->setDirty();
		mesh->addMeshBuffer(buffer);

	}
	else if ( type == ECMT_6BUF_4VTX_NP )
	{
		for ( int b=0; b<6; ++b )
		{
			SMeshBuffer* buffer = new SMeshBuffer();

			// Create indices
			const u16 u[6] = { 0,2,1,0,3,2};

			buffer->Indices.set_used(6);

			for ( int i=0; i<6; ++i )
				buffer->Indices[i] = u[i];

			// Create vertices
			buffer->Vertices.reallocate(4);

			switch ( b )
			{
				case 0:
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,	0, 0,-1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, 0,-1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 0,-1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  0, 0,-1, clr, 0, 0));
				break;
				case 1:
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,	1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 0, 0, clr, 0, 0));
				break;
				case 2:
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,	0, 0, 1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,  0, 0, 1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 0, 1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 0, 1, clr, 0, 0));
				break;
				case 3:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	-1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  -1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  -1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  -1, 0, 0, clr, 0, 0));
				break;
				case 4:
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,	0, 1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 1, 0, clr, 0, 0));
				break;
				case 5:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	0, -1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  0, -1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, -1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  0, -1, 0, clr, 0, 0));
				break;
			}

			// Recalculate bounding box and set cube size
			for (u32 i=0; i<4; ++i)
			{
				buffer->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
				buffer->Vertices[i].Pos *= size;
				if ( i == 0 )
					buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
				else
					buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
			}

			buffer->setDirty();
			mesh->addMeshBuffer(buffer);

		}
	}


	mesh->recalculateBoundingBox();
	return mesh;
}

IMesh* CGeometryCreator::createCubeMesh( ECUBE_MESH_TYPE type) const
{
	SMesh* mesh = new SMesh;

	const video::SColor clr(255,255,255,255);

	if ( type == ECMT_1BUF_12VTX_NA )
	{
		SMeshBuffer* buffer = new SMeshBuffer();

		// Create indices (pos, neg describes normal direction of front-face)
		const u16 u[36] = {	0,2,1,   0,3,2,		// NEG_Z
							1,5,4,   1,2,5,		// POS_X
							4,6,7,   4,5,6,		// POS_Z
							7,3,0,   7,6,3,		// NEG_X
							9,5,2,   9,8,5,		// POS_Y
							0,11,10, 0,10,7};	// NEG_Y

		buffer->Indices.set_used(36);

		for (u32 i=0; i<36; ++i)
			buffer->Indices[i] = u[i];

		// Create vertices
		buffer->Vertices.reallocate(12);

		buffer->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr, 0, 1));	// 0
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 1, 1));	// 1
		buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 1,-1, clr, 1, 0));	// 2
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 0, 0));	// 3
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 0, 1));	// 4
		buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 1, 1, clr, 0, 0));	// 5
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 1, 0));	// 6
		buffer->Vertices.push_back(video::S3DVertex(0,0,1, -1,-1, 1, clr, 1, 1));	// 7
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 0, 1));	// 8
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 1, 1));	// 9
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 1, 0));	// 10
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 0, 0));	// 11

		// Recalculate bounding box and set cube size
		buffer->BoundingBox.reset(0,0,0);

		for (u32 i=0; i<12; ++i)
		{

		    buffer->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
            //buffer->Vertices[i].Pos *= size;

			buffer->Vertices[i].Normal.normalize();
			buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
		}
        buffer->setDirty();
		mesh->addMeshBuffer(buffer);

	}
	else if ( type == ECMT_6BUF_4VTX_NP )
	{
		for ( int b=0; b<6; ++b )
		{
			SMeshBuffer* buffer = new SMeshBuffer();

			// Create indices
			const u16 u[6] = { 0,2,1,0,3,2};

			buffer->Indices.set_used(6);

			for ( int i=0; i<6; ++i )
				buffer->Indices[i] = u[i];

			// Create vertices
			buffer->Vertices.reallocate(4);

			switch ( b )
			{
				case 0:
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,	0, 0,-1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, 0,-1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 0,-1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  0, 0,-1, clr, 0, 0));
				break;
				case 1:
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,	1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 0, 0, clr, 0, 0));
				break;
				case 2:
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,	0, 0, 1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,  0, 0, 1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 0, 1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 0, 1, clr, 0, 0));
				break;
				case 3:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	-1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  -1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  -1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  -1, 0, 0, clr, 0, 0));
				break;
				case 4:
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,	0, 1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 1, 0, clr, 0, 0));
				break;
				case 5:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	0, -1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  0, -1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, -1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  0, -1, 0, clr, 0, 0));
				break;
			}

			// Recalculate bounding box and set cube size
			for (u32 i=0; i<4; ++i)
			{

				if ( i == 0 )
					buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
				else
					buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
			}
            buffer->setDirty();
			mesh->addMeshBuffer(buffer);

		}
	}

	mesh->recalculateBoundingBox();
	return mesh;
}

// creates a hill plane
IMesh* CGeometryCreator::createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize,
		const core::dimension2d<u32>& tc, video::SMaterial* material,
		f32 hillHeight, const core::dimension2d<f32>& ch,
		const core::dimension2d<f32>& textureRepeatCount) const
{
	core::dimension2d<u32> tileCount = tc;
	core::dimension2d<f32> countHills = ch;

	if (countHills.Width < 0.01f)
		countHills.Width = 1.f;
	if (countHills.Height < 0.01f)
		countHills.Height = 1.f;

	// center
	const core::position2d<f32> center((tileSize.Width * tileCount.Width) * 0.5f, (tileSize.Height * tileCount.Height) * 0.5f);

	// texture coord step
	const core::dimension2d<f32> tx(
			textureRepeatCount.Width / tileCount.Width,
			textureRepeatCount.Height / tileCount.Height);

	// add one more point in each direction for proper tile count
	++tileCount.Height;
	++tileCount.Width;

	SMeshBuffer* buffer = new SMeshBuffer();
	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	// create vertices from left-front to right-back
	u32 x;

	f32 sx=0.f, tsx=0.f;
	for (x=0; x<tileCount.Width; ++x)
	{
		f32 sy=0.f, tsy=0.f;
		for (u32 y=0; y<tileCount.Height; ++y)
		{
			vtx.Pos.set(sx - center.X, 0, sy - center.Y);
			vtx.TCoords.set(tsx, 1.0f - tsy);

			if (core::isnotzero(hillHeight))
				vtx.Pos.Y = sinf(vtx.Pos.X * countHills.Width * core::PI / center.X) *
					cosf(vtx.Pos.Z * countHills.Height * core::PI / center.Y) *
					hillHeight;

			buffer->Vertices.push_back(vtx);
			sy += tileSize.Height;
			tsy += tx.Height;
		}
		sx += tileSize.Width;
		tsx += tx.Width;
	}

	// create indices

	for (x=0; x<tileCount.Width-1; ++x)
	{
		for (u32 y=0; y<tileCount.Height-1; ++y)
		{
			const s32 current = x*tileCount.Height + y;

			buffer->Indices.push_back(current);
			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + tileCount.Height);

			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + 1 + tileCount.Height);
			buffer->Indices.push_back(current + tileCount.Height);
		}
	}

	// recalculate normals
	for (u32 i=0; i<buffer->Indices.size(); i+=3)
	{
		const core::vector3df normal = core::plane3d<f32>(
			buffer->Vertices[buffer->Indices[i+0]].Pos,
			buffer->Vertices[buffer->Indices[i+1]].Pos,
			buffer->Vertices[buffer->Indices[i+2]].Pos).Normal;

		buffer->Vertices[buffer->Indices[i+0]].Normal = normal;
		buffer->Vertices[buffer->Indices[i+1]].Normal = normal;
		buffer->Vertices[buffer->Indices[i+2]].Normal = normal;
	}

	if (material)
		buffer->Material = *material;

	buffer->recalculateBoundingBox();
	buffer->setHardwareMappingHint(EHM_STATIC);
	buffer->setDirty();

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	return mesh;
}

namespace
{

// Return the position on an exponential curve. Input from 0 to 1.
float geopos(float pos)
{
	pos = core::clamp<float>(pos, 0, 1);
	pos *= 5;

	const float out = powf(2.5f, pos - 5);

	return out;
}

}

//! Create a geoplane.
IMesh* CGeometryCreator::createGeoplaneMesh(f32 radius, u32 rows, u32 columns) const
{
	using namespace core;
	using namespace video;

	rows = clamp<u32>(rows, 3, 2048);
	columns = clamp<u32>(columns, 3, 2048);

	SMeshBuffer * const mb = new SMeshBuffer();
	S3DVertex v(0, 0, 0, 0, 1, 0, SColor(255, 255, 255, 255), 0, 0);
	const float anglestep = (2 * PI) / columns;

	mb->Vertices.reallocate((rows * columns) + 1);
	mb->Indices.reallocate((((rows - 2) * columns * 2) + columns) * 3);

	u32 i, j;
	mb->Vertices.push_back(v);
	for (j = 1; j < rows; j++)
	{
		const float len = radius * geopos((float) j/(rows-1));

		for (i = 0; i < columns; i++)
		{
			const float angle = anglestep * i;
			v.Pos = vector3df(len * sinf(angle), 0, len * cosf(angle));

			mb->Vertices.push_back(v);
		}
	}

	// Indices
	// First the inner fan
	for (i = 0; i < columns; i++)
	{
		mb->Indices.push_back(0);
		mb->Indices.push_back(1 + i);

		if (i == columns - 1)
			mb->Indices.push_back(1);
		else
			mb->Indices.push_back(2 + i);
	}

	// Then the surrounding quads
	for (j = 0; j < rows - 2; j++)
	{
		for (i = 0; i < columns; i++)
		{
			u32 start = ((j * columns) + i) + 1;
			u32 next = start + 1;
			u32 far = (((j + 1) * columns) + i) + 1;
			u32 farnext = far + 1;

			if (i == columns - 1)
			{
				next = ((j * columns)) + 1;
				farnext = (((j + 1) * columns)) + 1;
			}

			mb->Indices.push_back(start);
			mb->Indices.push_back(far);
			mb->Indices.push_back(next);

			mb->Indices.push_back(next);
			mb->Indices.push_back(far);
			mb->Indices.push_back(farnext);
		}
	}

	// Done
	SMesh * const mesh = new SMesh();

	mesh->addMeshBuffer(mb);
	mb->recalculateBoundingBox();
	mb->setHardwareMappingHint(EHM_STATIC);
	mb->setDirty();
	mesh->recalculateBoundingBox();


	return mesh;
}

IMesh* CGeometryCreator::createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight,
		const core::dimension2d<u32>& maxVtxBlockSize,
		bool debugBorders) const
{
	if (!texture || !heightmap)
		return 0;

	// debug border
	const s32 borderSkip = debugBorders ? 0 : 1;

	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	SMesh* mesh = new SMesh();

	const u32 tm = os::Timer::getRealTime()/1000;
	const core::dimension2d<u32> hMapSize= heightmap->getDimension();
	const core::dimension2d<u32> tMapSize= texture->getDimension();
	const core::position2d<f32> thRel(static_cast<f32>(tMapSize.Width) / hMapSize.Width, static_cast<f32>(tMapSize.Height) / hMapSize.Height);
	maxHeight /= 255.0f; // height step per color value

	core::position2d<u32> processed(0,0);
	while (processed.Y<hMapSize.Height)
	{
		while(processed.X<hMapSize.Width)
		{
			core::dimension2d<u32> blockSize = maxVtxBlockSize;
			if (processed.X + blockSize.Width > hMapSize.Width)
				blockSize.Width = hMapSize.Width - processed.X;
			if (processed.Y + blockSize.Height > hMapSize.Height)
				blockSize.Height = hMapSize.Height - processed.Y;

			SMeshBuffer* buffer = new SMeshBuffer();
			buffer->setHardwareMappingHint(scene::EHM_STATIC);
			buffer->Vertices.reallocate(blockSize.getArea());
			// add vertices of vertex block
			u32 y;
			core::vector2df pos(0.f, processed.Y*stretchSize.Height);
			const core::vector2df bs(1.f/blockSize.Width, 1.f/blockSize.Height);
			core::vector2df tc(0.f, 0.5f*bs.Y);
			for (y=0; y<blockSize.Height; ++y)
			{
				pos.X=processed.X*stretchSize.Width;
				tc.X=0.5f*bs.X;
				for (u32 x=0; x<blockSize.Width; ++x)
				{
					const f32 height = heightmap->getPixel(x+processed.X, y+processed.Y).getAverage() * maxHeight;

					vtx.Pos.set(pos.X, height, pos.Y);
					vtx.TCoords.set(tc);
					buffer->Vertices.push_back(vtx);
					pos.X += stretchSize.Width;
					tc.X += bs.X;
				}
				pos.Y += stretchSize.Height;
				tc.Y += bs.Y;
			}

			buffer->Indices.reallocate((blockSize.Height-1)*(blockSize.Width-1)*6);
			// add indices of vertex block
			s32 c1 = 0;
			for (y=0; y<blockSize.Height-1; ++y)
			{
				for (u32 x=0; x<blockSize.Width-1; ++x)
				{
					const s32 c = c1 + x;

					buffer->Indices.push_back(c);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1);

					buffer->Indices.push_back(c + 1);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1 + blockSize.Width);
				}
				c1 += blockSize.Width;
			}

			// recalculate normals
			for (u32 i=0; i<buffer->Indices.size(); i+=3)
			{
				const core::vector3df normal = core::plane3d<f32>(
					buffer->Vertices[buffer->Indices[i+0]].Pos,
					buffer->Vertices[buffer->Indices[i+1]].Pos,
					buffer->Vertices[buffer->Indices[i+2]].Pos).Normal;

				buffer->Vertices[buffer->Indices[i+0]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+1]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+2]].Normal = normal;
			}

			if (buffer->Vertices.size())
			{
				c8 textureName[64];
				// create texture for this block


				video::IImage* img = new CImage(texture->getColorFormat(), core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));//driver->createImage(texture->getColorFormat(), core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));
				texture->copyTo(img, core::position2di(0,0), core::recti(
					core::position2d<s32>(core::floor32(processed.X*thRel.X), core::floor32(processed.Y*thRel.Y)),
					core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y))), 0);

				sprintf(textureName, "terrain%u_%u", tm, mesh->getMeshBufferCount());

				buffer->Material.setTexture(0, scene::Engine::Instance()->addTexture(textureName, img));

				if (buffer->Material.getTexture(0))
				{
					c8 tmp[255];
					sprintf(tmp, "Generated terrain texture (%ux%u): %s",
						buffer->Material.getTexture(0)->getSize().Width,
						buffer->Material.getTexture(0)->getSize().Height,
						textureName);
					os::Printer::log(tmp);
				}
				else
					os::Printer::log("Could not create terrain texture.", textureName, ELL_ERROR);


				img->drop();
			}

			buffer->recalculateBoundingBox();
			buffer->setDirty();
			mesh->addMeshBuffer(buffer);


			// keep on processing
			processed.X += maxVtxBlockSize.Width - borderSkip;
		}

		// keep on processing
		processed.X = 0;
		processed.Y += maxVtxBlockSize.Height - borderSkip;
	}

	mesh->recalculateBoundingBox();
	return mesh;
}


/*
	a cylinder, a cone and a cross
	point up on (0,1.f, 0.f )
*/
IMesh* CGeometryCreator::createArrowMesh(const u32 tesselationCylinder,
						const u32 tesselationCone,
						const f32 height,
						const f32 cylinderHeight,
						const f32 width0,
						const f32 width1,
						const video::SColor vtxColor0,
						const video::SColor vtxColor1) const
{
	SMesh* mesh = (SMesh*)createCylinderMesh(width0, cylinderHeight, tesselationCylinder, vtxColor0, false);

	IMesh* mesh2 = createConeMesh(width1, height-cylinderHeight, tesselationCone, vtxColor1, vtxColor0);
	for (u32 i=0; i<mesh2->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh2->getMeshBuffer(i);
		for (u32 j=0; j<buffer->getVertexCount(); ++j)
			buffer->getPosition(j).Y += cylinderHeight;
		buffer->setDirty(EBT_VERTEX);
		buffer->recalculateBoundingBox();
		mesh->addMeshBuffer(buffer);
	}

	mesh->setHardwareMappingHint(EHM_STATIC);

	mesh->recalculateBoundingBox();
	return mesh;
}


/* A sphere with proper normals and texture coords */
IMesh* CGeometryCreator::createSphereMesh(f32 radius, u32 polyCountX, u32 polyCountY) const
{
	// thanks to Alfaz93 who made his code available for Irrlicht on which
	// this one is based!

	// we are creating the sphere mesh here.

	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level

	SMeshBuffer* buffer = new SMeshBuffer();

	buffer->Indices.reallocate((polyCountX * polyCountY) * 6);

	const video::SColor clr(255, 255,255,255);

	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			buffer->Indices.push_back(curr + polyCountXPitch);
			buffer->Indices.push_back(curr);
			buffer->Indices.push_back(curr + 1);
			buffer->Indices.push_back(curr + polyCountXPitch);
			buffer->Indices.push_back(curr+1);
			buffer->Indices.push_back(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		buffer->Indices.push_back(level + polyCountX - 1 + polyCountXPitch);
		buffer->Indices.push_back(level + polyCountX - 1);
		buffer->Indices.push_back(level + polyCountX);

		buffer->Indices.push_back(level + polyCountX - 1 + polyCountXPitch);
		buffer->Indices.push_back(level + polyCountX);
		buffer->Indices.push_back(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	const u32 polyCountSq1 = polyCountSq + 1; // bottom point
	const u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		buffer->Indices.push_back(polyCountSq);
		buffer->Indices.push_back(p2 + 1);
		buffer->Indices.push_back(p2);

		// create triangles which are at the bottom of the sphere

		buffer->Indices.push_back(polyCountSqM1 + p2);
		buffer->Indices.push_back(polyCountSqM1 + p2 + 1);
		buffer->Indices.push_back(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere

	buffer->Indices.push_back(polyCountSq);
	buffer->Indices.push_back(polyCountX);
	buffer->Indices.push_back(polyCountX-1);

	// create final triangle which is at the bottom of the sphere

	buffer->Indices.push_back(polyCountSqM1 + polyCountX - 1);
	buffer->Indices.push_back(polyCountSqM1);
	buffer->Indices.push_back(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const f64 AngleX = 2 * core::PI / polyCountX;
	const f64 AngleY = core::PI / polyCountY;

	u32 i=0;
	f64 axz;

	// we don't start at 0.

	f64 ay = 0;//AngleY / 2;

	buffer->Vertices.set_used((polyCountXPitch * polyCountY) + 2);
	for (u32 y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const f64 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			const core::vector3df pos(static_cast<f32>(radius * cos(axz) * sinay),
						static_cast<f32>(radius * cos(ay)),
						static_cast<f32>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = static_cast<f32>(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = buffer->Vertices[i-polyCountXPitch].TCoords.X;
			buffer->Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						static_cast<f32>(ay*core::RECIPROCAL_PI64));
			++i;
			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		buffer->Vertices[i] = video::S3DVertex(buffer->Vertices[i-polyCountX]);
		buffer->Vertices[i].TCoords.X=1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	buffer->Vertices[i] = video::S3DVertex(0.0f,radius,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	buffer->Vertices[i] = video::S3DVertex(0.0f,-radius,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

	// recalculate bounding box

	buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
	buffer->BoundingBox.addInternalPoint(buffer->Vertices[i-1].Pos);
	buffer->BoundingBox.addInternalPoint(radius,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(-radius,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,radius);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,-radius);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);


	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	return mesh;
}


/* A cylinder with proper normals and texture coords */
IMesh* CGeometryCreator::createCylinderMesh(f32 radius, f32 length,
			u32 tesselation, const video::SColor& color,
			bool closeTop, f32 oblique, u32 normalType) const
{
	SMeshBuffer* buffer = new SMeshBuffer();

	const f32 recTesselation = core::reciprocal((f32)tesselation);
	const f32 recTesselationHalf = recTesselation * 0.5f;
	const f32 angleStep = (core::PI * 2.f ) * recTesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	u32 i;
	video::S3DVertex v;
	v.Color = color;
	buffer->Vertices.reallocate(tesselation*4+4+(closeTop?2:1));
	buffer->Indices.reallocate((tesselation*2+1)*(closeTop?12:9));
	f32 tcx = 0.f;
	for ( i = 0; i <= tesselation; ++i )
	{
		const f32 angle = angleStep * i;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		switch (normalType)
		{
			case 0: v.Normal = v.Pos; break;
			case 1: v.Normal = v.Pos; break;
		}
		v.Normal.normalize();
		v.TCoords.X=tcx;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		switch (normalType)
		{
			case 0: v.Normal = v.Pos; break;
			case 1: v.Normal = core::vector3df(v.Pos.X-oblique, 0, v.Pos.Z); break;
		}
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);

		v.Pos.X = radius * cosf(angle + angleStepHalf);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle + angleStepHalf);
		switch (normalType)
		{
			case 0: v.Normal = v.Pos; break;
			case 1: v.Normal = v.Pos; break;
		}
		v.Normal.normalize();
		v.TCoords.X=tcx+recTesselationHalf;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		switch (normalType)
		{
			case 0: v.Normal = v.Pos; break;
			case 1: v.Normal = core::vector3df(v.Pos.X-oblique, 0, v.Pos.Z); break;
		}
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);
		tcx += recTesselation;
	}

	// indices for the main hull part
	const u32 nonWrappedSize = tesselation* 4;
	for (i=0; i != nonWrappedSize; i += 2)
	{
		buffer->Indices.push_back(i + 2);
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(i + 1);

		buffer->Indices.push_back(i + 2);
		buffer->Indices.push_back(i + 1);
		buffer->Indices.push_back(i + 3);
	}

	// two closing quads between end and start
	buffer->Indices.push_back(0);
	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(i + 1);

	buffer->Indices.push_back(0);
	buffer->Indices.push_back(i + 1);
	buffer->Indices.push_back(1);

	// close down
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	v.TCoords.X = 1.f;
	v.TCoords.Y = 1.f;
	buffer->Vertices.push_back(v);

	u32 index = buffer->Vertices.size() - 1;

	for ( i = 0; i != nonWrappedSize; i += 2 )
	{
		buffer->Indices.push_back(index);
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(i + 2);
	}

	buffer->Indices.push_back(index);
	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(0);

	if (closeTop)
	{
		// close top
		v.Pos.X = oblique;
		v.Pos.Y = length;
		v.Pos.Z = 0.f;
		v.Normal.X = 0.f;
		v.Normal.Y = 1.f;
		v.Normal.Z = 0.f;
		v.TCoords.X = 0.f;
		v.TCoords.Y = 0.f;
		buffer->Vertices.push_back(v);

		index = buffer->Vertices.size() - 1;

		for ( i = 0; i != nonWrappedSize; i += 2 )
		{
			buffer->Indices.push_back(i + 1);
			buffer->Indices.push_back(index);
			buffer->Indices.push_back(i + 3);
		}

		buffer->Indices.push_back(i + 1);
		buffer->Indices.push_back(index);
		buffer->Indices.push_back(1);
	}

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();

	return mesh;
}


/* A cone with proper normals and texture coords */
IMesh* CGeometryCreator::createConeMesh(f32 radius, f32 length, u32 tesselation,
					const video::SColor& colorTop,
					const video::SColor& colorBottom,
					f32 oblique) const
{
	SMeshBuffer* buffer = new SMeshBuffer();

	const f32 angleStep = (core::PI * 2.f ) / tesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	video::S3DVertex v;
	u32 i;

	v.Color = colorTop;
	for ( i = 0; i != tesselation; ++i )
	{
		f32 angle = angleStep * f32(i);

		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		buffer->Vertices.push_back(v);

		angle += angleStepHalf;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		buffer->Vertices.push_back(v);
	}
	const u32 nonWrappedSize = buffer->Vertices.size() - 1;

	// close top
	v.Pos.X = oblique;
	v.Pos.Y = length;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = 1.f;
	v.Normal.Z = 0.f;
	buffer->Vertices.push_back(v);

	u32 index = buffer->Vertices.size() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(index);
		buffer->Indices.push_back(i + 1);
	}

	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(index);
	buffer->Indices.push_back(0);

	// close down
	v.Color = colorBottom;
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	buffer->Vertices.push_back(v);

	index = buffer->Vertices.size() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		buffer->Indices.push_back(index);
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(i + 1);
	}

	buffer->Indices.push_back(index);
	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(0);

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);


	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	return mesh;
}


void CGeometryCreator::addToBuffer(const video::S3DVertex& v, SMeshBuffer* Buffer) const
{
	const s32 tnidx = Buffer->Vertices.linear_reverse_search(v);
	const bool alreadyIn = (tnidx != -1);
	u16 nidx = (u16)tnidx;
	if (!alreadyIn)
	{
		nidx = (u16)Buffer->Vertices.size();
		Buffer->Indices.push_back(nidx);
		Buffer->Vertices.push_back(v);
	}
	else
		Buffer->Indices.push_back(nidx);
}


IMesh* CGeometryCreator::createVolumeLightMesh(
		const u32 subdivideU, const u32 subdivideV,
		const video::SColor footColor, const video::SColor tailColor,
		const f32 lpDistance, const core::vector3df& lightDim) const
{
	SMeshBuffer* Buffer = new SMeshBuffer();
	Buffer->setHardwareMappingHint(EHM_STATIC);

	const core::vector3df lightPoint(0, -(lpDistance*lightDim.Y), 0);
	const f32 ax = lightDim.X * 0.5f; // X Axis
	const f32 az = lightDim.Z * 0.5f; // Z Axis

	Buffer->Vertices.clear();
	Buffer->Vertices.reallocate(6+12*(subdivideU+subdivideV));
	Buffer->Indices.clear();
	Buffer->Indices.reallocate(6+12*(subdivideU+subdivideV));
	//draw the bottom foot.. the glowing region
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, footColor, 0, 1),Buffer);
	addToBuffer(video::S3DVertex( ax, 0, az,  0,0,0, footColor, 1, 1),Buffer);
	addToBuffer(video::S3DVertex( ax, 0,-az,  0,0,0, footColor, 1, 0),Buffer);

	addToBuffer(video::S3DVertex( ax, 0,-az,  0,0,0, footColor, 1, 0),Buffer);
	addToBuffer(video::S3DVertex(-ax, 0,-az,  0,0,0, footColor, 0, 0),Buffer);
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, footColor, 0, 1),Buffer);

	f32 tu = 0.f;
	const f32 tuStep = 1.f/subdivideU;
	f32 bx = -ax;
	const f32 bxStep = lightDim.X * tuStep;
	// Slices in X/U space
	for (u32 i = 0; i <= subdivideU; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(bx, 0.0f, -az);
		core::vector3df end2(bx, 0.0f, az);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDim.Y;	// multiply it out by shootlength

		end1.X += bx;			// Add the original point location to the vector
		end1.Z -= az;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDim.Y;

		end2.X += bx;
		end2.Z += az;

		addToBuffer(video::S3DVertex(bx , 0,  az,  0,0,0, footColor, tu, 1),Buffer);
		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, footColor, tu, 0),Buffer);
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, tailColor, tu, 1),Buffer);

		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, footColor, tu, 0),Buffer);
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, tailColor, tu, 0),Buffer);
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, tailColor, tu, 1),Buffer);

		//back side
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, tailColor, tu, 1),Buffer);
		addToBuffer(video::S3DVertex(-bx , 0,  -az,  0,0,0, footColor, tu, 1),Buffer);
		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, footColor, tu, 0),Buffer);

		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, footColor, tu, 0),Buffer);
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, tailColor, tu, 0),Buffer);
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, tailColor, tu, 1),Buffer);
		tu += tuStep;
		bx += bxStep;
	}

	f32 tv = 0.f;
	const f32 tvStep = 1.f/subdivideV;
	f32 bz = -az;
	const f32 bzStep = lightDim.Z * tvStep;
	// Slices in Z/V space
	for(u32 i = 0; i <= subdivideV; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(-ax, 0.0f, bz);
		core::vector3df end2(ax, 0.0f, bz);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDim.Y;	// multiply it out by shootlength

		end1.X -= ax;			// Add the original point location to the vector
		end1.Z += bz;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDim.Y;

		end2.X += ax;
		end2.Z += bz;

		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, footColor, 0, tv),Buffer);
		addToBuffer(video::S3DVertex(ax , 0,  bz,  0,0,0, footColor, 1, tv),Buffer);
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, tailColor, 1, tv),Buffer);

		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, tailColor, 1, tv),Buffer);
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, tailColor, 0, tv),Buffer);
		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, footColor, 0, tv),Buffer);

		//back side
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, footColor, 0, tv),Buffer);
		addToBuffer(video::S3DVertex(-ax , 0,  -bz,  0,0,0, footColor, 1, tv),Buffer);
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, tailColor, 1, tv),Buffer);

		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, tailColor, 1, tv),Buffer);
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, tailColor, 0, tv),Buffer);
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, footColor, 0, tv),Buffer);
		tv += tvStep;
		bz += bzStep;
	}

	Buffer->recalculateBoundingBox();

	Buffer->Material.MaterialType = video::EMT_ONETEXTURE_BLEND;
	Buffer->Material.MaterialTypeParam = pack_textureBlendFunc( video::EBF_SRC_COLOR, video::EBF_SRC_ALPHA, video::EMFN_MODULATE_1X );

	Buffer->Material.Lighting = false;
	Buffer->Material.ZWriteEnable = video::EZW_OFF;

	Buffer->setDirty(EBT_VERTEX_AND_INDEX);

	Buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(Buffer);


	mesh->recalculateBoundingBox();
	return mesh;
}

IAnimatedMesh* CGeometryCreator::importMesh(const char* filename) const
{
   IAnimatedMesh* mesh=nullptr;

   if (!core::FileExists(filename))
   {
     mesh= new SAnimatedMesh(createCubeMesh( ECMT_1BUF_12VTX_NA));
     SDL_LogError(0," File %s don't exists ", filename);
     return mesh;
   }

  io::IReadFile* file = scene::Engine::Instance()->getFileSystem()->createAndOpenFile(filename);

  if(!file)
  {
    return mesh;
  }



  if (core::hasFileExtension ( filename, "3ds" ))
  {
    C3DSMeshFileLoader*  MeshFileLoader= new C3DSMeshFileLoader();
    mesh=MeshFileLoader->createMesh(file);
    delete MeshFileLoader;
  } else
  if (core::hasFileExtension ( filename, "obj" ))
  {
    COBJMeshFileLoader*  MeshFileLoader= new COBJMeshFileLoader();
    mesh=MeshFileLoader->createMesh(file);
    delete MeshFileLoader;

  } else
  if (core::hasFileExtension ( filename, "md2" ))
  {
    CMD2MeshFileLoader*  MeshFileLoader= new CMD2MeshFileLoader();
    mesh=MeshFileLoader->createMesh(file);
    delete MeshFileLoader;

  } else
  if (core::hasFileExtension ( filename, "md3" ))
  {
    CMD3MeshFileLoader*  MeshFileLoader= new CMD3MeshFileLoader();
    mesh = MeshFileLoader->createMesh(file);
    delete MeshFileLoader;
  } else
  if (core::hasFileExtension ( filename, "ms3d" ))
  {
    CMS3DMeshFileLoader*  MeshFileLoader= new CMS3DMeshFileLoader();
    mesh = MeshFileLoader->createMesh(file);
    delete MeshFileLoader;
  } else



  {
    os::Printer::log("Can't import file", ELL_ERROR);
    mesh= new SAnimatedMesh(createCubeMesh(ECMT_1BUF_12VTX_NA));
  }


  file->drop();


  return mesh;

}
//***************************************************************************3DS*********************************************************************

namespace
{
enum e3DSChunk
{
	// Primary chunk
	 C3DS_MAIN3DS = 0x4D4D,

	// Main Chunks
	 C3DS_EDIT3DS = 0x3D3D,
	 C3DS_KEYF3DS = 0xB000,
	 C3DS_VERSION = 0x0002,
	 C3DS_MESHVERSION = 0x3D3E,

	// sub chunks of C3DS_EDIT3DS
	 C3DS_EDIT_MATERIAL = 0xAFFF,
	 C3DS_EDIT_OBJECT   = 0x4000,

	// sub chunks of C3DS_EDIT_MATERIAL
	 C3DS_MATNAME       = 0xA000,
	 C3DS_MATAMBIENT    = 0xA010,
	 C3DS_MATDIFFUSE    = 0xA020,
	 C3DS_MATSPECULAR   = 0xA030,
	 C3DS_MATSHININESS  = 0xA040,
	 C3DS_MATSHIN2PCT   = 0xA041,
	 C3DS_TRANSPARENCY  = 0xA050,
	 C3DS_TRANSPARENCY_FALLOFF  = 0xA052,
	 C3DS_REFL_BLUR     = 0xA053,
	 C3DS_TWO_SIDE      = 0xA081,
	 C3DS_WIRE          = 0xA085,
	 C3DS_SHADING       = 0xA100,
	 C3DS_MATTEXMAP     = 0xA200,
	 C3DS_MATSPECMAP    = 0xA204,
	 C3DS_MATOPACMAP    = 0xA210,
	 C3DS_MATREFLMAP    = 0xA220,
	 C3DS_MATBUMPMAP    = 0xA230,
	 C3DS_MATMAPFILE    = 0xA300,
	 C3DS_MAT_TEXTILING = 0xA351,
	 C3DS_MAT_USCALE    = 0xA354,
	 C3DS_MAT_VSCALE    = 0xA356,
	 C3DS_MAT_UOFFSET   = 0xA358,
	 C3DS_MAT_VOFFSET   = 0xA35A,

	// subs of C3DS_EDIT_OBJECT
	 C3DS_OBJTRIMESH    = 0x4100,

	// subs of C3DS_OBJTRIMESH
	 C3DS_TRIVERT       = 0x4110,
	 C3DS_POINTFLAGARRAY= 0x4111,
	 C3DS_TRIFACE       = 0x4120,
	 C3DS_TRIFACEMAT    = 0x4130,
	 C3DS_TRIUV         = 0x4140,
	 C3DS_TRISMOOTH     = 0x4150,
	 C3DS_TRIMATRIX     = 0x4160,
	 C3DS_MESHCOLOR     = 0x4165,
	 C3DS_DIRECT_LIGHT  = 0x4600,
	 C3DS_DL_INNER_RANGE= 0x4659,
	 C3DS_DL_OUTER_RANGE= 0x465A,
	 C3DS_DL_MULTIPLIER = 0x465B,
	 C3DS_CAMERA        = 0x4700,
	 C3DS_CAM_SEE_CONE  = 0x4710,
	 C3DS_CAM_RANGES    = 0x4720,

	// subs of C3DS_KEYF3DS
	 C3DS_KF_HDR        = 0xB00A,
	 C3DS_AMBIENT_TAG   = 0xB001,
	 C3DS_OBJECT_TAG    = 0xB002,
	 C3DS_CAMERA_TAG    = 0xB003,
	 C3DS_TARGET_TAG    = 0xB004,
	 C3DS_LIGHTNODE_TAG = 0xB005,
	 C3DS_KF_SEG        = 0xB008,
	 C3DS_KF_CURTIME    = 0xB009,
	 C3DS_KF_NODE_HDR   = 0xB010,
	 C3DS_PIVOTPOINT    = 0xB013,
	 C3DS_BOUNDBOX      = 0xB014,
	 C3DS_MORPH_SMOOTH  = 0xB015,
	 C3DS_POS_TRACK_TAG = 0xB020,
	 C3DS_ROT_TRACK_TAG = 0xB021,
	 C3DS_SCL_TRACK_TAG = 0xB022,
	 C3DS_NODE_ID       = 0xB030,

	// Viewport definitions
	 C3DS_VIEWPORT_LAYOUT = 0x7001,
	 C3DS_VIEWPORT_DATA   = 0x7011,
	 C3DS_VIEWPORT_DATA_3 = 0x7012,
	 C3DS_VIEWPORT_SIZE   = 0x7020,

	// different color chunk types
	 C3DS_COL_RGB    = 0x0010,
	 C3DS_COL_TRU    = 0x0011,
	 C3DS_COL_LIN_24 = 0x0012,
	 C3DS_COL_LIN_F  = 0x0013,

	// percentage chunk types
	 C3DS_PERCENTAGE_I = 0x0030,
	 C3DS_PERCENTAGE_F = 0x0031,

	 C3DS_CHUNK_MAX		= 0xFFFF
};
}


//! Constructor
C3DSMeshFileLoader::C3DSMeshFileLoader()
:  Vertices(0), Indices(0), SmoothingGroups(0), TCoords(0),
	CountVertices(0), CountFaces(0), CountTCoords(0), Mesh(0)
{




	FileSystem=scene::Engine::Instance()->getFileSystem();



}


//! destructor
C3DSMeshFileLoader::~C3DSMeshFileLoader()
{
	cleanUp();


//	if (Mesh)
	//	Mesh->drop();
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* C3DSMeshFileLoader::createMesh(io::IReadFile* file)
{

	ChunkData data;

	readChunkData(file, data);

	if (data.header.id != C3DS_MAIN3DS )
		return 0;

	CurrentMaterial.clear();
	Materials.clear();
	MeshBufferNames.clear();
	cleanUp();

	if (Mesh)
		Mesh->drop();

	Mesh = new SMesh();

	if (readChunk(file, &data))
	{
		// success

		for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			SMeshBuffer* mb = ((SMeshBuffer*)Mesh->getMeshBuffer(i));
			// drop empty buffers
			if (mb->getIndexCount() == 0 || mb->getVertexCount() == 0)
			{
				Mesh->MeshBuffers.erase(i--);
				//mb->drop();
			}
			else
			{
				if (mb->Material.MaterialType == video::EMT_PARALLAX_MAP_SOLID)
				{
					SMesh tmp;
					tmp.addMeshBuffer(mb);
					//mb->drop();



					IMesh* tangentMesh = scene::Engine::Instance()->getMeshManipulator()->createMeshWithTangents(&tmp);
					Mesh->MeshBuffers[i]=tangentMesh->getMeshBuffer(0);
					// we need to grab because we replace the buffer manually.
					//Mesh->MeshBuffers[i]->grab();
					// clean up intermediate mesh struct
					//tangentMesh->drop();
				}
				Mesh->MeshBuffers[i]->recalculateBoundingBox();
			}
		}

		Mesh->recalculateBoundingBox();

		SAnimatedMesh* am = new SAnimatedMesh();
		am->Type = EAMT_3DS;
		am->addMesh(Mesh);
		am->recalculateBoundingBox();
		//Mesh->drop();
		//Mesh = 0;
		return am;
	}

	Mesh->drop();
	Mesh = 0;

	return 0;
}


bool C3DSMeshFileLoader::readPercentageChunk(io::IReadFile* file,
					ChunkData* chunk, f32& percentage)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load percentage chunk.", ELL_DEBUG);
#endif

	ChunkData data;
	readChunkData(file, data);

	short intpercentage;
	float fpercentage;

	switch(data.header.id)
	{
	case C3DS_PERCENTAGE_I:
	{
		// read short
		file->read(&intpercentage, 2);
#ifdef __BIG_ENDIAN__
		intpercentage = os::Byteswap::byteswap(intpercentage);
#endif
		percentage=intpercentage/100.0f;
		data.read += 2;
	}
	break;
	case C3DS_PERCENTAGE_F:
	{
		// read float
		file->read(&fpercentage, sizeof(float));
		data.read += sizeof(float);
#ifdef __BIG_ENDIAN__
		percentage = os::Byteswap::byteswap(fpercentage);
#else
		percentage = (f32)fpercentage;
#endif
	}
	break;
	default:
	{
		// unknown percentage chunk
		os::Printer::log("Unknown percentage chunk in 3Ds file.", ELL_WARNING);
		file->seek(data.header.length - data.read, true);
		data.read += data.header.length - data.read;
	}
	}

	chunk->read += data.read;

	return true;
}

bool C3DSMeshFileLoader::readColorChunk(io::IReadFile* file, ChunkData* chunk,
					video::SColor& out)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load color chunk.", ELL_DEBUG);
#endif
	ChunkData data;
	readChunkData(file, data);

	u8 c[3];
	f32 cf[3];

	switch(data.header.id)
	{
	case C3DS_COL_TRU:
	case C3DS_COL_LIN_24:
	{
		// read 8 bit data
		file->read(c, sizeof(c));
		out.set(255, c[0], c[1], c[2]);
		data.read += sizeof(c);
	}
	break;
	case C3DS_COL_RGB:
	case C3DS_COL_LIN_F:
	{
		// read float data
		file->read(cf, sizeof(cf));
#ifdef __BIG_ENDIAN__
		cf[0] = os::Byteswap::byteswap(cf[0]);
		cf[1] = os::Byteswap::byteswap(cf[1]);
		cf[2] = os::Byteswap::byteswap(cf[2]);
#endif
		out.set(255, (s32)(cf[0]*255.0f), (s32)(cf[1]*255.0f), (s32)(cf[2]*255.0f));
		data.read += sizeof(cf);
	}
	break;
	default:
	{
		// unknown color chunk size
		os::Printer::log("Unknown size of color chunk in 3Ds file.", ELL_WARNING);
		file->seek(data.header.length - data.read, true);
		data.read += data.header.length - data.read;
	}
	}

	chunk->read += data.read;

	return true;
}


bool C3DSMeshFileLoader::readMaterialChunk(io::IReadFile* file, ChunkData* parent)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load material chunk.", ELL_DEBUG);
#endif
	u16 matSection=0;

	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_MATNAME:
			{
				c8* c = new c8[data.header.length - data.read];
				file->read(c, data.header.length - data.read);

				if (strlen(c))
					CurrentMaterial.Name = c;

				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_MATAMBIENT:
			readColorChunk(file, &data, CurrentMaterial.Material.AmbientColor);
			break;
		case C3DS_MATDIFFUSE:
			readColorChunk(file, &data, CurrentMaterial.Material.DiffuseColor);
			break;
		case C3DS_MATSPECULAR:
			readColorChunk(file, &data, CurrentMaterial.Material.SpecularColor);
			break;
		case C3DS_MATSHININESS:
			readPercentageChunk(file, &data, CurrentMaterial.Material.Shininess);
			CurrentMaterial.Material.Shininess = (1.f-CurrentMaterial.Material.Shininess)*128.f;
			break;
		case C3DS_TRANSPARENCY:
			{
				f32 percentage;
				readPercentageChunk(file, &data, percentage);
				if (percentage>0.0f)
				{
					CurrentMaterial.Material.MaterialTypeParam=percentage;
					CurrentMaterial.Material.MaterialType=video::EMT_TRANSPARENT_VERTEX_ALPHA;
				}
				else
				{
					CurrentMaterial.Material.MaterialType=video::EMT_SOLID;
				}
			}
			break;
		case C3DS_WIRE:
			CurrentMaterial.Material.Wireframe=true;
			break;
		case C3DS_TWO_SIDE:
			CurrentMaterial.Material.BackfaceCulling=false;
			break;
		case C3DS_SHADING:
			{
				s16 flags;
				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				switch (flags)
				{
					case 0:
						CurrentMaterial.Material.Wireframe=true;
						break;
					case 1:
						CurrentMaterial.Material.Wireframe=false;
						CurrentMaterial.Material.GouraudShading=false;
						break;
					case 2:
						CurrentMaterial.Material.Wireframe=false;
						CurrentMaterial.Material.GouraudShading=true;
						break;
					default:
						// phong and metal missing
						break;
				}
				data.read += data.header.length - data.read;
			}
			break;
		case C3DS_MATTEXMAP:
		case C3DS_MATSPECMAP:
		case C3DS_MATOPACMAP:
		case C3DS_MATREFLMAP:
		case C3DS_MATBUMPMAP:
			{
				matSection=data.header.id;
				// Should contain a percentage chunk, but does
				// not always have it
				s16 testval;
				const long pos = file->getPos();
				file->read(&testval, 2);
#ifdef __BIG_ENDIAN__
				testval = os::Byteswap::byteswap(testval);
#endif
				file->seek(pos, false);
				if ((testval == C3DS_PERCENTAGE_I) ||
					(testval == C3DS_PERCENTAGE_F))
				switch (matSection)
				{
				case C3DS_MATTEXMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[0]);
				break;
				case C3DS_MATSPECMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[1]);
				break;
				case C3DS_MATOPACMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[2]);
				break;
				case C3DS_MATBUMPMAP:
					readPercentageChunk(file, &data, CurrentMaterial.Strength[4]);
				break;
				}
			}
			break;
		case C3DS_MATMAPFILE:
			{
				// read texture file name
				c8* c = new c8[data.header.length - data.read];
				file->read(c, data.header.length - data.read);
				switch (matSection)
				{
				case C3DS_MATTEXMAP:
					CurrentMaterial.Filename[0] = c;
				break;
				case C3DS_MATSPECMAP:
					CurrentMaterial.Filename[1] = c;
				break;
				case C3DS_MATOPACMAP:
					CurrentMaterial.Filename[2] = c;
				break;
				case C3DS_MATREFLMAP:
					CurrentMaterial.Filename[3] = c;
				break;
				case C3DS_MATBUMPMAP:
					CurrentMaterial.Filename[4] = c;
				break;
				}
				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_MAT_TEXTILING:
			{
				s16 flags;
				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				data.read += 2;
			}
			break;
		case C3DS_MAT_USCALE:
		case C3DS_MAT_VSCALE:
		case C3DS_MAT_UOFFSET:
		case C3DS_MAT_VOFFSET:
			{
				f32 value;
				file->read(&value, 4);
#ifdef __BIG_ENDIAN__
				value = os::Byteswap::byteswap(value);
#endif
				u32 i=0;
				if (matSection != C3DS_MATTEXMAP)
					i=1;
				u32 j=0,k=0;
				if (data.header.id == C3DS_MAT_VSCALE)
				{
					j=1;
					k=1;
				}
				else if (data.header.id == C3DS_MAT_UOFFSET)
				{
					j=2;
					k=0;
				}
				else if (data.header.id == C3DS_MAT_VOFFSET)
				{
					j=2;
					k=1;
				}
				CurrentMaterial.Material.getTextureMatrix(i)(j,k)=value;

				data.read += 4;
			}
			break;
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	Materials.push_back(CurrentMaterial);
	CurrentMaterial.clear();

	return true;
}



bool C3DSMeshFileLoader::readTrackChunk(io::IReadFile* file, ChunkData& data,
					IMeshBuffer* mb, const core::vector3df& pivot)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load track chunk.", ELL_DEBUG);
#endif
	u16 flags;
	u32 flags2;
	// Track flags
	file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
	flags = os::Byteswap::byteswap(flags);
#endif
	file->read(&flags2, 4);
#ifdef __BIG_ENDIAN__
	flags2 = os::Byteswap::byteswap(flags2);
#endif
	file->read(&flags2, 4);
#ifdef __BIG_ENDIAN__
	flags2 = os::Byteswap::byteswap(flags2);
#endif
	// Num keys
	file->read(&flags2, 4);
#ifdef __BIG_ENDIAN__
	flags2 = os::Byteswap::byteswap(flags2);
#endif
	file->read(&flags2, 4);
#ifdef __BIG_ENDIAN__
	flags2 = os::Byteswap::byteswap(flags2);
#endif
	// TCB flags
	file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
	flags = os::Byteswap::byteswap(flags);
#endif
	data.read += 20;

	f32 angle=0.0f;
	if (data.header.id== C3DS_ROT_TRACK_TAG)
	{
		// Angle
		file->read(&angle, sizeof(f32));
#ifdef __BIG_ENDIAN__
		angle = os::Byteswap::byteswap(angle);
#endif
		data.read += sizeof(f32);
	}
	core::vector3df vec;
	file->read(&vec.X, sizeof(f32));
	file->read(&vec.Y, sizeof(f32));
	file->read(&vec.Z, sizeof(f32));
#ifdef __BIG_ENDIAN__
	vec.X = os::Byteswap::byteswap(vec.X);
	vec.Y = os::Byteswap::byteswap(vec.Y);
	vec.Z = os::Byteswap::byteswap(vec.Z);
#endif
	data.read += 12;
	vec-=pivot;

	// apply transformation to mesh buffer
	if (false)//mb)
	{
		video::S3DVertex *vertices=(video::S3DVertex*)mb->getVertices();
		if (data.header.id==C3DS_POS_TRACK_TAG)
		{
			for (u32 i=0; i<mb->getVertexCount(); ++i)
				vertices[i].Pos+=vec;
		}
		else if (data.header.id==C3DS_ROT_TRACK_TAG)
		{
			//TODO
		}
		else if (data.header.id==C3DS_SCL_TRACK_TAG)
		{
			//TODO
		}
	}
	// skip further frames
	file->seek(data.header.length - data.read, true);
	data.read += data.header.length - data.read;
	return true;
}


bool C3DSMeshFileLoader::readFrameChunk(io::IReadFile* file, ChunkData* parent)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load frame chunk.", ELL_DEBUG);
#endif
	ChunkData data;

	//KF_HDR is always at the beginning
	readChunkData(file, data);
	if (data.header.id != C3DS_KF_HDR)
		return false;
	else
	{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load keyframe header.", ELL_DEBUG);
#endif
		u16 version;
		file->read(&version, 2);
#ifdef __BIG_ENDIAN__
		version = os::Byteswap::byteswap(version);
#endif
		core::stringc name;
		readString(file, data, name);
		u32 flags;
		file->read(&flags, 4);
#ifdef __BIG_ENDIAN__
		flags = os::Byteswap::byteswap(flags);
#endif

		data.read += 4;
		parent->read += data.read;
	}
	data.read=0;

	IMeshBuffer* mb=0;
	core::vector3df pivot,bboxCenter;
	while(parent->read < parent->header.length)
	{
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_OBJECT_TAG:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load object tag.", ELL_DEBUG);
#endif
				mb=0;
				pivot.set(0.0f, 0.0f, 0.0f);
			}
			break;
		case C3DS_KF_SEG:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load keyframe segment.", ELL_DEBUG);
#endif
				u32 flags;
				file->read(&flags, 4);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				file->read(&flags, 4);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				data.read += 8;
			}
			break;
		case C3DS_KF_NODE_HDR:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load keyframe node header.", ELL_DEBUG);
#endif
				s16 flags;
				c8* c = new c8[data.header.length - data.read-6];
				file->read(c, data.header.length - data.read-6);

				// search mesh buffer to apply these transformations to
				for (u32 i=0; i<MeshBufferNames.size(); ++i)
				{
					if (MeshBufferNames[i]==c)
					{
						mb=Mesh->getMeshBuffer(i);
						break;
					}
				}

				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				data.read += data.header.length - data.read;
				delete [] c;
			}
			break;
		case C3DS_KF_CURTIME:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load keyframe current time.", ELL_DEBUG);
#endif
				u32 flags;
				file->read(&flags, 4);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				data.read += 4;
			}
			break;
		case C3DS_NODE_ID:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load node ID.", ELL_DEBUG);
#endif
				u16 flags;
				file->read(&flags, 2);
#ifdef __BIG_ENDIAN__
				flags = os::Byteswap::byteswap(flags);
#endif
				data.read += 2;
			}
			break;
		case C3DS_PIVOTPOINT:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load pivot point.", ELL_DEBUG);
#endif
				file->read(&pivot.X, sizeof(f32));
				file->read(&pivot.Y, sizeof(f32));
				file->read(&pivot.Z, sizeof(f32));
#ifdef __BIG_ENDIAN__
				pivot.X = os::Byteswap::byteswap(pivot.X);
				pivot.Y = os::Byteswap::byteswap(pivot.Y);
				pivot.Z = os::Byteswap::byteswap(pivot.Z);
#endif
				data.read += 12;
			}
			break;
		case C3DS_BOUNDBOX:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load bounding box.", ELL_DEBUG);
#endif
				core::aabbox3df bbox;
				// abuse bboxCenter as temporary variable
				file->read(&bboxCenter.X, sizeof(f32));
				file->read(&bboxCenter.Y, sizeof(f32));
				file->read(&bboxCenter.Z, sizeof(f32));
#ifdef __BIG_ENDIAN__
				bboxCenter.X = os::Byteswap::byteswap(bboxCenter.X);
				bboxCenter.Y = os::Byteswap::byteswap(bboxCenter.Y);
				bboxCenter.Z = os::Byteswap::byteswap(bboxCenter.Z);
#endif
				bbox.reset(bboxCenter);
				file->read(&bboxCenter.X, sizeof(f32));
				file->read(&bboxCenter.Y, sizeof(f32));
				file->read(&bboxCenter.Z, sizeof(f32));
#ifdef __BIG_ENDIAN__
				bboxCenter.X = os::Byteswap::byteswap(bboxCenter.X);
				bboxCenter.Y = os::Byteswap::byteswap(bboxCenter.Y);
				bboxCenter.Z = os::Byteswap::byteswap(bboxCenter.Z);
#endif
				bbox.addInternalPoint(bboxCenter);
				bboxCenter=bbox.getCenter();
				data.read += 24;
			}
			break;
		case C3DS_MORPH_SMOOTH:
			{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load morph smooth.", ELL_DEBUG);
#endif
				f32 flag;
				file->read(&flag, 4);
#ifdef __BIG_ENDIAN__
				flag = os::Byteswap::byteswap(flag);
#endif
				data.read += 4;
			}
			break;
		case C3DS_POS_TRACK_TAG:
		case C3DS_ROT_TRACK_TAG:
		case C3DS_SCL_TRACK_TAG:
			readTrackChunk(file, data, mb, bboxCenter-pivot);
			break;
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
		data.read=0;
	}

	return true;
}


bool C3DSMeshFileLoader::readChunk(io::IReadFile* file, ChunkData* parent)
{
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_VERSION:
			{
				u16 version;
				file->read(&version, sizeof(u16));
#ifdef __BIG_ENDIAN__
				version = os::Byteswap::byteswap(version);
#endif
				file->seek(data.header.length - data.read - 2, true);
				data.read += data.header.length - data.read;
				if (version != 0x03)
					os::Printer::log("3ds file version is other than 3.", ELL_ERROR);
			}
			break;
		case C3DS_EDIT_MATERIAL:
			readMaterialChunk(file, &data);
			break;
		case C3DS_KEYF3DS:
			readFrameChunk(file, &data);
			break;
		case C3DS_EDIT3DS:
			break;
		case C3DS_MESHVERSION:
		case 0x01:
			{
				u32 version;
				file->read(&version, sizeof(u32));
#ifdef __BIG_ENDIAN__
				version = os::Byteswap::byteswap(version);
#endif
				data.read += sizeof(u32);
			}
			break;
		case C3DS_EDIT_OBJECT:
			{
				core::stringc name;
				readString(file, data, name);
				readObjectChunk(file, &data);
				composeObject(file, name);
			}
			break;

		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	return true;
}


bool C3DSMeshFileLoader::readObjectChunk(io::IReadFile* file, ChunkData* parent)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load object chunk.", ELL_DEBUG);
#endif
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_OBJTRIMESH:
			readObjectChunk(file, &data);
			break;

		case C3DS_TRIVERT:
			readVertices(file, data);
			break;

		case C3DS_POINTFLAGARRAY:
			{
				u16 numVertex, flags;
				file->read(&numVertex, sizeof(u16));
#ifdef __BIG_ENDIAN__
				numVertex= os::Byteswap::byteswap(numVertex);
#endif
				for (u16 i=0; i<numVertex; ++i)
				{
					file->read(&flags, sizeof(u16));
#ifdef __BIG_ENDIAN__
					flags = os::Byteswap::byteswap(flags);
#endif
				}
				data.read += (numVertex+1)*sizeof(u16);
			}
			break;

		case C3DS_TRIFACE:
			readIndices(file, data);
			readObjectChunk(file, &data); // read smooth and material groups
			break;

		case C3DS_TRIFACEMAT:
			readMaterialGroup(file, data);
			break;

		case C3DS_TRIUV: // getting texture coordinates
			readTextureCoords(file, data);
			break;

		case C3DS_TRIMATRIX:
			{
				f32 mat[4][3];
				file->read(&mat, 12*sizeof(f32));
				TransformationMatrix.makeIdentity();
				for (int i=0; i<4; ++i)
				{
					for (int j=0; j<3; ++j)
					{
#ifdef __BIG_ENDIAN__
						TransformationMatrix(i,j)=os::Byteswap::byteswap(mat[i][j]);
#else
						TransformationMatrix(i,j)=mat[i][j];
#endif
					}
				}
				data.read += 12*sizeof(f32);
			}
			break;
		case C3DS_MESHCOLOR:
			{
				u8 flag;
				file->read(&flag, sizeof(u8));
				++data.read;
			}
			break;
		case C3DS_TRISMOOTH: // TODO
			{
				SmoothingGroups = new u32[CountFaces];
				file->read(SmoothingGroups, CountFaces*sizeof(u32));
#ifdef __BIG_ENDIAN__
				for (u16 i=0; i<CountFaces; ++i)
					SmoothingGroups[i] = os::Byteswap::byteswap(SmoothingGroups[i]);
#endif
				data.read += CountFaces*sizeof(u32);
			}
			break;

		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	return true;
}


void C3DSMeshFileLoader::composeObject(io::IReadFile* file, const core::stringc& name)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Compose object.", ELL_DEBUG);
#endif
	if (Mesh->getMeshBufferCount() != Materials.size())
		loadMaterials(file);

	if (MaterialGroups.empty())
	{
		// no material group, so add all
		SMaterialGroup group;
		group.faceCount = CountFaces;
		group.faces = new u16[group.faceCount];
		for (u16 i=0; i<group.faceCount; ++i)
			group.faces[i] = i;
		MaterialGroups.push_back(group);

		// if we've got no material, add one without a texture
		if (Materials.empty())
		{
			SCurrentMaterial m;
			Materials.push_back(m);
			SMeshBuffer* mb = new scene::SMeshBuffer();
			Mesh->addMeshBuffer(mb);
			mb->getMaterial() = Materials[0].Material;
			//mb->drop();
			// add an empty mesh buffer name
			MeshBufferNames.push_back("");
		}
	}

	for (u32 i=0; i<MaterialGroups.size(); ++i)
	{
		SMeshBuffer* mb = 0;
		video::SMaterial* mat=0;
		u32 mbPos;
		// -3 because we add three vertices at once
		u32 maxPrimitives = core::min_(scene::Engine::Instance()->getMaximalPrimitiveCount(), (u32)((1<<16)-1))-3; // currently hardcoded s16 max value for index pointers

		// find mesh buffer for this group
		for (mbPos=0; mbPos<Materials.size(); ++mbPos)
		{
			if (MaterialGroups[i].MaterialName == Materials[mbPos].Name)
			{
				mb = (SMeshBuffer*)Mesh->getMeshBuffer(mbPos);
				mat=&Materials[mbPos].Material;
				MeshBufferNames[mbPos]=name;
				break;
			}
		}

		if (mb != 0)
		{
			// add geometry to the buffer.

			video::S3DVertex vtx;
			core::vector3df vec;
			vtx.Color=mat->DiffuseColor;
			if (mat->MaterialType==video::EMT_TRANSPARENT_VERTEX_ALPHA)
			{
				vtx.Color.setAlpha((int)(255.0f*mat->MaterialTypeParam));
			}
			vtx.Normal.set(0,0,0);

			for (s32 f=0; f<MaterialGroups[i].faceCount; ++f)
			{
				u32 vtxCount = mb->Vertices.size();
				if (vtxCount>maxPrimitives)
				{
					IMeshBuffer* tmp = mb;
					mb = new SMeshBuffer();
					Mesh->addMeshBuffer(mb);
					//mb->drop();
					Mesh->MeshBuffers[mbPos] = Mesh->MeshBuffers.getLast();
					Mesh->MeshBuffers[Mesh->MeshBuffers.size()-1] = tmp;
					mb->getMaterial() = tmp->getMaterial();
					vtxCount=0;
				}

				for (s32 v=0; v<3; ++v)
				{
					s32 idx = Indices[MaterialGroups[i].faces[f]*4 +v];

					if (CountVertices > idx)
					{
						vtx.Pos.X = Vertices[idx*3 + 0];
						vtx.Pos.Z = Vertices[idx*3 + 1];
						vtx.Pos.Y = Vertices[idx*3 + 2];
//						TransformationMatrix.transformVect(vtx.Pos);
					}

					if (CountTCoords > idx)
					{
						vtx.TCoords.X = TCoords[idx*2 + 0];
						vtx.TCoords.Y = 1.0f -TCoords[idx*2 + 1];
					}

					mb->Vertices.push_back(vtx);
				}

				// compute normal
				core::plane3d<f32> pl(mb->Vertices[vtxCount].Pos, mb->Vertices[vtxCount+2].Pos,
						mb->Vertices[vtxCount+1].Pos);

				mb->Vertices[vtxCount].Normal = pl.Normal;
				mb->Vertices[vtxCount+1].Normal = pl.Normal;
				mb->Vertices[vtxCount+2].Normal = pl.Normal;

				// add indices

				mb->Indices.push_back(vtxCount);
				mb->Indices.push_back(vtxCount+2);
				mb->Indices.push_back(vtxCount+1);
			}
		}
		else
			os::Printer::log("Found no matching material for Group in 3ds file.", ELL_WARNING);
	}

	cleanUp();
}


void C3DSMeshFileLoader::loadMaterials(io::IReadFile* file)
{
	if (Materials.empty())
		os::Printer::log("No materials found in 3ds file.", ELL_INFORMATION);

	// create a mesh buffer for every material
	MeshBufferNames.reallocate(Materials.size());
	for (u32 i=0; i<Materials.size(); ++i)
	{
		MeshBufferNames.push_back("");
		SMeshBuffer* m = new scene::SMeshBuffer();
		Mesh->addMeshBuffer(m);

		m->getMaterial() = Materials[i].Material;
		if (Materials[i].Filename[0].size())
		{
			video::ITexture* texture = scene::Engine::Instance()->getTexture(Materials[i].Filename[0]);
			if (!texture)
			{
				os::Printer::log("Could not load a texture for entry in 3ds file",
					Materials[i].Filename[0].c_str(), ELL_WARNING);
			}
			else
				m->getMaterial().setTexture(0, texture);
		}

		if (Materials[i].Filename[2].size())
		{
			video::ITexture* texture = scene::Engine::Instance()->getTexture(Materials[i].Filename[2]) ;
			if (!texture)
			{
				os::Printer::log("Could not load a texture for entry in 3ds file",
					Materials[i].Filename[2].c_str(), ELL_WARNING);
			}
			else
			{
				m->getMaterial().setTexture(0, texture);
				m->getMaterial().MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
			}
		}

		if (Materials[i].Filename[3].size())
		{
			video::ITexture* texture = scene::Engine::Instance()->getTexture(Materials[i].Filename[3]);
			if (!texture)
			{
				os::Printer::log("Could not load a texture for entry in 3ds file",
					Materials[i].Filename[3].c_str(), ELL_WARNING);
			}
			else
			{
				m->getMaterial().setTexture(1, m->getMaterial().getTexture(0));
				m->getMaterial().setTexture(0, texture);
				m->getMaterial().MaterialType = video::EMT_REFLECTION_2_LAYER;
			}
		}

		if (Materials[i].Filename[4].size())
		{
			video::ITexture* texture = scene::Engine::Instance()->getTexture(Materials[i].Filename[4]);
			if (!texture)
			{
				os::Printer::log("Could not load a texture for entry in 3ds file",
					Materials[i].Filename[4].c_str(), ELL_WARNING);
			}
			else
			{
				m->getMaterial().setTexture(1, texture);
				//SceneManager->getVideoDriver()->makeNormalMapTexture(texture, Materials[i].Strength[4]*10.f);
				m->getMaterial().MaterialType=video::EMT_PARALLAX_MAP_SOLID;
				m->getMaterial().MaterialTypeParam=.035f;
			}
		}

		//m->drop();
	}
}


void C3DSMeshFileLoader::cleanUp()
{
	delete [] Vertices;
	CountVertices = 0;
	Vertices = 0;
	delete [] Indices;
	Indices = 0;
	CountFaces = 0;
	delete [] SmoothingGroups;
	SmoothingGroups = 0;
	delete [] TCoords;
	TCoords = 0;
	CountTCoords = 0;

	MaterialGroups.clear();
}


void C3DSMeshFileLoader::readTextureCoords(io::IReadFile* file, ChunkData& data)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load texture coords.", ELL_DEBUG);
#endif
	file->read(&CountTCoords, sizeof(CountTCoords));
#ifdef __BIG_ENDIAN__
	CountTCoords = os::Byteswap::byteswap(CountTCoords);
#endif
	data.read += sizeof(CountTCoords);

	s32 tcoordsBufferByteSize = CountTCoords * sizeof(f32) * 2;

	if (data.header.length - data.read != tcoordsBufferByteSize)
	{
		os::Printer::log("Invalid size of tcoords found in 3ds file.", ELL_WARNING);
		return;
	}

	TCoords = new f32[CountTCoords * 3];
	file->read(TCoords, tcoordsBufferByteSize);
#ifdef __BIG_ENDIAN__
	for (int i=0;i<CountTCoords*2;i++) TCoords[i] = os::Byteswap::byteswap(TCoords[i]);
#endif
	data.read += tcoordsBufferByteSize;
}


void C3DSMeshFileLoader::readMaterialGroup(io::IReadFile* file, ChunkData& data)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load material group.", ELL_DEBUG);
#endif
	SMaterialGroup group;

	readString(file, data, group.MaterialName);

	file->read(&group.faceCount, sizeof(group.faceCount));
#ifdef __BIG_ENDIAN__
	group.faceCount = os::Byteswap::byteswap(group.faceCount);
#endif
	data.read += sizeof(group.faceCount);

	// read faces
	group.faces = new u16[group.faceCount];
	file->read(group.faces, sizeof(u16) * group.faceCount);
#ifdef __BIG_ENDIAN__
	for (u32 i=0;i<group.faceCount;++i)
		group.faces[i] = os::Byteswap::byteswap(group.faces[i]);
#endif
	data.read += sizeof(u16) * group.faceCount;

	MaterialGroups.push_back(group);
}


void C3DSMeshFileLoader::readIndices(io::IReadFile* file, ChunkData& data)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load indices.", ELL_DEBUG);
#endif
	file->read(&CountFaces, sizeof(CountFaces));
#ifdef __BIG_ENDIAN__
	CountFaces = os::Byteswap::byteswap(CountFaces);
#endif
	data.read += sizeof(CountFaces);

	s32 indexBufferByteSize = CountFaces * sizeof(u16) * 4;

	// Indices are u16s.
	// After every 3 Indices in the array, there follows an edge flag.
	Indices = new u16[CountFaces * 4];
	file->read(Indices, indexBufferByteSize);
#ifdef __BIG_ENDIAN__
	for (int i=0;i<CountFaces*4;++i)
		Indices[i] = os::Byteswap::byteswap(Indices[i]);
#endif
	data.read += indexBufferByteSize;
}


void C3DSMeshFileLoader::readVertices(io::IReadFile* file, ChunkData& data)
{
#ifdef _IRR_DEBUG_3DS_LOADER_
	os::Printer::log("Load vertices.", ELL_DEBUG);
#endif
	file->read(&CountVertices, sizeof(CountVertices));
#ifdef __BIG_ENDIAN__
	CountVertices = os::Byteswap::byteswap(CountVertices);
#endif
	data.read += sizeof(CountVertices);

	const s32 vertexBufferByteSize = CountVertices * sizeof(f32) * 3;

	if (data.header.length - data.read != vertexBufferByteSize)
	{
		os::Printer::log("Invalid size of vertices found in 3ds file", core::stringc(CountVertices), ELL_ERROR);
		return;
	}

	Vertices = new f32[CountVertices * 3];
	file->read(Vertices, vertexBufferByteSize);
#ifdef __BIG_ENDIAN__
	for (int i=0;i<CountVertices*3;i++)
		Vertices[i] = os::Byteswap::byteswap(Vertices[i]);
#endif
	data.read += vertexBufferByteSize;
}


void C3DSMeshFileLoader::readChunkData(io::IReadFile* file, ChunkData& data)
{
	file->read(&data.header, sizeof(ChunkHeader));
#ifdef __BIG_ENDIAN__
	data.header.id = os::Byteswap::byteswap(data.header.id);
	data.header.length = os::Byteswap::byteswap(data.header.length);
#endif
	data.read += sizeof(ChunkHeader);
}


void C3DSMeshFileLoader::readString(io::IReadFile* file, ChunkData& data, core::stringc& out)
{
	c8 c = 1;
	out = "";

	while (c)
	{
		file->read(&c, sizeof(c8));
		if (c)
			out.append(c);
	}
	data.read+=out.size()+1;
}

//********************************************************************OBJ**********************************************

static const u32 WORD_BUFFER_LENGTH = 512;

//! Constructor
COBJMeshFileLoader::COBJMeshFileLoader( )
{
		FileSystem=scene::Engine::Instance()->getFileSystem();
}


//! destructor
COBJMeshFileLoader::~COBJMeshFileLoader()
{

}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* COBJMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (!file)
		return 0;

    bool isDebug=false;

	const long filesize = file->getSize();
	if (!filesize)
		return 0;

	const u32 WORD_BUFFER_LENGTH = 512;

	core::array<core::vector3df, core::irrAllocatorFast<core::vector3df> > vertexBuffer(1000);
	core::array<core::vector3df, core::irrAllocatorFast<core::vector3df> > normalsBuffer(1000);
	core::array<core::vector2df, core::irrAllocatorFast<core::vector2df> > textureCoordBuffer(1000);

	SObjMtl * currMtl = new SObjMtl();
	Materials.push_back(currMtl);
	u32 smoothingGroup=0;

	const io::path fullName = file->getFileName();
	const io::path relPath = FileSystem->getFileDir(fullName)+"/";

	c8* buf = new c8[filesize];
	memset(buf, 0, filesize);
	file->read((void*)buf, filesize);
	const c8* const bufEnd = buf+filesize;

	// Process obj information
	const c8* bufPtr = buf;
	core::stringc grpName, mtlName;
	bool mtlChanged=false;
	bool useGroups =true; //!SceneManager->getParameters()->getAttributeAsBool(OBJ_LOADER_IGNORE_GROUPS);
	bool useMaterials =true;// !SceneManager->getParameters()->getAttributeAsBool(OBJ_LOADER_IGNORE_MATERIAL_FILES);
	irr::u32 lineNr = 1;	// only counts non-empty lines, still useful in debugging to locate errors
	core::array<int> faceCorners;
	faceCorners.reallocate(32); // should be large enough
	const core::stringc TAG_OFF = "off";
	irr::u32 degeneratedFaces = 0;

	while(bufPtr != bufEnd)
	{
		switch(bufPtr[0])
		{
		case 'm':	// mtllib (material)
		{
			if (useMaterials)
			{
				c8 name[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(name, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

				if(isDebug)os::Printer::log("Reading material file",name);

				readMTL(name, relPath);
			}
		}
			break;

		case 'v':               // v, vn, vt
			switch(bufPtr[1])
			{
			case ' ':          // vertex
				{
					core::vector3df vec;
					bufPtr = readVec3(bufPtr, vec, bufEnd);
					vertexBuffer.push_back(vec);
				}
				break;

			case 'n':       // normal
				{
					core::vector3df vec;
					bufPtr = readVec3(bufPtr, vec, bufEnd);
					normalsBuffer.push_back(vec);
				}
				break;

			case 't':       // texcoord
				{
					core::vector2df vec;
					bufPtr = readUV(bufPtr, vec, bufEnd);
					textureCoordBuffer.push_back(vec);
				}
				break;
			}
			break;

		case 'g': // group name
			{
				c8 grp[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(grp, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

               if(isDebug)os::Printer::log("Loaded group start",grp, ELL_DEBUG);

				if (useGroups)
				{
					if (0 != grp[0])
						grpName = grp;
					else
						grpName = "default";
				}
				mtlChanged=true;
			}
			break;

		case 's': // smoothing can be a group or off (equiv. to 0)
			{
				c8 smooth[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(smooth, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

if(isDebug)	os::Printer::log("Loaded smoothing group start",smooth, ELL_DEBUG);

				if (TAG_OFF==smooth)
					smoothingGroup=0;
				else
					smoothingGroup=core::strtoul10(smooth);

				(void)smoothingGroup; // disable unused variable warnings
			}
			break;

		case 'u': // usemtl
			// get name of material
			{
				c8 matName[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(matName, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

if(isDebug)	os::Printer::log("Loaded material start",matName, ELL_DEBUG);

				mtlName=matName;
				mtlChanged=true;
			}
			break;

		case 'f':               // face
		{
			c8 vertexWord[WORD_BUFFER_LENGTH]; // for retrieving vertex data
			video::S3DVertex v;
			// Assign vertex color from currently active material's diffuse color
			if (mtlChanged)
			{
				// retrieve the material
				SObjMtl *useMtl = findMtl(mtlName, grpName);
				// only change material if we found it
				if (useMtl)
					currMtl = useMtl;
				mtlChanged=false;
			}
			if (currMtl)
				v.Color = currMtl->Meshbuffer->Material.DiffuseColor;

			// get all vertices data in this face (current line of obj file)
			const core::stringc wordBuffer = copyLine(bufPtr, bufEnd);
			const c8* linePtr = wordBuffer.c_str();
			const c8* const endPtr = linePtr+wordBuffer.size();

			faceCorners.set_used(0); // fast clear

			// read in all vertices
			linePtr = goNextWord(linePtr, endPtr);
			while (0 != linePtr[0])
			{
				// Array to communicate with retrieveVertexIndices()
				// sends the buffer sizes and gets the actual indices
				// if index not set returns -1
				s32 Idx[3];
				Idx[0] = Idx[1] = Idx[2] = -1;

				// read in next vertex's data
				u32 wlength = copyWord(vertexWord, linePtr, WORD_BUFFER_LENGTH, endPtr);
				// this function will also convert obj's 1-based index to c++'s 0-based index
				retrieveVertexIndices(vertexWord, Idx, vertexWord+wlength+1, vertexBuffer.size(), textureCoordBuffer.size(), normalsBuffer.size());
				if ( -1 != Idx[0] && Idx[0] < (irr::s32)vertexBuffer.size() )
					v.Pos = vertexBuffer[Idx[0]];
				else
				{
					os::Printer::log("Invalid vertex index in this line:", wordBuffer.c_str(), ELL_ERROR);
					delete [] buf;
					return 0;
				}
				if ( -1 != Idx[1] && Idx[1] < (irr::s32)textureCoordBuffer.size() )
					v.TCoords = textureCoordBuffer[Idx[1]];
				else
					v.TCoords.set(0.0f,0.0f);
				if ( -1 != Idx[2] && Idx[2] < (irr::s32)normalsBuffer.size() )
					v.Normal = normalsBuffer[Idx[2]];
				else
				{
					v.Normal.set(0.0f,0.0f,0.0f);
					currMtl->RecalculateNormals=true;
				}

				int vertLocation;
				core::map<video::S3DVertex, int>::Node* n = currMtl->VertMap.find(v);
				if (n)
				{
					vertLocation = n->getValue();
				}
				else
				{
					currMtl->Meshbuffer->Vertices.push_back(v);
					vertLocation = currMtl->Meshbuffer->Vertices.size() -1;
					currMtl->VertMap.insert(v, vertLocation);
				}

				faceCorners.push_back(vertLocation);

				// go to next vertex
				linePtr = goNextWord(linePtr, endPtr);
			}

			// triangulate the face
			const int c = faceCorners[0];
			for ( u32 i = 1; i < faceCorners.size() - 1; ++i )
			{
				// Add a triangle
				const int a = faceCorners[i + 1];
				const int b = faceCorners[i];
				if (a != b && a != c && b != c)	// ignore degenerated faces. We can get them when we merge vertices above in the VertMap.
				{
					currMtl->Meshbuffer->Indices.push_back(a);
					currMtl->Meshbuffer->Indices.push_back(b);
					currMtl->Meshbuffer->Indices.push_back(c);
				}
				else
				{
					++degeneratedFaces;
				}
			}
		}
		break;

		case '#': // comment
		default:
			break;
		}	// end switch(bufPtr[0])
		// eat up rest of line
		bufPtr = goNextLine(bufPtr, bufEnd);
		++lineNr;
	}	// end while(bufPtr && (bufPtr-buf<filesize))

	if ( degeneratedFaces > 0 )
	{
		irr::core::stringc log(degeneratedFaces);
		log += " degenerated faces removed in ";
		log += irr::core::stringc(fullName);
		os::Printer::log(log.c_str(), ELL_INFORMATION);
	}

	SMesh* mesh = new SMesh();

	// Combine all the groups (meshbuffers) into the mesh
	for ( u32 m = 0; m < Materials.size(); ++m )
	{
		if ( Materials[m]->Meshbuffer->getIndexCount() > 0 )
		{
			Materials[m]->Meshbuffer->recalculateBoundingBox();
			if (Materials[m]->RecalculateNormals)
				scene::Engine::Instance()->getMeshManipulator()->recalculateNormals(Materials[m]->Meshbuffer);
			if (Materials[m]->Meshbuffer->Material.MaterialType == video::EMT_PARALLAX_MAP_SOLID)
			{
				SMesh tmp;
				tmp.addMeshBuffer(Materials[m]->Meshbuffer);
				IMesh* tangentMesh = scene::Engine::Instance()->getMeshManipulator()->createMeshWithTangents(&tmp);
				mesh->addMeshBuffer(tangentMesh->getMeshBuffer(0));
			//	tangentMesh->drop();
			}
			else
				mesh->addMeshBuffer( Materials[m]->Meshbuffer );
		}
	}

	// Create the Animated mesh if there's anything in the mesh
	SAnimatedMesh* animMesh = 0;
	if ( 0 != mesh->getMeshBufferCount() )
	{
		mesh->recalculateBoundingBox();
		animMesh = new SAnimatedMesh();
		animMesh->Type = EAMT_OBJ;
		animMesh->addMesh(mesh);
		animMesh->recalculateBoundingBox();
	}

	// Clean up the allocate obj file contents
	delete [] buf;
	// more cleaning up
	cleanUp();
	//mesh->drop();

	return animMesh;
}


const c8* COBJMeshFileLoader::readTextures(const c8* bufPtr, const c8* const bufEnd, SObjMtl* currMaterial, const io::path& relPath)
{
	u8 type=0; // map_Kd - diffuse color texture map
	// map_Ks - specular color texture map
	// map_Ka - ambient color texture map
	// map_Ns - shininess texture map
	if ((!strncmp(bufPtr,"map_bump",8)) || (!strncmp(bufPtr,"bump",4)))
		type=1; // normal map
	else if ((!strncmp(bufPtr,"map_d",5)) || (!strncmp(bufPtr,"map_opacity",11)))
		type=2; // opacity map
	else if (!strncmp(bufPtr,"map_refl",8))
		type=3; // reflection map
	// extract new material's name
	c8 textureNameBuf[WORD_BUFFER_LENGTH];
	bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

	f32 bumpiness = 6.0f;
	bool clamp = false;
	core::vector3df tscale(1.f);  //For  map_Kd texture scaling
	core::vector3df tpos(0.f);    //For  map_Kd texture translation
	// handle options
	while (textureNameBuf[0]=='-')
	{
		if (!strncmp(bufPtr,"-bm",3))
		{
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			currMaterial->Meshbuffer->Material.MaterialTypeParam=core::fast_atof(textureNameBuf);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			continue;
		}
		else
		if (!strncmp(bufPtr,"-blendu",7))
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		else
		if (!strncmp(bufPtr,"-blendv",7))
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		else
		if (!strncmp(bufPtr,"-cc",3))
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		else
		if (!strncmp(bufPtr,"-clamp",6))
			bufPtr = readBool(bufPtr, clamp, bufEnd);
		else
		if (!strncmp(bufPtr,"-texres",7))
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		else
		if (!strncmp(bufPtr,"-type",5))
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		else
		if (!strncmp(bufPtr,"-mm",3))
		{
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		}
		else
		if (!strncmp(bufPtr,"-o",2)) // texture coord translation
		{
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1])))
				tpos.X = core::fast_atof(textureNameBuf);

			// next parameters are optional, so skip rest of loop if no number is found
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
			tpos.Y = core::fast_atof(textureNameBuf);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
			tpos.Z = core::fast_atof(textureNameBuf);
		}
		else
		if (!strncmp(bufPtr,"-s",2)) // texture coord scale
		{
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1])))
				tscale.X = core::fast_atof(textureNameBuf);

			// next parameters are optional, so skip rest of loop if no number is found
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
			tscale.Y = core::fast_atof(textureNameBuf);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
			tscale.Z = core::fast_atof(textureNameBuf);
		}
		else
		if (!strncmp(bufPtr,"-t",2))
		{
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			// next parameters are optional, so skip rest of loop if no number is found
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
			if (!(core::isdigit(textureNameBuf[0]) || (textureNameBuf[0] == '-' && core::isdigit(textureNameBuf[1]))))
				continue;
		}
		// get next word
		bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	}

	if ((type==1) && (core::isdigit(textureNameBuf[0])))
	{
		currMaterial->Meshbuffer->Material.MaterialTypeParam=core::fast_atof(textureNameBuf);
		bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	}
	if (clamp)
		currMaterial->Meshbuffer->Material.setFlag(video::EMF_TEXTURE_WRAP, video::ETC_CLAMP);

	io::path texname(textureNameBuf);
	if (texname.size() )
	{
		video::ITexture * texture = scene::Engine::Instance()->getTexture(texname);
		if ( texture )
		{
			if (type==0)
			{
				currMaterial->Meshbuffer->Material.setTexture(0, texture);
				bool needsTextureMatrix = tscale != core::vector3df(1.f) || tpos != core::vector3df(0.f);
				if (needsTextureMatrix)
				{
					currMaterial->Meshbuffer->Material.getTextureMatrix(0).setTextureScale(tscale.X, tscale.Y);
					currMaterial->Meshbuffer->Material.getTextureMatrix(0).setTextureTranslate(tpos.X, tpos.Y);
				}
			}
			else if (type==1)
			{
				//if ( texture->getSource() == video::ETS_FROM_FILE)
				//	scene::Engine::Instance()->getVideoDriver()->makeNormalMapTexture(texture, bumpiness);
				currMaterial->Meshbuffer->Material.setTexture(1, texture);
				currMaterial->Meshbuffer->Material.MaterialType=video::EMT_PARALLAX_MAP_SOLID;
				currMaterial->Meshbuffer->Material.MaterialTypeParam=0.035f;
			}
			else if (type==2)
			{
				currMaterial->Meshbuffer->Material.setTexture(0, texture);
				currMaterial->Meshbuffer->Material.MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
			}
			else if (type==3)
			{
	//			currMaterial->Meshbuffer->Material.Textures[1] = texture;
	//			currMaterial->Meshbuffer->Material.MaterialType=video::EMT_REFLECTION_2_LAYER;
			}
			// Set diffuse material color to white so as not to affect texture color
			// Because Maya set diffuse color Kd to black when you use a diffuse color map
			// But is this the right thing to do?
			currMaterial->Meshbuffer->Material.DiffuseColor.set(
				currMaterial->Meshbuffer->Material.DiffuseColor.getAlpha(), 255, 255, 255 );
		}
	}
	return bufPtr;
}


void COBJMeshFileLoader::readMTL(const c8* fileName, const io::path& relPath)
{
	const io::path realFile(fileName);
	io::IReadFile * mtlReader;

	if (FileSystem->existFile(realFile))
		mtlReader = FileSystem->createAndOpenFile(realFile);
	else if (FileSystem->existFile(relPath + realFile))
		mtlReader = FileSystem->createAndOpenFile(relPath + realFile);
	else if (FileSystem->existFile(FileSystem->getFileBasename(realFile)))
		mtlReader = FileSystem->createAndOpenFile(FileSystem->getFileBasename(realFile));
	else
		mtlReader = FileSystem->createAndOpenFile(relPath + FileSystem->getFileBasename(realFile));
	if (!mtlReader)	// fail to open and read file
	{
		os::Printer::log("Could not open material file", realFile, ELL_WARNING);
		return;
	}


	//	getMeshTextureLoader()->setMaterialFile(mtlReader);
//		if ( SceneManager->getParameters()->existsAttribute(OBJ_TEXTURE_PATH) )
//			getMeshTextureLoader()->setTexturePath(SceneManager->getParameters()->getAttributeAsString(OBJ_TEXTURE_PATH));


	const long filesize = mtlReader->getSize();
	if (!filesize)
	{
		os::Printer::log("Skipping empty material file", realFile, ELL_WARNING);
		mtlReader->drop();
		return;
	}

	c8* buf = new c8[filesize];
	mtlReader->read((void*)buf, filesize);
	const c8* bufEnd = buf+filesize;

	SObjMtl* currMaterial = 0;

	const c8* bufPtr = buf;
	while(bufPtr != bufEnd)
	{
		switch(*bufPtr)
		{
			case 'n': // newmtl
			{
				// if there's an existing material, store it first
				if ( currMaterial )
					Materials.push_back( currMaterial );

				// extract new material's name
				c8 mtlNameBuf[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(mtlNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

				currMaterial = new SObjMtl;
				currMaterial->Name = mtlNameBuf;
			}
			break;
			case 'i': // illum - illumination
			if ( currMaterial )
			{
				const u32 COLOR_BUFFER_LENGTH = 16;
				c8 illumStr[COLOR_BUFFER_LENGTH];

				bufPtr = goAndCopyNextWord(illumStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
				currMaterial->Illumination = (c8)atol(illumStr);
			}
			break;
			case 'N':
			if ( currMaterial )
			{
				switch(bufPtr[1])
				{
				case 's': // Ns - shininess
					{
						const u32 COLOR_BUFFER_LENGTH = 16;
						c8 nsStr[COLOR_BUFFER_LENGTH];

						bufPtr = goAndCopyNextWord(nsStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
						f32 shininessValue = core::fast_atof(nsStr);

						// wavefront shininess is from [0, 1000], so scale for OpenGL
						shininessValue *= 0.128f;
						currMaterial->Meshbuffer->Material.Shininess = shininessValue;
					}
				break;
				case 'i': // Ni - refraction index
					{
						c8 tmpbuf[WORD_BUFFER_LENGTH];
						bufPtr = goAndCopyNextWord(tmpbuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					}
				break;
				}
			}
			break;
			case 'K':
			if ( currMaterial )
			{
				switch(bufPtr[1])
				{
				case 'd':		// Kd = diffuse
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->Material.DiffuseColor, bufEnd);

					}
					break;

				case 's':		// Ks = specular
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->Material.SpecularColor, bufEnd);
					}
					break;

				case 'a':		// Ka = ambience
					{
						bufPtr=readColor(bufPtr, currMaterial->Meshbuffer->Material.AmbientColor, bufEnd);
					}
					break;
				case 'e':		// Ke = emissive
					{
						currMaterial->Meshbuffer->Material.EmissiveColor.setAlpha(255);
						bufPtr=readColor(bufPtr, currMaterial->Meshbuffer->Material.EmissiveColor, bufEnd);
					}
					break;
				}	// end switch(bufPtr[1])
			}	// end case 'K': if ( 0 != currMaterial )...
			break;
			case 'b': // bump
			case 'm': // texture maps
			if (currMaterial)
			{
				bufPtr=readTextures(bufPtr, bufEnd, currMaterial, relPath);
			}
			break;
			case 'd': // d - transparency
			if ( currMaterial )
			{
				const u32 COLOR_BUFFER_LENGTH = 16;
				c8 dStr[COLOR_BUFFER_LENGTH];

				bufPtr = goAndCopyNextWord(dStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
				f32 dValue = core::fast_atof(dStr);

				currMaterial->Meshbuffer->Material.DiffuseColor.setAlpha( (s32)(dValue * 255) );
				if (dValue<1.0f)
					currMaterial->Meshbuffer->Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
			break;
			case 'T':
			if ( currMaterial )
			{
				switch ( bufPtr[1] )
				{
				case 'f':		// Tf - Transmitivity
					const u32 COLOR_BUFFER_LENGTH = 16;
					c8 redStr[COLOR_BUFFER_LENGTH];
					c8 greenStr[COLOR_BUFFER_LENGTH];
					c8 blueStr[COLOR_BUFFER_LENGTH];

					bufPtr = goAndCopyNextWord(redStr,   bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
					bufPtr = goAndCopyNextWord(greenStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
					bufPtr = goAndCopyNextWord(blueStr,  bufPtr, COLOR_BUFFER_LENGTH, bufEnd);

					f32 transparency = ( core::fast_atof(redStr) + core::fast_atof(greenStr) + core::fast_atof(blueStr) ) / 3;

					currMaterial->Meshbuffer->Material.DiffuseColor.setAlpha( (s32)(transparency * 255) );
					if (transparency < 1.0f)
						currMaterial->Meshbuffer->Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
				}
			}
			break;
			default: // comments or not recognised
			break;
		} // end switch(bufPtr[0])
		// go to next line
		bufPtr = goNextLine(bufPtr, bufEnd);
	}	// end while (bufPtr)

	// end of file. if there's an existing material, store it
	if ( currMaterial )
		Materials.push_back( currMaterial );

	delete [] buf;
	mtlReader->drop();
}


//! Read RGB color
const c8* COBJMeshFileLoader::readColor(const c8* bufPtr, video::SColor& color, const c8* const bufEnd)
{
	const u32 COLOR_BUFFER_LENGTH = 16;
	c8 colStr[COLOR_BUFFER_LENGTH];

	bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
	color.setRed((s32)(core::fast_atof(colStr) * 255.0f));
	bufPtr = goAndCopyNextWord(colStr,   bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
	color.setGreen((s32)(core::fast_atof(colStr) * 255.0f));
	bufPtr = goAndCopyNextWord(colStr,   bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
	color.setBlue((s32)(core::fast_atof(colStr) * 255.0f));
	return bufPtr;
}


//! Read 3d vector of floats
const c8* COBJMeshFileLoader::readVec3(const c8* bufPtr, core::vector3df& vec, const c8* const bufEnd)
{
	const u32 WORD_BUFFER_LENGTH = 256;
	c8 wordBuffer[WORD_BUFFER_LENGTH];

	bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	vec.X=-core::fast_atof(wordBuffer); // change handedness
	bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	vec.Y=core::fast_atof(wordBuffer);
	bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	vec.Z=core::fast_atof(wordBuffer);
	return bufPtr;
}


//! Read 2d vector of floats
const c8* COBJMeshFileLoader::readUV(const c8* bufPtr, core::vector2df& vec, const c8* const bufEnd)
{
	const u32 WORD_BUFFER_LENGTH = 256;
	c8 wordBuffer[WORD_BUFFER_LENGTH];

	bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	vec.X=core::fast_atof(wordBuffer);
	bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
	vec.Y=1-core::fast_atof(wordBuffer); // change handedness
	return bufPtr;
}


//! Read boolean value represented as 'on' or 'off'
const c8* COBJMeshFileLoader::readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd)
{
	const u32 BUFFER_LENGTH = 8;
	c8 tfStr[BUFFER_LENGTH];

	bufPtr = goAndCopyNextWord(tfStr, bufPtr, BUFFER_LENGTH, bufEnd);
	tf = strcmp(tfStr, "off") != 0;
	return bufPtr;
}


COBJMeshFileLoader::SObjMtl* COBJMeshFileLoader::findMtl(const core::stringc& mtlName, const core::stringc& grpName)
{
	COBJMeshFileLoader::SObjMtl* defMaterial = 0;
	// search existing Materials for best match
	// exact match does return immediately, only name match means a new group
	for (u32 i = 0; i < Materials.size(); ++i)
	{
		if ( Materials[i]->Name == mtlName )
		{
			if ( Materials[i]->Group == grpName )
				return Materials[i];
			else
				defMaterial = Materials[i];
		}
	}
	// we found a partial match
	if (defMaterial)
	{
		Materials.push_back(new SObjMtl(*defMaterial));
		Materials.getLast()->Group = grpName;
		return Materials.getLast();
	}
	// we found a new group for a non-existant material
	else if (grpName.size())
	{
		Materials.push_back(new SObjMtl(*Materials[0]));
		Materials.getLast()->Group = grpName;
		return Materials.getLast();
	}
	return 0;
}


//! skip space characters and stop on first non-space
const c8* COBJMeshFileLoader::goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
{
	// skip space characters
	if (acrossNewlines)
		while((buf != bufEnd) && core::isspace(*buf))
			++buf;
	else
		while((buf != bufEnd) && core::isspace(*buf) && (*buf != '\n'))
			++buf;

	return buf;
}


//! skip current word and stop at beginning of next one
const c8* COBJMeshFileLoader::goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
{
	// skip current word
	while(( buf != bufEnd ) && !core::isspace(*buf))
		++buf;

	return goFirstWord(buf, bufEnd, acrossNewlines);
}


//! Read until line break is reached and stop at the next non-space character
const c8* COBJMeshFileLoader::goNextLine(const c8* buf, const c8* const bufEnd)
{
	// look for newline characters
	while(buf != bufEnd)
	{
		// found it, so leave
		if (*buf=='\n' || *buf=='\r')
			break;
		++buf;
	}
	return goFirstWord(buf, bufEnd);
}


u32 COBJMeshFileLoader::copyWord(c8* outBuf, const c8* const inBuf, u32 outBufLength, const c8* const bufEnd)
{
	if (!outBufLength)
		return 0;
	if (!inBuf)
	{
		*outBuf = 0;
		return 0;
	}

	u32 i = 0;
	while(inBuf[i])
	{
		if (core::isspace(inBuf[i]) || &(inBuf[i]) == bufEnd)
			break;
		++i;
	}

	u32 length = core::min_(i, outBufLength-1);
	for (u32 j=0; j<length; ++j)
		outBuf[j] = inBuf[j];

	outBuf[length] = 0;
	return length;
}


core::stringc COBJMeshFileLoader::copyLine(const c8* inBuf, const c8* bufEnd)
{
	if (!inBuf)
		return core::stringc();

	const c8* ptr = inBuf;
	while (ptr<bufEnd)
	{
		if (*ptr=='\n' || *ptr=='\r')
			break;
		++ptr;
	}
	// we must avoid the +1 in case the array is used up
	return core::stringc(inBuf, (u32)(ptr-inBuf+((ptr < bufEnd) ? 1 : 0)));
}


const c8* COBJMeshFileLoader::goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* bufEnd)
{
	inBuf = goNextWord(inBuf, bufEnd, false);
	copyWord(outBuf, inBuf, outBufLength, bufEnd);
	return inBuf;
}


bool COBJMeshFileLoader::retrieveVertexIndices(c8* vertexData, s32* idx, const c8* bufEnd, u32 vbsize, u32 vtsize, u32 vnsize)
{
	c8 word[16] = "";
	const c8* p = goFirstWord(vertexData, bufEnd);
	u32 idxType = 0;	// 0 = posIdx, 1 = texcoordIdx, 2 = normalIdx

	u32 i = 0;
	while ( p != bufEnd )
	{
		if ( ( core::isdigit(*p)) || (*p == '-') )
		{
			// build up the number
			word[i++] = *p;
		}
		else if ( *p == '/' || *p == ' ' || *p == '\0' )
		{
			// number is completed. Convert and store it
			word[i] = '\0';
			// if no number was found index will become 0 and later on -1 by decrement
			idx[idxType] = core::strtol10(word);
			if (idx[idxType]<0)
			{
				switch (idxType)
				{
					case 0:
						idx[idxType] += vbsize;
						break;
					case 1:
						idx[idxType] += vtsize;
						break;
					case 2:
						idx[idxType] += vnsize;
						break;
				}
			}
			else
				idx[idxType]-=1;

			// reset the word
			word[0] = '\0';
			i = 0;

			// go to the next kind of index type
			if (*p == '/')
			{
				if ( ++idxType > 2 )
				{
					// error checking, shouldn't reach here unless file is wrong
					idxType = 0;
				}
			}
			else
			{
				// set all missing values to disable (=-1)
				while (++idxType < 3)
					idx[idxType]=-1;
				++p;
				break; // while
			}
		}

		// go to the next char
		++p;
	}

	return true;
}


void COBJMeshFileLoader::cleanUp()
{
	for (u32 i=0; i < Materials.size(); ++i )
	{
		//Materials[i]->Meshbuffer->drop();
		delete Materials[i];
	}

	Materials.clear();
}

//*************************************************************************************************************MD2

// structs needed to load the md2-format

const s32 MD2_MAGIC_NUMBER  = 844121161;
const s32 MD2_VERSION       = 8;
const s32 MD2_MAX_VERTS     = 2048;

// byte-align structures
#include "irrpack.h"

	struct SMD2Header
	{
		s32 magic;           // four character code "IDP2"
		s32 version;         // must be 8
		s32 skinWidth;	     // width of the texture
		s32 skinHeight;      // height of the texture
		s32 frameSize;       // size in bytes of an animation frame
		s32 numSkins;        // number of textures
		s32 numVertices;     // total number of vertices
		s32 numTexcoords;    // number of vertices with texture coords
		s32 numTriangles;    // number of triangles
		s32 numGlCommands;   // number of opengl commands (triangle strip or triangle fan)
		s32 numFrames;       // animation keyframe count
		s32 offsetSkins;     // offset in bytes to 64 character skin names
		s32 offsetTexcoords; // offset in bytes to texture coordinate list
		s32 offsetTriangles; // offset in bytes to triangle list
		s32 offsetFrames;    // offset in bytes to frame list
		s32 offsetGlCommands;// offset in bytes to opengl commands
		s32 offsetEnd;       // offset in bytes to end of file
	} PACK_STRUCT;

	struct SMD2Vertex
	{
		u8 vertex[3];        // [0] = X, [1] = Z, [2] = Y
		u8 lightNormalIndex; // index in the normal table
	} PACK_STRUCT;

	struct SMD2Frame
	{
		f32	scale[3];           // first scale the vertex position
		f32	translate[3];       // then translate the position
		c8	name[16];           // the name of the animation that this key belongs to
		SMD2Vertex vertices[1]; // vertex 1 of SMD2Header.numVertices
	} PACK_STRUCT;

	struct SMD2Triangle
	{
		u16 vertexIndices[3];
		u16 textureIndices[3];
	} PACK_STRUCT;

	struct SMD2TextureCoordinate
	{
		s16 s;
		s16 t;
	} PACK_STRUCT;

	struct SMD2GLCommand
	{
		f32 s, t;
		s32 vertexIndex;
	} PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

//! Constructor
CMD2MeshFileLoader::CMD2MeshFileLoader()
{

}




//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CMD2MeshFileLoader::createMesh(io::IReadFile* file)
{
	IAnimatedMesh* msh = new CAnimatedMeshMD2();
	if (msh)
	{
		if (loadFile(file, (CAnimatedMeshMD2*)msh) )
			return msh;

		msh->drop();
	}

	return 0;
}

//! loads an md2 file
bool CMD2MeshFileLoader::loadFile(io::IReadFile* file, CAnimatedMeshMD2* mesh)
{
	if (!file)
		return false;

	SMD2Header header;

	file->read(&header, sizeof(SMD2Header));

#ifdef __BIG_ENDIAN__
	header.magic = os::Byteswap::byteswap(header.magic);
	header.version = os::Byteswap::byteswap(header.version);
	header.skinWidth = os::Byteswap::byteswap(header.skinWidth);
	header.skinHeight = os::Byteswap::byteswap(header.skinHeight);
	header.frameSize = os::Byteswap::byteswap(header.frameSize);
	header.numSkins = os::Byteswap::byteswap(header.numSkins);
	header.numVertices = os::Byteswap::byteswap(header.numVertices);
	header.numTexcoords = os::Byteswap::byteswap(header.numTexcoords);
	header.numTriangles = os::Byteswap::byteswap(header.numTriangles);
	header.numGlCommands = os::Byteswap::byteswap(header.numGlCommands);
	header.numFrames = os::Byteswap::byteswap(header.numFrames);
	header.offsetSkins = os::Byteswap::byteswap(header.offsetSkins);
	header.offsetTexcoords = os::Byteswap::byteswap(header.offsetTexcoords);
	header.offsetTriangles = os::Byteswap::byteswap(header.offsetTriangles);
	header.offsetFrames = os::Byteswap::byteswap(header.offsetFrames);
	header.offsetGlCommands = os::Byteswap::byteswap(header.offsetGlCommands);
	header.offsetEnd = os::Byteswap::byteswap(header.offsetEnd);
#endif

	if (header.magic != MD2_MAGIC_NUMBER || header.version != MD2_VERSION)
	{
		os::Printer::log("MD2 Loader: Wrong file header", file->getFileName(), ELL_WARNING);
		return false;
	}

	//
	// prepare mesh and allocate memory
	//

	mesh->FrameCount = header.numFrames;

	// create keyframes
	mesh->FrameTransforms.set_used(header.numFrames);

	// create vertex arrays for each keyframe
	delete [] mesh->FrameList;
	mesh->FrameList = new core::array<CAnimatedMeshMD2::SMD2Vert>[header.numFrames];

	// allocate space in vertex arrays
	s32 i;
	for (i=0; i<header.numFrames; ++i)
		mesh->FrameList[i].reallocate(header.numVertices);

	// allocate interpolation buffer vertices
	mesh->InterpolationBuffer->Vertices.set_used(header.numTriangles*3);

	// populate triangles
	mesh->InterpolationBuffer->Indices.reallocate(header.numTriangles*3);
	const s32 count = header.numTriangles*3;
	for (i=0; i<count; i+=3)
	{
		mesh->InterpolationBuffer->Indices.push_back(i);
		mesh->InterpolationBuffer->Indices.push_back(i+1);
		mesh->InterpolationBuffer->Indices.push_back(i+2);
	}

	//
	// read texture coordinates
	//

	file->seek(header.offsetTexcoords);
	SMD2TextureCoordinate* textureCoords = new SMD2TextureCoordinate[header.numTexcoords];

	if (!file->read(textureCoords, sizeof(SMD2TextureCoordinate)*header.numTexcoords))
	{
		delete[] textureCoords;
		os::Printer::log("MD2 Loader: Error reading TextureCoords.", file->getFileName(), ELL_ERROR);
		return false;
	}

#ifdef __BIG_ENDIAN__
	for (i=0; i<header.numTexcoords; ++i)
	{
		textureCoords[i].s = os::Byteswap::byteswap(textureCoords[i].s);
		textureCoords[i].t = os::Byteswap::byteswap(textureCoords[i].t);
	}
#endif

	// read Triangles

	file->seek(header.offsetTriangles);

	SMD2Triangle *triangles = new SMD2Triangle[header.numTriangles];
	if (!file->read(triangles, header.numTriangles *sizeof(SMD2Triangle)))
	{
		delete[] triangles;
		delete[] textureCoords;

		os::Printer::log("MD2 Loader: Error reading triangles.", file->getFileName(), ELL_ERROR);
		return false;
	}

#ifdef __BIG_ENDIAN__
	for (i=0; i<header.numTriangles; ++i)
	{
		triangles[i].vertexIndices[0] = os::Byteswap::byteswap(triangles[i].vertexIndices[0]);
		triangles[i].vertexIndices[1] = os::Byteswap::byteswap(triangles[i].vertexIndices[1]);
		triangles[i].vertexIndices[2] = os::Byteswap::byteswap(triangles[i].vertexIndices[2]);
		triangles[i].textureIndices[0] = os::Byteswap::byteswap(triangles[i].textureIndices[0]);
		triangles[i].textureIndices[1] = os::Byteswap::byteswap(triangles[i].textureIndices[1]);
		triangles[i].textureIndices[2] = os::Byteswap::byteswap(triangles[i].textureIndices[2]);
	}
#endif

	// read Vertices

	u8 buffer[MD2_MAX_VERTS*4+128];
	SMD2Frame* frame = (SMD2Frame*)buffer;

	file->seek(header.offsetFrames);

	for (i = 0; i<header.numFrames; ++i)
	{
		// read vertices

		file->read(frame, header.frameSize);

#ifdef __BIG_ENDIAN__
		frame->scale[0] = os::Byteswap::byteswap(frame->scale[0]);
		frame->scale[1] = os::Byteswap::byteswap(frame->scale[1]);
		frame->scale[2] = os::Byteswap::byteswap(frame->scale[2]);
		frame->translate[0] = os::Byteswap::byteswap(frame->translate[0]);
		frame->translate[1] = os::Byteswap::byteswap(frame->translate[1]);
		frame->translate[2] = os::Byteswap::byteswap(frame->translate[2]);
#endif
		//
		// store frame data
		//

		CAnimatedMeshMD2::SAnimationData adata;
		adata.begin = i;
		adata.end = i;
		adata.fps = 7;

		// Add new named animation if necessary
		if (frame->name[0])
		{
			// get animation name
			for (s32 s = 0; s < 16 && frame->name[s]!=0 && (frame->name[s] < '0' || frame->name[s] > '9'); ++s)
			{
				adata.name += frame->name[s];
			}

			// Does this keyframe have the same animation name as the current animation?
			if (!mesh->AnimationData.empty() && mesh->AnimationData[mesh->AnimationData.size()-1].name == adata.name)
			{
				// Increase the length of the animation
				++mesh->AnimationData[mesh->AnimationData.size() - 1].end;
			}
			else
			{
				// Add the new animation
				mesh->AnimationData.push_back(adata);
			}
		}

		// save keyframe scale and translation

		mesh->FrameTransforms[i].scale.X = frame->scale[0];
		mesh->FrameTransforms[i].scale.Z = frame->scale[1];
		mesh->FrameTransforms[i].scale.Y = frame->scale[2];
		mesh->FrameTransforms[i].translate.X = frame->translate[0];
		mesh->FrameTransforms[i].translate.Z = frame->translate[1];
		mesh->FrameTransforms[i].translate.Y = frame->translate[2];

		// add vertices
		for (s32 j=0; j<header.numTriangles; ++j)
		{
			for (u32 ti=0; ti<3; ++ti)
			{
				CAnimatedMeshMD2::SMD2Vert v;
				u32 num = triangles[j].vertexIndices[ti];
				v.Pos.X = frame->vertices[num].vertex[0];
				v.Pos.Z = frame->vertices[num].vertex[1];
				v.Pos.Y = frame->vertices[num].vertex[2];
				v.NormalIdx = frame->vertices[num].lightNormalIndex;

				mesh->FrameList[i].push_back(v);
			}
		}

		// calculate bounding boxes
		if (header.numVertices)
		{
			core::aabbox3d<f32> box;
			core::vector3df pos;
			pos.X = f32(mesh->FrameList[i] [0].Pos.X) * mesh->FrameTransforms[i].scale.X + mesh->FrameTransforms[i].translate.X;
			pos.Y = f32(mesh->FrameList[i] [0].Pos.Y) * mesh->FrameTransforms[i].scale.Y + mesh->FrameTransforms[i].translate.Y;
			pos.Z = f32(mesh->FrameList[i] [0].Pos.Z) * mesh->FrameTransforms[i].scale.Z + mesh->FrameTransforms[i].translate.Z;

			box.reset(pos);

			for (s32 j=1; j<header.numTriangles*3; ++j)
			{
				pos.X = f32(mesh->FrameList[i] [j].Pos.X) * mesh->FrameTransforms[i].scale.X + mesh->FrameTransforms[i].translate.X;
				pos.Y = f32(mesh->FrameList[i] [j].Pos.Y) * mesh->FrameTransforms[i].scale.Y + mesh->FrameTransforms[i].translate.Y;
				pos.Z = f32(mesh->FrameList[i] [j].Pos.Z) * mesh->FrameTransforms[i].scale.Z + mesh->FrameTransforms[i].translate.Z;

				box.addInternalPoint(pos);
			}
			mesh->BoxList.push_back(box);
		}
	}

	// populate interpolation buffer with texture coordinates and colors
	if (header.numFrames)
	{
		f32 dmaxs = 1.0f/(header.skinWidth);
		f32 dmaxt = 1.0f/(header.skinHeight);

		for (s32 t=0; t<header.numTriangles; ++t)
		{
			for (s32 n=0; n<3; ++n)
			{
				mesh->InterpolationBuffer->Vertices[t*3 + n].TCoords.X = (textureCoords[triangles[t].textureIndices[n]].s + 0.5f) * dmaxs;
				mesh->InterpolationBuffer->Vertices[t*3 + n].TCoords.Y = (textureCoords[triangles[t].textureIndices[n]].t + 0.5f) * dmaxt;
				mesh->InterpolationBuffer->Vertices[t*3 + n].Color = video::SColor(255,255,255,255);
			}
		}
	}

	// clean up
	delete [] triangles;
	delete [] textureCoords;

	// init buffer with start frame.
	mesh->getMesh(0);
	return true;
}
//******************************************************************************MD3

//! Constructor
CMD3MeshFileLoader::CMD3MeshFileLoader()
{
}


//! destructor
CMD3MeshFileLoader::~CMD3MeshFileLoader()
{
}


IAnimatedMesh* CMD3MeshFileLoader::createMesh(io::IReadFile* file)
{
	CAnimatedMeshMD3 * mesh = new CAnimatedMeshMD3();

	if ( mesh->loadModelFile ( 0, file ) )
		return mesh;

	//mesh->drop ();
	return 0;
}
//***************************************************************************************MS3D

// byte-align structures
#include "irrpack.h"

namespace {
// File header
struct MS3DHeader
{
	char ID[10];
	int Version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	u8 pad1[3];
	u8 Flags;
	float Vertex[3];
	char BoneID;
	u8 RefCount;
	u8 pad2[2];
} PACK_STRUCT;

#define MS3DVERTEX_NUM_PAD_BYTES 5

// Triangle information
struct MS3DTriangle
{
	u16 Flags;
	u16 VertexIndices[3];
	float VertexNormals[3][3];
	float S[3], T[3];
	u8 SmoothingGroup;
	u8 GroupIndex;
	u8 pad1[2];
} PACK_STRUCT;

#define MS3DTRIANGLE_NUM_PAD_BYTES 2

// Material information
struct MS3DMaterial
{
    char Name[32];
    float Ambient[4];
    float Diffuse[4];
    float Specular[4];
    float Emissive[4];
    float Shininess;	// 0.0f - 128.0f
    float Transparency;	// 0.0f - 1.0f
    u8 Mode;	// 0, 1, 2 is unused now
    char Texture[128];
    char Alphamap[128];
    u8 pad1[3];
} PACK_STRUCT;

#define MS3DMATERIAL_NUM_PAD_BYTES 3

// Joint information
struct MS3DJoint
{
	u8 pad[3];
	u8 Flags;
	char Name[32];
	char ParentName[32];
	float Rotation[3];
	float Translation[3];
	u16 NumRotationKeyframes;
	u16 NumTranslationKeyframes;
} PACK_STRUCT;

#define MS3DJOINT_NUM_PAD_BYTES 3

// Keyframe data
struct MS3DKeyframe
{
	float Time;
	float Parameter[3];
} PACK_STRUCT;

// vertex weights in 1.8.x
struct MS3DVertexWeights
{
	char boneIds[3];
	u8 weights[3];
} PACK_STRUCT;

} // end namespace

// Default alignment
#include "irrunpack.h"

// Get float encoded in little endian in  way not causing troubles when floats have to be memory aligned.
static inline float get_unaligned_le_float(const u8 *ptr)
{
    union {
        u8 u[4];
        float f;
    } tmp;
#ifdef __BIG_ENDIAN__
    tmp.u[0] = ptr[3];
    tmp.u[1] = ptr[2];
    tmp.u[2] = ptr[1];
    tmp.u[3] = ptr[0];
#else
    tmp.f = *(float*)ptr;
#endif
    return tmp.f;
}


struct SGroup
{
	core::stringc Name;
	core::array<u16> VertexIds;
	u16 MaterialIdx;
};

//! Constructor
CMS3DMeshFileLoader::CMS3DMeshFileLoader()
:  AnimatedMesh(0)
{


}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CMS3DMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (!file)
		return 0;

	AnimatedMesh = new CSkinnedMesh();

	if ( load(file) )
	{
		AnimatedMesh->finalize();
	}
	else
	{
		AnimatedMesh->drop();
		AnimatedMesh = 0;
	}

	return AnimatedMesh;
}


//! loads a milkshape file
bool CMS3DMeshFileLoader::load(io::IReadFile* file)
{
	if (!file)
		return false;

		bool showLoad=false;

	// find file size
	const long fileSize = file->getSize();

	// read whole file

	u8* buffer = new u8[fileSize];
	size_t read = file->read(buffer, fileSize);
	if (read != (size_t)fileSize)
	{
		delete [] buffer;
		os::Printer::log("Could not read full file. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read header

	const u8 *pPtr = (u8*)((void*)buffer);
	MS3DHeader *pHeader = (MS3DHeader*)pPtr;
	pPtr += sizeof(MS3DHeader);

	if ( strncmp( pHeader->ID, "MS3D000000", 10 ) != 0 )
	{
		delete [] buffer;
		os::Printer::log("Not a valid Milkshape3D Model File. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

#ifdef __BIG_ENDIAN__
	pHeader->Version = os::Byteswap::byteswap(pHeader->Version);
#endif
	if ( pHeader->Version < 3 || pHeader->Version > 4 )
	{
		delete [] buffer;
		os::Printer::log("Only Milkshape3D version 3 and 4 (1.3 to 1.8) is supported. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}
#ifdef _IRR_DEBUG_MS3D_LOADER_
	os::Printer::log("Loaded header version", core::stringc(pHeader->Version).c_str());
#endif

	// get pointers to data

	// vertices
	u16 numVertices = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numVertices = os::Byteswap::byteswap(numVertices);
#endif
#ifdef _IRR_DEBUG_MS3D_LOADER_
	os::Printer::log("Load vertices", core::stringc(numVertices).c_str());
#endif

	pPtr += sizeof(u16);
	MS3DVertex *vertices = new MS3DVertex[numVertices];
	if (pPtr + ((sizeof(MS3DVertex) - MS3DVERTEX_NUM_PAD_BYTES) * numVertices) > buffer+fileSize)
	{
		delete [] buffer;
		os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
		return false;
	}
	for (u16 tmp=0; tmp<numVertices; ++tmp)
	{
		//printf("&vertices[tmp].Vertex[0] = %p (%d)\n", &vertices[tmp].Vertex[0], (int)((long long)(&vertices[tmp].Vertex[0]) % 4));
		memcpy(&vertices[tmp].Flags, pPtr, sizeof(struct MS3DVertex) - MS3DVERTEX_NUM_PAD_BYTES);
#ifdef __BIG_ENDIAN__
		vertices[tmp].Vertex[0] = os::Byteswap::byteswap(vertices[tmp].Vertex[0]);
		vertices[tmp].Vertex[1] = os::Byteswap::byteswap(vertices[tmp].Vertex[1]);
		vertices[tmp].Vertex[2] = -os::Byteswap::byteswap(vertices[tmp].Vertex[2]);
#else
		vertices[tmp].Vertex[2] = -vertices[tmp].Vertex[2];
#endif
		// Go to the next vertex structure
		pPtr += sizeof(struct MS3DVertex) - MS3DVERTEX_NUM_PAD_BYTES;
	}

	// triangles
	u16 numTriangles = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numTriangles = os::Byteswap::byteswap(numTriangles);
#endif

if (showLoad)	os::Printer::log("Load Triangles", core::stringc(numTriangles).c_str());

	pPtr += sizeof(u16);
	MS3DTriangle *triangles = new MS3DTriangle[numTriangles];
	if (pPtr + ((sizeof(MS3DTriangle) - MS3DTRIANGLE_NUM_PAD_BYTES) * numTriangles) > buffer+fileSize)
	{
		delete [] buffer;
		os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
		return false;
	}
	for (u16 tmp=0; tmp<numTriangles; ++tmp)
	{
		memcpy(&triangles[tmp].Flags, pPtr, sizeof(struct MS3DTriangle) - MS3DTRIANGLE_NUM_PAD_BYTES);
#ifdef __BIG_ENDIAN__
		triangles[tmp].Flags = os::Byteswap::byteswap(triangles[tmp].Flags);
		for (u16 j=0; j<3; ++j)
		{
			triangles[tmp].VertexIndices[j] = os::Byteswap::byteswap(triangles[tmp].VertexIndices[j]);
			triangles[tmp].VertexNormals[j][0] = os::Byteswap::byteswap(triangles[tmp].VertexNormals[j][0]);
			triangles[tmp].VertexNormals[j][1] = os::Byteswap::byteswap(triangles[tmp].VertexNormals[j][1]);
			triangles[tmp].VertexNormals[j][2] = -os::Byteswap::byteswap(triangles[tmp].VertexNormals[j][2]);
			triangles[tmp].S[j] = os::Byteswap::byteswap(triangles[tmp].S[j]);
			triangles[tmp].T[j] = os::Byteswap::byteswap(triangles[tmp].T[j]);
		}
#else
		triangles[tmp].VertexNormals[0][2] = -triangles[tmp].VertexNormals[0][2];
		triangles[tmp].VertexNormals[1][2] = -triangles[tmp].VertexNormals[1][2];
		triangles[tmp].VertexNormals[2][2] = -triangles[tmp].VertexNormals[2][2];
#endif
		// Go to the next triangle structure
		pPtr += sizeof(struct MS3DTriangle) - MS3DTRIANGLE_NUM_PAD_BYTES;
	}

	// groups
	u16 numGroups = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numGroups = os::Byteswap::byteswap(numGroups);
#endif

	if (showLoad)os::Printer::log("Load Groups", core::stringc(numGroups).c_str());

	pPtr += sizeof(u16);

	core::array<SGroup> groups;
	groups.reallocate(numGroups);

	//store groups
	u32 i;
	for (i=0; i<numGroups; ++i)
	{
		groups.push_back(SGroup());
		SGroup& grp = groups.getLast();

		// The byte flag is before the name, so add 1
		grp.Name = ((const c8*) pPtr) + 1;

		pPtr += 33; // name and 1 byte flags
		u16 triangleCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
		triangleCount = os::Byteswap::byteswap(triangleCount);
#endif
		pPtr += sizeof(u16);
		grp.VertexIds.reallocate(triangleCount);

		//pPtr += sizeof(u16) * triangleCount; // triangle indices
		for (u16 j=0; j<triangleCount; ++j)
		{
#ifdef __BIG_ENDIAN__
			grp.VertexIds.push_back(os::Byteswap::byteswap(*(u16*)pPtr));
#else
			grp.VertexIds.push_back(*(u16*)pPtr);
#endif
			pPtr += sizeof (u16);
		}

		grp.MaterialIdx = *(u8*)pPtr;
		if (grp.MaterialIdx == 255)
			grp.MaterialIdx = 0;

		pPtr += sizeof(c8); // material index
		if (pPtr > buffer+fileSize)
		{
			delete [] buffer;
			os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
			return false;
		}
	}

	// load materials
	u16 numMaterials = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numMaterials = os::Byteswap::byteswap(numMaterials);
#endif

	if (showLoad)os::Printer::log("Load Materials", core::stringc(numMaterials).c_str());

	pPtr += sizeof(u16);

	if(numMaterials == 0)
	{
		// if there are no materials, add at least one buffer
		AnimatedMesh->addMeshBuffer();
	}

	MS3DMaterial *material = new MS3DMaterial;
	for (i=0; i<numMaterials; ++i)
	{
		memcpy(material, pPtr, sizeof(struct MS3DMaterial) - MS3DMATERIAL_NUM_PAD_BYTES);
#ifdef __BIG_ENDIAN__
		for (u16 j=0; j<4; ++j)
			material->Ambient[j] = os::Byteswap::byteswap(material->Ambient[j]);
		for (u16 j=0; j<4; ++j)
			material->Diffuse[j] = os::Byteswap::byteswap(material->Diffuse[j]);
		for (u16 j=0; j<4; ++j)
			material->Specular[j] = os::Byteswap::byteswap(material->Specular[j]);
		for (u16 j=0; j<4; ++j)
			material->Emissive[j] = os::Byteswap::byteswap(material->Emissive[j]);
		material->Shininess = os::Byteswap::byteswap(material->Shininess);
		material->Transparency = os::Byteswap::byteswap(material->Transparency);
#endif
		pPtr += (sizeof(MS3DMaterial) - MS3DMATERIAL_NUM_PAD_BYTES);
		if (pPtr > buffer+fileSize)
		{
			delete [] buffer;
			os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
			return false;
		}

	if (showLoad)	SDL_Log("Create mesh buffer " );
		scene::SSkinMeshBuffer *tmpBuffer = AnimatedMesh->addMeshBuffer();

		tmpBuffer->Material.MaterialType = video::EMT_SOLID;

		tmpBuffer->Material.AmbientColor = video::SColorf(material->Ambient[0], material->Ambient[1], material->Ambient[2], material->Ambient[3]).toSColor ();
		tmpBuffer->Material.DiffuseColor = video::SColorf(material->Diffuse[0], material->Diffuse[1], material->Diffuse[2], material->Diffuse[3]).toSColor ();
		tmpBuffer->Material.EmissiveColor = video::SColorf(material->Emissive[0], material->Emissive[1], material->Emissive[2], material->Emissive[3]).toSColor ();
		tmpBuffer->Material.SpecularColor = video::SColorf(material->Specular[0], material->Specular[1], material->Specular[2], material->Specular[3]).toSColor ();
		tmpBuffer->Material.Shininess = material->Shininess;


		core::stringc TexturePath(material->Texture);

		if (TexturePath.trim()!="")
		{
			TexturePath=stripPathFromString(file->getFileName(),true) + stripPathFromString(TexturePath,false);

			SDL_Log("Load %s " ,TexturePath.c_str());

			tmpBuffer->Material.setTexture(0, scene::Engine::Instance()->getTexture(TexturePath));
		}

		core::stringc AlphamapPath=(const c8*)material->Alphamap;
		if (AlphamapPath.trim()!="")
		{
			AlphamapPath=stripPathFromString(file->getFileName(),true) + stripPathFromString(AlphamapPath,false);
			SDL_Log("Load %s " ,AlphamapPath.c_str());
			tmpBuffer->Material.setTexture(2, scene::Engine::Instance()->getTexture(AlphamapPath));
		}
	}
	delete material;

	// animation time


	f32 framesPerSecond = get_unaligned_le_float(pPtr);


	os::Printer::log("FPS", core::stringc(framesPerSecond).c_str());

	pPtr += sizeof(float) * 2; // fps and current time

	if (framesPerSecond<1.f)
		framesPerSecond=1.f;
	AnimatedMesh->setAnimationSpeed(framesPerSecond);

// ignore, calculated inside SkinnedMesh
//	s32 frameCount = *(int*)pPtr;
#ifdef __BIG_ENDIAN__
//	frameCount = os::Byteswap::byteswap(frameCount);
#endif
	pPtr += sizeof(int);

	u16 jointCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	jointCount = os::Byteswap::byteswap(jointCount);
#endif

	os::Printer::log("Joints", core::stringc(jointCount).c_str());

	pPtr += sizeof(u16);
	if (pPtr > buffer+fileSize)
	{
		delete [] buffer;
		os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
		return false;
	}

	core::array<core::stringc> parentNames;
	parentNames.reallocate(jointCount);

	// load joints
	for (i=0; i<jointCount; ++i)
	{
		u32 j;
		MS3DJoint *pJoint = new MS3DJoint;
		//printf("&pJoint->Rotation[0] = %p (%d)\n", &pJoint->Rotation[0], (int)((long long)(&pJoint->Rotation[0]) % 4));
		memcpy(&pJoint->Flags, pPtr, sizeof(MS3DJoint) - MS3DJOINT_NUM_PAD_BYTES);

#ifdef __BIG_ENDIAN__
		for (j=0; j<3; ++j)
			pJoint->Rotation[j] = os::Byteswap::byteswap(pJoint->Rotation[j]);
		for (j=0; j<3; ++j)
			pJoint->Translation[j] = os::Byteswap::byteswap(pJoint->Translation[j]);
		pJoint->NumRotationKeyframes= os::Byteswap::byteswap(pJoint->NumRotationKeyframes);
		pJoint->NumTranslationKeyframes = os::Byteswap::byteswap(pJoint->NumTranslationKeyframes);
#endif
		pPtr = pPtr + sizeof(MS3DJoint) - MS3DJOINT_NUM_PAD_BYTES;
		if (pPtr > buffer+fileSize)
		{
			delete [] buffer;
			os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
			return false;
		}

		ISkinnedMesh::SJoint *jnt = AnimatedMesh->addJoint();

		jnt->Name = pJoint->Name;

		if (showLoad)
		{
		os::Printer::log("Joint", jnt->Name.c_str());
		os::Printer::log("Rotation keyframes", core::stringc(pJoint->NumRotationKeyframes).c_str());
		os::Printer::log("Translation keyframes", core::stringc(pJoint->NumTranslationKeyframes).c_str());
		}

		jnt->LocalMatrix.makeIdentity();
		jnt->LocalMatrix.setRotationRadians(
			core::vector3df(pJoint->Rotation[0], pJoint->Rotation[1], pJoint->Rotation[2]) );
		// convert right-handed to left-handed
		jnt->LocalMatrix[2]=-jnt->LocalMatrix[2];
		jnt->LocalMatrix[6]=-jnt->LocalMatrix[6];
		jnt->LocalMatrix[8]=-jnt->LocalMatrix[8];
		jnt->LocalMatrix[9]=-jnt->LocalMatrix[9];

		jnt->LocalMatrix.setTranslation(
			core::vector3df(pJoint->Translation[0], pJoint->Translation[1], -pJoint->Translation[2]) );
		jnt->Animatedposition.set(jnt->LocalMatrix.getTranslation());
		jnt->Animatedrotation.set(jnt->LocalMatrix.getRotationDegrees());

		parentNames.push_back( (c8*)pJoint->ParentName );

		/*if (pJoint->NumRotationKeyframes ||
			pJoint->NumTranslationKeyframes)
			HasAnimation = true;
		 */

		MS3DKeyframe* kf = new MS3DKeyframe;

		// get rotation keyframes
		const u16 numRotationKeyframes = pJoint->NumRotationKeyframes;
		for (j=0; j < numRotationKeyframes; ++j)
		{
			memcpy(kf, pPtr, sizeof(MS3DKeyframe));
			//printf("rotation kf = %p (%d)\n", kf, (int)((long long)kf % 4));
#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);
			if (pPtr > buffer+fileSize)
			{
				delete [] buffer;
				os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
				return false;
			}

			ISkinnedMesh::SRotationKey *k=AnimatedMesh->addRotationKey(jnt);
			k->frame = kf->Time * framesPerSecond-1;

			core::matrix4 tmpMatrix;

			tmpMatrix.setRotationRadians(
				core::vector3df(kf->Parameter[0], kf->Parameter[1], kf->Parameter[2]) );
			// convert right-handed to left-handed
			tmpMatrix[2]=-tmpMatrix[2];
			tmpMatrix[6]=-tmpMatrix[6];
			tmpMatrix[8]=-tmpMatrix[8];
			tmpMatrix[9]=-tmpMatrix[9];

			tmpMatrix=jnt->LocalMatrix*tmpMatrix;

			// IRR_TEST_BROKEN_QUATERNION_USE: TODO - switched from tmpMatrix to tmpMatrix.getTransposed() for downward compatibility.
			//								   Not tested so far if this was correct or wrong before quaternion fix!
			k->rotation  = core::quaternion(tmpMatrix.getTransposed());
		}

		// get translation keyframes
		const u16 numTranslationKeyframes = pJoint->NumTranslationKeyframes;
		for (j=0; j<numTranslationKeyframes; ++j)
		{
			memcpy(kf, pPtr, sizeof(MS3DKeyframe));
			//printf("translation kf = %p (%d)\n", kf, (int)((long long)kf % 4));

#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);
			if (pPtr > buffer+fileSize)
			{
				delete [] buffer;
				os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
				return false;
			}

			ISkinnedMesh::SPositionKey *k=AnimatedMesh->addPositionKey(jnt);
			k->frame = kf->Time * framesPerSecond-1;

			k->position = core::vector3df
				(kf->Parameter[0]+pJoint->Translation[0],
				 kf->Parameter[1]+pJoint->Translation[1],
				 -kf->Parameter[2]-pJoint->Translation[2]);
		}

		delete kf;
		delete pJoint;
	}

	core::array<MS3DVertexWeights> vertexWeights;
	f32 weightFactor=0;

	if (jointCount && (pHeader->Version == 4) && (pPtr < buffer+fileSize))
	{
		s32 subVersion = *(s32*)pPtr; // comment subVersion, always 1
#ifdef __BIG_ENDIAN__
		subVersion = os::Byteswap::byteswap(subVersion);
#endif
		pPtr += sizeof(s32);

		for (u32 j=0; j<4; ++j) // four comment groups
		{

			os::Printer::log("Skipping comment group", core::stringc(j+1).c_str());

			u32 numComments = *(u32*)pPtr;
#ifdef __BIG_ENDIAN__
			numComments = os::Byteswap::byteswap(numComments);
#endif
			pPtr += sizeof(u32);
			for (i=0; i<numComments; ++i)
			{
				// according to scorpiomidget this field does
				// not exist for model comments. So avoid to
				// read it
				if (j!=3)
					pPtr += sizeof(s32); // index
				s32 commentLength = *(s32*)pPtr;
#ifdef __BIG_ENDIAN__
				commentLength = os::Byteswap::byteswap(commentLength);
#endif
				pPtr += sizeof(s32);
				pPtr += commentLength;
			}

			if (pPtr > buffer+fileSize)
			{
				delete [] buffer;
				os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
				return false;
			}
		}

		if (pPtr < buffer+fileSize)
		{
			subVersion = *(s32*)pPtr; // vertex subVersion, 1 or 2
#ifdef __BIG_ENDIAN__
			subVersion = os::Byteswap::byteswap(subVersion);
#endif
			if (subVersion==1)
				weightFactor=1.f/255.f;
			else
				weightFactor=1.f/100.f;
			pPtr += sizeof(s32);


	if (showLoad)		os::Printer::log("Reading vertex weights");

			// read vertex weights, ignoring data 'extra' from 1.8.2
			vertexWeights.reallocate(numVertices);
			const char offset = (subVersion==1)?6:10;
			for (i=0; i<numVertices; ++i)
			{
				vertexWeights.push_back(*(MS3DVertexWeights*)pPtr);
				pPtr += offset;
			}

			if (pPtr > buffer+fileSize)
			{
				delete [] buffer;
				os::Printer::log("Loading failed. Corrupted data found.", file->getFileName(), ELL_ERROR);
				return false;
			}
		}

		if (pPtr < buffer+fileSize)
		{
			subVersion = *(s32*)pPtr; // joint subVersion, 1 or 2
#ifdef __BIG_ENDIAN__
			subVersion = os::Byteswap::byteswap(subVersion);
#endif
			pPtr += sizeof(s32);
			// skip joint colors

		if (showLoad)	os::Printer::log("Skip joint color");

			pPtr += 3*sizeof(float)*jointCount;

			if (pPtr > buffer+fileSize)
			{
				delete [] buffer;
				os::Printer::log("Loading failed. Corrupted data found", file->getFileName(), ELL_ERROR);
				return false;
			}
		}

		if (pPtr < buffer+fileSize)
		{
			subVersion = *(s32*)pPtr; // model subVersion, 1 or 2
#ifdef __BIG_ENDIAN__
			subVersion = os::Byteswap::byteswap(subVersion);
#endif
			pPtr += sizeof(s32);

		if (showLoad)	os::Printer::log("Skip model extra information");

			// now the model extra information would follow
			// we also skip this for now
		}
	}

	//find parent of every joint
	for (u32 jointnum=0; jointnum<AnimatedMesh->getAllJoints().size(); ++jointnum)
	{
		for (u32 j2=0; j2<AnimatedMesh->getAllJoints().size(); ++j2)
		{
			if (jointnum != j2 && parentNames[jointnum] == AnimatedMesh->getAllJoints()[j2]->Name )
			{
				AnimatedMesh->getAllJoints()[j2]->Children.push_back(AnimatedMesh->getAllJoints()[jointnum]);
				break;
			}
		}
	}

	// create vertices and indices, attach them to the joints.
	video::S3DVertex v;
	core::array<video::S3DVertex> *Vertices;
	core::array<u16> Indices;

	for (i=0; i<numTriangles; ++i)
	{
		u32 tmp = groups[triangles[i].GroupIndex].MaterialIdx;
		Vertices = &AnimatedMesh->getMeshBuffers()[tmp]->Vertices_Standard;

		for (s32 j = 2; j!=-1; --j)
		{
			const u32 vertidx = triangles[i].VertexIndices[j];

			v.TCoords.X = triangles[i].S[j];
			v.TCoords.Y = triangles[i].T[j];

			v.Normal.X = triangles[i].VertexNormals[j][0];
			v.Normal.Y = triangles[i].VertexNormals[j][1];
			v.Normal.Z = triangles[i].VertexNormals[j][2];

			if(triangles[i].GroupIndex < groups.size() &&
					groups[triangles[i].GroupIndex].MaterialIdx < AnimatedMesh->getMeshBuffers().size())
				v.Color = AnimatedMesh->getMeshBuffers()[groups[triangles[i].GroupIndex].MaterialIdx]->Material.DiffuseColor;
			else
				v.Color.set(255,255,255,255);

			v.Pos.X = vertices[vertidx].Vertex[0];
			v.Pos.Y = vertices[vertidx].Vertex[1];
			v.Pos.Z = vertices[vertidx].Vertex[2];

			// check if we already have this vertex in our vertex array
			s32 index = -1;
			for (u32 iV = 0; iV < Vertices->size(); ++iV)
			{
				if (v == (*Vertices)[iV])
				{
					index = (s32)iV;
					break;
				}
			}

			if (index == -1)
			{
				index = Vertices->size();
				const u32 matidx = groups[triangles[i].GroupIndex].MaterialIdx;
				if (vertexWeights.size()==0)
				{
					const s32 boneid = vertices[vertidx].BoneID;
					if ((u32)boneid < AnimatedMesh->getAllJoints().size())
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						w->strength = 1.0f;
						w->vertex_id = index;
					}
				}
				else if (jointCount) // new weights from 1.8.x
				{
					f32 sum = 1.0f;
					s32 boneid = vertices[vertidx].BoneID;
					if (((u32)boneid < AnimatedMesh->getAllJoints().size()) && (vertexWeights[vertidx].weights[0] != 0))
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						sum -= (w->strength = vertexWeights[vertidx].weights[0]*weightFactor);
						w->vertex_id = index;
					}
					boneid = vertexWeights[vertidx].boneIds[0];
					if (((u32)boneid < AnimatedMesh->getAllJoints().size()) && (vertexWeights[vertidx].weights[1] != 0))
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						sum -= (w->strength = vertexWeights[vertidx].weights[1]*weightFactor);
						w->vertex_id = index;
					}
					boneid = vertexWeights[vertidx].boneIds[1];
					if (((u32)boneid < AnimatedMesh->getAllJoints().size()) && (vertexWeights[vertidx].weights[2] != 0))
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						sum -= (w->strength = vertexWeights[vertidx].weights[2]*weightFactor);
						w->vertex_id = index;
					}
					boneid = vertexWeights[vertidx].boneIds[2];
					if (((u32)boneid < AnimatedMesh->getAllJoints().size()) && (sum > 0.f))
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						w->strength = sum;
						w->vertex_id = index;
					}
					// fallback, if no bone chosen. Seems to be an error in the specs
					boneid = vertices[vertidx].BoneID;
					if ((sum == 1.f) && ((u32)boneid < AnimatedMesh->getAllJoints().size()))
					{
						ISkinnedMesh::SWeight *w=AnimatedMesh->addWeight(AnimatedMesh->getAllJoints()[boneid]);
						w->buffer_id = matidx;
						w->strength = 1.f;
						w->vertex_id = index;
					}
				}

				Vertices->push_back(v);
			}
			Indices.push_back(index);
		}
	}

	//create groups
	s32 iIndex = -1;
	for (i=0; i<groups.size(); ++i)
	{
		SGroup& grp = groups[i];

		if (grp.MaterialIdx >= AnimatedMesh->getMeshBuffers().size())
			grp.MaterialIdx = 0;

		core::array<u16>& indices = AnimatedMesh->getMeshBuffers()[grp.MaterialIdx]->Indices;

		for (u32 k=0; k < grp.VertexIds.size(); ++k)
			for (u32 l=0; l<3; ++l)
				indices.push_back(Indices[++iIndex]);
	}

	delete [] buffer;
	delete [] triangles;
	delete [] vertices;

	return true;
}


core::stringc CMS3DMeshFileLoader::stripPathFromString(const core::stringc& inString, bool returnPath) const
{
	s32 slashIndex=inString.findLast('/'); // forward slash
	s32 backSlash=inString.findLast('\\'); // back slash

	if (backSlash>slashIndex) slashIndex=backSlash;

	if (slashIndex==-1)//no slashes found
	{
		if (returnPath)
			return core::stringc(); //no path to return
		else
			return inString;
	}

	if (returnPath)
		return inString.subString(0, slashIndex + 1);
	else
		return inString.subString(slashIndex+1, inString.size() - (slashIndex+1));
}


} // end namespace scene
} // end namespace irr
/*


        	// debug border
	const s32 borderSkip =0;// debugBorders ? 0 : 1;
	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	 core::dimension2d<f32> stretchSize    =core::dimension2d<f32>(1.0f,1.0f);
     core::dimension2d<u32> maxVtxBlockSize=core::dimension2d<u32>(2,2);



	const u32 tm = os::Timer::getRealTime()/1000;
	const core::dimension2d<u32> hMapSize= core::dimension2d<u32>(256,256);
	const core::dimension2d<u32> tMapSize= core::dimension2d<u32>(256,256);
	const core::position2d<f32> thRel(static_cast<f32>(tMapSize.Width) / hMapSize.Width, static_cast<f32>(tMapSize.Height) / hMapSize.Height);
	float maxHeight = 0.5f; // height step per color value

	core::position2d<u32> processed(0,0);
	while (processed.Y<hMapSize.Height)
	{
		while(processed.X<hMapSize.Width)
		{
			core::dimension2d<u32> blockSize = maxVtxBlockSize;

			if (processed.X + blockSize.Width > hMapSize.Width)
				blockSize.Width = hMapSize.Width - processed.X;

			if (processed.Y + blockSize.Height > hMapSize.Height)
				blockSize.Height = hMapSize.Height - processed.Y;

			SMeshBuffer* buffer = new SMeshBuffer();
			buffer->setHardwareMappingHint(scene::EHM_STATIC);
			buffer->Vertices.reallocate(blockSize.getArea());
			// add vertices of vertex block
			u32 y;
			core::vector2df pos(0.f, processed.Y*stretchSize.Height);
			const core::vector2df bs(1.f/blockSize.Width, 1.f/blockSize.Height);
			core::vector2df tc(0.f, 0.5f*bs.Y);
			for (y=0; y<blockSize.Height; ++y)
			{
				pos.X=processed.X*stretchSize.Width;
				tc.X=0.5f*bs.X;
				for (u32 x=0; x<blockSize.Width; ++x)
				{
					const f32 height =1;

					vtx.Pos.set(pos.X, height, pos.Y);
					vtx.TCoords.set(tc);
					buffer->Vertices.push_back(vtx);
					pos.X += stretchSize.Width;
					tc.X += bs.X;
				}
				pos.Y += stretchSize.Height;
				tc.Y += bs.Y;
			}

			buffer->Indices.reallocate((blockSize.Height-1)*(blockSize.Width-1)*6);
			// add indices of vertex block
			s32 c1 = 0;
			for (y=0; y<blockSize.Height-1; ++y)
			{
				for (u32 x=0; x<blockSize.Width-1; ++x)
				{
					const s32 c = c1 + x;

					buffer->Indices.push_back(c);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1);

					buffer->Indices.push_back(c + 1);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1 + blockSize.Width);
				}
				c1 += blockSize.Width;
			}

			// recalculate normals
			for (u32 i=0; i<buffer->Indices.size(); i+=3)
			{
				const core::vector3df normal = core::plane3d<f32>(
					buffer->Vertices[buffer->Indices[i+0]].Pos,
					buffer->Vertices[buffer->Indices[i+1]].Pos,
					buffer->Vertices[buffer->Indices[i+2]].Pos).Normal;

				buffer->Vertices[buffer->Indices[i+0]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+1]].Normal = normal;
				buffer->Vertices[buffer->Indices[i+2]].Normal = normal;
			}

			if (buffer->Vertices.size())
			{
				c8 textureName[64];
				// create texture for this block


				video::IImage* img = new CImage(texture->getColorFormat(), core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));//driver->createImage(texture->getColorFormat(), core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));
				texture->copyTo(img, core::position2di(0,0), core::recti(
					core::position2d<s32>(core::floor32(processed.X*thRel.X), core::floor32(processed.Y*thRel.Y)),
					core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y))), 0);

				sprintf(textureName, "terrain%u_%u", tm, mesh->getMeshBufferCount());

				buffer->Material.setTexture(0, scene::Engine::Instance()->addTexture(textureName, img));

				if (buffer->Material.getTexture(0))
				{
					c8 tmp[255];
					sprintf(tmp, "Generated terrain texture (%ux%u): %s",
						buffer->Material.getTexture(0)->getSize().Width,
						buffer->Material.getTexture(0)->getSize().Height,
						textureName);
					os::Printer::log(tmp);
				}
				else
					os::Printer::log("Could not create terrain texture.", textureName, ELL_ERROR);


				img->drop();
			}

			buffer->recalculateBoundingBox();
			Mesh->addMeshBuffer(buffer);


			// keep on processing
			processed.X += maxVtxBlockSize.Width - borderSkip;
		}

		// keep on processing
		processed.X = 0;
		processed.Y += maxVtxBlockSize.Height - borderSkip;
	}

	Mesh->recalculateBoundingBox();
*/
