// PropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GLObjectViewer.h"
#include "PropertyDialog.h"
#include "MainFrm.h"

#include "GLHierarchy/Persistence.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"

RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog dialog


CPropertyDialog::CPropertyDialog(CWnd* pParent /*=NULL*/)
	:m_pObject(NULL),propertiesUpdated(false)
{
	//{{AFX_DATA_INIT(CPropertyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    
}

void CPropertyDialog::clear(void)
{
	((CEdit*)GetDlgItem(IDC_NAME))->SetWindowText(TEXT(""));
	((CComboBox*)GetDlgItem(IDC_SHADERLIST))->SetCurSel(-1);
	((CComboBox*)GetDlgItem(IDC_SHADERLIST))->ResetContent();
	((CButton*)GetDlgItem(IDC_VISIBLE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_TRANSPARENT))->SetCheck(BST_UNCHECKED);

	propertiesUpdated = false;
}

void CPropertyDialog::updateShaders(void)
{
	CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
	void *pos = NULL;

	CPersistence *obj = CPersistence::Object(pos);
	while (obj != NULL)
	{
		if (obj->getId().isSubClassOf(CShader::CShaderClassID::GetClassId()))
		{
			int idx = shaders->AddString(CA2T(obj->getName().c_str()));
			shaders->SetItemDataPtr(idx,obj);
		}
		obj = CPersistence::Object(pos);
	}
}

void CPropertyDialog::setObject(CObject3D *obj)
{
    m_pObject = obj;

	if (obj != NULL)
	{
		((CEdit*)GetDlgItem(IDC_NAME))->SetWindowText(CA2T(obj->getName().c_str()));

		CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
		if (obj->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
		{
			CShadedGeometry *g = (CShadedGeometry*)obj;
			if (g->hasShader())
			{
				CShader *shader = g->getShader();
				int idx = shaders->FindString(-1,CA2T(shader->getName().c_str()));
				shaders->SetCurSel(idx);
			}
			else
				shaders->SetCurSel(-1);
		}
		else
			shaders->SetCurSel(-1);
		
		const CObjectProperties& props = obj->getProperties();
		((CButton*)GetDlgItem(IDC_VISIBLE))->SetCheck(props.isVisible() ? BST_CHECKED : BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_TRANSPARENT))->SetCheck(props.isTransparent() ? BST_CHECKED : BST_UNCHECKED);

		propertiesUpdated = false;
	}
	else
		clear();
}



void CPropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyDialog, CDialogBar)
	//{{AFX_MSG_MAP(CPropertyDialog)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_CBN_EDITCHANGE(IDC_SHADERLIST, OnEditchangeShaderlist)
	ON_BN_CLICKED(IDC_VISIBLE, OnVisible)
	ON_BN_CLICKED(IDC_TRANSPARENT, OnTransparent)
	ON_CBN_SELCHANGE(IDC_SHADERLIST, OnSelchangeShaderlist)
	ON_BN_CLICKED(IDC_EDITOR, OnEditor)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(IDC_APPLY, OnUpdateProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog message handlers

void CPropertyDialog::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(propertiesUpdated ? TRUE : FALSE);
}

void CPropertyDialog::OnApply() 
{
	if (m_pObject != NULL)
	{
		CString newName("default");
		((CEdit*)GetDlgItem(IDC_NAME))->GetWindowText(newName);
		string oldname = m_pObject->getName();
		string new_name = std::string(CT2A(newName));
		if (oldname.compare(new_name))
			m_pObject->setName(new_name);

		CObjectProperties& props = m_pObject->getProperties();
		int check = ((CButton*)GetDlgItem(IDC_VISIBLE))->GetCheck();
		props.setVisible((check == BST_CHECKED) ? true : false);

		CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
		int idx = shaders->GetCurSel();
		if (idx >= 0)
		{
			CShader *pShader = (CShader*)shaders->GetItemDataPtr(idx);
			if (m_pObject->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
				((CShadedGeometry*)m_pObject)->setShader(pShader);
		}
		
		propertiesUpdated = false;

		CMainFrame *parent = (CMainFrame*)(AfxGetMainWnd());
		parent->redrawScene();
	}
}

void CPropertyDialog::OnChangeName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogBar::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	propertiesUpdated = true;
}

void CPropertyDialog::OnEditchangeShaderlist() 
{
	// TODO: Add your control notification handler code here
	propertiesUpdated = true;
}

void CPropertyDialog::OnVisible() 
{
	propertiesUpdated = true;
}

void CPropertyDialog::OnTransparent() 
{
	propertiesUpdated = true;
}

void CPropertyDialog::OnSelchangeShaderlist() 
{
	// TODO: Add your control notification handler code here
	propertiesUpdated = true;
}

void CPropertyDialog::OnEditor() 
{
	// TODO: Add your control notification handler code here
	
}
