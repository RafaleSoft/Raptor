// TexureUnitSetup.cpp: implementation of the CTexureUnitSetup class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif

#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "TextureSet.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#if !defined(AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_)
    #include "Subsys/RegisterCombiner.h"
#endif

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
    #include "TextureFactoryConfig.h"
#endif



RAPTOR_NAMESPACE

static CTextureUnitSetup::CTextureUnitSetupClassID textureId;
const CPersistence::CPersistenceClassID& CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(void)
{
	return textureId;
}

#define DEL_LIST \
	if (handle.handle > 0) \
		glDeleteLists(handle.handle,1);


raptor::CTextureUnitSetup::GL_TEXTURE_COMBINER_TAG::GL_TEXTURE_COMBINER_TAG()
{
	rgb_op = GL_MODULATE;
	alpha_op = GL_MODULATE;
	src_rgb_0 = GL_TEXTURE;
	src_alpha_0 = GL_TEXTURE;

#if defined(GL_ARB_texture_env_combine)
	src_rgb_1 = GL_PREVIOUS_ARB;
	src_rgb_2 = GL_CONSTANT_ARB;
	src_alpha_1 = GL_PREVIOUS_ARB;
	src_alpha_2 = GL_CONSTANT_ARB;
#else
	src_rgb_1 = 0;
	src_rgb_2 = 0;
	src_alpha_1 = 0;
	src_alpha_2 = 0;
#endif	// GL_ARB_texture_env_combine

	op_rgb_0 = GL_SRC_COLOR;
	op_rgb_1 = GL_SRC_COLOR;
	op_rgb_2 = GL_SRC_COLOR;
	op_alpha_0 = GL_SRC_ALPHA;
	op_alpha_1 = GL_SRC_ALPHA;
	op_alpha_2 = GL_SRC_ALPHA;
	rgb_scale = 1.0f;
	alpha_scale = 1.0f;
	rgb_combiner = true;
	alpha_combiner = false;
	color.r = 0.0f;
	color.g = 0.0f;
	color.b = 0.0f;
	color.a = 1.0f;
}


raptor::CTextureUnitSetup::GL_TEXTURE_SHADER_TAG::GL_TEXTURE_SHADER_TAG()
{
	shaderOperation = GL_NONE;
	cullMode[0] = GL_LESS;
	cullMode[1] = GL_LESS;
	cullMode[2] = GL_LESS;
	cullMode[3] = GL_LESS;
	genMode[0] = GL_NONE;
	genMode[1] = GL_NONE;
	genMode[2] = GL_NONE;
	genMode[3] = GL_NONE;
#if defined(GL_NV_texture_shader)
	dotProductMapping = GL_UNSIGNED_IDENTITY_NV;
	previousInput = GL_TEXTURE0_ARB;
#else
	dotProductMapping = 0;
	previousInput = 0;
#endif
	offsetTextureMatrix[0] = 1.0f;
	offsetTextureMatrix[1] = 0.0f;
	offsetTextureMatrix[2] = 0.0f;
	offsetTextureMatrix[3] = 1.0f;
	textureScale = 1.0f;
	textureBias = 0.0f;
	constEye[0] = 0.0f;
	constEye[1] = 0.0f;
	constEye[2] = 1.0f;
	constEye[3] = 1.0f;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureUnitSetup::CTextureUnitSetup():
	CPersistence(textureId,"TMU_Setup")
{
    //! For optimisation and compatibility purposes, only enable the first 4 TMU image units as a default state.
	nbUnits = CTextureFactory::getDefaultFactory().getConfig().getNumTextureImages();

    useUnit = new bool[nbUnits];
    imageUnit = new CTextureObject*[nbUnits];
    tmuShader = new GL_TEXTURE_SHADER[nbUnits];
    tmuCombiner = new GL_TEXTURE_COMBINER[nbUnits];

    for (unsigned int i=0;i<nbUnits;i++)
    {
        useUnit[i] = (i < 4);
        imageUnit[i] = NULL;
        tmuCombiner[i].rgb_combiner = false;
    }

	use_register_combiners = false;
    register_combiners = new CRegisterCombiner;

#ifdef GL_ARB_texture_env_combine
	if (!Raptor::glIsExtensionSupported("GL_ARB_texture_env_combine"))
		Raptor::GetErrorManager()->generateRaptorError(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_WARNING,
                                                        "No texture combiners available for TextureUnitSetup instances");
#endif

#ifdef GL_NV_texture_shader
	if (!Raptor::glIsExtensionSupported("GL_NV_texture_shader"))
		Raptor::GetErrorManager()->generateRaptorError(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_WARNING,
                                                        "No texture shaders available for TextureUnitSetup instances");
#endif

}

CTextureUnitSetup::~CTextureUnitSetup()
{
    if (register_combiners != NULL)
        delete register_combiners;
    if (useUnit != NULL)
        delete [] useUnit;
    if (imageUnit != NULL)
	{
		for (unsigned int i=0;i<nbUnits;i++)
		{
			if (imageUnit[i] != NULL)
				imageUnit[i]->releaseReference();
		}

        delete [] imageUnit;
	}
    if (tmuCombiner != NULL)
        delete [] tmuCombiner;
    if (tmuShader != NULL)
        delete [] tmuShader;
}


const CTextureUnitSetup& CTextureUnitSetup::operator=(const CTextureUnitSetup& rsh)
{
	// Is it really necessary to compare numbre of units ?
    for (unsigned int i=0;i<nbUnits;i++)
    {
        useUnit[i] = rsh.useUnit[i];
        imageUnit[i] = rsh.imageUnit[i];
        tmuShader[i] = rsh.tmuShader[i];
        tmuCombiner[i] = rsh.tmuCombiner[i];
    }

	use_register_combiners = rsh.use_register_combiners;
	*register_combiners = *rsh.register_combiners;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
void CTextureUnitSetup::setMap(CTextureObject *to, TEXTURE_IMAGE_UNIT unit)
{
	if (imageUnit != NULL)
	{
		if (imageUnit[unit] != NULL)
			imageUnit[unit]->releaseReference();
        imageUnit[unit] = to;
		if (to != NULL)
			to->addReference();
	}
}

CTextureObject* const CTextureUnitSetup::getDiffuseMap(void) const 
{
    CTextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_0]; 

    return tmu; 
}

CTextureObject* const CTextureUnitSetup::getNormalMap(void) const 
{ 
    CTextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_1]; 

    return tmu; 
}

CTextureObject* const CTextureUnitSetup::getLightMap(void) const
{ 
    CTextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_2]; 

    return tmu; 
}

CTextureObject* CTextureUnitSetup::getEnvironmentMap(void) const 
{ 
    CTextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_3]; 

    return tmu; 
}

void CTextureUnitSetup::setDiffuseMap(CTextureObject* to) 
{
	setMap(to,IMAGE_UNIT_0);
}

void CTextureUnitSetup::setNormalMap(CTextureObject* to) 
{ 
	setMap(to,IMAGE_UNIT_1);
}

void CTextureUnitSetup::setLightMap(CTextureObject* to)
{ 
    setMap(to,IMAGE_UNIT_2);
}

void CTextureUnitSetup::setEnvironmentMap(CTextureObject* to) 
{ 
    setMap(to,IMAGE_UNIT_3);
}

bool CTextureUnitSetup::enableImageUnit(CTextureUnitSetup::TEXTURE_IMAGE_UNIT unit, bool enable)
{
    if ((unsigned int)unit < nbUnits)
    {
        useUnit[unit] = enable;
        return true;
    }
    else
        return false;
}

CTextureUnitSetup::GL_TEXTURE_SHADER& CTextureUnitSetup::getTMUShader(CTextureUnitSetup::TEXTURE_IMAGE_UNIT unit)
{
    unsigned int numUnit = MIN((unsigned int)unit,nbUnits);

    return tmuShader[numUnit];
}

CTextureUnitSetup::GL_TEXTURE_COMBINER& CTextureUnitSetup::getTMUCombiner(TEXTURE_IMAGE_UNIT unit)
{
    unsigned int numUnit = MIN((unsigned int)unit,nbUnits);

    return tmuCombiner[numUnit];
}

void RAPTOR_FASTCALL CTextureUnitSetup::glRender(GL_TEXTURE_COMBINER *C)
{
	if (C != NULL)
	{
		//	declare combiner
#if defined(GL_ARB_texture_env_combine)
		if ((C->rgb_combiner) || (C->alpha_combiner))
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_ARB);
		else
#endif
			return;
#if defined(GL_ARB_texture_env_combine)
		if (C->rgb_combiner)
		{
			//	rgb operators
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,C->rgb_op);

			//	texels sources
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,C->src_rgb_0);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,C->src_rgb_1);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_ARB,C->src_rgb_2);

			//	texels selection
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,C->op_rgb_0);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,C->op_rgb_1);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,C->op_rgb_2);

			glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE_ARB,C->rgb_scale);

			if ((C->src_rgb_0 == GL_CONSTANT_ARB) ||
				(C->src_rgb_1 == GL_CONSTANT_ARB) ||
				(C->src_rgb_2 == GL_CONSTANT_ARB))
			{
				glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,C->color);
			}
		}

		if (C->alpha_combiner)
		{
			//	alpha operators
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA_ARB,C->alpha_op);

			//	texels sources
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA_ARB,C->src_alpha_0);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA_ARB,C->src_alpha_1);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA_ARB,C->src_alpha_2);

			//	texels selection
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA_ARB,C->op_alpha_0);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA_ARB,C->op_alpha_1);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_ALPHA_ARB,C->op_alpha_2);

			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,C->alpha_scale);
		}

		CATCH_GL_ERROR
#endif
	}
}


bool RAPTOR_FASTCALL CTextureUnitSetup::glRender(CTextureUnitSetup::GL_TEXTURE_SHADER *S)
{
	if (S != NULL)
	{
		if (S->shaderOperation == GL_NONE)
			return true;

		if (S->shaderOperation == CGL_TEXTURE_GEN_PROJECTION)
		{
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_Q);
		
			glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
			glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
			glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

			return true;
		}

		if (S->shaderOperation == CGL_TEXTURE_GEN_COORD)
		{
			if (S->genMode[0] != GL_NONE)
			{
				glEnable(GL_TEXTURE_GEN_S);
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,S->genMode[0]);
			}
			else
				glDisable(GL_TEXTURE_GEN_S);

			if (S->genMode[1] != GL_NONE)
			{
				glEnable(GL_TEXTURE_GEN_T);
				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,S->genMode[1]);
			}
			else
				glDisable(GL_TEXTURE_GEN_T);

			if (S->genMode[2] != GL_NONE)
			{
				glEnable(GL_TEXTURE_GEN_R);
				glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,S->genMode[2]);
			}
			else
				glDisable(GL_TEXTURE_GEN_R);

			if (S->genMode[3] != GL_NONE)
			{
				glEnable(GL_TEXTURE_GEN_Q);
				glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,S->genMode[3]);
			}
			else
				glDisable(GL_TEXTURE_GEN_Q);

			if ((S->genMode[0] == GL_NONE) &&
				(S->genMode[1] == GL_NONE) &&
				(S->genMode[2] == GL_NONE) &&
				(S->genMode[3] == GL_NONE))
			{
				int matrixMode = GL_MODELVIEW;

				glGetIntegerv(GL_MATRIX_MODE,&matrixMode);
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glMatrixMode(matrixMode);
			}

			return true;
		}
		else 
		{
#if defined(GL_NV_texture_shader)
			if (!Raptor::glIsExtensionSupported("GL_NV_texture_shader"))
				return false;
			glEnable(GL_TEXTURE_SHADER_NV);
			glTexEnvi(GL_TEXTURE_SHADER_NV,GL_SHADER_OPERATION_NV,S->shaderOperation);

			switch(S->shaderOperation)
			{
				case GL_TEXTURE_1D:
				case GL_TEXTURE_2D:
	#ifdef GL_EXT_texture3D
				case GL_TEXTURE_3D_EXT:
	#endif
				case GL_TEXTURE_RECTANGLE_NV:
				case GL_TEXTURE_CUBE_MAP_ARB:
				case GL_PASS_THROUGH_NV:
					break;
				case GL_CULL_FRAGMENT_NV:
					glTexEnviv(GL_TEXTURE_SHADER_NV,GL_CULL_MODES_NV,S->cullMode);
					break;
				case GL_DEPENDENT_AR_TEXTURE_2D_NV:
				case GL_DEPENDENT_GB_TEXTURE_2D_NV:
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_PREVIOUS_TEXTURE_INPUT_NV,S->previousInput);
					break;
				case GL_OFFSET_TEXTURE_2D_NV:
				case GL_OFFSET_TEXTURE_2D_SCALE_NV:
				case GL_OFFSET_TEXTURE_RECTANGLE_NV:
				case GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV:
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_PREVIOUS_TEXTURE_INPUT_NV,S->previousInput);
					glTexEnvfv(GL_TEXTURE_SHADER_NV,GL_OFFSET_TEXTURE_MATRIX_NV,S->offsetTextureMatrix);
					if ((S->textureScale != 1.0f) || (S->textureBias != 0.0f))
					{
						glTexEnvf(GL_TEXTURE_SHADER_NV,GL_OFFSET_TEXTURE_SCALE_NV,S->textureScale);
						glTexEnvf(GL_TEXTURE_SHADER_NV,GL_OFFSET_TEXTURE_BIAS_NV,S->textureBias);
					}
					break;
				case GL_DOT_PRODUCT_NV:
				case GL_DOT_PRODUCT_DEPTH_REPLACE_NV:
				case GL_DOT_PRODUCT_TEXTURE_2D_NV:
				case GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV:
				case GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV:
				case GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV:
				case GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV:
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_PREVIOUS_TEXTURE_INPUT_NV,S->previousInput);
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV,S->dotProductMapping);
					break;
				case GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV:
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_PREVIOUS_TEXTURE_INPUT_NV,S->previousInput);
					glTexEnvi(GL_TEXTURE_SHADER_NV,GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV,S->dotProductMapping);
					glTexEnvfv(GL_TEXTURE_SHADER_NV,GL_CONST_EYE_NV,S->constEye);
					break;
				default:
					break;
			}
#endif
		}

		GLint consistent = 1;

#if defined(GL_NV_texture_shader)
		glGetTexEnviv(GL_TEXTURE_SHADER_NV,GL_SHADER_CONSISTENT_NV,&consistent);
#endif

		CATCH_GL_ERROR

		return (consistent == 1);
	}
	else
		return false;
}


RAPTOR_HANDLE CTextureUnitSetup::glBuildSetup(void)
{
	RAPTOR_HANDLE handle;
	handle.hClass = Global::COpenGLClassID::GetClassId().ID();
	handle.handle = glGenLists(1);


	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	GLint previousTMU = GL_TEXTURE0_ARB;

   	//	make sure we disable combiners if registers are used
	if (use_register_combiners)
	{
        for (unsigned int i=0;i<nbUnits;i++)
        {
		    tmuCombiner[i].rgb_combiner = false;
		    tmuCombiner[i].alpha_combiner	= false;
        }
	}

	if (glActiveTextureARB == NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_TEXTURE_MISS);

        //! Disables all image unit above 1, there is always a texture unit available ( 0 )
        for (unsigned int i=1;i<nbUnits;i++)
            useUnit[i] = false;
	}
	else
		glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&previousTMU);

	glNewList(handle.handle,GL_COMPILE);

    for (unsigned int i=0;i<nbUnits;i++)
    {
	    if (useUnit[i])
	    {
		    if (glActiveTextureARB != NULL)
			    glActiveTextureARB(GL_TEXTURE0_ARB+i);

		    if (imageUnit[i] != NULL)
		    {
			    // It is preferable not to render texture extensions in a display list.
			    glEnable(imageUnit[i]->target & 0xFFFF);
                // generators cannot be used in display lists
                CTextureGenerator *G = imageUnit[i]->m_pTexelGenerator;
                if (G != NULL)
                    G->enableGenerator(false);

			    imageUnit[i]->glRender();

                if (G != NULL)
                    G->enableGenerator(true);
		    }
		    else
			    glDisable(GL_TEXTURE_2D);

		    glRender(&tmuCombiner[i]);

		    glRender(&tmuShader[i]);
	    }
    }

	if (glActiveTextureARB != NULL)
		glActiveTextureARB(previousTMU);

#if defined(GL_NV_register_combiners)
	if ((use_register_combiners) && (register_combiners != NULL))
	{
        register_combiners->glRender();
	}
#endif
	
	glEndList();

	CATCH_GL_ERROR

	return handle;
}



RAPTOR_HANDLE CTextureUnitSetup::glBuildUnSetup(void)
{
	RAPTOR_HANDLE handle;
	handle.hClass = Global::COpenGLClassID::GetClassId().ID();
	handle.handle = glGenLists(1);


	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	GLint previousTMU = GL_TEXTURE0_ARB;

    //	make sure we disable combiners if registers are used
	if (use_register_combiners)
	{
        for (unsigned int i=0;i<nbUnits;i++)
        {
		    tmuCombiner[i].rgb_combiner = false;
		    tmuCombiner[i].alpha_combiner	= false;
        }
	}

	if (glActiveTextureARB == NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_TEXTURE_MISS);
		
        //! Disables all image unit above 1, there is always a texture unit available ( 0 )
        for (unsigned int i=1;i<nbUnits;i++)
            useUnit[i] = false;
	}
	else
		glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&previousTMU);

	glNewList(handle.handle,GL_COMPILE);

	for (unsigned int i=0;i<nbUnits;i++)
    {
	    if (useUnit[i])
	    {
		    if (glActiveTextureARB != NULL)
			    glActiveTextureARB(GL_TEXTURE0_ARB+i);

			glBindTexture(imageUnit[i]->target & 0xFFFF,0);
		    glDisable(imageUnit[i]->target & 0xFFFF);

		    // Unsetup of combiners
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

			// Unsetup of shaders
		    glDisable(GL_TEXTURE_GEN_S);
		    glDisable(GL_TEXTURE_GEN_T);
		    glDisable(GL_TEXTURE_GEN_R);
		    glDisable(GL_TEXTURE_GEN_Q);
		    glMatrixMode(GL_TEXTURE);
		    glLoadIdentity();
		    glMatrixMode(GL_MODELVIEW);
    #if defined(GL_NV_texture_shader)
		    if (Raptor::glIsExtensionSupported("GL_NV_texture_shader"))
			    glDisable(GL_TEXTURE_SHADER_NV);
    #endif
	    }		
    }

	if (glActiveTextureARB != NULL)
		glActiveTextureARB(previousTMU);

	#if defined(GL_NV_register_combiners)
		// Unsetup of registers
		if (use_register_combiners)
		{
			glDisable(GL_REGISTER_COMBINERS_NV);
		}
	#endif
					
	glEndList();

	CATCH_GL_ERROR

	return handle;
}



bool ExportCombiner(CRaptorIO& o,CTextureUnitSetup::GL_TEXTURE_COMBINER& cb)
{
	if (cb.rgb_combiner)
		o<<'y';
	else
		o<<'n';

	if (cb.alpha_combiner)
		o<<'y';
	else
		o<<'n';

	o << cb.rgb_op;
	o << cb.rgb_scale;
	o << cb.op_rgb_0;
	o << cb.op_rgb_1;
	o << cb.op_rgb_2;
	o << cb.src_rgb_0;
	o << cb.src_rgb_1;
	o << cb.src_rgb_2;

	o << cb.alpha_op;
	o << cb.alpha_scale;
	o << cb.op_alpha_0;
	o << cb.op_alpha_1;
	o << cb.op_alpha_2;
	o << cb.src_alpha_0;
	o << cb.src_alpha_1;
	o << cb.src_alpha_2;

	o << cb.color;

	return true;
}

bool ImportCombiner(CRaptorIO& i,CTextureUnitSetup::GL_TEXTURE_COMBINER& cb)
{
	char val;
	i>>val;
	cb.rgb_combiner = val == 'y';
	i>>val;
	cb.alpha_combiner = val == 'y';

	i >> cb.rgb_op;
	i >> cb.rgb_scale;
	i >> cb.op_rgb_0;
	i >> cb.op_rgb_1;
	i >> cb.op_rgb_2;
	i >> cb.src_rgb_0;
	i >> cb.src_rgb_1;
	i >> cb.src_rgb_2;

	i >> cb.alpha_op;
	i >> cb.alpha_scale;
	i >> cb.op_alpha_0;
	i >> cb.op_alpha_1;
	i >> cb.op_alpha_2;
	i >> cb.src_alpha_0;
	i >> cb.src_alpha_1;
	i >> cb.src_alpha_2;

	i >> cb.color;

	return true;
}


#if defined(GL_NV_register_combiners)
bool ExportCombinerInput(CRaptorIO& o,CRegisterCombiner::GL_COMBINER_INPUT& cbi)
{
	o << cbi.inputAlpha;
	o << cbi.inputRGB;
	o << cbi.mappingAlpha;
	o << cbi.mappingRGB;
	o << cbi.usageAlpha;
	o << cbi.usageRGB;

	return true;
}
bool ImportCombinerInput(CRaptorIO& i,CRegisterCombiner::GL_COMBINER_INPUT& cbi)
{
	i >> cbi.inputAlpha;
	i >> cbi.inputRGB;
	i >> cbi.mappingAlpha;
	i >> cbi.mappingRGB;
	i >> cbi.usageAlpha;
	i >> cbi.usageRGB;

	return true;
}
bool ExportRegisterCombiner(CRaptorIO& o,CRegisterCombiner::GL_COMBINER& cbr)
{
	if (cbr.active)
		o<<'y';
	else
		o<<'n';

	ExportCombinerInput(o,cbr.a);
	ExportCombinerInput(o,cbr.b);
	ExportCombinerInput(o,cbr.c);
	ExportCombinerInput(o,cbr.d);
	
	o << cbr.bias;
	o << cbr.scale;
	o << cbr.abOutputAlpha;
	o << cbr.abOutputRGB;
	o << cbr.cdOutputAlpha;
	o << cbr.cdOutputRGB;
	o << cbr.functionAlpha;
	o << cbr.functionRGB;
	o << cbr.sumOutputAlpha;
	o << cbr.sumOutputRGB;

	return true;
}
bool ImportRegisterCombiner(CRaptorIO& i,CRegisterCombiner::GL_COMBINER& cbr)
{
	char val;
	i>>val;
	cbr.active = val == 'y';

	ImportCombinerInput(i,cbr.a);
	ImportCombinerInput(i,cbr.b);
	ImportCombinerInput(i,cbr.c);
	ImportCombinerInput(i,cbr.d);
	
	i >> cbr.bias;
	i >> cbr.scale;
	i >> cbr.abOutputAlpha;
	i >> cbr.abOutputRGB;
	i >> cbr.cdOutputAlpha;
	i >> cbr.cdOutputRGB;
	i >> (unsigned int&)cbr.functionAlpha;
	i >> (unsigned int&)cbr.functionRGB;
	i >> cbr.sumOutputAlpha;
	i >> cbr.sumOutputRGB;

	return true;
}
#endif


bool ImportTextureShader(CRaptorIO& i,CTextureUnitSetup::GL_TEXTURE_SHADER &tsh)
{
	return true;
}

bool ExportTextureShader(CRaptorIO& o,CTextureUnitSetup::GL_TEXTURE_SHADER &tsh)
{
	return true;
}

bool CTextureUnitSetup::exportObject(CRaptorIO& o)
{
		CPersistence::exportObject(o);

		//	TMU objects
		//CTextureFactory	exportFactory;
        /*
		if (use_tmu0)
			o<<'y';
		else
			o<<'n';
		if (use_tmu1)
			o<<'y';
		else
			o<<'n';
		if (use_tmu2)
			o<<'y';
		else
			o<<'n';
		if (use_tmu3)
			o<<'y';
		else
			o<<'n';
            */
/*
		unsigned int tmuObjects = 0;
		if (tmu0 != NULL) { tmuObjects += 1; exportFactory.push_back(tmu0); }
		if (tmu1 != NULL) { tmuObjects += 2; exportFactory.push_back(tmu1); }
		if (tmu2 != NULL) { tmuObjects += 4; exportFactory.push_back(tmu2); }
		if (tmu3 != NULL) { tmuObjects += 8; exportFactory.push_back(tmu3); }
		o << tmuObjects;
		exportFactory.Export(o);
		exportFactory.clear();
*/
		o<<'y';
		ExportCombiner(o,tmuCombiner[0]);
		o<<'y';
		ExportCombiner(o,tmuCombiner[1]);
		o<<'y';
		ExportCombiner(o,tmuCombiner[2]);
		o<<'y';
		ExportCombiner(o,tmuCombiner[3]);

		ExportTextureShader(o,tmuShader[0]);
		ExportTextureShader(o,tmuShader[1]);
		ExportTextureShader(o,tmuShader[2]);
		ExportTextureShader(o,tmuShader[3]);

		if (use_register_combiners)
		{
			o<<'y';

#if defined(GL_NV_register_combiners)
			ExportRegisterCombiner(o,register_combiners->r0);
			ExportRegisterCombiner(o,register_combiners->r1);
			ExportRegisterCombiner(o,register_combiners->r2);
			ExportRegisterCombiner(o,register_combiners->r3);
			ExportRegisterCombiner(o,register_combiners->r4);
			ExportRegisterCombiner(o,register_combiners->r5);
			ExportRegisterCombiner(o,register_combiners->r6);
			ExportRegisterCombiner(o,register_combiners->r7);

			ExportCombinerInput(o,register_combiners->a);
			ExportCombinerInput(o,register_combiners->b);
			ExportCombinerInput(o,register_combiners->c);
			ExportCombinerInput(o,register_combiners->d);
			ExportCombinerInput(o,register_combiners->e);
			ExportCombinerInput(o,register_combiners->f);
			ExportCombinerInput(o,register_combiners->g);

			o << register_combiners->inputAlpha;
			o << register_combiners->mappingAlpha;
			o << register_combiners->usageAlpha;
			o << register_combiners->color_sum;
			o << register_combiners->multiplier;
			o << register_combiners->function;
#endif
		}

		return true;
}

bool CTextureUnitSetup::importMap(TEXTURE_IMAGE_UNIT unit,CRaptorIO& io)
{
	string textureName;
	string setName;
	string name;
    io >> name;
	bool useunit = false;
	bool enable = useUnit[unit];

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "set")
            io >> setName;
		else if (data == "texname")
            io >> textureName;
		else if (data == "enable")
		{
			io >> enable;
			useunit = true;
		}
		else
			io >> name;
	
		data = io.getValueName();
	}

	if ((!setName.empty()) && (!textureName.empty()))
	{
		CTextureSet *pSet = (CTextureSet *)CPersistence::FindObject(setName);
		if (pSet != NULL)
		{
			CTextureObject *pImage = pSet->getTexture(textureName);
			setMap(pImage,unit);
			if (useUnit)
				useUnit[unit] = enable;
		}
	}
	
	io >> name;

	return true;
}

bool CTextureUnitSetup::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "DiffuseMap")
		{
			importMap(IMAGE_UNIT_0,io);
		}
		else if (data == "NormalMap")
		{
			importMap(IMAGE_UNIT_1,io);
		}
		else if (data == "LightMap")
		{
			importMap(IMAGE_UNIT_2,io);
		}
		else if (data == "EnvironmentMap")
		{
			importMap(IMAGE_UNIT_3,io);
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
/*
		char val;

		//	TMU objects
		CTextureFactory	importFactory;
		i>>val;
		use_tmu0 = val == 'y';
		i>>val;
		use_tmu1 = val == 'y';
		i>>val;
		use_tmu2 = val == 'y';
		i>>val;
		use_tmu3 = val == 'y';

		unsigned int pos = 0;
		unsigned int tmuObjects = 0;
		i >> tmuObjects;
		importFactory.Import(i);
		if (tmuObjects & 1) { tmu0 = importFactory.at(pos++); }
		if (tmuObjects & 2) { tmu1 = importFactory.at(pos++); }
		if (tmuObjects & 4) { tmu2 = importFactory.at(pos++); }
		if (tmuObjects & 8) { tmu3 = importFactory.at(pos++); }
		importFactory.clear();

		//	initial combiner
		i>>val;
		ImportCombiner(i,void_to_tmu0);

		//	combiner 0
		i>>val;
		ImportCombiner(i,tmu0_to_tmu1);

		// combiner 1
		i>>val;
		ImportCombiner(i,tmu1_to_tmu2);

		// combiner 2
		i>>val;
		ImportCombiner(i,tmu2_to_tmu3);

		ImportTextureShader(i,tmu0_shader);
		ImportTextureShader(i,tmu1_shader);
		ImportTextureShader(i,tmu2_shader);
		ImportTextureShader(i,tmu3_shader);


		i>>val;
		use_register_combiners = val == 'y';

#if defined(GL_NV_register_combiners)
		if (use_register_combiners)
		{
			ImportRegisterCombiner(i,register_combiners.r0);
			ImportRegisterCombiner(i,register_combiners.r1);
			ImportRegisterCombiner(i,register_combiners.r2);
			ImportRegisterCombiner(i,register_combiners.r3);
			ImportRegisterCombiner(i,register_combiners.r4);
			ImportRegisterCombiner(i,register_combiners.r5);
			ImportRegisterCombiner(i,register_combiners.r6);
			ImportRegisterCombiner(i,register_combiners.r7);

			ImportCombinerInput(i,register_combiners.a);
			ImportCombinerInput(i,register_combiners.b);
			ImportCombinerInput(i,register_combiners.c);
			ImportCombinerInput(i,register_combiners.d);
			ImportCombinerInput(i,register_combiners.e);
			ImportCombinerInput(i,register_combiners.f);
			ImportCombinerInput(i,register_combiners.g);

			i >> register_combiners.inputAlpha;
			i >> register_combiners.mappingAlpha;
			i >> register_combiners.usageAlpha;
			i >> register_combiners.color_sum;
			i >> register_combiners.multiplier;
			i >> register_combiners.function;
		}
#endif
		return true;
	}
	else
		return false;
		*/
}



