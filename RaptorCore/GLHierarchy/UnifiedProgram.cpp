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
	if (numParam < m_locations.size())
    {
        GLint location = m_locations[numParam].locationIndex;
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
                        m_locations[idx].locationIndex = location;
                        m_locations[idx].locationType = type;
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
                        GL_VERTEX_ATTRIB userLocation = pValue.attribute;
                        m_locations[idx].locationType = type;
                        if ((userLocation < maxAttribs) && (userLocation != location))
                            m_locations[idx].locationIndex = userLocation;
                        else
                            m_locations[idx].locationIndex = location;
                    }
                }
            }
        }
    }

    delete [] name;
#endif

    CATCH_GL_ERROR
}



