// ShaderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GLObjectViewer.h"
#include "ShaderDialog.h"
#include "MainFrm.h"


#include "GLHierarchy/Persistence.h"
#include "GLHierarchy/Material.h"

RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CShaderDialog dialog


CShaderDialog::CShaderDialog()
	: CDialog(),m_pShader(NULL)
{
	//{{AFX_DATA_INIT(CShaderDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CShaderDialog::refreshShaders(void)
{
	CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
	shaders->ResetContent();
	void *pos = NULL;

	CPersistence *obj = CPersistence::Object(pos);
	while (obj != NULL)
	{
		if (obj->getId().isSubClassOf(CPersistence::CPersistenceClassID::GetClassId()))
		{
			int idx = shaders->AddString(CA2T(obj->getName().c_str()));
			shaders->SetItemDataPtr(idx,obj);
		}
		obj = CPersistence::Object(pos);
	}

	int idx = shaders->GetCurSel();
	if (idx < 0)
		m_pShader = NULL;
	else
		m_pShader = (CShader*)shaders->GetItemDataPtr(idx);
}

void CShaderDialog::setValue(int nID,float value)
{
	char buffer[8];

	CEdit *pEdit = ((CEdit*)GetDlgItem(nID));
	if (value >= 0.0f)
	{
		sprintf(buffer,"%d",(int)(255 * value));
		pEdit->SetWindowText(CA2T(buffer));
	}
	else
		pEdit->SetWindowText(TEXT(""));
}

void CShaderDialog::getValue(int nID,float &value)
{
	TCHAR buffer[8];

	CEdit *pEdit = ((CEdit*)GetDlgItem(nID));
	pEdit->GetWindowText(buffer,8);
	value = atof(CT2A(buffer)) / 255.0f;

	if (value < 0.0f)
		value = 0.0f;
}


void CShaderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShaderDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShaderDialog, CDialog)
	//{{AFX_MSG_MAP(CShaderDialog)
	ON_CBN_EDITCHANGE(IDC_SHADERLIST, OnEditchangeShaderlist)
	ON_CBN_SELCHANGE(IDC_SHADERLIST, OnSelchangeShaderlist)
	ON_BN_CLICKED(IDC_MODIFYSHADER, OnModifyShader)
	ON_BN_CLICKED(IDC_COLORAMBIENT, OnColorambient)
	ON_BN_CLICKED(IDC_COLORDIFFUSE, OnColordiffuse)
	ON_BN_CLICKED(IDC_COLORSPECULAR, OnColorspecular)
	ON_BN_CLICKED(IDC_DIFFUSEMAP, OnDiffusemap)
	ON_BN_CLICKED(IDC_LIGHTMAP, OnLightmap)
	ON_BN_CLICKED(IDC_ENVIRONMAP, OnEnvironmap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShaderDialog message handlers
void CShaderDialog::OnEditchangeShaderlist() 
{
	// validate & update current datas ?
}

void CShaderDialog::OnSelchangeShaderlist() 
{
	CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
	int idx = shaders->GetCurSel();
	m_pShader = (CShader*)shaders->GetItemDataPtr(idx);

	CColor::RGBA baseColor = m_pShader->getColor();
	setValue(IDC_BASERED,baseColor.r);
	setValue(IDC_BASEGREEN,baseColor.g);
	setValue(IDC_BASEBLUE,baseColor.b);
	setValue(IDC_BASEALPHA,baseColor.a);

	if (m_pShader->hasMaterial())
	{
		CMaterial *pMaterial = m_pShader->getMaterial();
		CColor::RGBA ambient = pMaterial->getAmbient();
		setValue(IDC_AMBIENTRED,ambient.r);
		setValue(IDC_AMBIENTGREEN,ambient.g);
		setValue(IDC_AMBIENTBLUE,ambient.b);
		setValue(IDC_AMBIENTALPHA,ambient.a);

		CColor::RGBA diffuse = pMaterial->getDiffuse();
		setValue(IDC_DIFFUSERED,diffuse.r);
		setValue(IDC_DIFFUSEGREEN,diffuse.g);
		setValue(IDC_DIFFUSEBLUE,diffuse.b);
		setValue(IDC_DIFFUSEALPHA,diffuse.a);

		CColor::RGBA specular = pMaterial->getSpecular();
		setValue(IDC_SPECULARRED,specular.r);
		setValue(IDC_SPECULARGREEN,specular.g);
		setValue(IDC_SPECULARBLUE,specular.b);
		setValue(IDC_SPECULARALPHA,specular.a);
	}
}

void CShaderDialog::OnModifyShader(void)
{
	if (m_pShader != NULL)
	{
		CComboBox* shaders = ((CComboBox*)GetDlgItem(IDC_SHADERLIST));
		TCHAR buffer[MAX_PATH];
		shaders->GetWindowText(buffer,MAX_PATH);
		string newName = CT2A(buffer);
		if (!newName.empty() && newName.compare(m_pShader->getName()))
		{
			m_pShader->setName(newName);
			CMainFrame *parent = (CMainFrame*)(AfxGetMainWnd());
			parent->updateTools(NULL);
		}

		CColor::RGBA baseColor;
		getValue(IDC_BASERED,baseColor.r);
		getValue(IDC_BASEGREEN,baseColor.g);
		getValue(IDC_BASEBLUE,baseColor.b);
		getValue(IDC_BASEALPHA,baseColor.a);

		CMaterial *pMaterial = m_pShader->getMaterial();
		CColor::RGBA ambient;
		getValue(IDC_AMBIENTRED,ambient.r);
		getValue(IDC_AMBIENTGREEN,ambient.g);
		getValue(IDC_AMBIENTBLUE,ambient.b);
		getValue(IDC_AMBIENTALPHA,ambient.a);
		pMaterial->setAmbient(ambient);

		CColor::RGBA diffuse;
		getValue(IDC_DIFFUSERED,diffuse.r);
		getValue(IDC_DIFFUSEGREEN,diffuse.g);
		getValue(IDC_DIFFUSEBLUE,diffuse.b);
		getValue(IDC_DIFFUSEALPHA,diffuse.a);
		pMaterial->setDiffuse(diffuse);

		CColor::RGBA specular;
		getValue(IDC_SPECULARRED,specular.r);
		getValue(IDC_SPECULARGREEN,specular.g);
		getValue(IDC_SPECULARBLUE,specular.b);
		getValue(IDC_SPECULARALPHA,specular.a);
		pMaterial->setSpecular(specular);

		CMainFrame *parent = (CMainFrame*)(AfxGetMainWnd());
		parent->redrawScene();
	}
}

void CShaderDialog::OnColorambient() 
{
	// TODO: Add your control notification handler code here
	
}

void CShaderDialog::OnColordiffuse() 
{
	// TODO: Add your control notification handler code here
	
}

void CShaderDialog::OnColorspecular() 
{
	// TODO: Add your control notification handler code here
	
}

void CShaderDialog::OnDiffusemap() 
{
	// TODO: Add your control notification handler code here
	
}

void CShaderDialog::OnLightmap() 
{
	// TODO: Add your control notification handler code here
	
}

void CShaderDialog::OnEnvironmap() 
{
	// TODO: Add your control notification handler code here
	
}
