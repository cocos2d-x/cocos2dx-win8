/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
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
#include "DirectXRender.h"

#include "CCEGLView.h"

#include "CCSet.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "CCTouch.h"
#include "CCTouchDispatcher.h"
#include "CCIMEDispatcher.h"
#include "CCKeypadDispatcher.h"
#include "CCApplication.h"

using namespace DirectX;
NS_CC_BEGIN;

static CCEGLView * s_pMainWindow;

CCEGLView::CCEGLView()
: m_pDelegate(NULL)
, m_fScreenScaleFactor(1.0f)
, m_fWinScaleX(1.0f)
, m_fWinScaleY(1.0f)
{
    m_rcViewPort.bottom = 0;
    m_rcViewPort.left = 0;
    m_rcViewPort.right = 0;
    m_rcViewPort.top = 0;

    m_d3dDevice = DirectXRender::SharedDXRender()->m_d3dDevice.Get();
    m_d3dContext = DirectXRender::SharedDXRender()->m_d3dContext.Get();
    m_swapChain = DirectXRender::SharedDXRender()->m_swapChain.Get();
    m_renderTargetView = DirectXRender::SharedDXRender()->m_renderTargetView.Get();
    m_depthStencilView = DirectXRender::SharedDXRender()->m_depthStencilView.Get();

    m_projectionMatrix = XMMatrixIdentity();
	m_viewMatrix = XMMatrixIdentity();
	mMatrixMode = -1;
}

CCEGLView::~CCEGLView()
{
}

ID3D11Device* CCEGLView::GetDevice()
{
    return m_d3dDevice;
}

ID3D11DeviceContext* CCEGLView::GetDeviceContext()
{
    return m_d3dContext;
}

ID3D11DepthStencilView* CCEGLView::GetDepthStencilView()
{
    return m_depthStencilView;
}

bool CCEGLView::Create()
{
	bool bRet = false;
	do 
	{
        DirectXRender^ render = DirectXRender::SharedDXRender();
        m_initWinWidth = (int)render->m_window->Bounds.Width;
        m_initWinHeight = (int)render->m_window->Bounds.Height;
        setDesignResolution(m_initWinWidth, m_initWinHeight);
        SetBackBufferRenderTarget();
        m_oldViewState = 0;//int(Windows::UI::ViewManagement::ApplicationView::Value);
		s_pMainWindow = this;
		bRet = true;
	} while (0);

	return bRet;
}

CCSize CCEGLView::getSize()
{
    return m_sizeInPoints;
}

CCSize CCEGLView::getSizeInPixel()
{
    return getSize();
}

bool CCEGLView::isOpenGLReady()
{
    return s_pMainWindow != NULL;
}

void CCEGLView::release()
{
	s_pMainWindow = NULL;

    SetMap::iterator setIter = m_pSets.begin();
    for ( ; setIter != m_pSets.end(); ++setIter)
    {
        CC_SAFE_DELETE(setIter->second);
    }
    m_pSets.clear();

    TouchMap::iterator touchIter = m_pTouches.begin();
    for ( ; touchIter != m_pTouches.end(); ++touchIter)
    {
        CC_SAFE_DELETE(touchIter->second);
    }
    m_pTouches.clear();

    CC_SAFE_DELETE(m_pDelegate);
    DirectXRender::SharedDXRender()->CloseWindow();
}

void CCEGLView::setTouchDelegate(EGLTouchDelegate * pDelegate)
{
    m_pDelegate = pDelegate;
}

void CCEGLView::render()
{
	DirectXRender::SharedDXRender()->Render();
}
void CCEGLView::swapBuffers()
{
    DirectXRender::SharedDXRender()->Present();
}

void CCEGLView::setViewPortInPoints(float x, float y, float w, float h)
{
	float factor = m_fScreenScaleFactor / CC_CONTENT_SCALE_FACTOR();
	
    D3DViewport(
        (int)(x * factor * m_fWinScaleX) + m_rcViewPort.left,
		(int)(y * factor * m_fWinScaleY) + m_rcViewPort.top,
		(int)(w * factor * m_fWinScaleX),
		(int)(h * factor * m_fWinScaleY));
}

void CCEGLView::setScissorInPoints(float x, float y, float w, float h)
{
    float factor = m_fScreenScaleFactor / CC_CONTENT_SCALE_FACTOR();
    // Switch coordinate system's origin from bottomleft(OpenGL) to topleft(DirectX)
    y = m_sizeInPoints.height - (y + h); 
    D3DScissor(
        (int)(x * factor * m_fWinScaleX) + m_rcViewPort.left,
		(int)(y * factor * m_fWinScaleY) + m_rcViewPort.top,
		(int)(w * factor * m_fWinScaleX),
		(int)(h * factor * m_fWinScaleY));
}

void CCEGLView::setIMEKeyboardState(bool /*bOpen*/)
{
}

void CCEGLView::getScreenRectInView(CCRect& rect)
{
	DirectXRender^ render = DirectXRender::SharedDXRender();
    float winWidth = render->m_window->Bounds.Width;
    float winHeight = render->m_window->Bounds.Height;
    
    rect.origin.x = float(- m_rcViewPort.left) / m_fScreenScaleFactor;
	rect.origin.y = float((m_rcViewPort.bottom - m_rcViewPort.top) - winHeight) / (2.0f * m_fScreenScaleFactor);
    rect.size.width = float(winWidth) / m_fScreenScaleFactor;
    rect.size.height = float(winHeight) / m_fScreenScaleFactor;
}

void CCEGLView::setScreenScale(float factor)
{
    m_fScreenScaleFactor = factor;
}

bool CCEGLView::canSetContentScaleFactor()
{
    return false;
}

void CCEGLView::setContentScaleFactor(float contentScaleFactor)
{
    m_fScreenScaleFactor = contentScaleFactor;
}

void CCEGLView::setDesignResolution(int dx, int dy)
{
    // 重新计算 contentScale 和 m_rcViewPort 
    m_sizeInPoints.width = (float)dx;
    m_sizeInPoints.height = (float)dy;
    
    DirectXRender^ render = DirectXRender::SharedDXRender();
    float winWidth = render->m_window->Bounds.Width;
    float winHeight = render->m_window->Bounds.Height;

    m_fScreenScaleFactor = min(winWidth / dx, winHeight / dy);
    m_fScreenScaleFactor *= CCDirector::sharedDirector()->getContentScaleFactor();

    int viewPortW = (int)(m_sizeInPoints.width * m_fScreenScaleFactor);
    int viewPortH = (int)(m_sizeInPoints.height * m_fScreenScaleFactor);

    // calculate client new width and height
    m_rcViewPort.left   = LONG((winWidth - viewPortW) / 2);
    m_rcViewPort.top    = LONG((winHeight - viewPortH) / 2);
    m_rcViewPort.right  = LONG(m_rcViewPort.left + viewPortW);
    m_rcViewPort.bottom = LONG(m_rcViewPort.top + viewPortH);
}

void CCEGLView::SetBackBufferRenderTarget()
{
    m_d3dContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void CCEGLView::D3DPerspective( FLOAT fovy, FLOAT aspect, FLOAT zNear, FLOAT zFar)
{
	CCfloat xmin, xmax, ymin, ymax;

	ymax = zNear * (CCfloat)tanf(fovy * (float)M_PI / 360);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	XMMATRIX tmpMatrix;
	tmpMatrix = XMMatrixPerspectiveOffCenterRH(xmin, xmax, ymin, ymax, zNear, zFar);
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}

void CCEGLView::D3DOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	XMMATRIX tmpMatrix;
	tmpMatrix = XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}

void CCEGLView::D3DLookAt(float fEyeX, float fEyeY, float fEyeZ, float fLookAtX, float fLookAtY, float fLookAtZ, float fUpX, float fUpY, float fUpZ)
{
	XMMATRIX tmpMatrix;
	tmpMatrix = XMMatrixLookAtRH(XMVectorSet(fEyeX,fEyeY,fEyeZ,0.f), XMVectorSet(fLookAtX,fLookAtY,fLookAtZ,0.f), XMVectorSet(fUpX,fUpY,fUpZ,0.f));
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}

void CCEGLView::D3DLoadIdentity()
{
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixIdentity();
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixIdentity();
	}
}

void CCEGLView::D3DViewport(int x, int y, int width, int height)
{
	D3D11_VIEWPORT viewport;

	// Setup the viewport for rendering.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = (float)x;
	viewport.TopLeftY = (float)y;

	// Create the viewport.
	m_d3dContext->RSSetViewports(1, &viewport);
}

void CCEGLView::D3DScissor(int x,int y,int w,int h)
{
	D3D11_RECT scissorRects;

	scissorRects.top = y;
	scissorRects.left = x;
	scissorRects.right = x+w;
	scissorRects.bottom = y+h;

	m_d3dContext->RSSetScissorRects(1,&scissorRects);
}

void CCEGLView::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void CCEGLView::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void CCEGLView::SetProjectionMatrix(const XMMATRIX& projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
	return;
}

void CCEGLView::SetViewMatrix(const XMMATRIX& viewMatrix)
{
	m_viewMatrix = viewMatrix;
	return;
}

void CCEGLView::D3DMatrixMode(int matrixMode)
{
	mMatrixMode = matrixMode;
}


void CCEGLView::D3DPushMatrix()
{
	MatrixStruct matrixStruct;
	matrixStruct.projection = m_projectionMatrix;
	matrixStruct.view = m_viewMatrix;
	m_MatrixStack.push(matrixStruct);
}
void CCEGLView::D3DPopMatrix()
{
	MatrixStruct matrixStruct;
	matrixStruct = m_MatrixStack.top();
	m_viewMatrix = matrixStruct.view;
	m_projectionMatrix = matrixStruct.projection;
	m_MatrixStack.pop();
}

//viewMatrix !!!m2*m2!!!
void CCEGLView::D3DTranslate(float x, float y, float z)
{
	XMMATRIX tmpMatrix;
	tmpMatrix = XMMatrixTranslation(x,y,z);
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}
void CCEGLView::D3DRotate(float angle, float x, float y, float z)
{
	XMMATRIX tmpMatrix;
	if ( x )
	{
		tmpMatrix = XMMatrixRotationX(angle);
		if ( mMatrixMode == CC_PROJECTION )
		{
			m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
		}
		else if ( mMatrixMode == CC_MODELVIEW )
		{
			m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
		}
	}
	if ( y )
	{
		tmpMatrix = XMMatrixRotationY(angle);
		if ( mMatrixMode == CC_PROJECTION )
		{
			m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
		}
		else if ( mMatrixMode == CC_MODELVIEW )
		{
			m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
		}
	}
	if ( z )
	{
		tmpMatrix = XMMatrixRotationZ(angle);
		if ( mMatrixMode == CC_PROJECTION )
		{
			m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
		}
		else if ( mMatrixMode == CC_MODELVIEW )
		{
			m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
		}
	}
}
void CCEGLView::D3DScale(float x, float y, float z)
{
	XMMATRIX tmpMatrix;
	tmpMatrix = XMMatrixScaling(x,y,z);
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}

void CCEGLView::D3DMultMatrix(const float *m)
{
	XMMATRIX tmpMatrix=XMMATRIX(m);
	if ( mMatrixMode == CC_PROJECTION )
	{
		m_projectionMatrix = XMMatrixMultiply(tmpMatrix,m_projectionMatrix);
	}
	else if ( mMatrixMode == CC_MODELVIEW )
	{
		m_viewMatrix = XMMatrixMultiply(tmpMatrix,m_viewMatrix);
	}
}

void CCEGLView::D3DDepthFunc(int func)
{
	ID3D11DepthStencilState *dss0 = 0;
	ID3D11DepthStencilState *dss1 = 0;
	D3D11_DEPTH_STENCIL_DESC dsd;
	UINT sref;
	//m_d3dContext->ClearDepthStencilView(m_d3dContext->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	bool en = TRUE;
	int wm = D3D11_DEPTH_WRITE_MASK_ALL;

	switch(func)
	{
	case CC_NEVER:		func = D3D11_COMPARISON_NEVER; break;
	case CC_LESS:		func = D3D11_COMPARISON_LESS; break;
	case CC_EQUAL:		func = D3D11_COMPARISON_EQUAL; break;
	case CC_LEQUAL:		func = D3D11_COMPARISON_LESS_EQUAL; break;
	case CC_GREATER:	func = D3D11_COMPARISON_GREATER; break;
	case CC_NOTEQUAL:	func = D3D11_COMPARISON_NOT_EQUAL; break;
	case CC_GEQUAL:		func = D3D11_COMPARISON_GREATER_EQUAL; break;
	case CC_ALWAYS:		func = D3D11_COMPARISON_ALWAYS; break;
	default:en = FALSE; wm = D3D11_DEPTH_WRITE_MASK_ZERO;break;
	}

	m_d3dContext->OMGetDepthStencilState(&dss0,&sref);

	if(dss0)
		dss0->GetDesc(&dsd);
	else
	{
		ZeroMemory(&dsd,sizeof(D3D11_DEPTH_STENCIL_DESC));
		sref = 0;
	}

	dsd.DepthEnable = en;
	dsd.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(wm);
	dsd.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(func);

	dsd.StencilEnable = en;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if(FAILED(m_d3dDevice->CreateDepthStencilState(&dsd,&dss1)))
		exit(-1);
	
	m_d3dContext->OMSetDepthStencilState(dss1,sref);

	if(dss0)
		dss0->Release();
	dss1->Release();
}

void CCEGLView::D3DBlendFunc(int sfactor, int dfactor)
{
	int sfactor2 = sfactor;
	int dfactor2 = dfactor;
	switch(sfactor)
	{
	case CC_ZERO:					sfactor=D3D11_BLEND_ZERO; sfactor2=D3D11_BLEND_ZERO;break;
	case CC_ONE:					sfactor=D3D11_BLEND_ONE; sfactor2=D3D11_BLEND_ONE;break;
	case CC_DST_COLOR:				sfactor=D3D11_BLEND_DEST_COLOR; sfactor2=D3D11_BLEND_DEST_ALPHA;break;
	case CC_ONE_MINUS_DST_COLOR:	sfactor=D3D11_BLEND_INV_DEST_COLOR; sfactor2=D3D11_BLEND_INV_DEST_ALPHA;break;
	case CC_SRC_ALPHA_SATURATE:		sfactor2=D3D11_BLEND_SRC_ALPHA_SAT; sfactor=D3D11_BLEND_SRC_ALPHA_SAT;break;
	case CC_SRC_ALPHA:				sfactor2=D3D11_BLEND_SRC_ALPHA; sfactor=D3D11_BLEND_SRC_ALPHA;break;
	case CC_ONE_MINUS_SRC_ALPHA:	sfactor2=D3D11_BLEND_INV_SRC_ALPHA; sfactor=D3D11_BLEND_INV_SRC_ALPHA;break;
	case CC_DST_ALPHA:				sfactor2=D3D11_BLEND_DEST_ALPHA; sfactor=D3D11_BLEND_DEST_ALPHA;break;
	case CC_ONE_MINUS_DST_ALPHA:	sfactor2=D3D11_BLEND_INV_DEST_ALPHA; sfactor=D3D11_BLEND_INV_DEST_ALPHA;break;
	}
	switch(dfactor)
	{
	case CC_ZERO:					dfactor=D3D11_BLEND_ZERO; dfactor2=D3D11_BLEND_ZERO;break;
	case CC_ONE:					dfactor=D3D11_BLEND_ONE; dfactor2=D3D11_BLEND_ONE;break;
	case CC_SRC_COLOR:				dfactor=D3D11_BLEND_SRC_COLOR; dfactor2=D3D11_BLEND_SRC_ALPHA;break;
	case CC_ONE_MINUS_SRC_COLOR:	dfactor=D3D11_BLEND_INV_SRC_COLOR; dfactor2=D3D11_BLEND_INV_SRC_ALPHA;break;
	case CC_SRC_ALPHA:				dfactor2=D3D11_BLEND_SRC_ALPHA; dfactor=D3D11_BLEND_SRC_ALPHA;break;
	case CC_ONE_MINUS_SRC_ALPHA:	dfactor2=D3D11_BLEND_INV_SRC_ALPHA; dfactor=D3D11_BLEND_INV_SRC_ALPHA;break;
	case CC_DST_ALPHA:				dfactor2=D3D11_BLEND_DEST_ALPHA; dfactor=D3D11_BLEND_DEST_ALPHA;break;
	case CC_ONE_MINUS_DST_ALPHA:	dfactor2=D3D11_BLEND_INV_DEST_ALPHA; dfactor=D3D11_BLEND_INV_DEST_ALPHA;break;
	}

	ID3D11BlendState* dbs0;
	ID3D11BlendState* dbs1;
	D3D11_BLEND_DESC dbd;
	float blendFactor[4]={0.0f,0.0f,0.0f,0.0f};
	UINT sref;
	m_d3dContext->OMGetBlendState(&dbs0,blendFactor,&sref);

	if(dbs0)
		dbs0->GetDesc(&dbd);
	else
	{
		ZeroMemory(&dbd,sizeof(D3D11_BLEND_DESC));
		sref = 0;
	}

	if ( (dbd.RenderTarget[0].SrcBlend != (D3D11_BLEND)sfactor)|| (dbd.RenderTarget[0].DestBlend != (D3D11_BLEND)dfactor) )
	{
		if ( (sfactor==-1) && (dfactor==-1) )
		{
			dbd.RenderTarget[0].BlendEnable = FALSE;
			sfactor = dfactor = D3D11_BLEND_ONE;
			sfactor2 = dfactor2 = D3D11_BLEND_ONE;
		}
		else
		{
			dbd.RenderTarget[0].BlendEnable = TRUE;
		}
		dbd.AlphaToCoverageEnable = FALSE;
		dbd.IndependentBlendEnable = FALSE;
		dbd.RenderTarget[0].SrcBlend = (D3D11_BLEND)sfactor;
		dbd.RenderTarget[0].DestBlend = (D3D11_BLEND)dfactor;
		dbd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		dbd.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)sfactor2;
		dbd.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)dfactor2;
		dbd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		dbd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		memcpy( &dbd.RenderTarget[1], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[2], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[3], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[4], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[5], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[6], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &dbd.RenderTarget[7], &dbd.RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
	}

	if(FAILED(m_d3dDevice->CreateBlendState(&dbd,&dbs1)))
		exit(-1);
	m_d3dContext->OMSetBlendState(dbs1, blendFactor, 0xffffffff);
	if(dbs0)
		dbs0->Release();
	dbs1->Release();
}

void CCEGLView::clearRender(ID3D11RenderTargetView* renderTargetView)
{
	float color[4]={0.f,0.f,0.f,1.f};
	if ( !renderTargetView )
	{
        m_d3dContext->ClearRenderTargetView(m_renderTargetView, color);
	}
	else
	{
		m_d3dContext->ClearRenderTargetView(renderTargetView, color);
	}
    m_d3dContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CCEGLView::D3DClearColor(float r, float b, float g, float a)
{
	m_color[0] = r;
	m_color[1] = b;
	m_color[2] = g;
	m_color[3] = a;
}

void CCEGLView::GetClearColor(float* color)
{
	color[0] = m_color[0];
	color[1] = m_color[1];
	color[2] = m_color[2];
	color[3] = m_color[3];
}

CCEGLView& CCEGLView::sharedOpenGLView()
{
    CC_ASSERT(s_pMainWindow);
    return *s_pMainWindow;
}

void CCEGLView::OnWindowSizeChanged()
{
    m_renderTargetView = DirectXRender::SharedDXRender()->m_renderTargetView.Get();
    m_depthStencilView = DirectXRender::SharedDXRender()->m_depthStencilView.Get();

    // 重新确定 viewPort
    DirectXRender^ render = DirectXRender::SharedDXRender();
    float winWidth = render->m_window->Bounds.Width;
    float winHeight = render->m_window->Bounds.Height;

    m_fWinScaleX = (float)winWidth / m_initWinWidth;
    m_fWinScaleY = (float)winHeight / m_initWinHeight;

    CCDirector::sharedDirector()->reshapeProjection(getSize());

    int newState = 0;//int(Windows::UI::ViewManagement::ApplicationView::Value);
    CCApplication::sharedApplication().applicationViewStateChanged(newState, m_oldViewState);
    m_oldViewState = newState;
}

void CCEGLView::OnCharacterReceived(unsigned int keyCode)
{
    switch (keyCode)
    {
    case VK_BACK:
        CCIMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
        break;

    case VK_RETURN:
        CCIMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
        break;

//    case VK_TAB:
//
//        break;
//
    case VK_ESCAPE:
        // ESC input
		//CCDirector::sharedDirector()->end();
        //CCKeypadDispatcher::sharedDispatcher()->dispatchKeypadMSG(kTypeBackClicked);
        
        break;

    default:
        if (keyCode < 0x20)
        {
            break;
        }
        else if (keyCode < 128)
        {
            // ascii char
            CCIMEDispatcher::sharedDispatcher()->dispatchInsertText((const char *)&keyCode, 1);
        }
        else
        {
            char szUtf8[8] = {0};
            int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&keyCode, 1, szUtf8, sizeof(szUtf8), NULL, NULL);

            CCIMEDispatcher::sharedDispatcher()->dispatchInsertText(szUtf8, nLen);
        }
        break;
    }
}

void CCEGLView::OnPointerPressed(int id, const CCPoint& point)
{
    // prepare CCTouch
    CCTouch* pTouch = m_pTouches[id];
    if (! pTouch)
    {
        pTouch = new CCTouch();
        m_pTouches[id] = pTouch;
    }

    // prepare CCSet
    CCSet* pSet = m_pSets[id];
    if (! pSet)
    {
        pSet = new CCSet();
        m_pSets[id] = pSet;
    }

    if (! pTouch || ! pSet)
        return;

    pTouch->SetTouchInfo((point.x - m_rcViewPort.left) / m_fScreenScaleFactor / m_fWinScaleX, 
        (point.y - m_rcViewPort.top) / m_fScreenScaleFactor / m_fWinScaleY);
    pSet->addObject(pTouch);

    m_pDelegate->touchesBegan(pSet, NULL);
}

void CCEGLView::OnPointerReleased(int id, const CCPoint& point)
{
    CCTouch* pTouch = m_pTouches[id];
    CCSet* pSet = m_pSets[id];

    if (! pTouch || ! pSet)
        return;

    pTouch->SetTouchInfo((point.x - m_rcViewPort.left) / m_fScreenScaleFactor / m_fWinScaleX, 
        (point.y - m_rcViewPort.top) / m_fScreenScaleFactor / m_fWinScaleY);

    m_pDelegate->touchesEnded(pSet, NULL);
    pSet->removeObject(pTouch);

    CC_SAFE_DELETE(m_pTouches[id]);
    CC_SAFE_DELETE(m_pSets[id]);
}

void CCEGLView::OnPointerMoved(int id, const CCPoint& point)
{
    CCTouch* pTouch = m_pTouches[id];
    CCSet* pSet = m_pSets[id];
    
    if (! pTouch || ! pSet)
        return;

    pTouch->SetTouchInfo((point.x - m_rcViewPort.left) / m_fScreenScaleFactor / m_fWinScaleX, 
        (point.y - m_rcViewPort.top) / m_fScreenScaleFactor / m_fWinScaleY);
    m_pDelegate->touchesMoved(pSet, NULL);
}

NS_CC_END;
