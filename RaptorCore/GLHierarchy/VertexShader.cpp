// VertexShader.cpp: implementation of the CVertexShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexShader.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif


RAPTOR_NAMESPACE

bool CVertexShader::m_bVertexReady = false;
static CVertexShader::CVertexShaderClassID vertexId;
const CPersistence::CPersistenceClassID& CVertexShader::CVertexShaderClassID::GetClassId(void)
{
	return vertexId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVertexShader::CVertexShader(const std::string& name)
	:CShaderProgram(vertexId,name)
{
    m_bValid = false;
	m_handle.handle = 0;	// default openGL vertex processing pipeline
	m_handle.hClass = CVertexShader::CVertexShaderClassID::GetClassId().ID();

    glInitShaders();
}

CVertexShader::CVertexShader(const CVertexShader& shader)
	:CShaderProgram(shader)
{
	m_bValid = shader.m_bValid;
}

CVertexShader* CVertexShader::glClone()
{
	return new CVertexShader(*this);
}

CVertexShader::~CVertexShader()
{
#if defined(GL_ARB_vertex_program)
	if (!m_bVertexReady)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CVertexShader::CVertexShaderClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor Vertex shaders are not initialised : no shader were created with a valid GL context");
#endif
	}
	else
	{
        glStop();

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.handle))
			pExtensions->glDeleteProgramsARB(1,&m_handle.handle);
	}
#endif
}

void CVertexShader::glInitShaders()
{
    GLint maxLocals = 96;    //  implementation dependant, but at least 96
    GLint maxMats = 8;

#if defined(GL_ARB_vertex_program)
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
    pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,&maxLocals);
    glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&maxMats);
#endif

	if (!m_bVertexReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_vertex_program"))
		{
#if defined(GL_ARB_vertex_program)
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bVertexReady = (NULL != pExtensions->glIsProgramARB);
#else
			m_bVertexReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = "ASM vertex";
			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(CShaderProgram::CShaderProgramClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   CRaptorMessages::ID_NO_GPU_PROGRAM,
														   args);
#endif
		}
	}
}


//
//	Management of vertex attributes
//

void RAPTOR_FASTCALL CVertexShader::glVertex(const GL_COORD_VERTEX &v)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.handle == 0)
		glVertex4fv(v);
#if defined(GL_ARB_vertex_program)
	else if (m_bVertexReady)
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::POSITION, v);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glVertexWeightEXT(float w)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib1fARB(CProgramParameters::WEIGHTS, w);
	}
#endif
	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glNormal(const GL_COORD_VERTEX &n)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.handle == 0)
		glNormal3fv(n);
#if defined(GL_ARB_vertex_program)
	else if (m_bVertexReady)
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::NORMAL, n);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glColor(const CColor::RGBA &c)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.handle == 0)
		glColor4fv(c);
#if defined(GL_ARB_vertex_program)
	else if (m_bVertexReady)
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::PRIMARY_COLOR, c);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glSecondaryColorEXT(const CColor::RGBA &c)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::SECONDARY_COLOR, c);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glFogCoordEXT(float f)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib1fARB(CProgramParameters::FOG_COORDINATE, f);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glMultiTexCoord(CProgramParameters::GL_VERTEX_ATTRIB tmu, const GL_COORD_VERTEX &t)
{
	// Assert the tmu is valid
	if (tmu < CProgramParameters::TEXCOORD0)
		return;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (m_handle.handle == 0)
	{
#if defined(GL_ARB_multitexture)
		pExtensions->glMultiTexCoord4fvARB(tmu - CProgramParameters::TEXCOORD0 + GL_TEXTURE0_ARB, t);
#else
		glTexCoord4fv(t);
#endif
	}
#if defined(GL_ARB_vertex_program)
	else if (m_bVertexReady)
		pExtensions->glVertexAttrib4fvARB(tmu,t);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glAdditionalVertexParam(const GL_COORD_VERTEX &v)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::ADDITIONAL_PARAM1, v);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glAdditionalVertexParam2(const GL_COORD_VERTEX &v)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::ADDITIONAL_PARAM2, v);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glPointParameter(float minSize,float maxSize,const GL_COORD_VERTEX& attenuation)
{
#if defined(GL_EXT_point_parameters)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (minSize >= 0.0)
		pExtensions->glPointParameterfEXT(GL_POINT_SIZE_MIN_EXT,minSize);
	if (maxSize >= 0.0)
		pExtensions->glPointParameterfEXT(GL_POINT_SIZE_MAX_EXT,maxSize);
	
	pExtensions->glPointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT,attenuation);
#endif

	CATCH_GL_ERROR
}


//
//	Direct access to program parameters
//
void RAPTOR_FASTCALL CVertexShader::glVertex(const GL_VERTEX_DATA &v)
{
	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexShader::glVertexPointer(CProgramParameters::GL_VERTEX_ATTRIB numParam, GLint size, GLenum type, GLsizei stride, const void* pointer)
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
        if (pointer != NULL)
        {
		    pExtensions->glEnableVertexAttribArrayARB(numParam);
		    pExtensions->glVertexAttribPointerARB(numParam,size,type,false,stride,pointer);
        }
        else
            pExtensions->glDisableVertexAttribArrayARB(numParam);
	}
#endif

	CATCH_GL_ERROR
}

void CVertexShader::glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v) const
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CVertexShader::glProgramParameter(unsigned int numParam,const CColor::RGBA &v) const
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CVertexShader::glProgramParameter(unsigned int numParam,const CGenericVector<float> &v) const
{
#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v.vector());
	}
#endif

	CATCH_GL_ERROR
}




//
//	Vertex Shader management
//
void CVertexShader::glRender(void)
{
	if (m_handle.handle == 0)
		return;

#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.handle))
		{
			glEnable(GL_VERTEX_PROGRAM_ARB);

			pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,m_handle.handle);

			if (m_bApplyParameters)
			{
				for (unsigned int i=0;i<m_parameters.getNbParameters();i++)
				{
					const CProgramParameters::CParameterBase& param_value = m_parameters[i];
					const GL_COORD_VERTEX &vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;
					pExtensions->glProgramLocalParameter4fvARB(	GL_VERTEX_PROGRAM_ARB, i, vector);
				}
				m_bApplyParameters = false;
			}
		}
	}
#endif

	CATCH_GL_ERROR
}

void CVertexShader::glStop(void)
{
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
	glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

	CATCH_GL_ERROR
}

bool CVertexShader::glLoadProgram(const std::string &program)
{
	m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_vertex_program)
	if (m_bVertexReady)
	{
		if (pExtensions->glIsProgramARB(m_handle.handle))
			pExtensions->glDeleteProgramsARB(1,&m_handle.handle);
	}
#endif

	// default openGL vertex processing pipeline
	string ogl = "opengl";
    for (unsigned int i=0;i<program.size()&&i<6;i++)
	    ogl[i] = toupper(program[i]);

	if (ogl == "OPENGL")
	{
		m_handle.handle = 0;
        m_bValid = true;
	}
#if defined(GL_ARB_vertex_program)
	else if (m_bVertexReady)
	{
        //!    In case of a previous error, we need to initialize error checking
        //!    to be sure that the error detected will only be due to shader loading.
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CVertexShader::CVertexShaderClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Raptor encountered errors before loading vertex shader, check with debug infos.");
            while (err != GL_NO_ERROR)
                err = ::glGetError();
        }

		pExtensions->glGenProgramsARB(1,&m_handle.handle);
		pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,m_handle.handle);

		pExtensions->glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
										GL_PROGRAM_FORMAT_ASCII_ARB,
										program.size(),
										(void*)(program.data()));
		err = glGetError();
		
		if (err != GL_NO_ERROR)// && (err == GL_INVALID_OPERATION))
		{
			int pos = 0;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&pos);
			const unsigned char * str = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

            vector<CRaptorMessages::MessageArgument> args;
            CRaptorMessages::MessageArgument arg1;
            arg1.arg_int = pos;
            args.push_back(arg1);
            CRaptorMessages::MessageArgument arg2;
            arg2.arg_sz = (const char*)str;
            args.push_back(arg2);

            Raptor::GetErrorManager()->generateRaptorError(	CVertexShader::CVertexShaderClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
											                CRaptorMessages::ID_PROGRAM_ERROR,args); 
		}

		m_bValid = ((err == GL_NO_ERROR) && glGetProgramStatus());
		if (!m_bValid)
		{
			Raptor::GetErrorManager()->generateRaptorError(CVertexShader::CVertexShaderClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   CRaptorMessages::ID_NO_GPU_PROGRAM);
			Raptor::GetErrorManager()->generateRaptorError(CVertexShader::CVertexShaderClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   getName().data());
		}
		
		// Unbind program to avoid side effects
		glStop();
	}
#endif

	CATCH_GL_ERROR

	return m_bValid;
}

bool CVertexShader::glGetProgramStatus(void)
{
	if (m_handle.handle == 0)
		return false;

	if (!m_bVertexReady)
		return false;

#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (pExtensions->glIsProgramARB(m_handle.handle))
		pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,m_handle.handle);
	else
		return false;

	int value = 0;
	pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &value);
	if (value < 1)
	{
		GL_VERTEX_SHADER_CAPS caps;
		if (glGetProgramCaps(caps))
		{
			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB, &value);
			if (value > caps.max_instructions)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &value);
			if (value > caps.max_native_instructions)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB, &value);
			if (value > caps.max_temporaries)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &value);
			if (value > caps.max_native_temporaries)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB, &value);
			if (value > caps.max_parameters)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &value);
			if (value > caps.max_native_parameters)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ATTRIBS_ARB, &value);
			if (value > caps.max_attribs)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &value);
			if (value > caps.max_native_attribs)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ADDRESS_REGISTERS_ARB, &value);
			if (value > caps.max_address_registers)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &value);
			if (value > caps.max_native_address_registers)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &value);
			if (value < 1)
				return false;
			return true;
		}
		else
			return false;
	}
	else
#endif
		return true;
}

bool CVertexShader::glGetProgramCaps(GL_VERTEX_SHADER_CAPS& caps)
{
	if (m_bVertexReady)
	{
#if defined(GL_ARB_vertex_program)
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_INSTRUCTIONS_ARB,&caps.max_instructions);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,&caps.max_native_instructions);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_TEMPORARIES_ARB,&caps.max_temporaries);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,&caps.max_native_temporaries);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_PARAMETERS_ARB,&caps.max_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,&caps.max_local_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,&caps.max_env_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,&caps.max_native_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ATTRIBS_ARB,&caps.max_attribs);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,&caps.max_native_attribs);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,&caps.max_address_registers);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,&caps.max_native_address_registers);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&caps.max_vertex_attribs);
		glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&caps.max_program_matrices);
		glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,&caps.max_program_matrix_stack_depth);
        GLboolean bVal = GL_FALSE;
        glGetBooleanv(GL_VERTEX_PROGRAM_POINT_SIZE_ARB,&bVal);
        caps.vertex_program_point_size = (bVal == GL_TRUE);
        glGetBooleanv(GL_VERTEX_PROGRAM_TWO_SIDE_ARB,&bVal);
        caps.vertex_program_two_side = (bVal == GL_TRUE);

		return true;
#else
		return false;
#endif
	}
	else
		return false;
}

