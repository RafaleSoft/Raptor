// RegisterCombiner.cpp: implementation of the CRegisterCombiner class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_)
    #include "RegisterCombiner.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegisterCombiner::CRegisterCombiner()
{
#if defined(GL_NV_register_combiners)
    inputAlpha = GL_ZERO;
	mappingAlpha = GL_UNSIGNED_INVERT_NV;
	usageAlpha = GL_ALPHA;
#endif
}

CRegisterCombiner::~CRegisterCombiner()
{

}

#if defined(GL_NV_register_combiners)
CRegisterCombiner::GL_COMBINER_INPUT::GL_COMBINER_INPUT()
{
	inputRGB = GL_ZERO;
	inputAlpha = GL_ZERO;
	mappingRGB = GL_UNSIGNED_IDENTITY_NV;
	mappingAlpha = GL_UNSIGNED_IDENTITY_NV;
	usageRGB = GL_RGB;
	usageAlpha = GL_ALPHA;
}

CRegisterCombiner::GL_COMBINER::GL_COMBINER()
{
	functionRGB = CGL_DOT_DOT_DISCARD;
	functionAlpha = CGL_MULT_MULT_SUM;
	abOutputRGB = GL_PRIMARY_COLOR_NV;
	cdOutputRGB = GL_SPARE0_NV;
	sumOutputRGB = GL_DISCARD_NV;
	abOutputAlpha = GL_PRIMARY_COLOR_NV;
	cdOutputAlpha = GL_SPARE0_NV;
	sumOutputAlpha = GL_DISCARD_NV;
	scale = GL_NONE;
	bias = GL_NONE;
	active = false;
}
#endif

void CRegisterCombiner::glRender(void)
{
#if defined(GL_NV_register_combiners)
	glEnable(GL_REGISTER_COMBINERS_NV);
	int nb_rc = 0;
	GLenum stage = GL_COMBINER0_NV;

	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();
	PFN_GL_COMBINER_INPUT_NV_PROC glCombinerInputNV = pExtensions->glCombinerInputNV;
	PFN_GL_COMBINER_OUTPUT_NV_PROC glCombinerOutputNV = pExtensions->glCombinerOutputNV;
	PFN_GL_COMBINER_PARAMETER_I_NV_PROC glCombinerParameteriNV = pExtensions->glCombinerParameteriNV;
	PFN_GL_FINAL_COMBINER_INPUT_NV glFinalCombinerInputNV = pExtensions->glFinalCombinerInputNV;

    const CRegisterCombiner::GL_COMBINER *tab_rc[8] = { &r0, &r1,&r2,&r3,&r4,&r5,&r6,&r7 };

	//	General Combiners setup
	while ((nb_rc < 8) && (tab_rc[nb_rc]->active))
	{
		//	RGB input
		glCombinerInputNV(	stage,
							GL_RGB,
							GL_VARIABLE_A_NV,
							tab_rc[nb_rc]->a.inputRGB,
							tab_rc[nb_rc]->a.mappingRGB,
							tab_rc[nb_rc]->a.usageRGB);
		glCombinerInputNV(	stage,
							GL_RGB,
							GL_VARIABLE_B_NV,
							tab_rc[nb_rc]->b.inputRGB,
							tab_rc[nb_rc]->b.mappingRGB,
							tab_rc[nb_rc]->b.usageRGB);
		glCombinerInputNV(	stage,
							GL_RGB,
							GL_VARIABLE_C_NV,
							tab_rc[nb_rc]->c.inputRGB,
							tab_rc[nb_rc]->c.mappingRGB,
							tab_rc[nb_rc]->c.usageRGB);
		glCombinerInputNV(	stage,
							GL_RGB,
							GL_VARIABLE_D_NV,
							tab_rc[nb_rc]->d.inputRGB,
							tab_rc[nb_rc]->d.mappingRGB,
							tab_rc[nb_rc]->d.usageRGB);

		//	Alpha input
		glCombinerInputNV(	stage,
							GL_ALPHA,
							GL_VARIABLE_A_NV,
							tab_rc[nb_rc]->a.inputAlpha,
							tab_rc[nb_rc]->a.mappingAlpha,
							tab_rc[nb_rc]->a.usageAlpha);
		glCombinerInputNV(	stage,
							GL_ALPHA,
							GL_VARIABLE_B_NV,
							tab_rc[nb_rc]->b.inputAlpha,
							tab_rc[nb_rc]->b.mappingAlpha,
							tab_rc[nb_rc]->b.usageAlpha);
		glCombinerInputNV(	stage,
							GL_ALPHA,
							GL_VARIABLE_C_NV,
							tab_rc[nb_rc]->c.inputAlpha,
							tab_rc[nb_rc]->c.mappingAlpha,
							tab_rc[nb_rc]->c.usageAlpha);
		glCombinerInputNV(	stage,
							GL_ALPHA,
							GL_VARIABLE_D_NV,
							tab_rc[nb_rc]->d.inputAlpha,
							tab_rc[nb_rc]->d.mappingAlpha,
							tab_rc[nb_rc]->d.usageAlpha);

		//	RGB Output
		glCombinerOutputNV(	stage,
							GL_RGB,
							tab_rc[nb_rc]->abOutputRGB,
							tab_rc[nb_rc]->cdOutputRGB,
							tab_rc[nb_rc]->sumOutputRGB,
							tab_rc[nb_rc]->scale,
							tab_rc[nb_rc]->bias,
							(tab_rc[nb_rc]->functionRGB & 0x4) ? true : false,
							(tab_rc[nb_rc]->functionRGB & 0x2) ? true : false,
							(tab_rc[nb_rc]->functionRGB & 0x1) ? true : false);

		//	Alpha Output
		glCombinerOutputNV(	stage,
							GL_ALPHA,
							tab_rc[nb_rc]->abOutputAlpha,
							tab_rc[nb_rc]->cdOutputAlpha,
							tab_rc[nb_rc]->sumOutputAlpha,
							tab_rc[nb_rc]->scale,
							tab_rc[nb_rc]->bias,
							(tab_rc[nb_rc]->functionAlpha & 0x4) ? true : false,
							(tab_rc[nb_rc]->functionAlpha & 0x2) ? true : false,
							(tab_rc[nb_rc]->functionAlpha & 0x1) ? true : false);

		nb_rc++;
		stage++;
	}

	//	Final Combiners setup
	glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV,nb_rc);
	
	glFinalCombinerInputNV(	GL_VARIABLE_A_NV,
							a.inputRGB,
							a.mappingRGB,
							a.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_B_NV,
							b.inputRGB,
							b.mappingRGB,
							b.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_C_NV,
							c.inputRGB,
							c.mappingRGB,
							c.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_D_NV,
							d.inputRGB,
							d.mappingRGB,
							d.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_E_NV,
							e.inputRGB,
							e.mappingRGB,
							e.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_F_NV,
							f.inputRGB,
							f.mappingRGB,
							f.usageRGB);
	glFinalCombinerInputNV(	GL_VARIABLE_G_NV,
							inputAlpha,
							mappingAlpha,
							usageAlpha);
	
	CATCH_GL_ERROR

#endif
}

