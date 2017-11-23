// GLBenchFillRate.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"


#include "Engine/ViewPoint.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/VertexShader.h"
#include "System/Raptor.h"
#include "System/Memory.h"

RAPTOR_NAMESPACE

//
// static datas
//
#define LOOP_SIZE			5000
#define LOOP_LOADTEXTURE	50
#define LOOP_DRAWPIXELS		100

#define TX				4.0f
#define	TY				4.0f

static const size_t NB_RESULTS = 13;
static const char* NAME = "GL Bench Fill Rate";
static const char* DESCRIPTION = "Benchmark for pixel and texel fill rate";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
	{	"HAL Swap buffers: ",
		"Draw Pixel Transfer RGBA: ",
		"Texel Transfer RGBA (1024x1024): ",
		"Texel Transfer BGRA (1024x1024): ",
		"Clear 32bits color buffer: ",
		"Clear 24bits Z-buffer: ",
		"Single texturing small ( 256x256 ): ",
		"Single texturing large ( 1024x1024 ): ",
		"Single texturing compressed ( 1024x1024 ): ",
		"Single texturing blended bilinear filtering ( 1024x1024 ): ",
		"Multi texturing modulate bilinear filtering ( 1024x1024 ): ",
		"Single texturing trilinear filtering ( 1024x1024 ): ",
		"Single texturing x16 anisotropic filtering ( 1024x1024 ): "
	};

#include "GLBenchModuleTemplate.cxx"

//
/////////////////////////////////////////////////////////////////////////////

class Display : public GLBenchDisplay
{
public:
	Display();
	virtual ~Display();

	
	int draw;
	float viewScale;
	CTextureSet *txt;

	RAPTOR_HANDLE setupHANDLE;

private:
	virtual	void GLInitContext(void);
	virtual void glDraw(void);

	GLuint	square;
	GLuint	square2;
};

Display::Display() : GLBenchDisplay()
{
	draw = 0;
	viewScale = 1.33f;
	square = square2 = 0;
}

Display::~Display()
{
	delete txt;
}

void Display::GLInitContext()
{
	CViewPoint *vp = getDisplay()->getViewPoint();
	vp->setViewVolume(-viewScale, viewScale, -1.0f, 1.0f, 1.0f, 1000.0f, CViewPoint::ORTHOGRAPHIC);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.0f,0.0f,0.0f,0.5f);

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CTextureFactoryConfig& config = f.getConfig();
	txt = new CTextureSet();

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	CTextureObject* T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\M1_1024.jpg");
	txt->addTexture(T);

	if (Raptor::glIsExtensionSupported("GL_ARB_texture_compression"))
	{
		T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
		T->glSetTransparency(128);
		const CTextureFactoryConfig::ICompressor *compressor = config.getCurrentCompressor();
		if (0 < config.getNumCompressors())
			config.setCurrentCompressor(config.getCompressor("OpenGL"));
		f.glLoadTexture(T,"Datas\\M1_1024.jpg");
		config.setCurrentCompressor(compressor);
	}
	else
	{
		T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
		T->glSetTransparency(128);
		f.glLoadTexture(T,"Datas\\M1_1024.jpg");
	}
	txt->addTexture(T);

	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\M74_1024.jpg");
	txt->addTexture(T);

	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_TRILINEAR);
	config.setGenerateMipmap(false);
	T->glSetTransparency(128);	f.glLoadTexture(T,"Datas\\M1_1024.jpg");
	T->selectMipMapLevel(1);	f.glLoadTexture(T,"Datas\\M1_512.jpg");
	T->selectMipMapLevel(2);	f.glLoadTexture(T,"Datas\\M1_256.jpg");
	T->selectMipMapLevel(3);	f.glLoadTexture(T,"Datas\\M1_128.jpg");
	T->selectMipMapLevel(4);	f.glLoadTexture(T,"Datas\\M1_64.jpg");
	T->selectMipMapLevel(5);	f.glLoadTexture(T,"Datas\\M1_32.jpg");
	T->selectMipMapLevel(6);	f.glLoadTexture(T,"Datas\\M1_16.jpg");
	T->selectMipMapLevel(7);	f.glLoadTexture(T,"Datas\\M1_8.jpg");
	T->selectMipMapLevel(8);	f.glLoadTexture(T,"Datas\\M1_4.jpg");
	T->selectMipMapLevel(9);	f.glLoadTexture(T,"Datas\\M1_2.jpg");
	T->selectMipMapLevel(10);	f.glLoadTexture(T,"Datas\\M1_1.jpg");
	config.setGenerateMipmap(true);
	txt->addTexture(T);


	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\M1_256.jpg");
	txt->addTexture(T);

#if defined(GL_EXT_texture_filter_anisotropic)
	if (Raptor::glIsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		float anisotropy = 1.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&anisotropy);
		config.setCurrentAnisotropy(anisotropy);

		T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_ANISOTROPIC);
		config.setGenerateMipmap(false);
		T->glSetTransparency(255);	f.glLoadTexture(T,"Datas\\M1_1024.jpg");
		T->selectMipMapLevel(1);	f.glLoadTexture(T,"Datas\\M1_512.jpg");
		T->selectMipMapLevel(2);	f.glLoadTexture(T,"Datas\\M1_256.jpg");
		T->selectMipMapLevel(3);	f.glLoadTexture(T,"Datas\\M1_128.jpg");
		T->selectMipMapLevel(4);	f.glLoadTexture(T,"Datas\\M1_64.jpg");
		T->selectMipMapLevel(5);	f.glLoadTexture(T,"Datas\\M1_32.jpg");
		T->selectMipMapLevel(6);	f.glLoadTexture(T,"Datas\\M1_16.jpg");
		T->selectMipMapLevel(7);	f.glLoadTexture(T,"Datas\\M1_8.jpg");
		T->selectMipMapLevel(8);	f.glLoadTexture(T,"Datas\\M1_4.jpg");
		T->selectMipMapLevel(9);	f.glLoadTexture(T,"Datas\\M1_2.jpg");
		T->selectMipMapLevel(10);	f.glLoadTexture(T,"Datas\\M1_1.jpg");
		config.setGenerateMipmap(true);
		txt->addTexture(T);
	}
#endif

	square = glGenLists(1);
	glNewList(square, GL_COMPILE);
	glBegin(GL_QUADS);
		for (int j = 0; j<TY; j++)
		{
			for (int i = 0; i<TX; i++)
			{
				// Bottom left corner
				glTexCoord2f(i / TX, j / TY);
				glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

				// Bottom right corner
				glTexCoord2f((i + 1.0f) / TX, j / TY);
				glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

				// Upper right corner
				glTexCoord2f((i + 1.0f) / TX, (j + 1.0f) / TY);
				glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);

				// Upper left corner
				glTexCoord2f(i / TX, (j + 1.0f) / TY);
				glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);
			}
		}
		glEnd();
	glEndList();

	CVertexShader s("GL_SHADER");
	GL_COORD_VERTEX texCoord1(0, 0, 0, 1);
	square2 = glGenLists(1);
	glNewList(square2, GL_COMPILE);
	glBegin(GL_QUADS);
	for (int j = 0; j<TY; j++)
	{
		for (int i = 0; i<TX; i++)
		{
			// Bottom left corner
			texCoord1.x = i / TX; texCoord1.y = j / TY;
			s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
			s.glMultiTexCoord(CProgramParameters::TEXCOORD0, texCoord1);
			glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

			// Bottom right corner
			texCoord1.x = (i + 1.0f) / TX; texCoord1.y = j / TY;
			s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
			s.glMultiTexCoord(CProgramParameters::TEXCOORD0, texCoord1);
			glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

			// Upper right corner
			texCoord1.x = (i + 1.0f) / TX; texCoord1.y = (j + 1.0f) / TY;
			s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
			s.glMultiTexCoord(CProgramParameters::TEXCOORD0, texCoord1);
			glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);

			// Upper left corner
			texCoord1.x = i / TX; texCoord1.y = (j + 1.0f) / TY;
			s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
			s.glMultiTexCoord(CProgramParameters::TEXCOORD0, texCoord1);
			glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);
		}
	}
	glEnd();
	glEndList();
}


void Display::glDraw(void)
{
	if (draw == 4)
	{
		float r = ((float)(rand() % 256)) / 256.0f;
		float g = ((float)(rand() % 256)) / 256.0f;
		float b = ((float)(rand() % 256)) / 256.0f;

		CTimeObject::markTime(this);

		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}
	else if (draw == 5)
	{
		float z = ((float)(rand() % 256)) / 256.0f;

		CTimeObject::markTime(this);

		glClearDepth(z);
		glClear(GL_DEPTH_BUFFER_BIT);
		glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}
	else if (draw == 6)
	{
		CTimeObject::markTime(this);

		//glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}


	CTimeObject::markTime(this);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glMatrixMode(GL_TEXTURE);

	glPushMatrix();
	glTranslatef(0.5f, 0.5f, 0.0f);
	glRotatef(dt, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.5f, -0.5f, 0.0f);
	
	if (draw == 3)
	{
		glBegin(GL_QUADS);
		for (int j = 0; j < TY; j++)
		{
			for (int i = 0; i < TX; i++)
			{
				// Bottom left corner
				glTexCoord2f(dt*0.01f*i / TX, dt*0.01f*j / TY);
				glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

				// Bottom right corner
				glTexCoord2f(dt*0.01f*(i + 1.0f) / TX, dt*0.01f*j / TY);
				glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*j / TY - 1.0f), -2.0f);

				// Upper right corner
				glTexCoord2f(dt*0.01f*(i + 1.0f) / TX, dt*0.01f*(j + 1.0f) / TY);
				glVertex3f(viewScale*(2.0f*(i + 1) / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);

				// Upper left corner
				glTexCoord2f(dt*0.01f*i / TX, dt*0.01f*(j + 1.0f) / TY);
				glVertex3f(viewScale*(2.0f*i / TX - 1.0f), (2.0f*(j + 1.0f) / TY - 1.0f), -2.0f);
			}
		}
		glEnd();
	}
	else if (draw == 2)
	{
		CRaptorDisplay::glRender(setupHANDLE);
		glCallList(square2);
	}
	else
		glCallList(square);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();

	dt = dt + 0.2f;
	if (dt > 360.0f)
		dt = 0.0f;
	glFlush();

	totalTime += CTimeObject::deltaMarkTime(this);
}


extern "C" GLBENCH_API void Bench(CWnd *parent)
{
	if (parent == NULL)
		return;

	CHostMemoryManager::GetInstance()->setGarbageMaxSize(64000000);
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
	glCS.swap_buffer = CRaptorDisplayConfig::SWAP_EXCHANGE;
	glCS.display_mode = CGL_RGBA | CGL_DEPTH;
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE;
	GLDisplay->GLCreateWindow("OpenGL Context",parent,glCS);

	// Calibration;
	unsigned int	resultCount = 0;
	GLDisplay->glCalibrate(glCS.width, glCS.height);
	
	//
	//	Pixel transfer rate : Draw Pixels
	//
	{
		resultCount++;
		unsigned int nb = 0;
		unsigned char *pixels = new unsigned char[2048*2048*4];

GLDisplay->glMakeCurrent();
		CTimeObject::markTime(parent);
		for (unsigned int i=0;i<LOOP_DRAWPIXELS;i++)
		{
			glDrawPixels(glCS.width,glCS.height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
			nb++;
		}
		float bench_dt = CTimeObject::deltaMarkTime(parent);
GLDisplay->glMakeCurrent(false);

		double megatexelspersec = nb / bench_dt * glCS.width * glCS.height / 1000000.0;
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].fps_rate = nb / bench_dt;
		results.result_items[resultCount].fragment_rate = megatexelspersec;
		delete [] pixels;
	}

	//
	//	Pixel transfer rate : Texture Loading
	//
	CTextureObject *T = NULL;
	{
		resultCount++;
		unsigned int nb = 0;
		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		CTextureFactoryConfig &tfConfig = factory.getConfig();
		tfConfig.setTexelFormat(CTextureFactoryConfig::BYTEORDER_RGBA);
		tfConfig.useTextureResize(false);

		GLint maxSize = 1024;
		CImage load;

GLDisplay->glMakeCurrent();
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxSize);
		T = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA);
		load.allocatePixels(maxSize, maxSize);
		unsigned char *buffer = new unsigned char[maxSize*4*maxSize];

		float bench_dt = 0.0f;
		for (unsigned int i=0;i<LOOP_LOADTEXTURE;i++)
		{
			CTimeObject::markTime(parent);
			factory.glLoadTexture(T, load);
			bench_dt += CTimeObject::deltaMarkTime(parent);
			nb++;
		}
		
GLDisplay->glMakeCurrent(false);

		double megatexelspersec = nb / bench_dt* maxSize * maxSize / 1000000.0;
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].fps_rate = nb / bench_dt;
		results.result_items[resultCount].fragment_rate = megatexelspersec;


		resultCount++;
		nb = 0;
GLDisplay->glMakeCurrent();
		tfConfig.setTexelFormat(CTextureFactoryConfig::BYTEORDER_BGRA);
		
		bench_dt = 0.0f;
		for (unsigned int i=0;i<LOOP_LOADTEXTURE;i++)
		{
			CTimeObject::markTime(parent);
			factory.glLoadTexture(T, load);
			bench_dt += CTimeObject::deltaMarkTime(parent);
			nb++;
		}
GLDisplay->glMakeCurrent(false);

		megatexelspersec = nb / bench_dt * maxSize * maxSize / 1000000.0;
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].fps_rate = nb / bench_dt;
		results.result_items[resultCount].fragment_rate = megatexelspersec;

		T->releaseReference();
	}


	//
	//	Clear Color buffer
	//
	resultCount++;
	GLDisplay->draw = 4;
	GLDisplay->resetTotalTime();
	BenchStep(resultCount,LOOP_SIZE,GLDisplay);

	//
	//	Clear Z-buffer
	//
	resultCount++;
	GLDisplay->draw = 5;
	GLDisplay->resetTotalTime();
	BenchStep(resultCount,LOOP_SIZE,GLDisplay);

	//
	//	Bench 256*256
	//
	resultCount++;
	GLDisplay->draw = 0;
	GLDisplay->resetTotalTime();
GLDisplay->glMakeCurrent(true);
	T = GLDisplay->txt->getTexture(4);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,GLDisplay);

	//
	//	Bench 1024*1024
	//
	resultCount++;
	GLDisplay->draw = 0;
GLDisplay->glMakeCurrent(true);
	T = GLDisplay->txt->getTexture(0);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,GLDisplay);

	//
	//	Bench 1024*1024 compressed
	//
	resultCount++;
	GLDisplay->draw = 0;

	if (Raptor::glIsExtensionSupported("GL_ARB_texture_compression"))
	{
GLDisplay->glMakeCurrent(true);
		T = GLDisplay->txt->getTexture(1);
		T->glRender();
GLDisplay->glMakeCurrent(false);
		BenchStep(resultCount,LOOP_SIZE,GLDisplay);
	}


	//
	//	Bench 1024*1024 uncompressed blended
	//
	resultCount++;
	unsigned int nb = 0;
	GLDisplay->draw = 1;
	GLDisplay->resetTotalTime();

GLDisplay->glMakeCurrent(true);
	glClearColor(0.0f,0.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	T = GLDisplay->txt->getTexture(0);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,GLDisplay);

GLDisplay->glMakeCurrent(true);
	glDisable(GL_BLEND);
GLDisplay->glMakeCurrent(false);


	//
	//	Bench 1024*1024 multi-texturing
	//
	resultCount++;
	GLDisplay->draw = 2;
GLDisplay->glMakeCurrent(true);
	CTextureUnitSetup setup;
	setup.setDiffuseMap(GLDisplay->txt->getTexture(0));
	setup.setNormalMap(GLDisplay->txt->getTexture(2));
	/*RAPTOR_HANDLE*/ GLDisplay->setupHANDLE = setup.glBuildSetup();
	CRaptorDisplay::glRender(GLDisplay->setupHANDLE);
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,GLDisplay);


	//
	//	Bench 1024*1024 single texturing tri-linear filtering
	//
	resultCount++;
	GLDisplay->draw = 3;
GLDisplay->glMakeCurrent(true);
	setup.setDiffuseMap(GLDisplay->txt->getTexture(3));
	setup.setNormalMap(NULL);
	RAPTOR_HANDLE setupHANDLE2 = setup.glBuildSetup();
	CRaptorDisplay::glRender(setupHANDLE2);
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,GLDisplay);


	//
	//	Bench 1024*1024 single texturing anisotropic filtering
	//
	resultCount++;
	GLDisplay->draw = 3;
#if defined(GL_EXT_texture_filter_anisotropic)
	if (Raptor::glIsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
GLDisplay->glMakeCurrent(true);
		T = GLDisplay->txt->getTexture(5);
		T->glRender();
GLDisplay->glMakeCurrent(false);

		CTextureFactoryConfig &tfConfig = CTextureFactory::getDefaultFactory().getConfig();
		BenchStep(resultCount,LOOP_SIZE,GLDisplay);
	}
#endif

	//
	//	Bench done, release everything
	//
	GLDisplay->glMakeCurrent(false);
	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
