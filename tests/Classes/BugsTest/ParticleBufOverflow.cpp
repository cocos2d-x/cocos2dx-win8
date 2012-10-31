/*
  Buffer overflow on CCParticlesSystemQuad:
  Should be first action, no any previous CCParticlesSystemQuad should be created.
  a) Create an object with less than 99 particles (should be rendered first)
  b) Create another object with more particles (and less than 100).
  c) Check the behaviour

  Two possibilities.
  1) In case if video driver is protected against buffer overflow
     D3D11 WARNING: ID3D11DeviceContext::DrawIndexed: Index buffer has not enough space! 
	 [ EXECUTION WARNING #359: DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL]
     visually - some flickering is possible, partially displayed particles and so on

  2) If memory (heap or something else) is really overwritten
     Random crashes, undefined behaviour

  Created by Denis Mingulov on 01.09.2012.
*/

#include "pch.h"
#include "ParticleBufOverflow.h"
#include "../testResource.h"

bool BugParticleBufOverflowLayer::init()
{
    if (BugsTestBaseLayer::init())
    {
        CCSize size = CCDirector::sharedDirector()->getWinSize();

		CCParticleFlower *first = new CCParticleFlower();
	    first->setTexture( CCTextureCache::sharedTextureCache()->addImage(s_stars1) );
		first->initWithTotalParticles(5);
		first->autorelease();
		first->setPosition(ccp(size.width/4, size.height/4));
		addChild(first);


		CCParticleFlower *second = new CCParticleFlower();
	    second->setTexture( CCTextureCache::sharedTextureCache()->addImage(s_stars2) );
		second->initWithTotalParticles(99);
		second->autorelease();
		second->setPosition(ccp(size.width*3/4, size.height*3/4));
		addChild(second);

        return true;
	}

	return false;
}
