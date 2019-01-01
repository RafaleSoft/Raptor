#if !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
#define AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE_BEGIN



class CPhongShader : public CShader
{
public:
	//!	Default constructor.
	CPhongShader(void);

	//! Destructor.
	virtual ~CPhongShader(void);

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Implements base glRender.
	virtual void glRender(void);

	//! Initialize all shadings, must be called before use.
	void glInit();


private:
	//!	Copy constructor for cloning.
	CPhongShader(const CPhongShader& shader);

	static int diffuseMap;
	static int lightEnable;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
