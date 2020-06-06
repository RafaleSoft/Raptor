/***************************************************************************/
/*                                                                         */
/*  GLVectorFont.h                                                         */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
	//! Generates the display lists for vector glyphs for a whole charset.
	//!	Vector fonts genetares glyphs only for caracters in the range [32 .. 126]
	//! @see CGLFont
	virtual bool glGenGlyphs(float precision = 1.0f,
							 float extrusion = 0.0f,
							 float scale = 1.0f);

	//!	Write a single line of text at position (x,y), origin is at bottom-left corner of window.
	//! @see CGLFont
	virtual void glWrite(	const std::string &text,
							int x,
							int y,
							const CColor::RGBA	&color);

	//!	Write a list of lines, origin is at bottom-left corner of window.
	//! @see CGLFont
	virtual void glWrite(const std::vector<CGLFont::FONT_TEXT_ITEM> &lines);


private:
	//!	This method generates a display list independantly for a vector font.
	//!	It is called by genGlyphs sequentially for each char of the font.
	//! @see genGlyphs for parameters.
	RAPTOR_HANDLE glBuildVectors(const std::string &str,
								 float scale = 1.0f,
								 float *width = NULL,
								 float *height = NULL);

	//!	Vertex Input State Resource binder.
	void	*m_pBinder;

	//!	Uniform buffer data for font rendering:
	//!	- Uniform Buffer pointer.
	uint8_t	*m_fontUniform;
	//!	- Size of this uniform.
	size_t m_fontUniformSize;
};

RAPTOR_NAMESPACE_END

#endif //! !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
