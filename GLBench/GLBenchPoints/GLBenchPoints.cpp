// GLBenchPoints.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"

#include "Engine/TimeObject.h"
#include "System/Memory.h"
#include "System/Raptor.h"
#include "MFCExtension/CWnd/GLWnd.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/Geometry.h"


RAPTOR_NAMESPACE

static const size_t NB_RESULTS = 5;
static const char* NAME = "GL Bench Points";
static const char* DESCRIPTION = "Benchmark for point drawing primitives";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
	{	"GL Points: ",
		"Colored GL Points: ",
		"Sized GL Points: ",
		"Blended Sized GL Points: ",
		"Blended Sized Point sprites: "
	};

#include "GLBenchModuleTemplate.cxx"

//
// static datas
//
#define SCORE_SCALE		0.00001
#define MAX_VERTEX		2000000
#define LOOP_SIZE		100

#define SCORE_FACTOR		1
#define SCORE_FACTOR_2		2
#define SCORE_FACTOR_3		1.5
#define SCORE_FACTOR_4		5



CTextureObject *sprite = NULL;
//
/////////////////////////////////////////////////////////////////////////////


class Display : public GLBenchDisplay
{
public:
	Display();
	virtual ~Display();

	int draw;

private:
	virtual	void GLInitContext(void);
	virtual void glDraw(void);
	void Draw(void);

	float dt;
	CGeometry	*m_points;
};

Display::Display() : GLBenchDisplay()
{
	draw = 0;
	dt = 0.0f;
	m_points = NULL;
}

Display::~Display()
{
}


void Display::Draw(void)
{
	m_points->glRender();
	//for (int i=0;i<MAX_VERTEX;i+=500000)
	//	glDrawElements( GL_POINTS, 500000, GL_UNSIGNED_INT,indexes+i);
    //glDrawElements( GL_POINTS, MAX_VERTEX, GL_UNSIGNED_INT,&indexes[0]);
//	glFlush();
}

void Display::glDraw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0f,0.0f,-4.0f);
	glRotatef(dt,1.0f,0.0f,0.0f);
	glRotatef(dt+dt,0.0f,1.0f,0.0f);
	glRotatef(dt+dt+dt,0.0f,0.0f,1.0f);

	if (draw == 0)
	{
		glColor4f(1.0,1.0,1.0,1.0f);
		Draw();
	}
	else if (draw == 1)
	{
		Draw();
	}
	else if (draw == 2)
	{
		glColor4f(0.3f,0.7f,0.8f,0.3f);
		Draw();
	}

	glPopMatrix();

	dt = dt + 1.0f;
	if (dt > 360.0f)
		dt = 0.0f;
}


void Display::GLInitContext()
{
	GL_COORD_VERTEX	*vertex = (GL_COORD_VERTEX*)(CHostMemoryManager::GetInstance()->allocate(sizeof(GL_COORD_VERTEX),MAX_VERTEX,16));
	CColor::RGBA *colors = (CColor::RGBA*)(CHostMemoryManager::GetInstance()->allocate(sizeof(CColor::RGBA),MAX_VERTEX,16));
	GLushort *indexes = (GLushort*)(CHostMemoryManager::GetInstance()->allocate(sizeof(GLushort),MAX_VERTEX,16));

	for (int i=0;i<MAX_VERTEX;i++)
	{
		colors[i].r = 1.0f / 255.0f * (rand() % 256);
		colors[i].g = 1.0f / 255.0f * (rand() % 256);
		colors[i].b = 1.0f / 255.0f * (rand() % 256);
		colors[i].a = 1.0f;
	}

	GL_COORD_VERTEX p0;
	GL_COORD_VERTEX p1;
	GL_COORD_VERTEX p2;
	GL_COORD_VERTEX p3;

	float size = 4.0f;

	p0.x = size * -0.866f; p0.y = 0.0f; p0.z = size * -0.5f;
	p1.x = size *0.866f; p1.y = 0.0f; p1.z = size * -0.5f;
	p2.x = 0.0f; p2.y = 0.0f; p2.z = size * 1.0f;
	p3.x = 0.0f; p3.y = size * 1.5612f; p3.z = size * 0.25f;

	vertex[0].x = p3.x;
	vertex[0].y = p3.y;
	vertex[0].z = p3.z;
	vertex[0].h = 1.0f;

	for (int i=1;i<MAX_VERTEX;i++)
	{
		int idx = (rand() % 4);
		switch(idx)
		{
			case 0:
				vertex[i].x = vertex[i-1].x - 0.5f * (vertex[i-1].x - p0.x);
				vertex[i].y = vertex[i-1].y - 0.5f * (vertex[i-1].y - p0.y);
				vertex[i].z = vertex[i-1].z - 0.5f * (vertex[i-1].z - p0.z);
				vertex[i].h = 1.0f;
				break;
			case 1:
				vertex[i].x = vertex[i-1].x - 0.5f * (vertex[i-1].x - p1.x);
				vertex[i].y = vertex[i-1].y - 0.5f * (vertex[i-1].y - p1.y);
				vertex[i].z = vertex[i-1].z - 0.5f * (vertex[i-1].z - p1.z);
				vertex[i].h = 1.0f;
				break;
			case 2:
				vertex[i].x = vertex[i-1].x - 0.5f * (vertex[i-1].x - p2.x);
				vertex[i].y = vertex[i-1].y - 0.5f * (vertex[i-1].y - p2.y);
				vertex[i].z = vertex[i-1].z - 0.5f * (vertex[i-1].z - p2.z);
				vertex[i].h = 1.0f;
				break;
			case 3:
				vertex[i].x = vertex[i-1].x - 0.5f * (vertex[i-1].x - p3.x);
				vertex[i].y = vertex[i-1].y - 0.5f * (vertex[i-1].y - p3.y);
				vertex[i].z = vertex[i-1].z - 0.5f * (vertex[i-1].z - p3.z);
				vertex[i].h = 1.0f;
				break;
		}
	}

	for (int i=0;i<MAX_VERTEX;i++)
	{
		indexes[i] = i;
	}

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	
	m_points = new CGeometry("BenchPoints");
	CGeometry::CRenderingModel model(	CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY|
										CGeometry::CRenderingModel::CGL_COLORS);
	m_points->setRenderingModel(model);

	m_points->glSetVertices(MAX_VERTEX,NULL);
	m_points->glSetColors(MAX_VERTEX,NULL);
	m_points->glSetVertices(MAX_VERTEX,vertex);
	m_points->glSetColors(MAX_VERTEX,colors);

	CGeometryPrimitive*	pointsPrimitive = m_points->createPrimitive(CGeometryPrimitive::POINT);
	pointsPrimitive->setIndexes(MAX_VERTEX,indexes);

	CHostMemoryManager::GetInstance()->release(vertex);
	CHostMemoryManager::GetInstance()->release(colors);
	CHostMemoryManager::GetInstance()->release(indexes);


	CTextureFactory &txt = CTextureFactory::getDefaultFactory();

	sprite = txt.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA);
    sprite->setSize(32,32);
	sprite->glSetTransparency(192);

	CImage spr;
	spr.allocatePixels(32,32);

	unsigned char *data = spr.getPixels();
	unsigned int offset = 0;
	for (int j=0;j<32;j++)
	{
		for (int i=0;i<32;i++)
		{
			data[offset++] = 8 * i;
			data[offset++] = 8 * j;
			data[offset++] = (i+j) * 4;
			data[offset++] = rand() % 256;
		}
	}

	txt.glLoadTexture(sprite,spr);
}

extern "C" void GLBENCH_API Bench(CWnd *parent)
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
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE; //CGL_75FPS;
	GLDisplay->GLCreateWindow("OpenGL Context",parent,glCS);

	//
	//	Bench base primitive
	//
	GLDisplay->draw = 0;
	unsigned int	nb=0;
	CTimeObject::markTime(parent);

    int i=0;
	for (i=0;i<LOOP_SIZE;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	float bench_dt = CTimeObject::deltaMarkTime(parent);
	int pointspersec = nb / bench_dt * MAX_VERTEX;

	results.result_items[0].fps_rate = nb / bench_dt;
	results.result_items[0].score = pointspersec * SCORE_SCALE * SCORE_FACTOR;
	stringstream desc;
	desc << " " << pointspersec << " points / sec";
	strcat(RESULT_DESCRIPTION[0],desc.str().c_str());

	//
	//	Bench base colored primitive
	//
	nb=0;
	GLDisplay->draw = 1;
	GLDisplay->glMakeCurrent();
	glEnableClientState(GL_COLOR_ARRAY);
	CTimeObject::markTime(parent);

	for (i=0;i<LOOP_SIZE;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	bench_dt = CTimeObject::deltaMarkTime(parent);
	pointspersec = nb / bench_dt * MAX_VERTEX;
	
	results.result_items[1].fps_rate = nb / bench_dt;
	results.result_items[1].score = pointspersec * SCORE_SCALE * SCORE_FACTOR_2;
	desc.str("");
	desc << " " << pointspersec << " colored points / sec";
	strcat(RESULT_DESCRIPTION[1],desc.str().c_str());

	//
	//	Bench point size primitive
	//
	nb=0;
	GLDisplay->draw = 0;
	GLDisplay->glMakeCurrent();
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	
	CTimeObject::markTime(parent);
	glPointSize(20.0);
#if defined(GL_EXT_point_parameters)
	if (Raptor::glIsExtensionSupported(GL_EXT_POINT_PARAMETERS_EXTENSION_NAME))
	{
		CVertexShader s;
		GL_COORD_VERTEX quadric(1.0f, 1.0f, 10.0f, 1.0f);
		s.glPointParameter(0.0f,20.0,quadric);
	}
#endif
	for (i=0;i<LOOP_SIZE;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	bench_dt = CTimeObject::deltaMarkTime(parent);
	pointspersec = nb / bench_dt * MAX_VERTEX;
	
	results.result_items[2].fps_rate = nb / bench_dt;
	results.result_items[2].score = pointspersec * SCORE_SCALE * SCORE_FACTOR_3;
	desc.str("");
	desc << " " << pointspersec << " sized points (20 pixels) / sec";
	strcat(RESULT_DESCRIPTION[2],desc.str().c_str());


	//
	//	Bench blended point size primitive
	//
	nb=0;
	GLDisplay->draw = 2;
	GLDisplay->glMakeCurrent();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(32.0);
#if defined(GL_EXT_point_parameters)
	if (Raptor::glIsExtensionSupported(GL_EXT_POINT_PARAMETERS_EXTENSION_NAME))
	{
		CVertexShader s;
		GL_COORD_VERTEX quadric(1.0f, 1.0f, 10.0f, 1.0f);
		s.glPointParameter(0.0f,32.0,quadric);
	}
#endif

	CTimeObject::markTime(parent);
	for (i=0;i<LOOP_SIZE;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	bench_dt = CTimeObject::deltaMarkTime(parent);
	pointspersec = nb / bench_dt * MAX_VERTEX;

	results.result_items[3].fps_rate = nb / bench_dt;
	results.result_items[3].score = pointspersec * SCORE_SCALE * SCORE_FACTOR_3;
	desc.str("");
	desc << " " << pointspersec << " blended sized points (32 pixels) / sec";
	strcat(RESULT_DESCRIPTION[3],desc.str().c_str());

	//
	//	Bench point sprites
	//
#if defined(GL_NV_point_sprite)
	GLDisplay->glMakeCurrent();
	glPointSize(16.0);
	if (Raptor::glIsExtensionSupported(GL_NV_POINT_SPRITE_EXTENSION_NAME))
	{
		//AfxMessageBox("Code need to be adapted !");
	#if defined(GL_EXT_point_parameters)
		if (Raptor::glIsExtensionSupported(GL_EXT_POINT_PARAMETERS_EXTENSION_NAME))
		{
			CVertexShader s;
			GL_COORD_VERTEX quadric(1.0f, 0.2f, 0.0f, 1.0f);
			s.glPointParameter(0.0f,16.0,quadric);
		}
	
		GLDisplay->draw = 2;
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_POINT_SPRITE_NV);
		//GLDisplay->glPointParameterfEXT(GL_POINT_SPRITE_R_MODE_NV,GL_ZERO);
		glTexEnvi(GL_POINT_SPRITE_NV,GL_COORD_REPLACE_NV,GL_TRUE);
		sprite->glvkRender();
		CTimeObject::markTime(parent);
		for (i=0;i<LOOP_SIZE;i++)
		{
			GLDisplay->SendMessage(WM_PAINT);
			nb++;
		}

		bench_dt = CTimeObject::deltaMarkTime(parent);
	#endif
	}
	else
	{
		bench_dt = 1;
	}
#else
	bench_dt = 1;
#endif
	pointspersec = nb / bench_dt * MAX_VERTEX;

	results.result_items[4].fps_rate = nb / bench_dt;
	results.result_items[4].score = pointspersec * SCORE_SCALE * SCORE_FACTOR_4;
	desc.str("");
	desc << " " << pointspersec << " blended sized point sprites (16*16 pixels) / sec";
	strcat(RESULT_DESCRIPTION[4],desc.str().c_str());


	//
	//	Bench done, release everything
	//
	GLDisplay->glMakeCurrent();

	GLDisplay->glMakeCurrent(false);
	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
