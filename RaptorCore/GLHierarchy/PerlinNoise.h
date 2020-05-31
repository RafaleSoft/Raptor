/***************************************************************************/
/*                                                                         */
/*  PerlinNoise.h                                                          */
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


#if !defined(AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_)
#define AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
    #include "GLHierarchy/TextureGenerator.h"
#endif
#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;



class RAPTOR_API CPerlinNoise : public ITextureGenerator  
{
public:
	typedef CImage::IImageOP* OPS;
	typedef enum
	{
		NOISE1,
		NOISE2,
		NOISE3,
		NOISE4
	} NOISE_MODEL;

    //! The constructor specifies the texture model for this static generator.
	CPerlinNoise(const CVaArray<OPS>& ops = CVaArray<OPS>());
	virtual ~CPerlinNoise();

    //! Implementes a static texture generator
    //! @see base class CTextureGenerator
    virtual ITextureGenerator::GENERATOR_KIND    getKind(void) const { return ITextureGenerator::STATIC; };

    //! This method 'loads' the texture with the generated noise
    //! @see base class CTextureGenerator
    virtual void glGenerate(ITextureObject* I, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const;

	//!	Provides noise generation control.
	void setNoiseModel(NOISE_MODEL model) { m_model = model; };

	//!	Configure noise amplitude
	void setAmplitude(float amplitude) { m_amplitude = amplitude; };

	//!	Gnerate a 'tileable' texture (mirrored in u & v)
	void generateMirrorTexture(bool mirror) { m_textureMirror = mirror; };


protected:
    float noise(float x, float y, float z);


private:
    CVaArray<OPS>	m_ImageOps;
    unsigned int	*m_iPermutation;
	NOISE_MODEL		m_model;
	bool			m_textureMirror;
	float			m_amplitude;
	float			m_baseFrequency;

    GL_COORD_VERTEX G[16];

	//!	Compute 2D/3D noise values in texture texels (data)
	void generateNoise(	unsigned char *data,
						unsigned int width,
						unsigned int height,
						unsigned int depth,
						unsigned int transparency);

	//!	Compute gradient : dot on unit cube vector selected by random hash
    float grad(unsigned int hash, float x, float y, float z);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_)

