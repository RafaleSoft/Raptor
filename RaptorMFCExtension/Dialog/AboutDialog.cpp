// AboutDialog.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "CWnd/GLWnd.h"
#endif
#if !defined(AFX_ABOUTDIALOG_H__23A6FAE2_FE89_466A_B177_290ACC171E05__INCLUDED_)
	#include "AboutDialog.h"
#endif
#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/ViewPoint.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
    #include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_)
	#include "GLHierarchy/SimpleObject.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif


RAPTOR_NAMESPACE

#define NB_LINES 52
static char *text[NB_LINES] =
{
	"Raptor OGLSDK by Rafale Soft Inc.(c)    ",
	" ",
	"3D Engine & MFC extensions based on",
	"OpenGL 2.1 ICD renderer",
	"+ OpenGL extensions based on ARB 2.1.1",
	" ",
	"Features:",
	" ",
	"- CGLWnd extends MFC CWnd",
	" ",
	"- CGLView, CGLDocument, CGLFrame & ",
	"CGLMDIChildWnd extend MFC SDI & MDI",
	" architecture",
	" ",
	"- Texture repository CTextureFactory object",
	"and Animated Textures",
	" ",
	"- Fully configurable TMU for Texture",
	" Programs and Register Combiners",
	" ",
	"- Support for Vertex Programs",
	" ",
	"- CMaterial & CLight objects",
	" ",
	"- 2D & 3D True Type fonts CGLFont object",
	" ",
	"- 3D hierarchical geometry with",
	"CObject3D, CGeometry & C3DSet",
	" ",
	"- Bump mapped geometry CBumppedGeometry",
	"objects for Generic, DOT3, and ",
	" Pixel Shader Bump mapping",
	" ",
	"- 2D Layers CGLLayers objets",
	" ",
	"- Physical property CPhysics objects",
	" ",
	"- High level clipping Bounding Boxes",
	" ",
	"- Collision and clipping 3D engine",
	" ",
	"- SMP Multiprocessor support with",
	"seperate math/graphx threaded pipelines",
	" ",
	"- CAnimator + CModifiers time ",
	"synchronized modifier for CTime objects",
	" ",
	"- Real Time Planar Shadows, Shadow Volumes",
	"and Shadow Maps",
	" ",
	" Special thanks to VisionAstral for",
	"his great contribution"
};


class CAboutVideo : public CSimpleObject
{
public:
	CAboutVideo(CTextureObject *tex,float width, float height)
		:m_pTxt(tex),tw(width),th(height)
	{ setBoundingBox(GL_COORD_VERTEX(-1,-1,-3,1),GL_COORD_VERTEX(1,1,-1,1)); };
	virtual ~CAboutVideo() {};

	virtual C3DEngine::CLIP_RESULT RAPTOR_FASTCALL glClip(void) const
	{ return C3DEngine::CLIP_NONE; };
	virtual void glClipRender() { glRender(); };
	virtual void glRender();

private:
	CTextureObject	*m_pTxt;
    float   tw;
    float   th;
};

void CAboutVideo::glRender()
{
	m_pTxt->glRender();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex3f(-2.0f,-1.5f,-2.0f);
		glTexCoord2f(tw,0.0f); glVertex3f(2.0f,-1.5f,-2.0f);
		glTexCoord2f(tw,th); glVertex3f(2.0f,1.5f,-2.0f);
		glTexCoord2f(0.0f,th); glVertex3f(-2.0f,1.5f,-2.0f);
	glEnd();
}

/////////////////////////////////////////////////////////////////////////////
// GL Display for AboutDialog
/////////////////////////////////////////////////////////////////////////////
class CGLDisplay : public CGLWnd
{
public:
	CGLDisplay();
	virtual ~CGLDisplay();

	virtual	void GLInitContext(void);

	CAnimator	*m_pAnimator;
	CTextureObject	*m_pTxt;
};

CGLDisplay::CGLDisplay():
	m_pTxt(NULL),
	m_pAnimator(NULL)
{
}

CGLDisplay::~CGLDisplay()
{
	if (m_pAnimator)
	{
		m_pAnimator->endPlayBack();
		m_pAnimator = NULL;
	}

	if (NULL != m_pTxt)
		m_pTxt->releaseReference();
}

void CGLDisplay::GLInitContext()
{
	CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
	CViewPoint *pVP = pDisplay->getViewPoint();
	glColor4f(1.0,1.0,1.0,1.0f);

	pVP->setViewVolume(-1.0,1.0,-1.0,1.0,1.0,1000.0,CViewPoint::PERSPECTIVE);
	pVP->glRenderViewPointModel();

    CRenderingProperties *props = pDisplay->getRenderingProperties();
    props->setTexturing(CRenderingProperties::ENABLE);

    CRaptorDataManager  *dataManager = CRaptorDataManager::getInstance();
    if (dataManager != NULL)
		dataManager->ExportFile("rapsplsh.AVI","");

	TCHAR Tbuffer[1024];
    DWORD res = GetEnvironmentVariable(TEXT("TMP"),Tbuffer,1024);
	stringstream buffer;
	buffer << CT2A(Tbuffer) << "\\rapsplsh.AVI";
	
	glClearColor(SystemRed,SystemGreen,SystemBlue,0.0);

	m_pAnimator = CAnimator::GetAnimator();
	ITextureGenerator *pGenerator = m_pAnimator->glStartPlayBack(buffer.str().c_str(),true);

	CTextureFactory f;
    m_pTxt = f.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
										CTextureObject::CGL_MULTIPLY,
										CTextureObject::CGL_BILINEAR,
										pGenerator);

    float tw = (float)(pGenerator->getGenerateWidth()) / (float)(m_pTxt->getWidth());
	float th = (float)(pGenerator->getGenerateHeight()) / (float)(m_pTxt->getHeight());
	
	C3DScene *pScene = pDisplay->getRootScene();
	CAboutVideo* pVideo = new CAboutVideo(m_pTxt,tw,th);
	pScene->addObject(pVideo);

	CATCH_GL_ERROR
}



/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog
/////////////////////////////////////////////////////////////////////////////

CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDialog::IDD, pParent)
{
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CAboutDialog)
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers

BOOL CAboutDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Setting up Raptor version item
	{
        DWORD version = Raptor::GetVersion();
		stringstream vrs;
        vrs << "Version: " << RAPTOR_VERSION_STR;
		this->SetDlgItemText(IDC_VERSION,CA2T(vrs.str().c_str()));
	}

	//	Setting up frame display
	CPaintDC dc(this);
	rect.top = 0; rect.left = 10;
	rect.right = 200; rect.bottom = 180;
	pos = rect.bottom - 10;
	line = 0;
	brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	font.CreateFont(10,6,0,0,FW_NORMAL,
					0,0,0,0,OUT_TT_PRECIS,
					CLIP_TT_ALWAYS,PROOF_QUALITY,
					VARIABLE_PITCH,TEXT("Small Fonts"));

	//	Setting up OpenGL display
	GLDisplay = new CGLDisplay;

	CRaptorDisplayConfig glCS;
	glCS.width = 140;
	glCS.height = 140;
	glCS.x = 240;
	glCS.y = 40;
	glCS.caption = "Raptor Release test";
	glCS.display_mode = CGL_RGBA | CGL_DEPTH | CGL_DOUBLE;
	glCS.refresh_rate.fps = 75;
	GLDisplay->GLCreateWindow("OpenGL Context",this,glCS);
	GLDisplay->glMakeCurrent();

	//	want 30 fps => 33.3ms
	this->SetTimer(1,33,NULL);

	CATCH_GL_ERROR

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDialog::OnOK() 
{
	this->KillTimer(1);
	GLDisplay->DestroyWindow();
	delete GLDisplay;
	CDialog::OnOK();
}

void CAboutDialog::OnTimer(UINT nIDEvent)
{
	::InvalidateRect(m_hWnd,&rect,FALSE);
	CTimeObject::deltaTime();
	GLDisplay->glRender();
	GLDisplay->m_pAnimator->animate();
	CDialog::OnTimer(nIDEvent);
}

void CAboutDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(font);
	dc.SetStretchBltMode(HALFTONE);
	dc.SetBrushOrg(0,0);
	dc.FillRect(rect,&brush);

	// displaying info
	pos--;
	if (pos < rect.top)
	{
		line++;
		pos = rect.top + 9;
	}

	int curpos = pos;

	for (int i=line;i<line+35;i++)
	{
		if (i<NB_LINES)
			if (curpos < rect.bottom)
				dc.TextOut(20,curpos,text[i]);

		curpos += 10;
	}

	if (line > NB_LINES)
	{
		line = 0;
		pos = rect.bottom;
	}
}
