#if !defined(AFX_GLDOCUMENT_H__248DDDD2_9C7C_11D3_9856_A83014000000__INCLUDED_)
#define AFX_GLDOCUMENT_H__248DDDD2_9C7C_11D3_9856_A83014000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "System/RenderEntryPoint.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CGLView;

/////////////////////////////////////////////////////////////////////////////
// CGLDocument document
class RAPTOR_API CGLDocument :	public CDocument,
								public CRenderEntryPoint
{
public:
	virtual ~CGLDocument();

	//!	This method notifies all GLViews attached to this document
	//!	that a rendering is required. This method is not usually called
	//!	directly, indeed it is preferable to call glPostRedisplay() from
	//!	the main application.
	//!	This method is local and DO NOT HANDLE automatically animator & 
	//!	3D engine pipeline processing ( implements CRenderEntryPoint )
	virtual void glRender(void);

	//!	Initialises the GL context owned by this object.
	//!	Ensures that the current context is valid during initialisation
	//!	Other inits ( texture, extensions, ... ) in other functions might
	//!	not be done correctly if the current context is not selected ( glMakeCurrent )
	//!	This method is supposed to be called only ONCE !
	virtual	void GLInitContext(void);

	//!	Function for OpenGL display, called by Render
	//!	Shouldn't be called directly
	virtual void GLDisplayFunc(int viewID);


	//! Attach a view to this document
	//! @return : the view id of the new attachment
	int attachView(const CGLView* view);

	//!	Returns the number of GLViews that will render this GLDocument
	int GetNbViews(void) const { return m_nbAttachedViews; };

	//! Returns initialisation state
	bool isInitialised(void) const { return m_initialised; };

	//! Initialise the document, issuing at least a call to GLInitcontext if necessary.
	//! This method shall be prefered to a direct call to GLInitContext since additional
	//! data may need to be initialised in the future.
	//! @return : false if already initialised.
	bool glInitialize(void);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLDocument)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CGLDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//	Construction/Destruction
	CGLDocument(CString name = "GL_DOCUMENT"); // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGLDocument)

private:
	bool		m_initialised;
	int			m_nbAttachedViews;
};

RAPTOR_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLDOCUMENT_H__248DDDD2_9C7C_11D3_9856_A83014000000__INCLUDED_)
