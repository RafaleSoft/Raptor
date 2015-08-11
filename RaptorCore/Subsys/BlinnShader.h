#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
#define AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CBlinnShader : public CShader
{
public:
	//!	Default constructor.
	CBlinnShader(void);

	//! Destructor.
	virtual ~CBlinnShader(void);

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Implements base class.
	virtual void glRender();

	//! Initialize all shadings, must be called before use.
	void glInit();

private:
	//!	Copy constructor for cloning.
	CBlinnShader(const CBlinnShader& shader);

	static int lightEnable;
	static int diffuseMap;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
