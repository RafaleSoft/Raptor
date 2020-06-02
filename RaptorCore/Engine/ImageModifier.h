// ImageModifier.h: interface for the CImageModifier class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_IMAGEMODIFIER_H__98DC74E1_150B_449D_8BA0_C9CB8F9AEA3D__INCLUDED_)
#define AFX_IMAGEMODIFIER_H__98DC74E1_150B_449D_8BA0_C9CB8F9AEA3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif
#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
    #include "GLHierarchy/TextureGenerator.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;


class RAPTOR_API CImageModifier : public CModifier, public ITextureGenerator
{
public:
    //! Predefined image modifier function kind
	typedef enum
	{
		CGL_MOTIONFADER_MODIFIER,
		CGL_BLOWFADER_MODIFIER,
		CGL_SPINFADER_MODIFIER,
		CGL_STATICFADER_MODIFIER
	} RENDERING_MODIFIER;

	//!		Image modifier stock functions
	//!	Rq : the height passed to this method should
	//!	be 2 pixels less than the real height of both
	//!	src & dst to avoid border cross issues
	typedef void (RAPTOR_CCALL *MODIFIER_STOCK_FNC)(int width,
                                                int height,
                                                unsigned char *src,
                                                unsigned char *dst,
                                                unsigned long dwParam);


public:
	CImageModifier(TIME_FUNCTION_TYPE timeFunction = CModifier::CGL_TIME_CONSTANT,
				              float			timeArg0 = 0,
				              float			timeArg1 = 0,
				              float			timeArg2 = 0,
				              float			timeArg3 = 0,
                              USER_FUNCTION            pUserFunction = NULL,
				              const std::string& name = "GL_IMAGEMODIFIER");
	virtual ~CImageModifier();

	//!	Base implementation
    virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_IMAGE_MODIFIER; };

    //!	Base implementation
    virtual ITextureGenerator::GENERATOR_KIND    getKind(void) const { return ITextureGenerator::ANIMATED; };

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const;


	//!	Select the image to which the time image modifier will apply
	bool setImage(const ITextureObject *simage);

	//!	Select the function to apply, by user pointer or using
	//!	one of the stock modifiers ( see CGLTypes.h )
	bool selectModifierFunction(MODIFIER_STOCK_FNC fnc,unsigned long dwParam);
	bool selectModifierFunction(RENDERING_MODIFIER fnc,unsigned long dwParam);

	//!	Called by the framework to prepare modifier buffers
	//!	before modifiers are applied
	virtual void onPrepareBuffer(void) {};

	//! Inherited from CPersistence
	DECLARE_CLASS_ID(CImageModifierClassID,"ImageModifier",CModifier)


protected:
	//!	Compute physics resultant after dt time increment
	//!	This method should not be called directly. Any
	//!	required user actions should performed in onPrepareBuffer
	//!	method by derivation
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

    //!	Returns the front frame buffer 
	//!	( src buffer that will be used for modification  )
	unsigned char * const getFrontBuffer(void) const { return m_pSrcBuffer; };
	//!	( dst buffer that will be used as next frame source )
	unsigned char * const getBackBuffer(void) const { return m_pDstBuffer; };



private:    
    //! Implements base class
    virtual void glGenerate(ITextureObject* t, uint32_t x, uint32_t y, uint32_t w, uint32_t h);


	MODIFIER_STOCK_FNC	m_pfnModifier;
	unsigned long		m_dwFncParam;

	ITextureObject		*m_pImage;
    unsigned char       *m_pBufferImage;
	unsigned char       *m_pBufferPointer;

	unsigned char		*m_pSrcBuffer;
	unsigned char		*m_pDstBuffer;

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_IMAGEMODIFIER_H__98DC74E1_150B_449D_8BA0_C9CB8F9AEA3D__INCLUDED_)

