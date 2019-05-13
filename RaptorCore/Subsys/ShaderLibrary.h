// ShaderLibrary.h: interface for the CShaderLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
#define AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVertexProgram;
class CFragmentProgram;
class CGeometryProgram;
class CShader;

//!
//!	This class provides a library or shader sources.
//! Theses sources are loaded into vertex or fragment 
//! program objects that can be reused.
//!	Available shaders depend on Raptor releases.
//!
class CShaderLibrary
{
public:
	static CShaderLibrary*	GetInstance(void);

	//!	Destroy the library.
	void destroy();

	//! Initialise default factory shaders.
	//! Depends on Raptor versions.
	//!	Returns true if library is initialized, false
	//!	if failed or already initialized.
	bool glInitFactory(void);

	//!	Returns the default null shader.
	const CShader& getNullShader(void)
	{ return *m_pNullShader; };

	//! Returns les list on shaders names.
	//! Each name refers to a corresponding object
	//! in memory, that can be retrived through
	//! its persistence name.
	void getFactoryShaders(vector<std::string> &);

	//!	Adds a set of shader sources to this library?
	bool glAddToLibrary(const char * const *shader_sources);
	

private:
	/*
	typedef struct PROGRAM_t
	{
		CVertexProgram		*vp;
		CFragmentProgram	*fp;
		CGeometryProgram	*gp;
		RAPTOR_HANDLE	program;
	} PROGRAM;
	*/

	//!	Unavailable instance constructor
	CShaderLibrary();

	//! Unavailable instance destructor;
	virtual ~CShaderLibrary();

	//! The instance.
	static CShaderLibrary *m_pInstance;

	//!	A default shader.
	static CShader *m_pNullShader;
	
	//!	The list of programs
	//vector<PROGRAM>	m_programs;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)

