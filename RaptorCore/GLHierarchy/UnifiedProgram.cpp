// UnifiedProgram.cpp: implementation of the CUnifiedProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
    #include "UnifiedProgram.h"
#endif

#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnifiedProgram::CUnifiedProgram(const CPersistence::CPersistenceClassID &classId, const std::string& name):
    CShaderProgram(classId,name)
{
    m_bReLinked = true;
    m_bValid = false;
}


CUnifiedProgram::CUnifiedProgram(const CUnifiedProgram& shader)
	:CShaderProgram(shader)
{
	m_bReLinked = shader.m_bReLinked;
	m_bValid = shader.m_bValid;
}


CUnifiedProgram::~CUnifiedProgram()
{

}

void CUnifiedProgram::glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v) const
{
    if (m_handle.handle() == 0)
        return;

    glParameter(numParam,v);
}

void CUnifiedProgram::glProgramParameter(unsigned int numParam,const CColor::RGBA &v) const
{
    if (m_handle.handle() == 0)
        return;

    glParameter(numParam,v);
}


void CUnifiedProgram::glParameter(unsigned int numParam,const float *v) const
{
#if defined(GL_ARB_shader_objects)
	if (numParam < m_parameters.getNbParameters())
    {
		const CProgramParameters::CParameterBase& param_value = m_parameters[numParam];
		GLint location = param_value.locationIndex;
		if (location == -1)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			RAPTOR_ERROR(	CShaderProgram::CShaderProgramClassID::GetClassId(),
							CRaptorMessages::ID_NO_RESOURCE);
#endif
		}
		else
        {
            const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
            pExtensions->glUniform4fvARB(location,1,v);
        }
    }
#endif

    CATCH_GL_ERROR
}


std::string CUnifiedProgram::glGetProgramString(void)
{
	if (m_handle.handle() == 0)
		return "";

#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	int value = 0;
	pExtensions->glGetObjectParameterivARB(m_handle, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &value);
	char *source = new char[value];
	GLsizei length = 0;
	pExtensions->glGetShaderSourceARB(m_handle, value, &length, source);

	std::string program_source = source;
	delete[] source;
	return program_source;
#else
	return "";
#endif
}

bool CUnifiedProgram::glBindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_shader_objects)
	if (program.handle() == 0)
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glAttachObjectARB(program.handle(), m_handle.handle());

	CATCH_GL_ERROR

	m_bReLinked = true;

	return true;
#else
	return false;
#endif
}

bool CUnifiedProgram::glUnbindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_shader_objects)
	if ((program.handle() == 0) || (m_handle.handle() == 0))
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glDetachObjectARB(program.handle(), m_handle.handle());

	CATCH_GL_ERROR

	return true;
#else
	return false;
#endif
}

void CUnifiedProgram::glRender(void)
{
	if (m_handle.handle() == 0)
		return;

#if defined(GL_ARB_vertex_shader)
	if (m_bReLinked)
	{
		size_t nbParams = m_parameters.getNbParameters();
		for (size_t i = 0; i<nbParams; i++)
		{
			CProgramParameters::CParameterBase& value = m_parameters[i];
			value.locationIndex = -1;
			value.locationType = GL_FLOAT_VEC4_ARB;
		}

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

		if (program != 0)
		{
			glQueryUniformLocations(RAPTOR_HANDLE(0, (void*)program));
			glQueryAttributeLocations(RAPTOR_HANDLE(0, (void*)program));
			//glGetBufferMemoryRequirements(RAPTOR_HANDLE(0, (void*)program));
			m_bReLinked = false;
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			RAPTOR_ERROR(	CShaderProgram::CShaderProgramClassID::GetClassId(),
							CRaptorMessages::ID_WRONG_RENDERING);
		}
#endif
	}

	if (!m_bApplyParameters)
		return;

	CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler = CTextureUnitSetup::IMAGE_UNIT_0;
	GL_COORD_VERTEX vector(0.0f, 0.0f, 0.0f, 0.0f);
	CColor::RGBA color(0.0f, 0.0f, 0.0f, 0.0f);
	GL_MATRIX matrix;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
	{
		const CProgramParameters::CParameterBase& param_value = m_parameters[idx];
		if (param_value.locationIndex >= 0)
		{
			if (param_value.isA(vector))
			{
				vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;
				switch (param_value.locationType)
				{
					case GL_FLOAT:
					{
						pExtensions->glUniform1fvARB(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC2_ARB:
					{
						pExtensions->glUniform2fvARB(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC3_ARB:
					{
						pExtensions->glUniform3fvARB(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC4_ARB:
					{
						pExtensions->glUniform4fvARB(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_INT:
					{
						int val = vector.x;
						pExtensions->glUniform1iARB(param_value.locationIndex, val);
						break;
					}
					case GL_INT_VEC2_ARB:
					{
						int val[2];
						val[0] = vector.x;
						val[1] = vector.y;
						pExtensions->glUniform2iARB(param_value.locationIndex, val[0], val[1]);
						break;
					}
					case GL_INT_VEC3_ARB:
					{
						int val[3];
						val[0] = vector.x;
						val[1] = vector.y;
						val[2] = vector.z;
						pExtensions->glUniform3ivARB(param_value.locationIndex,1, &val[0]);
						break;
					}
					case GL_INT_VEC4_ARB:
					{
						int val[4];
						val[0] = vector.x;
						val[1] = vector.y;
						val[2] = vector.z;
						val[3] = vector.h;
						pExtensions->glUniform4ivARB(param_value.locationIndex, 1, &val[0]);
						break;
					}
				}
			}
			else if (param_value.isA(sampler))
			{
				sampler = ((const CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT>&)param_value).p;
				pExtensions->glUniform1iARB(param_value.locationIndex, sampler);
			}
			else if (param_value.isA(matrix))
			{
				matrix = ((const CProgramParameters::CParameter<GL_MATRIX>&)param_value).p;
				pExtensions->glUniformMatrix4fvARB(param_value.locationIndex, 1, GL_TRUE, matrix);
			}
			else if (param_value.isA(color))
			{
				color = ((const CProgramParameters::CParameter<CColor::RGBA>&)param_value).p;
				pExtensions->glUniform4fvARB(param_value.locationIndex, 1, color);
			}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			else
			{
				RAPTOR_ERROR(	CShaderProgram::CShaderProgramClassID::GetClassId(),
								CRaptorMessages::ID_UPDATE_FAILED);
			}
#endif
		}
	}
#endif

	m_bApplyParameters = false;
	CATCH_GL_ERROR
}

/*
 *	The API is missing interface for doubles.
 *
void CUnifiedProgram::glProgramParameter(unsigned int numParam,GL_HIRES_COORD_VERTEX &v)
{
	if (m_handle.handle == 0)
        return;

#if defined(GL_ARB_shader_objects)
    if (numParam < m_parameters.getNbParameters())
    {
        GLint location = m_locations[numParam].locationIndex;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        if (location == -1)
        {
            Raptor::GetErrorManager()->generateRaptorError(	getId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_NO_RESOURCE);
        }
        else
#endif
        {
            const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
            pExtensions->glUniform4dvARB(location,1,v);
        }
    }
#endif

    CATCH_GL_ERROR
}
*/


uint64_t CUnifiedProgram::glGetBufferMemoryRequirements(RAPTOR_HANDLE program)
{
	if (program.handle() == 0)
		return 0;

	uint64_t uniform_size = 0;

#if defined(GL_ARB_uniform_buffer_object)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GLint active_blocks_count = 0;

	//! Despite the fact that the GL_VERSION text is greater than 3.1, it seems there is a bug in the call
	//! below : the actual value should be returned by glGetProgramiv and not by glGetObjectParameteriv.
	//pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_DATA_SIZE_ARB, &block_size);

		uniform_size += block_size;

		GLint active_uniforms = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB, &active_uniforms);
		GLint indices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES_ARB, &indices[0]);

		GLint active_uniform_max_length = 0;
		//pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);
		pExtensions->glGetObjectParameterivARB(program.handle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);

		GLint active_uniform_length = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_NAME_LENGTH_ARB, &active_uniform_length);

		GLsizei length = 0;
		char uniformBlockName[256];
		pExtensions->glGetActiveUniformBlockNameARB(program.handle(), i, 256, &length, uniformBlockName);

		GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(program.handle(), uniformBlockName);

		GLint binding = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);

		GLint max_bindings = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB, &max_bindings);

		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);
	}
#endif

	return uniform_size;
}


bool isTypeVector(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
    switch (shaderKind)
    {
        case GL_FLOAT:
        case GL_FLOAT_VEC2_ARB:
        case GL_FLOAT_VEC3_ARB:
        case GL_FLOAT_VEC4_ARB:
        case GL_INT:
        case GL_INT_VEC2_ARB:
        case GL_INT_VEC3_ARB:
        case GL_INT_VEC4_ARB:
        {
            return true;
            break;
        }
        default:
        {
            //  boolean values are not handled, they are too far from Raptor float vectors.
            return false;
        }
    }
#else
	return false;
#endif
}

bool isTypeMatrix(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
	switch (shaderKind)
	{
		case GL_FLOAT_MAT2_ARB:
		case GL_FLOAT_MAT3_ARB:
		case GL_FLOAT_MAT4_ARB:
		{
			return true;
			break;
		}
		default:
		{
			//  boolean values are not handled, they are too far from Raptor float vectors.
			return false;
		}
	}
#else
	return false;
#endif
}
bool isTypeSampler(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
	switch (shaderKind)
	{
		case GL_SAMPLER_1D_ARB:
		case GL_SAMPLER_2D_ARB:
		case GL_SAMPLER_3D_ARB:
		case GL_SAMPLER_CUBE_ARB:
		case GL_SAMPLER_1D_SHADOW_ARB:
		case GL_SAMPLER_2D_SHADOW_ARB:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		{
			return true;
			break;
		}
		default:
		{
			//  boolean values are not handled, they are too far from Raptor float vectors.
			return false;
		}
	}
#else
	return false;
#endif
}
void CUnifiedProgram::glQueryUniformLocations(RAPTOR_HANDLE program)
{
    if (program.handle() == 0)
        return;

#if defined(GL_ARB_shader_objects)
    // Query the size of uniforms
    GLint attrMaxLength = 0;
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

    pExtensions->glGetObjectParameterivARB(program.handle(),GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB,&attrMaxLength);
	//	Predefined uniforms are not taken into account ! (e.g. gl_ModelViewProjectionMatrix)
	//	So, take a bit of space.
	attrMaxLength = MAX(32, attrMaxLength);
    GLcharARB *name = new GLcharARB[attrMaxLength];

    // Query the number of active uniforms
    GLint count = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_ACTIVE_UNIFORMS_ARB,&count);

	// Loop over each of the active uniforms, and set their value
	for (GLint i = 0; i < count; i++)
	{
        GLint size = 0;
        GLenum type = GL_FLOAT_VEC4_ARB;

	    pExtensions->glGetActiveUniformARB(program.handle(), i, attrMaxLength, NULL, &size, &type,name);
        if (strlen(name) > 0)
        {
	        GLint location = pExtensions->glGetUniformLocationARB(program.handle(), name);

            if (location >= 0) 
            {
                for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
                {
					CProgramParameters::CParameterBase& value = m_parameters[idx];

					//! TODO : add type checking here
					if (value.name() == name)
                    {
						value.locationIndex = location;
						value.locationType = type;
                    }
                }
            }
        }
    }

    delete [] name;
#endif

    CATCH_GL_ERROR
}

void CUnifiedProgram::glQueryAttributeLocations(RAPTOR_HANDLE program)
{
    if (program.handle() == 0)
        return;

#if defined(GL_ARB_shader_objects)
    // Query the size of attributes
    GLint attrMaxLength = 0;
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

    pExtensions->glGetObjectParameterivARB(program.handle(),GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB,&attrMaxLength);
	attrMaxLength = MAX(32, attrMaxLength);
    GLcharARB *name = new GLcharARB[attrMaxLength];

    // Query the number of active attributes
    GLint count = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_ACTIVE_ATTRIBUTES_ARB,&count);
    GLint maxAttribs = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&maxAttribs);

    // Loop over each of the active attributes, and set their value
	for (GLint i = 0; i < count; i++)
	{
        GLint size = 0;
        GLenum type = GL_FLOAT_VEC4_ARB;

        pExtensions->glGetActiveAttribARB(program.handle(), i, attrMaxLength, NULL, &size, &type,name);
        if (strlen(name) > 0)
        {
            GLint location = pExtensions->glGetAttribLocationARB(program.handle(), name);
            if (location >= 0)
            {
                for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
                {
					CProgramParameters::CParameterBase& value = m_parameters[idx];
					CProgramParameters::GL_VERTEX_ATTRIB userLocation = CProgramParameters::POSITION;
					if ((value.name() == name) && value.isA(userLocation) && isTypeVector(type))
                    {
						userLocation = ((const CProgramParameters::CParameter<CProgramParameters::GL_VERTEX_ATTRIB>&)value).p;

                        // the location retrieved will only be used if the user value is invalid.
						value.locationType = type;
						if ((userLocation < maxAttribs) && (userLocation != location))
							value.locationIndex = userLocation;
						else
							value.locationIndex = location;
                    }
                }
            }
        }
    }

    delete [] name;
#endif

    CATCH_GL_ERROR
}



