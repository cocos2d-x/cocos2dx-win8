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

#include "CCFileUtils.h"
#include "CCDirector.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) && (CC_TARGET_PLATFORM != CC_PLATFORM_AIRPLAY)

#include <stack>

#include "CCLibxml2.h"
#include "CCString.h"
#include "CCSAXParser.h"
//#include "support/zip_support/unzip.h"

NS_CC_BEGIN;

static const char *__suffixiPhoneRetinaDisplay = "-hd";
static const char *__suffixiPad = "-ipad";
static const char *__suffixiPadRetinaDisplay = "-ipadhd";

typedef enum 
{
    SAX_NONE = 0,
    SAX_KEY,
    SAX_DICT,
    SAX_INT,
    SAX_REAL,
    SAX_STRING,
    SAX_ARRAY
}CCSAXState;

typedef enum
{
    SAX_RESULT_NONE = 0,
    SAX_RESULT_DICT,
    SAX_RESULT_ARRAY
}CCSAXResult;

class CCDictMaker : public CCSAXDelegator
{
public:
    CCSAXResult m_eResultType;
    CCMutableArray<CCObject*>* m_pRootArray;
    CCDictionary<std::string, CCObject*> *m_pRootDict;
    CCDictionary<std::string, CCObject*> *m_pCurDict;
    std::stack<CCDictionary<std::string, CCObject*>*> m_tDictStack;
    std::string m_sCurKey;///< parsed key
    CCSAXState m_tState;
    CCMutableArray<CCObject*> *m_pArray;

    std::stack<CCMutableArray<CCObject*>*> m_tArrayStack;
    std::stack<CCSAXState>  m_tStateStack;

public:
    CCDictMaker()
        : m_eResultType(SAX_RESULT_NONE),
          m_pRootArray(NULL),
          m_pRootDict(NULL),
          m_pCurDict(NULL),
          m_tState(SAX_NONE),
          m_pArray(NULL)
    {
    }

    ~CCDictMaker()
    {
    }

    CCDictionary<std::string, CCObject*> *dictionaryWithContentsOfFile(const char *pFileName)
    {
        m_eResultType = SAX_RESULT_DICT;
        CCSAXParser parser;

        if (false == parser.init("UTF-8"))
        {
            return NULL;
        }
        parser.setDelegator(this);

        parser.parse(pFileName);
        return m_pRootDict;
    }

    CCMutableArray<CCObject*>* arrayWithContentsOfFile(const char* pFileName)
    {
        m_eResultType = SAX_RESULT_ARRAY;
        CCSAXParser parser;

        if (false == parser.init("UTF-8"))
        {
            return NULL;
        }
        parser.setDelegator(this);

        parser.parse(pFileName);
        return m_pArray;
    }

    void startElement(void *ctx, const char *name, const char **atts)
    {
        CC_UNUSED_PARAM(ctx);
        CC_UNUSED_PARAM(atts);
        std::string sName((char*)name);
        if( sName == "dict" )
        {
            m_pCurDict = new CCDictionary<std::string, CCObject*>();
            if (m_eResultType == SAX_RESULT_DICT && ! m_pRootDict)
            {
				// Because it will call m_pCurDict->release() later, so retain here.
                m_pRootDict = m_pCurDict;
				m_pRootDict->retain();
            }
            m_tState = SAX_DICT;

            CCSAXState preState = SAX_NONE;
            if (! m_tStateStack.empty())
            {
                preState = m_tStateStack.top();
            }

            if (SAX_ARRAY == preState)
            {
                // add the dictionary into the array
                m_pArray->addObject(m_pCurDict);
            }
            else if (SAX_DICT == preState)
            {
                // add the dictionary into the pre dictionary
                CCAssert(! m_tDictStack.empty(), "The state is wrong!");
                CCDictionary<std::string, CCObject*>* pPreDict = m_tDictStack.top();
                pPreDict->setObject(m_pCurDict, m_sCurKey);
            }

			m_pCurDict->release();

            // record the dict state
            m_tStateStack.push(m_tState);
            m_tDictStack.push(m_pCurDict);
        }
        else if(sName == "key")
        {
            m_tState = SAX_KEY;
        }
        else if(sName == "integer")
        {
            m_tState = SAX_INT;
        }
        else if(sName == "real")
        {
            m_tState = SAX_REAL;
        }
        else if(sName == "string")
        {
            m_tState = SAX_STRING;
        }
        else if (sName == "array")
        {
            m_tState = SAX_ARRAY;
            m_pArray = new CCMutableArray<CCObject*>();
            if (m_eResultType == SAX_RESULT_ARRAY && m_pRootArray == NULL)
            {
                m_pRootArray = m_pArray;
                m_pRootArray->retain();
            }
            CCSAXState preState = SAX_NONE;
            if (! m_tStateStack.empty())
            {
                preState = m_tStateStack.top();
            }

            //CCSAXState preState = m_tStateStack.empty() ? SAX_DICT : m_tStateStack.top();
            if (preState == SAX_DICT)
            {
                m_pCurDict->setObject(m_pArray, m_sCurKey);
            }
            else if (preState == SAX_ARRAY)
            {
                CCAssert(! m_tArrayStack.empty(), "The state is worng!");
                CCMutableArray<CCObject*>* pPreArray = m_tArrayStack.top();
                pPreArray->addObject(m_pArray);
            }
            m_pArray->release();
            // record the array state
            m_tStateStack.push(m_tState);
            m_tArrayStack.push(m_pArray);
        }
        else
        {
            m_tState = SAX_NONE;
        }
    }

    void endElement(void *ctx, const char *name)
    {
        CC_UNUSED_PARAM(ctx);
        CCSAXState curState = m_tStateStack.empty() ? SAX_DICT : m_tStateStack.top();
        std::string sName((char*)name);
        if( sName == "dict" )
        {
            m_tStateStack.pop();
            m_tDictStack.pop();
            if ( !m_tDictStack.empty())
            {
                m_pCurDict = m_tDictStack.top();
            }
        }
        else if (sName == "array")
        {
            m_tStateStack.pop();
            m_tArrayStack.pop();
            if (! m_tArrayStack.empty())
            {
                m_pArray = m_tArrayStack.top();
            }
        }
        else if (sName == "true")
        {
            CCString *str = new CCString("1");
            if (SAX_ARRAY == curState)
            {
                m_pArray->addObject(str);
            }
            else if (SAX_DICT == curState)
            {
                m_pCurDict->setObject(str, m_sCurKey);
            }
            str->release();
        }
        else if (sName == "false")
        {
            CCString *str = new CCString("0");
            if (SAX_ARRAY == curState)
            {
                m_pArray->addObject(str);
            }
            else if (SAX_DICT == curState)
            {
                m_pCurDict->setObject(str, m_sCurKey);
            }
            str->release();
        }
        m_tState = SAX_NONE;
    }

    void textHandler(void *ctx, const char *ch, int len)
    {
        CC_UNUSED_PARAM(ctx);
        if (m_tState == SAX_NONE)
        {
            return;
        }

        CCSAXState curState = m_tStateStack.empty() ? SAX_DICT : m_tStateStack.top();
        CCString *pText = new CCString();
        pText->m_sString = std::string((char*)ch,0,len);

        switch(m_tState)
        {
        case SAX_KEY:
            m_sCurKey = pText->m_sString;
            break;
        case SAX_INT:
        case SAX_REAL:
        case SAX_STRING:
            {
                if (SAX_ARRAY == curState)
                {
                    m_pArray->addObject(pText);
                }
                else if (SAX_DICT == curState)
                {
                    CCAssert(!m_sCurKey.empty(), "not found key : <integet/real>");
                    m_pCurDict->setObject(pText, m_sCurKey);
                }
                break;
            }
        default:
            break;
        }
        pText->release();
    }
};

std::string& CCFileUtils::removeSuffixFromFile(std::string& path)
{
	// XXX win32 now can only support iphone retina, because 
	// we don't know it is ipad retina or iphone retina.
	// fixe me later
    if( CC_CONTENT_SCALE_FACTOR() == 2 )
    {
        std::string::size_type pos = path.rfind("/") + 1; // the begin index of last part of path

        std::string::size_type suffixPos = path.rfind(__suffixiPhoneRetinaDisplay);
        if (std::string::npos != suffixPos && suffixPos > pos)
        {
            CCLog("cocos2d: FilePath(%s) contains suffix(%s), remove it.", path.c_str(),
                __suffixiPhoneRetinaDisplay);
            path.replace(suffixPos, strlen(__suffixiPhoneRetinaDisplay), "");
        }
    }

    return path;
}

CCDictionary<std::string, CCObject*> *CCFileUtils::dictionaryWithContentsOfFile(const char *pFileName)
{
	//convert to full path
	std::string fileFullPath;
	fileFullPath = CCFileUtils::fullPathFromRelativePath(pFileName);
	CCDictionary<std::string, CCObject*> *ret = dictionaryWithContentsOfFileThreadSafe(fileFullPath.c_str());
	ret->autorelease();

	return ret;
}

CCDictionary<std::string, CCObject*> *CCFileUtils::dictionaryWithContentsOfFileThreadSafe(const char *pFileName)
{
	CCDictMaker tMaker;
    return tMaker.dictionaryWithContentsOfFile(pFileName);
}

CCMutableArray<CCObject*>* CCFileUtils::arrayWithContentsOfFileThreadSafe(const char* pFileName)
{
    CCDictMaker tMaker;
    return tMaker.arrayWithContentsOfFile(pFileName);
}

CCMutableArray<CCObject*>* CCFileUtils::arrayWithContentsOfFile(const char* pFileName)
{
	//convert to full path
	std::string fileFullPath;
	fileFullPath = CCFileUtils::fullPathFromRelativePath(pFileName);
	CCMutableArray<CCObject*>* ret = arrayWithContentsOfFileThreadSafe(fileFullPath.c_str());
    ret->autorelease();
    return ret;
}

unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

class CacheDataInfo
{
public:
    CacheDataInfo()
    : data(NULL)
    , size(0)
    {
    }

    ~CacheDataInfo()
    {
        CC_SAFE_DELETE_ARRAY(data);
    }

    unsigned char* data;
    unsigned long size;
};

class CacheHandler
{
    std::map<unsigned int, CacheDataInfo*> * s_pCache;
public:
    CacheHandler() : s_pCache(0)
    {

    }

    ~CacheHandler()
    {
        CCFileUtils::purgeCachedFileData();
        delete s_pCache;
    }

    std::map<unsigned int, CacheDataInfo*>& getCache()
    {
        if (! s_pCache)
        {
            s_pCache = new std::map<unsigned int, CacheDataInfo*>;
        }
        return *s_pCache;
    }
};

static CacheHandler s_CacheHandler;

unsigned char* CCFileUtils::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
    unsigned char * pBuffer = NULL;
    std::string strFileName = pszFileName;
    std::string extension = "";
    int nExPos = strFileName.rfind('.');
    if (nExPos != std::string::npos)
    {
        extension = strFileName.substr(nExPos);
    }

    do 
    {
        // only cache plist file data now
        if (0 != extension.compare(".plist"))
        {
            pBuffer = getFileDataPlatform(pszFileName, pszMode, pSize);
            break;
        }

        // get data from cache 
        unsigned int hashKey = BKDRHash((char*)pszFileName);
        CacheDataInfo* pTemp = NULL;
        std::map<unsigned int, CacheDataInfo*>::iterator it = s_CacheHandler.getCache().find(hashKey);
        if (it != s_CacheHandler.getCache().end())
        {
            pTemp = it->second;
            pBuffer = new unsigned char[pTemp->size];
            memcpy(pBuffer, pTemp->data, pTemp->size);
            *pSize = pTemp->size;
            CCLOG("------------- get %s data from cache --------------", pszFileName);
            break;
        }


        CCLOG("------------- get %s data from file--------------", pszFileName);
        // get data from file, and cache the data
        pBuffer = getFileDataPlatform(pszFileName, pszMode, pSize);
        CacheDataInfo* pCache = new CacheDataInfo();
        pCache->data = new unsigned char[*pSize];
        pCache->size = *pSize;
        memcpy(pCache->data, pBuffer, pCache->size);
        s_CacheHandler.getCache().insert(std::pair<unsigned int, CacheDataInfo*>(hashKey, pCache));
    } while (0);

    return pBuffer;
}

void CCFileUtils::purgeCachedFileData()
{
    std::map<unsigned int, CacheDataInfo*>::iterator it;
    for (it = s_CacheHandler.getCache().begin(); it != s_CacheHandler.getCache().end(); it++)
    {
        CacheDataInfo* pCache = it->second;
        CC_SAFE_DELETE(pCache);
    }
    s_CacheHandler.getCache().clear();
}

unsigned char* CCFileUtils::getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long * pSize)
{
    unsigned char * pBuffer = NULL;
 /* unzFile pFile = NULL;
    *pSize = 0;

    do 
    {
        CC_BREAK_IF(!pszZipFilePath || !pszFileName);
        CC_BREAK_IF(strlen(pszZipFilePath) == 0);

        pFile = unzOpen(pszZipFilePath);
        CC_BREAK_IF(!pFile);

        int nRet = unzLocateFile(pFile, pszFileName, 1);
        CC_BREAK_IF(UNZ_OK != nRet);

        char szFilePathA[260];
        unz_file_info FileInfo;
        nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
        CC_BREAK_IF(UNZ_OK != nRet);

        nRet = unzOpenCurrentFile(pFile);
        CC_BREAK_IF(UNZ_OK != nRet);

        pBuffer = new unsigned char[FileInfo.uncompressed_size];
        int nSize = 0;
        nSize = unzReadCurrentFile(pFile, pBuffer, FileInfo.uncompressed_size);
        CCAssert(nSize == 0 || nSize == (int)FileInfo.uncompressed_size, "the file size is wrong");

        *pSize = FileInfo.uncompressed_size;
        unzCloseCurrentFile(pFile);
    } while (0);

    if (pFile)
    {
        unzClose(pFile);
    }
*/
    return pBuffer;
}


/// functions iOS specific
void CCFileUtils::setiPhoneRetinaDisplaySuffix(const char *suffix)
{
}

void CCFileUtils::setiPadSuffix(const char *suffix)
{
}

void CCFileUtils::setiPadRetinaDisplaySuffix(const char *suffix)
{
}

bool CCFileUtils::iPadFileExistsAtPath(const char *filename)
{
	return false;
}

bool CCFileUtils::iPadRetinaDisplayFileExistsAtPath(const char *filename)
{
	return false;
}

bool CCFileUtils::iPhoneRetinaDisplayFileExistsAtPath(const char *filename)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Notification support when getFileData from invalid file path.
//////////////////////////////////////////////////////////////////////////
static bool s_bPopupNotify = true;

void CCFileUtils::setIsPopupNotify(bool bNotify)
{
    s_bPopupNotify = bNotify;
}

bool CCFileUtils::getIsPopupNotify()
{
    return s_bPopupNotify;
}

NS_CC_END;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "win32/CCFileUtils_win32.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)
#include "wophone/CCFileUtils_wophone.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "android/CCFileUtils_android.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
#include "marmalade/CCFileUtils_marmalade.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)
#include "bada/CCFileUtils_bada.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
#include "qnx/CCFileUtils_qnx.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#include "Linux/CCFileUtils_Linux.cpp"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)
    //#include "win8_metro/CCFileUtils_win8_metro.cpp"
#endif

#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
