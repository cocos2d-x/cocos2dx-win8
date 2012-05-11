/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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

#include "CCDrawingPrimitives.h"
#include "ccTypes.h"
#include "ccMacros.h"
#include "CCGL.h"
#include "CCDirector.h"
#include "D3Dcommon.h"
#include <string.h>
#include <cmath>
#include "DirectXHelper.h"
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

#ifndef M_PI
	#define M_PI       3.14159265358979323846
#endif

namespace   cocos2d {
	static CCDrawingPrimitive *pSharedDrawingPrimitive = NULL;

void ccDrawPoint(const CCPoint& point)
{
	ccVertex2F* vertice = new ccVertex2F[1];
	vertice[0].x = point.x * CC_CONTENT_SCALE_FACTOR();
	vertice[0].y = point.y * CC_CONTENT_SCALE_FACTOR();
	CCDrawingPrimitive::Drawing(vertice, 1, DrawingPoints);	
}

void ccDrawPoints(const CCPoint *points, unsigned int numberOfPoints)
{
	ccVertex2F* vertices = new ccVertex2F[numberOfPoints];
	for (int i=0;i<numberOfPoints;i++)
	{
		vertices[i].x = points[i].x * CC_CONTENT_SCALE_FACTOR();
		vertices[i].y = points[i].y * CC_CONTENT_SCALE_FACTOR();
	}

	CCDrawingPrimitive::Drawing(vertices, numberOfPoints,DrawingPoints);	
}

void ccDrawLine(const CCPoint& origin, const CCPoint& destination)
{
	ccVertex2F vertices[2] = 
    {
        {origin.x * CC_CONTENT_SCALE_FACTOR(), origin.y * CC_CONTENT_SCALE_FACTOR()},
        {destination.x * CC_CONTENT_SCALE_FACTOR(), destination.y * CC_CONTENT_SCALE_FACTOR()},
    };
	
	CCDrawingPrimitive::Drawing(vertices, 2, DrawingLines);
	
}

void ccDrawPoly(const CCPoint *poli, int numberOfPoints, bool closePolygon){
	ccDrawPoly(poli,numberOfPoints,closePolygon,false);
}
void ccDrawPoly(const CCPoint *poli, int numberOfPoints, bool closePolygon, bool fill)
{
	
	if (closePolygon)
	{
		ccVertex2F* vertices = new ccVertex2F[numberOfPoints+1];
		for (int i=0;i<numberOfPoints;i++)
		{
			vertices[i].x = poli[i].x * CC_CONTENT_SCALE_FACTOR();
			vertices[i].y = poli[i].y * CC_CONTENT_SCALE_FACTOR();
		}
		vertices[numberOfPoints].x = poli[0].x * CC_CONTENT_SCALE_FACTOR();
		vertices[numberOfPoints].y = poli[0].y * CC_CONTENT_SCALE_FACTOR();
		CCDrawingPrimitive::Drawing(vertices, numberOfPoints+1,DrawingPolyClosed);
	} 
	else
	{
		ccVertex2F* vertices = new ccVertex2F[numberOfPoints];
		for (int i=0;i<numberOfPoints;i++)
		{
			vertices[i].x = poli[i].x * CC_CONTENT_SCALE_FACTOR();
			vertices[i].y = poli[i].y * CC_CONTENT_SCALE_FACTOR();
		}
		CCDrawingPrimitive::Drawing(vertices, numberOfPoints,DrawingPolyOpened);
	}
	
}

void ccDrawCircle(const CCPoint& center, float r, float a, int segs, bool drawLineToCenter)
{
	int additionalSegment = 1;
	if (drawLineToCenter)
	{
		++additionalSegment;
	}

	const float coef = 2.0f * (float) (M_PI) /segs;

	ccVertex2F* vertices = new ccVertex2F[segs+1];
	if( ! vertices )
	{
		return;
	}
	
	for(int i=0;i<=segs;i++)
	{
		float rads = i*coef;
		vertices[i].x = (r * cosf(rads + a) + center.x) * CC_CONTENT_SCALE_FACTOR();
		vertices[i].y = (r * sinf(rads + a) + center.y) * CC_CONTENT_SCALE_FACTOR();
	
	}
	vertices[segs+1].x = center.x * CC_CONTENT_SCALE_FACTOR();
	vertices[segs+1].y = center.y * CC_CONTENT_SCALE_FACTOR();	

	CCDrawingPrimitive::Drawing(vertices, segs+additionalSegment,DrawingPolyOpened);

}

void ccDrawQuadBezier(const CCPoint& origin, const CCPoint& control, const CCPoint& destination, int segments)
{
	ccVertex2F* vertices = new ccVertex2F[segments + 1];
	
	float t = 0.0f;
	for(int i = 0; i < segments; i++)
	{
		float x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
		float y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
		vertices[i].x = x * CC_CONTENT_SCALE_FACTOR();
		vertices[i].y = y * CC_CONTENT_SCALE_FACTOR();
		t += 1.0f / segments;
	}

	vertices[segments].x = destination.x  * CC_CONTENT_SCALE_FACTOR();
	vertices[segments].y = destination.y  * CC_CONTENT_SCALE_FACTOR();
	
	CCDrawingPrimitive::Drawing(vertices, segments+1,DrawingPolyOpened);		
}

void ccDrawCubicBezier(const CCPoint& origin, const CCPoint& control1, const CCPoint& control2, const CCPoint& destination, int segments)
{
	ccVertex2F *vertices = new ccVertex2F[segments + 1];
	
	float t = 0;
	for(int i = 0; i < segments; ++i)
	{
		float x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
		float y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
		vertices[i].x = x * CC_CONTENT_SCALE_FACTOR();
		vertices[i].y = y * CC_CONTENT_SCALE_FACTOR();
		t += 1.0f / segments;
	}
	vertices[segments].x = destination.x  * CC_CONTENT_SCALE_FACTOR();
	vertices[segments].y = destination.y  * CC_CONTENT_SCALE_FACTOR();

	CCDrawingPrimitive::Drawing(vertices, segments+1,DrawingPolyOpened);
}

void CCDrawingPrimitive::D3DColor4f(float red, float green, float blue, float alpha)
{
	if (! pSharedDrawingPrimitive)
	{
		pSharedDrawingPrimitive = new CCDrawingPrimitive();
	}

	pSharedDrawingPrimitive->m_currentColor.x = red;
	pSharedDrawingPrimitive->m_currentColor.y = green;
	pSharedDrawingPrimitive->m_currentColor.z = blue;
	pSharedDrawingPrimitive->m_currentColor.w = alpha;
};

void CCDrawingPrimitive::Drawing(ccVertex2F *vertices, unsigned int numberOfPoints, DXDrawingType type)
{
	
	if (! pSharedDrawingPrimitive)
	{
		pSharedDrawingPrimitive = new CCDrawingPrimitive();
	}
	pSharedDrawingPrimitive->m_DarwingType = type;
	pSharedDrawingPrimitive->m_vertexAmount = numberOfPoints;
	pSharedDrawingPrimitive->m_vertices = vertices;
	pSharedDrawingPrimitive->Render();

}

void CCDrawingPrimitive::Drawing3D(ccVertex3F *vertices, unsigned int numberOfPoints, DXDrawingType type)
{

	if (! pSharedDrawingPrimitive)
	{
		pSharedDrawingPrimitive = new CCDrawingPrimitive();
	}
	pSharedDrawingPrimitive->m_DarwingType = type;
	pSharedDrawingPrimitive->m_vertexAmount = numberOfPoints;
	pSharedDrawingPrimitive->m_vertices3D = vertices;
	pSharedDrawingPrimitive->Render3D();

}

CCDrawingPrimitive::CCDrawingPrimitive()
{

	InitializeShader();
	initVertexBuffer(200);

	m_currentColor = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
}

CCDrawingPrimitive::~CCDrawingPrimitive()
{
	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	/*
	if ( m_indexBuffer )
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	*/
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	//*
	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	//*/
	m_vertices =NULL;
}

void CCDrawingPrimitive::initVertexBuffer(unsigned int numberOfPoints)
{

	VertexType* verticesT = new VertexType[numberOfPoints];
	if ( !verticesT )
	{
		//
	}
	memset(verticesT, 0, (sizeof(VertexType) * numberOfPoints));
	/*
	for (int i=0; i<numberOfPoints; i++)
	{
		verticesT[i].position = XMFLOAT3((vertices[i]).x, (vertices[i]).y, 1.0f);
		verticesT[i].color = m_currentColor;
	}
	*/
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*numberOfPoints;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = verticesT;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = CCID3D11Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return ;
	}

	if ( verticesT )
	{
		delete[] verticesT;
		verticesT = 0;
	}
	/*
	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = indices;
	pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	*/
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory( &matrixBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
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
		return ;
	}
}

void CCDrawingPrimitive::RenderVertexBuffer()
{
	
	VertexType* verticesTmp;
	// Create the vertex array.
	verticesTmp = new VertexType[m_vertexAmount];
	if(!verticesTmp)
	{
		return ;
	}

	memset(verticesTmp, 0, (sizeof(VertexType) * m_vertexAmount));

	for (int i=0; i<m_vertexAmount; i++)
	{
		verticesTmp[i].position = XMFLOAT3((m_vertices[i]).x, (m_vertices[i]).y, 1.0f);
		verticesTmp[i].color = m_currentColor;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;
	if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))){return ;}
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)verticesTmp, (sizeof(VertexType) * m_vertexAmount));
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

	//CCID3D11DeviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	if (m_DarwingType==DrawingPoints)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	} 
	else if(m_DarwingType==DrawingLines)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if(m_DarwingType==DrawingTrangles)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if(m_DarwingType==DrawingPolyClosed)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	else if(m_DarwingType==DrawingPolyOpened)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	
	return;
}

void CCDrawingPrimitive::RenderVertexBuffer3D()
{

	VertexType* verticesTmp;
	// Create the vertex array.
	verticesTmp = new VertexType[m_vertexAmount];
	if(!verticesTmp)
	{
		return ;
	}

	memset(verticesTmp, 0, (sizeof(VertexType) * m_vertexAmount));

	for (int i=0; i<m_vertexAmount; i++)
	{
		verticesTmp[i].position = XMFLOAT3((m_vertices3D[i]).x, (m_vertices3D[i]).y, (m_vertices3D[i]).z);
		verticesTmp[i].color = m_currentColor;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))){return ;}
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)verticesTmp, (sizeof(VertexType) * m_vertexAmount));
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

	//CCID3D11DeviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	if (m_DarwingType==DrawingPoints)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	} 
	else if(m_DarwingType==DrawingLines)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if(m_DarwingType==DrawingTrangles)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if(m_DarwingType==DrawingPolyClosed)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	else if(m_DarwingType==DrawingPolyOpened)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}

	return;
}

bool CCDrawingPrimitive::InitializeShader()
{
	BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	loader->LoadShader(
		L"CCDrawingVertexShader.cso",
		layoutDesc,
		ARRAYSIZE(layoutDesc),
		&m_vertexShader,
		&m_layout
		);

	loader->LoadShader(
		L"CCDrawingPixelShader.cso",
		&m_pixelShader
		);

	return true;
}

void CCDrawingPrimitive::OutputShaderErrorMessage(ID3D10Blob* errorMessage,WCHAR* shaderFilename)
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
	//MessageBox(CCHWND, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool CCDrawingPrimitive::SetShaderParameters(XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	MatrixBufferType* dataPtr;
	if(FAILED(CCID3D11DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))){return false;}
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	CCID3D11DeviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;
	CCID3D11DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void CCDrawingPrimitive::RenderShader()
{
	// Set the vertex input layout.
	CCID3D11DeviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	//CCID3D11DeviceContext->DrawIndexed( 6, 0, 0 );
	CCID3D11DeviceContext->Draw( m_vertexAmount, 0 );

	return;
}


void CCDrawingPrimitive::Render()
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderVertexBuffer();

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(viewMatrix, projectionMatrix);
	if(!result)
	{
		return ;
	}

	// Now render the prepared buffers with the shader.
	RenderShader();
}

void CCDrawingPrimitive::Render3D()
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderVertexBuffer3D();

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(viewMatrix, projectionMatrix);
	if(!result)
	{
		return ;
	}

	// Now render the prepared buffers with the shader.
	RenderShader();
}

}//namespace   cocos2d 
