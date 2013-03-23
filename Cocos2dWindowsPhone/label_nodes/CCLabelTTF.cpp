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

bool CCLabelTTF::initWithString(const char *label, const CCSize& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize)
{
	CCAssert(label != NULL, "");
	if (CCSprite::init())
	{
		m_tDimensions = CCSizeMake( dimensions.width * CC_CONTENT_SCALE_FACTOR(), dimensions.height * CC_CONTENT_SCALE_FACTOR() );
		m_eAlignment = alignment;

		if (m_pFontName)
		{
			delete m_pFontName;
			m_pFontName = NULL;
		}
		m_pFontName = new std::string(fontName);

		m_fFontSize = fontSize * CC_CONTENT_SCALE_FACTOR();
		this->setString(label);
		return true;
	}
	return false;
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

void CCLabelTTF::setString(const char *label)
{
	if (m_pString)
	{
		delete m_pString;
		m_pString = NULL;
	}
	m_pString = new std::string(label);

	CCTexture2D *texture;
	if( m_tDimensions.equals(CCSizeZero ) )
	{
		texture = new CCTexture2D();
		texture->initWithString(label, m_pFontName->c_str(), m_fFontSize);
	}
	else
	{
		texture = new CCTexture2D();
		texture->initWithString(label, m_tDimensions, m_eAlignment, m_pFontName->c_str(), m_fFontSize);
	}
	this->setTexture(texture);
	texture->release();

	CCRect rect = CCRectZero;
	rect.size = m_pobTexture->getContentSize();
	this->setTextureRect(rect);
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
NS_CC_END