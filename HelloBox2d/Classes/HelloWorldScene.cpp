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

#include "HelloWorldScene.h"
#include "CCCommon.h"
#include "BasicLoader.h"
#include <CCParticleExamples.h>
#include <vector>
#include <time.h>


using namespace cocos2d;
using namespace Windows::Foundation;
using namespace Windows::Devices::Sensors;

b2World *world;
b2Fixture *armFixture;
b2Body *armBody;
b2RevoluteJoint *armJoint;
b2MouseJoint *mouseJoint;
b2Body *groundBody;
vector<b2Body*> *bullets;
UINT currentBullet;
b2Body *bulletBody;
b2WeldJoint *bulletJoint;
bool releasingArm;
vector<b2Body*> *targets;
vector<b2Body*> *enemies;
MyContactListener *contactListener;

#define PIX_TO_MET 0.03125f
#define MET_TO_PIX 32.0f
#define PTM_RATIO MET_TO_PIX
#define MED_FULL 210.0f
#define MED_HALF 105.0f
#define SMALL_FULL 100.0f
#define SMALL_HALF 50.0f
#define LARGE_GAP 24.0f
#define SMALL_GAP 12.0f
#define FLOOR_HEIGHT 64.0f

HelloWorld::~HelloWorld()
{
	if (_targets)
	{
		_targets->release();
		_targets = NULL;
	}

	if (_projectiles)
	{
		_projectiles->release();
		_projectiles = NULL;
	}
	if (!_CCBoxes.empty())
	{
		_CCBoxes.clear();
	}
	if (!_b2Boxes.empty())
	{
		_b2Boxes.clear();
	}
	// cpp don't need to call super dealloc
	// virtual destructor will do this
}

HelloWorld::HelloWorld()
	:_targets(NULL)
	,_projectiles(NULL)
	,_projectilesDestroyed(0)
{
}

CCScene* HelloWorld::scene()
{
	CCScene * scene = NULL;
	do 
	{		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		HelloWorld *layer = HelloWorld::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	bool bRet = false;

	do 
	{
		if ( !CCLayer::init() )
		{
			break;
		}

		this->setTouchEnabled(true);
		b2Vec2 gravity;
		gravity.Set(0.0f, -10.0f);

		world = new b2World(gravity);
		world->SetContinuousPhysics(true);

		CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

		CCSprite *sprite = CCSprite::create("bg.png" );
		sprite->setAnchorPoint(CCPointZero);
		this->addChild(sprite, -1);

		sprite = CCSprite::create("catapult_base_2.png");
		sprite->setAnchorPoint(CCPointZero);
		sprite->setPosition(ccp(181.0f, FLOOR_HEIGHT));
		this->addChild(sprite, 0);

		sprite = CCSprite::create("squirrel_1.png");
		sprite->setAnchorPoint(CCPointZero);
		sprite->setPosition(ccp(11.0f, FLOOR_HEIGHT));
		this->addChild(sprite, 0);

		sprite = CCSprite::create("catapult_base_1.png");
		sprite->setAnchorPoint(CCPointZero);
		sprite->setPosition(ccp(181.0f, FLOOR_HEIGHT));
		this->addChild(sprite, 9);

		sprite = CCSprite::create("squirrel_2.png");
		sprite->setAnchorPoint(CCPointZero);
		sprite->setPosition(ccp(240.0f, FLOOR_HEIGHT));
		this->addChild(sprite, 9);

		sprite = CCSprite::create("fg.png");
		sprite->setAnchorPoint(CCPointZero);
		this->addChild(sprite, 10);


		CCLabelTTF* pLabel = CCLabelTTF::create("Angry Squirrels !!!", "Comic Sans MS", 12);
		//CCLabelTTF* pLabel = CCLabelTTF::labelWithString("Angry Squirrels !!!", "Times New Roman", 12);
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		pLabel->setPosition( ccp(size.width / 3, size.height - 50) );
		pLabel->setColor(ccc3(160, 80, 5));
		this->addChild(pLabel, 0);

		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0,0);
		groundBody = world->CreateBody(&groundBodyDef);
		b2EdgeShape groundBox;
		b2FixtureDef boxShapeDef;
		boxShapeDef.shape = &groundBox;

		//bottom
		groundBox.Set(b2Vec2(0,FLOOR_HEIGHT/PTM_RATIO), b2Vec2(screenSize.width*10.0f/PTM_RATIO,FLOOR_HEIGHT/PTM_RATIO));
		groundBody->CreateFixture(&boxShapeDef);

		//top
		groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO));
		groundBody->CreateFixture(&boxShapeDef);

		//left
		groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(0,0));
		groundBody->CreateFixture(&boxShapeDef);

		//right
		//groundBox.Set(b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width/PTM_RATIO,0));
		//groundBody->CreateFixture(&boxShapeDef);

		CCSprite *arm = CCSprite::create("catapult_arm.png");
		//arm->setPosition(ccp(230.0f, FLOOR_HEIGHT+ 91.0f));
		this->addChild(arm, 1);

		b2BodyDef armBodyDef;
		armBodyDef.type = b2_dynamicBody;
		armBodyDef.linearDamping = 1;
		armBodyDef.angularDamping = 1;
		armBodyDef.position.Set(230.0f/PTM_RATIO, (FLOOR_HEIGHT + 91.0f)/PTM_RATIO);
		armBodyDef.userData = arm;
		armBody = world->CreateBody(&armBodyDef);

		b2PolygonShape armBox;
		b2FixtureDef armBoxDef;
		armBoxDef.shape = &armBox;
		armBoxDef.density = 0.3f;
		armBox.SetAsBox(11.0f/PTM_RATIO, 91.0f/PTM_RATIO);
		armFixture = armBody->CreateFixture(&armBoxDef);


		b2RevoluteJointDef armJointDef;
		armJointDef.Initialize(groundBody, armBody, b2Vec2(233.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
		armJointDef.enableMotor = true;
		armJointDef.enableLimit = true;
		armJointDef.motorSpeed  = -5;
		//armJointDef.lowerAngle  = CC_DEGREES_TO_RADIANS(9);
		//armJointDef.upperAngle  = CC_DEGREES_TO_RADIANS(75);
		armJointDef.maxMotorTorque = 500;

		armJoint = (b2RevoluteJoint*)world->CreateJoint(&armJointDef);
		armJoint->SetLimits(CC_DEGREES_TO_RADIANS(9), CC_DEGREES_TO_RADIANS(75));

		targets = new vector<b2Body*>;
		enemies = new vector<b2Body*>;

		contactListener = new MyContactListener();
		world->SetContactListener(contactListener);
		//this->setPosition(ccp(-480,0));

		this->schedule( schedule_selector(HelloWorld::tick) );

		this->schedule(schedule_selector(HelloWorld::resetGame), 1.0f);

		//this->resetGame();
		//start = true;
		bRet = true;
	} while (0);

	return bRet;
}

void HelloWorld::resetGame(ccTime dt)
{
	this->unschedule(schedule_selector(HelloWorld::resetGame));
	this->createBullet(6);
	this->attachBullet();
	this->createTargets();
}

void HelloWorld::resetBullet(ccTime dt)
{
	this->unschedule(schedule_selector(HelloWorld::resetBullet));
	if(enemies->size() == 0)
	{
		//woohoo you won!
	}
	else if(this->attachBullet())
	{
		this->runAction(CCMoveTo::create(2.0f, CCPointZero));
	}
}

bool HelloWorld::attachBullet()
{
	if(currentBullet < bullets->size())
	{
		bulletBody = (b2Body *)bullets->at(currentBullet++);
		bulletBody->SetTransform(b2Vec2(230.0f/PTM_RATIO, (155.0f+FLOOR_HEIGHT)/PTM_RATIO), 0.0f);
		bulletBody->SetActive(true);
		b2WeldJointDef weldJointDef;
		weldJointDef.Initialize(bulletBody, armBody, b2Vec2(230.0f/PTM_RATIO, (155.0f + FLOOR_HEIGHT)/PTM_RATIO));
		weldJointDef.collideConnected = false;
		bulletJoint = (b2WeldJoint *)world->CreateJoint(&weldJointDef);
		return true;
	}
	return false;
}

void HelloWorld::createTargets()
{
	bool explodingBoxes = false;
	this->createTarget("brick_2.png", ccp(675.0, FLOOR_HEIGHT), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(741.0, FLOOR_HEIGHT), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(741.0, FLOOR_HEIGHT+23.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_3.png", ccp(672.0, FLOOR_HEIGHT+46.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(707.0, FLOOR_HEIGHT+58.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(707.0, FLOOR_HEIGHT+81.0f), 0.0f, false, false, explodingBoxes);

	this->createTarget("head_dog.png", ccp(702.0, FLOOR_HEIGHT), 0.0f, true, false, true);
	this->createTarget("head_cat.png", ccp(680.0, FLOOR_HEIGHT+58.0f), 0.0f, true, false, true);
	this->createTarget("head_dog.png", ccp(740.0, FLOOR_HEIGHT+58.0f), 0.0f, true, false, true);

	this->createTarget("brick_2.png", ccp(770.0, FLOOR_HEIGHT), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_2.png", ccp(770.0, FLOOR_HEIGHT+46.0f), 0.0f, false, false, explodingBoxes);

	this->createTarget("head_dog.png", ccp(805.0, FLOOR_HEIGHT), 0.0f, true, false, true);

	this->createTarget("brick_platform.png", ccp(839.0, FLOOR_HEIGHT), 0.0f, false, true, false);
	this->createTarget("brick_2.png", ccp(854.0, FLOOR_HEIGHT+28.f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_2.png", ccp(854.0, FLOOR_HEIGHT+28.0f+46.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("head_cat.png", ccp(881.0, FLOOR_HEIGHT+28.0f), 0.0f, true, false, true);
	this->createTarget("brick_2.png", ccp(909.0, FLOOR_HEIGHT+28.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(909.0, FLOOR_HEIGHT+28.0f+46.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_1.png", ccp(909.0, FLOOR_HEIGHT+28.0f+46.0f+23.0f), 0.0f, false, false, explodingBoxes);
	this->createTarget("brick_2.png", ccp(882.0, FLOOR_HEIGHT+108.0f), 90.0f, false, false, explodingBoxes);

}

void HelloWorld::createTarget(const char *imageName, CCPoint position, float rotation, bool isCircle, bool isStatic, bool isEnemy)
{
	CCSprite *sprite = CCSprite::create(imageName);
	this->addChild(sprite, 1);
	b2BodyDef bodyDef;
	bodyDef.type = isStatic?b2_staticBody:b2_dynamicBody;

	bodyDef.position.Set((position.x+sprite->getContentSize().width/2.0f)/PTM_RATIO,
		(position.y+sprite->getContentSize().height/2.0f)/PTM_RATIO);
	bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation);
	bodyDef.userData = sprite;
	b2Body *body = world->CreateBody(&bodyDef);

	b2FixtureDef boxDef;
	b2CircleShape circle;
	circle.m_radius = sprite->getContentSize().width/2.0f/PTM_RATIO;
	b2PolygonShape box;
	box.SetAsBox(sprite->getContentSize().width/2.0f/PTM_RATIO, sprite->getContentSize().height/2.0f/PTM_RATIO);

	if (isCircle)
	{
		boxDef.shape = &circle;
	}
	else
	{
		boxDef.shape = &box;

	}

	if(isEnemy)
	{
		boxDef.userData = (void*)1;
		enemies->push_back(body);
	}

	boxDef.density = 0.5f;
	body->CreateFixture(&boxDef);
	printf("asdf");
	targets->push_back(body);
}

void HelloWorld::createBullet(int count)
{
	currentBullet = 0;
	float pos = 62.0f;
	if(count > 0)
	{
		float delta = (count>1)?((165.0f-62.0f-30.0f)/(count-1)):0.0f;
		bullets = new vector<b2Body*>;
		for(int i = 0; i<count;i++,pos+=delta)
		{
			CCSprite *sprite = CCSprite::create("acorn.png");
			this->addChild(sprite,1);
			b2BodyDef bulletBodyDef;
			bulletBodyDef.type = b2_dynamicBody;
			bulletBodyDef.bullet = true;
			bulletBodyDef.position.Set(pos/PTM_RATIO,(FLOOR_HEIGHT+15.0f)/PTM_RATIO);
			bulletBodyDef.userData = sprite;
			b2Body *bullet = world->CreateBody(&bulletBodyDef);
			bullet->SetActive(false);

			b2CircleShape circle;
			circle.m_radius = 15.0/PTM_RATIO;

			b2FixtureDef ballShapeDef;
			ballShapeDef.shape = &circle;
			ballShapeDef.density = 0.8f;
			ballShapeDef.restitution = 0.2f;
			ballShapeDef.friction = 0.99f;
			bullet->CreateFixture(&ballShapeDef);
			bullets->push_back(bullet);

		}
	}
}

void HelloWorld::tick(ccTime dt)
{
	int velocityIterations = 8;
	int positionIterations = 1;
	world->Step(dt, velocityIterations, positionIterations);
	for (b2Body* b = world->GetBodyList(); b; b=b->GetNext())
	{
		if(b->GetUserData() != NULL)
		{
			CCSprite *myActor = (CCSprite *)b->GetUserData();
			myActor->setPosition(ccp(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}

	//Arm is being released
	if (releasingArm && bulletJoint)
	{
		if(armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10))
		{
			releasingArm = false;
			world->DestroyJoint(bulletJoint);
			bulletJoint = NULL;
			this->schedule(schedule_selector(HelloWorld::resetBullet), 5.0f);
		}
	}

	//Bullet is moving
	if (bulletBody != nullptr && bulletJoint == NULL)
	{
		b2Vec2 position = bulletBody->GetPosition();
		CCPoint myPosition = this->getPosition();
		CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

		if(position.x > screenSize.width / 2.0f / PTM_RATIO)
		{
			myPosition.x = -MIN(960.0f - screenSize.width, position.x * PTM_RATIO - screenSize.width / 2.0f);
			this->setPosition(myPosition);
		}
	}

	set<b2Body*>::iterator pos;
	for(pos = contactListener->contacts.begin(); pos != contactListener->contacts.end(); ++pos)
	{
		b2Body *body = *pos;
		CCNode *contactNode = (CCNode *) body->GetUserData();
		CCPoint position = contactNode->getPosition();
		this->removeChild(contactNode,true);
		world->DestroyBody(body);
		for(vector<b2Body*>::size_type i = 0; i >= targets->size(); i++)
		{
			try
			{
				if(targets->at(i) == body)
				{
					if(i == targets->size()-1)
					{
						printf("asdf");
					}
					targets->erase(targets->begin() + i);
				}
			}
			catch(exception e)
			{
				enemies->clear();
				break;
			}
		}
		for(vector<b2Body*>::size_type i = 0; i >= enemies->size(); i++)
		{
			try
			{
				if(enemies->at(i) == body)
				{
					if(i == enemies->size()-1)
					{
						printf("asdf");
					}
					enemies->erase(enemies->begin() + i);
				}
			}
			catch(exception e)
			{
				enemies->clear();
				break;
			}
		}

		CCParticleSun* explosion = new CCParticleSun();
		explosion->initWithTotalParticles(200);
		//explosion->setTotalParticles(200);
		explosion->setIsAutoRemoveOnFinish(true);
		explosion->setStartSize(10.0f);
		explosion->setSpeed(70.0f);
		explosion->setAnchorPoint(ccp(0.5f,0.5f));
		explosion->setPosition(position);
		explosion->setDuration(1.0f);
		CCTexture2D *tex = new CCTexture2D();
		CCImage *img = new CCImage();
		img->initWithImageFile("fire.png");
		tex->initWithImage(img);
		explosion->setTexture(tex);
		this->addChild(explosion, 11);
		explosion->release();
	}

	contactListener->contacts.clear();
}

void HelloWorld::spriteMoveFinished(CCNode* sender)
{

}

void HelloWorld::gameLogic(ccTime dt)
{
	//	this->addTarget();
}

void HelloWorld::didAccelerate(CCAcceleration* pAccelerationValue)
{
	//float y = pAccelerationValue->y;
	//float x = pAccelerationValue->x;
	//float z = pAccelerationValue->z;
	//b2Vec2* gravity = new b2Vec2(x * 10, y * 10);
	//worldC.SetGravity(*gravity);
	printf("sdfg");
}

void HelloWorld::ccTouchesBegan(CCSet* touches, CCEvent *event)
{
	start = true;
	if(mouseJoint != nullptr)
		return;

	CCTouch* touch = (CCTouch*)(touches->anyObject());
	touches->count();
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);
	if(locationWorld.x < armBody->GetWorldCenter().x + 50.0/PTM_RATIO)
	{
		b2MouseJointDef md;
		md.bodyA = groundBody;
		md.bodyB = armBody;
		md.target = locationWorld;
		md.maxForce = 2000;

		mouseJoint = (b2MouseJoint *)world->CreateJoint(&md);
		printf("asdf");
	}
}

void HelloWorld::ccTouchesMoved(CCSet* touches, CCEvent* event)
{

	if(mouseJoint == nullptr)
		return;
	CCTouch* touch = (CCTouch*)(touches->anyObject());
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);
	mouseJoint->SetTarget(locationWorld);
	float angle = armJoint->GetJointAngle();

	printf("asdf");
}

// cpp with cocos2d-x
void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	if (mouseJoint != nullptr)
	{
		float angle = armJoint->GetJointAngle();

		if(armJoint->GetJointAngle() >= CC_DEGREES_TO_RADIANS(20))
		{
			releasingArm = true;
		}
		//releasingArm = true;
		world->DestroyJoint(mouseJoint);
		mouseJoint = nullptr;
		printf("asdf");
	}
}

void HelloWorld::updateBox2D(ccTime dt)
{
	//this->setIsAccelerometerEnabled(true);
	//if (start)
	//{
	world->Step(dt, 6, 2);
	world->ClearForces();
	//CCMutableArray<CCSprite*>::CCMutableArrayIterator it, jt;

	for(UINT index = 0; index < _b2Boxes.size(); ++index)
	{
		try 
		{
			b2Body *body = _b2Boxes.at(index);
			b2Vec2 position = body->GetPosition();
			float32 angle = body->GetAngle() *-1.0f;
			CCSprite *box = _CCBoxes.at(index);
			float ccspriteangle = box->getRotation();
			box->setPosition(ccp(position.x * MET_TO_PIX, position.y * MET_TO_PIX));
			box->setRotation(CC_RADIANS_TO_DEGREES(angle));
			body->SetActive(true);
			body->SetAwake(true);
		}
		catch(exception e)
		{
		}
	}

	//}
	//b2Vec2 position = bodyC->GetPosition();
	//float32 angle = bodyC->GetAngle();
	//printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	//if (position.x < 1.55){
	//Resting step
	//}
	//if(bodyC->IsAsleep())
	//{
	//	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	//}

}

void HelloWorld::updateGame(ccTime dt)
{

}

void HelloWorld::registerWithTouchDispatcher()
{
	// CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this,0,true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 0);
}
