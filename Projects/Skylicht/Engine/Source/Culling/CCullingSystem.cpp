/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CCullingSystem.h"
#include "Entity/CEntityManager.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Camera/CCamera.h"

namespace Skylicht
{
	CCullingSystem::CCullingSystem()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CCullingSystem::~CCullingSystem()
	{

	}

	void CCullingSystem::beginQuery()
	{
		m_cullings.set_used(0);
		m_transforms.set_used(0);
		m_invTransforms.set_used(0);
		m_meshs.set_used(0);
	}

	void CCullingSystem::onQuery(CEntityManager *entityManager, CEntity *entity)
	{
		CCullingData *culling = entity->getData<CCullingData>();

		CVisibleData *visible = entity->getData<CVisibleData>();
		if (culling != NULL && visible != NULL && visible->Visible == false)
		{
			culling->Visible = false;
		}
		else if (culling != NULL)
		{
			CRenderMeshData *mesh = entity->getData<CRenderMeshData>();
			if (mesh != NULL)
			{
				CWorldTransformData *transform = entity->getData<CWorldTransformData>();
				CWorldInverseTransformData *invTransform = entity->getData<CWorldInverseTransformData>();

				if (transform != NULL)
				{
					m_cullings.push_back(culling);
					m_meshs.push_back(mesh);
					m_transforms.push_back(transform);
					m_invTransforms.push_back(invTransform);
				}
			}
		}
	}

	void CCullingSystem::init(CEntityManager *entityManager)
	{

	}

	void CCullingSystem::update(CEntityManager *entityManager)
	{
		CCullingData **cullings = m_cullings.pointer();
		CRenderMeshData **meshs = m_meshs.pointer();
		CWorldTransformData **transforms = m_transforms.pointer();
		CWorldInverseTransformData **invTransforms = m_invTransforms.pointer();

		IRenderPipeline *rp = entityManager->getRenderPipeline();

		core::matrix4 invTrans;

		u32 numEntity = m_cullings.size();
		for (u32 i = 0; i < numEntity; i++)
		{
			// get mesh bbox
			CCullingData *culling = cullings[i];
			CWorldTransformData *transform = transforms[i];
			CWorldInverseTransformData *invTransform = invTransforms[i];
			CRenderMeshData *meshData = meshs[i];
			CMesh *mesh = meshData->getMesh();

			culling->Visible = true;

			// check material first			
			for (CMaterial *material : mesh->Material)
			{
				if (material != NULL && rp->canRenderMaterial(material) == false)
				{
					culling->Visible = false;
					break;
				}
			}

			if (culling->Visible == false)
				continue;

			// transform world bbox
			culling->BBox = mesh->getBoundingBox();
			transform->World.transformBoxEx(culling->BBox);

			// 1. Detect by bounding box
			CCamera *camera = entityManager->getCamera();
			culling->Visible = culling->BBox.intersectsWithBox(camera->getViewFrustum().getBoundingBox());

			// 2. Detect algorithm 
			if (culling->Visible == true)
			{
				if (culling->Type == CCullingData::FrustumBox && invTransform != NULL)
				{
					SViewFrustum frust = camera->getViewFrustum();

					// transform the frustum to the node's current absolute transformation
					invTrans = invTransform->WorldInverse;
					frust.transform(invTrans);

					core::vector3df edges[8];
					mesh->getBoundingBox().getEdges(edges);

					for (s32 i = 0; i < scene::SViewFrustum::VF_PLANE_COUNT; ++i)
					{
						bool boxInFrustum = false;
						for (u32 j = 0; j < 8; ++j)
						{
							if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
							{
								boxInFrustum = true;
								break;
							}
						}

						if (!boxInFrustum)
						{
							culling->Visible = false;
							break;
						}
					}
				}
				else if (culling->Type == CCullingData::Occlusion)
				{
					// todo
					// later
				}
			}
		}
	}

	void CCullingSystem::render(CEntityManager *entityManager)
	{

	}

	void CCullingSystem::postRender(CEntityManager *entityManager)
	{

	}
}