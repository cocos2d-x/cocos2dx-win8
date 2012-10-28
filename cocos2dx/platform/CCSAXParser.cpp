/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010 cocos2d-x.org  http://cocos2d-x.org
* Copyright (c) 2010 Максим Аксенов
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

#include "CCSAXParser.h"
#include "CCMutableDictionary.h"
#include "CCLibxml2.h"
#include "CCFileUtils.h"

NS_CC_BEGIN;

CCSAXParser::CCSAXParser()
{
	m_pDelegator = NULL;

}

CCSAXParser::~CCSAXParser(void)
{
}

bool CCSAXParser::init(const char *pszEncoding)
{
    CC_UNUSED_PARAM(pszEncoding);
	// nothing to do
	return true;
}

bool CCSAXParser::parse(const char *pszFile)
{
//	CCFileData data(pszFile, "rt");
//	unsigned long size = data.getSize();
//	char *pBuffer = (char*) data.getBuffer();
//	
//	if (!pBuffer)
//	{
//		return false;
//	}
//		
//
//	LIBXML_TEST_VERSION
//	xmlSAXHandler saxHandler;
//	memset( &saxHandler, 0, sizeof(saxHandler) );
//	// Using xmlSAXVersion( &saxHandler, 2 ) generate crash as it sets plenty of other pointers...
//	saxHandler.initialized = XML_SAX2_MAGIC;  // so we do this to force parsing as SAX2.
//	saxHandler.startElement = &CCSAXParser::startElement;
//	saxHandler.endElement = &CCSAXParser::endElement;
//	saxHandler.characters = &CCSAXParser::textHandler;
//	
//	int result = xmlSAXUserParseMemory( &saxHandler, this, pBuffer, size );
//	if ( result != 0 )
//	{
//		return false;
//	}
//
//	xmlCleanupParser();
//
//#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
//	xmlMemoryDump();
//#endif
	
	return true;
}

void CCSAXParser::startElement(void *ctx, const CC_XML_CHAR *name, const CC_XML_CHAR **atts)
{
	((CCSAXParser*)(ctx))->m_pDelegator->startElement(ctx, (char*)name, (const char**)atts);
}

void CCSAXParser::endElement(void *ctx, const CC_XML_CHAR *name)
{
	((CCSAXParser*)(ctx))->m_pDelegator->endElement(ctx, (char*)name);
}
void CCSAXParser::textHandler(void *ctx, const CC_XML_CHAR *name, int len)
{
	((CCSAXParser*)(ctx))->m_pDelegator->textHandler(ctx, (char*)name, len);
}
void CCSAXParser::setDelegator(CCSAXDelegator* pDelegator)
{
	m_pDelegator = pDelegator;
}

NS_CC_END;


