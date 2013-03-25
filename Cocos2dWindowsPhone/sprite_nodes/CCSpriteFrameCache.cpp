/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2009      Jason Booth
* Copyright (c) 2009      Robert J Payne
* Copyright (c) 2011      Zynga Inc.
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

#include "cocoa/CCNS.h"
#include "ccMacros.h"
#include "CCTextureCache.h"
#include "CCSpriteFrameCache.h"
#include "CCSpriteFrame.h"
#include "CCSprite.h"
#include "TransformUtils.h"
#include "CCFileUtils.h"
#include "CCString.h"

using namespace std;

NS_CC_BEGIN

static CCSpriteFrameCache *pSharedSpriteFrameCache = NULL;

CCSpriteFrameCache* CCSpriteFrameCache::sharedSpriteFrameCache(void)
{
	if (! pSharedSpriteFrameCache)
	{
		pSharedSpriteFrameCache = new CCSpriteFrameCache();
        pSharedSpriteFrameCache->init();
	}

	return pSharedSpriteFrameCache;
}

void CCSpriteFrameCache::purgeSharedSpriteFrameCache(void)
{
	CC_SAFE_RELEASE_NULL(pSharedSpriteFrameCache);
}

bool CCSpriteFrameCache::init(void)
{
	m_pSpriteFrames= new CCDictionary();
	m_pSpriteFramesAliases = new CCDictionary();
	return true;
}

CCSpriteFrameCache::~CCSpriteFrameCache(void)
{
	CC_SAFE_RELEASE(m_pSpriteFrames);
	CC_SAFE_RELEASE(m_pSpriteFramesAliases);
}

void CCSpriteFrameCache::addSpriteFramesWithDictionary(CCDictionary *dictionary, CCTexture2D *pobTexture)
{
    /*
    Supported Zwoptex Formats:

    ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
    ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
    ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
    ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+
    */

    CCDictionary *metadataDict = (CCDictionary*)dictionary->objectForKey("metadata");
    CCDictionary *framesDict = (CCDictionary*)dictionary->objectForKey("frames");
    int format = 0;

    // get the format
    if(metadataDict != NULL) 
    {
        format = metadataDict->valueForKey("format")->intValue();
    }

    // check the format
    CCAssert(format >=0 && format <= 3, "format is not supported for CCSpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(framesDict, pElement)
    {
        CCDictionary* frameDict = (CCDictionary*)pElement->getObject();
        std::string spriteFrameName = pElement->getStrKey();
        CCSpriteFrame* spriteFrame = (CCSpriteFrame*)m_pSpriteFrames->objectForKey(spriteFrameName);
        if (spriteFrame)
        {
            continue;
        }
        
        if(format == 0) 
        {
            float x = frameDict->valueForKey("x")->floatValue();
            float y = frameDict->valueForKey("y")->floatValue();
            float w = frameDict->valueForKey("width")->floatValue();
            float h = frameDict->valueForKey("height")->floatValue();
            float ox = frameDict->valueForKey("offsetX")->floatValue();
            float oy = frameDict->valueForKey("offsetY")->floatValue();
            int ow = frameDict->valueForKey("originalWidth")->intValue();
            int oh = frameDict->valueForKey("originalHeight")->intValue();
            // check ow/oh
            if(!ow || !oh)
            {
                //CCLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the CCSpriteFrame. AnchorPoint won't work as expected. Regenrate the .plist");
            }
            // abs ow/oh
            ow = abs(ow);
            oh = abs(oh);
            // create frame
            spriteFrame = new CCSpriteFrame();
            spriteFrame->initWithTexture(pobTexture, 
                                        CCRectMake(x, y, w, h), 
                                        false,
                                        CCPointMake(ox, oy),
                                        CCSizeMake((float)ow, (float)oh)
                                        );
        } 
        else if(format == 1 || format == 2) 
        {
            CCRect frame = CCRectFromString(frameDict->valueForKey("frame")->getCString());
            bool rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = frameDict->valueForKey("rotated")->boolValue();
            }

            CCPoint offset = CCPointFromString(frameDict->valueForKey("offset")->getCString());
            CCSize sourceSize = CCSizeFromString(frameDict->valueForKey("sourceSize")->getCString());

            // create frame
            spriteFrame = new CCSpriteFrame();
            spriteFrame->initWithTexture(pobTexture, 
                frame,
                rotated,
                offset,
                sourceSize
                );
        } 
        else if (format == 3)
        {
            // get values
            CCSize spriteSize = CCSizeFromString(frameDict->valueForKey("spriteSize")->getCString());
            CCPoint spriteOffset = CCPointFromString(frameDict->valueForKey("spriteOffset")->getCString());
            CCSize spriteSourceSize = CCSizeFromString(frameDict->valueForKey("spriteSourceSize")->getCString());
            CCRect textureRect = CCRectFromString(frameDict->valueForKey("textureRect")->getCString());
            bool textureRotated = frameDict->valueForKey("textureRotated")->boolValue();

            // get aliases
            CCArray* aliases = (CCArray*) (frameDict->objectForKey("aliases"));
            CCString * frameKey = new CCString(spriteFrameName);

            CCObject* pObj = NULL;
            CCARRAY_FOREACH(aliases, pObj)
            {
                std::string oneAlias = ((CCString*)pObj)->getCString();
                if (m_pSpriteFramesAliases->objectForKey(oneAlias.c_str()))
                {
                    //CCLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
                }

                m_pSpriteFramesAliases->setObject(frameKey, oneAlias.c_str());
            }
            frameKey->release();
            // create frame
            spriteFrame = new CCSpriteFrame();
            spriteFrame->initWithTexture(pobTexture,
                            CCRectMake(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                            textureRotated,
                            spriteOffset,
                            spriteSourceSize);
        }

        // add sprite frame
        m_pSpriteFrames->setObject(spriteFrame, spriteFrameName);
        spriteFrame->release();
    }
}

void CCSpriteFrameCache::addSpriteFramesWithFile(const char *pszPlist, CCTexture2D *pobTexture)
{
	const char *pszPath = CCFileUtils::fullPathFromRelativePath(pszPlist);
	CCDictionary *dict = CCFileUtils::dictionaryWithContentsOfFileThreadSafe(pszPath);

	addSpriteFramesWithDictionary(dict, pobTexture);

	dict->release();
}

void CCSpriteFrameCache::addSpriteFramesWithFile(const char* plist, const char* textureFileName)
{
	CCAssert(textureFileName, "texture name should not be null");
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(textureFileName);

	if (texture)
	{
		addSpriteFramesWithFile(plist, texture);
	}
	else
	{
		CCLOG("cocos2d: CCSpriteFrameCache: couldn't load texture file. File not found %s", textureFileName);
	}
}

void CCSpriteFrameCache::addSpriteFramesWithFile(const char *pszPlist)
{
	const char *pszPath = CCFileUtils::fullPathFromRelativePath(pszPlist);
	CCDictionary *dict = CCFileUtils::dictionaryWithContentsOfFileThreadSafe(pszPath);
	
	string texturePath("");

	CCDictionary* metadataDict = (CCDictionary*)dict->objectForKey(string("metadata"));
    if (metadataDict)
	{
		// try to read  texture file name from meta data
		texturePath = string(valueForKey("textureFileName", metadataDict));
	}

	if (! texturePath.empty())
	{
		// build texture path relative to plist file
        texturePath = CCFileUtils::fullPathFromRelativeFile(texturePath.c_str(), pszPath);
	}
	else
	{
		// build texture path by replacing file extension
        texturePath = pszPath;

		// remove .xxx
		size_t startPos = texturePath.find_last_of("."); 
		texturePath = texturePath.erase(startPos);

		// append .png
		texturePath = texturePath.append(".png");

		CCLOG("cocos2d: CCSpriteFrameCache: Trying to use file %s as texture", texturePath.c_str());
	}

	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(texturePath.c_str());

	if (pTexture)
	{
        addSpriteFramesWithDictionary(dict, pTexture);
	}
	else
	{
		CCLOG("cocos2d: CCSpriteFrameCache: Couldn't load texture");
	}

	dict->release();
}

void CCSpriteFrameCache::addSpriteFrame(CCSpriteFrame *pobFrame, const char *pszFrameName)
{
	m_pSpriteFrames->setObject(pobFrame, std::string(pszFrameName));
}

void CCSpriteFrameCache::removeSpriteFrames(void)
{
	m_pSpriteFrames->removeAllObjects();
	m_pSpriteFramesAliases->removeAllObjects();
}

void CCSpriteFrameCache::removeUnusedSpriteFrames(void)
{
	//m_pSpriteFrames->begin();
	//std::string key = "";
	//CCSpriteFrame *spriteFrame = NULL;
	//while( (spriteFrame = m_pSpriteFrames->next(&key)) )
	//{
	//	if( spriteFrame->retainCount() == 1 ) 
	//	{
	//		CCLOG("cocos2d: CCSpriteFrameCache: removing unused frame: %s", key.c_str());
	//		m_pSpriteFrames->removeObjectForKey(key);
	//	}
	//}
	//m_pSpriteFrames->end();
	bool bRemoved = false;
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(m_pSpriteFrames, pElement)
    {
        CCSpriteFrame* spriteFrame = (CCSpriteFrame*)pElement->getObject();
        if( spriteFrame->retainCount() == 1 ) 
        {
            CCLOG("cocos2d: CCSpriteFrameCache: removing unused frame: %s", pElement->getStrKey());
            m_pSpriteFrames->removeObjectForElememt(pElement);
            bRemoved = true;
        }
    }

    //// XXX. Since we don't know the .plist file that originated the frame, we must remove all .plist from the cache
    //if( bRemoved )
    //{
    //    m_pLoadedFileNames->clear();
    //}
}


void CCSpriteFrameCache::removeSpriteFrameByName(const char *pszName)
{
	// explicit nil handling
	if( ! pszName )
	{
		return;
	}

	// Is this an alias ?
	CCString *key = (CCString*)m_pSpriteFramesAliases->objectForKey(string(pszName));

	if (key)
	{
        m_pSpriteFrames->removeObjectForKey(key->m_sString);
		m_pSpriteFramesAliases->removeObjectForKey(key->m_sString);
	}
	else
	{
        m_pSpriteFrames->removeObjectForKey(std::string(pszName));
	}
}

void CCSpriteFrameCache::removeSpriteFramesFromFile(const char* plist)
{
	const char* path = CCFileUtils::fullPathFromRelativePath(plist);
	CCDictionary* dict = CCFileUtils::dictionaryWithContentsOfFileThreadSafe(path);

	removeSpriteFramesFromDictionary((CCDictionary*)dict);

	dict->release();
}

void CCSpriteFrameCache::removeSpriteFramesFromDictionary(CCDictionary *dictionary)
{
    CCDictionary* framesDict = (CCDictionary*)dictionary->objectForKey("frames");
    CCArray* keysToRemove = CCArray::create();

    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(framesDict, pElement)
    {
        if (m_pSpriteFrames->objectForKey(pElement->getStrKey()))
        {
            keysToRemove->addObject(CCString::create(pElement->getStrKey()));
        }
    }

    m_pSpriteFrames->removeObjectsForKeys(keysToRemove);
}

void CCSpriteFrameCache::removeSpriteFramesFromTexture(CCTexture2D* texture)
{
	//vector<string> keysToRemove;

	//m_pSpriteFrames->begin();
	//std::string key = "";
	//CCDictionary *frameDict = NULL;
	//while( (frameDict = (CCDictionary*)m_pSpriteFrames->next(&key)) )
	//{
	//	CCSpriteFrame *frame = m_pSpriteFrames->objectForKey(key);
	//	if (frame && (frame->getTexture() == texture))
	//	{
	//		keysToRemove.push_back(key);
	//	}
	//}
	//m_pSpriteFrames->end();

	//vector<string>::iterator iter;
	//for (iter = keysToRemove.begin(); iter != keysToRemove.end(); ++iter)
	//{
	//	m_pSpriteFrames->removeObjectForKey(*iter);
	//}
	CCArray* keysToRemove = CCArray::create();

    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(m_pSpriteFrames, pElement)
    {
        string key = pElement->getStrKey();
        CCSpriteFrame* frame = (CCSpriteFrame*)m_pSpriteFrames->objectForKey(key.c_str());
        if (frame && (frame->getTexture() == texture))
        {
            keysToRemove->addObject(CCString::create(pElement->getStrKey()));
        }
    }

    m_pSpriteFrames->removeObjectsForKeys(keysToRemove);
}

CCSpriteFrame* CCSpriteFrameCache::spriteFrameByName(const char *pszName)
{
    CCSpriteFrame* frame = (CCSpriteFrame*)m_pSpriteFrames->objectForKey(pszName);
    if (!frame)
    {
        // try alias dictionary
        CCString *key = (CCString*)m_pSpriteFramesAliases->objectForKey(pszName);  
        if (key)
        {
            frame = (CCSpriteFrame*)m_pSpriteFrames->objectForKey(key->getCString());
            if (! frame)
            {
                CCLOG("cocos2d: CCSpriteFrameCache: Frame '%s' not found", pszName);
            }
        }
    }
    return frame;
}

const char * CCSpriteFrameCache::valueForKey(const char *key, CCDictionary *dict)
{
	if (dict)
	{
		CCString *pString = (CCString*)dict->objectForKey(std::string(key));
		return pString ? pString->m_sString.c_str() : "";
	}
	return "";
}
NS_CC_END
