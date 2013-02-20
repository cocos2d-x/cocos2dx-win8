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

#include "CCData.h"
#include "CCFileUtils.h"

#include <stdio.h>

using namespace std;
NS_CC_BEGIN

CCData::CCData(void)
: m_pData(NULL)
{
}

CCData::~CCData(void)
{
	CC_SAFE_DELETE_ARRAY(m_pData);
}

CCData* CCData::dataWithContentsOfFile(const string &strPath)
{
    CCFileData data(strPath.c_str(), "rb");
    unsigned long  nSize = data.getSize();
    unsigned char* pBuffer = data.getBuffer();

    if (! pBuffer)
    {
        return NULL;
    }

	CCData *pRet = new CCData();
    pRet->m_pData = new char[nSize];
    memcpy(pRet->m_pData, pBuffer, nSize);

	return pRet;
}

void* CCData::bytes(void)
{
	return m_pData;
}

//@todo implement
CCData* CCData::dataWithBytes(unsigned char *pBytes, int size)
{
    CC_UNUSED_PARAM(pBytes);
    CC_UNUSED_PARAM(size);
	return NULL;
}
NS_CC_END
