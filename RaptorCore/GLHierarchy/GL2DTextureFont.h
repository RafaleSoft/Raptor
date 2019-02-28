// GL2DFont.h: interface for the CGL2DFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_)
#define AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
	#include "GLHierarchy/GL2DFont.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class RAPTOR_API CGL2DTextureFont : public CGL2DFont
{
public:
	CGL2DTextureFont(const std::string& name = "GL2DTextureFont");
	virtual ~CGL2DTextureFont();

	//! Creates a texture font model. A 2D font is first generated by calling create2D
	//! @param filename : the filename of the font to open. If empty, the SIMPLEX subset of Hershey font is used.
	//!	@param size : the size of the glyphs generated.
	//! @param antialias : turn on/off font antialiasing (alpha based for correct superposition with frame content)
	//! @return true if the whole creation process succeeded, false otherwise
	bool glInit(const std::string &filename, unsigned int size, bool antialiased);

	//!	2D font writer: this method directly draws into the current buffer with textures.
	//! @param text : the text to write
	//! @param x : the x horizontal position where writing starts
	//! @param y : the y vertical position where writing starts
	//! Rq: a texture font must be initialised.
	virtual void glWrite(const std::string &text, int x, int y);


protected:
	//! Generates the display lists for vector glyphs for a whole charset
	//! @see CGLFont
	virtual bool glGenGlyphs(float precision = 1.0f,
							 float extrusion = 0.0f,
							 float scale = 1.0f);

private:
	bool			m_bAntialiased;
	int				m_char_w;
	int				m_char_h;
	CTextureObject	*m_texture;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_)
