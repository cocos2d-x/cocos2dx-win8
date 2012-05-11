#include "SimpleAudioEngine.h"
#include "Audio.h"

#include <map>

#include "CCCommon.h"
using namespace std;

namespace CocosDenshion {

Audio* s_audioController = NULL;
static Audio* sharedAudioController()
{
    if (! s_audioController)
    {
        s_audioController = new Audio;
        s_audioController->Initialize();
        s_audioController->CreateResources();
    }

    return s_audioController;
}

SimpleAudioEngine::SimpleAudioEngine()
{
}

SimpleAudioEngine::~SimpleAudioEngine()
{
}

SimpleAudioEngine* SimpleAudioEngine::sharedEngine()
{
    static SimpleAudioEngine s_SharedEngine;
    return &s_SharedEngine;
}

void SimpleAudioEngine::end()
{
    // 停止音乐播放并回收资源
    sharedAudioController()->StopBackgroundMusic(true);
    sharedAudioController()->StopAllSoundEffects();
    sharedAudioController()->ReleaseResources();
}

void SimpleAudioEngine::setResource(const char* pszZipFileName)
{
}

void SimpleAudioEngine::render()
{
    sharedAudioController()->Render();
}

//////////////////////////////////////////////////////////////////////////
// BackgroundMusic
//////////////////////////////////////////////////////////////////////////

void SimpleAudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    if (! pszFilePath)
    {
        return;
    }

    sharedAudioController()->PlayBackgroundMusic(pszFilePath, bLoop);
}

void SimpleAudioEngine::stopBackgroundMusic(bool bReleaseData)
{
    sharedAudioController()->StopBackgroundMusic(bReleaseData);
}

void SimpleAudioEngine::pauseBackgroundMusic()
{
    sharedAudioController()->PauseBackgroundMusic();
}

void SimpleAudioEngine::resumeBackgroundMusic()
{
    sharedAudioController()->ResumeBackgroundMusic();
}

void SimpleAudioEngine::rewindBackgroundMusic()
{
    sharedAudioController()->RewindBackgroundMusic();
}

bool SimpleAudioEngine::willPlayBackgroundMusic()
{
    return false;
}

bool SimpleAudioEngine::isBackgroundMusicPlaying()
{
    return sharedAudioController()->IsBackgroundMusicPlaying();
}

//////////////////////////////////////////////////////////////////////////
// effect function
//////////////////////////////////////////////////////////////////////////

unsigned int SimpleAudioEngine::playEffect(const char* pszFilePath, bool bLoop)
{
    unsigned int sound;
    sharedAudioController()->PlaySoundEffect(pszFilePath, bLoop, sound);

    return sound;
}

void SimpleAudioEngine::stopEffect(unsigned int nSoundId)
{
    sharedAudioController()->StopSoundEffect(nSoundId);
}

void SimpleAudioEngine::preloadEffect(const char* pszFilePath)
{
    sharedAudioController()->PreloadSoundEffect(pszFilePath);
}

void SimpleAudioEngine::pauseEffect(unsigned int nSoundId)
{
    sharedAudioController()->PauseSoundEffect(nSoundId);
}

void SimpleAudioEngine::resumeEffect(unsigned int nSoundId)
{
    sharedAudioController()->ResumeSoundEffect(nSoundId);
}

void SimpleAudioEngine::pauseAllEffects()
{
    sharedAudioController()->PauseAllSoundEffects();
}

void SimpleAudioEngine::resumeAllEffects()
{
    sharedAudioController()->ResumeAllSoundEffects();
}

void SimpleAudioEngine::stopAllEffects()
{
    sharedAudioController()->StopAllSoundEffects();
}

void SimpleAudioEngine::preloadBackgroundMusic(const char* pszFilePath)
{

}

void SimpleAudioEngine::unloadEffect(const char* pszFilePath)
{
    sharedAudioController()->UnloadSoundEffect(pszFilePath);
}

//////////////////////////////////////////////////////////////////////////
// volume interface
//////////////////////////////////////////////////////////////////////////

float SimpleAudioEngine::getBackgroundMusicVolume()
{
    return sharedAudioController()->GetBackgroundVolume();
}

void SimpleAudioEngine::setBackgroundMusicVolume(float volume)
{
    sharedAudioController()->SetBackgroundVolume(volume);
}

float SimpleAudioEngine::getEffectsVolume()
{
    return sharedAudioController()->GetSoundEffectVolume();
}

void SimpleAudioEngine::setEffectsVolume(float volume)
{
    sharedAudioController()->SetSoundEffectVolume(volume);
}

} // end of namespace CocosDenshion
