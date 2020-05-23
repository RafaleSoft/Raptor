/***************************************************************************/
/*                                                                         */
/*  GLBenchPolygons.cpp                                                    */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "StdAfx.h"

#include "Engine/TimeObject.h"
#include "System/Memory.h"
#include "ToolBox/RaptorToolBox.h"
#include "MFCExtension/CWnd/GLWnd.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/3DSet.h"

RAPTOR_NAMESPACE

static const size_t NB_RESULTS = 1;
static const char* NAME = "GL Bench Polygons";
static const char* DESCRIPTION = "Benchmark for polygon drawing primitives";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
	{ "GL Triangles: " };

#include "GLBenchModuleTemplate.cxx"


/////////////////////////////////////////////////////////////////////////////
// The one and only CGLBenchPolygonsApp object

GL_COORD_VERTEX	*vertex = NULL;
CColor::RGBA *colors = NULL;
GLuint *indexes = NULL;

//
/////////////////////////////////////////////////////////////////////////////

class Display : public CGLWnd
{
public:
	Display();
	virtual ~Display();

	virtual	void GLInitContext(void);
	virtual void GLDisplayFunc(void);

private:
	float dt;

	CGeometry	*smallTeapot;
};

Display::Display():
	smallTeapot(NULL)
{
	dt = 0.0f;
}

Display::~Display()
{
}

void Display::GLDisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor4f(1.0,1.0,1.0,1.0f);

	glPushMatrix();

	glTranslatef(0.0f,0.0f,-5.0f);
	glRotatef(dt,0.0f,1.0f,0.0f);

	smallTeapot->glRender();

	glPopMatrix();

	dt += 1.0f;
	if (dt > 360.0f)
		dt -= 360.0f;
}

void Display::GLInitContext()
{
	glClearColor(0.0f,0.0f,0.0f,0.0);

	C3DSet	*set = NULL;
    CRaptorToolBox::load3DStudioScene("Datas\\smallTeapot.3DS",set,NULL);

    C3DSet::C3DSetIterator pos = set->getIterator();
	smallTeapot = (CGeometry*)(set->getChild(pos++));

	smallTeapot->rotationX(-90.0f);
}

extern "C" void GLBENCH_API Bench(CWnd *parent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (parent == NULL)
		return;

	Display *GLDisplay = new Display;

	RECT rect;
	parent->GetClientRect( &rect);

	CRaptorDisplayConfig glCS;
	glCS.width = (rect.right - rect.left);
	glCS.height = (rect.bottom - rect.top);
	glCS.x = 0;
	glCS.y = 0;
	stringstream caption;
    caption << "Raptor Release GLBench : ";
	caption << glCS.width << " " << glCS.height;
	glCS.caption = caption.str();
	glCS.acceleration = CRaptorDisplayConfig::HARDWARE;
	glCS.double_buffer = true;
	glCS.depth_buffer = true;
	glCS.display_mode = CGL_RGBA | CGL_DEPTH;
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE; //CGL_75FPS;
	GLDisplay->GLCreateWindow("OpenGL Context",parent,glCS);

	GLDisplay->glMakeCurrent();

	//	frame rate management
	unsigned int	nb=0;
	CTimeObject::markTime(parent);

	for (int i=0;i<1000;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	float bench_dt = CTimeObject::deltaMarkTime(parent);
	results.result_items[0].fps_rate = nb / bench_dt;
	results.result_items[0].fragment_rate = 0;
	results.result_items[0].score = (unsigned int)(floor)(nb / bench_dt);
	results.result_items[0].driver_overhead = 0;

	GLDisplay->glMakeCurrent(false);

	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
