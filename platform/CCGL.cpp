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

#include "CCGL.h"
#include "CCStdC.h"

NS_CC_BEGIN;
/*
void gluPerspective(CCfloat fovy, CCfloat aspect, CCfloat zNear, CCfloat zFar)
{	
    CCfloat xmin, xmax, ymin, ymax;

    ymax = zNear * (CCfloat)tanf(fovy * (float)M_PI / 360);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;



    glFrustumf(xmin, xmax,
        ymin, ymax,
        zNear, zFar);	
}

void gluLookAt(float fEyeX, float fEyeY, float fEyeZ,
    float fCenterX, float fCenterY, float fCenterZ,
    float fUpX, float fUpY, float fUpZ)
{
    CCfloat m[16];
    CCfloat x[3], y[3], z[3];
    CCfloat mag;

    //* Make rotation matrix *

    //* Z vector *
    z[0] = fEyeX - fCenterX;
    z[1] = fEyeY - fCenterY;
    z[2] = fEyeZ - fCenterZ;
    mag = (float)sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }

    //* Y vector *
    y[0] = fUpX;
    y[1] = fUpY;
    y[2] = fUpZ;

    //* X vector = Y cross Z *
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    //* Recompute Y = Z cross X *
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    //* cross product gives area of parallelogram, which is < 1.0 for
    //* non-perpendicular unit-length vectors; so normalize x, y here


    mag = (float)sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }

    mag = (float)sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }

#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0f;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0f;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0f;
    M(3, 0) = 0.0f;
    M(3, 1) = 0.0f;
    M(3, 2) = 0.0f;
    M(3, 3) = 1.0f;
#undef M
    {
        int a;
        CCfloat fixedM[16];
        for (a = 0; a < 16; ++a)
            fixedM[a] = m[a];
        glMultMatrixf(fixedM);
    }

    //* Translate Eye to Origin *
    glTranslatef(-fEyeX, -fEyeY, -fEyeZ);
}
*/
NS_CC_END;
