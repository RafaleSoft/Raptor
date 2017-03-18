// ShaderProgram.cpp: implementation of the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "ShaderProgram.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

bool	CShaderProgram::m_bVertexReady = false;
bool	CShaderProgram::m_bFragmentReady = false;
bool	CShaderProgram::m_bVertexProgramReady = false;
bool	CShaderProgram::m_bFragmentProgramReady = false;
bool	CShaderProgram::m_bGeometryProgramReady = false;

static CShaderProgram::CShaderProgramClassID shaderId;
const CPersistence::CPersistenceClassID& CShaderProgram::CShaderProgramClassID::GetClassId(void)
{
	return shaderId;
}

RAPTOR_NAMESPACE_END

//////////////////////////////////////////////////////////////////////
// Static datas
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShaderProgram::CShaderProgram(const CPersistence::CPersistenceClassID& id,const std::string& name):
	CPersistence(id,name),
	m_bValid(false),
	m_handle(),
	m_bApplyParameters(false),
	m_parameters()
{
}

CShaderProgram::CShaderProgram(const CShaderProgram& shader)
    :CPersistence(shaderId,shader.getName())
{
	m_bValid = shader.m_bValid;
	m_handle = shader.m_handle;
	m_bApplyParameters = shader.m_bApplyParameters;
	m_parameters = shader.m_parameters;
}

CShaderProgram::~CShaderProgram()
{
	// TODO : Recycle handle
}


void CShaderProgram::glInitShaders()
{
	if (!m_bVertexReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_vertex_program"))
		{
#if defined(GL_ARB_vertex_program)
			const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bVertexReady = pExtensions->glIsProgramARB != NULL;
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
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}

    if (!m_bVertexProgramReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_vertex_shader"))
		{
#if defined(GL_ARB_vertex_shader)
			const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bVertexProgramReady = pExtensions->glCreateShaderObjectARB != NULL;
#else
			m_bVertexProgramReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
            arg.arg_sz = "GLSL vertex";
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}

	if (!m_bFragmentReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_fragment_program"))
		{
#if defined(GL_ARB_fragment_program)
			const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bFragmentReady = pExtensions->glIsProgramARB != NULL;
#else
			m_bFragmentReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
            arg.arg_sz = "ASM fragment";
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}

    if (!m_bFragmentProgramReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_fragment_shader"))
		{
#if defined(GL_ARB_fragment_shader)
			const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bFragmentProgramReady = pExtensions->glCreateShaderObjectARB != NULL;
#else
			m_bFragmentProgramReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
            arg.arg_sz = "GLSL fragment";
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}

	if (!m_bGeometryProgramReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_geometry_shader4"))
		{
#if defined(GL_ARB_geometry_shader4)
			const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bGeometryProgramReady = pExtensions->glCreateShaderObjectARB != NULL;
#else
			m_bGeometryProgramReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
            arg.arg_sz = "GLSL geometry";
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}
}


void CShaderProgram::setProgramParameters(const CProgramParameters &v)
{
    m_parameters = v;
    m_bApplyParameters = true;
}

bool CShaderProgram::glLoadProgramFromFile(const std::string &program)
{
	CRaptorIO *shdr = CRaptorIO::Create(program, CRaptorIO::DISK_READ);
	if (NULL == shdr)
		return false;

	if (shdr->getStatus() == CRaptorIO::IO_OK)
	{
		string programstr;
		while (shdr->getStatus() == CRaptorIO::IO_OK)
		{
			string line;
			*shdr >> line;
			if (shdr->getStatus() == CRaptorIO::IO_OK)
				programstr = programstr + line + "\n";
		}

		return glLoadProgram(programstr);
	}
	else
		return false;
}

bool CShaderProgram::exportObject(CRaptorIO& o)
{
	return true;
}

bool CShaderProgram::importObject(CRaptorIO& i)
{
	return true;
}

