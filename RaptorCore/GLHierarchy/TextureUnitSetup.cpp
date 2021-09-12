/***************************************************************************/
/*                                                                         */
/*  TexureUnitSetup.cpp                                                    */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_)
    #include "Subsys/RegisterCombiner.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLTextureObject.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

static CTextureUnitSetup::CTextureUnitSetupClassID textureId;
const CPersistence::CPersistenceClassID& CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(void)
{
	return textureId;
}

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)

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
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureUnitSetup::CTextureUnitSetup() :
	CPersistence(textureId, "TMU_Setup"), pfn_glActiveTexture(NULL)
{
	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();
	pfn_glActiveTexture = pExtensions->glActiveTextureARB;
	if (NULL == pfn_glActiveTexture)
	{
		RAPTOR_FATAL(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
						CRaptorMessages::ID_TEXTURE_MISS);
	}

	//! For optimisation and compatibility purposes, only enable the first 4 TMU image units as a default state.
	nbUnits = CTextureFactory::glGetDefaultFactory().getConfig().getNumTextureImages();

	useUnit = new bool[nbUnits];
	imageUnit = new ITextureObject*[nbUnits];
	unitFunctions = new GLenum[nbUnits];

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
	tmuShader = new GL_TEXTURE_SHADER[nbUnits];
	tmuCombiner = new GL_TEXTURE_COMBINER[nbUnits];
#endif

	useUnit[0] = true;				// Texture unit 0 will always be defined.
	for (unsigned int i = 0; i < nbUnits; i++)
	{
		if (NULL != pfn_glActiveTexture)
			useUnit[i] = (i < 4);	// Default will use 4 texture units.
		imageUnit[i] = NULL;
		unitFunctions[i] = CTextureUnitSetup::CGL_NONE;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		tmuCombiner[i].rgb_combiner = false;
#endif
	}

	use_register_combiners = false;
	register_combiners = new CRegisterCombiner;

#ifdef GL_ARB_texture_env_combine
	if (!IRaptor::glIsExtensionSupported(GL_ARB_TEXTURE_ENV_COMBINE_EXTENSION_NAME))
	{
		RAPTOR_WARNING(CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
					   "No texture combiners available for TextureUnitSetup instances");
	}
#endif

#ifdef GL_NV_texture_shader
	if (!IRaptor::glIsExtensionSupported(GL_NV_TEXTURE_SHADER_EXTENSION_NAME))
	{
		RAPTOR_WARNING(	CTextureUnitSetup::CTextureUnitSetupClassID::GetClassId(),
						"No texture shaders available for TextureUnitSetup instances");
	}
#endif

}

CTextureUnitSetup::~CTextureUnitSetup()
{
    if (NULL != register_combiners)
        delete register_combiners;
    if (NULL != useUnit)
        delete [] useUnit;
    if (NULL != imageUnit)
	{
		for (unsigned int i=0;i<nbUnits;i++)
		{
			if (NULL != imageUnit[i])
				imageUnit[i]->releaseReference();
		}

        delete [] imageUnit;
	}
	if (NULL != unitFunctions)
		delete [] unitFunctions;

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
    if (NULL != tmuCombiner)
        delete [] tmuCombiner;
    if (NULL != tmuShader)
        delete [] tmuShader;
#endif
}


const CTextureUnitSetup& CTextureUnitSetup::operator=(const CTextureUnitSetup& rsh)
{
	// Is it really necessary to compare numbre of units ?
    for (unsigned int i=0;i<nbUnits;i++)
    {
        useUnit[i] = rsh.useUnit[i];
        imageUnit[i] = rsh.imageUnit[i];
		if (NULL != imageUnit[i])
			imageUnit[i]->addReference();
		unitFunctions[i] = rsh.unitFunctions[i];

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
        tmuShader[i] = rsh.tmuShader[i];
        tmuCombiner[i] = rsh.tmuCombiner[i];
#endif
    }

	use_register_combiners = rsh.use_register_combiners;
	*register_combiners = *rsh.register_combiners;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
void CTextureUnitSetup::setMap(ITextureObject *to, TEXTURE_IMAGE_UNIT unit, TEXTURE_UNIT_FUNCTION env_mode)
{
	if (imageUnit != NULL)
	{
		if (setUnitFunction(unit, env_mode))
		{
			if (imageUnit[unit] != NULL)
				imageUnit[unit]->releaseReference();
			imageUnit[unit] = to;
			if (to != NULL)
				to->addReference();
		}
	}
}

bool CTextureUnitSetup::setUnitFunction(TEXTURE_IMAGE_UNIT unit, TEXTURE_UNIT_FUNCTION env_mode)
{
	bool res = false;

	if ((uint32_t)unit < nbUnits)
	{
		if (NULL != pfn_glActiveTexture)
		{
			GLenum mode = GL_NONE;
			res = true;

			switch (env_mode)
			{
				case CGL_OPAQUE:
					mode = GL_REPLACE;		// 0x1E01
					break;
				case CGL_MULTIPLY:
					mode = GL_MODULATE;		// 0x2100
					break;
				case CGL_ALPHA_TRANSPARENT:
					mode = GL_DECAL;		// 0x2101
					break;
				case CGL_CONSTANT_BLENDED:
					mode = GL_BLEND;		// 0x0BE2
					break;
				case CGL_NONE:
					mode = CGL_NONE;		// 0
					break;
				default:
					res = false;
					break;
			}
			
			if (res)
				unitFunctions[unit] = mode;
		}
	}

	return res;
}

ITextureObject* const CTextureUnitSetup::getDiffuseMap(void) const 
{
    ITextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_0]; 

    return tmu; 
}

ITextureObject* const CTextureUnitSetup::getNormalMap(void) const 
{ 
    ITextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_1]; 

    return tmu; 
}

ITextureObject* const CTextureUnitSetup::getLightMap(void) const
{ 
    ITextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_2]; 

    return tmu; 
}

ITextureObject* CTextureUnitSetup::getEnvironmentMap(void) const 
{ 
    ITextureObject *tmu = NULL;

    if (imageUnit != NULL)
        tmu = imageUnit[IMAGE_UNIT_3]; 

    return tmu; 
}

void CTextureUnitSetup::setDiffuseMap(ITextureObject* to, TEXTURE_UNIT_FUNCTION env_mode)
{
	setMap(to,IMAGE_UNIT_0, env_mode);
}

void CTextureUnitSetup::setNormalMap(ITextureObject* to, TEXTURE_UNIT_FUNCTION env_mode)
{ 
	setMap(to,IMAGE_UNIT_1, env_mode);
}

void CTextureUnitSetup::setLightMap(ITextureObject* to, TEXTURE_UNIT_FUNCTION env_mode)
{ 
    setMap(to,IMAGE_UNIT_2, env_mode);
}

void CTextureUnitSetup::setEnvironmentMap(ITextureObject* to, TEXTURE_UNIT_FUNCTION env_mode)
{ 
    setMap(to,IMAGE_UNIT_3, env_mode);
}

bool CTextureUnitSetup::enableImageUnit(CTextureUnitSetup::TEXTURE_IMAGE_UNIT unit, bool enable)
{
	bool res = false;

    if ((unsigned int)unit < nbUnits)
    {
		if (NULL != pfn_glActiveTexture)
		{
			useUnit[unit] = enable;
			res = true;
		}
    }
    
	return res;
}

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
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
#endif

//!
//!	These features are deprecated since OpenGL 3.0 deprecation model.
//!	Kept for compatibility purpose, subject to removal in future versions of Raptor.	
//!
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)

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
			if (!IRaptor::glIsExtensionSupported(GL_NV_TEXTURE_SHADER_EXTENSION_NAME))
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
	RAPTOR_HANDLE handle(COpenGL::COpenGLClassID::GetClassId().ID(),glGenLists(1));
	
   	//	make sure we disable combiners if registers are used
	if (use_register_combiners)
	{
        for (unsigned int i=0;i<nbUnits;i++)
        {
		    tmuCombiner[i].rgb_combiner = false;
		    tmuCombiner[i].alpha_combiner	= false;
        }
	}

	GLint previousTMU = GL_TEXTURE0_ARB;
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&previousTMU);

	glNewList(handle.glname(),GL_COMPILE);

    for (unsigned int i=0;i<nbUnits;i++)
    {
	    if (useUnit[i])
	    {
		    if (pfn_glActiveTexture != NULL)
				pfn_glActiveTexture(GL_TEXTURE0_ARB+i);

		    if (imageUnit[i] != NULL)
		    {	// TODO: make this section generic
				COpenGLTextureObject* txt = imageUnit[i]->getGLTextureObject();

			    // It is preferable not to render texture extensions in a display list.
			    glEnable(txt->target);
                // generators cannot be used in display lists
				ITextureGenerator *G = txt->getTexelGenerator();
                if (G != NULL)
                    G->enableGenerator(false);

				txt->glvkRender();

                if (G != NULL)
                    G->enableGenerator(true);
		    }
		    else
			    glDisable(GL_TEXTURE_2D);

		    glRender(&tmuCombiner[i]);

		    glRender(&tmuShader[i]);
	    }
    }

	if (pfn_glActiveTexture != NULL)
		pfn_glActiveTexture(previousTMU);

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

#else
#endif

void CTextureUnitSetup::glRender(void)
{
	for (unsigned int i = 0; i < nbUnits; i++)
	{
		if (useUnit[i])
		{
			if (pfn_glActiveTexture != NULL)
				pfn_glActiveTexture(GL_TEXTURE0_ARB + i);

			if (imageUnit[i] != NULL)
			{
				COpenGLTextureObject* txt = imageUnit[i]->getGLTextureObject();

				// It is preferable not to render texture extensions in a display list.
				// generators cannot be used in display lists
				glEnable(txt->target);
				glBindTexture(txt->target, txt->texname);

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
				if (unitFunctions[i] != CGL_NONE)
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, unitFunctions[i]);
#endif
			}
			else
				glDisable(GL_TEXTURE_2D);
		}
	}

	if (pfn_glActiveTexture != NULL)
		pfn_glActiveTexture(GL_TEXTURE0_ARB);

	CATCH_GL_ERROR
}




#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
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
#endif

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

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
bool ImportTextureShader(CRaptorIO& i,CTextureUnitSetup::GL_TEXTURE_SHADER &tsh)
{
	return true;
}

bool ExportTextureShader(CRaptorIO& o,CTextureUnitSetup::GL_TEXTURE_SHADER &tsh)
{
	return true;
}
#endif

bool CTextureUnitSetup::exportObject(CRaptorIO& o)
{
		CPersistence::exportObject(o);

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
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
#endif
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

bool CTextureUnitSetup::importMap(CRaptorIO& io)
{
	CTextureUnitSetup::TEXTURE_UNIT_FUNCTION unitFunction = CTextureUnitSetup::CGL_NONE;
	CTextureUnitSetup::TEXTURE_IMAGE_UNIT unit = CTextureUnitSetup::IMAGE_UNIT_0;
	std::string textureName;
	std::string setName;
	std::string name;
    
	bool useunit = false;
	bool enable = useUnit[unit];

	io >> name;
	string data = io.getValueName();
	while (io.hasMoreValues())
    {
		if (data == "TextureSet")
            io >> setName;
		else if (data == "TextureUnit")
		{
			std::string tunit;
			io >> tunit;

			if (tunit == "Unit0")
				unit = CTextureUnitSetup::IMAGE_UNIT_0;
			else if (tunit == "Unit1")
				unit = CTextureUnitSetup::IMAGE_UNIT_1;
			else if (tunit == "Unit2")
				unit = CTextureUnitSetup::IMAGE_UNIT_2;
			else if (tunit == "Unit3")
				unit = CTextureUnitSetup::IMAGE_UNIT_3;
			else if (tunit == "Unit4")
				unit = CTextureUnitSetup::IMAGE_UNIT_4;
			else if (tunit == "Unit5")
				unit = CTextureUnitSetup::IMAGE_UNIT_5;
			else if (tunit == "Unit6")
				unit = CTextureUnitSetup::IMAGE_UNIT_6;
			else if (tunit == "Unit7")
				unit = CTextureUnitSetup::IMAGE_UNIT_7;
			else if (tunit == "Unit8")
				unit = CTextureUnitSetup::IMAGE_UNIT_8;
			else if (tunit == "Unit9")
				unit = CTextureUnitSetup::IMAGE_UNIT_9;
			else if (tunit == "Unit10")
				unit = CTextureUnitSetup::IMAGE_UNIT_10;
			else if (tunit == "Unit11")
				unit = CTextureUnitSetup::IMAGE_UNIT_11;
			else if (tunit == "Unit12")
				unit = CTextureUnitSetup::IMAGE_UNIT_12;
			else if (tunit == "Unit13")
				unit = CTextureUnitSetup::IMAGE_UNIT_13;
			else if (tunit == "Unit14")
				unit = CTextureUnitSetup::IMAGE_UNIT_14;
			else if (tunit == "Unit15")
				unit = CTextureUnitSetup::IMAGE_UNIT_15;
		}
		else if (data == "texname")
            io >> textureName;
		else if (data == "TextureFunction")
		{
			std::string function;
			io >> function;

			if (function == "Opaque")
				unitFunction = CTextureUnitSetup::CGL_OPAQUE;
			else if (function == "Multiply")
				unitFunction = CTextureUnitSetup::CGL_MULTIPLY;
			else if (function == "AlphaTransparent")
				unitFunction = CTextureUnitSetup::CGL_ALPHA_TRANSPARENT;
			else if (function == "ConstantBlended")
				unitFunction = CTextureUnitSetup::CGL_CONSTANT_BLENDED;
			else
				unitFunction = CTextureUnitSetup::CGL_NONE;
		}
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
			ITextureObject *pImage = pSet->getTexture(textureName);
			setMap(pImage, unit, unitFunction);
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
    while (io.hasMoreValues())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "Map")
			importMap(io);
		else
			io >> name;
		
		data = io.getValueName();
	}

	io >> name;

	return true;
}



