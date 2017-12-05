#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
#define AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "System/RaptorDisplay.h"
#endif

#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "System/RenderEntryPoint.h"
#endif

RAPTOR_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CGLWnd window

class RAPTOR_API CGLWnd :	public CWnd, public CRenderEntryPoint
{
// Construction
public:
	CGLWnd(CString name = "GLWND");
	virtual ~CGLWnd() {};

	//! Returns the display associated with this view.
	//! The display is only valid if the window has properly
	//! been created.
	const CRaptorDisplay* const getDisplay(void) const { return m_pDisplay; };


	//! Creates a valid CGLWnd to display openGL.
	//! This is the only method that should be called to
	//! create the CGLWnd object as a GL display.
	bool GLCreateWindow (	CString name,CWnd *parent,
											const CRaptorDisplayConfig& glCreateStruct);

	//!	Initialises the GL context owned by this object.
	//!	Ensures that the current context is valid during initialisation
	//!	Other inits ( texture, extensions, ... ) in other functions might
	//!	not be done correctly if the current context is not selected ( glMakeCurrent )
	virtual	void GLInitContext(void);

	//!	Function for OpenGL display, called by Render
	//!	Shouldn't be called directly
	virtual void GLDisplayFunc(void);

	virtual void glMakeCurrent(bool restoreContext = true);
	virtual void glRender(void);

	//!	Stores a display config to be used as default when needed ( such as window creation ... )
	static void SetDefaultDisplayConfig(const CRaptorDisplayConfig &pcs);

	//!	Returns the display config set as default here above
	static const CRaptorDisplayConfig &GetDefaultDisplayConfig(void);


protected:
	//	system colors
	float				SystemRed,SystemGreen,SystemBlue;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CGLWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
private:
	// opengl display
	CRaptorDisplay	*m_pDisplay;
};

RAPTOR_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
