// UnifiedProgram.cpp: implementation of the CUnifiedProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
    #include "UnifiedProgram.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
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
    m_iMaxLocation = -1;
    m_bReLinked = true;
    m_bValid = false;
}


CUnifiedProgram::CUnifiedProgram(const CUnifiedProgram& shader)
	:CShaderProgram(shader)
{
	m_iMaxLocation = shader.m_iMaxLocation;
	m_bReLinked = shader.m_bReLinked;
	m_bValid = shader.m_bValid;
}


CUnifiedProgram::~CUnifiedProgram()
{

}

void CUnifiedProgram::glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v)
{
    if (m_handle.handle == 0)
        return;

    glParameter(numParam,v);
}

void CUnifiedProgram::glProgramParameter(unsigned int numParam,const CColor::RGBA &v)
{
    if (m_handle.handle == 0)
        return;

    glParameter(numParam,v);
}


void CUnifiedProgram::glParameter(unsigned int numParam,const float *v)
{
#if defined(GL_ARB_shader_objects)
	if (numParam < m_parameters.getNbParameters())
    {
		GLint location = m_parameters[numParam].locationIndex;
		if (location == -1)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_NO_RESOURCE);
#endif
		}
		else
        {
            const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
            pExtensions->glUniform4fvARB(location,1,v);
        }
    }
#endif

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
    int param = numParam;
    if (param < m_iMaxLocation)
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
	if (program.handle == 0)
		return 0;

	uint64_t uniform_size = 0;

#if defined(GL_ARB_uniform_buffer_object)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	GLint active_blocks_count = 0;
	GLint active_uniform_max_length = 0;
	pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);
	pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle, i, GL_UNIFORM_BLOCK_DATA_SIZE_ARB, &block_size);

		uniform_size += block_size;

		/*
		GLint active_uniforms = 0;
		pExtensions->glGetActiveUniformBlockivARB(m_shaderProgram.handle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB, &active_uniforms);

		const char* * uniformNames = new const char*[active_uniforms];
		for (GLint j = 0; j < active_uniforms; j++)
		uniformNames[j] = new const char[active_uniform_max_length];
		GLuint* uniformIndices = new GLuint[active_uniforms];
		pExtensions->glGetUniformIndicesARB(m_shaderProgram.handle, active_uniforms, uniformNames, uniformIndices);
		*/
	}
#endif

	return uniform_size;
}

/*

GLuint err = glGetError();
while (GL_NO_ERROR != err)
err = glGetError();

GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(m_shaderProgram.handle, "UniformBufferObject");
GLsizei uniformBlockSize = 0;
pExtensions->glGetActiveUniformBlockivARB(	m_shaderProgram.handle, uniformBlockIndex,
GL_UNIFORM_BLOCK_DATA_SIZE_ARB,
&uniformBlockSize);

GLsizei length = 0;
char uniformBlockName[256];
pExtensions->glGetActiveUniformBlockNameARB(m_shaderProgram.handle, uniformBlockIndex,
256, &length,uniformBlockName);

//pExtensions->glUniformBlockBindingARB(m_shaderProgram.handle, uniformBlockIndex, 0);
err = glGetError();

GLint active_blocks_count = 0;
GLint active_uniform_max_length = 0;
pExtensions->glGetProgramivARB(m_shaderProgram.handle, GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);
err = glGetError();

pExtensions->glGetProgramivARB(m_shaderProgram.handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);
err = glGetError();

const char* const names[1] = { "view" };
GLuint index[1];
pExtensions->glGetUniformIndicesARB(m_shaderProgram.handle, 1, names, index);
GLint offset = -1;
pExtensions->glGetActiveUniformsivARB(m_shaderProgram.handle, 1, index, GL_UNIFORM_OFFSET_ARB, &offset);
GLint singleSize = -1;
pExtensions->glGetActiveUniformsivARB(m_shaderProgram.handle, 1, index, GL_UNIFORM_SIZE_ARB, &singleSize);
GLint singleType = -1;
pExtensions->glGetActiveUniformsivARB(m_shaderProgram.handle, 1, index, GL_UNIFORM_TYPE_ARB, &singleType);

err = glGetError();

uint64_t size = m_pVProgram->glGetBufferMemoryRequirements(m_shaderProgram);
*/

bool CUnifiedProgram::matchKind(unsigned int shaderKind, CProgramParameters::PARAMETER_KIND parameterKind)
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
            return ((CProgramParameters::VECTOR == parameterKind) ||
					(CProgramParameters::ATTRIBUTE == parameterKind));
            break;
        }
        case GL_FLOAT_MAT2_ARB:
        case GL_FLOAT_MAT3_ARB:
        case GL_FLOAT_MAT4_ARB:
        {
            return (CProgramParameters::MATRIX == parameterKind);
            break;
        }
        case GL_SAMPLER_1D_ARB:
        case GL_SAMPLER_2D_ARB:
        case GL_SAMPLER_3D_ARB:
        case GL_SAMPLER_CUBE_ARB:
        case GL_SAMPLER_1D_SHADOW_ARB:
        case GL_SAMPLER_2D_SHADOW_ARB:
        case GL_SAMPLER_2D_RECT_ARB:
        case GL_SAMPLER_2D_RECT_SHADOW_ARB:
        {
            return (CProgramParameters::SAMPLER == parameterKind);
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
    if (program.handle == 0)
        return;

#if defined(GL_ARB_shader_objects)
    // Query the size of uniforms
    GLint attrMaxLength = 0;
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

    pExtensions->glGetObjectParameterivARB(program.handle,GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB,&attrMaxLength);
	//	Predefined uniforms are not taken into account ! (e.g. gl_ModelViewProjectionMatrix)
	//	So, take a bit of space.
	attrMaxLength = MAX(32, attrMaxLength);
    GLcharARB *name = new GLcharARB[attrMaxLength];

    // Query the number of active uniforms
    GLint count = 0;
	pExtensions->glGetObjectParameterivARB(program.handle, GL_OBJECT_ACTIVE_UNIFORMS_ARB,&count);

	// Loop over each of the active uniforms, and set their value
	for (GLint i = 0; i < count; i++)
	{
        GLint size = 0;
        GLenum type = GL_FLOAT_VEC4_ARB;

	    pExtensions->glGetActiveUniformARB(program.handle, i, attrMaxLength, NULL, &size, &type,name);
        if (strlen(name) > 0)
        {
	        GLint location = pExtensions->glGetUniformLocationARB(program.handle, name);

            if (location >= 0) 
            {
                for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
                {
                    CProgramParameters::PROGRAM_PARAMETER_VALUE& pValue = m_parameters[idx];

                    if ((pValue.name == name) && 
                        (pValue.kind != CProgramParameters::ATTRIBUTE) &&
                        matchKind(type,pValue.kind))
                    {
						pValue.locationIndex = location;
						pValue.locationType = type;
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
    if (program.handle == 0)
        return;

#if defined(GL_ARB_shader_objects)
    // Query the size of attributes
    GLint attrMaxLength = 0;
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

    pExtensions->glGetObjectParameterivARB(program.handle,GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB,&attrMaxLength);
	attrMaxLength = MAX(32, attrMaxLength);
    GLcharARB *name = new GLcharARB[attrMaxLength];

    // Query the number of active attributes
    GLint count = 0;
	pExtensions->glGetObjectParameterivARB(program.handle, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB,&count);
    GLint maxAttribs = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&maxAttribs);

    // Loop over each of the active attributes, and set their value
	for (GLint i = 0; i < count; i++)
	{
        GLint size = 0;
        GLenum type = GL_FLOAT_VEC4_ARB;

        pExtensions->glGetActiveAttribARB(program.handle, i, attrMaxLength, NULL, &size, &type,name);
        if (strlen(name) > 0)
        {
            GLint location = pExtensions->glGetAttribLocationARB(program.handle, name);
            if (location >= 0)
            {
                for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
                {
                    CProgramParameters::PROGRAM_PARAMETER_VALUE& pValue = m_parameters[idx];

                    if ((pValue.name == name) && 
                        (pValue.kind == CProgramParameters::ATTRIBUTE) &&
                        matchKind(type,pValue.kind))
                    {
                        // the location retrieved will only be used if the user value is invalid.
						CProgramParameters::GL_VERTEX_ATTRIB userLocation = pValue.attribute;
						pValue.locationType = type;
						if ((userLocation < maxAttribs) && (userLocation != location))
							pValue.locationIndex = userLocation;
						else
							pValue.locationIndex = location;
                    }
                }
            }
        }
    }

    delete [] name;
#endif

    CATCH_GL_ERROR
}



