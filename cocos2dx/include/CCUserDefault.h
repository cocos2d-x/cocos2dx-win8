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

#ifndef __SUPPORT_CCUSERDEFAULT_H__
#define __SUPPORT_CCUSERDEFAULT_H__

#include "CCPlatformMacros.h"

#include <string>


NS_CC_BEGIN;

/**
 * CCUserDefault acts as a tiny database. You can save and get base type values by it.
 * For example, setBoolForKey("played", true) will add a bool value true into the database.
 * Its key is "played". You can get the value of the key by getBoolForKey("played").
 * 
 * It supports the following base types:
 * bool, int, float, double, string
 */
class CC_DLL CCUserDefault
{
public:
	~CCUserDefault();

	// get value methods

	/**
	@brief Get bool value by key, if the key doesn't exist, a default value will return.
	 You can set the default value, or it is false.
	*/
	bool	getBoolForKey(const char* pKey, bool defaultValue = false);
	/**
	@brief Get integer value by key, if the key doesn't exist, a default value will return.
	 You can set the default value, or it is 0.
	*/
	int		getIntegerForKey(const char* pKey, int defaultValue = 0);
	/**
	@brief Get float value by key, if the key doesn't exist, a default value will return.
	 You can set the default value, or it is 0.0f.
	*/
	float	getFloatForKey(const char* pKey, float defaultValue=0.0f);
	/**
	@brief Get double value by key, if the key doesn't exist, a default value will return.
	 You can set the default value, or it is 0.0.
	*/
	double  getDoubleForKey(const char* pKey, double defaultValue=0.0);
	/**
	@brief Get string value by key, if the key doesn't exist, a default value will return.
	You can set the default value, or it is "".
	*/
	std::string getStringForKey(const char* pKey, const std::string & defaultValue = "");

	// set value methods

	/**
	@brief Set bool value by key.
	*/
	void	setBoolForKey(const char* pKey, bool value);
	/**
	@brief Set integer value by key.
	*/
	void	setIntegerForKey(const char* pKey, int value);
	/**
	@brief Set float value by key.
	*/
	void	setFloatForKey(const char* pKey, float value);
	/**
	@brief Set double value by key.
	*/
	void	setDoubleForKey(const char* pKey, double value);
	/**
	@brief Set string value by key.
	*/
	void	setStringForKey(const char* pKey, const std::string & value);

	static CCUserDefault* sharedUserDefault();
	static void purgeSharedUserDefault();
	const static std::string& getXMLFilePath();

private:
	CCUserDefault() {}
	static bool createXMLFile();
	static bool isXMLFileExist();
	static void initXMLFilePath();
	
	static CCUserDefault* m_spUserDefault;
	static std::string m_sFilePath;
	static bool m_sbIsFilePathInitialized;
};

NS_CC_END;

#endif // __SUPPORT_CCUSERDEFAULT_H__
