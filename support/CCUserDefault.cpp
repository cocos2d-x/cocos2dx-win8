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
#include "CCUserDefault.h"
#include "platform/CCFileUtils.h"
#include "tinyxml\tinyxml.h"


// root name of xml
#define USERDEFAULT_ROOT_NAME	"userDefaultRoot"

#define XML_FILE_NAME "UserDefault.xml"
#define WSTR_XML_FILE_NAME L"UserDefault.xml"

using namespace std;

NS_CC_BEGIN;

static TiXmlElement* getXMLNodeForKey(const char* pKey, TiXmlElement** rootNode, TiXmlDocument **doc)
{
	TiXmlElement* curNode = NULL;
	// check the key value
	if (! pKey)
	{
		return NULL;
	}

	do 
	{
		TiXmlDocument* xmlDoc = new TiXmlDocument();
		*doc = xmlDoc;
		CCFileData data(CCUserDefault::sharedUserDefault()->getXMLFilePath().c_str(),"rt");
		const char* pXmlBuffer = (const char*)data.getBuffer();
		if(NULL == pXmlBuffer)
		{
			CCLOG("can not read xml file");
			break;
		}
		xmlDoc->Parse(pXmlBuffer);
		// get root node
		*rootNode = xmlDoc->RootElement();
		if (NULL == *rootNode)
		{
			CCLOG("read root node error");
			break;
		}
		// find the node
		curNode = (*rootNode)->FirstChildElement();
		while (NULL != curNode)
		{
			const char* nodeName = curNode->Value();
			if (!strcmp(nodeName, pKey))
			{
				break;
			}

			curNode = curNode->NextSiblingElement();
		}
	} while (0);

	return curNode;

	
}

static void setValueForKey(const char* pKey, const char* pValue)
{
	TiXmlElement* rootNode;
	TiXmlDocument* doc;
	TiXmlElement* node;
	// check the params
	if (! pKey || ! pValue)
	{
		return;
	}
	// find the node
	node = getXMLNodeForKey(pKey, &rootNode, &doc);
	// if node exist, change the content
	if (node)
	{
		node->FirstChild()->SetValue(pValue);
	}
	else
	{
		if (rootNode)
		{
			TiXmlElement* tmpNode = new TiXmlElement(pKey);
			rootNode->LinkEndChild(tmpNode);

			TiXmlText* content = new TiXmlText(pValue);
			tmpNode->LinkEndChild(content);
			
		}	
	}

		// save file and free doc
	if (doc)
	{
		doc->SaveFile(CCUserDefault::sharedUserDefault()->getWStrXMLFilePath().c_str());
		delete doc;
	}
}

/**
 * implements of CCUserDefault
 */

CCUserDefault* CCUserDefault::m_spUserDefault = 0;
string CCUserDefault::m_sFilePath = string("");
wstring CCUserDefault::m_wsFilePath = wstring(L"");
bool CCUserDefault::m_sbIsFilePathInitialized = false;

/**
 * If the user invoke delete CCUserDefault::sharedUserDefault(), should set m_spUserDefault
 * to null to avoid error when he invoke CCUserDefault::sharedUserDefault() later.
 */
CCUserDefault::~CCUserDefault()
{
	m_spUserDefault = NULL;
}

void CCUserDefault::purgeSharedUserDefault()
{
	CC_SAFE_DELETE(m_spUserDefault);
	m_spUserDefault = NULL;
}

bool CCUserDefault::getBoolForKey(const char* pKey, bool defaultValue)
{
    const char* value = NULL;
	TiXmlElement* rootNode;
	TiXmlDocument* doc;
	TiXmlElement* node;
	node =  getXMLNodeForKey(pKey, &rootNode, &doc);
	// find the node
	if (node)
	{
        value = (const char*)(node->FirstChild()->Value());
	}

	bool ret = defaultValue;

	if (value)
	{
		ret = (! strcmp(value, "true"));
	}

    if (doc) delete doc;

	return ret;
}

int CCUserDefault::getIntegerForKey(const char* pKey, int defaultValue)
{
	const char* value = NULL;
	TiXmlElement* rootNode;
	TiXmlDocument* doc;
	TiXmlElement* node;
	node =  getXMLNodeForKey(pKey, &rootNode, &doc);
	// find the node
	if (node)
	{
        value = (const char*)(node->FirstChild()->Value());
	}

	int ret = defaultValue;

	if (value)
	{
		ret = atoi(value);
	}

	if(doc)
	{
		delete doc;
	}


	return ret;
}

float CCUserDefault::getFloatForKey(const char* pKey, float defaultValue)
{
	float ret = (float)getDoubleForKey(pKey, (double)defaultValue);
 
	return ret;
}

double CCUserDefault::getDoubleForKey(const char* pKey, double defaultValue)
{
    const char* value = NULL;
	TiXmlElement* rootNode;
	TiXmlDocument* doc;
	TiXmlElement* node;
	node =  getXMLNodeForKey(pKey, &rootNode, &doc);
	// find the node
	if (node)
	{
        value = (const char*)(node->FirstChild()->Value());
	}

	double ret = defaultValue;

	if (value)
	{
		ret = atof(value);
	}

    if (doc) delete doc;

	return ret;
}

string CCUserDefault::getStringForKey(const char* pKey, const std::string & defaultValue)
{
    const char* value = NULL;
	TiXmlElement* rootNode;
	TiXmlDocument* doc;
	TiXmlElement* node;
	node =  getXMLNodeForKey(pKey, &rootNode, &doc);
	// find the node
	if (node)
	{
        value = (const char*)(node->FirstChild()->Value());
	}

	string ret = defaultValue;

	if (value)
	{
		ret = string(value);
	}

    if (doc) delete doc;

	return ret;
}

void CCUserDefault::setBoolForKey(const char* pKey, bool value)
{
    // save bool value as sring

	if (true == value)
	{
		setStringForKey(pKey, "true");
	}
	else
	{
		setStringForKey(pKey, "false");
	}
}

void CCUserDefault::setIntegerForKey(const char* pKey, int value)
{
	// check key
	if (! pKey)
	{
		return;
	}

	// format the value
	char tmp[50];
	memset(tmp, 0, 50);
	sprintf(tmp, "%d", value);

	setValueForKey(pKey, tmp);
}

void CCUserDefault::setFloatForKey(const char* pKey, float value)
{
	setDoubleForKey(pKey, value);
}

void CCUserDefault::setDoubleForKey(const char* pKey, double value)
{
	// check key
	if (! pKey)
	{
		return;
	}

	// format the value
    char tmp[50];
	memset(tmp, 0, 50);
	sprintf(tmp, "%f", value);

	setValueForKey(pKey, tmp);
}

void CCUserDefault::setStringForKey(const char* pKey, const std::string & value)
{
	// check key
	if (! pKey)
	{
		return;
	}

	setValueForKey(pKey, value.c_str());
}

CCUserDefault* CCUserDefault::sharedUserDefault()
{
	initXMLFilePath();

	// only create xml file one time
	// the file exists after the programe exit
	if ((! isXMLFileExist()) && (! createXMLFile()))
	{
		return NULL;
	}

	if (! m_spUserDefault)
	{
		m_spUserDefault = new CCUserDefault();
	}

	return m_spUserDefault;
}

bool CCUserDefault::isXMLFileExist()
{
	wstring filepath = m_wsFilePath.c_str();
	FILE *fp = _wfopen(filepath.c_str(), L"r");
	bool bRet = false;

	if (fp)
	{
		bRet = true;
		fclose(fp);
	}

	return bRet;
}

void CCUserDefault::initXMLFilePath()
{
	if (! m_sbIsFilePathInitialized)
	{
		m_sFilePath += CCFileUtils::getWriteablePath() + XML_FILE_NAME;
		m_wsFilePath += CCUtf8ToUnicode(CCFileUtils::getWriteablePath().c_str()) + WSTR_XML_FILE_NAME;
		m_sbIsFilePathInitialized = true;
	}	
}

// create new xml file
bool CCUserDefault::createXMLFile()
{
	bool bRet = false;
	// ?¡§¨°?¨°???TiXmlDocument¨¤¨¤????   
    TiXmlDocument *pDoc = new TiXmlDocument;  
    if (NULL==pDoc)  
    {  
        return false;  
    }  
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","","");  
	if (NULL==pDeclaration)  
	{  
		return false;  
	}  
	pDoc->LinkEndChild(pDeclaration); 
	// ¨¦¨²3¨¦¨°????¨´?¨²¦Ì?¡êoMyApp  
	TiXmlElement *pRootEle = new TiXmlElement(USERDEFAULT_ROOT_NAME);  
	if (NULL==pRootEle)  
	{  
		return false;  
	}  
	pDoc->LinkEndChild(pRootEle);  
	bRet = pDoc->SaveFile(m_wsFilePath.c_str());

	if(pDoc)
	{
		delete pDoc;
	}

	return bRet;
}

const string& CCUserDefault::getXMLFilePath()
{
	return m_sFilePath;
}

const wstring& CCUserDefault::getWStrXMLFilePath()
{
	return m_wsFilePath;
}

void CCUserDefault::flush()
{
}

NS_CC_END;
