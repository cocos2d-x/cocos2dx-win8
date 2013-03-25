/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

// For licensing information relating to this distribution please see Third Party Notices file.

#ifndef __CCLABEL_H__
#define __CCLABEL_H__
#include "CCSprite.h"
#include "CCTexture2D.h"

NS_CC_BEGIN

	/** @brief CCLabelTTF is a subclass of CCTextureNode that knows how to render text labels
	*
	* All features from CCTextureNode are valid in CCLabelTTF
	*
	* CCLabelTTF objects are slow. Consider using CCLabelAtlas or CCLabelBMFont instead.
	*/
class CC_DLL CCLabelTTF : public CCSprite, public CCLabelProtocol
{
public:
	CCLabelTTF();
	virtual ~CCLabelTTF();
	char * description();
	/** Creates an label.
     */
    static CCLabelTTF * create();
    /** creates a CCLabelTTF with a font name and font size in points
     @since v2.0.1
     */
    static CCLabelTTF * create(const char *string, const char *fontName, float fontSize);
    
    /** creates a CCLabelTTF from a fontname, horizontal alignment, dimension in points,  and font size in points.
     @since v2.0.1
     */
    static CCLabelTTF * create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment);
  
    /** creates a CCLabel from a fontname, alignment, dimension in points and font size in points
     @since v2.0.1
     */
    static CCLabelTTF * create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment, 
                               CCVerticalTextAlignment vAlignment);
	///** initializes the CCLabelTTF with a font name, alignment, dimension and font size */
	bool initWithString(const char *label, const CCSize& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize);
	///** initializes the CCLabelTTF with a font name and font size */
	//bool initWithString(const char *label, const char *fontName, float fontSize);
	/** initializes the CCLabelTTF with a font name and font size */
	bool initWithString(const char *string, const char *fontName, float fontSize);

	/** initializes the CCLabelTTF with a font name, alignment, dimension and font size */
	bool initWithString(const char *string, const char *fontName, float fontSize,
		const CCSize& dimensions, CCTextAlignment hAlignment);

	/** initializes the CCLabelTTF with a font name, alignment, dimension and font size */
	bool initWithString(const char *string, const char *fontName, float fontSize,
		const CCSize& dimensions, CCTextAlignment hAlignment, 
		CCVerticalTextAlignment vAlignment);

	/** changes the string to render
	* @warning Changing the string is as expensive as creating a new CCLabelTTF. To obtain better performance use CCLabelAtlas
	*/
	virtual void setString(const char *label);
	virtual const char* getString(void);

	virtual CCLabelProtocol* convertToLabelProtocol() { return (CCLabelProtocol*)this; }
protected:
	CCSize m_tDimensions;
	CCTextAlignment m_eAlignment;
	std::string * m_pFontName;
	float m_fFontSize;
	std::string * m_pString;
};

NS_CC_END
#endif //__CCLABEL_H__

