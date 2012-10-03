/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2009      Valentin Milea
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

#ifndef __ACTION_CCACTION_MANAGER_H__
#define __ACTION_CCACTION_MANAGER_H__

#include "CCAction.h"
#include "CCMutableArray.h"
#include "CCObject.h"
#include "selector_protocol.h"

namespace cocos2d {

struct _hashElement;
/** 
 @brief CCActionManager is a singleton that manages all the actions.
 Normally you won't need to use this singleton directly. 99% of the cases you will use the CCNode interface,
 which uses this singleton.
 But there are some cases where you might need to use this singleton.
 Examples:
	- When you want to run an action where the target is different from a CCNode. 
	- When you want to pause / resume the actions
 
 @since v0.8
 */
class CC_DLL CCActionManager : public CCObject, public SelectorProtocol
{
public:
	CCActionManager(void);
	~CCActionManager(void);
	bool init(void);

    // actions
    
	/** Adds an action with a target. 
	 If the target is already present, then the action will be added to the existing target.
	 If the target is not present, a new instance of this target will be created either paused or not, and the action will be added to the newly created target.
	 When the target is paused, the queued actions won't be 'ticked'.
	 */
	void addAction(CCAction *pAction, CCNode *pTarget, bool paused);

    /** Removes all actions from all the targets.
    */
	void removeAllActions(void);

    /** Removes all actions from a certain target.
	 All the actions that belongs to the target will be removed.
	 */
	void removeAllActionsFromTarget(CCObject *pTarget);

    /** Removes an action given an action reference.
    */
	void removeAction(CCAction *pAction);

    /** Removes an action given its tag and the target */
	void removeActionByTag(unsigned int tag, CCObject *pTarget);

	/** Gets an action given its tag an a target
	 @return the Action the with the given tag
	 */
	CCAction* getActionByTag(unsigned int tag, CCObject *pTarget);

    /** Returns the numbers of actions that are running in a certain target. 
	 * Composable actions are counted as 1 action. Example:
	 * - If you are running 1 Sequence of 7 actions, it will return 1.
	 * - If you are running 7 Sequences of 2 actions, it will return 7.
	 */
	unsigned int numberOfRunningActionsInTarget(CCObject *pTarget);

    /** Pauses the target: all running actions and newly added actions will be paused.
	*/
	void pauseTarget(CCObject *pTarget);

    /** Resumes the target. All queued actions will be resumed.
	*/
	void resumeTarget(CCObject *pTarget);

	/** purges the shared action manager. It releases the retained instance.
	 * because it uses this, so it can not be static
	 @since v0.99.0
	 */
	void purgeSharedManager(void);
 
public:
	/** returns a shared instance of the CCActionManager */
	static CCActionManager* sharedManager(void);

protected:
	// declared in CCActionManager.m

	void removeActionAtIndex(unsigned int uIndex, struct _hashElement *pElement);
    void deleteHashElement(struct _hashElement *pElement);
	void actionAllocWithHashElement(struct _hashElement *pElement);
	void update(ccTime dt);

protected:
	struct _hashElement	*m_pTargets;
	struct _hashElement	*m_pCurrentTarget;
	bool			m_bCurrentTargetSalvaged;
};

}

#endif // __ACTION_CCACTION_MANAGER_H__
