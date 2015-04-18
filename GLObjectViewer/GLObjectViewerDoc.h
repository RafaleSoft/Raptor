// GLObjectViewerDoc.h : interface of the CGLObjectViewerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBJECTVIEWERDOC_H__F9318D04_ACA5_4F88_9E3D_4B78E62FE530__INCLUDED_)
#define AFX_GLOBJECTVIEWERDOC_H__F9318D04_ACA5_4F88_9E3D_4B78E62FE530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MFCExtension/CWnd/GLDocument.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Material.h"

#include "RootInstance.h"


RAPTOR_NAMESPACE

class RootInstance;

class CGLObjectViewerDoc : public raptor::CGLDocument
{
protected: // create from serialization only
	CGLObjectViewerDoc();
	DECLARE_DYNCREATE(CGLObjectViewerDoc)

	CObject3DInstance	*getRoot(void) const
	{ return m_pRootInstance; };

	void setBBoxes(bool bboxes);

	void selectObject(CObject3D *pObject);
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLObjectViewerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGLObjectViewerDoc();


// Generated message map functions
protected:
	//{{AFX_MSG(CGLObjectViewerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    virtual	void GLInitContext(void);

    bool		updateView;
    C3DSet		*m_pRoot;
	CRootInstance	*m_pRootInstance;
	CMaterial	*m_pDefaultMaterial;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBJECTVIEWERDOC_H__F9318D04_ACA5_4F88_9E3D_4B78E62FE530__INCLUDED_)
