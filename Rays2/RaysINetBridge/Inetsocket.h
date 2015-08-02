#if !defined(AFX_INETSOCKET_H__ECC9CF5B_CFC4_429B_90F8_5CCAE7120A99__INCLUDED_)
#define AFX_INETSOCKET_H__ECC9CF5B_CFC4_429B_90F8_5CCAE7120A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Inetsocket.h : header file
//


class CRaysINetBridgeDlg;

/////////////////////////////////////////////////////////////////////////////
// CInetSocket command target

class CInetSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CInetSocket(CRaysINetBridgeDlg *m_pBride);
	virtual ~CInetSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInetSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CInetSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
private:
	unsigned char buffer[1024];

	CRaysINetBridgeDlg *m_pBride;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INETSOCKET_H__ECC9CF5B_CFC4_429B_90F8_5CCAE7120A99__INCLUDED_)
