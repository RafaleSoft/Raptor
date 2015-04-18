// BlurFilter.h: interface for the CBlurFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLURFILTER_H__46C7BB3A_6996_4E57_A387_A56F89777EF9__INCLUDED_)
#define AFX_BLURFILTER_H__46C7BB3A_6996_4E57_A387_A56F89777EF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE


//! This class implements a gaussian blur filter.
//! The width & heigth of the blur can be controlled as well as the 
//! blur curve, but no radial effects can be applied because the filter
//! must have a separable kernel to be computed efficiently.
class RAPTOR_API CBlurFilter : public CRaptorDisplayFilter  
{
public:
	typedef enum
	{
		BLUR_BOX,
		BLUR_BOX_LINEAR,
		BLUR_GAUSSIAN,
		BLUR_GAUSSIAN_LINEAR
	} BLUR_MODEL;

	CBlurFilter();

    //! Implements base class
    virtual bool glInitFilter(void);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

	//!	Defines or updates the kernel size of the blur.
	//!	Default size is 8, max size is 64. 
	//! If size is invalid, blur size is defaulted to 8.
	void setBlurSize(unsigned int size);

	//!	Returns the size of the filter in pixels.
	void getBlurSize(unsigned int &s)
	{ s = m_size; }

	//! Defines the blur model
	void setBlurModel(BLUR_MODEL model);

	//! Returns the blur model
	void getBlurModel(BLUR_MODEL &m)
	{ m = m_model; }


private:
	//!	Forbidden destructor
	virtual ~CBlurFilter();

	//! Implements base class
    virtual void glRenderFilter();

	//! Implements base class
	virtual void glRenderFilterOutput(void);

	//!	Rebuild the blur filter to take into account
	//!	new size
	bool glBuildFilter(int width,int height);

    CTextureObject	*xKernelPass;
    CShader			*hBlur;			// horizontal blur shader
	CShader			*vBlur;			// vertical blur shader
    CRaptorDisplay	*xBuffer;
	unsigned int	m_size;
	bool			m_bRebuild;
	vector<float>	m_gaussian_coeffs;
	vector<float>	m_gaussian_offsets;
	BLUR_MODEL		m_model;
    CShaderProgram::CProgramParameters	params;
};

#endif // !defined(AFX_BLURFILTER_H__46C7BB3A_6996_4E57_A387_A56F89777EF9__INCLUDED_)

