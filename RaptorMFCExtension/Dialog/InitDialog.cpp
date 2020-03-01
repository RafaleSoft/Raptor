// InitDialog.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_INITDIALOG_H__57A7FAB0_3A1F_11D3_9142_B236E7C6A77C__INCLUDED_)
	#include "InitDialog.h"
#endif
#if !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
	#include "CWnd/GLView.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif


RAPTOR_NAMESPACE


#define GetIntegerv(value,param)\
				glGetIntegerv(##value,##param);\
				text.Format(TEXT("%d"),iparam[0]);\
				Wnd->SetWindowText(text);\
				break;

#define GetFloatv(value,param)\
				glGetFloatv(##value,##param);\
				text.Format(TEXT("%f"),fparam[0]);\
				Wnd->SetWindowText(text);\
				break;

#define GetBooleanv(value,param)\
				glGetBooleanv(##value,##param);\
				Wnd->SetWindowText((bparam[0] ? TEXT("true") : TEXT("false")));\
				break;



/////////////////////////////////////////////////////////////////////////////
// CInitDialog dialog


CInitDialog::CInitDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInitDialog::IDD, pParent),GLContext(NULL)
{
	//{{AFX_DATA_INIT(CInitDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitDialog, CDialog)
	//{{AFX_MSG_MAP(CInitDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitDialog message handlers

BOOL CInitDialog::OnInitDialog() 
{
	CString s;
	CWnd *w;
	GLint iparam = 0;

	CDialog::OnInitDialog();
	
	CComboBox *b = (CComboBox*)this->GetDlgItem(IDC_COMBO1);
	b->SetCurSel(0);

	if (GLContext!=NULL)
		GLContext->glMakeCurrent();
	else
	{
		CRaptorDisplayConfig glCS;
		glCS.width = 0;
		glCS.height = 0;
		glCS.x = 0;
		glCS.y = 0;
		glCS.caption = "Raptor Release test";
		glCS.stencil_buffer = true;
		glCS.double_buffer = true;
		glCS.depth_buffer = true;
		glCS.display_mode = CGL_RGBA | CGL_DEPTH;
		glCS.refresh_rate.fps = 75;
		Context.GLCreateWindow("OpenGL Context",this,glCS);
		Context.glMakeCurrent();
	}

	w=this->GetDlgItem(IDC_VENDOR);
	LPSTR v = CA2T((char*)glGetString(GL_VENDOR));
	s.Format(TEXT("Vendor: %s"),v);
	w->SetWindowText(s);

	w=this->GetDlgItem(IDC_RENDERER);
	LPSTR r = CA2T((char*)glGetString(GL_RENDERER));
	s.Format(TEXT("Renderer: %s"), r);
	w->SetWindowText(s);

	w=this->GetDlgItem(IDC_VERSION);
	LPSTR vv = CA2T((char*)glGetString(GL_VERSION));
	s.Format(TEXT("Version: %s"), vv);
    w->SetWindowText(s);


    w=this->GetDlgItem(IDC_SL_VERSION);
#if defined(GL_ARB_shading_language_100)
	LPSTR g = CA2T((char*)glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));
	s.Format(TEXT("GLSL: %s"), g);
#else
    s = "GLSL: not supported";
#endif
    w->SetWindowText(s);


	w=this->GetDlgItem(IDC_MAXTMU);
#if defined(GL_ARB_multitexture)
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&iparam);
#else
	iparam = 1;
#endif
	s.Format(TEXT("Nb TMU: %d"),iparam);
	w->SetWindowText(s);

	w=this->GetDlgItem(IDC_EXTENSIONS);
	w->SetWindowText(TEXT("Extensions: "));
	CListBox *l = (CListBox*)this->GetDlgItem(IDC_LIST1);

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	CTextureFactoryConfig &config = factory.getConfig();
	w=this->GetDlgItem(IDC_COMPRESSORS);
	iparam = config.getNumCompressors();
	s.Format(TEXT("Compressors: %d"), iparam);
	w->SetWindowText(s);

	for (int i=0;i<iparam;i++)
	{
		s.Format(TEXT("Texture Compressor %d"), i);
		b->AddString(LPCTSTR(s));
	}
	
#if defined(RAPTOR_SMP_CODE_GENERATION)
	this->GetDlgItem(IDC_SMP)->SetWindowText(TEXT("SMP is used"));
#else
	this->GetDlgItem(IDC_SMP)->SetWindowText("SMP is not used");
#endif


    string::size_type pos;
	string extensions = Raptor::glGetExtensions()->glExtensions();
    while ((pos=extensions.find(" ")) != string::npos)
	{
		l->AddString(CA2T(extensions.substr(0,pos).data()));
		extensions.erase(0,pos+1);
	}
	l->AddString(CA2T(extensions.data()));

	CATCH_GL_ERROR
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInitDialog::OnSelchangeCombo1() 
{
	CComboBox *b = (CComboBox*)this->GetDlgItem(IDC_COMBO1);
	CWnd *Wnd = this->GetDlgItem(IDC_VALUE);
	int pos=b->GetCurSel();

	if (GLContext!=NULL)
		GLContext->glMakeCurrent();
	else
		Context.glMakeCurrent();


	CString		text;
	GLint		iparam[4];
	GLfloat		fparam[4];
	GLboolean	bparam[4];

	switch (pos)
	{
		case 0:GetIntegerv(GL_MAX_LIGHTS,iparam);
		case 1:GetIntegerv(GL_MAX_CLIP_PLANES,iparam);
		case 2:GetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,iparam);
		case 3:GetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,iparam);
		case 4:GetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH,iparam);
		case 5:GetIntegerv(GL_SUBPIXEL_BITS,iparam);
		case 6:GetIntegerv(GL_MAX_TEXTURE_SIZE,iparam);
		case 7:GetIntegerv(GL_MAX_PIXEL_MAP_TABLE,iparam);
		case 8:GetIntegerv(GL_MAX_NAME_STACK_DEPTH,iparam);
		case 9:GetIntegerv(GL_MAX_LIST_NESTING,iparam);
		case 10:GetIntegerv(GL_MAX_EVAL_ORDER,iparam);
		case 11:glGetIntegerv(GL_MAX_VIEWPORT_DIMS,iparam);
				text.Format(TEXT("%d %d"),iparam[0],iparam[1]);
				Wnd->SetWindowText(text);
				break;
		case 12:GetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH,iparam);
		case 13:GetIntegerv(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,iparam);
		case 14:GetBooleanv(GL_AUX_BUFFERS,bparam);
		case 15:GetBooleanv(GL_RGBA_MODE,bparam);
		case 16:GetBooleanv(GL_INDEX_MODE,bparam);
		case 17:GetBooleanv(GL_DOUBLEBUFFER,bparam);
		case 18:GetBooleanv(GL_STEREO,bparam);
		case 19:GetFloatv(GL_POINT_SIZE_RANGE,fparam);
		case 20:GetFloatv(GL_POINT_SIZE_GRANULARITY,fparam);
		case 21:GetFloatv(GL_LINE_WIDTH_RANGE,fparam);
		case 22:GetFloatv(GL_LINE_WIDTH_GRANULARITY,fparam);
		case 23:GetIntegerv(GL_RED_BITS,iparam);
		case 24:GetIntegerv(GL_GREEN_BITS,iparam);
		case 25:GetIntegerv(GL_BLUE_BITS,iparam);
		case 26:GetIntegerv(GL_ALPHA_BITS,iparam);
		case 27:GetIntegerv(GL_INDEX_BITS,iparam);
		case 28:GetIntegerv(GL_DEPTH_BITS,iparam);
		case 29:GetIntegerv(GL_STENCIL_BITS,iparam);
		case 30:GetIntegerv(GL_ACCUM_RED_BITS,iparam);
		case 31:GetIntegerv(GL_ACCUM_GREEN_BITS,iparam);
		case 32:GetIntegerv(GL_ACCUM_BLUE_BITS,iparam);
		case 33:GetIntegerv(GL_ACCUM_ALPHA_BITS,iparam);
#if defined(GL_NV_register_combiners)
		case 34:GetIntegerv(GL_NUM_GENERAL_COMBINERS_NV,iparam);
		case 35:GetIntegerv(GL_MAX_GENERAL_COMBINERS_NV,iparam);
#else
		case 34:
		case 35:
			Wnd->SetWindowText(TEXT("0"));
#endif
#if defined(GL_EXT_texture_filter_anisotropic)
		case 36:GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,fparam);
#else
		case 36:
			Wnd->SetWindowText(TEXT("0"));
#endif
#if defined(GL_EXT_texture3D)
        case 37:GetFloatv(GL_MAX_3D_TEXTURE_SIZE_EXT,fparam);
#else
        case 37:
			Wnd->SetWindowText(TEXT("0"));
#endif
		default:
		{
			CTextureFactory &factory = CTextureFactory::getDefaultFactory();
			CTextureFactoryConfig &config = factory.getConfig();
			int nbCompressors = config.getNumCompressors();
			if (pos < nbCompressors + 38)
			{

				const CTextureFactoryConfig::ICompressor *compressor = config.getCompressor(pos - 38);
				Wnd->SetWindowText(CA2T(compressor->getName().c_str()));

			}
			else
			{
				text = "N/A";
				Wnd->SetWindowText(text);
			}
			break;
		}
	}

	CATCH_GL_ERROR
}

void CInitDialog::OnOK() 
{
	Context.DestroyWindow();

	CDialog::OnOK();
}

void CInitDialog::OnCancel() 
{
	Context.DestroyWindow();

	CDialog::OnCancel();
}


void CInitDialog::OnLbnSelchangeList1()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
}
