#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
#define AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CBumpShader : public CShader
{
public:
	CBumpShader(void);
	virtual ~CBumpShader(void);

	//!	Implements base glRender.
	virtual void glRender(void);
	virtual void glStop(void);

	//! Initialize all shadings, must be called before use.
	void glInit();


private:
	static int lightEnable;
	static int diffuseMap;
	static int normalMap;
	static int eyePos;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
