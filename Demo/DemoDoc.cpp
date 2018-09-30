// DemoDoc.cpp : implementation of the CDemoDoc class
//

#include "StdAfx.h"
#include "DemoDoc.h"
#include "ObjectStore.h"

#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Object3DShadow.h"
#include "GLHierarchy/Projector.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureUnitSetup.h"

#include "Engine/3DPath.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "Engine/Environment.h"
#include "Engine/GeometricModifier.h"
#include "Engine/ViewPoint.h"

#include "System/Raptor.h"
#include "System/RaptorConsole.h"
#include "System/RaptorDisplay.h"
#include "System/RaptorGLExtensions.h"
#include "System/RaptorMessages.h"

#include "ToolBox/BasicObjects.h"
#include "ToolBox/Filters/DOFFilter.h"
#include "ToolBox/Filters/HDRFilter.h"


static const float TIME_FACTOR = 1.0f;
static const float HEAT_SIZE = 0.2f;
static const float HEAT_HALFSIZE = 0.1f;

/////////////////////////////////////////////////////////////////////////////
// CDemoDoc


class HeatSpots : public CRaptorDisplayFilter, public CGeometricModifier
{
public:
    HeatSpots(int hcels = 50,int vcels = 50)
		:CGeometricModifier(CGL_TIME_LINEAR,0,1,0,0,NULL,CModifier::CModifierClassID::GetClassId(),"HEAT_MODIFIER"),
	     m_hcels(hcels+1), m_vcels(vcels+1),m_index(0),m_d_grid(NULL),m_dt(0.0f)
    {   };

    virtual ~HeatSpots() { glDestroyFilter(); };

	virtual void glRenderFilter(void) {};
	virtual void glRenderFilterOutput(void);

    virtual bool glInitFilter(void);

	virtual void glDestroyFilter(void);

    virtual void RAPTOR_FASTCALL updateVertices(float dt,GL_COORD_VERTEX *vertices);

    void AddSpot(const GL_COORD_VERTEX& spotPos) { m_spots.push_back(spotPos); };

    CLight* const *lights;

private:
    int			m_hcels;
	int			m_vcels;
    int         m_index;
	float		m_dt;
    vector<GL_COORD_VERTEX> m_spots;
	float		*m_d_grid;
};


void RAPTOR_FASTCALL HeatSpots::updateVertices(float dt,GL_COORD_VERTEX *vertices)
{
    if (m_d_grid == NULL)
        return;
	m_dt = m_dt + dt;
	if (m_dt < 0.04f)
		return;
	m_dt = 0.0f;

   	int offset = 2 * m_hcels * m_index;

	for (int i=0;i<m_hcels;i++)
	{
		float rd = 0.02f * ((float)rand() / RAND_MAX) - 0.01f;
		m_d_grid[offset + 2 * i] = rd;

		rd = 0.02f * ((float)rand() / RAND_MAX) - 0.01f;
		m_d_grid[offset + 2 * i + 1] = rd;
	}

	int index = m_index;
    float stepx = HEAT_SIZE / (m_hcels - 1);
    float stepy = HEAT_SIZE / (m_vcels - 1);

	for (int j=0;j<m_vcels;j++)
	{
		int offset2 = 2 * m_hcels * index;

		for (int i=0;i<m_hcels;i++)
		{
			bool border = (j==0) || (i==0) || (j==(m_vcels-1)) || (i==(m_hcels-1));

			GL_COORD_VERTEX &V = vertices[m_hcels * j + i];
			V.x = -HEAT_HALFSIZE + i * stepx;
			V.y = -HEAT_HALFSIZE + j * stepy;
			V.z = -1.0f;
			V.h = 1.0f;
			if (!border)
			{
				float d = 1 - 10.0f * sqrt(V.x*V.x+V.y*V.y);

				V.x += d * m_d_grid[offset2 + 2 * i];
				V.y += d * m_d_grid[offset2 + 2 * i + 1];
			}
		}

		index++;
		if( index == m_vcels)
			index = 0;
	}


	m_index--;
	if( m_index < 0)
		m_index = m_vcels - 1;
}

void HeatSpots::glRenderFilterOutput()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	glActiveTextureARB(GL_TEXTURE0_ARB);
	colorInput->glvkRender();
    glDrawBuffer();

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
    for (unsigned int i=0;i<m_spots.size();i++)
    {
        GL_COORD_VERTEX lPos = lights[i+1]->getLightClipPosition();
        float visibility = lights[i+1]->getLightVisibility();

        if ((lPos.z < 1.0f) && (lPos.z > -1.0f) && (visibility > 0.0f))
        {
            float factor = 10.0f * (1.0f - lPos.z);

            glPushMatrix();
				//  add 5% to position above the spot light
                glTranslatef(lPos.x, 1.05f*lPos.y, -1.0f );
                glScalef(factor,factor,0.0f);
                glMatrixMode(GL_TEXTURE);
	            glPushMatrix();
	                glTranslatef(0.5f*(lPos.x+1), 
								 0.5f*(1.05f*lPos.y+1),
								 0.0f);
					glScalef(HEAT_HALFSIZE*factor,
							 HEAT_HALFSIZE*factor,
							 0.0f);
					glTranslatef(-0.5f, 
								 -0.5f,
								 0.0f);
					getObject()->glRender();
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }
    }

    glPopAttrib();

	glBindTexture(GL_TEXTURE_2D,0);
}

bool HeatSpots::glInitFilter()
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();
	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && 
		(m_fModel == CRaptorDisplayFilter::RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_BILINEAR,
															colorExternalSource);
	}

	filterFactory.getConfig().useTextureResize(previousResize);

	m_d_grid = new float[2 * m_hcels * m_vcels];
	memset(m_d_grid,0,2*m_hcels*m_vcels*sizeof(float));

	CBasicObjects::CRectMesh *mesh = new CBasicObjects::CRectMesh();
	mesh->setDimensions(HEAT_SIZE,HEAT_SIZE,m_hcels,m_vcels);
	mesh->getRenderingModel().removeModel(CGeometry::CRenderingModel::CGL_NORMALS);

	setGeometry(mesh);

    return true;
}

void HeatSpots::glDestroyFilter(void)
{
	setGeometry(NULL);

	if (m_d_grid != NULL)
	{
		delete [] m_d_grid;
		m_d_grid = NULL;
	}

	CRaptorDisplayFilter::glDestroyFilter();
}


/////////////////////////////////////////////////////////////////////////////
// CDemoDoc construction/destruction

CDemoDoc::CDemoDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay *pDisplay):
	m_objects(NULL),dof(NULL),
	m_pViewPoint(NULL),m_pBuffer(NULL),
	m_pTexture(0),
    m_viewPathStairs(NULL),m_lookPathStairs(NULL),
    m_viewPathRoom(NULL),m_lookPathRoom(NULL),
	nbFrames(0),startTime(0),finishTime(0),lastFrame(0),
	minfps(FLT_MAX),maxfps(-FLT_MAX)
{
	m_pDisplay = pDisplay;
    m_wnd = wnd;
}

CDemoDoc::~CDemoDoc()
{
    // Everything is destroyed by purge at exit
}


/////////////////////////////////////////////////////////////////////////////
// CDemoDoc commands
//	Default virtual implementation
void CDemoDoc::GLInitContext(void)
{
	if ((!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(WGL_ARB_PBUFFER_EXTENSION_NAME)) ||
		(!Raptor::glIsExtensionSupported(WGL_ARB_RENDER_TEXTURE_EXTENSION_NAME) && 
		!Raptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_OBJECT_EXTENSION_NAME)))
	{
        CRaptorMessages * const msg = Raptor::GetMessages();
        msg->displayMessage("It seems your CPU/GPU does not support necessary extensions to run this demo in full hardware. Rendering might be wrong");
	}

    CRaptorConsole *pConsole = Raptor::GetConsole();
    pConsole->glInit();
    pConsole->showStatus(true);
    pConsole->showObjectCount(true);
    pConsole->showTriangleCount(true);
	pConsole->showFrameTime(true);

	m_objects = new CObjectStore;
	m_objects->SetPerf(true);

	m_objects->LoadModels();
	m_objects->BuildObjects();
    m_objects->BuildScene();

	CTimeObject::setTimeFactor(TIME_FACTOR);
	CAnimator *animator = new CAnimator();
	animator->animateAll(true);
	CAnimator::SetAnimator(animator);
    
    CRaptorDisplay * const pCurrentDisplay = CRaptorDisplay::GetCurrentDisplay();
    CRenderingProperties *rp = pCurrentDisplay->getRenderingProperties();
	rp->clear(CGL_RGBA|CGL_DEPTH);
	rp->setMultisampling(CRenderingProperties::ENABLE);

    C3DScene *m_pScene = pCurrentDisplay->getRootScene();
	//pCurrentDisplay->getViewPoint()->setCurrentPath(-1);

    pConsole->activateConsole(true);
    pConsole->runBatch("Demo.cmd");
    pConsole->activateConsole(false);

    m_pScene->glManageEnvironment(CEnvironment::SHADOW_VOLUME,0,0);
    //m_pScene->glManageEnvironment(CEnvironment::SHADOW_MAP,1024,1024);
    m_pScene->useSceneGlobalAmbient(CColor::RGBA(0.1f,0.1f,0.1f,1.0f));
	m_pScene->useZSort();

    CLight* const *lights = m_objects->getLights();
    HeatSpots *pSpots= new HeatSpots(10,10);
    for (int i=1;i<9;i++)
    {
        GL_COORD_VERTEX lpos = lights[i]->getLightPosition();
        pSpots->AddSpot(lpos);
    }
    pSpots->lights = lights;
	pCurrentDisplay->glvkBindDisplay(*pSpots);
	pSpots->releaseReference();

    dof = new CDOFFilter;
    dof->setDOFParams(0.85f, 10.0f);
	dof->setBlurNbPass(4);
	pCurrentDisplay->glvkBindDisplay(*dof);
	dof->releaseReference();

	CRaptorDisplayConfig rda;
	pCurrentDisplay->glQueryStatus(rda,GL_CONFIG_STATE_QUERY);
    rda.caption = "Raptor HDR Filter";
	hdr = new CHDRFilter(rda);
    //pCurrentDisplay->glBindDisplay(*hdr);
	hdr->setBlurNbPass(2);
	hdr->setHFTreshold(0.9f);
}

void CDemoDoc::glRender()
{
	m_pDisplay->glvkBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();


	nbFrames++;

	//	Benchmark
	//	( wait a little for the process to be alone running )
	if (nbFrames > 10)
	{
        ULONGLONG val = GetTickCount64();

		if (startTime == 0)
			startTime = val;
		finishTime = val;

		float fps = m_pDisplay->getRTFPS();
		if (fps > 0)
		{
			if (fps < minfps)
				minfps = fps;
			if (fps > maxfps)
				maxfps = fps;
		}

        lastFrame = val;
	}
}

