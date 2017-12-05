#if !defined(AFX_MAINDIALOGBAR_H__F48678E7_A634_4D19_9AED_82E571B03EB7__INCLUDED_)
#define AFX_MAINDIALOGBAR_H__F48678E7_A634_4D19_9AED_82E571B03EB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainDialogBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainDialogBar dialog

class CMainDialogBar : public CDialogBar
{
// Construction
public:
	CMainDialogBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMainDialogBar)
	enum { IDD = IDD_DIALOGBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDialogBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainDialogBar)
	afx_msg void OnBackFaces();
	afx_msg void OnWireframe();
	afx_msg void OnLighting();
	afx_msg void OnTexturing();
	afx_msg void OnBBoxes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDIALOGBAR_H__F48678E7_A634_4D19_9AED_82E571B03EB7__INCLUDED_)
