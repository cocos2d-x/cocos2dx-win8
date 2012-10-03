/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2006-2010 Erin Catto http://www.box2d.org
* 
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0 
* 
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and limitations under the License.
*/

#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include "pch.h"

#include <new>
#include <cstring>
using namespace std;

b2ChainShape::~b2ChainShape()
{
	b2Free(m_vertices);
	m_vertices = NULL;
	m_count = 0;
}

void b2ChainShape::CreateLoop(const b2Vec2* vertices, int32 count)
{
	b2Assert(m_vertices == NULL && m_count == 0);
	b2Assert(count >= 3);
	m_count = count + 1;
	m_vertices = (b2Vec2*)b2Alloc(m_count * sizeof(b2Vec2));
	memcpy(m_vertices, vertices, count * sizeof(b2Vec2));
	m_vertices[count] = m_vertices[0];
	m_prevVertex = m_vertices[m_count - 2];
	m_nextVertex = m_vertices[1];
	m_hasPrevVertex = true;
	m_hasNextVertex = true;
}

void b2ChainShape::CreateChain(const b2Vec2* vertices, int32 count)
{
	b2Assert(m_vertices == NULL && m_count == 0);
	b2Assert(count >= 2);
	m_count = count;
	m_vertices = (b2Vec2*)b2Alloc(count * sizeof(b2Vec2));
	memcpy(m_vertices, vertices, m_count * sizeof(b2Vec2));
	m_hasPrevVertex = false;
	m_hasNextVertex = false;
}

void b2ChainShape::SetPrevVertex(const b2Vec2& prevVertex)
{
	m_prevVertex = prevVertex;
	m_hasPrevVertex = true;
}

void b2ChainShape::SetNextVertex(const b2Vec2& nextVertex)
{
	m_nextVertex = nextVertex;
	m_hasNextVertex = true;
}

b2Shape* b2ChainShape::Clone(b2BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(b2ChainShape));
	b2ChainShape* clone = new (mem) b2ChainShape;
	clone->CreateChain(m_vertices, m_count);
	clone->m_prevVertex = m_prevVertex;
	clone->m_nextVertex = m_nextVertex;
	clone->m_hasPrevVertex = m_hasPrevVertex;
	clone->m_hasNextVertex = m_hasNextVertex;
	return clone;
}

int32 b2ChainShape::GetChildCount() const
{
	// edge count = vertex count - 1
	return m_count - 1;
}

void b2ChainShape::GetChildEdge(b2EdgeShape* edge, int32 index) const
{
	b2Assert(0 <= index && index < m_count - 1);
	edge->m_type = b2Shape::e_edge;
	edge->m_radius = m_radius;

	edge->m_vertex1 = m_vertices[index + 0];
	edge->m_vertex2 = m_vertices[index + 1];

	if (index > 0)
	{
		edge->m_vertex0 = m_vertices[index - 1];
		edge->m_hasVertex0 = true;
	}
	else
	{
		edge->m_vertex0 = m_prevVertex;
		edge->m_hasVertex0 = m_hasPrevVertex;
	}

	if (index < m_count - 2)
	{
		edge->m_vertex3 = m_vertices[index + 2];
		edge->m_hasVertex3 = true;
	}
	else
	{
		edge->m_vertex3 = m_nextVertex;
		edge->m_hasVertex3 = m_hasNextVertex;
	}
}

bool b2ChainShape::TestPoint(const b2Transform& xf, const b2Vec2& p) const
{
	B2_NOT_USED(xf);
	B2_NOT_USED(p);
	return false;
}

bool b2ChainShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
							const b2Transform& xf, int32 childIndex) const
{
	b2Assert(childIndex < m_count);

	b2EdgeShape edgeShape;

	int32 i1 = childIndex;
	int32 i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	edgeShape.m_vertex1 = m_vertices[i1];
	edgeShape.m_vertex2 = m_vertices[i2];

	return edgeShape.RayCast(output, input, xf, 0);
}

void b2ChainShape::ComputeAABB(b2AABB* aabb, const b2Transform& xf, int32 childIndex) const
{
	b2Assert(childIndex < m_count);

	int32 i1 = childIndex;
	int32 i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	b2Vec2 v1 = b2Mul(xf, m_vertices[i1]);
	b2Vec2 v2 = b2Mul(xf, m_vertices[i2]);

	aabb->lowerBound = b2Min(v1, v2);
	aabb->upperBound = b2Max(v1, v2);
}

void b2ChainShape::ComputeMass(b2MassData* massData, float32 density) const
{
	B2_NOT_USED(density);

	massData->mass = 0.0f;
	massData->center.SetZero();
	massData->I = 0.0f;
}
