// GLFont.h: interface for the CGLFont class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
#define AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CGLFont : public CPersistence
{
public:
	virtual ~CGLFont();


	//! Generates the display lists for 3D & Vector glyphs for a whole charset: glTTBuildGeometry is called
	//!	for each character of the charset with the following parameters:
	//! @param precision : controls the precision of font interpolation of the geometry.
	//! A low precision produces a large geometry that is closer of the ideal character:
	//!	it is a minimum pixel distance allowed between generated geometry and character.
	//! @param extrusion : controls the extrusion of each character 3D model
	//! @param scale : controls the global size of the generated 3D geometries.
	//! @return true is generation succeeded, false otherwise.
	//! Rq: a 3D font must be created with create3D, a vector font with createVector
	virtual bool glGenGlyphs(float precision = 1.0f,
							 float extrusion = 0.0f,
							 float scale = 1.0f );

	//! Returns the glyphs settings used for generation in genGlyphs
	//! @param precision : returns precision
	//! @param extrusion : returns extrusion
	//! @param scale : returns scals
	//! @param glyphset : selects the generated charset for which parameters are queried.
	//! If the charset has not been generated, the parameters are left unchanged and returns false.
	//! @return true if the charset is found and then parameters are returned, false otherwise.
	//! Rq: a 3D font must be created with create3D.
	bool getGlyphSettings(	float &precision,
							float &extrusion,
							float &scale,
							unsigned int glyphset);

	//! Selects the current glyph set for writing
	bool selectCurrentGlyphset(unsigned int glyphset)
	{
		if ((glyphset >= m_glfontglyph.size()) || m_glfontglyph.empty())
			return false;
		m_currentGlyphset = glyphset;
		return true;
	}

	//!	Writes text using glyphs without generating a new display list.
	virtual void glWrite(const std::string &text, int x, int y);

	//!	Computes a 3D font width for the specified text string
	//! @param s : the text for which width is queried
	//! @param numglyphset : the generated charset used ( with genGlyphs ) 
	//! @return the computed width.
	float getWidth(const std::string &s, unsigned int numglyphset) const;


	//!	Computes a 3D font height for the specified text string
	//! @param s : the text for which height is queried
	//! @param numglyphset : the generated charset used ( with genGlyphs ) 
	//! @return the computed height.
	float getHeight(const std::string &s, unsigned int numglyphset) const;


	//	Callbacks
	//	
	//!	this callback is called at every vertex generation
	//!	before issuing a glVertex command
	//!	override to perform custom transformations
	virtual void onVertex(float &,float &,float &) 
	{
	};
	//!	this callback is called at every vertex generation
	//!	before issuing a glTexCoord command
	//!	override to perform custom transformations
	virtual void onTexCoord(float &,float &) 
	{
	};


	//!	Implements CPersistence
	DECLARE_CLASS_ID(CGLFontClassID,"Font",CPersistence)


protected:
	//!	Font construction shall only be called from FontFactory.
	CGLFont(const std::string& name = "GLFont");

	typedef struct glyph_t
	{
		GLuint		glList;
		GLfloat		halfWidth;
		GLfloat		height;
		GLfloat		advance;
	} glyph;

	string	m_fontFileName;
	unsigned int m_currentGlyphset;
	vector<glyph*>	m_glfontglyph;


private:
	//!	Private methods & operatods
	CGLFont(const CGLFont&);
	CGLFont& operator=(const CGLFont&);

	unsigned int	m_size;
	vector<void*>	m_glfontglyphsettings;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)

