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

static CShaderProgram::CProgramParameters::PROGRAM_PARAMETER_VALUE noValue;

CShaderProgram::CProgramParameters::PROGRAM_PARAMETER_VALUE& 
	CShaderProgram::CProgramParameters::operator[](unsigned int v) 
{
	if (mValues.size() > v)
		return mValues[v]; 
	else
	{
		noValue.name = "";
		noValue.kind = VECTOR;
		noValue.locationIndex = -1;
		noValue.locationType = GL_FLOAT_VEC4_ARB;
		noValue.vector = GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f);
		IDENT_MATRIX(noValue.matrix);
		noValue.attribute = POSITION;
		noValue.sampler = CTextureUnitSetup::IMAGE_UNIT_0;
		return noValue;
	}
}

const CShaderProgram::CProgramParameters::PROGRAM_PARAMETER_VALUE&
CShaderProgram::CProgramParameters::operator[](unsigned int v) const
{
	if (mValues.size() > v)
		return mValues[v];
	else
	{
		noValue.name = "";
		noValue.kind = VECTOR;
		noValue.locationIndex = -1;
		noValue.locationType = GL_FLOAT_VEC4_ARB;
		noValue.vector = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f);
		IDENT_MATRIX(noValue.matrix);
		noValue.attribute = POSITION;
		noValue.sampler = CTextureUnitSetup::IMAGE_UNIT_0;
		return noValue;
	}
}

bool CShaderProgram::CProgramParameters::addParameter(const std::string& name, const GL_COORD_VERTEX& vertex)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = VECTOR;
	value.vector = vertex;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CShaderProgram::CProgramParameters::addParameter(const std::string& name, const GL_MATRIX& matrix)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = MATRIX;
	value.matrix = matrix;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CShaderProgram::CProgramParameters::addParameter(const std::string& name, GL_VERTEX_ATTRIB attribute)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = ATTRIBUTE;
	value.attribute = attribute;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CShaderProgram::CProgramParameters::addParameter(const std::string& name, CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = SAMPLER;
	value.sampler = sampler;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

CShaderProgram::CProgramParameters& CShaderProgram::CProgramParameters::operator=(const CProgramParameters& params)
{
	// The case that is not handled is when
	//	params is used to update only a part of currently registered parameters.
	// TODO: try to solve by setting m_bRelinked = true when params has more parameters than mValues.
	if (mValues.size() != params.getNbParameters())
		mValues = params.mValues;

	for (size_t i = 0; i < params.getNbParameters(); i++)
	{
		for (size_t j = 0; j < mValues.size(); j++)
		{
			if (mValues[j].name == params[i].name)
			{
				switch (mValues[j].kind)
				{
					case CProgramParameters::MATRIX:
						mValues[j].matrix = params[i].matrix;
						break;
					case CProgramParameters::SAMPLER:
						mValues[j].sampler = params[i].sampler;
						break;
					case CProgramParameters::VECTOR:
						mValues[j].vector = params[i].vector;
						break;
					case CProgramParameters::ATTRIBUTE:
						mValues[j].attribute = params[i].attribute;
						break;
				}
			}
		}
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShaderProgram::CShaderProgram(const CPersistence::CPersistenceClassID& id,const std::string& name):
	CPersistence(id,name),
	m_bApplyParameters(false)
{
}

CShaderProgram::CShaderProgram(const CShaderProgram& shader)
    :CPersistence(shaderId,shader.getName())
{
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

bool CShaderProgram::glLoadProgramFromStream(CRaptorIO &stream)
{
	string programstr;

	while (stream.getStatus() == CRaptorIO::IO_OK)
	{
		string line;
		stream >> line;
		if (stream.getStatus() == CRaptorIO::IO_OK)
			programstr = programstr + line + "\n";
	}

	return glLoadProgram(programstr);
}

bool CShaderProgram::exportObject(CRaptorIO& o)
{
	return true;
}

bool CShaderProgram::importObject(CRaptorIO& i)
{
	return true;
}

