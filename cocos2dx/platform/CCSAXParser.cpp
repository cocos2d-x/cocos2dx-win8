/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org  http://cocos2d-x.org
 Copyright (c) 2010 Максим Аксенов
 
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

#include "CCSAXParser.h"
#include "CCMutableDictionary.h"
#include "CCFileUtils.h"
#include "tinyxml\tinyxml.h"


NS_CC_BEGIN;

class CC_DLL XmlSaxHander : public TiXmlVisitor
{
public:
	XmlSaxHander():m_ccsaxParserImp(0){};

	virtual bool VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute );
	virtual bool VisitExit( const TiXmlElement& element );
	virtual bool Visit( const TiXmlText& text );
	virtual bool Visit( const TiXmlUnknown&){ return true; }

	void setCCSAXParserImp(CCSAXParser* parser)
	{
		m_ccsaxParserImp = parser;
	}

private:
	CCSAXParser *m_ccsaxParserImp;
};


bool XmlSaxHander::VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute )
{
	//CCLog(" VisitEnter %s",element.Value());

	std::vector<const char*> attsVector;
	for( const TiXmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
	{
		//CCLog("%s", attrib->Name());
		attsVector.push_back(attrib->Name());
		//CCLog("%s",attrib->Value());
		attsVector.push_back(attrib->Value());
	}
	attsVector.push_back(nullptr);

	CCSAXParser::startElement(m_ccsaxParserImp, (const CC_XML_CHAR *)element.Value(), (const CC_XML_CHAR **)(&attsVector[0]));
	return true;
}
bool XmlSaxHander::VisitExit( const TiXmlElement& element )
{
	//CCLog("VisitExit %s",element.Value());

	CCSAXParser::endElement(m_ccsaxParserImp, (const CC_XML_CHAR *)element.Value());
	return true;
}

bool XmlSaxHander::Visit( const TiXmlText& text )
{
	//CCLog("Visit %s",text.Value());
	CCSAXParser::textHandler(m_ccsaxParserImp, (const CC_XML_CHAR *)text.Value(), text.ValueTStr().size());
	return true;
}


CCSAXParser::CCSAXParser()
{
	m_pDelegator = NULL;

}

CCSAXParser::~CCSAXParser(void)
{
}

bool CCSAXParser::init(const char *pszEncoding)
{
    CC_UNUSED_PARAM(pszEncoding);
	// nothing to do
	return true;
}

bool CCSAXParser::parse(const char *pszFile)
{
	CCFileData data(pszFile, "rt");
	unsigned long size = data.getSize();
	char *pBuffer = (char*) data.getBuffer();
	
	if (!pBuffer)
	{
		return false;
	}
		
	TiXmlDocument tinyDoc;
	tinyDoc.Parse(pBuffer,0,TIXML_ENCODING_UTF8);
	XmlSaxHander printer;
	printer.setCCSAXParserImp(this);
	return tinyDoc.Accept( &printer );	
}

void CCSAXParser::startElement(void *ctx, const CC_XML_CHAR *name, const CC_XML_CHAR **atts)
{
	((CCSAXParser*)(ctx))->m_pDelegator->startElement(ctx, (char*)name, (const char**)atts);
}

void CCSAXParser::endElement(void *ctx, const CC_XML_CHAR *name)
{
	((CCSAXParser*)(ctx))->m_pDelegator->endElement(ctx, (char*)name);
}
void CCSAXParser::textHandler(void *ctx, const CC_XML_CHAR *name, int len)
{
	((CCSAXParser*)(ctx))->m_pDelegator->textHandler(ctx, (char*)name, len);
}
void CCSAXParser::setDelegator(CCSAXDelegator* pDelegator)
{
	m_pDelegator = pDelegator;
}

NS_CC_END;


