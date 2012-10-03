/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009-2010 Ricardo Quesada
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

#ifndef __CCPARALLAX_NODE_H__
#define __CCPARALLAX_NODE_H__

#include "CCNode.h"
/*#include "support/data_support/ccArray.h"*/

namespace cocos2d {
	struct _ccArray;

	/** @brief CCParallaxNode: A node that simulates a parallax scroller

	The children will be moved faster / slower than the parent according the the parallax ratio.

	*/
	class CC_DLL CCParallaxNode : public CCNode 
	{
		/** array that holds the offset / ratio of the children */
		CC_SYNTHESIZE(struct _ccArray *, m_pParallaxArray, ParallaxArray)

	public:
		/** Adds a child to the container with a z-order, a parallax ratio and a position offset
		It returns self, so you can chain several addChilds.
		@since v0.8
		*/
		CCParallaxNode();
		virtual ~CCParallaxNode();
		static CCParallaxNode * node();
		virtual void addChild(CCNode * child, unsigned int z, const CCPoint& parallaxRatio, const CCPoint& positionOffset);
		// super methods
		virtual void addChild(CCNode * child, unsigned int zOrder, int tag);
		virtual void removeChild(CCNode* child, bool cleanup);
		virtual void removeAllChildrenWithCleanup(bool cleanup);
		virtual void visit(void);
	private:
		CCPoint absolutePosition();
	protected:
		CCPoint	m_tLastPosition;
	};

} // namespace cocos2d
#endif //__CCPARALLAX_NODE_H__


