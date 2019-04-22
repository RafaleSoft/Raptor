// RaptorFilteredDisplay.h: interface for the CRaptorFilteredDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORFILTEREDDISPLAY_H__CF464057_8489_43FF_954F_52ECFD722D13__INCLUDED_)
#define AFX_RAPTORFILTEREDDISPLAY_H__CF464057_8489_43FF_954F_52ECFD722D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)
	#include "RaptorScreenDisplay.h"
#endif


RAPTOR_NAMESPACE_BEGIN

//class CFragmentShader;
class CTextureSet;
class CVertexShader;
class CRaptorDisplayFilter;
class CTextureQuad;


class CRaptorFilteredDisplay : public CRaptorScreenDisplay
{
public:
	CRaptorFilteredDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorFilteredDisplay();

	//!	This method can bind the current display to a classic device context
	//!	as if it were a CRaptorScreenDisplay. This method can also bind self
	//!	display to CRaptorDisplayFilter classes, which are rendered in sequence
	//!	when glRender is called. When a filter is bound, the filter is initialised
	//!	and its reference count is incremented, and then decremented when the 
	//! display is destroyed ( or when its resources are released ). 
	//! The user can then bind a filter to this display
	//!	and release the reference count of the filter, the display will destroy the 
	//!	filter when it is no more needed.
	virtual bool glvkBindDisplay(const RAPTOR_HANDLE& device);

	//! see base class
	virtual bool glvkUnBindDisplay(void);

	//!	Renders the display, if it has been bound.
	//! Returns true if rendered without errors, false otherwise.
	virtual bool glRender(void);

	//! Applies the viewpoint to the buffer image,
	//! leaving the filter display unmodifiable because
	//! it only renders an flat image.
	virtual void setViewPoint(IViewPoint *viewPoint);

	virtual void glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy);

	//!	Returns the buffer display.
	virtual IViewPoint *const getViewPoint(void) const;

    //! Returns the rendering properties of the 'drawing display'
    //! ( if it were not overloaded, it would return the properties of the 'filter drawing'
    //! which must not be changed by user calls )
	virtual IRenderingProperties &getRenderingProperties(void) const;

	//! @see CRaptorDisplay
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const;

private:
    //! Inherited from CPersistence : handles m_pImage destruction
    void unLink(const CPersistence* obj);

    //! Inherited from CRaptorScreenDisplay
	virtual void glRenderScene();

    //! Inherited from CRaptorScreenDisplay
    virtual bool glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const;

    //! Inherited from CRaptorScreenDisplay
    virtual void glReleaseResources(void);

    //! Internal for private use only.
    bool glCreateRenderDisplay(void);


	CRaptorDisplay	*m_pDisplay;
    CRaptorDisplay	*m_pFSAADisplay;

    RAPTOR_HANDLE	drawBuffer;
	CTextureQuad	*m_pDrawBuffer;

	bool			m_bBufferBound;
	CTextureSet		*m_pImageSet;
	vector<CRaptorDisplayFilter*> m_pFilters;

    //! specific attributes are needed because buffered
    //! and screen attributes must be managed simultanously
    CRaptorDisplayConfig	filter_cs;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORFILTEREDDISPLAY_H__CF464057_8489_43FF_954F_52ECFD722D13__INCLUDED_)

