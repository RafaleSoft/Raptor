// MagnifierFilter.h: interface for the CMagnifierFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGNIFIERFILTER_H__3660D446_2F92_4D02_A795_BFF8336D61D2__INCLUDED_)
#define AFX_MAGNIFIERFILTER_H__3660D446_2F92_4D02_A795_BFF8336D61D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif
#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif

RAPTOR_NAMESPACE

//! This class implements a bicubic image magnifier.
//! It can be used to antialias upscaled rendering, though it is not an antialiasing filter. 
//! All kernels are separable and are applyied to a 4x4 texels region.
class RAPTOR_API CMagnifierFilter : public CRaptorDisplayFilter  
{
public:
	CMagnifierFilter();

    //! Implements base class
    virtual bool glInitFilter(void);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

    //!
    //! These functions select a filtering kernel
    //!
    //! A cubic sinc approximation
    void setSincKernel(float A=-0.75f);
    //! A mitchell-netravali reconstruction filter.
    //! use: B=1, C=0 for a cubic spline;
    //! use: B=1/3, C=1/3 is recommended
    //! use: B=0, C=1/2 for a Catmull-Rom Spline
    void setMitchellNetravaliKernel(float B=0.33f, float C=0.33f);


private:
	//! Forbidden destructor
	virtual ~CMagnifierFilter();

    //! Implements base class
    virtual void glRenderFilter();

	//! Implements base class
	virtual void glRenderFilterOutput(void);

	//! Default available kernels: sinc
    float sincKernel(float x,float A,float ) const;

	//! Default available kernels: Mitchell & Netravelli
    float mitchellNetravaliKernel(float x,float B,float C) const;

	//!	Rebuild the kernel data in an RGBA texture.
	void computeKernel(void);

	bool			m_bRebuild;
    CTextureObject	*kernelTexture;

    CRaptorDisplay	*xBuffer;
    CTextureObject	*xKernelPass;
    CShader			*m_pXKernelShader;
    CShader			*m_pYKernelShader;

    GL_COORD_VERTEX	kernelParams;
	CShaderProgram::CProgramParameters v_params_x;
	CShaderProgram::CProgramParameters v_params_y;
	CShaderProgram::CProgramParameters f_params;

    float (CMagnifierFilter::*kernelBuilder)(float x,float k1,float k2) const;
};

#endif // !defined(AFX_MAGNIFIERFILTER_H__3660D446_2F92_4D02_A795_BFF8336D61D2__INCLUDED_)
