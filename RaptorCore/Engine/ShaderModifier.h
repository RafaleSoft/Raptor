// ShaderModifier.h: interface for the CShaderModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERMODIFIER_H__5ED408B3_1FAE_4869_8A40_6ABF39D8DF1E__INCLUDED_)
#define AFX_SHADERMODIFIER_H__5ED408B3_1FAE_4869_8A40_6ABF39D8DF1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CObject3D;
class CShader;
class CShaderWrapper;

class RAPTOR_API CShaderModifier : public CModifier
{
public:
	CShaderModifier(TIME_FUNCTION_TYPE timeFunction,
		float			timeArg0,
		float			timeArg1,
		float			timeArg2,
		float			timeArg3,
		USER_FUNCTION   pUserFunction,
		const CPersistence::CPersistenceClassID& id,
		const std::string& name = "GL_SHADERMODIFIER");
	virtual ~CShaderModifier();

	//!	Base implementation
	virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_SHADER_MODIFIER; };

	//!	Returns the Geometry object updated by the modifier
	void setShader(CShader *shader);

	//!	Returns the Shader object updated by the modifier
	CShader *getObject() const;


protected:
	//!	Implements CPersistence
	virtual void unLink(const CPersistence*);


private:
	//!	Handler to be overloaded to declare an update
	//!	of the managed vertex program/shader
	virtual void RAPTOR_FASTCALL updateVertexParameters(float dt, CProgramParameters &v);

	//!	Handler to be overloaded to declare an update
	//!	of the managed fragment program/shader
	virtual void RAPTOR_FASTCALL updateFragmentParameters(float dt, CProgramParameters &v);

	//!	Handler to be overloaded to declare an update
	//!	of the managed geometry program/shader
	virtual void RAPTOR_FASTCALL updateGeometryParameters(float dt, CProgramParameters &v);

	//!	Compute physics resultant after dt time increment
	//!	This method should not be called directly.
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

	//!	Helper to clean all used resources.
	void RAPTOR_FASTCALL cleanWrapper();

	//!	A geometry wrapper to intercept rendering
	//! and perform a geometry update in geometry buffers
	CShaderWrapper	*m_pWrapper;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERMODIFIER_H__5ED408B3_1FAE_4869_8A40_6ABF39D8DF1E__INCLUDED_)

