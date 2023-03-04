
#include "CSkinnedMesh.h"
#include "IGameObject.h"
#include "Engine.h"



namespace irr
{
namespace scene
{


IMeshRenderer::IMeshRenderer(scene::IMesh* mesh):IComponent(),Mesh(mesh)
{

setPriority(100);
DebugDataVisible=0;

Materials.clear();

if (Mesh)
{
    video::SMaterial mat;

    for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
    {
        IMeshBuffer* mb = Mesh->getMeshBuffer(i);
        if (mb)
            mat = mb->getMaterial();

        Materials.push_back(mat);
    }
}

}

video::SMaterial& IMeshRenderer::getMaterial(u32 i)
{

	if (i >= Materials.size())
		i=Materials.size();
	if (i <= 0)
		i=0;


	return Materials[i];
}

u32 IMeshRenderer::getMaterialCount() const
{
	return Materials.size();
}
const core::aabbox3d<f32>& IMeshRenderer::getBoundingBox() const
{
	return Mesh ? Mesh->getBoundingBox() : Box;
}
/*
    std::shared_ptr<IJointValue> value = m_parent->getComponent<IJointValue>();

             if (value->get())
             {

             }

             std::shared_ptr<IAnimator> animator = m_parent->getComponent<IAnimator>();
                    if (animator.get())
                    {
                        m = animator.get()->getMeshForCurrentFrame();
                        RenderFromIdentity=animator.get()->getRenderFromIdentity();
                    }
*/

void IMeshRenderer::OnRender(IShader* shader)
{
//if (!Mesh || Mesh->getMeshType() != EAMT_SKINNED)
//return;
scene::Engine* engine= scene::Engine::Instance();
if (!Mesh || !engine) return;


    Box = Mesh->getBoundingBox();

    core::matrix4 model = m_parent->Transform->getAbsoluteTransformation();
    video::SMaterial mat;

	if (DebugDataVisible & scene::EDS_BBOX)
	{
			engine->Draw3DBox(Box, video::SColor(255,255,255,255));
    }


        // show bounding box
		if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
		{
    	      const u32 count = Mesh->getMeshBufferCount();
			for (u32 g=0; g <Mesh->getMeshBufferCount(); ++g)
			{
				const IMeshBuffer* mb = Mesh->getMeshBuffer(g);
				core::aabbox3df obb = mb->getBoundingBox();
				model.transformBox(obb);
				engine->Draw3DBox(obb, video::SColor(255,190,128,128));
			}
		}

		// show normals
		if (DebugDataVisible & scene::EDS_NORMALS)
		{
			const f32 debugNormalLength = 0.5f;
			const video::SColor debugNormalColor = video::SColor(255,190,128,0);
			const u32 count = Mesh->getMeshBufferCount();

			// draw normals
			for (u32 g=0; g < count; ++g)
			{
				scene::IMeshBuffer* mb = Mesh->getMeshBuffer(g);
				engine->DrawMeshBufferNormals(model,mb, debugNormalLength, debugNormalColor);
			}
		}

     if (Mesh->getMeshType() == EAMT_SKINNED)
     {


               if (DebugDataVisible & scene::EDS_SKELETON)
                {

                    for (u32 g=0; g < ((ISkinnedMesh*)Mesh)->getAllJoints().size(); ++g)
                    {
                        ISkinnedMesh::SJoint *joint=((ISkinnedMesh*)Mesh)->getAllJoints()[g];

                        for (u32 n=0;n<joint->Children.size();++n)
                        {
                           core::vector3df lpos=joint->GlobalAnimatedMatrix.getTranslation();
                           core::vector3df ldir=joint->Children[n]->GlobalAnimatedMatrix.getTranslation();

                           core::vector3df posok;
                           core::vector3df dirok;


                           model.transformVect(posok,lpos);
                        model.transformVect(dirok,ldir);

                           video::DrawLine3D(posok,dirok,                                   video::SColor(255,51,66,255));
                        }
                    }
				}



         engine->setShader(shader);
        std::shared_ptr<IAnimator> animator = m_parent->getComponent<IAnimator>();
        if (animator.get())
        {

                     bool RenderFromIdentity=animator.get()->getRenderFromIdentity();



                     for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
                    {
                        mat = Materials[g];
                        IMeshBuffer* meshBuffer = Mesh->getMeshBuffer(g);
                        int PrimitiveCount            =meshBuffer->getPrimitiveCount();
                        E_PRIMITIVE_TYPE primitiveType=meshBuffer->getPrimitiveType();

                             if (RenderFromIdentity)
                             {

                              shader->setMat4("model",core::IdentityMatrix);
                             } else
                             {

                               shader->setMat4("model",m_parent->Transform->getAbsoluteTransformation() * ((SSkinMeshBuffer*)meshBuffer)->Transformation );

                             }

                        engine->setMaterial(mat);
                        meshBuffer->render(PrimitiveCount,primitiveType);


                    }



        } else{


                for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
                {
                    mat = Materials[g];
                    IMeshBuffer* meshBuffer = Mesh->getMeshBuffer(g);
                    int PrimitiveCount            =meshBuffer->getPrimitiveCount();
                    E_PRIMITIVE_TYPE primitiveType=meshBuffer->getPrimitiveType();
                    shader->setMat4("model",m_parent->Transform->getAbsoluteTransformation() * ((SSkinMeshBuffer*)meshBuffer)->Transformation );
                    engine->setMaterial(mat);
                    meshBuffer->render(PrimitiveCount,primitiveType);
                    engine->Draw3DBox(meshBuffer->getBoundingBox(),SColor(255,0,255,255));

                }

        }


     } else{

        engine->setShader(shader,m_parent->Transform->getAbsoluteTransformation());
        for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
        {
            mat = Materials[g];
            IMeshBuffer* meshBuffer = Mesh->getMeshBuffer(g);
            int PrimitiveCount            =meshBuffer->getPrimitiveCount();
            E_PRIMITIVE_TYPE primitiveType=meshBuffer->getPrimitiveType();
            engine->setMaterial(mat);
            meshBuffer->render(PrimitiveCount,primitiveType);

           //engine->Draw3DBox(meshBuffer->getBoundingBox(),SColor(255,0,255,255));



        }
     }





//Mesh->getBoundingBox().

//    engine->Draw3DBox(Mesh->getBoundingBox(),SColor(255,0,255,255));



/*

for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
{
	IMeshBuffer* buffer = Mesh->getMeshBuffer(g);

     u32 vtxcnt = buffer->getVertexCount();
	 u32 idxcnt = buffer->getIndexCount();
	 if (idxcnt<=0) continue;
	 u16* idx = reinterpret_cast<u16*>(buffer->getIndices());

	for (u32 i=0; i<idxcnt; i+=3)
		{
			const core::vector3df& v1 = buffer->getPosition(idx[i+0]);
			const core::vector3df& v2 = buffer->getPosition(idx[i+1]);
			const core::vector3df& v3 = buffer->getPosition(idx[i+2]);

			core::vector3df a1,a2,a3;
			 model.transformVect(a1,v1);
			 model.transformVect(a2,v2);
			 model.transformVect(a3,v3);

			engine->Draw3DTriangleLines(v1,v2,v3,SColor(255,0,255,255));
			//engine->Draw3DTriangleLines(a1,a2,a3,SColor(255,0,0,255));
			//engine->draw3DTriangle(v1,v2,v3,SColor(0,0,255,255));

		}
}
*/

}

}
}
