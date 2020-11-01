// Filters.cpp: implementation of the CFilters class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_FILTERS_H__FA3B0FCF_C410_4469_B9B0_DEE07A8424A2__INCLUDED_)
	#include "Filters.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE

typedef struct
{
	const char *shader_name;
	const char *shader_fname;
	const char *class_name;
} factory_shader;

static const size_t NB_FACTORY_SHADERS = 33;
static factory_shader fsh[NB_FACTORY_SHADERS] = {	{ "DOF_GEO_SHADER", "DOF.gs", "GeometryShader" },
													{ "DOF_TEX_SHADER", "DOF.ps", "FragmentShader" },
													{ "DOF_OLD_VTX_SHADER", "DOF.vs", "VertexShader" },
													{ "DOF_OLD_TEX_SHADER", "DOF_old.ps", "FragmentShader" },
													{ "DOF_VTX_PROGRAM", "DOF.vp", "VertexProgram" },
													{ "DOF_TEX_PROGRAM", "DOF.fp", "FragmentProgram" },
													{ "COLOR_TEX_SHADER", "Color.ps", "FragmentShader" },
													{ "COLOR_OLD_TEX_SHADER", "Color_old.ps", "FragmentShader" },
													{ "COLOR_TEX_PROGRAM", "Color.fp", "FragmentProgram" },
													{ "MAGNIFIER_GEO_SHADER", "Magnifier.gs", "GeometryShader" },
													{ "MAGNIFIER_X_TEX_SHADER", "Magnifier_X.ps", "FragmentShader" },
													{ "MAGNIFIER_Y_TEX_SHADER", "Magnifier_Y.ps", "FragmentShader" },
													{ "MAGNIFIER_VTX_SHADER", "Magnifier.vs", "VertexShader" },
													{ "MAGNIFIER_OLDX_TEX_SHADER", "Magnifier_oldX.ps", "FragmentShader" },
													{ "MAGNIFIER_OLDY_TEX_SHADER", "Magnifier_oldY.ps", "FragmentShader" },
													{ "MAGNIFIER_VTX_PROGRAM", "Magnifier.vp", "VertexProgram" },
													{ "MAGNIFIER_X_TEX_PROGRAM", "Magnifier_X.fp", "FragmentProgram" },
													{ "MAGNIFIER_Y_TEX_PROGRAM", "Magnifier_Y.fp", "FragmentProgram" },
													{ "HDR_THRESHOLD_TEX_SHADER", "HDR_threshold.ps", "FragmentShader" },
													{ "HDR_COMPOSER_TEX_SHADER", "HDR_composer.ps", "FragmentShader" },
													{ "HDR_LUMINANCE_TEX_SHADER", "HDR_luminance.ps", "FragmentShader" },
													{ "HDR_LUMINANCELAST_TEX_SHADER", "HDR_luminanceLast.ps", "FragmentShader" },
													{ "HDR_THRESHOLD_OLD_TEX_SHADER", "HDR_threshold_old.ps", "FragmentShader" },
													{ "HDR_COMPOSER_OLD_TEX_SHADER", "HDR_composer_old.ps", "FragmentShader" },
													{ "HDR_LUMINANCE_OLD_TEX_SHADER", "HDR_luminance_old.ps", "FragmentShader" },
													{ "HDR_LUMINANCELAST_OLD_TEX_SHADER", "HDR_luminanceLast_old.ps", "FragmentShader" },
													{ "HDR_THRESHOLD_TEX_PROGRAM", "HDR_threshold.fp", "FragmentProgram" },
													{ "HDR_COMPOSER_TEX_PROGRAM", "HDR_composer.fp", "FragmentProgram" },
													{ "HDR_LUMINANCE_TEX_PROGRAM", "HDR_luminance.fp", "FragmentProgram" },
													{ "HDR_LUMINANCELAST_TEX_PROGRAM", "HDR_luminanceLast.fp", "FragmentProgram" },
													{ "MB_ACCUM_TEX_SHADER", "MB_accum.ps", "FragmentShader" },
													{ "MB_ACCUM2_TEX_SHADER", "MB_accum2.ps", "FragmentShader" },
													{ "MB_ACCUM_OLD_TEX_SHADER", "MB_accum_old.ps", "FragmentShader" } };



static map<std::string, factory_shader>	s_factoryShaders;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilters::CFilters()
{

}

CFilters::~CFilters()
{

}

bool CFilters::glInstallFilters(void)
{
	CRaptorDataManager *dataManager = CRaptorDataManager::GetInstance();
	if (NULL == dataManager)
		return false;
	if (!dataManager->managePackage("Toolbox.pck"))
		return false;

	bool res = true;
	for (size_t nb_shaders = 0; nb_shaders < NB_FACTORY_SHADERS; nb_shaders++)
	{
		factory_shader &fs = fsh[nb_shaders];
		string shader_path = dataManager->exportFile(fs.shader_fname);
		if (!shader_path.empty())
			res = res && CShaderProgram::glAddToLibrary(fs.shader_name, shader_path, fs.class_name);
	}

    return res;
}

bool CFilters::glInstallFilter(const std::string& )
{
    bool res = true;

    return res;
}

