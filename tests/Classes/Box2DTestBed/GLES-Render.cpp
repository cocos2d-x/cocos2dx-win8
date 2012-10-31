#include "pch.h"
#include "GLES-Render.h"
#include "CCGL.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <cmath>
#include "DirectXHelper.h"
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

extern HRESULT LoadVertexShader(
	_In_ ID3D11Device* d3dDevice,
	_In_ WCHAR* szFileName,
	_In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
	_In_ uint32_t layoutDescNumElements,
	_Out_ ID3D11VertexShader** shader,
	_Out_opt_ ID3D11InputLayout** layout
	);

extern HRESULT LoadPixelShader(
	_In_ ID3D11Device* d3dDevice,
	_In_ WCHAR* szFileName,
	_Out_ ID3D11PixelShader** shader
	);

GLESDebugDraw::GLESDebugDraw()
	: mRatio( 1.0f )
{
	InitializeShader(NULL);
	initVertexBuffer(200);
	m_currentColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}
GLESDebugDraw::GLESDebugDraw( float32 ratio )
	: mRatio( ratio )
{
	InitializeShader(NULL);
	initVertexBuffer(200);
	m_currentColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

GLESDebugDraw::~GLESDebugDraw()
{
	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
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
}

void GLESDebugDraw::DrawPolygon(const b2Vec2* old_vertices, int vertexCount, const b2Color& color)
{
	ccVertex2F* vertices = new ccVertex2F[vertexCount+1];
	for( int i=0;i<vertexCount;i++) 
	{
		vertices[i].x = old_vertices[i].x * mRatio;
		vertices[i].y = old_vertices[i].y * mRatio;		
	}
	vertices[vertexCount].x = old_vertices[0].x * mRatio;
	vertices[vertexCount].y = old_vertices[0].y * mRatio;	

	m_currentColor = XMFLOAT4(color.r, color.g, color.b,1);
	Drawing(vertices,vertexCount+1,DrawingBox2DPolyClosed);

	delete[] vertices;
}

void GLESDebugDraw::DrawSolidPolygon(const b2Vec2* old_vertices, int vertexCount, const b2Color& color)
{
	ccVertex2F* verticesTrangles = new ccVertex2F[(vertexCount-2)*3];
	for( int i=0,j=0;i<(vertexCount-2);i++,j+=3) 
	{
		verticesTrangles[j].x   = old_vertices[0].x * mRatio;
		verticesTrangles[j].y   = old_vertices[0].y * mRatio;		
		verticesTrangles[j+1].x = old_vertices[i+1].x * mRatio;
		verticesTrangles[j+1].y = old_vertices[i+1].y * mRatio;	
		verticesTrangles[j+2].x = old_vertices[i+2].x * mRatio;
		verticesTrangles[j+2].y = old_vertices[i+2].y * mRatio;	
	}
		
	m_currentColor = XMFLOAT4(color.r, color.g, color.b,0.5f);
	Drawing(verticesTrangles,(vertexCount-2)*3,DrawingBox2DTrangle);

	delete[] verticesTrangles;

	ccVertex2F* vertices = new ccVertex2F[vertexCount+1];
	for( int i=0;i<vertexCount;i++) 
	{
		vertices[i].x = old_vertices[i].x * mRatio;
		vertices[i].y = old_vertices[i].y * mRatio;		
	}
	vertices[vertexCount].x = old_vertices[0].x * mRatio;
	vertices[vertexCount].y = old_vertices[0].y * mRatio;
	
	m_currentColor = XMFLOAT4(color.r, color.g, color.b,1);
	Drawing(vertices,vertexCount+1,DrawingBox2DPolyClosed);

	delete[] vertices;
}

void GLESDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
		
	const float32 k_segments = 16.0f;
	int vertexCount=16;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	
	ccVertex2F*	glVertices = new ccVertex2F[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertices[i].x = v.x * mRatio;
		glVertices[i].y = v.y * mRatio;
		theta += k_increment;
	}

	ccVertex2F* verticesTrangles = new ccVertex2F[vertexCount*3];
	for( int i=0,j=0;i<(vertexCount-1);i++,j+=3) 
	{
		verticesTrangles[j].x   = center.x * mRatio;
		verticesTrangles[j].y   = center.y * mRatio;		
		verticesTrangles[j+1].x = glVertices[i].x * mRatio;
		verticesTrangles[j+1].y = glVertices[i].y * mRatio;	
		verticesTrangles[j+2].x = glVertices[i+1].x * mRatio;
		verticesTrangles[j+2].y = glVertices[i+1].y * mRatio;	
	}
	
	verticesTrangles[vertexCount-2].x   = center.x * mRatio;
	verticesTrangles[vertexCount-2].y   = center.y * mRatio;		
	verticesTrangles[vertexCount-1].x   = glVertices[vertexCount].x * mRatio;
	verticesTrangles[vertexCount-1].y   = glVertices[vertexCount].y * mRatio;	
	verticesTrangles[vertexCount].x     = glVertices[0].x * mRatio;
	verticesTrangles[vertexCount].y     = glVertices[0].y * mRatio;

	m_currentColor = XMFLOAT4(color.r, color.g, color.b,1);
	
	Drawing(verticesTrangles,vertexCount*3,DrawingBox2DTrangle);

	delete[] glVertices;
	delete[] verticesTrangles;
}

void GLESDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
		
	const float32 k_segments = 16.0f;
	int vertexCount=16;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	
	ccVertex2F*	glVertices = new ccVertex2F[vertexCount+1];
	for (int i = 0; i < vertexCount; i++)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertices[i].x = v.x * mRatio;
		glVertices[i].y = v.y * mRatio;
		theta += k_increment;
	}
	b2Vec2 v2 = center + radius * b2Vec2(cosf(0), sinf(0));
	glVertices[vertexCount].x = v2.x * mRatio;
	glVertices[vertexCount].y = v2.y * mRatio;

	ccVertex2F* verticesTrangles = new ccVertex2F[vertexCount*3];
	for( int i=0,j=0;i<(vertexCount);i++,j+=3) 
	{
		verticesTrangles[j].x   = center.x * mRatio;
		verticesTrangles[j].y   = center.y * mRatio;		
		verticesTrangles[j+1].x = glVertices[i].x * mRatio;
		verticesTrangles[j+1].y = glVertices[i].y * mRatio;	
		verticesTrangles[j+2].x = glVertices[i+1].x * mRatio;
		verticesTrangles[j+2].y = glVertices[i+1].y * mRatio;	
	}

	m_currentColor = XMFLOAT4(color.r, color.g, color.b,0.5);
	Drawing(verticesTrangles,vertexCount*3,DrawingBox2DTrangle);

	m_currentColor = XMFLOAT4(color.r, color.g, color.b,1);
	Drawing(glVertices,vertexCount+1,DrawingBox2DLines);
	
	// Draw the axis line
	DrawSegment(center,center+radius*axis,color);

	delete[] glVertices;
	delete[] verticesTrangles;
}

void GLESDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	m_currentColor = XMFLOAT4(color.r, color.g, color.b, 1.0f);
	ccVertex2F	glVertices[] = 
	{
		{p1.x * mRatio, p1.y * mRatio},
		{p2.x * mRatio, p2.y * mRatio}
	};
	
	Drawing(glVertices, 2, DrawingBox2DLines);
}

void GLESDebugDraw::DrawTransform(const b2Transform& xf)
{
	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;

	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1,p2,b2Color(1,0,0));
	
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1,p2,b2Color(0,1,0));
}

void GLESDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{	
	ccVertex2F			glVertices[] = {
		p.x * mRatio, p.y * mRatio
	};
	
	Drawing(glVertices, 1, DrawingBox2DPoints);
}

void GLESDebugDraw::DrawString(int x, int y, const char *string, ...)
{
//	NSLog(@"DrawString: unsupported: %s", string);
	//printf(string);
	/* Unsupported as yet. Could replace with bitmap font renderer at a later date */
}

void GLESDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	m_currentColor = XMFLOAT4(c.r, c.g, c.b,1);

	ccVertex2F				glVertices[] = {
		aabb->lowerBound.x * mRatio, aabb->lowerBound.y * mRatio,
		aabb->upperBound.x * mRatio, aabb->lowerBound.y * mRatio,
		aabb->upperBound.x * mRatio, aabb->upperBound.y * mRatio,
		aabb->lowerBound.x * mRatio, aabb->upperBound.y * mRatio,
		aabb->lowerBound.x * mRatio, aabb->lowerBound.y * mRatio,
	};
	Drawing(glVertices, 5, DrawingBox2DLines);
	
}

void GLESDebugDraw::Drawing(ccVertex2F *vertices, unsigned int numberOfPoints, DXBox2DDrawingType type)
{
	m_DarwingType = type;
	m_vertexAmount = numberOfPoints;
	m_vertices = vertices;
	Render();
}

void GLESDebugDraw::initVertexBuffer(unsigned int numberOfPoints)
{

	VertexType* verticesT = new VertexType[numberOfPoints];
	if ( !verticesT )
	{
		//
	}
	memset(verticesT, 0, (sizeof(VertexType) * numberOfPoints));

	D3D11_BUFFER_DESC vertexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*numberOfPoints;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;


	// Now create the vertex buffer.
	result = CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);
	if(FAILED(result))
	{
		return ;
	}

	if ( verticesT )
	{
		delete[] verticesT;
		verticesT = 0;
	}

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

void GLESDebugDraw::RenderVertexBuffer()
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
	if (m_DarwingType==DrawingBox2DPoints)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	} 
	else if(m_DarwingType==DrawingBox2DLines)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if(m_DarwingType==DrawingBox2DTrangle)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if(m_DarwingType==DrawingBox2DPolyClosed)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	else if(m_DarwingType==DrawingBox2DPolyOpened)
	{
		CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	
	return;
}

bool GLESDebugDraw::InitializeShader(WCHAR* fxFilename)
{
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	loader->LoadShader(
		L"CCDrawingVertexShader.cso",
		polygonLayout,
		ARRAYSIZE(polygonLayout),
		&m_vertexShader,
		&m_layout
		);

	loader->LoadShader(
		L"CCDrawingPixelShader.cso",
		&m_pixelShader
		);


	return true;
}

void GLESDebugDraw::OutputShaderErrorMessage(ID3D10Blob* errorMessage,WCHAR* shaderFilename)
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

bool GLESDebugDraw::SetShaderParameters(XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix)
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

void GLESDebugDraw::RenderShader()
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


void GLESDebugDraw::Render()
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
