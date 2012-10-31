/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2010      Steve Oldmeadow
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

#ifndef _SIMPLE_AUDIO_ENGINE_H_
#define _SIMPLE_AUDIO_ENGINE_H_

#include "Export.h"
#include <stddef.h>

namespace CocosDenshion {

/**
@class          SimpleAudioEngine
@brief  		offer a VERY simple interface to play background music & sound effect
*/
class SimpleAudioEngine
{
public:
    SimpleAudioEngine();
    ~SimpleAudioEngine();

    /**
    @brief Get the shared Engine object,it will new one when first time be called
    */
    static SimpleAudioEngine* sharedEngine();

    /**
    @brief Release the shared Engine object
    @warning It must be called before the application exit, or a memroy leak will be casued.
    */
	static void end();

    /**
    @brief  Set the zip file name
    @param pszZipFileName The relative path of the .zip file
    */
    static void setResource(const char* pszZipFileName);

    /**
     @brief execute every frame
     @param none
     */
    void render();

    /**
     @brief Preload background music
     @param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
     */
    void preloadBackgroundMusic(const char* pszFilePath);
    
    /**
    @brief Play background music
    @param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
    @param bLoop Whether the background music loop or not
    */
    void playBackgroundMusic(const char* pszFilePath, bool bLoop = false);

    /**
    @brief Stop playing background music
    @param bReleaseData If release the background music data or not.As default value is false
    */
    void stopBackgroundMusic(bool bReleaseData = false);

    /**
    @brief Pause playing background music
    */
    void pauseBackgroundMusic();

    /**
    @brief Resume playing background music
    */
    void resumeBackgroundMusic();

    /**
    @brief Rewind playing background music
    */
    void rewindBackgroundMusic();

    bool willPlayBackgroundMusic();

    /**
    @brief Whether the background music is playing
    @return If is playing return true,or return false
    */
    bool isBackgroundMusicPlaying();

    // properties
    /**
    @brief The volume of the background music max value is 1.0,the min value is 0.0
    */
    float getBackgroundMusicVolume();

    /**
    @brief set the volume of background music
    @param volume must be in 0.0~1.0
    */
    void setBackgroundMusicVolume(float volume);

    /**
    @brief The volume of the effects max value is 1.0,the min value is 0.0
    */
    float getEffectsVolume();

    /**
    @brief set the volume of sound effecs
    @param volume must be in 0.0~1.0
    */
    void setEffectsVolume(float volume);

    // for sound effects
    /**
    @brief Play sound effect
    @param pszFilePath The path of the effect file,or the FileName of T_SoundResInfo
	@bLoop Whether to loop the effect playing, default value is false
    */
    unsigned int playEffect(const char* pszFilePath, bool bLoop = false);

	/**
    @brief Pause playing sound effect
    @param nSoundId The return value of function playEffect
    */
	void pauseEffect(unsigned int nSoundId);

	/**
    @brief Pause all playing sound effect
    @param nSoundId The return value of function playEffect
    */
	void pauseAllEffects();

	/**
    @brief Resume playing sound effect
    @param nSoundId The return value of function playEffect
    */
	void resumeEffect(unsigned int nSoundId);

	/**
    @brief Resume all playing sound effect
    @param nSoundId The return value of function playEffect
    */
	void resumeAllEffects();

    /**
    @brief Stop playing sound effect
    @param nSoundId The return value of function playEffect
    */
    void stopEffect(unsigned int nSoundId);

    /**
    @brief Stop all playing sound effects
    */
    void stopAllEffects();

    /**
    @brief  		preload a compressed audio file
    @details	    the compressed audio will be decode to wave, then write into an 
    internal buffer in SimpleaudioEngine
    */
    void preloadEffect(const char* pszFilePath);

    /**
    @brief  		unload the preloaded effect from internal buffer
    @param[in]		pszFilePath		The path of the effect file,or the FileName of T_SoundResInfo
    */
    void unloadEffect(const char* pszFilePath);
};

} // end of namespace CocosDenshion

#endif // _SIMPLE_AUDIO_ENGINE_H_
