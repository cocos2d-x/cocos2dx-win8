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

#ifndef __CC_COMMON_H__
#define __CC_COMMON_H__

#include "CCPlatformMacros.h"
#include <string>

NS_CC_BEGIN;

/// The max length of CCLog message.
static const int kMaxLogLen = 16*1024;

/**
@brief Output Debug message.
*/
void CC_DLL CCLog(const char * pszFormat, ...);

/**
 * lua can not deal with ...
 */
inline void CC_DLL CCLuaLog(const char * pszFormat)
{
	CCLog(pszFormat);
}

std::wstring CC_DLL CCUtf8ToUnicode(const char * pszUtf8Str);
std::string CC_DLL CCUnicodeToUtf8(const wchar_t* pwszStr);

/**
@brief Pop out a message box
*/
void CC_DLL CCMessageBox(const char * pszMsg, const char * pszTitle);

/**
@brief Enum the language type supportted now
*/
typedef enum LanguageType
{
    kLanguageEnglish = 0,
    kLanguageChinese,
    kLanguageChinese_Simplified = kLanguageChinese,
    kLanguageChinese_Traditional,
    kLanguageJapanese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageRussian
} ccLanguageType;

NS_CC_END;

#endif	// __CC_COMMON_H__
