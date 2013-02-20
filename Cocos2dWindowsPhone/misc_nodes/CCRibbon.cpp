/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2009 Jason Booth
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

#include "pch.h"

#include "CCRibbon.h"
#include "CCTextureCache.h"
#include "CCPointExtension.h"
#include "CCDirector.h"
#include "CCFileUtils.h"
#include "DirectXHelper.h"
#include <fstream>
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

namespace cocos2d {

/*
* A ribbon is a dynamically generated list of polygons drawn as a single or series
* of triangle strips. The primary use of Ribbon is as the drawing class of Motion Streak,
* but it is quite useful on it's own. When manually drawing a ribbon, you can call addPointAt
* and pass in the parameters for the next location in the ribbon. The system will automatically
* generate new polygons, texture them accourding to your texture width, etc, etc.
*
* Ribbon data is stored in a RibbonSegment class. This class statically allocates enough verticies and
* texture coordinates for 50 locations (100 verts or 48 triangles). The ribbon class will allocate
* new segments when they are needed, and reuse old ones if available. The idea is to avoid constantly
* allocating new memory and prefer a more static method. However, since there is no way to determine
* the maximum size of some ribbons (motion streaks), a truely static allocation is not possible.
*
*/

//
// Ribbon
//
CCRibbon * CCRibbon::ribbonWithWidth(float w, const char *path, float length, const ccColor4B& color, float fade)
{
	CCRibbon *pRet = new CCRibbon();
	if(pRet && pRet->initWithWidth(w, path, length, color, fade))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet)
	return NULL;
}

bool CCRibbon::initWithWidth(float w, const char *path, float length, const ccColor4B& color, float fade)
{
	m_pSegments = new CCMutableArray<CCRibbonSegment*>();
	m_pDeletedSegments = new CCMutableArray<CCRibbonSegment*>();

	/* 1 initial segment */
	CCRibbonSegment* seg = new CCRibbonSegment();
	seg->init();
	m_pSegments->addObject(seg);
	seg->release();

	m_fTextureLength = length;

	m_tColor = color;
	m_fFadeTime = fade;
	m_tLastLocation = CCPointZero;
	m_fLastWidth = w/2;
	m_fTexVPos = 0.0f;

	m_fCurTime = 0;
	m_bPastFirstPoint = false;

	/* XXX:
	Ribbon, by default uses this blend function, which might not be correct
	if you are using premultiplied alpha images,
	but 99% you might want to use this blending function regarding of the texture
	*/
	m_tBlendFunc.src = CC_SRC_ALPHA;
	m_tBlendFunc.dst = CC_ONE_MINUS_SRC_ALPHA;

	m_pTexture = CCTextureCache::sharedTextureCache()->addImage(path);
	CC_SAFE_RETAIN(m_pTexture);

	/* default texture parameter */
	ccTexParams params = { CC_LINEAR, CC_LINEAR, CC_REPEAT, CC_REPEAT };
	m_pTexture->setTexParameters(&params);
	return true;
}

CCRibbon::~CCRibbon()
{
    CC_SAFE_RELEASE(m_pSegments);
    CC_SAFE_RELEASE(m_pDeletedSegments);
    CC_SAFE_RELEASE(m_pTexture);
}

CCPoint CCRibbon::rotatePoint(const CCPoint& vec, float rotation)
{
	CCPoint ret;
	ret.x = (vec.x * cosf(rotation)) - (vec.y * sinf(rotation));
	ret.y = (vec.x * sinf(rotation)) + (vec.y * cosf(rotation));
	return ret;
}

void CCRibbon::update(ccTime delta)
{
	m_fCurTime += delta;
	m_fDelta = delta;
}

float CCRibbon::sideOfLine(const CCPoint& p, const CCPoint& l1, const CCPoint& l2)
{
	CCPoint vp = ccpPerp(ccpSub(l1, l2));
	CCPoint vx = ccpSub(p, l1);
	return ccpDot(vx, vp);
}

// adds a new segment to the ribbon
void CCRibbon::addPointAt(CCPoint location, float width)
{
    location.x *= CC_CONTENT_SCALE_FACTOR();
    location.y *= CC_CONTENT_SCALE_FACTOR();

    width = width * 0.5f;
	// if this is the first point added, cache it and return
	if (!m_bPastFirstPoint)
	{
		m_fLastWidth = width;
		m_tLastLocation = location;
		m_bPastFirstPoint = true;
		return;
	}

	CCPoint sub = ccpSub(m_tLastLocation, location);
	float r = ccpToAngle(sub) + (float)M_PI_2;
	CCPoint p1 = ccpAdd(this->rotatePoint(ccp(-width, 0), r), location);
	CCPoint p2 = ccpAdd(this->rotatePoint(ccp(+width, 0), r), location);
	float len = sqrtf(powf(m_tLastLocation.x - location.x, 2) + powf(m_tLastLocation.y - location.y, 2));
	float tend = m_fTexVPos + len/m_fTextureLength;
	CCRibbonSegment* seg;
	// grab last segment
	seg = m_pSegments->getLastObject();
	// lets kill old segments
	if (m_pSegments && m_pSegments->count()>0)
	{
		CCMutableArray<CCRibbonSegment*>::CCMutableArrayIterator it;
		for (it = m_pSegments->begin(); it != m_pSegments->end(); ++it)
		{
			if (*it != seg && (*it)->m_bFinished)
			{
				m_pDeletedSegments->addObject(*it);
			}
		}
	}
	
	m_pSegments->removeObjectsInArray(m_pDeletedSegments);
	// is the segment full?
	if (seg->m_uEnd >= 50)
	{
		m_pSegments->removeObjectsInArray(m_pDeletedSegments);
	}
	// grab last segment and append to it if it's not full
	seg = m_pSegments->getLastObject();
	// is the segment full?
	if (seg->m_uEnd >= 50)
	{
		CCRibbonSegment* newSeg;
		// grab it from the cache if we can
		if (m_pDeletedSegments->count() > 0)
		{
			newSeg = m_pDeletedSegments->getObjectAtIndex(0);
			newSeg->retain();							// will be released later
			m_pDeletedSegments->removeObject(newSeg);
			newSeg->reset();
		}
		else
		{
			newSeg = new CCRibbonSegment(); // will be released later
			newSeg->init();
		}

		newSeg->m_pCreationTime[0] = seg->m_pCreationTime[seg->m_uEnd- 1];
		int v = (seg->m_uEnd-1)*6;
		int c = (seg->m_uEnd-1)*4;	
		newSeg->m_pVerts[0] = seg->m_pVerts[v];
		newSeg->m_pVerts[1] = seg->m_pVerts[v+1];
		newSeg->m_pVerts[2] = seg->m_pVerts[v+2];
		newSeg->m_pVerts[3] = seg->m_pVerts[v+3];
		newSeg->m_pVerts[4] = seg->m_pVerts[v+4];
		newSeg->m_pVerts[5] = seg->m_pVerts[v+5];

		newSeg->m_pCoords[0] = seg->m_pCoords[c];
		newSeg->m_pCoords[1] = seg->m_pCoords[c+1];
		newSeg->m_pCoords[2] = seg->m_pCoords[c+2];
		newSeg->m_pCoords[3] = seg->m_pCoords[c+3];	  
		newSeg->m_uEnd++;
		seg = newSeg;
		m_pSegments->addObject(seg);
		newSeg->release();// it was retained before
	}  
	if (seg->m_uEnd == 0)
	{
		// first edge has to get rotation from the first real polygon
		CCPoint lp1 = ccpAdd(this->rotatePoint(ccp(-m_fLastWidth, 0), r), m_tLastLocation);
		CCPoint lp2 = ccpAdd(this->rotatePoint(ccp(+m_fLastWidth, 0), r), m_tLastLocation);
		seg->m_pCreationTime[0] = m_fCurTime - m_fDelta;
		seg->m_pVerts[0] = lp1.x;
		seg->m_pVerts[1] = lp1.y;
		seg->m_pVerts[2] = 0.0f;
		seg->m_pVerts[3] = lp2.x;
		seg->m_pVerts[4] = lp2.y;
		seg->m_pVerts[5] = 0.0f;
		seg->m_pCoords[0] = 0.0f;
		seg->m_pCoords[1] = m_fTexVPos;
		seg->m_pCoords[2] = 1.0f;
		seg->m_pCoords[3] = m_fTexVPos;
		seg->m_uEnd++;
	}

	int v = seg->m_uEnd*6;
	int c = seg->m_uEnd*4;
	// add new vertex
	seg->m_pCreationTime[seg->m_uEnd] = m_fCurTime;
	seg->m_pVerts[v] = p1.x;
	seg->m_pVerts[v+1] = p1.y;
	seg->m_pVerts[v+2] = 0.0f;
	seg->m_pVerts[v+3] = p2.x;
	seg->m_pVerts[v+4] = p2.y;
	seg->m_pVerts[v+5] = 0.0f;


	seg->m_pCoords[c] = 0.0f;
	seg->m_pCoords[c+1] = tend;
	seg->m_pCoords[c+2] = 1.0f;
	seg->m_pCoords[c+3] = tend;

	m_fTexVPos = tend;
	m_tLastLocation = location;
	m_tLastPoint1 = p1;
	m_tLastPoint2 = p2;
	m_fLastWidth = width;
	seg->m_uEnd++;
}

void CCRibbon::draw()
{
	CCNode::draw();

	if (m_pSegments->count() > 0)
	{

        bool newBlend = ( m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST ) ? true : false;
        if( newBlend )
        {
            CCD3DCLASS->D3DBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
        }

		if(m_pSegments && m_pSegments->count() > 0)
		{
			CCRibbonSegment* seg;
			CCMutableArray<CCRibbonSegment*>::CCMutableArrayIterator it;
			for( it = m_pSegments->begin(); it != m_pSegments->end(); it++)
			{
				seg = (CCRibbonSegment*)*it;
				seg->draw(m_fCurTime, m_fFadeTime, m_tColor,m_pTexture);
			}
		}

		if( newBlend )
		{
			CCD3DCLASS->D3DBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
		}

	}
}

// Ribbon - CocosNodeTexture protocol
void CCRibbon::setTexture(CCTexture2D* var)
{
	CC_SAFE_RETAIN(var);
	CC_SAFE_RELEASE(m_pTexture);
	m_pTexture = var;
	this->setContentSize(m_pTexture->getContentSizeInPixels());
	/* XXX Don't update blending function in Ribbons */
}

CCTexture2D *CCRibbon::getTexture()
{
	return m_pTexture;
}

void CCRibbon::setTextureLength(float var)
{
	m_fTextureLength = var;
}

float CCRibbon::getTextureLength()
{
	return m_fTextureLength;
}
void CCRibbon::setBlendFunc(ccBlendFunc var)
{
	m_tBlendFunc = var;
}

ccBlendFunc CCRibbon::getBlendFunc()
{
	return m_tBlendFunc;
}

void CCRibbon::setColor(const ccColor4B& var)
{
	m_tColor = var;
}
const ccColor4B& CCRibbon::getColor()
{
	return m_tColor;
} 

//
//RibbonSegment
//

CCDXRibbonSegment CCRibbonSegment::mDXRibbonSegment;

bool CCRibbonSegment::init()
{
	this->reset();
	mDXRibbonSegment.setIsInit(FALSE);
	return true;
}

char * CCRibbonSegment::description()
{
	char *ret = new char[100] ;
	sprintf(ret, "<CCRibbonSegment | end = %u, begin = %u>", m_uEnd, m_uBegin);
	return ret;
}

CCRibbonSegment::~CCRibbonSegment()
{
	CCLOGINFO("cocos2d: deallocing.");
}

void CCRibbonSegment::reset()
{
	m_uEnd = 0;
	m_uBegin = 0;
	m_bFinished = false;
}

void CCRibbonSegment::draw(float curTime, float fadeTime, const ccColor4B& color,CCTexture2D* texture)
{
	CCubyte r = color.r;
	CCubyte g = color.g;
	CCubyte b = color.b;
	CCubyte a = color.a;

	if (m_uBegin < 50)
	{
		// the motion streak class will call update and cause time to change, thus, if curTime_ != 0
		// we have to generate alpha for the ribbon each frame.
		if (curTime == 0)
		{
			// no alpha over time, so just set the color
			for ( int i=0; i<100; i++ )
			{
				m_pColors[i*4+0] = r;
				m_pColors[i*4+1] = g;
				m_pColors[i*4+2] = b;
				m_pColors[i*4+3] = a;
			}
		}
		else
		{
			// generate alpha/color for each point
			unsigned int i = m_uBegin;
			for (; i < m_uEnd; ++i)
			{
				int idx = i*8;
				m_pColors[idx] = r;
				m_pColors[idx+1] = g;
				m_pColors[idx+2] = b;
				m_pColors[idx+4] = r;
				m_pColors[idx+5] = g;
				m_pColors[idx+6] = b;
				float alive = ((curTime - m_pCreationTime[i]) / fadeTime);
				if (alive > 1)
				{
					m_uBegin++;
					m_pColors[idx+3] = 0;
					m_pColors[idx+7] = 0;
				}
				else
				{
					m_pColors[idx+3] = (CCubyte)(255.f - (alive * 255.f));
					m_pColors[idx+7] = m_pColors[idx+3];
				}
			}
		}
		mDXRibbonSegment.Render(m_pVerts,m_pCoords,m_pColors,m_uBegin,m_uEnd,texture);
	}
	else
	{
		m_bFinished = true;
	}
}


CCDXRibbonSegment::CCDXRibbonSegment()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_vertexBuffer = 0;
	mIsInit = FALSE;
}
CCDXRibbonSegment::~CCDXRibbonSegment()
{
	FreeBuffer();
}
void CCDXRibbonSegment::FreeBuffer()
{
	CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_matrixBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_layout);
	CC_SAFE_RELEASE_NULL_DX(m_pixelShader);
	CC_SAFE_RELEASE_NULL_DX(m_vertexShader);
}

void CCDXRibbonSegment::setIsInit(bool isInit)
{
	mIsInit = isInit;
}

void CCDXRibbonSegment::RenderVertexBuffer(CCfloat* verts,CCfloat* coords,CCubyte* colors)
{
	VertexType* verticesTmp;
	// Create the vertex array.
	verticesTmp = new VertexType[100];
	if(!verticesTmp)
	{
		return ;
	}
	memset(verticesTmp,0,sizeof(VertexType)*100);

	for ( int i=0; i<100; i++ )
	{
		verticesTmp[i].color = XMFLOAT4(colors[i*4]/255.f, colors[i*4+1]/255.f, colors[i*4+2]/255.f, colors[i*4+3]/255.f);
	}
	for ( int i=0; i<100; i++ )
	{
		verticesTmp[i].texture = XMFLOAT2(coords[i*2], coords[i*2+1]);
		verticesTmp[i].position = XMFLOAT3(verts[i*3], verts[i*3+1], verts[i*3+2]);
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		delete[] verticesTmp;
		return;
	}
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)verticesTmp, (sizeof(VertexType) * 100));
	CCID3D11DeviceContext->Unmap(m_vertexBuffer, 0);

	if ( verticesTmp )
	{
		delete[] verticesTmp;
		verticesTmp = 0;
	}

	////////////////////////
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexType); 
	offset = 0;
	CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return;
}

void CCDXRibbonSegment::initVertexBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*100;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Now create the vertex buffer.
	if(FAILED(CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer)))
	{
		return ;
	}
}

bool CCDXRibbonSegment::InitializeShader()
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	//ID3D10Blob* vertexShaderBuffer;
	//ID3D10Blob* pixelShaderBuffer;
	D3D11_BUFFER_DESC matrixBufferDesc;
	//D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	loader->LoadShader(
		L"CCRibbonSegmentVertexShader.cso",
		layoutDesc,
		ARRAYSIZE(layoutDesc),
		&m_vertexShader,
		&m_layout
		);

	loader->LoadShader(
		L"CCRibbonSegmentPixelShader.cso",
		&m_pixelShader
		);


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = CCID3D11Device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void CCDXRibbonSegment::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	// MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool CCDXRibbonSegment::SetShaderParameters(XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	if(FAILED(CCID3D11DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))){return false;}
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	CCID3D11DeviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	CCID3D11DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	CCID3D11DeviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void CCDXRibbonSegment::RenderShader(unsigned int begin,unsigned int end,CCTexture2D* texture)
{
	CCID3D11DeviceContext->IASetInputLayout(m_layout);
	CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	CCID3D11DeviceContext->PSSetSamplers(0, 1, texture->GetSamplerState());
	CCID3D11DeviceContext->Draw((end - begin)*2,begin*2);

	return;
}


void CCDXRibbonSegment::Render(CCfloat* verts,CCfloat* coords,CCubyte* colors,unsigned int begin,unsigned int end,CCTexture2D* texture)
{

	if ( !mIsInit )
	{
		mIsInit = TRUE;
		FreeBuffer();
		initVertexBuffer();
		InitializeShader();
	}
	
	XMMATRIX viewMatrix, projectionMatrix;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderVertexBuffer(verts,coords,colors);

	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(viewMatrix, projectionMatrix, texture->getTextureResource());

	// Now render the prepared buffers with the shader.
	RenderShader(begin,end,texture);
}



}// namespace cocos2d
