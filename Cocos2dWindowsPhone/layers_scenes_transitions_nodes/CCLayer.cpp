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
#include "pch.h"
#include <stdarg.h>
#include "CCLayer.h"
#include "CCTouchDispatcher.h"
#include "CCKeypadDispatcher.h"
#include "CCAccelerometer.h"
#include "CCDirector.h"
#include "CCPointExtension.h"
#include "CCScriptSupport.h"
#include "CCFileUtils.h"
#include "DirectXHelper.h"
#include <fstream>
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

NS_CC_BEGIN

// CCLayer
CCLayer::CCLayer()
:m_bTouchEnabled(false)
,m_bAccelerometerEnabled(false)
,m_bKeypadEnabled(false)
,m_pScriptHandlerEntry(NULL)
{
	setAnchorPoint(ccp(0.5f, 0.5f));
	m_bIsRelativeAnchorPoint = false;
}

CCLayer::~CCLayer()
{
    unregisterScriptTouchHandler();
}

bool CCLayer::init()
{
	bool bRet = false;
	do 
	{
		CCDirector * pDirector;
		CC_BREAK_IF(!(pDirector = CCDirector::sharedDirector()));
		this->setContentSize(pDirector->getWinSize());
        m_bTouchEnabled = false;
        m_bAccelerometerEnabled = false;
		// success
		bRet = true;
	} while(0);
	return bRet;
}

CCLayer *CCLayer::create()
{
	CCLayer *pRet = new CCLayer();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
    else
    {
	    CC_SAFE_DELETE(pRet)
	    return NULL;
    }
}

/// Touch and Accelerometer related

void CCLayer::registerWithTouchDispatcher()
{
	CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();

    if (m_pScriptHandlerEntry)
    {
        if (m_pScriptHandlerEntry->isMultiTouches())
        {
            pDispatcher->addStandardDelegate(this,0);
            LUALOG("[LUA] Add multi-touches event handler: %d", m_pScriptHandlerEntry->getHandler());
        }
        else
        {
            pDispatcher->addTargetedDelegate(this,
                                                                       m_pScriptHandlerEntry->getPriority(),
                                                                       m_pScriptHandlerEntry->getSwallowsTouches());
            LUALOG("[LUA] Add touch event handler: %d", m_pScriptHandlerEntry->getHandler());
        }
        return;
    }
	pDispatcher->addStandardDelegate(this,0);
}

void CCLayer::registerScriptTouchHandler(int nHandler, bool bIsMultiTouches, int nPriority, bool bSwallowsTouches)
{
    unregisterScriptTouchHandler();
    m_pScriptHandlerEntry = CCTouchScriptHandlerEntry::create(nHandler, bIsMultiTouches, nPriority, bSwallowsTouches);
    m_pScriptHandlerEntry->retain();
}

void CCLayer::unregisterScriptTouchHandler(void)
{
    if (m_pScriptHandlerEntry)
    {
        m_pScriptHandlerEntry->release();
        m_pScriptHandlerEntry = NULL;
    }
}

int CCLayer::excuteScriptTouchHandler(int nEventType, CCTouch *pTouch)
{
    return CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerTouchEvent(this, nEventType, pTouch);
}

int CCLayer::excuteScriptTouchHandler(int nEventType, CCSet *pTouches)
{
    return CCScriptEngineManager::sharedManager()->getScriptEngine()->executeLayerTouchesEvent(this, nEventType, pTouches);
}


/// isTouchEnabled getter
bool CCLayer::isTouchEnabled()
{
    return m_bTouchEnabled;
}
/// isTouchEnabled setter
void CCLayer::setTouchEnabled(bool enabled)
{
    if (m_bTouchEnabled != enabled)
    {
        m_bTouchEnabled = enabled;
        if (m_bRunning)
        {
            if (enabled)
            {
                this->registerWithTouchDispatcher();
            }
            else
            {
                // have problems?
              CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
            }
        }
    }
}

void CCLayer::setTouchMode(ccTouchesMode mode)
{
    if(m_eTouchMode != mode)
    {
        m_eTouchMode = mode;
        
		if( m_bTouchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

void CCLayer::setTouchPriority(int priority)
{
    if (m_nTouchPriority != priority)
    {
        m_nTouchPriority = priority;
        
		if( m_bTouchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

int CCLayer::getTouchPriority()
{
    return m_nTouchPriority;
}

int CCLayer::getTouchMode()
{
    return m_eTouchMode;
}

/// isAccelerometerEnabled getter
bool CCLayer::isAccelerometerEnabled()
{
    return m_bAccelerometerEnabled;
}
/// isAccelerometerEnabled setter
void CCLayer::setAccelerometerEnabled(bool enabled)
{
    if (enabled != m_bAccelerometerEnabled)
    {
        m_bAccelerometerEnabled = enabled;

        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            if (enabled)
            {
               pDirector->getAccelerometer()->setDelegate(this);
            }
            else
            {
              pDirector->getAccelerometer()->setDelegate(NULL);
            }
        }
    }
}


void CCLayer::setAccelerometerInterval(double interval) {
    if (m_bAccelerometerEnabled)
    {
        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
           pDirector->getAccelerometer()->setAccelerometerInterval(interval);
        }
    }
}

/// Callbacks
void CCLayer::onEnter()
{
	// register 'parent' nodes first
	// since events are propagated in reverse order
	if (m_bTouchEnabled)
	{
		this->registerWithTouchDispatcher();
	}

	// then iterate over all the children
	CCNode::onEnter();

    // add this layer to concern the Accelerometer Sensor
    if (m_bAccelerometerEnabled)
    {
       // CCAccelerometer::sharedAccelerometer()->setDelegate(this);
    }

    // add this layer to concern the kaypad msg
    if (m_bKeypadEnabled)
    {
        //CCKeypadDispatcher::sharedDispatcher()->addDelegate(this);
    }
}

void CCLayer::onExit()
{
	CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
	if( m_bTouchEnabled )
	{
		pDispatcher->removeDelegate(this);
		unregisterScriptTouchHandler();
	}

    // remove this layer from the delegates who concern Accelerometer Sensor
    if (m_bAccelerometerEnabled)
    {
       // CCAccelerometer::sharedAccelerometer()->setDelegate(NULL);
    }

    // remove this layer from the delegates who concern the kaypad msg
    if (m_bKeypadEnabled)
    {
        //CCKeypadDispatcher::sharedDispatcher()->removeDelegate(this);
    }

	CCNode::onExit();
}

void CCLayer::onEnterTransitionDidFinish()
{
    if (m_bAccelerometerEnabled)
    {
        CCAccelerometer::sharedAccelerometer()->setDelegate(this);
    }
    
    CCNode::onEnterTransitionDidFinish();
}

bool CCLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_pScriptHandlerEntry)
    {
        return excuteScriptTouchHandler(CCTOUCHBEGAN, pTouch);
    }
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
	CCAssert(false, "Layer#ccTouchBegan override me");
	return true;
}

void CCLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHMOVED, pTouch);
        return;
    }
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}
    
void CCLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHENDED, pTouch);
        return;
    }
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void CCLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHCANCELLED, pTouch);
        return;
    }
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}    

void CCLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHBEGAN, pTouches);
        return;
    }
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHMOVED, pTouches);
        return;
    }
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHENDED, pTouches);
        return;
    }
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void CCLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    if (m_pScriptHandlerEntry)
    {
        excuteScriptTouchHandler(CCTOUCHCANCELLED, pTouches);
        return;
    }
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

/// ColorLayer
CCDXLayerColor CCLayerColor::mDXLayerColor;

CCLayerColor::CCLayerColor()
: m_cOpacity(0)
, m_tColor( ccc3(0,0,0) )
{
    // default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;

	mDXLayerColor.setIsInit(FALSE);
}
    
CCLayerColor::~CCLayerColor()
{
}

// Opacity and RGB color protocol
/// opacity getter
CCubyte CCLayerColor::getOpacity()
{
	return m_cOpacity;
}
/// opacity setter
void CCLayerColor::setOpacity(CCubyte var)
{
	m_cOpacity = var;
	updateColor();
}

/// color getter
const ccColor3B& CCLayerColor::getColor()
{
	return m_tColor;
}

/// color setter
void CCLayerColor::setColor(const ccColor3B& var)
{
	m_tColor = var;
	updateColor();
}


/// blendFunc getter
ccBlendFunc CCLayerColor::getBlendFunc()
{
	return m_tBlendFunc;
}
/// blendFunc setter
void CCLayerColor::setBlendFunc(ccBlendFunc var)
{
	m_tBlendFunc = var;
}


CCLayerColor * CCLayerColor::create(const ccColor4B& color, CCfloat width, CCfloat height)
{
	CCLayerColor * pLayer = new CCLayerColor();
	if( pLayer && pLayer->initWithColor(color,width,height))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}
CCLayerColor * CCLayerColor::create(const ccColor4B& color)
{
	CCLayerColor * pLayer = new CCLayerColor();
	if(pLayer && pLayer->initWithColor(color))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}

CCLayerColor* CCLayerColor::create()
{
    CCLayerColor* pRet = new CCLayerColor();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool CCLayerColor::initWithColor(const ccColor4B& color, CCfloat width, CCfloat height)
{
	// default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;

	m_tColor.r = color.r;
	m_tColor.g = color.g;
	m_tColor.b = color.b;
	m_cOpacity = color.a;

	for (unsigned int i=0; i<sizeof(m_pSquareVertices) / sizeof(m_pSquareVertices[0]); i++ )
	{
		m_pSquareVertices[i].x = 0.0f;
        m_pSquareVertices[i].y = 0.0f;
	}

	this->updateColor();
	this->setContentSize(CCSizeMake(width,height));
	return true;
}

bool CCLayerColor::initWithColor(const ccColor4B& color)
{
	CCSize s = CCDirector::sharedDirector()->getWinSize();
	this->initWithColor(color, s.width, s.height);
	return true;
}

/// override contentSize
void CCLayerColor::setContentSize(const CCSize& size)
{
	m_pSquareVertices[1].x = size.width * CC_CONTENT_SCALE_FACTOR();
	m_pSquareVertices[2].y = size.height * CC_CONTENT_SCALE_FACTOR();
	m_pSquareVertices[3].x = size.width * CC_CONTENT_SCALE_FACTOR();
	m_pSquareVertices[3].y = size.height * CC_CONTENT_SCALE_FACTOR();

	CCLayer::setContentSize(size);
}

void CCLayerColor::changeWidthAndHeight(CCfloat w ,CCfloat h)
{
	this->setContentSize(CCSizeMake(w, h));
}

void CCLayerColor::changeWidth(CCfloat w)
{
	this->setContentSize(CCSizeMake(w, m_tContentSize.height));
}

void CCLayerColor::changeHeight(CCfloat h)
{
	this->setContentSize(CCSizeMake(m_tContentSize.width, h));
}

void CCLayerColor::updateColor()
{
	for( unsigned int i=0; i < 4; i++ )
	{
		m_pSquareColors[i].r = m_tColor.r;
		m_pSquareColors[i].g = m_tColor.g;
		m_pSquareColors[i].b = m_tColor.b;
		m_pSquareColors[i].a = m_cOpacity;
	}
}

void CCLayerColor::draw()
{
	CCLayer::draw();

	// Default GL states: CC_TEXTURE_2D, CC_VERTEX_ARRAY, CC_COLOR_ARRAY, CC_TEXTURE_COORD_ARRAY
	// Needed states: CC_VERTEX_ARRAY, CC_COLOR_ARRAY
	// Unneeded states: CC_TEXTURE_2D, CC_TEXTURE_COORD_ARRAY
	/*=glDisableClientState(CC_TEXTURE_COORD_ARRAY);
	glDisable(CC_TEXTURE_2D);

	glVertexPointer(2, CC_FLOAT, 0, m_pSquareVertices);
	glColorPointer(4, CC_UNSIGNED_BYTE, 0, m_pSquareColors);
	=*/
	bool newBlend = false;
	if( m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST ) {
		newBlend = true;
		CCD3DCLASS->D3DBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);
	}
	else if( m_cOpacity != 255 ) {
		newBlend = true;
		CCD3DCLASS->D3DBlendFunc(CC_SRC_ALPHA, CC_ONE_MINUS_SRC_ALPHA);
	}
	mDXLayerColor.Render(m_pSquareVertices,m_pSquareColors);
	//=glDrawArrays(CC_TRIANGLE_STRIP, 0, 4);

	if( newBlend )
	{
		CCD3DCLASS->D3DBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	}
	/*=
	// restore default GL state
	glEnableClientState(CC_TEXTURE_COORD_ARRAY);
	glEnable(CC_TEXTURE_2D);=*/
}



CCDXLayerColor::CCDXLayerColor()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_vertexBuffer = 0;
	mIsInit = FALSE;
}
CCDXLayerColor::~CCDXLayerColor()
{
	FreeBuffer();
}
void CCDXLayerColor::FreeBuffer()
{
	CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_matrixBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_layout);
	CC_SAFE_RELEASE_NULL_DX(m_pixelShader);
	CC_SAFE_RELEASE_NULL_DX(m_vertexShader);
}
void CCDXLayerColor::setIsInit(bool isInit)
{
	mIsInit = isInit;
}


void CCDXLayerColor::initVertexBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Now create the vertex buffer.
	CCID3D11Device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);
}

void CCDXLayerColor::RenderVertexBuffer(ccVertex2F* squareVertices,ccColor4B* squareColors)
{
	// Create the vertex array.
	VertexType verticesTmp[4];
	
	verticesTmp[0].position = DirectX::XMFLOAT3(squareVertices[0].x, squareVertices[0].y, 0);
	verticesTmp[1].position = DirectX::XMFLOAT3(squareVertices[1].x, squareVertices[1].y, 0);
	verticesTmp[2].position = DirectX::XMFLOAT3(squareVertices[2].x, squareVertices[2].y, 0);
	verticesTmp[3].position = DirectX::XMFLOAT3(squareVertices[3].x, squareVertices[3].y, 0);

	verticesTmp[0].color = DirectX::XMFLOAT4(squareColors[0].r/255.0f, squareColors[0].g/255.0f, squareColors[0].b/255.0f, squareColors[0].a/255.0f);
	verticesTmp[1].color = DirectX::XMFLOAT4(squareColors[1].r/255.0f, squareColors[1].g/255.0f, squareColors[1].b/255.0f, squareColors[1].a/255.0f);
	verticesTmp[2].color = DirectX::XMFLOAT4(squareColors[2].r/255.0f, squareColors[2].g/255.0f, squareColors[2].b/255.0f, squareColors[2].a/255.0f);
	verticesTmp[3].color = DirectX::XMFLOAT4(squareColors[3].r/255.0f, squareColors[3].g/255.0f, squareColors[3].b/255.0f, squareColors[3].a/255.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	if(FAILED(CCID3D11DeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return ;
	}
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)verticesTmp, (sizeof(VertexType) * 4));
	CCID3D11DeviceContext->Unmap(m_vertexBuffer, 0);


	////////////////////////
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexType); 
	offset = 0;
	CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return;
}

bool CCDXLayerColor::InitializeShader()
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
		 };

		 loader->LoadShader(
			 L"CCLayerColorVertexShader.cso",
			 layoutDesc,
			 ARRAYSIZE(layoutDesc),
			 &m_vertexShader,
			 &m_layout
			 );

		 loader->LoadShader(
			 L"CCLayerColorPixelShader.cso",
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

void CCDXLayerColor::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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


bool CCDXLayerColor::SetShaderParameters(XMMATRIX &viewMatrix,XMMATRIX &projectionMatrix)
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

	return true;
}

void CCDXLayerColor::RenderShader()
{
	CCID3D11DeviceContext->IASetInputLayout(m_layout);
	CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	CCID3D11DeviceContext->Draw(4,0);
}


void CCDXLayerColor::Render(ccVertex2F* squareVertices,ccColor4B* squareColors)
{
	if ( !mIsInit )
	{
		mIsInit = TRUE;
		FreeBuffer();
		initVertexBuffer();
		InitializeShader();
	}
	
	XMMATRIX viewMatrix, projectionMatrix;
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);
	RenderVertexBuffer(squareVertices,squareColors);
	SetShaderParameters(viewMatrix, projectionMatrix);
	RenderShader();
}

//
// CCLayerGradient
// 

CCLayerGradient* CCLayerGradient::create(const ccColor4B& start, const ccColor4B& end)
{
    CCLayerGradient * pLayer = new CCLayerGradient();
    if( pLayer && pLayer->initWithColor(start, end))
    {
        pLayer->autorelease();
        return pLayer;
    }
    CC_SAFE_DELETE(pLayer);
    return NULL;
}

CCLayerGradient* CCLayerGradient::create(const ccColor4B& start, const ccColor4B& end, const CCPoint& v)
{
    CCLayerGradient * pLayer = new CCLayerGradient();
    if( pLayer && pLayer->initWithColor(start, end, v))
    {
        pLayer->autorelease();
        return pLayer;
    }
    CC_SAFE_DELETE(pLayer);
    return NULL;
}

CCLayerGradient* CCLayerGradient::node()
{
    return CCLayerGradient::create();
}

CCLayerGradient* CCLayerGradient::create()
{
    CCLayerGradient* pRet = new CCLayerGradient();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool CCLayerGradient::init()
{
	return initWithColor(ccc4(0, 0, 0, 255), ccc4(0, 0, 0, 255));
}

bool CCLayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end)
{
    return initWithColor(start, end, ccp(0, -1));
}

bool CCLayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end, const CCPoint& v)
{
    m_endColor.r  = end.r;
    m_endColor.g  = end.g;
    m_endColor.b  = end.b;

    m_cEndOpacity   = end.a;
    m_cStartOpacity    = start.a;
    m_AlongVector   = v;

    m_bCompressedInterpolation = true;

    return CCLayerColor::initWithColor(ccc4(start.r, start.g, start.b, 255));
}

void CCLayerGradient::updateColor()
{
    CCLayerColor::updateColor();

    float h = ccpLength(m_AlongVector);
    if (h == 0)
        return;

    float c = sqrtf(2.0f);
    CCPoint u = ccp(m_AlongVector.x / h, m_AlongVector.y / h);

    // Compressed Interpolation mode
    if (m_bCompressedInterpolation)
    {
        float h2 = 1 / ( fabsf(u.x) + fabsf(u.y) );
        u = ccpMult(u, h2 * (float)c);
    }

    float opacityf = (float)m_cOpacity / 255.0f;

    ccColor4F S = {
        m_tColor.r / 255.0f,
        m_tColor.g / 255.0f,
        m_tColor.b / 255.0f,
        m_cStartOpacity * opacityf / 255.0f
    };

    ccColor4F E = {
        m_endColor.r / 255.0f,
        m_endColor.g / 255.0f,
        m_endColor.b / 255.0f,
        m_cEndOpacity * opacityf / 255.0f
    };

    // (-1, -1)
    m_pSquareColors[0].r = E.r + (S.r - E.r) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].g = E.g + (S.g - E.g) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].b = E.b + (S.b - E.b) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].a = E.a + (S.a - E.a) * ((c + u.x + u.y) / (2.0f * c));
    // (1, -1)
    m_pSquareColors[1].r = E.r + (S.r - E.r) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].g = E.g + (S.g - E.g) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].b = E.b + (S.b - E.b) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].a = E.a + (S.a - E.a) * ((c - u.x + u.y) / (2.0f * c));
    // (-1, 1)
    m_pSquareColors[2].r = E.r + (S.r - E.r) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].g = E.g + (S.g - E.g) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].b = E.b + (S.b - E.b) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].a = E.a + (S.a - E.a) * ((c + u.x - u.y) / (2.0f * c));
    // (1, 1)
    m_pSquareColors[3].r = E.r + (S.r - E.r) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].g = E.g + (S.g - E.g) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].b = E.b + (S.b - E.b) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].a = E.a + (S.a - E.a) * ((c - u.x - u.y) / (2.0f * c));
}

const ccColor3B& CCLayerGradient::getStartColor()
{
    return m_tColor;
}

void CCLayerGradient::setStartColor(const ccColor3B& color)
{
    setColor(color);
}

void CCLayerGradient::setEndColor(const ccColor3B& color)
{
    m_endColor = color;
    updateColor();
}

const ccColor3B& CCLayerGradient::getEndColor()
{
    return m_endColor;
}

void CCLayerGradient::setStartOpacity(CCubyte o)
{
    m_cStartOpacity = o;
    updateColor();
}

CCubyte CCLayerGradient::getStartOpacity()
{
    return m_cStartOpacity;
}

void CCLayerGradient::setEndOpacity(CCubyte o)
{
    m_cEndOpacity = o;
    updateColor();
}

CCubyte CCLayerGradient::getEndOpacity()
{
    return m_cEndOpacity;
}

void CCLayerGradient::setVector(const CCPoint& var)
{
    m_AlongVector = var;
    updateColor();
}

const CCPoint& CCLayerGradient::getVector()
{
    return m_AlongVector;
}

bool CCLayerGradient::isCompressedInterpolation()
{
    return m_bCompressedInterpolation;
}

void CCLayerGradient::setCompressedInterpolation(bool compress)
{
    m_bCompressedInterpolation = compress;
    updateColor();
}

/// MultiplexLayer

CCLayerMultiplex::CCLayerMultiplex()
: m_nEnabledLayer(0)
, m_pLayers(NULL)
{
}
CCLayerMultiplex::~CCLayerMultiplex()
{
	CC_SAFE_RELEASE(m_pLayers);
}

CCLayerMultiplex * CCLayerMultiplex::layerWithLayers(CCLayer * layer, ...)
{
	va_list args;
	va_start(args,layer);

	CCLayerMultiplex * pMultiplexLayer = new CCLayerMultiplex();
	if(pMultiplexLayer && pMultiplexLayer->initWithLayers(layer, args))
	{
		pMultiplexLayer->autorelease();
		va_end(args);
		return pMultiplexLayer;
	}
	va_end(args);
	CC_SAFE_DELETE(pMultiplexLayer);
	return NULL;
}

CCLayerMultiplex * CCLayerMultiplex::layerWithLayer(CCLayer* layer)
{
	CCLayerMultiplex * pMultiplexLayer = new CCLayerMultiplex();
	pMultiplexLayer->initWithLayer(layer);
	pMultiplexLayer->autorelease();
	return pMultiplexLayer;
}
void CCLayerMultiplex::addLayer(CCLayer* layer)
{
	CCAssert(m_pLayers, "");
	m_pLayers->addObject(layer);
}

bool CCLayerMultiplex::initWithLayer(CCLayer* layer)
{
	m_pLayers = new CCArray;
	m_pLayers->addObject(layer);
	m_nEnabledLayer = 0;
	this->addChild(layer);
	return true;
}

bool CCLayerMultiplex::initWithLayers(CCLayer *layer, va_list params)
{
	m_pLayers = new CCArray;
	//m_pLayers->retain();

	m_pLayers->addObject(layer);

	CCLayer *l = va_arg(params,CCLayer*);
	while( l ) {
		m_pLayers->addObject(l);
		l = va_arg(params,CCLayer*);
	}

	m_nEnabledLayer = 0;
	this->addChild((CCLayerMultiplex*)m_pLayers->objectAtIndex(m_nEnabledLayer));

	return true;
}


void CCLayerMultiplex::switchTo(unsigned int n)
{
	CCAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

	this->removeChild((CCLayerMultiplex*)m_pLayers->objectAtIndex(m_nEnabledLayer), true);

	m_nEnabledLayer = n;

	this->addChild((CCLayerMultiplex*)m_pLayers->objectAtIndex(n));
}

void CCLayerMultiplex::switchToAndReleaseMe(unsigned int n)
{
	CCAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

	this->removeChild((CCLayerMultiplex*)m_pLayers->objectAtIndex(m_nEnabledLayer), true);

	//[layers replaceObjectAtIndex:enabledLayer withObject:[NSNull null]];
	m_pLayers->replaceObjectAtIndex(m_nEnabledLayer, NULL);

	m_nEnabledLayer = n;

	this->addChild((CCLayerMultiplex*)m_pLayers->objectAtIndex(n));
}

/// isKeypadEnabled getter
bool CCLayer::isKeypadEnabled()
{
    return m_bKeypadEnabled;
}
/// isKeypadEnabled setter
void CCLayer::setKeypadEnabled(bool enabled)
{
    if (enabled != m_bKeypadEnabled)
    {
        m_bKeypadEnabled = enabled;

        if (m_bRunning)
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            if (enabled)
            {
                pDirector->getKeypadDispatcher()->addDelegate(this);
            }
            else
            {
                pDirector->getKeypadDispatcher()->removeDelegate(this);
            }
        }
    }
}



NS_CC_END
