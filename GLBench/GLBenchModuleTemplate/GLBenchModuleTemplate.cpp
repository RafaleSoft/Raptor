// GLBenchModuleTemplate.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"

#include "Engine/TimeObject.h"
#include "MFCExtension/CWnd/GLWnd.h"

RAPTOR_NAMESPACE

static const size_t NB_RESULTS = 1;
static const char* NAME = "GLBench ModuleTemplate";
static const char* DESCRIPTION = "GL Bench Module plug-in template";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
{ "Bench module template" };

#include "GLBenchModuleTemplate.cxx"

class Display : public CGLWnd
{
public:
	Display();
	virtual ~Display();

	virtual	void GLInitContext(void);
	virtual void GLDisplayFunc(void);

private:
	float dt;
};

Display::Display()
{
	dt = 0.0f;
}

Display::~Display()
{
}

void Display::GLDisplayFunc()
{
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
	glCS.display_mode = CGL_RGBA | CGL_DEPTH | CGL_DOUBLE;
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE;
	GLDisplay->GLCreateWindow("OpenGL Context",parent,glCS);

	GLDisplay->glMakeCurrent();

	//	frame rate management
	CTimeObject::markTime(parent);
	
	unsigned int	nb=0;
	for (int i=0;i<1000;i++)
	{
		GLDisplay->Invalidate();
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	float dt = CTimeObject::deltaMarkTime(parent);

	results.result_items[0].rate = nb / dt;
	results.result_items[0].score = (unsigned int)(floor)(nb / dt);

	GLDisplay->glMakeCurrent(false);

	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
