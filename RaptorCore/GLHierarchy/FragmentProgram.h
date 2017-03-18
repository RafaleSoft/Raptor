// FragmentProgram.h: interface for the CFragmentProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
#define AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
	#include "UnifiedProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//!
//! This class handles OpenGL2.0 fragment shaders. It is conceived to handle a program
//! rather than being only a shader because OGL2 shading language relates more to the concept
//! of a program with variables, compilation, linking and so on.
//! CAUTION : this class embbeds a fragment shader extension object, while CFragmentShader holds
//! an assembly program only.
class RAPTOR_API CFragmentProgram : public CUnifiedProgram  
{
public:
    typedef struct GL_FRAGMENT_PROGRAM_CAPS_t
	{
        int		max_fragment_uniform_components;
        int		max_texture_coords;
		int		max_texture_image_units;
		int		max_fragment_uniform_blocks;
	} GL_FRAGMENT_PROGRAM_CAPS;

public:
	//!	Default Constructor.
	CFragmentProgram(const std::string& name="FRAGMENT_PROGRAM");

	//!	Destructor.
	virtual ~CFragmentProgram();

	//!	Clone this shader.
	virtual CFragmentProgram* glClone();

    //! Loads an OpenGL 2.0 vertex shader program.
	bool glLoadProgram(const std::string &program);

    //! Returns current status of this fragment program
    //!	@see CShaderProgram.
	virtual bool glGetProgramStatus(void);

	//! This method returns the underlying hardware capabilities to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information on each capability item.
	static bool glGetProgramCaps(GL_FRAGMENT_PROGRAM_CAPS& caps);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CFragmentProgramClassID,"FragmentProgram",CShaderProgram)
	

private:
	//!	Forbidden operators
	CFragmentProgram& operator=(const CFragmentProgram&);

	//! Copy constructor.
	CFragmentProgram(const CFragmentProgram& shader);

    //! Specific init of shader parameters
    virtual void	glInitShaders();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)

