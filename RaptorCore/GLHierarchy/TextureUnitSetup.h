/***************************************************************************/
/*                                                                         */
/*  TexureUnitSetup.h                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
#define AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;
class CRegisterCombiner;


class RAPTOR_API CTextureUnitSetup : public CPersistence
{
public:
    //! A type to identify a texture image unit ( do not confuse with an texture unit:
    //! a texture object is bound to a texture image unit, whereas a texture unit fetches samples
    //! in a texture image unit )
    //! All values may not be accessible, depending on the hardware : enableImageUnit
    //! wil return false if the unit cannot be accessed
    //! ( @see CTextureFactoryConfig for details)
    typedef enum
    {
        IMAGE_UNIT_0,
        IMAGE_UNIT_1,
        IMAGE_UNIT_2,
        IMAGE_UNIT_3,
        IMAGE_UNIT_4,
        IMAGE_UNIT_5,
        IMAGE_UNIT_6,
        IMAGE_UNIT_7,
        IMAGE_UNIT_8,
        IMAGE_UNIT_9,
        IMAGE_UNIT_10,
        IMAGE_UNIT_11,
        IMAGE_UNIT_12,
        IMAGE_UNIT_13,
        IMAGE_UNIT_14,
        IMAGE_UNIT_15,
    } TEXTURE_IMAGE_UNIT;


	//!
	//!	These features are deprecated since OpenGL 3.0 deprecation model.
	//!	Kept for compatibility purpose, subject to removal in future versions of Raptor.	
	//!
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
	//!	Definition of a texture unit combiner structure. This combiner is applied
	//!	to a texture unit to apply a function on data incoming from the previous texture unit
	typedef struct RAPTOR_API GL_TEXTURE_COMBINER_TAG
	{
		unsigned int	rgb_op;		// combiner operations
		unsigned int	alpha_op;

		unsigned int	src_rgb_0;	// combiner texel source
		unsigned int	src_rgb_1;
		unsigned int	src_rgb_2;
		unsigned int	src_alpha_0;
		unsigned int	src_alpha_1;
		unsigned int	src_alpha_2;

		unsigned int	op_rgb_0;	// combiner texel operands ( extracted from sources )
		unsigned int	op_rgb_1;
		unsigned int	op_rgb_2;
		unsigned int	op_alpha_0;
		unsigned int	op_alpha_1;
		unsigned int	op_alpha_2;

		float			rgb_scale;	// scale result
		float			alpha_scale;

		CColor::RGBA	color;		// TMU constant color

		bool	rgb_combiner;
		bool	alpha_combiner;

		GL_TEXTURE_COMBINER_TAG();
	} GL_TEXTURE_COMBINER;

	
	//!	The concept of texture shader is extended to coordinate
	//!	generation functions. CGL_TEXTURE_GEN_COORD is used as
	//! the the shader operation, each generation model is given
	//!	in genMode as GL_OBJECT_LINEAR,GL_EYE_LINEAR,GL_SPHERE_MAP,
	//! GL_NORMAL_MAP_ARB & GL_REFLECTION_MAP_ARB ( if cube maping )
	//!	The TEXTURE_GEN_PROJECTION mode of the shader generates a
	//!	'projective' function using OGL TexCoord Generation ( the
	//!	texture matrix is updated ).
	#define CGL_TEXTURE_GEN_COORD		GL_TEXTURE_GEN_MODE
	#define CGL_TEXTURE_GEN_PROJECTION	(GL_EYE_PLANE + 1)

	typedef struct RAPTOR_API GL_TEXTURE_SHADER_TAG
	{
		unsigned int	shaderOperation;
		int	            cullMode[4];			//! a cull mode for each coord : S,T,R,Q
		int				genMode[4];				//!	generation function for each coord : S,T,R,Q
		unsigned int	dotProductMapping;
		int				previousInput;
		float			offsetTextureMatrix[4];
		float			textureScale;
		float			textureBias;
		float			constEye[4];

		GL_TEXTURE_SHADER_TAG();
	} GL_TEXTURE_SHADER;
#endif


public:
	//!	Constructor.
	//!	Initial settings:
	//!	- No textures are accessed ( all GL_TEXTURE_OBJECTS are NULL )
	//!	- All texture units are used ( 4 )
	//!	- No combiners are initialised ( initialised with default values but not used )
	//! - No register combiners are used ( no their use is exclusive with basic combiners )
	//! - No fragment programs are enabled
	CTextureUnitSetup();
	virtual ~CTextureUnitSetup();

    //! Returns true is this->class ID is a subclass of id
    //virtual bool isSubClassOf(PERSISTENCE_CLASS_ID id) const;


    //! assignment operator
	const CTextureUnitSetup& operator=(const CTextureUnitSetup& rsh);

    //! Enable or disable the usage of a specific TMU image unit
    //! @param unit : specifies the image unit to modify ( 0 based )
    //! @param enable : if true, enables the unit, disables otherwise.
    //! @return : true is setting is valid, false otherwise ( e.g. invalid unit )
    bool enableImageUnit(TEXTURE_IMAGE_UNIT unit, bool enable);

    //! Enable or disable register combiners usage ( fixed enhanced nVidia OpenGL texture shaders )
    void useRegisterCombiners(bool enable) { use_register_combiners = enable; }; 

    //! Texture objects configuration.
    //! For compatibility with old OpenGL specs, 4 TMU images are associated to 4 TMU units ( & 4 TMU coords )
    void setDiffuseMap(ITextureObject* to);
    ITextureObject* const getDiffuseMap(void) const;

    void setNormalMap(ITextureObject* to);
    ITextureObject* const getNormalMap(void) const;

    void setLightMap(ITextureObject* to);
    ITextureObject* const getLightMap(void) const;

    void setEnvironmentMap(ITextureObject* to);
    ITextureObject* getEnvironmentMap(void) const;


	//!
	//!	These features are deprecated since OpenGL 3.0 deprecation model.
	//!	Kept for compatibility purpose, subject to removal in future versions of Raptor.	
	//!
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)

	//! Returns a fixed pipeline TMU shader for image unit 'unit' ( 0 based )
	GL_TEXTURE_SHADER&   getTMUShader(TEXTURE_IMAGE_UNIT unit);

	//! Returns a fixed pipeline TMU combiner for image unit 'unit' ( 0 based )
	GL_TEXTURE_COMBINER& getTMUCombiner(TEXTURE_IMAGE_UNIT unit);

	//!	Generate a renderable which sets the desired TMU configuration
	//!	If use_register_combiner is true, then nVidia register combiners are used
	//!	Otherwise, Texture combiners are used
	RAPTOR_HANDLE glBuildSetup(void);

	//!	Renders a texture unit combiner separately
	//!	( tmus are supposed to be set properly )
	static void RAPTOR_FASTCALL glRender(GL_TEXTURE_COMBINER *C);

	//!	Renders a texture shader separately and returns the shader consistency
	//!	( tmus are supposed to be set properly )
	static bool RAPTOR_FASTCALL glRender(GL_TEXTURE_SHADER *S);
#else
#endif
	//!	Renders the texture unit setup, binding texture objects to units.
	//!	( Texture generator are NOT called as in glBuildSetup display list: same behavior.
	//!		In order to render the generator bound to the texture, texture object glRender
	//!		must be called explicitely. )
	void glRender();



	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CTextureUnitSetupClassID,"TextureUnitSetup",CPersistence)


private:
	void setMap(ITextureObject *to,TEXTURE_IMAGE_UNIT unit);

    bool importMap(TEXTURE_IMAGE_UNIT unit,CRaptorIO& io);

    //	Register combiners and generic texture combiners are exclusive
	bool				use_register_combiners;
    CRegisterCombiner   *register_combiners;

	unsigned int		nbUnits;
    bool				*useUnit;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
    GL_TEXTURE_SHADER   *tmuShader;
    GL_TEXTURE_COMBINER *tmuCombiner;
#endif
    ITextureObject	    **imageUnit;
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC pfn_glActiveTexture;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)

