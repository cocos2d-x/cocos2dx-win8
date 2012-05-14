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
#include "CCGrabber.h"
#include "ccMacros.h"
#include "CCTexture2D.h"
#include "platform/platform.h"
#include "CCDirector.h"

namespace cocos2d
{
	CCGrabber::CCGrabber(void)
		: m_fbo(0)
		, m_oldFBO(0)
	{

	}

	void CCGrabber::grab(CCTexture2D *pTexture)
	{
		Initialize(CCDirector::sharedDirector()->getOpenGLView()->GetDevice(), pTexture);
	}
	
	void CCGrabber::beforeRender(CCTexture2D *pTexture)
	{
		CCID3D11DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		CCD3DCLASS->D3DClearColor(0.0f,0.0f,0.0f,1.0f);
		CCD3DCLASS->clearRender(m_renderTargetView);
		CCID3D11DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void CCGrabber::afterRender(cocos2d::CCTexture2D *pTexture)
	{
		CCEGLView* eglView = CCDirector::sharedDirector()->getOpenGLView();
		eglView->SetBackBufferRenderTarget();
	}

	CCGrabber::~CCGrabber()
	{
		if(m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = 0;
		}
		if (m_depthStencil) {
			m_depthStencil->Release();
			m_depthStencil = 0;
		}
		if (m_depthStencilView) {
			m_depthStencilView->Release();
			m_renderTargetView = 0;
		}
		CCLOGINFO("cocos2d: deallocing %p", this);
	}

	bool CCGrabber::Initialize(ID3D11Device* device, CCTexture2D *pTexture)
	{
		HRESULT result;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		ID3D11ShaderResourceView* tmpShaderResourceView=pTexture->getTextureResource();
		ID3D11Resource* tmpResource;
		tmpShaderResourceView->GetResource(&tmpResource);

		D3D11_TEXTURE2D_DESC pDesc;
		((ID3D11Texture2D*)tmpResource)->GetDesc(&pDesc);
		// Setup the description of the render target view.
		renderTargetViewDesc.Format = pDesc.Format;//textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		result = device->CreateRenderTargetView(tmpResource, &renderTargetViewDesc, &m_renderTargetView);

		if(FAILED(result))
		{
			return false;
		}
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			pDesc.Width,
			pDesc.Height,
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL
        );
		result = device->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            &m_depthStencil
            );
		if (FAILED(result)) {
			return false;
        }
		result = device->CreateDepthStencilView(m_depthStencil, 
				&CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D), 
				&m_depthStencilView );
		if(FAILED(result)) {
			return false;
		}
		return true;
	}
} // end of namespace cocos2d
