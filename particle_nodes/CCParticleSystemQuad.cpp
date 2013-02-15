/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2009		Leonardo Kasperavičius
* Copyright (c) 2011      Zynga Inc.
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
#include "platform/CCGL.h"

#include "CCParticleSystemQuad.h"
#include "CCSpriteFrame.h"
#include "CCDirector.h"
#include "CCFileUtils.h"
#include "DirectXHelper.h"
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

namespace cocos2d {

CCDXParticleSystemQuad CCParticleSystemQuad::mDXParticleSystemQuad;

//implementation CCParticleSystemQuad
// overriding the init method
bool CCParticleSystemQuad::initWithTotalParticles(unsigned int numberOfParticles)
{
	// base initialization
	if( CCParticleSystem::initWithTotalParticles(numberOfParticles) ) 
	{
		// allocating data space
		m_pQuads = new ccV2F_C4B_T2F_Quad[m_uTotalParticles];
		m_pIndices = new CCushort[m_uTotalParticles * 6];
		memset(m_pQuads,0,sizeof(ccV2F_C4B_T2F_Quad)*m_uTotalParticles);
		memset(m_pIndices,0,sizeof(CCushort)*m_uTotalParticles * 6);
		if( !m_pQuads || !m_pIndices) 
		{
			CCLOG("cocos2d: Particle system: not enough memory");
			if( m_pQuads )
				delete [] m_pQuads;
			if(m_pIndices)
				delete [] m_pIndices;
			this->release();
			return NULL;
		}

		// initialize only once the texCoords and the indices
        if (m_pTexture)
        {
            this->initTexCoordsWithRect(CCRectMake((float)0, (float)0, (float)m_pTexture->getPixelsWide(), (float)m_pTexture->getPixelsHigh()));
        }
        else
        {
            this->initTexCoordsWithRect(CCRectMake((float)0, (float)0, (float)1, (float)1));
        }

		this->initIndices();

#if CC_USES_VBO
		/*
		glEnable(CC_VERTEX_ARRAY);

		// create the VBO buffer
		glGenBuffers(1, &m_uQuadsID);

		// initial binding
		glBindBuffer(CC_ARRAY_BUFFER, m_uQuadsID);
		glBufferData(CC_ARRAY_BUFFER, sizeof(m_pQuads[0])*m_uTotalParticles, m_pQuads, CC_DYNAMIC_DRAW);
		glBindBuffer(CC_ARRAY_BUFFER, 0);
		*/
#endif
		return true;
	}
	return false;
}

CCParticleSystemQuad::CCParticleSystemQuad()
:m_pQuads(NULL)
,m_pIndices(NULL)
{
}

CCParticleSystemQuad::~CCParticleSystemQuad()
{
	CC_SAFE_DELETE_ARRAY(m_pQuads);
	CC_SAFE_DELETE_ARRAY(m_pIndices);
#if CC_USES_VBO
    //glDeleteBuffers(1, &m_uQuadsID);
#endif
}

// implementation CCParticleSystemQuad
CCParticleSystemQuad * CCParticleSystemQuad::particleWithFile(const char *plistFile)
{
    CCParticleSystemQuad *pRet = new CCParticleSystemQuad();
    if (pRet && pRet->initWithFile(plistFile))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet)
        return pRet;
}

// pointRect should be in Texture coordinates, not pixel coordinates
void CCParticleSystemQuad::initTexCoordsWithRect(const CCRect& pointRect)
{
    // convert to pixels coords

    CCRect rect = CCRectMake(
        pointRect.origin.x * CC_CONTENT_SCALE_FACTOR(),
        pointRect.origin.y * CC_CONTENT_SCALE_FACTOR(),
        pointRect.size.width * CC_CONTENT_SCALE_FACTOR(),
        pointRect.size.height * CC_CONTENT_SCALE_FACTOR());

    CCfloat wide = (CCfloat) pointRect.size.width;
    CCfloat high = (CCfloat) pointRect.size.height;

    if (m_pTexture)
    {
        wide = (CCfloat)m_pTexture->getPixelsWide();
        high = (CCfloat)m_pTexture->getPixelsHigh();
    }

#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    CCfloat left = (rect.origin.x*2+1) / (wide*2);
    CCfloat bottom = (rect.origin.y*2+1) / (high*2);
    CCfloat right = left + (rect.size.width*2-2) / (wide*2);
    CCfloat top = bottom + (rect.size.height*2-2) / (high*2);
#else
    CCfloat left = rect.origin.x / wide;
    CCfloat bottom = rect.origin.y / high;
    CCfloat right = left + rect.size.width / wide;
    CCfloat top = bottom + rect.size.height / high;
#endif // ! CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL

	// Important. Texture in cocos2d are inverted, so the Y component should be inverted
	CC_SWAP( top, bottom, float);

	for(unsigned int i=0; i<m_uTotalParticles; i++) 
	{
		// bottom-left vertex:
		m_pQuads[i].bl.texCoords.u = left;
		m_pQuads[i].bl.texCoords.v = bottom;
		// bottom-right vertex:
		m_pQuads[i].br.texCoords.u = right;
		m_pQuads[i].br.texCoords.v = bottom;
		// top-left vertex:
		m_pQuads[i].tl.texCoords.u = left;
		m_pQuads[i].tl.texCoords.v = top;
		// top-right vertex:
		m_pQuads[i].tr.texCoords.u = right;
		m_pQuads[i].tr.texCoords.v = top;
	}
}
void CCParticleSystemQuad::setTextureWithRect(CCTexture2D *texture, const CCRect& rect)
{
	// Only update the texture if is different from the current one
	if( !m_pTexture || texture->getName() != m_pTexture->getName() )
	{
		CCParticleSystem::setTexture(texture);
	}

	this->initTexCoordsWithRect(rect);
}
void CCParticleSystemQuad::setTexture(CCTexture2D* texture)
{
	const CCSize& s = texture->getContentSize();
	this->setTextureWithRect(texture, CCRectMake(0, 0, s.width, s.height));
}
void CCParticleSystemQuad::setDisplayFrame(CCSpriteFrame *spriteFrame)
{
	CCAssert( CCPoint::CCPointEqualToPoint( spriteFrame->getOffsetInPixels() , CCPointZero ), "QuadParticle only supports SpriteFrames with no offsets");

	// update texture before updating texture rect
	if ( !m_pTexture || spriteFrame->getTexture()->getName() != m_pTexture->getName())
	{
		this->setTexture(spriteFrame->getTexture());
	}
}
void CCParticleSystemQuad::initIndices()
{
	for(unsigned int i = 0; i < m_uTotalParticles; ++i)
	{
        const unsigned int i6 = i*6;
        const unsigned int i4 = i*4;
		m_pIndices[i6+0] = (CCushort) i4+0;
		m_pIndices[i6+1] = (CCushort) i4+1;
		m_pIndices[i6+2] = (CCushort) i4+2;

// 		m_pIndices[i6+5] = (CCushort) i4+1;
// 		m_pIndices[i6+4] = (CCushort) i4+2;
// 		m_pIndices[i6+3] = (CCushort) i4+3;
		//dx!!!!!!!
		m_pIndices[i6+5] = (CCushort) i4+3;
		m_pIndices[i6+4] = (CCushort) i4+2;
		m_pIndices[i6+3] = (CCushort) i4+0;
	}
}
void CCParticleSystemQuad::updateQuadWithParticle(tCCParticle* particle, const CCPoint& newPosition)
{
	// colors
    ccV2F_C4B_T2F_Quad *quad = &(m_pQuads[m_uParticleIdx]);

	ccColor4B color = {(CCubyte)(particle->color.r * 255), (CCubyte)(particle->color.g * 255), (CCubyte)(particle->color.b * 255), 
		(CCubyte)(particle->color.a * 255)};
	quad->bl.colors = color;
	quad->br.colors = color;
	quad->tl.colors = color;
	quad->tr.colors = color;

	// vertices
	CCfloat size_2 = particle->size/2;
	if( particle->rotation ) 
	{
		CCfloat x1 = -size_2;
		CCfloat y1 = -size_2;

		CCfloat x2 = size_2;
		CCfloat y2 = size_2;
		CCfloat x = newPosition.x;
		CCfloat y = newPosition.y;

		CCfloat r = (CCfloat)-CC_DEGREES_TO_RADIANS(particle->rotation);
		CCfloat cr = cosf(r);
		CCfloat sr = sinf(r);
		CCfloat ax = x1 * cr - y1 * sr + x;
		CCfloat ay = x1 * sr + y1 * cr + y;
		CCfloat bx = x2 * cr - y1 * sr + x;
		CCfloat by = x2 * sr + y1 * cr + y;
		CCfloat cx = x2 * cr - y2 * sr + x;
		CCfloat cy = x2 * sr + y2 * cr + y;
		CCfloat dx = x1 * cr - y2 * sr + x;
		CCfloat dy = x1 * sr + y2 * cr + y;

		// bottom-left
		quad->bl.vertices.x = ax;
		quad->bl.vertices.y = ay;

		// bottom-right vertex:
		quad->br.vertices.x = bx;
		quad->br.vertices.y = by;

		// top-left vertex:
		quad->tl.vertices.x = dx;
		quad->tl.vertices.y = dy;

		// top-right vertex:
		quad->tr.vertices.x = cx;
		quad->tr.vertices.y = cy;
	} else {
		// bottom-left vertex:
		quad->bl.vertices.x = newPosition.x - size_2;
		quad->bl.vertices.y = newPosition.y - size_2;

		// bottom-right vertex:
		quad->br.vertices.x = newPosition.x + size_2;
		quad->br.vertices.y = newPosition.y - size_2;

		// top-left vertex:
		quad->tl.vertices.x = newPosition.x - size_2;
		quad->tl.vertices.y = newPosition.y + size_2;

		// top-right vertex:
		quad->tr.vertices.x = newPosition.x + size_2;
		quad->tr.vertices.y = newPosition.y + size_2;				
	}
}
void CCParticleSystemQuad::postStep()
{
#if CC_USES_VBO
	//glBindBuffer(CC_ARRAY_BUFFER, m_uQuadsID);
	//glBufferSubData(CC_ARRAY_BUFFER, 0, sizeof(m_pQuads[0])*m_uParticleCount, m_pQuads);
	//glBindBuffer(CC_ARRAY_BUFFER, 0);
#endif
}

// overriding draw method
void CCParticleSystemQuad::draw()
{	
	CCParticleSystem::draw();

#if CC_USES_VBO
   // glBindBuffer(CC_ARRAY_BUFFER, m_uQuadsID);

#if CC_ENABLE_CACHE_TEXTTURE_DATA
    //glBufferData(CC_ARRAY_BUFFER, sizeof(m_pQuads[0])*m_uTotalParticles, m_pQuads, CC_DYNAMIC_DRAW);	
#endif
	/*
	glVertexPointer(2,CC_FLOAT, kQuadSize, 0);

	glColorPointer(4, CC_UNSIGNED_BYTE, kQuadSize, (CCvoid*) offsetof(ccV2F_C4B_T2F,colors) );

	glTexCoordPointer(2, CC_FLOAT, kQuadSize, (CCvoid*) offsetof(ccV2F_C4B_T2F,texCoords) );
	*/
#else   // vertex array list

#endif // ! CC_USES_VBO

    bool newBlend = (m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST) ? true : false;
	if( newBlend ) 
	{
		CCD3DCLASS->D3DBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
	}

    CCAssert( m_uParticleIdx == m_uParticleCount, "Abnormal error in particle quad");

	mDXParticleSystemQuad.Render(m_pQuads,m_pIndices,m_uTotalParticles,m_uParticleIdx,m_pTexture);

	// restore blend state
	if( newBlend )
	{
		CCD3DCLASS->D3DBlendFunc( CC_BLEND_SRC, CC_BLEND_DST );
	}

#if CC_USES_VBO
	//glBindBuffer(CC_ARRAY_BUFFER, 0);
#endif

	// restore GL default state
	// -
}


CCDXParticleSystemQuad::CCDXParticleSystemQuad()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_inputLayout = 0;
	m_matrixBuffer = 0;
	m_indexBuffer = 0;
	m_vertexBuffer = 0;
	//there is no any allocated memory yet
	m_uMaxTotalParticles = 0;

	m_bIsInit = FALSE;
}
CCDXParticleSystemQuad::~CCDXParticleSystemQuad()
{
	FreeBuffer();
}
void CCDXParticleSystemQuad::FreeBuffer()
{
	CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_indexBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_matrixBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_inputLayout);
	CC_SAFE_RELEASE_NULL_DX(m_pixelShader);
	CC_SAFE_RELEASE_NULL_DX(m_vertexShader);
}

void CCDXParticleSystemQuad::setIsInit(bool isInit)
{
	m_bIsInit = isInit;
}

void CCDXParticleSystemQuad::initVertexAndIndexBuffer(unsigned short* indices, unsigned int uTotalParticles)
{
	int vertexCount = uTotalParticles * 4;
	//CCLog("Last indxCount:%d)", uTotalParticles*6);
	D3D11_BUFFER_DESC vertexBufferDesc;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// Now create the vertex buffer.
	if(FAILED(CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer)))
	{
		return ;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	//indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(CCushort) * uTotalParticles * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = indices;

	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//if(FAILED(CCID3D11Device->CreateBuffer(&indexBufferDesc, NULL, &m_indexBuffer)))
	if(FAILED(CCID3D11Device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return ;
	}
}

void CCDXParticleSystemQuad::RenderVertexBuffer(ccV2F_C4B_T2F_Quad *quad,unsigned int uTotalParticles)
{
	VertexType* verticesTmp;
	// Create the vertex array.
	verticesTmp = new VertexType[uTotalParticles * 4];
	if(!verticesTmp)
	{
		return ;
	}
	ZeroMemory(verticesTmp,sizeof(VertexType)*uTotalParticles * 4);
	for ( int i=0; i<uTotalParticles; i++ )
	{
		verticesTmp[4*i+0].position = XMFLOAT2(quad->tl.vertices.x, quad->tl.vertices.y);
		verticesTmp[4*i+1].position = XMFLOAT2(quad->tr.vertices.x, quad->tr.vertices.y);
		verticesTmp[4*i+2].position = XMFLOAT2(quad->br.vertices.x, quad->br.vertices.y);
		verticesTmp[4*i+3].position = XMFLOAT2(quad->bl.vertices.x, quad->bl.vertices.y);

		verticesTmp[4*i+0].texture = XMFLOAT2(quad->tl.texCoords.u, quad->tl.texCoords.v);
		verticesTmp[4*i+1].texture = XMFLOAT2(quad->tr.texCoords.u, quad->tr.texCoords.v);
		verticesTmp[4*i+2].texture = XMFLOAT2(quad->br.texCoords.u, quad->br.texCoords.v);
		verticesTmp[4*i+3].texture = XMFLOAT2(quad->bl.texCoords.u, quad->bl.texCoords.v);

		verticesTmp[4*i+0].color = XMFLOAT4(quad->tl.colors.r/255.0f, quad->tl.colors.g/255.0f, quad->tl.colors.b/255.0f, quad->tl.colors.a/255.0f);
		verticesTmp[4*i+1].color = XMFLOAT4(quad->tr.colors.r/255.0f, quad->tr.colors.g/255.0f, quad->tr.colors.b/255.0f, quad->tr.colors.a/255.0f);
		verticesTmp[4*i+2].color = XMFLOAT4(quad->br.colors.r/255.0f, quad->br.colors.g/255.0f, quad->br.colors.b/255.0f, quad->br.colors.a/255.0f);
		verticesTmp[4*i+3].color = XMFLOAT4(quad->bl.colors.r/255.0f, quad->bl.colors.g/255.0f, quad->bl.colors.b/255.0f, quad->bl.colors.a/255.0f);
		
		quad++;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;
	
	result = CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		// clean up allocated resources
		delete[] verticesTmp;
		return ;
	}
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)verticesTmp, (sizeof(VertexType) * uTotalParticles * 4));
	CCID3D11DeviceContext->Unmap(m_vertexBuffer, 0);

	if ( verticesTmp )
	{
		delete[] verticesTmp;
		verticesTmp = 0;
	}

	////////////////////////
	unsigned int stride;
	unsigned int offset;
	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	CCID3D11DeviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool CCDXParticleSystemQuad::InitializeShader()
{
	HRESULT result;
	D3D11_BUFFER_DESC matrixBufferDesc;

	 BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
	 D3D11_INPUT_ELEMENT_DESC layoutDesc[] = 
	 {
		 { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	 };

	 loader->LoadShader(
		 L"CCParticleVertexShader.cso",
		 layoutDesc,
		 ARRAYSIZE(layoutDesc),
		 &m_vertexShader,
		 &m_inputLayout
		 );

	 loader->LoadShader(
		 L"CCParticlePixelShader.cso",
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

void CCDXParticleSystemQuad::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool CCDXParticleSystemQuad::SetShaderParameters(XMMATRIX &viewMatrix,XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = CCID3D11DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	CCID3D11DeviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	CCID3D11DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	CCID3D11DeviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void CCDXParticleSystemQuad::RenderShader(unsigned int particleIdx,CCTexture2D* texture)
{
	// Set the vertex input layout.
	CCID3D11DeviceContext->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	CCID3D11DeviceContext->PSSetSamplers(0, 1, texture->GetSamplerState());
	//CCLog("CCDXParticleSystemQuad:RenderShader(idx:%d, m_indexCount:)",particleIdx);
	// Render the triangle.
	CCID3D11DeviceContext->DrawIndexed((particleIdx*6),0, 0 );

	return;
}

void CCDXParticleSystemQuad::Render(ccV2F_C4B_T2F_Quad *quad,unsigned short* indices,unsigned int uTotalParticles,unsigned int particleIdx,CCTexture2D* texture)
{

	if ( !m_bIsInit )
	{
		m_bIsInit = TRUE;
		FreeBuffer();
		m_uMaxTotalParticles = uTotalParticles;
		initVertexAndIndexBuffer(indices, uTotalParticles);
		InitializeShader();
	}
	
	if(m_uMaxTotalParticles < uTotalParticles)
	{
		m_uMaxTotalParticles = uTotalParticles;
		initVertexAndIndexBuffer(indices, uTotalParticles);
	}
	
	
	XMMATRIX viewMatrix, projectionMatrix;
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);
	RenderVertexBuffer(quad, uTotalParticles);
	SetShaderParameters(viewMatrix, projectionMatrix, texture->getTextureResource());
	RenderShader(particleIdx, texture);
}

}// namespace cocos2d
