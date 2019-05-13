// ShaderLibrary.cpp: implementation of the CShaderLibrary class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "ShaderLibrary.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
	#include "BlinnShader.h"
#endif
#if !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
	#include "PhongShader.h"
#endif
#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
#endif
#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "EMBMShader.h"
#endif
#if !defined(AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_)
	#include "AOComputeShader.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static bool				s_initialized = false;
static map<std::string,std::string>	s_factoryShaders;
static const int		NB_MANDATORY_SHADERS = 16;
static const std::string s_nullShaderName = "NULL_SHADER";

CShaderLibrary *CShaderLibrary::m_pInstance = NULL;
CShader *CShaderLibrary::m_pNullShader = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderLibrary::CShaderLibrary()
{

}

void CShaderLibrary::destroy()
{
	delete m_pInstance;
}

CShaderLibrary::~CShaderLibrary()
{
	map<std::string,std::string>::iterator itr = s_factoryShaders.begin();
	while (itr != s_factoryShaders.end())
	{
		CPersistence *p = CPersistence::FindObject((*itr).first);
		if (NULL != p)
			if (p->getId().isSubClassOf(CShaderProgram::CShaderProgramClassID::GetClassId()))
			{
				CShaderProgram *program = (CShaderProgram *)p;
				delete program;
			}
		itr++;
	}

	m_pInstance = NULL;
}


CShaderLibrary *CShaderLibrary::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CShaderLibrary();
	}

	return m_pInstance;
}

void CShaderLibrary::getFactoryShaders(vector<std::string> & res)
{
	res.clear();

	map<std::string,std::string>::iterator itr = s_factoryShaders.begin();
	while (itr != s_factoryShaders.end())
	{
		itr++;
		res.push_back((*itr).first);
	}
}

bool CShaderLibrary::glAddToLibrary(const char * const *shader_sources)
{

	int pos = 0;
	while (shader_sources[pos] != 0)
	{
		const char *fs = shader_sources[pos];
		pos++;
		const char *fs_name = shader_sources[pos];
		pos++;

		if (fs != 0)
		{
			CShaderProgram *program = NULL;
			if (!strncmp(fs, "!!ARBfp", 7))
			{
				program = new CFragmentShader(fs_name);
			}
			else if (!strncmp(fs, "!!ARBvp", 7))
			{
				program = new CVertexShader(fs_name);
			}
			else if (NULL != strstr(fs, "gl_Position"))
			{
				program = new CVertexProgram(fs_name);
			}
			else if (NULL != strstr(fs, "gl_FragColor"))
			{
				program = new CFragmentProgram(fs_name);
			}

			//! Errors are genereted in debug mode in LoadProgram.
			//! No extra check is needed here.
			if (program != NULL)
			{
				program->glLoadProgram(fs);
				s_factoryShaders.insert(map<std::string, std::string>::value_type(fs_name, fs));
			}
		}
	}

	return true;
}

bool CShaderLibrary::glInitFactory(void)
{
	if (s_initialized)
		return false;

	CContextManager *manager = CContextManager::GetInstance();
	if (CContextManager::INVALID_CONTEXT == manager->glGetCurrentContext())
		return false;

	CRaptorDataManager *dataManager = CRaptorDataManager::GetInstance();
	const char * const *shader_source_library = dataManager->GetShaderList();

	if (shader_source_library == NULL)
		return false;
	else
		glAddToLibrary(shader_source_library);


    CATCH_GL_ERROR

    //! check mandatory shaders for Raptor to work with minimal settings
    unsigned int nbMandatory = 0;
    const char *shaders[NB_MANDATORY_SHADERS] = {	"BUMP_TEX_SHADER", 
													"EMBM_TEX_SHADER",
													"BUMP_VTX_SHADER", 	
													"EMBM_VTX_SHADER",
													"PROJECTION_TEX_SHADER", 
													"SHADOWMAP_TEX_SHADER",
													"SHADOWMAP_TEX_SHADER_PCF",  
													"SHADOWMAP_TEX_SHADER_PCF_4X",
													"PPIXEL_BLINN_VTX_PROGRAM",
													"PPIXEL_BLINN_TEX_PROGRAM",
													"PPIXEL_PHONG_VTX_PROGRAM",
													"PPIXEL_PHONG_TEX_PROGRAM",
													"PPIXEL_BUMP_VTX_PROGRAM",
													"PPIXEL_BUMP_TEX_PROGRAM",
													"AMBIENT_OCCLUSION_VTX_PROGRAM",
													"AMBIENT_OCCLUSION_TEX_PROGRAM"};

    for (int i=0;i<NB_MANDATORY_SHADERS;i++)
    {
        if (s_factoryShaders.find(shaders[i]) != s_factoryShaders.end())
              nbMandatory++;
    }

	//	This is not a very clean way to prevent reentry !
	s_initialized = (nbMandatory == NB_MANDATORY_SHADERS);

	if (s_initialized)
	{
		CBlinnShader *pBlinnShader = new CBlinnShader();
		pBlinnShader->glInit();

		CPhongShader *pPhongShader = new CPhongShader();
		pPhongShader->glInit();

		CBumpShader *pBumpShader = new CBumpShader();
		pBumpShader->glInit();

		CEMBMShader *pEMBMShader = new CEMBMShader();
		map<std::string,std::string>::const_iterator it = s_factoryShaders.find("PPIXEL_BUMP_VTX_PROGRAM");
		map<std::string,std::string>::const_iterator it2 = s_factoryShaders.find("PPIXEL_BUMP_TEX_PROGRAM");
		pEMBMShader->glInit((*it).second,(*it2).second);
		//s_factoryShaders.insert(map<std::string,std::string>::value_type("PPIXEL_EMBM_TEX_PROGRAM",embm_shader));

		CAOComputeShader *pAOComputeShader = new CAOComputeShader();
		pAOComputeShader->glInit();
	}

	m_pNullShader = new CShader(s_nullShaderName);

	return s_initialized;
}

