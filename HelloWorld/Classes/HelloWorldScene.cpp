#include "HelloWorldScene.h"


USING_NS_CC;


CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();


	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::node();


	// add layer as a child to scene
	scene->addChild(layer);


	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}





	/////////////////////////////
	// 3. add your codes below...


	// add a label shows "Hello World"
	// create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::labelWithString("Hello World", "Arial", 24);
	// ask director the window size
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	// position the label on the center of the screen
	pLabel->setPosition( ccp(size.width / 2, size.height - 50) );


	// add the label as a child to this layer
	this->addChild(pLabel, 1);


	// add "HelloWorld" splash screen"
	CCSprite* pSprite = CCSprite::spriteWithFile("HelloWorld.png");


	// position the sprite on the center of the screen
	pSprite->setPosition( ccp(size.width/2, size.height/2) );


	// add the sprite as a child to this layer
	this->addChild(pSprite, 0);


	return true;
}
