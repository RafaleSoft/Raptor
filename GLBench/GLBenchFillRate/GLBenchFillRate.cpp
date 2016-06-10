// GLBenchFillRate.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"

#include "Engine/TimeObject.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/VertexShader.h"
#include "System/Raptor.h"
#include "MFCExtension/CWnd/GLWnd.h"

RAPTOR_NAMESPACE

//
// static datas
//
#define LOOP_SIZE			2000
#define LOOP_LOADTEXTURE	100
#define LOOP_DRAWPIXELS		50

#define TX				4.0f
#define	TY				4.0f

static const size_t NB_RESULTS = 13;
static const char* NAME = "GL Bench Fill Rate";
static const char* DESCRIPTION = "Benchmark for pixel and texel fill rate";
static char RESULT_DESCRIPTION[NB_RESULTS][256] = 
	{	"HAL Swap buffers: ", "Pixel Transfer RGBA: ",
		"Texel Transfer RGBA: ", "Texel Transfer BGRA: ",
		"Clear color buffer: ", "Clear Z-buffer: ",
		"Single texturing small ( 256x256 ): ",
		"Single texturing large ( 1024x1024 ): ",
		"Single texturing compressed ( 1024x1024 ): ",
		"Single texturing blended ( 1024x1024 ): ",
		"Multi texturing ( 1024x1024 ): ",
		"Single texturing trilinear ( 1024x1024 ): ",
		"Single texturing anisotropic ( 1024x1024 ): "
	};

#include "GLBenchModuleTemplate.cxx"

//
/////////////////////////////////////////////////////////////////////////////

class Display : public CGLWnd
{
public:
	Display();
	virtual ~Display();

	virtual	void GLInitContext(void);
	virtual void GLDisplayFunc(void);

	int draw;
	float viewScale;
	CTextureSet *txt;

	float totalTime;

private:
	float dt;
	void Draw(void);
};

Display::Display()
{
	draw = 0;
	dt = 0.0f;
	totalTime = 0.0f;
	viewScale = 1.33f;
}

Display::~Display()
{
	delete txt;
}

void Display::Draw(void)
{
	CVertexShader s("GL_SHADER");
	GL_COORD_VERTEX texCoord1(0,0,0,1);
	
	glBegin(GL_QUADS);
		for (int j=0;j<TY;j++)
		{
			for (int i=0;i<TX;i++)
			{
				// Bottom left corner
				if (draw == 2)
				{
					texCoord1.x = i/TX; texCoord1.y = j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
				}
				if (draw == 3)
				{
					texCoord1.x = dt*0.01f*i/TX; texCoord1.y = dt*0.01f*j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				else
				{
					texCoord1.x = i/TX; texCoord1.y = j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				glVertex3f(viewScale*(2.0f*i/TX-1.0f),(2.0f*j/TY-1.0f),-2.0f);

				// Bottom right corner
				if (draw == 2)
				{
					texCoord1.x = (i+1.0f)/TX; texCoord1.y = j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
				}
				if (draw == 3)
				{
					texCoord1.x = dt*0.01f*(i+1.0f)/TX; texCoord1.y = dt*0.01f*j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				else
				{
					texCoord1.x = (i+1.0f)/TX; texCoord1.y = j/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				glVertex3f(viewScale*(2.0f*(i+1)/TX-1.0f),(2.0f*j/TY-1.0f),-2.0f);

				// Upper right corner
				if (draw == 2)
				{
					texCoord1.x = (i+1.0f)/TX; texCoord1.y = (j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
				}
				if (draw == 3)
				{
					texCoord1.x = dt*0.01f*(i+1.0f)/TX; texCoord1.y = dt*0.01f*(j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				else
				{
					texCoord1.x = (i+1.0f)/TX; texCoord1.y = (j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				glVertex3f(viewScale*(2.0f*(i+1)/TX-1.0f),(2.0f*(j+1.0f)/TY-1.0f),-2.0f);

				// Upper left corner
				if (draw == 2)
				{
					texCoord1.x = i/TX; texCoord1.y = (j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
				}
				if (draw == 3)
				{
					texCoord1.x = dt*0.01f*i/TX; texCoord1.y = dt*0.01f*(j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				else
				{
					texCoord1.x = i/TX; texCoord1.y = (j+1.0f)/TY;
					s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
				}
				glVertex3f(viewScale*(2.0f*i/TX-1.0f),(2.0f*(j+1.0f)/TY-1.0f),-2.0f);
			}
		}
	glEnd();
}

void Display::GLDisplayFunc()
{
	if (draw == 4)
	{
		CTimeObject::markTime(this);

		float r = ((float)(rand() % 256)) / 256.0f;
		float g = ((float)(rand() % 256)) / 256.0f;
		float b = ((float)(rand() % 256)) / 256.0f;
		glClearColor(r,g,b,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}
	else if (draw == 5)
	{
		CTimeObject::markTime(this);

		float z = ((float)(rand() % 256)) / 256.0f;
		glClearDepth(z);
		glClear(GL_DEPTH_BUFFER_BIT);
		glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}
	else if (draw == 6)
	{
		CTimeObject::markTime(this);

		glFlush();

		totalTime += CTimeObject::deltaMarkTime(this);
		return;
	}
	

	glEnable(GL_TEXTURE_2D);
	CTimeObject::markTime(this);

	glPushMatrix();
	glMatrixMode(GL_TEXTURE);

	glPushMatrix();
	glTranslatef(0.5f,0.5f,0.0f);
	glRotatef(dt,0.0f,0.0f,1.0f);
	glTranslatef(-0.5f,-0.5f,0.0f);
	Draw();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();

	dt = dt + 1.0f;
	if (dt > 360.0f)
		dt = 0.0f;
	glFlush();

	totalTime += CTimeObject::deltaMarkTime(this);
}


void Display::GLInitContext()
{
	glClearColor(0.0f,0.0f,0.0f,0.5f);

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
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
		CTextureFactoryConfig& config = f.getConfig();
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
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\M1_1024.jpg");
	T->selectMipMapLevel(1);
	f.glLoadTexture(T,"Datas\\M1_512.jpg");
	T->selectMipMapLevel(2);
	f.glLoadTexture(T,"Datas\\M1_256.jpg");
	T->selectMipMapLevel(3);
	f.glLoadTexture(T,"Datas\\M1_128.jpg");
	T->selectMipMapLevel(4);
	f.glLoadTexture(T,"Datas\\M1_64.jpg");
	T->selectMipMapLevel(5);
	f.glLoadTexture(T,"Datas\\M1_32.jpg");
	T->selectMipMapLevel(6);
	f.glLoadTexture(T,"Datas\\M1_16.jpg");
	T->selectMipMapLevel(7);
	f.glLoadTexture(T,"Datas\\M1_8.jpg");
	T->selectMipMapLevel(8);
	f.glLoadTexture(T,"Datas\\M1_4.jpg");
	T->selectMipMapLevel(9);
	f.glLoadTexture(T,"Datas\\M1_2.jpg");
	T->selectMipMapLevel(10);
	f.glLoadTexture(T,"Datas\\M1_1.jpg");
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
		CTextureFactoryConfig &tfConfig = f.getConfig();
		tfConfig.setCurrentAnisotropy(anisotropy);

		T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_ANISOTROPIC);
		T->glSetTransparency(255);
		f.glLoadTexture(T,"Datas\\M1_1024.jpg");
		T->selectMipMapLevel(1);
		f.glLoadTexture(T,"Datas\\M1_512.jpg");
		T->selectMipMapLevel(2);
		f.glLoadTexture(T,"Datas\\M1_256.jpg");
		T->selectMipMapLevel(3);
		f.glLoadTexture(T,"Datas\\M1_128.jpg");
		T->selectMipMapLevel(4);
		f.glLoadTexture(T,"Datas\\M1_64.jpg");
		T->selectMipMapLevel(5);
		f.glLoadTexture(T,"Datas\\M1_32.jpg");
		T->selectMipMapLevel(6);
		f.glLoadTexture(T,"Datas\\M1_16.jpg");
		T->selectMipMapLevel(7);
		f.glLoadTexture(T,"Datas\\M1_8.jpg");
		T->selectMipMapLevel(8);
		f.glLoadTexture(T,"Datas\\M1_4.jpg");
		T->selectMipMapLevel(9);
		f.glLoadTexture(T,"Datas\\M1_2.jpg");
		T->selectMipMapLevel(10);
		f.glLoadTexture(T,"Datas\\M1_1.jpg");
		txt->addTexture(T);
	}
#endif
}


void BenchStep(	unsigned int step,unsigned int size,
				float calibration,CGLWnd *display,unsigned int width, unsigned int height,
				const char* desc1, const char* desc2)
{
	unsigned int	nb=0;

	CTimeObject::markTime(display);

	for (unsigned i=0;i<size;i++)
	{
		display->SendMessage(WM_PAINT);
		nb++;
	}

	float dt = CTimeObject::deltaMarkTime(display);

	results.result_items[step].rate = nb / dt;

	double megatexelspersec = nb / double(dt - calibration) * width * height / 1000000.0;
	results.result_items[step].score = megatexelspersec;

	stringstream desc;
	desc << desc1 << megatexelspersec << desc2;
	strcat(RESULT_DESCRIPTION[step],desc.str().c_str());
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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-GLDisplay->viewScale,GLDisplay->viewScale,-1.0f,1.0f,1.0f,1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
GLDisplay->glMakeCurrent(false);

	//	frame rate management
	unsigned int	resultCount = 0;
	float	calibration;
	CTextureObject *T = NULL;


	//
	//	Swap buffers
	//
	GLDisplay->draw = 6;
	GLDisplay->totalTime = 0.0f;

	CTimeObject::markTime(parent);

    int i=0;
	unsigned int nb=0;
	for (i=0;i<LOOP_SIZE;i++)
	{
		GLDisplay->SendMessage(WM_PAINT);
		nb++;
	}

	calibration = CTimeObject::deltaMarkTime(parent);
	double megatexelspersec = nb / (calibration - GLDisplay->totalTime) * glCS.width * glCS.height / 1000000.0;

	results.result_items[resultCount].rate = nb / (calibration - GLDisplay->totalTime);
	results.result_items[resultCount].score = megatexelspersec;
	stringstream desc;
	desc << "swap buffer " << megatexelspersec << " Mega pixels / sec";
	strcat(RESULT_DESCRIPTION[resultCount],desc.str().c_str());

	//
	//	Pixel transfer rate : Draw Pixels
	//
	{
		resultCount++;
		nb = 0;
		unsigned char *pixels = new unsigned char[2048*2048*4];

GLDisplay->glMakeCurrent();
		CTimeObject::markTime(parent);
		for (i=0;i<LOOP_DRAWPIXELS;i++)
		{
			glDrawPixels(glCS.width,glCS.height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
			nb++;
		}
		float bench_dt = CTimeObject::deltaMarkTime(parent);
GLDisplay->glMakeCurrent(true);

		megatexelspersec = nb / bench_dt * glCS.width * glCS.height / 1000000.0;
		
		desc.str("Draw Pixels transfer ");
		desc << megatexelspersec << " Mega pixels / sec";
		strcat(RESULT_DESCRIPTION[resultCount],desc.str().c_str());
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].rate = nb / bench_dt;
		delete [] pixels;
	}

	//
	//	Pixel transfer rate : Texture Loading
	//
	{
		resultCount++;
		nb = 0;
		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		CTextureFactoryConfig &tfConfig = factory.getConfig();
		tfConfig.setTexelFormat(CTextureFactoryConfig::BYTEORDER_RGBA);

		GLint maxSize = 1024;

GLDisplay->glMakeCurrent(true);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxSize);
		T = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA);
		factory.glResizeTexture(T,maxSize,maxSize);
		unsigned char *buffer = new unsigned char[maxSize*4*maxSize];

		CTimeObject::markTime(parent);
		for (i=0;i<LOOP_LOADTEXTURE;i++)
		{
			factory.glLoadTexture(T,".buffer");
			nb++;
		}
		float bench_dt = CTimeObject::deltaMarkTime(parent);

		megatexelspersec = nb / bench_dt* maxSize * maxSize / 1000000.0;
		
		desc.str("");
		desc << "Texture (" << maxSize << ") texel transfer " << megatexelspersec << " Mega texels / sec";
		strcat(RESULT_DESCRIPTION[resultCount],desc.str().c_str());
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].rate = nb / bench_dt;


		resultCount++;
		nb = 0;
		tfConfig.setTexelFormat(CTextureFactoryConfig::BYTEORDER_BGRA);
		CTimeObject::markTime(parent);
		for (i=0;i<LOOP_LOADTEXTURE;i++)
		{
			factory.glLoadTexture(T,".buffer");
			nb++;
		}
		bench_dt = CTimeObject::deltaMarkTime(parent);

		megatexelspersec = nb / bench_dt * maxSize * maxSize / 1000000.0;
		
		desc.str("");
		desc << "Texture (" << maxSize << ") texel transfer " << megatexelspersec << " Mega texels / sec";
		strcat(RESULT_DESCRIPTION[resultCount],desc.str().c_str());
		results.result_items[resultCount].score = megatexelspersec;
		results.result_items[resultCount].rate = nb / bench_dt;

		T->releaseReference();
GLDisplay->glMakeCurrent(false);
	}


	//
	//	Clear Color buffer
	//
	resultCount++;
	GLDisplay->draw = 4;
	GLDisplay->totalTime = 0.0f;
	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
			"clear 32bits color buffer "," Mega pixels / sec");

	//
	//	Clear Z-buffer
	//
	resultCount++;
	GLDisplay->draw = 5;
	GLDisplay->totalTime = 0.0f;
	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
			"clear 24bits Z-buffer "," Mega pixels / sec");

	//
	//	Bench 256*256
	//
	resultCount++;
	GLDisplay->draw = 0;
	GLDisplay->totalTime = 0.0f;
GLDisplay->glMakeCurrent(true);
	T = GLDisplay->txt->getTexture(4);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
		"uncompressed bilinear "," Mega texels / sec");

	//
	//	Bench 1024*1024
	//
	resultCount++;
	GLDisplay->draw = 0;
GLDisplay->glMakeCurrent(true);
	T = GLDisplay->txt->getTexture(0);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
		"uncompressed bilinear "," Mega texels / sec");

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
		BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
			"compressed bilinear "," Mega texels / sec");
	}


	//
	//	Bench 1024*1024 uncompressed blended
	//
	resultCount++;
	nb = 0;
	GLDisplay->draw = 1;
	GLDisplay->totalTime = 0.0f;

GLDisplay->glMakeCurrent(true);
	glClearColor(0.0f,0.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	T = GLDisplay->txt->getTexture(0);
	T->glRender();
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
		"uncompressed blended bilinear "," Mega texels / sec");

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
	RAPTOR_HANDLE setupHANDLE = setup.glBuildSetup();
	CRaptorDisplay::glRender(setupHANDLE);
GLDisplay->glMakeCurrent(false);

	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
		"uncompressed modulate bilinear "," Mega texels / sec");


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

	BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
		"uncompressed trilinear "," Mega texels / sec");


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
		desc.str("");
		desc << "uncompressed anisotropy (" << tfConfig.getCurrentAnisotropy() << ") ";
		BenchStep(resultCount,LOOP_SIZE,calibration,GLDisplay,glCS.width,glCS.height,
			desc.str().c_str()," Mega texels / sec");
	}
#endif

	//
	//	Bench done, release everything
	//
	GLDisplay->glMakeCurrent(false);
	GLDisplay->DestroyWindow();
	delete GLDisplay;
}
