/***************************************************************************/
/*                                                                         */
/*  GLBenchModuleTemplate.cpp                                              */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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
#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE

static const size_t NB_RESULTS = 2;
static const char* NAME = "GLBench ModuleTemplate";
static const char* DESCRIPTION = "GL Bench Module plug-in template";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
{	"HAL Swap buffers: ",
	"Bench module template"
};

#include "GLBenchModuleTemplate.cxx"

class Display : public GLBenchDisplay
{
public:
	Display() {};
	virtual ~Display();

private:
	virtual	void GLInitContext(void);
	virtual void glDraw(void);
};

Display::~Display()
{
}

void Display::glDraw()
{
	CTimeObject::markTime(this);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor4f(dt/360.0,dt/360.0,1.0,1.0f);

	glPushMatrix();

	glTranslatef(0.0f,0.0f,-5.0f);
	glRotatef(dt,0.0f,0.0f,1.0f);

	glBegin(GL_QUADS);
		glVertex3f(-1.0f,-1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(-1.0f,1.0f,0.0f);
	glEnd();

	glPopMatrix();

	totalTime += CTimeObject::deltaMarkTime(this);

	dt += 1.0f;
	if (dt > 360.0f)
		dt -= 360.0f;
}

void Display::GLInitContext()
{
	glClearColor(0.0f,0.0f,0.0f,0.0);
}

extern "C" GLBENCH_API void Bench(CWnd *parent)
{
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
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE;
	GLDisplay->GLCreateWindow("OpenGL Context",parent,glCS);


	GLDisplay->glMakeCurrent();

	GLDisplay->glCalibrate(glCS.width, glCS.height);
	BenchStep(1, 1000, GLDisplay);

	GLDisplay->glMakeCurrent(false);

	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
