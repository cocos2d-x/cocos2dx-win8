/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
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
#include "CCLabelTTF.h"
#include "CCDirector.h"

NS_CC_BEGIN
	//
	//CCLabelTTF
	//
	CCLabelTTF::CCLabelTTF()
	: m_eAlignment(kCCTextAlignmentCenter)
	, m_pFontName(NULL)
	, m_fFontSize(0.0)
	, m_pString(NULL)
{
}

CCLabelTTF::~CCLabelTTF()
{
	CC_SAFE_DELETE(m_pFontName);
	CC_SAFE_DELETE(m_pString);        
}

CCLabelTTF * CCLabelTTF::create()
{
	CCLabelTTF * pRet = new CCLabelTTF();
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

CCLabelTTF * CCLabelTTF::create(const char *string, const char *fontName, float fontSize)
{
	return CCLabelTTF::create(string, fontName, fontSize,
		CCSizeZero, kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
}

CCLabelTTF * CCLabelTTF::create(const char *string, const char *fontName, float fontSize,
								const CCSize& dimensions, CCTextAlignment hAlignment)
{
	return CCLabelTTF::create(string, fontName, fontSize, dimensions, hAlignment, kCCVerticalTextAlignmentTop);
}

CCLabelTTF* CCLabelTTF::create(const char *string, const char *fontName, float fontSize,
							   const CCSize &dimensions, CCTextAlignment hAlignment, 
							   CCVerticalTextAlignment vAlignment)
{
	CCLabelTTF *pRet = new CCLabelTTF();
	if(pRet && pRet->initWithString(string, fontName, fontSize, dimensions, hAlignment, vAlignment))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

//bool CCLabelTTF::initWithString(const char *label, const char *fontName, float fontSize)
//{
//	CCAssert(label != NULL, "");
//	if (CCSprite::init())
//	{
//		m_tDimensions = CCSizeZero;

//           if (m_pFontName)
//           {
//               delete m_pFontName;
//               m_pFontName = NULL;
//           }
//           m_pFontName = new std::string(fontName);

//		m_fFontSize = fontSize * CC_CONTENT_SCALE_FACTOR();
//		this->setString(label);
//		return true;
//	}
//	return false;
//}
bool CCLabelTTF::init()
{
    return this->initWithString("", "Helvetica", 12);
}
bool CCLabelTTF::initWithString(const char *label, const char *fontName, float fontSize, 
								const CCSize& dimensions, CCTextAlignment alignment)
{
	return this->initWithString(label, fontName, fontSize, dimensions, alignment, kCCVerticalTextAlignmentTop);
}

bool CCLabelTTF::initWithString(const char *label, const char *fontName, float fontSize)
{
	return this->initWithString(label, fontName, fontSize, 
		CCSizeZero, kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);
}

bool CCLabelTTF::initWithString(const char *string, const char *fontName, float fontSize,
								const cocos2d::CCSize &dimensions, CCTextAlignment hAlignment,
								CCVerticalTextAlignment vAlignment)
{
	CCAssert(string != NULL, "Lable can't be NULL!");
	if (CCSprite::init())
	{
		m_tDimensions = CCSizeMake( dimensions.width * CC_CONTENT_SCALE_FACTOR(), dimensions.height * CC_CONTENT_SCALE_FACTOR() );
		m_eAlignment = hAlignment;
		m_vAlignment = vAlignment;
		if (m_pFontName)
		{
			delete m_pFontName;
			m_pFontName = NULL;
		}
		m_pFontName = new std::string(fontName);

		m_fFontSize = fontSize * CC_CONTENT_SCALE_FACTOR();
		this->setString(string);
		return true;
	}
	return false;
}

void CCLabelTTF::setString(const char *string)
{
    CCAssert(string != NULL, "Invalid string");
    
    if (m_string.compare(string))
    {
        m_string = string;
        
        this->updateTexture();
    }
}

const char* CCLabelTTF::getString(void)
{
	return m_pString->c_str();
}

char * CCLabelTTF::description()
{
	char *ret = new char[100] ;
	sprintf(ret, "<CCLabelTTF | FontName = %s, FontSize = %.1f>", m_pFontName->c_str(), m_fFontSize);
	return ret;
}

float CCLabelTTF::getFontSize()
{
    return m_fFontSize;
}

void CCLabelTTF::setFontSize(float fontSize)
{
    if (m_fFontSize != fontSize)
    {
        m_fFontSize = fontSize;
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

const char* CCLabelTTF::getFontName()
{
    return m_pFontName->c_str();
}

void CCLabelTTF::setFontName(const char *fontName)
{
    if (m_pFontName->compare(fontName))
    {
        delete m_pFontName;
        m_pFontName = new std::string(fontName);
        
        // Force update
        if (m_string.size() > 0)
        {
            this->updateTexture();
        }
    }
}

// Helper
bool CCLabelTTF::updateTexture()
{
    bool bRet = false;
    CCTexture2D *tex;
    
    do
    {
        // let system compute label's width or height when its value is 0
        // refer to cocos2d-x issue #1430
        tex = new CCTexture2D();
        CC_BREAK_IF(!tex);

        bRet = tex->initWithString( m_string.c_str(),
                            m_pFontName->c_str(),
                            m_fFontSize * CC_CONTENT_SCALE_FACTOR(),
                            CC_SIZE_POINTS_TO_PIXELS(m_tDimensions), 
                            m_hAlignment,
                            m_vAlignment);
        CC_BREAK_IF(!bRet);
	
        this->setTexture(tex);
        tex->release();
	
	    CCRect rect = CCRectZero;
        rect.size = m_pobTexture->getContentSize();
        this->setTextureRect(rect);
    }while(false);

    return bRet;
}
NS_CC_END