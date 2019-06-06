// UnifiedProgram.h: interface for the CUnifiedProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
#define AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
    #include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CUnifiedProgram : public CShaderProgram  
{
public:
	//!	Destructor.
	virtual ~CUnifiedProgram();

	//!	Load OpenGL parameters all at once
	virtual void glRender(void);

    //! Loads a unique parameter immediately
    virtual void glProgramParameter(unsigned int numParam,
									const GL_COORD_VERTEX &v) const;

	//! Same as above but passes a color instead of a geo vector
	virtual void glProgramParameter(unsigned int numParam,
									const CColor::RGBA &v) const;

	//! Implements CShaderProgram.
	virtual std::string glGetProgramString(void);

	//! This method attaches the vertex program to a program object for
	//! linking and validation. Checking is performed on the handle to accept only valid programs.
	//! CShader use this method only when necessary
	//! @return : true if binding is done without errors, false otherwise.
	virtual bool glBindProgram(RAPTOR_HANDLE program);

	//! Symetric as above : detaches the vertex program from the program object, performing checking.
	bool glUnbindProgram(RAPTOR_HANDLE program);

	//! Computes and returns and size required to allocate a uniform
	//! buffer object backing uniform blocks
	//! @return the required size in bytes.
	uint64_t glGetBufferMemoryRequirements(RAPTOR_HANDLE program);


protected:
	//!	Default Constructor.
    CUnifiedProgram(const CPersistence::CPersistenceClassID &classId, 
					const std::string& name);

	//! Copy constructor.
	CUnifiedProgram(const CUnifiedProgram& shader);

    //! Updates locations of uniform variables
    void glQueryUniformLocations(RAPTOR_HANDLE program);

    //! Updates locations of attributes variables
    void glQueryAttributeLocations(RAPTOR_HANDLE program);

    //! Program has been re-linked, perform post processing again
    bool    m_bReLinked;


private:
	//!	Forbidden operators
	CUnifiedProgram& operator=(const CUnifiedProgram&);

    //! factorize uniform settings
    void glParameter( unsigned int numParam,const float *v) const;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)

