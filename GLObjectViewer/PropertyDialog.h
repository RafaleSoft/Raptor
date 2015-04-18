#if !defined(AFX_PROPERTYDIALOG_H__CA929CA3_17D7_49FD_8555_54B91D4F4222__INCLUDED_)
#define AFX_PROPERTYDIALOG_H__CA929CA3_17D7_49FD_8555_54B91D4F4222__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GLHierarchy/Object3D.h"

RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog dialog

class CPropertyDialog : public CDialogBar
{
// Construction
public:
	CPropertyDialog(CWnd* pParent = NULL);   // standard constructor

	//!	Remove any property information in the dialog
	void clear(void);

	//!	Updates the property infos with object's datas
	void setObject(CObject3D* obj);

	//!	Performs a reinit of the shaders list
	void updateShaders(void);

// Dialog Data
	//{{AFX_DATA(CPropertyDialog)
	enum { IDD = IDD_DIALOGPROPS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyDialog)
	afx_msg void OnApply();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnChangeName();
	afx_msg void OnEditchangeShaderlist();
	afx_msg void OnVisible();
	afx_msg void OnTransparent();
	afx_msg void OnSelchangeShaderlist();
	afx_msg void OnEditor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CObject3D	*m_pObject;
	bool		propertiesUpdated;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYDIALOG_H__CA929CA3_17D7_49FD_8555_54B91D4F4222__INCLUDED_)
