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

#ifndef __CCSAXPARSER_H__
#define __CCSAXPARSER_H__

#include "CCPlatformConfig.h"
#include "CCCommon.h"

NS_CC_BEGIN;


typedef unsigned char CC_XML_CHAR;

class CC_DLL CCSAXDelegator
{
public:
	virtual void startElement(void *ctx, const char *name, const char **atts) = 0;
	virtual void endElement(void *ctx, const char *name) = 0;
	virtual void textHandler(void *ctx, const char *s, int len) = 0;
};

class CC_DLL CCSAXParser
{
	CCSAXDelegator*	m_pDelegator;
public:

	CCSAXParser();
	~CCSAXParser(void);

	bool init(const char *pszEncoding);
	bool parse(const char *pszFile);
	void setDelegator(CCSAXDelegator* pDelegator);

	static void startElement(void *ctx, const CC_XML_CHAR *name, const CC_XML_CHAR **atts);
	static void endElement(void *ctx, const CC_XML_CHAR *name);
	static void textHandler(void *ctx, const CC_XML_CHAR *name, int len);
};
NS_CC_END;

#endif //__CCSAXPARSER_H__
