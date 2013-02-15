/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009      Sindesso Pty Ltd http://www.sindesso.com/
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
#include "CCActionPageTurn3D.h"

namespace cocos2d 
{
	CCPageTurn3D* CCPageTurn3D::actionWithSize(const ccGridSize& gridSize, ccTime time)
	{
		CCPageTurn3D *pAction = new CCPageTurn3D();

		if (pAction)
		{
			if (pAction->initWithSize(gridSize, time))
			{
				pAction->autorelease();
			}
			else
			{
				CC_SAFE_RELEASE_NULL(pAction);
			}
		}

		return pAction;
	}

	/*
	 * Update each tick
	 * Time is the percentage of the way through the duration
	 */
	void CCPageTurn3D::update(ccTime time)
	{
		float tt = MAX(0, time - 0.25f);
		float deltaAy = (tt * tt * 500);
		float ay = -100 - deltaAy;
		
		float deltaTheta = - (float) M_PI_2 * sqrtf( time) ;
		float theta = /*0.01f */ + (float) M_PI_2 +deltaTheta;
		
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);
		
		for (int i = 0; i <= m_sGridSize.x; ++i)
		{
			for (int j = 0; j <= m_sGridSize.y; ++j)
			{
				// Get original vertex
				ccVertex3F p = originalVertex(ccg(i ,j));
				
				float R = sqrtf((p.x * p.x) + ((p.y - ay) * (p.y - ay)));
				float r = R * sinTheta;
				float alpha = asinf( p.x / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );
				
				// If beta > PI then we've wrapped around the cone
				// Reduce the radius to stop these points interfering with others
				if (beta <= M_PI)
				{
					p.x = ( r * sinf(beta));
				}
				else
				{
					// Force X = 0 to stop wrapped
					// points
					p.x = 0;
				}

                p.y = ( R + ay - ( r * (1 - cosBeta) * sinTheta));

                // We scale z here to avoid the animation being
                // too much bigger than the screen due to perspectve transform
                p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7;// "100" didn't work for

				//	Stop z coord from dropping beneath underlying page in a transition
				// issue #751
				if( p.z < 0.5f )
				{
					p.z = 0.5f;
				}
				
				// Set new coords
				setVertex(ccg(i, j), p);
				
			}
		}
	}
} // end of namespace cocos2d
