
#include "CSkinnedMesh.h"
#include "IGameObject.h"
#include "Engine.h"



namespace irr
{
namespace scene
{

IGeoTerrain::IGeoTerrain(core::vector3df size, s32 maxLOD, E_TERRAIN_PATCH_SIZE patchSize,float smoothFactor):
IComponent(),
	TerrainData(patchSize, maxLOD, core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 0.0f, 0.0f), size), RenderBuffer(0),
	VerticesToRender(0), IndicesToRender(0), DynamicSelectorUpdate(false),
	OverrideDistanceThreshold(false), UseDefaultRotationPivot(true), ForceRecalculation(true),
	FixedBorderLOD(-1),	CameraMovementDelta(10.0f), CameraRotationDelta(1.0f),CameraFOVDelta(0.1f),
	TCoordScale1(1.0f), TCoordScale2(1.0f), SmoothFactor(smoothFactor)
{

DebugDataVisible=0;

}

void IGeoTerrain::OnUpdate(u32 timeMs)
{

preRenderCalculationsIfNeeded();

}
void IGeoTerrain::OnRender(IShader* shader)
{

	if (!Mesh->getMeshBufferCount())
	{
    	     SDL_LogError(0,"[IGEOTERRAIN] Mesh error.");
			return;
	}

	scene::Engine* engine =scene::Engine::Instance();







        // For use with geomorphing


//	 SDL_Log("vtx: %d, idx: %d  %d  %d", vtxcnt, idxcnt,PrimitiveCount,IndicesToRender);
	engine->setShader(shader);
	shader->setMat4("model",core::IdentityMatrix);
	if (shader->findUniform("CameraPosition"))
	{
	 shader->setVec3("CameraPosition",engine->getCameraPosition());
	}


	//shader->setInt("uTextureUnit0",0);
	//shader->setInt("uTextureUnit1",1);
    //engine->setMaterial(RenderBuffer->getMaterial());
    engine->renderMeshBuffer(RenderBuffer,true);

    //engine->renderMesh(Mesh);



		//SDL_Log(" %d %d",IndicesToRender,RenderBuffer->Indices.allocated_size());


			if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
			{
			const s32 count = TerrainData.PatchCount * TerrainData.PatchCount;
			s32 visible = 0;


				engine->Draw3DBox(TerrainData.BoundingBox, video::SColor(255,255,0,0));
				for (s32 j = 0; j < count; ++j)
				{
					engine->Draw3DBox(TerrainData.Patches[j].BoundingBox, video::SColor(255,0,255,255));
					visible += (TerrainData.Patches[j].CurrentLOD >= 0);
				}

                        static u32 lastTime = 0;
                        const u32 now = os::Timer::getRealTime();
                        if (now - lastTime > 1000)
                        {
                            SDL_Log("Count: %d, Visible: %d", count, visible);
                            lastTime = now;
                        }


/*

                     u16* idx = reinterpret_cast<u16*>(RenderBuffer->getIndices());
                     int idxcnt = RenderBuffer->getIndexCount();

                    for (u32 i=0; i<idxcnt; i+=3)
                        {
                            const core::vector3df& v1 = RenderBuffer->getPosition(idx[i+0]);
                            const core::vector3df& v2 = RenderBuffer->getPosition(idx[i+1]);
                            const core::vector3df& v3 = RenderBuffer->getPosition(idx[i+2]);

                            //video::DrawTriangle3D(v1,v2,v3,SColor(255,0,0,255));
                            video::DrawTriangle3DLines(v1,v2,v3,SColor(255,255,0,255));
                            //engine->Draw3DTriangleLines(a1,a2,a3,SColor(255,0,0,255));
                            //engine->draw3DTriangle(v1,v2,v3,SColor(0,0,255,255));

                        }
*/

			}






ForceRecalculation = false;


}


	//! Apply transformation changes(scale, position, rotation)
	void IGeoTerrain::applyTransformation()
	{
		if (!Mesh->getMeshBufferCount())
			return;



		core::matrix4 rotMatrix;
		rotMatrix.setRotationDegrees(TerrainData.Rotation);

		const s32 vtxCount = Mesh->getMeshBuffer(0)->getVertexCount();
		for (s32 i = 0; i < vtxCount; ++i)
		{
			RenderBuffer->Vertices[i].Pos = Mesh->getMeshBuffer(0)->getPosition(i) * TerrainData.Scale + TerrainData.Position;

			RenderBuffer->Vertices[i].Pos -= TerrainData.RotationPivot;
			rotMatrix.inverseRotateVect(RenderBuffer->Vertices[i].Pos);
			RenderBuffer->Vertices[i].Pos += TerrainData.RotationPivot;
		}

		calculateDistanceThresholds(true);
		calculatePatchData();

		RenderBuffer->setDirty(EBT_VERTEX);
	}

void IGeoTerrain::free()
{
        delete [] TerrainData.Patches;

		if (Mesh)
			Mesh->drop();

		if (RenderBuffer)
			RenderBuffer->drop();
}
void IGeoTerrain::init()
{

        Mesh = new SMesh();



		RenderBuffer = new SMeshBufferLightMap();
//        RenderBuffer = new SMeshBuffer();





		RenderBuffer->setHardwareMappingHint(scene::EHM_STATIC, scene::EBT_VERTEX);
		RenderBuffer->setHardwareMappingHint(scene::EHM_DYNAMIC, scene::EBT_INDEX);
		FileSystem= scene::Engine::Instance()->getFileSystem();


}

	//! Initializes the terrain data. Loads the vertices from the heightMapFile
	bool IGeoTerrain::loadHeightMap(const io::path& heightMapFileName, video::SColor vertexColor)
	{





	   FileSystem= scene::Engine::Instance()->getFileSystem();




		Mesh->MeshBuffers.clear();
		const u32 startTime = os::Timer::getRealTime();
		video::IImage* heightMap = scene::Engine::Instance()->createImageFromFile(heightMapFileName);

		if (!heightMap)
		{
			os::Printer::log("Could not load terrain, because file could not be opened.",heightMapFileName, ELL_ERROR);
			return false;
		}
		//SDL_Log("[IGEOTERRAIN]  size %d x %d",heightMap->getDimension().Width,heightMap->getDimension().Height);

		if (heightMap->getDimension().Width<=0 || heightMap->getDimension().Height<=0)
		{
		os::Printer::log("Could not load terrain, because terrain size.",heightMapFileName, ELL_ERROR);
		return false;
		}




		// Get the dimension of the heightmap data
		TerrainData.Size = heightMap->getDimension().Width;

		switch (TerrainData.PatchSize)
		{
			case ETPS_9:
				if (TerrainData.MaxLOD > 3)
				{
					TerrainData.MaxLOD = 3;
				}
			break;
			case ETPS_17:
				if (TerrainData.MaxLOD > 4)
				{
					TerrainData.MaxLOD = 4;
				}
			break;
			case ETPS_33:
				if (TerrainData.MaxLOD > 5)
				{
					TerrainData.MaxLOD = 5;
				}
			break;
			case ETPS_65:
				if (TerrainData.MaxLOD > 6)
				{
					TerrainData.MaxLOD = 6;
				}
			break;
			case ETPS_129:
				if (TerrainData.MaxLOD > 7)
				{
					TerrainData.MaxLOD = 7;
				}
			break;
		}

		// --- Generate vertex data from heightmap ----
		// resize the vertex array for the mesh buffer one time (makes loading faster)
		scene::SMeshBufferLightMap *mb=0;
//		scene::SMeshBuffer *mb=0;




		const u32 numVertices = TerrainData.Size * TerrainData.Size;

		if (numVertices <= 65536)
		{
			//small enough for 16bit buffers
			mb=new scene::SMeshBufferLightMap();
            //mb=new scene::SMeshBuffer();
		//	RenderBuffer->getIndexBuffer().setType(video::EIT_16BIT);
		}
		else
		{
			//we need 32bit buffers
			SDL_LogError(0,"[IGEOTERRAIN] Engine son't suport 32 indices");
			mb=new scene::SMeshBufferLightMap();
            //mb=new scene::SMeshBuffer();
		//	RenderBuffer->getIndexBuffer().setType(video::EIT_32BIT);
		}




		mb->Vertices.set_used(numVertices);




		// Read the heightmap to get the vertex data
		// Apply positions changes, scaling changes
		const f32 tdSize = 1.0f/(f32)(TerrainData.Size-1);
		s32 index = 0;
		float fx=0.f;
		float fx2=0.f;

		for (s32 x = 0; x < TerrainData.Size; ++x)
		{
			float fz=0.f;
			float fz2=0.f;
			for (s32 z = 0; z < TerrainData.Size; ++z)
			{
				video::S3DVertex2TCoords& vertex= static_cast<video::S3DVertex2TCoords*>(mb->getVertices())[index++];
				//video::S3DVertex& vertex= static_cast<video::S3DVertex*>(mb->getVertices())[index++];
				vertex.Normal.set(0.0f, 1.0f, 0.0f);
				vertex.Color = vertexColor;




				video::SColor pixel =heightMap->getPixel(x,z);
				//float  hi =   ((pixel.getRed() / 255.0f) + (pixel.getGreen() / 255.0f) + (pixel.getBlue() / 255.0f)) / 3.0f *100.0f;


				vertex.Pos.X = fx;
				vertex.Pos.Y = (f32) heightMap->getPixel(TerrainData.Size-x-1,z).getLightness();

			//	vertex.Pos.Y =hi;

				//const f32 height = heightmap->getPixel(x+processed.X, y+processed.Y).getAverage() * maxHeight;

				vertex.Pos.Z = fz;



				vertex.TCoords.X = 1.0f-fx2;
				vertex.TCoords.Y = fz2;

				vertex.TCoords2.X = 1.0f-fx2;
				vertex.TCoords2.Y = fz2;


				++fz;
				fz2 += tdSize;
			}
			++fx;
			fx2 += tdSize;
		}

		// drop heightMap, no longer needed

		heightMap->drop();


		smoothTerrain(mb, SmoothFactor);

		// calculate smooth normals for the vertices

		calculateNormals(mb);

		// add the MeshBuffer to the mesh
		Mesh->addMeshBuffer(mb);



		// We copy the data to the renderBuffer, after the normals have been calculated.
		RenderBuffer->Vertices.set_used(numVertices);

		for (u32 i = 0; i < numVertices; ++i)
		{
		    RenderBuffer->Vertices[i] = mb->Vertices[i];
			RenderBuffer->Vertices[i].Pos *= TerrainData.Scale;
			RenderBuffer->Vertices[i].Pos += TerrainData.Position;
		}






		scaleTexture(1.0f,20.0f);





		// calculate all the necessary data for the patches and the terrain

		calculateDistanceThresholds();
		createPatches();
		calculatePatchData();

		// set the default rotation pivot point to the terrain nodes center
		TerrainData.RotationPivot = TerrainData.Center;

		// Rotate the vertices of the terrain by the rotation
		// specified. Must be done after calculating the terrain data,
		// so we know what the current center of the terrain is.
		setRotation(TerrainData.Rotation);

		// Pre-allocate memory for indices

		RenderBuffer->Indices.set_used(TerrainData.PatchCount * TerrainData.PatchCount *	TerrainData.CalcPatchSize * TerrainData.CalcPatchSize * 6);


		RenderBuffer->setDirty();




		const u32 endTime = os::Timer::getRealTime();

        applyTransformation();

		preRenderCalculationsIfNeeded();




    	SDL_Log("Generated terrain data (%dx%d) in %.4f seconds",TerrainData.Size, TerrainData.Size, (endTime - startTime) / 1000.0f );
    	scene::Engine::Instance()->testGLError(__LINE__);

		return true;
	}


	bool IGeoTerrain::preRenderCalculationsIfNeeded()
	{

		// Determine the camera rotation, based on the camera direction.
		const core::vector3df cameraPosition = scene::Engine::Instance()->getCameraPosition();
		const core::vector3df cameraRotation =scene::Engine::Instance()->getViewMatrix().getRotationDegrees();// core::line3d<f32>(cameraPosition,scene::Engine::Instance()->getCameraTarget()).getVector().getHorizontalAngle();
		core::vector3df cameraUp = scene::Engine::Instance()->getCameraUp();
		cameraUp.normalize();
		const f32 CameraFOV = scene::Engine::Instance()->getCameraFov();

		// Only check on the Camera's Y Rotation
		if (!ForceRecalculation)
		{
			if ((fabsf(cameraRotation.X - OldCameraRotation.X) < CameraRotationDelta) &&
				(fabsf(cameraRotation.Y - OldCameraRotation.Y) < CameraRotationDelta))
			{
				if ((fabs(cameraPosition.X - OldCameraPosition.X) < CameraMovementDelta) &&
					(fabs(cameraPosition.Y - OldCameraPosition.Y) < CameraMovementDelta) &&
					(fabs(cameraPosition.Z - OldCameraPosition.Z) < CameraMovementDelta))
				{
					if (fabs(CameraFOV-OldCameraFOV) < CameraFOVDelta &&
						cameraUp.dotProduct(OldCameraUp) > (1.f - (cos(core::DEGTORAD * CameraRotationDelta))))
					{
						return false;
					}
				}
			}
		}

		//we need to redo calculations...


		OldCameraPosition = cameraPosition;
		OldCameraRotation = cameraRotation;
		OldCameraUp = cameraUp;
		OldCameraFOV = CameraFOV;

		preRenderLODCalculations();
		preRenderIndicesCalculations();
		return true;
	}

	bool IGeoTerrain::preRenderLODCalculations()
	{


		const core::vector3df cameraPosition = scene::Engine::Instance()->getCameraPosition();
		const SViewFrustum frustum = scene::Engine::Instance()->getViewFrustum();
		if (!frustum.getBoundingBox().intersectsWithBox(TerrainData.BoundingBox)) return false;


		// Determine each patches LOD based on distance from camera (and whether or not they are in
		// the view frustum).
		const s32 count = TerrainData.PatchCount * TerrainData.PatchCount;
		for (s32 j = 0; j < count; ++j)
		{
			if (frustum.getBoundingBox().intersectsWithBox(TerrainData.Patches[j].BoundingBox))
			{
				const f32 distance = cameraPosition.getDistanceFromSQ(TerrainData.Patches[j].Center);



				if ( FixedBorderLOD >= 0 )
				{
					TerrainData.Patches[j].CurrentLOD = FixedBorderLOD;
					if (j < TerrainData.PatchCount
						|| j >= (count - TerrainData.PatchCount)
						|| (j % TerrainData.PatchCount) == 0
						|| (j % TerrainData.PatchCount) == TerrainData.PatchCount-1)
						continue;
				}

				TerrainData.Patches[j].CurrentLOD = 0;

				for (s32 i = TerrainData.MaxLOD - 1; i>0; --i)
				{
					if (distance >= TerrainData.LODDistanceThreshold[i])
					{
						TerrainData.Patches[j].CurrentLOD = i;
						break;
					}
				}
			}
			else
			{
				TerrainData.Patches[j].CurrentLOD = -1;
			}
		}
		return true;
	}


	void IGeoTerrain::preRenderIndicesCalculations()
	{

		IndicesToRender = 0;
		RenderBuffer->Indices.set_used(0);

		s32 index = 0;
		// Then generate the indices for all patches that are visible.
		for (s32 i = 0; i < TerrainData.PatchCount; ++i)
		{
			for (s32 j = 0; j < TerrainData.PatchCount; ++j)
			{
				if (TerrainData.Patches[index].CurrentLOD >= 0)
				{
					s32 x = 0;
					s32 z = 0;

					// calculate the step we take this patch, based on the patches current LOD
					const s32 step = 1 << TerrainData.Patches[index].CurrentLOD;

					// Loop through patch and generate indices
					while (z < TerrainData.CalcPatchSize)
					{
						const s32 index11 = getIndex(j, i, index, x, z);
						const s32 index21 = getIndex(j, i, index, x + step, z);
						const s32 index12 = getIndex(j, i, index, x, z + step);
						const s32 index22 = getIndex(j, i, index, x + step, z + step);

						RenderBuffer->Indices.push_back(index12);
						RenderBuffer->Indices.push_back(index11);
						RenderBuffer->Indices.push_back(index22);
						RenderBuffer->Indices.push_back(index22);
						RenderBuffer->Indices.push_back(index11);
						RenderBuffer->Indices.push_back(index21);
						IndicesToRender+=6;

						// increment index position horizontally
						x += step;

						// we've hit an edge
						if (x >= TerrainData.CalcPatchSize)
						{
							x = 0;
							z += step;
						}
					}
				}
				++index;
			}
		}


		RenderBuffer->setDirty(EBT_INDEX);


	}

	//! calculate smooth normals
	void IGeoTerrain::calculateNormals(SMeshBufferLightMap* mb)
	//void IGeoTerrain::calculateNormals(SMeshBuffer* mb)
	{
		s32 count;
		core::vector3df a, b, c, t;

		for (s32 x=0; x<TerrainData.Size; ++x)
		{
			for (s32 z=0; z<TerrainData.Size; ++z)
			{
				count = 0;
				core::vector3df normal;

				// top left
				if (x>0 && z>0)
				{
					a = mb->Vertices[(x-1)*TerrainData.Size+z-1].Pos;
					b = mb->Vertices[(x-1)*TerrainData.Size+z].Pos;
					c = mb->Vertices[x*TerrainData.Size+z].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					a = mb->Vertices[(x-1)*TerrainData.Size+z-1].Pos;
					b = mb->Vertices[x*TerrainData.Size+z-1].Pos;
					c = mb->Vertices[x*TerrainData.Size+z].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					count += 2;
				}

				// top right
				if (x>0 && z<TerrainData.Size-1)
				{
					a = mb->Vertices[(x-1)*TerrainData.Size+z].Pos;
					b = mb->Vertices[(x-1)*TerrainData.Size+z+1].Pos;
					c = mb->Vertices[x*TerrainData.Size+z+1].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					a = mb->Vertices[(x-1)*TerrainData.Size+z].Pos;
					b = mb->Vertices[x*TerrainData.Size+z+1].Pos;
					c = mb->Vertices[x*TerrainData.Size+z].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					count += 2;
				}

				// bottom right
				if (x<TerrainData.Size-1 && z<TerrainData.Size-1)
				{
					a = mb->Vertices[x*TerrainData.Size+z+1].Pos;
					b = mb->Vertices[x*TerrainData.Size+z].Pos;
					c = mb->Vertices[(x+1)*TerrainData.Size+z+1].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					a = mb->Vertices[x*TerrainData.Size+z+1].Pos;
					b = mb->Vertices[(x+1)*TerrainData.Size+z+1].Pos;
					c = mb->Vertices[(x+1)*TerrainData.Size+z].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					count += 2;
				}

				// bottom left
				if (x<TerrainData.Size-1 && z>0)
				{
					a = mb->Vertices[x*TerrainData.Size+z-1].Pos;
					b = mb->Vertices[x*TerrainData.Size+z].Pos;
					c = mb->Vertices[(x+1)*TerrainData.Size+z].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					a = mb->Vertices[x*TerrainData.Size+z-1].Pos;
					b = mb->Vertices[(x+1)*TerrainData.Size+z].Pos;
					c = mb->Vertices[(x+1)*TerrainData.Size+z-1].Pos;
					b -= a;
					c -= a;
					t = b.crossProduct(c);
					t.normalize();
					normal += t;

					count += 2;
				}

				if (count != 0)
				{
					normal.normalize();
				}
				else
				{
					normal.set(0.0f, 1.0f, 0.0f);
				}

				mb->Vertices[x * TerrainData.Size + z].Normal = normal;
			}
		}
			//	mb->setDirty(EBT_VERTEX);
	}


	//! create patches, stuff that needs to be done only once for patches goes here.
	void IGeoTerrain::createPatches()
	{
		TerrainData.PatchCount = (TerrainData.Size - 1) / (TerrainData.CalcPatchSize);

		if (TerrainData.Patches)
			delete [] TerrainData.Patches;

		TerrainData.Patches = new SPatch[TerrainData.PatchCount * TerrainData.PatchCount];
	}



	//! smooth the terrain
	void IGeoTerrain::smoothTerrain(SMeshBufferLightMap* mb, s32 smoothFactor)
	//void IGeoTerrain::smoothTerrain(SMeshBuffer* mb, s32 smoothFactor)
	{

		for (s32 run = 0; run < smoothFactor; ++run)
		{
			s32 yd = TerrainData.Size;
			for (s32 y = 1; y < TerrainData.Size - 1; ++y)
			{
				for (s32 x = 1; x < TerrainData.Size - 1; ++x)
				{
					mb->Vertices[x + yd].Pos.Y =
						(mb->Vertices[x-1 + yd].Pos.Y + //left
						mb->Vertices[x+1 + yd].Pos.Y + //right
						mb->Vertices[x + yd - TerrainData.Size].Pos.Y + //above
						mb->Vertices[x + yd + TerrainData.Size].Pos.Y) * 0.25f; //below
				}
				yd += TerrainData.Size;
			}
		}
	//	mb->setDirty(EBT_VERTEX);

	}
	//! used to calculate the internal STerrainData structure both at creation and after scaling/position calls.
	void IGeoTerrain::calculatePatchData()
	{
		// Reset the Terrains Bounding Box for re-calculation
		TerrainData.BoundingBox.reset(RenderBuffer->getPosition(0));

		for (s32 x = 0; x < TerrainData.PatchCount; ++x)
		{
			for (s32 z = 0; z < TerrainData.PatchCount; ++z)
			{
				const s32 index = x * TerrainData.PatchCount + z;
				SPatch& patch = TerrainData.Patches[index];
				patch.CurrentLOD = 0;

				const s32 xstart = x*TerrainData.CalcPatchSize;
				const s32 xend = xstart+TerrainData.CalcPatchSize;
				const s32 zstart = z*TerrainData.CalcPatchSize;
				const s32 zend = zstart+TerrainData.CalcPatchSize;
				// For each patch, calculate the bounding box (mins and maxes)
				patch.BoundingBox.reset(RenderBuffer->getPosition(xstart*TerrainData.Size + zstart));

				for (s32 xx = xstart; xx <= xend; ++xx)
					for (s32 zz = zstart; zz <= zend; ++zz)
						patch.BoundingBox.addInternalPoint(RenderBuffer->Vertices[xx * TerrainData.Size + zz].Pos);

				// Reconfigure the bounding box of the terrain as a whole
				TerrainData.BoundingBox.addInternalBox(patch.BoundingBox);

				// get center of Patch
				patch.Center = patch.BoundingBox.getCenter();

				// Assign Neighbours
				// Top
				if (x > 0)
					patch.Top = &TerrainData.Patches[(x-1) * TerrainData.PatchCount + z];
				else
					patch.Top = 0;

				// Bottom
				if (x < TerrainData.PatchCount - 1)
					patch.Bottom = &TerrainData.Patches[(x+1) * TerrainData.PatchCount + z];
				else
					patch.Bottom = 0;

				// Left
				if (z > 0)
					patch.Left = &TerrainData.Patches[x * TerrainData.PatchCount + z - 1];
				else
					patch.Left = 0;

				// Right
				if (z < TerrainData.PatchCount - 1)
					patch.Right = &TerrainData.Patches[x * TerrainData.PatchCount + z + 1];
				else
					patch.Right = 0;
			}
		}

		// get center of Terrain
		TerrainData.Center = TerrainData.BoundingBox.getCenter();

		// if the default rotation pivot is still being used, update it.
		if (UseDefaultRotationPivot)
		{
			TerrainData.RotationPivot = TerrainData.Center;
		}
	}

	//! used to calculate or recalculate the distance thresholds
	void IGeoTerrain::calculateDistanceThresholds(bool scalechanged)
	{
		// Only update the LODDistanceThreshold if it's not manually changed
		if (!OverrideDistanceThreshold)
		{
			TerrainData.LODDistanceThreshold.set_used(0);
			// Determine new distance threshold for determining what LOD to draw patches at
			TerrainData.LODDistanceThreshold.reallocate(TerrainData.MaxLOD);

			const f64 size = TerrainData.PatchSize * TerrainData.PatchSize *
					TerrainData.Scale.X * TerrainData.Scale.Z;
			for (s32 i=0; i<TerrainData.MaxLOD; ++i)
			{
				TerrainData.LODDistanceThreshold.push_back(size * ((i+1+ i / 2) * (i+1+ i / 2)));
			}
		}
	}
void IGeoTerrain::setCurrentLODOfPatches(s32 lod)
	{
		const s32 count = TerrainData.PatchCount * TerrainData.PatchCount;
		for (s32 i=0; i< count; ++i)
			TerrainData.Patches[i].CurrentLOD = lod;
	}


	void IGeoTerrain::setCurrentLODOfPatches(const core::array<s32>& lodarray)
	{
		const s32 count = TerrainData.PatchCount * TerrainData.PatchCount;
		for (s32 i=0; i<count; ++i)
			TerrainData.Patches[i].CurrentLOD = lodarray[i];
	}

	void IGeoTerrain::smoothTerrain( s32 smoothFactor)
	{
        for (s32 run = 0; run < smoothFactor; ++run)
            {
                s32 yd = TerrainData.Size;
                for (s32 y = 1; y < TerrainData.Size - 1; ++y)
                {
                    for (s32 x = 1; x < TerrainData.Size - 1; ++x)
                    {



                    RenderBuffer->Vertices[x + yd].Pos.Y =
                            (RenderBuffer->Vertices[x-1 + yd].Pos.Y + //left
                            RenderBuffer->Vertices[x+1 + yd].Pos.Y + //right
                            RenderBuffer->Vertices[x + yd - TerrainData.Size].Pos.Y + //above
                            RenderBuffer->Vertices[x + yd + TerrainData.Size].Pos.Y) * 0.25f; //below*/


                    }
                    yd += TerrainData.Size;
                }
            }
            RenderBuffer->setDirty(EBT_VERTEX);
	}

	//! Creates a planar texture mapping on the terrain
	//! \param resolution: resolution of the planar mapping. This is the value
	//! specifying the relation between world space and texture coordinate space.
	void IGeoTerrain::scaleTexture(f32 resolution, f32 resolution2)
	{
		TCoordScale1 = resolution;
		TCoordScale2 = resolution2;

		const f32 resBySize = resolution / (f32)(TerrainData.Size-1);
		const f32 res2BySize = resolution2 / (f32)(TerrainData.Size-1);
		u32 index = 0;
		f32 xval = 0.f;
		f32 x2val = 0.f;
		for (s32 x=0; x<TerrainData.Size; ++x)
		{
			f32 zval=0.f;
			f32 z2val=0.f;
			for (s32 z=0; z<TerrainData.Size; ++z)
			{
				RenderBuffer->Vertices[index].TCoords.X = 1.f-xval;
				RenderBuffer->Vertices[index].TCoords.Y = zval;

				if (RenderBuffer->getVertexType()==video::EVT_2TCOORDS)
				{
					if (resolution2 == 0)
					{
						((video::S3DVertex2TCoords&)RenderBuffer->Vertices[index]).TCoords2 = RenderBuffer->Vertices[index].TCoords;
					}
					else
					{
						((video::S3DVertex2TCoords&)RenderBuffer->Vertices[index]).TCoords2.X = 1.f-x2val;
						((video::S3DVertex2TCoords&)RenderBuffer->Vertices[index]).TCoords2.Y = z2val;
					}
				}

				++index;
				zval += resBySize;
				z2val += res2BySize;
			}
			xval += resBySize;
			x2val += res2BySize;
		}

		RenderBuffer->setDirty(EBT_VERTEX);
	}

//! Gets the height
	f32 IGeoTerrain::getHeight(f32 x, f32 z) const
	{
		if (!Mesh->getMeshBufferCount())
			return 0;

		core::matrix4 rotMatrix;
		rotMatrix.setRotationDegrees(TerrainData.Rotation);
		core::vector3df pos(x, 0.0f, z);
		rotMatrix.rotateVect(pos);
		pos -= TerrainData.Position;
		pos /= TerrainData.Scale;

		s32 X(core::floor32(pos.X));
		s32 Z(core::floor32(pos.Z));

		f32 height = -FLT_MAX;
		if (X >= 0 && X < TerrainData.Size-1 &&
				Z >= 0 && Z < TerrainData.Size-1)
		{
			const video::S3DVertex2TCoords* Vertices = (const video::S3DVertex2TCoords*)Mesh->getMeshBuffer(0)->getVertices();
			//const video::S3DVertex* Vertices = (const video::S3DVertex*)Mesh->getMeshBuffer(0)->getVertices();

			const core::vector3df& a = Vertices[X * TerrainData.Size + Z].Pos;
			const core::vector3df& b = Vertices[(X + 1) * TerrainData.Size + Z].Pos;
			const core::vector3df& c = Vertices[X * TerrainData.Size + (Z + 1)].Pos;
			const core::vector3df& d = Vertices[(X + 1) * TerrainData.Size + (Z + 1)].Pos;

			// offset from integer position
			const f32 dx = pos.X - X;
			const f32 dz = pos.Z - Z;

			if (dx > dz)
				height = a.Y + (d.Y - b.Y)*dz + (b.Y - a.Y)*dx;
			else
				height = a.Y + (d.Y - c.Y)*dx + (c.Y - a.Y)*dz;

			height *= TerrainData.Scale.Y;
			height += TerrainData.Position.Y;
		}

		return height;
	}

	//! used to get the indices when generating index data for patches at varying levels of detail.
	u32 IGeoTerrain::getIndex(const s32 PatchX, const s32 PatchZ,					const s32 PatchIndex, u32 vX, u32 vZ) const
	{
		// top border
		if (vZ == 0)
		{
			if (TerrainData.Patches[PatchIndex].Top &&
				TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Top->CurrentLOD &&
				(vX % (1 << TerrainData.Patches[PatchIndex].Top->CurrentLOD)) != 0 )
			{
				vX -= vX % (1 << TerrainData.Patches[PatchIndex].Top->CurrentLOD);
			}
		}
		else
		if (vZ == (u32)TerrainData.CalcPatchSize) // bottom border
		{
			if (TerrainData.Patches[PatchIndex].Bottom &&
				TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Bottom->CurrentLOD &&
				(vX % (1 << TerrainData.Patches[PatchIndex].Bottom->CurrentLOD)) != 0)
			{
				vX -= vX % (1 << TerrainData.Patches[PatchIndex].Bottom->CurrentLOD);
			}
		}

		// left border
		if (vX == 0)
		{
			if (TerrainData.Patches[PatchIndex].Left &&
				TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Left->CurrentLOD &&
				(vZ % (1 << TerrainData.Patches[PatchIndex].Left->CurrentLOD)) != 0)
			{
				vZ -= vZ % (1 << TerrainData.Patches[PatchIndex].Left->CurrentLOD);
			}
		}
		else
		if (vX == (u32)TerrainData.CalcPatchSize) // right border
		{
			if (TerrainData.Patches[PatchIndex].Right &&
				TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Right->CurrentLOD &&
				(vZ % (1 << TerrainData.Patches[PatchIndex].Right->CurrentLOD)) != 0)
			{
				vZ -= vZ % (1 << TerrainData.Patches[PatchIndex].Right->CurrentLOD);
			}
		}

		if (vZ >= (u32)TerrainData.PatchSize)
			vZ = TerrainData.CalcPatchSize;

		if (vX >= (u32)TerrainData.PatchSize)
			vX = TerrainData.CalcPatchSize;

		return (vZ + ((TerrainData.CalcPatchSize) * PatchZ)) * TerrainData.Size +
			(vX + ((TerrainData.CalcPatchSize) * PatchX));
	}



	//! Gets the indices for a specified patch at a specified Level of Detail.
	//! \param mb: A reference to an array of u32 indices.
	//! \param patchX: Patch x coordinate.
	//! \param patchZ: Patch z coordinate.
	//! \param LOD: The level of detail to get for that patch. If -1, then get
	//! the CurrentLOD. If the CurrentLOD is set to -1, meaning it's not shown,
	//! then it will retrieve the triangles at the highest LOD (0).
	//! \return: Number if indices put into the buffer.
	s32 IGeoTerrain::getIndicesForPatch(core::array<u32>& indices, s32 patchX, s32 patchZ, s32 LOD)
	{
		if (patchX < 0 || patchX > TerrainData.PatchCount-1 ||
				patchZ < 0 || patchZ > TerrainData.PatchCount-1)
			return -1;

		if (LOD < -1 || LOD > TerrainData.MaxLOD - 1)
			return -1;

		core::array<s32> cLODs;
		bool setLODs = false;

		// If LOD of -1 was passed in, use the CurrentLOD of the patch specified
		if (LOD == -1)
		{
			LOD = TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].CurrentLOD;
		}
		else
		{
			getCurrentLODOfPatches(cLODs);
			setCurrentLODOfPatches(LOD);
			setLODs = true;
		}

		if (LOD < 0)
			return -2; // Patch not visible, don't generate indices.

		// calculate the step we take for this LOD
		const s32 step = 1 << LOD;

		// Generate the indices for the specified patch at the specified LOD
		const s32 index = patchX * TerrainData.PatchCount + patchZ;

		s32 x = 0;
		s32 z = 0;

		indices.set_used(TerrainData.PatchSize * TerrainData.PatchSize * 6);

		// Loop through patch and generate indices
		s32 rv=0;
		while (z<TerrainData.CalcPatchSize)
		{
			const s32 index11 = getIndex(patchZ, patchX, index, x, z);
			const s32 index21 = getIndex(patchZ, patchX, index, x + step, z);
			const s32 index12 = getIndex(patchZ, patchX, index, x, z + step);
			const s32 index22 = getIndex(patchZ, patchX, index, x + step, z + step);

			indices[rv++] = index12;
			indices[rv++] = index11;
			indices[rv++] = index22;
			indices[rv++] = index22;
			indices[rv++] = index11;
			indices[rv++] = index21;

			// increment index position horizontally
			x += step;

			if (x >= TerrainData.CalcPatchSize) // we've hit an edge
			{
				x = 0;
				z += step;
			}
		}

		if (setLODs)
			setCurrentLODOfPatches(cLODs);

		return rv;
	}


	//! Populates an array with the CurrentLOD of each patch.
	//! \param LODs: A reference to a core::array<s32> to hold the values
	//! \return Returns the number of elements in the array
	s32 IGeoTerrain::getCurrentLODOfPatches(core::array<s32>& LODs) const
	{
		s32 numLODs;
		LODs.clear();

		const s32 count = TerrainData.PatchCount * TerrainData.PatchCount;
		for (numLODs = 0; numLODs < count; numLODs++)
			LODs.push_back(TerrainData.Patches[numLODs].CurrentLOD);

		return LODs.size();
	}


	//! Manually sets the LOD of a patch
	//! \param patchX: Patch x coordinate.
	//! \param patchZ: Patch z coordinate.
	//! \param LOD: The level of detail to set the patch to.
	void IGeoTerrain::setLODOfPatch(s32 patchX, s32 patchZ, s32 LOD)
	{
		TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].CurrentLOD = LOD;
	}


	//! Override the default generation of distance thresholds for determining the LOD a patch
	//! is rendered at.
	bool IGeoTerrain::overrideLODDistance(s32 LOD, f64 newDistance)
	{
		OverrideDistanceThreshold = true;

		if (LOD < 0 || LOD > TerrainData.MaxLOD - 1)
			return false;

		TerrainData.LODDistanceThreshold[LOD] = newDistance * newDistance;

		return true;
	}

	//! Return the bounding box of the entire terrain.
	const core::aabbox3d<f32>& IGeoTerrain::getBoundingBox() const
	{
		return TerrainData.BoundingBox;
	}


	//! Return the bounding box of a patch
	const core::aabbox3d<f32>& IGeoTerrain::getBoundingBox(s32 patchX, s32 patchZ) const
	{
		return TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].BoundingBox;
	}


	//! Gets the meshbuffer data based on a specified Level of Detail.
	//! \param mb: A reference to an SMeshBuffer object
	//! \param LOD: The Level Of Detail you want the indices from.
	void IGeoTerrain::getMeshBufferForLOD(SMeshBufferLightMap& mb, s32 LOD ) const
	//void IGeoTerrain::getMeshBufferForLOD(SMeshBuffer& mb, s32 LOD ) const

	{
		if (!Mesh->getMeshBufferCount())
			return;

		LOD = core::clamp(LOD, 0, TerrainData.MaxLOD - 1);

		const u32 numVertices = Mesh->getMeshBuffer(0)->getVertexCount();
		mb.Vertices.reallocate(numVertices);
		video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)Mesh->getMeshBuffer(0)->getVertices();
		//video::S3DVertex* vertices = (video::S3DVertex*)Mesh->getMeshBuffer(0)->getVertices();

		for (u32 n=0; n<numVertices; ++n)
			mb.Vertices.push_back(vertices[n]);



		// calculate the step we take for all patches, since LOD is the same
		const s32 step = 1 << LOD;

		// Generate the indices for all patches at the specified LOD
		s32 index = 0;
		for (s32 i=0; i<TerrainData.PatchCount; ++i)
		{
			for (s32 j=0; j<TerrainData.PatchCount; ++j)
			{
				s32 x = 0;
				s32 z = 0;

				// Loop through patch and generate indices
				while (z < TerrainData.CalcPatchSize)
				{
					const s32 index11 = getIndex(j, i, index, x, z);
					const s32 index21 = getIndex(j, i, index, x + step, z);
					const s32 index12 = getIndex(j, i, index, x, z + step);
					const s32 index22 = getIndex(j, i, index, x + step, z + step);

					mb.Indices.push_back(index12);
					mb.Indices.push_back(index11);
					mb.Indices.push_back(index22);
					mb.Indices.push_back(index22);
					mb.Indices.push_back(index11);
					mb.Indices.push_back(index21);

					// increment index position horizontally
					x += step;

					if (x >= TerrainData.CalcPatchSize) // we've hit an edge
					{
						x = 0;
						z += step;
					}
				}
				++index;
			}
		}
	}


		//! Sets the scale of the scene node.
	//! \param scale: New scale of the node
	void IGeoTerrain::setScale(const core::vector3df& scale)
	{
		TerrainData.Scale = scale;
		applyTransformation();
		calculateNormals(RenderBuffer);
		ForceRecalculation = true;
	}


	//! Sets the rotation of the node. This only modifies
	//! the relative rotation of the node.
	//! \param rotation: New rotation of the node in degrees.
	void IGeoTerrain::setRotation(const core::vector3df& rotation)
	{
		TerrainData.Rotation = rotation;
		applyTransformation();
		ForceRecalculation = true;
	}


	//! Sets the pivot point for rotation of this node. This is useful for the TiledTerrainManager to
	//! rotate all terrain tiles around a global world point.
	//! NOTE: The default for the RotationPivot will be the center of the individual tile.
	void IGeoTerrain::setRotationPivot(const core::vector3df& pivot)
	{
		UseDefaultRotationPivot = false;
		TerrainData.RotationPivot = pivot;
	}


	//! Sets the position of the node.
	//! \param newpos: New postition of the scene node.
	void IGeoTerrain::setPosition(const core::vector3df& newpos)
	{
		TerrainData.Position = newpos;
		applyTransformation();
		ForceRecalculation = true;
	}
}
}
