/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2009      On-Core

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "ccMacros.h"
#include "effects/CCGrid.h"
#include "CCDirector.h"
#include "effects/CCGrabber.h"
#include "support/ccUtils.h"
#include "CCGL.h"
#include "CCPointExtension.h"
#include "CCFileUtils.h"
#include "DirectXHelper.h"
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

namespace cocos2d
{
	// implementation of CCGridBase

	CCGridBase* CCGridBase::gridWithSize(const ccGridSize& gridSize)
	{
		CCGridBase *pGridBase = new CCGridBase();

		if (pGridBase)
		{
			if (pGridBase->initWithSize(gridSize))
			{
				pGridBase->autorelease();
			}
			else
			{
				CC_SAFE_RELEASE_NULL(pGridBase);
			}
		}

		return pGridBase;
	}

	CCGridBase* CCGridBase::gridWithSize(const ccGridSize& gridSize, CCTexture2D *texture, bool flipped)
	{
		CCGridBase *pGridBase = new CCGridBase();

		if (pGridBase)
		{
			if (pGridBase->initWithSize(gridSize, texture, flipped))
			{
				pGridBase->autorelease();
			}
			else
			{
				CC_SAFE_RELEASE_NULL(pGridBase);
			}
		}

		return pGridBase;
	}

	bool CCGridBase::initWithSize(const ccGridSize& gridSize, CCTexture2D *pTexture, bool bFlipped)
	{
		bool bRet = true;

		m_bActive = false;
		m_nReuseGrid = 0;
		m_sGridSize = gridSize;

		m_indexCount = 0, m_vertexCount = 0;

		m_pTexture = pTexture;
		CC_SAFE_RETAIN(m_pTexture);
		m_bIsTextureFlipped = bFlipped;

		const CCSize& texSize = m_pTexture->getContentSizeInPixels();
		m_obStep.x = texSize.width / m_sGridSize.x;
		m_obStep.y = texSize.height / m_sGridSize.y;

		m_pGrabber = new CCGrabber();
		if (m_pGrabber)
		{
			m_pGrabber->grab(m_pTexture);
		}
		else
		{
			bRet = false;
		}

		calculateVertexPoints();

		initVertexBuffer();
		InitializeShader();

		return bRet;
	}

	bool CCGridBase::initWithSize(const ccGridSize& gridSize)
	{
		CCDirector *pDirector = CCDirector::sharedDirector();
		m_winSize = pDirector->getWinSizeInPixels();

		unsigned long POTWide = m_winSize.width;//ccNextPOT((unsigned int)m_winSize.width);
		unsigned long POTHigh = m_winSize.height;//ccNextPOT((unsigned int)m_winSize.height);

		// we only use rgba8888
		CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_RGBA8888;

		void *data = calloc((int)(POTWide * POTHigh * 4), 1);
		if (! data)
		{
			CCLOG("cocos2d: CCGrid: not enough memory.");
			this->release();
			return false;
		}

		CCTexture2D *pTexture = new CCTexture2D();
		pTexture->initWithData(data, format, POTWide, POTHigh, m_winSize);

		free(data);

		if (! pTexture)
		{
			CCLOG("cocos2d: CCGrid: error creating texture");
			delete this;
			return false;
		}

		initWithSize(gridSize, pTexture, false);

		pTexture->release();

		return true;
	}

	CCGridBase::~CCGridBase(void)
	{
		CCLOGINFO("cocos2d: deallocing %p", this);

		setActive(false);
		CC_SAFE_RELEASE(m_pTexture);
		CC_SAFE_RELEASE(m_pGrabber);

		CC_SAFE_FREE(m_pTexCoordinates);
		CC_SAFE_FREE(m_pVertices);
		CC_SAFE_FREE(m_pIndices);
		CC_SAFE_FREE(m_pOriginalVertices);

		CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);
		CC_SAFE_RELEASE_NULL_DX(m_indexBuffer);
		CC_SAFE_RELEASE_NULL_DX(m_matrixBuffer);
		CC_SAFE_RELEASE_NULL_DX(m_layout);
		CC_SAFE_RELEASE_NULL_DX(m_pixelShader);
		CC_SAFE_RELEASE_NULL_DX(m_vertexShader);
	}

	// properties
	void CCGridBase::setActive(bool bActive)
	{
		m_bActive = bActive;
		if (! bActive)
		{
			CCDirector *pDirector = CCDirector::sharedDirector();
			ccDirectorProjection proj = pDirector->getProjection();
			pDirector->setProjection(proj);
		}
	}

	void CCGridBase::setIsTextureFlipped(bool bFlipped)
	{
		if (m_bIsTextureFlipped != bFlipped)
		{
			m_bIsTextureFlipped = bFlipped;
			calculateVertexPoints();
			initVertexBuffer();
		}
	}

	// This routine can be merged with Director
	void CCGridBase::applyLandscape(void)
	{
		CCDirector *pDirector = CCDirector::sharedDirector();

		CCSize winSize = pDirector->getDisplaySizeInPixels();
		float w = winSize.width / 2;
		float h = winSize.height / 2;

		ccDeviceOrientation orientation = pDirector->getDeviceOrientation();

		switch (orientation)
		{
		case CCDeviceOrientationLandscapeLeft:
			CCD3DCLASS->D3DTranslate(w,h,0);
			CCD3DCLASS->D3DRotate(-90,0,0,1);
			CCD3DCLASS->D3DTranslate(-h,-w,0);
			break;
		case CCDeviceOrientationLandscapeRight:
			CCD3DCLASS->D3DTranslate(w,h,0);
			CCD3DCLASS->D3DRotate(90,0,0,1);
			CCD3DCLASS->D3DTranslate(-h,-w,0);
			break;
		case CCDeviceOrientationPortraitUpsideDown:
			CCD3DCLASS->D3DTranslate(w,h,0);
			CCD3DCLASS->D3DRotate(180,0,0,1);
			CCD3DCLASS->D3DTranslate(-w,-h,0);
			break;
		default:
			break;
		}
	}

	void CCGridBase::set2DProjection()
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();

		CCD3DCLASS->D3DLoadIdentity();

		// set view port for user FBO, fixed bug #543 #544
		CCD3DCLASS->D3DViewport((int)0, (int)0, (int)winSize.width, (int)winSize.height);
		CCD3DCLASS->D3DMatrixMode(CC_PROJECTION);
		CCD3DCLASS->D3DLoadIdentity();
		CCD3DCLASS->D3DOrtho(0, winSize.width, 0, winSize.height, -1024, 1024);
		CCD3DCLASS->D3DMatrixMode(CC_MODELVIEW);
	}

	// This routine can be merged with Director
	void CCGridBase::set3DProjection()
	{
		CCSize	winSize = CCDirector::sharedDirector()->getDisplaySizeInPixels();

		// set view port for user FBO, fixed bug #543 #544
		CCD3DCLASS->D3DViewport(0, 0, (int)winSize.width, (int)winSize.height);
		CCD3DCLASS->D3DMatrixMode(CC_PROJECTION);
		CCD3DCLASS->D3DLoadIdentity();
		CCD3DCLASS->D3DPerspective(60, (float)winSize.width/winSize.height, 0.5f, 1500.0f);

		CCD3DCLASS->D3DMatrixMode(CC_MODELVIEW);	
		CCD3DCLASS->D3DLoadIdentity();
		CCD3DCLASS->D3DLookAt( winSize.width/2, winSize.height/2, CCDirector::sharedDirector()->getZEye(),
			winSize.width/2, winSize.height/2, 0,
			0.0f, 1.0f, 0.0f
			);
	}

	void CCGridBase::beforeDraw(void)
	{
		set2DProjection();
		m_pGrabber->beforeRender(m_pTexture);
	}

	void CCGridBase::afterDraw(cocos2d::CCNode *pTarget)
	{
		m_pGrabber->afterRender(m_pTexture);

		set3DProjection();
		applyLandscape();

		if (pTarget->getCamera()->getDirty())
		{
			const CCPoint& offset = pTarget->getAnchorPointInPixels();
			//
			// XXX: Camera should be applied in the AnchorPoint
			//
			CCD3DCLASS->D3DTranslate(offset.x, offset.y, 0);
			pTarget->getCamera()->locate();
			CCD3DCLASS->D3DTranslate(-offset.x, -offset.y, 0);
		}

		// restore projection for default FBO .fixed bug #543 #544
		CCDirector::sharedDirector()->setProjection(CCDirector::sharedDirector()->getProjection());
		CCDirector::sharedDirector()->applyOrientation();
		blit();
	}

	void CCGridBase::blit(void)
	{
		CCAssert(0, "");
	}

	void CCGridBase::reuse(void)
	{
		CCAssert(0, "");
	}

	void CCGridBase::calculateVertexPoints(void)
	{
		CCAssert(0, "");
	}
	
	void CCGridBase::initVertexBuffer()
	{
		CCAssert(0, "");
	}

	void CCGridBase::RenderVertexBuffer()
	{
		CCAssert(0, "");
	}
	
	void CCGrid3D::initVertexBuffer()
	{
		m_indexCount = m_sGridSize.x * m_sGridSize.y * 6;
		unsigned long* indices;
		indices = new unsigned long[m_indexCount];
		if(!indices)
		{
			return ;
		}
		memset(indices, 0, (sizeof(unsigned long) * m_indexCount));

		m_vertexCount = (m_sGridSize.x+1) * (m_sGridSize.y+1);
		
		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;
		CCushort *idxArray = m_pIndices;

		for (int x = 0; x < m_sGridSize.x; ++x)
		{
			for (int y = 0; y < m_sGridSize.y; ++y)
			{
				int idx = (y * m_sGridSize.x) + x;

				unsigned long tempidx[6] = {idxArray[6*idx], idxArray[6*idx+1], idxArray[6*idx+2], idxArray[6*idx+3], idxArray[6*idx+4], idxArray[6*idx+5]};
				memcpy(&indices[6*idx], tempidx, 6*sizeof(unsigned long));
			}
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Now create the vertex buffer.
		if(FAILED(CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer)))
		{
			delete[] indices;
			return ;
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		indexData.pSysMem = indices;
		CCID3D11Device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

		CC_SAFE_DELETE_ARRAY(indices);
	}

	void CCGrid3D::RenderVertexBuffer()
	{
		m_vertexCount = (m_sGridSize.x+1) * (m_sGridSize.y+1);
		VertexType* vertices = new VertexType[m_vertexCount];

		if ( !vertices )
		{
			return ;
		}

		memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));
		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;

		for (int x = 0; x < m_sGridSize.x+1; ++x)
		{
			for (int y = 0; y < m_sGridSize.y+1; ++y)
			{
				int idx = (y * (m_sGridSize.x+1)) + x;

				vertices[idx].position = XMFLOAT3(vertArray[3*idx], vertArray[3*idx+1], vertArray[3*idx+2]);
				vertices[idx].texture = XMFLOAT2(texArray[2*idx], texArray[2*idx+1]);
			}
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		VertexType* verticesPtr;

		// Lock the vertex buffer so it can be written to.
		if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return ;
		}
		// Get a pointer to the data in the vertex buffer.
		verticesPtr = (VertexType*)mappedResource.pData;
		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, vertices, (sizeof(VertexType) * m_vertexCount));
		// Unlock the vertex buffer.
		CCID3D11DeviceContext->Unmap(m_vertexBuffer, 0);

		CC_SAFE_DELETE_ARRAY(vertices);

		////////////////////////
		unsigned int stride;
		unsigned int offset;
		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType); 
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		CCID3D11DeviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return;
	}

	bool CCGridBase::InitializeShader()
	{
		HRESULT result;
		ID3D10Blob* errorMessage;
		D3D11_BUFFER_DESC matrixBufferDesc;
		// Initialize the pointers this function will use to null.
		errorMessage = 0;

		BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		loader->LoadShader(
			L"CCGridVertexShader.cso",
			layoutDesc,
			ARRAYSIZE(layoutDesc),
			&m_vertexShader,
			&m_layout
			);

		loader->LoadShader(
			L"CCGridPixelShader.cso",
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

	void CCGridBase::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

		//// Pop a message up on the screen to notify the user to check the text file for compile errors.
		//MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

		return;
	}

	bool CCGridBase::SetShaderParameters(XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
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

	void CCGridBase::RenderShader()
	{
		CCID3D11DeviceContext->IASetInputLayout(m_layout);
		CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
		CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
		CCID3D11DeviceContext->PSSetSamplers(0, 1, m_pTexture->GetSamplerState());
		CCID3D11DeviceContext->DrawIndexed( m_indexCount, 0, 0 );
	}

	void CCGridBase::Render()
	{
// 		if ( getIsDepthTest())
// 		{
// 			CCDirector::sharedDirector()->setDepthTest(true);
// 			CCID3D11DeviceContext->ClearDepthStencilView(CCD3DCLASS->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
// 		}
// 		else
// 		{
// 			CCDirector::sharedDirector()->setDepthTest(false);
// 		}
		XMMATRIX viewMatrix, projectionMatrix;

		// Get the world, view, and projection matrices from the camera and d3d objects.
		CCD3DCLASS->GetViewMatrix(viewMatrix);
		CCD3DCLASS->GetProjectionMatrix(projectionMatrix);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		RenderVertexBuffer();

		// Set the shader parameters that it will use for rendering.
		SetShaderParameters(viewMatrix, projectionMatrix, m_pTexture->getTextureResource());

		// Now render the prepared buffers with the shader.
		RenderShader();
	}

	// implementation of CCGrid3D

	CCGrid3D* CCGrid3D::gridWithSize(const ccGridSize& gridSize, CCTexture2D *pTexture, bool bFlipped)
	{
		CCGrid3D *pRet= new CCGrid3D();

		if (pRet)
		{
			if (pRet->initWithSize(gridSize, pTexture, bFlipped))
			{
				pRet->autorelease();
			}
			else
			{
				delete pRet;
				pRet = NULL;
			}
		}

		return pRet;
	}

	CCGrid3D* CCGrid3D::gridWithSize(const ccGridSize& gridSize)
	{
		CCGrid3D *pRet= new CCGrid3D();

		if (pRet)
		{
			if (pRet->initWithSize(gridSize))
			{
				pRet->autorelease();
			}
			else
			{
				delete pRet;
				pRet = NULL;
			}
		}

		return pRet;
	}

	CCGrid3D::~CCGrid3D(void)
	{

	}

	void CCGrid3D::blit(void)
	{
		int n = m_sGridSize.x * m_sGridSize.y;

		Render();
	}

	void CCGrid3D::calculateVertexPoints(void)
	{
		float width = (float)m_pTexture->getPixelsWide();
		float height = (float)m_pTexture->getPixelsHigh();
		float imageH = m_pTexture->getContentSizeInPixels().height;

		int x, y, i;

		m_pVertices = malloc((m_sGridSize.x+1) * (m_sGridSize.y+1) * sizeof(ccVertex3F));
		m_pOriginalVertices = malloc((m_sGridSize.x+1) * (m_sGridSize.y+1) * sizeof(ccVertex3F));
		m_pTexCoordinates = malloc((m_sGridSize.x+1) * (m_sGridSize.y+1) * sizeof(CCPoint));
		m_pIndices = (CCushort*)malloc(m_sGridSize.x * m_sGridSize.y * sizeof(CCushort) * 6);

		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;
		CCushort *idxArray = m_pIndices;

		for (x = 0; x < m_sGridSize.x; ++x)
		{
			for (y = 0; y < m_sGridSize.y; ++y)
			{
				int idx = (y * m_sGridSize.x) + x;

				float x1 = x * m_obStep.x;
				float x2 = x1 + m_obStep.x;
				float y1 = y * m_obStep.y;
				float y2= y1 + m_obStep.y;

				CCushort a = (CCushort)(x * (m_sGridSize.y + 1) + y);
				CCushort b = (CCushort)((x + 1) * (m_sGridSize.y + 1) + y);
				CCushort c = (CCushort)((x + 1) * (m_sGridSize.y + 1) + (y + 1));
				CCushort d = (CCushort)(x * (m_sGridSize.y + 1) + (y + 1));

				CCushort tempidx[6] = {a, b, d, b, c, d};

				memcpy(&idxArray[6*idx], tempidx, 6*sizeof(CCushort));

				int l1[4] = {a*3, b*3, c*3, d*3};
				ccVertex3F e = {x1, y1, 0};
				ccVertex3F f = {x2, y1, 0};
				ccVertex3F g = {x2, y2, 0};
				ccVertex3F h = {x1, y2, 0};

				ccVertex3F l2[4] = {e, f, g, h};

				int tex1[4] = {a*2, b*2, c*2, d*2};
				CCPoint tex2[4] = {ccp(x1, y1), ccp(x2, y1), ccp(x2, y2), ccp(x1, y2)};

				for (i = 0; i < 4; ++i)
				{
					vertArray[l1[i]] = l2[i].x;
					vertArray[l1[i] + 1] = l2[i].y;
					vertArray[l1[i] + 2] = l2[i].z;

					texArray[tex1[i]] = tex2[i].x / width;
					if (!m_bIsTextureFlipped)//dx!!!!!!!
					{
						texArray[tex1[i] + 1] = (imageH - tex2[i].y) / height;
					}
					else
					{
						texArray[tex1[i] + 1] = tex2[i].y / height;
					}
				}
			}
		}

		memcpy(m_pOriginalVertices, m_pVertices, (m_sGridSize.x+1) * (m_sGridSize.y+1) * sizeof(ccVertex3F));
	}

	ccVertex3F CCGrid3D::vertex(const ccGridSize& pos)
	{
		int index = (pos.x * (m_sGridSize.y+1) + pos.y) * 3;
		float *vertArray = (float*)m_pVertices;

		ccVertex3F vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

		return vert;
	}

	ccVertex3F CCGrid3D::originalVertex(const ccGridSize& pos)
	{
		int index = (pos.x * (m_sGridSize.y+1) + pos.y) * 3;
		float *vertArray = (float*)m_pOriginalVertices;

		ccVertex3F vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

		return vert;
	}

	void CCGrid3D::setVertex(const ccGridSize& pos, const ccVertex3F& vertex)
	{
		int index = (pos.x * (m_sGridSize.y + 1) + pos.y) * 3;
		float *vertArray = (float*)m_pVertices;
		vertArray[index] = vertex.x;
		vertArray[index+1] = vertex.y;
		vertArray[index+2] = vertex.z;
	}

	void CCGrid3D::reuse(void)
	{
		if (m_nReuseGrid > 0)
		{
			memcpy(m_pOriginalVertices, m_pVertices, (m_sGridSize.x+1) * (m_sGridSize.y+1) * sizeof(ccVertex3F));
			--m_nReuseGrid;
		}
	}

	// implementation of CCTiledGrid3D

	CCTiledGrid3D::~CCTiledGrid3D(void)
	{
	}

	CCTiledGrid3D* CCTiledGrid3D::gridWithSize(const ccGridSize& gridSize, CCTexture2D *pTexture, bool bFlipped)
	{
		CCTiledGrid3D *pRet= new CCTiledGrid3D();

		if (pRet)
		{
			if (pRet->initWithSize(gridSize, pTexture, bFlipped))
			{
				pRet->autorelease();
			}
			else
			{
				delete pRet;
				pRet = NULL;
			}
		}

		return pRet;
	}

	CCTiledGrid3D* CCTiledGrid3D::gridWithSize(const ccGridSize& gridSize)
	{
		CCTiledGrid3D *pRet= new CCTiledGrid3D();

		if (pRet)
		{
			if (pRet->initWithSize(gridSize))
			{
				pRet->autorelease();
			}
			else
			{
				delete pRet;
				pRet = NULL;
			}
		}

		return pRet;
	}

	void CCTiledGrid3D::blit(void)
	{
		int n = m_sGridSize.x * m_sGridSize.y;
		Render();

	}

	void CCTiledGrid3D::calculateVertexPoints(void)
	{
		float width = (float)m_pTexture->getPixelsWide();
		float height = (float)m_pTexture->getPixelsHigh();
		float imageH = m_pTexture->getContentSizeInPixels().height;

		int numQuads = m_sGridSize.x * m_sGridSize.y;

		m_pVertices = malloc(numQuads * 12 * sizeof(CCfloat));
		m_pOriginalVertices = malloc(numQuads * 12 * sizeof(CCfloat));
		m_pTexCoordinates = malloc(numQuads * 8 * sizeof(CCfloat));
		m_pIndices = (CCushort *)malloc(numQuads * 6 * sizeof(CCushort));

		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;
		CCushort *idxArray = m_pIndices;

		int x, y;

		for( x = 0; x < m_sGridSize.x; x++ )
		{
			for( y = 0; y < m_sGridSize.y; y++ )
			{
				float x1 = x * m_obStep.x;
				float x2 = x1 + m_obStep.x;
				float y1 = y * m_obStep.y;
				float y2 = y1 + m_obStep.y;

				*vertArray++ = x1;
				*vertArray++ = y1;
				*vertArray++ = 0;
				*vertArray++ = x2;
				*vertArray++ = y1;
				*vertArray++ = 0;
				*vertArray++ = x1;
				*vertArray++ = y2;
				*vertArray++ = 0;
				*vertArray++ = x2;
				*vertArray++ = y2;
				*vertArray++ = 0;

				float newY1 = y1;
				float newY2 = y2;

				if (!m_bIsTextureFlipped) //dx!!!!!!!!!!!
				{
					newY1 = imageH - y1;
					newY2 = imageH - y2;
				}

				*texArray++ = x1 / width;
				*texArray++ = newY1 / height;
				*texArray++ = x2 / width;
				*texArray++ = newY1 / height;
				*texArray++ = x1 / width;
				*texArray++ = newY2 / height;
				*texArray++ = x2 / width;
				*texArray++ = newY2 / height;
			}
		}

		for (x = 0; x < numQuads; x++)
		{
			idxArray[x*6+0] = (CCushort)(x * 4 + 0);
			idxArray[x*6+1] = (CCushort)(x * 4 + 1);
			idxArray[x*6+2] = (CCushort)(x * 4 + 2);

			idxArray[x*6+3] = (CCushort)(x * 4 + 1);
			idxArray[x*6+4] = (CCushort)(x * 4 + 2);
			idxArray[x*6+5] = (CCushort)(x * 4 + 3);
		}

		memcpy(m_pOriginalVertices, m_pVertices, numQuads * 12 * sizeof(CCfloat));
	}

	void CCTiledGrid3D::setTile(const ccGridSize& pos, const ccQuad3& coords)
	{
		int idx = (m_sGridSize.y * pos.x + pos.y) * 4 * 3;
		float *vertArray = (float*)m_pVertices;
		memcpy(&vertArray[idx], &coords, sizeof(ccQuad3));
	}

	ccQuad3 CCTiledGrid3D::originalTile(const ccGridSize& pos)
	{
		int idx = (m_sGridSize.y * pos.x + pos.y) * 4 * 3;
		float *vertArray = (float*)m_pOriginalVertices;

		ccQuad3 ret;
		memcpy(&ret, &vertArray[idx], sizeof(ccQuad3));

		return ret;
	}

	ccQuad3 CCTiledGrid3D::tile(const ccGridSize& pos)
	{
		int idx = (m_sGridSize.y * pos.x + pos.y) * 4 * 3;
		float *vertArray = (float*)m_pVertices;

		ccQuad3 ret;
		memcpy(&ret, &vertArray[idx], sizeof(ccQuad3));

		return ret;
	}

	void CCTiledGrid3D::reuse(void)
	{
		if (m_nReuseGrid > 0)
		{
			int numQuads = m_sGridSize.x * m_sGridSize.y;

			memcpy(m_pOriginalVertices, m_pVertices, numQuads * 12 * sizeof(CCfloat));
			--m_nReuseGrid;
		}
	}

	void CCTiledGrid3D::initVertexBuffer()
	{
		m_indexCount = m_sGridSize.x * m_sGridSize.y * 6;
		unsigned long* indices;
		indices = new unsigned long[m_indexCount];
		if(!indices)
		{
			return ;
		}
		memset(indices, 0, (sizeof(unsigned long) * m_indexCount));

		m_vertexCount = (m_sGridSize.x+1) * (m_sGridSize.y+1) * 4;
		
		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;
		CCushort *idxArray = m_pIndices;

		for (int x = 0; x < m_sGridSize.x; ++x)
		{
			for (int y = 0; y < m_sGridSize.y; ++y)
			{
				int idx = (y * m_sGridSize.x) + x;

				unsigned long tempidx[6] = {idxArray[6*idx], idxArray[6*idx+1], idxArray[6*idx+2], idxArray[6*idx+3], idxArray[6*idx+4], idxArray[6*idx+5]};
				memcpy(&indices[6*idx], tempidx, 6*sizeof(unsigned long));
			}
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Now create the vertex buffer.
		if(FAILED(CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer)))
		{
			return ;
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		indexData.pSysMem = indices;
		CCID3D11Device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

		CC_SAFE_DELETE_ARRAY(indices);
	}

	void CCTiledGrid3D::RenderVertexBuffer()
	{
		m_vertexCount = (m_sGridSize.x+1) * (m_sGridSize.y+1) * 4;
		VertexType* vertices = new VertexType[m_vertexCount];

		if ( !vertices )
		{
			return ;
		}

		memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));
		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;

		for (int x = 0; x < m_sGridSize.x; ++x)
		{
			for (int y = 0; y < m_sGridSize.y; ++y)
			{
				int idx = (y * m_sGridSize.x) + x;
				vertices[4*idx].position = XMFLOAT3(vertArray[12*idx], vertArray[12*idx+1], vertArray[12*idx+2]);
				vertices[4*idx+1].position = XMFLOAT3(vertArray[12*idx+3], vertArray[12*idx+4], vertArray[12*idx+5]);
				vertices[4*idx+2].position = XMFLOAT3(vertArray[12*idx+6], vertArray[12*idx+7], vertArray[12*idx+8]);
				vertices[4*idx+3].position = XMFLOAT3(vertArray[12*idx+9], vertArray[12*idx+10], vertArray[12*idx+11]);
				
				vertices[4*idx].texture = XMFLOAT2(texArray[8*idx], texArray[8*idx+1]);
				vertices[4*idx+1].texture = XMFLOAT2(texArray[8*idx+2], texArray[8*idx+3]);
				vertices[4*idx+2].texture = XMFLOAT2(texArray[8*idx+4], texArray[8*idx+5]);
				vertices[4*idx+3].texture = XMFLOAT2(texArray[8*idx+6], texArray[8*idx+7]);
			}
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		VertexType* verticesPtr;

		// Lock the vertex buffer so it can be written to.
		if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return;
		}
		// Get a pointer to the data in the vertex buffer.
		verticesPtr = (VertexType*)mappedResource.pData;
		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, vertices, (sizeof(VertexType) * m_vertexCount));
		// Unlock the vertex buffer.
		CCID3D11DeviceContext->Unmap(m_vertexBuffer, 0);

		CC_SAFE_DELETE_ARRAY(vertices);

		////////////////////////
		unsigned int stride;
		unsigned int offset;
		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType); 
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		CCID3D11DeviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return;
	}

} // end of namespace cocos2d
