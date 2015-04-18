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

	//! Returns les list on shaders names.
	//! Each name refers to a corresponding object
	//! in memory, that can be retrived through
	//! its persistence name.
	void getFactoryShaders(vector<std::string> &);

	//!	Registers a program for future usage
	//!	Returns false if program is already registered, true otherwise
	bool registerProgram(CVertexProgram *vp, CFragmentProgram *fp,RAPTOR_HANDLE program);

	//!	Returns a previously registerd program.
	//!	The handle may be invalid if the required combination is
	//!	unknown in program list.
	RAPTOR_HANDLE getRegisteredProgram(CVertexProgram *vp, CFragmentProgram *fp);



private:
	typedef struct PROGRAM_t
	{
		CVertexProgram	*vp;
		CFragmentProgram	*fp;
		RAPTOR_HANDLE	program;
	} PROGRAM;


	//!	Unavailable instance constructor
	CShaderLibrary();

	//! Unavailable instance destructor;
	virtual ~CShaderLibrary();

	//! The instance.
	static CShaderLibrary *m_pInstance;
	
	//!	The list of programs
	vector<PROGRAM>	m_programs;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)

