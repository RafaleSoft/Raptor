/***************************************************************************/
/*                                                                         */
/*  GL2DFont.h                                                             */
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


#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
#define AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class FTFace;
class TTBitmapFont;

#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
	#include "GLHierarchy/GLFont.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API CGL2DFont : public CGLFont
{
public:
	CGL2DFont(const std::string& name = "GL2DFont");
	virtual ~CGL2DFont();

	//! initialise the 2D font:
	//! - opens the font file
	//! - configure the font with size.
	bool init(const std::string &name, unsigned int size);

	//!	Computes a 2D font height for the specified text string
	//! @param s : the text for which height is queried
	//! @return the computed height.
	float getHeight(const std::string &s) const;

	//!	Computes a 2D font width for the specified text string
	//! @param s : the text for which width is queried
	//! @return the computed width.
	float getWidth(const std::string &s) const;

	//!	2D bitmap font writer: this method directly draws into the current buffer with bitmaps.
	//!	A 2D font must be initialised.
	//! @param text : the text to write.
	//! @param x : the x horizontal position where writing starts.
	//! @param y : the y vertical position where writing starts.
	//! @param color : a user provided color to write text.
	virtual void glWrite(const std::string &text, 
						 int x, 
						 int y,
						 const CColor::RGBA	&color);

	//! Return underlying bitmap font
	const TTBitmapFont* const getBitmapFont() const { return m_bmfont; };

protected:
	//! Unimplemented.
	//! @see CGLFont
	virtual bool glGenGlyphs(float precision = 1.0f,
							 float extrusion = 0.0f,
							 float scale = 1.0f);

	TTBitmapFont	*m_bmfont;
	FTFace			*m_face;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)