// BWFilter.h: interface for the CBWFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_)
#define AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif


RAPTOR_NAMESPACE

//! This class implements a monochrome filter. The default visual aspect is a
//! Black & White filter like windows logout. The percentage of filter controls
//! a linear interpolation between the unmodified source and pure black & white.
//! A specific color can also be specified, to procude effects like scepia and others.
class RAPTOR_API CColorControlFilter : public CRaptorDisplayFilter  
{
public:
	CColorControlFilter();

    //! Implements base class
    virtual bool glInitFilter(void);

	//!	Color blending function.
    //! @param rgba : Defines a fundamental color for a blending effect, the default of rgba is white for B&W aspect.
	//! @param percentage : defines the percentage of B&W of the filter, the remaining part is the original color (Valid values are [0.0 .. 1.0])
    void setColorBlend(float r,float g,float b,float a,float percentage);

	//! Defines brightness and saturation output.
    //! The default is white for B&W aspect.
    void setCorrection(float brightness,float saturation);

	//! Returns color control parameters.
    void getCorrection(float &brightness,float &saturation);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

private:
	//! Forbidden destructor
	virtual ~CColorControlFilter();

	//! Implements base class
    virtual void glRenderFilter(void);

	//! Implements base class
	virtual void glRenderFilterOutput(void);

    CShader			*BWShader;
	CProgramParameters	fp_params;
};

#endif // !defined(AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_)
