// RaptorDisplayFilter.h: interface for the CRaptorDisplayFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
#define AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "CGLTypes.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "GLHierarchy/ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CShader;
class ITextureGenerator;
class CTextureObject;
class CTextureSet;
class CTextureQuad;



//! This class manages filtering process of a display.
//! A filter has a color & depth source that can come from a previous filter,
//!  as well as color & depth output that can be given to the next filter.
//! Thus, it is a filtering chain element.
class RAPTOR_API CRaptorDisplayFilter : public CObjectReference
{
public:
	typedef enum
	{
		RENDER_TEXTURE,
		RENDER_BUFFER
	} FILTER_MODEL;

	//! Initialise the filter. Must be implemented, and 
    //! implemention must call-back this initialiser.
    virtual bool glInitFilter(void);

    //! Release the filter resources allocated in glInitFilter
    //! Implementation must call-back the releaser.
    virtual void glDestroyFilter(void);



	//!	Return the filter model describing the filter processing
	CRaptorDisplayFilter::FILTER_MODEL getFilterModel(void) const 
	{ return m_fModel; };

	//!	Set the new filtering processing model.
	//! @return the previous filtering model.
	CRaptorDisplayFilter::FILTER_MODEL setFilterModel(CRaptorDisplayFilter::FILTER_MODEL model) 
	{ FILTER_MODEL oldModel = m_fModel; m_fModel = model; return oldModel; };

    //! Enables the filter. If true, the filter will process the current frame normally.
    //! Is false, the filter is skipped, the next filter ( if any ) is used.
    void enableFilter(bool enable) { m_bEnabled = enable; };

	//! Enables the filter rendering in an output buffer 
	//! (can be a texture or a render buffer, depending on FILTER_MODEL).
	void enableBufferOutput(bool enable) { m_bBufferedOutputEnabled = enable; };

    //! Returns the enable state.
	//! If filter is not enabled, the previous output is simply rendered
	//! for the next input.
    bool isEnabled(void) const { return m_bEnabled; };

    //! Updates dimension factors in both directions
    //! @return true if factors are updated, otherwise returns false if error.
    bool setSizeFactors(float xFactor, float yFactor);

    //! Generic rendering entry point
    void glRender(void);

	//! Link to the previous filter
	//! (use previous output as current input)
	void setPreviousFilter(CRaptorDisplayFilter *pFilter);



	//!
	//!	RENDER_BUFFER Model : each filter creates an output
	//!	that is a static input for next filter.
	//!

	//! Assign the color input on which the filter has to proceed
    void setColorInput(CTextureObject* pInput);

	//! @Return : the input colortexture, can be from previous filter. 
    CTextureObject*  getColorInput(void);

	//! Assign the depth input on which the filter has to proceed
    void setDepthInput(CTextureObject* pInput);

	//! @Return : the output colortexture from the source generator. 
    virtual CTextureObject*  glCreateColorOutput(void);

	//! @Return : the output colortexture from the source generator.
	//!	(output can be a previous filter output if filter is disabled)
    CTextureObject*  getColorOutput(void);

	//! @Return : the input colortexture from the source generator. 
    virtual CTextureObject*  glCreateDepthOutput(void);



	//!
	//!	RENDER_TEXTURE Model : each filter creates a source
	//!	that is consumed on demand by the next filter.
	//!

	//! Assign the color source on which the filter has to proceed
    void setColorSource(ITextureGenerator* pSource);

    //! Assign the depth source on which the filter has to proceed.
    //! This source depends strongly on hardware capabilities for good performance,
    //! local memory blocs 
    void setDepthSource(ITextureGenerator* pSource);

    //! Create an output texture generator. This output
    //! can then be given as the color source of the next filter.
    //! The default computes a color buffer matching colorSource config
    //! (@see CRaptorDisplayConfig )
    virtual ITextureGenerator*  glCreateColorSource(void);

    //! Create an output texture generator. This output
    //! can then be given as the depth source of the next filter.
    //! The default returns depthSource, user filters must return a valid value.
    virtual ITextureGenerator*  glCreateDepthSource(void);

    //! Returns a generic pointer on the objet.
	//! This is handle can be used for binding to a display.
    virtual operator RAPTOR_HANDLE() const;



protected:
    CRaptorDisplayFilter();
	virtual ~CRaptorDisplayFilter();

	//!	Physical filter rendering. 
	//!	Must be implemented by filters implementation.
	virtual void glRenderFilter(void) = 0;

	//!	Rendering of the final stage of the filter. 
	//!	Must be implemented by filters implementation.
	virtual void glRenderFilterOutput(void) = 0;

    //! Call this method to render the buffer as a full quad covering the entire buffer surface.
    //! User can draw the filter any other way, this method is only a helper
    void glDrawBuffer(void);

    //! Enable state of this filter
    bool			m_bEnabled;
	bool			m_bBufferedOutputEnabled;
	FILTER_MODEL	m_fModel;

    //! Scale factors between source buffer and this filter.
    float   m_fXfactor;
    float   m_fYfactor;

    RAPTOR_HANDLE	drawBuffer;
	CTextureQuad	*m_pDrawBuffer;
    
    CShader     *pFilter;

	CRaptorDisplayFilter	*m_pPreviousFilter;

    ITextureGenerator	*colorExternalSource;
    ITextureGenerator	*depthExternalSource;
    ITextureGenerator	*colorInternalSource;
    ITextureGenerator	*depthInternalSource;

	CTextureObject		*colorInput;
	CTextureObject		*depthInput;
	CTextureObject		*colorOutput;
	CTextureObject		*depthOutput;

	CTextureSet			*m_pRenderTextures;
	CTextureSet			*m_pOutputTextures;

private:
    CRaptorDisplayFilter(const CRaptorDisplayFilter& ) {};
    CRaptorDisplayFilter& operator=(const CRaptorDisplayFilter& ) { return *this; };
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)

