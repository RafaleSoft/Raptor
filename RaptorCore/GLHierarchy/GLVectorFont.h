// GLVectorFont.h: interface for the CGLVectorFont class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
#define AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
#include "GLHierarchy/GLFont.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API CGLVectorFont : public CGLFont
{
public:
	CGLVectorFont(const std::string& name = "GLVectorFont");
	virtual ~CGLVectorFont();


protected:
	//! Generates the display lists for vector glyphs for a whole charset
	//! @see CGLFont
	virtual bool glGenGlyphs(float precision = 1.0f,
							 float extrusion = 0.0f,
							 float scale = 1.0f);

private:
	//!	This method generates a display list independantly for a vector font.
	//!	It is called by genGlyphs sequentially for each char of the font.
	//! @see genGlyphs for parameters.
	RAPTOR_HANDLE glBuildVectors(const std::string &str,
								 float scale = 1.0f,
								 float *width = NULL,
								 float *height = NULL);

};

RAPTOR_NAMESPACE_END

#endif //! !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
