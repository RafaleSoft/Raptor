#if !defined(AFX_TREEDIALOGBAR_H__D8BCDE5E_1FDD_4631_9C51_7B667E40128A__INCLUDED_)
#define AFX_TREEDIALOGBAR_H__D8BCDE5E_1FDD_4631_9C51_7B667E40128A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeDialogBar.h : header file
//

#include "GLHierarchy/3DSet.h"

RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CTreeDialogBar dialog

class CTreeDialogBar : public CDialogBar
{
// Construction
public:
	CTreeDialogBar(CWnd* pParent = NULL);   // standard constructor

	void setScene(C3DSet *pScene);

// Dialog Data
	//{{AFX_DATA(CTreeDialogBar)
	enum { IDD = IDD_DIALOGTREE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeDialogBar)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTreeDialogBar)
	afx_msg void OnSelchangedObjecttree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addSet(C3DSet *pSet,HTREEITEM location);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEDIALOGBAR_H__D8BCDE5E_1FDD_4631_9C51_7B667E40128A__INCLUDED_)
