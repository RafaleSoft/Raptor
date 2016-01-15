// PerlinNoise.h: interface for the CPerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

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
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CPerlinNoise : public ITextureGenerator  
{
public:
	typedef CTextureFactoryConfig::IImageOP::OP_KIND OPS;

    //! The constructor specifies the texture model for this static generator.
	CPerlinNoise(const CVaArray<OPS>& ops = CVaArray<OPS>());
	virtual ~CPerlinNoise();

    //! Implementes a static texture generator
    //! @see base class CTextureGenerator
    virtual ITextureGenerator::GENERATOR_KIND    getKind(void) const { return ITextureGenerator::STATIC; };

    //! This method 'loads' the texture with the generated noise
    //! @see base class CTextureGenerator
    virtual void glGenerate(CTextureObject* );

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const;


protected:
    float noise(float x, float y, float z);

private:
    CVaArray<OPS> m_ImageOps;
    unsigned int *m_iPermutation;

    GL_COORD_VERTEX G[16];

    float grad(unsigned int hash, float x, float y, float z);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_)

