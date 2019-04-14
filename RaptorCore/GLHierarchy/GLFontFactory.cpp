// GLFontFactory.cpp: implementation of the CGLFontFactory class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLFONTFACTORY_H__E18CD490_4CB0_4ECA_916D_85B155FF04C3__INCLUDED_)
	#include "GLFontFactory.h"
#endif
#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
	#include "GLHierarchy/GL2DFont.h"
#endif
#if !defined(AFX_GL3DFONT_H__81553B3D_92A7_4C82_80E7_B6F98CBD75FA__INCLUDED_)
	#include "GLHierarchy/GL3DFont.h"
#endif
#if !defined(AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_)
	#include "GLHierarchy/GL2DTextureFont.h"
#endif
#if !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
	#include "GLHierarchy/GLVectorFont.h"
#endif


RAPTOR_NAMESPACE

CGL2DFont* CGLFontFactory::create2DFont(const std::string& filename,
										unsigned int size,
										const std::string& name)
{
	CGL2DFont *font = new CGL2DFont(name);
	if (NULL != font)
	{
		if (font->init(filename, size))
			return font;
		else
		{
			delete font;
			font = NULL;
		}
	}

	return font;
}

CGL3DFont* CGLFontFactory::create3DFont(const std::string& filename,
										unsigned int size,
										bool faceTextured,
										bool sideTextured,
										const std::string& name)
{
	CGL3DFont *font = new CGL3DFont(name);
	if (NULL != font)
	{
		if (font->init(filename, size, faceTextured, sideTextured))
			return font;
		else
		{
			delete font;
			font = NULL;
		}
	}

	return font;
}

CGLVectorFont* CGLFontFactory::createVectorFont(const std::string &filename,
												unsigned int size,
												const std::string& name)
{
	CGLVectorFont *font = new CGLVectorFont(name);
	return font;
}

CGL2DTextureFont* CGLFontFactory::glCreateTextureFont(const std::string &filename,
													  unsigned int size,
													  bool antialiased,
													  const std::string& name)
{
	CGL2DTextureFont *font = new CGL2DTextureFont(name);
	if (NULL != font)
	{
		if (font->glInit(filename, size, antialiased))
			return font;
		else
		{
			delete font;
			font = NULL;
		}
	}

	return font;
}
