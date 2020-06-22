// ParticleDisplay.cpp: implementation of the CParticleDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParticleDisplay.h"


#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/Particle.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/ITextureObject.h"
#include "Engine/3DScene.h"
#include "Engine/ParticleManager.h"
#include "Engine/ImageModifier.h"
#include "System/Raptor.h"

#include "ToolBox/ParticleCompute.h"


static GL_COORD_VERTEX center;
static CImage smoke;


class MyCompute : public CParticleManager::CParticleCompute
{
public:
	MyCompute() {};
	virtual ~MyCompute() {};

	virtual std::string getModelName(void) const
	{ return "mycompute"; };
	virtual void compute(CParticle& particle,float dt);
	virtual void initialize(CParticle& particle);

private:
	void MyLifeProc(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
	void MyBirthProc(CParticle::PARTICLE_ATTRIBUTE& attrs);
};

void MyCompute::compute(CParticle& particle,float dt)
{
	// Life : compute only alive particules
	unsigned int nbDead = 0;
	size_t nbParticle = particle.getQuantity();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		MyLifeProc(attrs, dt);
		if(attrs.color.h < 0.0f)
	    {
			if(m_bRevive)
			{
				MyBirthProc(attrs);
				if (nbDead > 0)
					particle.getAttribute(i-nbDead) = attrs;
			}
			else
				nbDead++;
		}
	}
	particle.setQuantity(particle.getSize() - nbDead);
}

void MyCompute::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		MyBirthProc(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void MyCompute::MyBirthProc(CParticle::PARTICLE_ATTRIBUTE& particles)
{
	// Set initial position
	particles.position.x = ((rand() & 0x1F) - 16) * 0.0001f;
	particles.position.y = ((rand() & 0x3F)) * 0.0001f;
	particles.position.z = ((rand() & 0x1F) - 16) * 0.0001f;
	particles.position.h = 1.0f;

	// Set color
	particles.color.x = 1.0f;	// red
	particles.color.y = 1.0f;	// green
	particles.color.z = 1.0f;	// blue
	particles.color.h = 1.0f;	// alpha
	
	// Set size
	particles.size = ((rand() & 0x1F) - 16 ) * 0.1f + 4.0f;
	
	// Set fading rate
	particles.fade = float(rand() & 0x1F) * 0.001f + 0.0006f;

	// Set rotation angle
	particles.angle = float(rand() % 360);

	particles.position.x += center.x;
	particles.position.y += center.y;
	particles.position.z += center.z;
}

void MyCompute::MyLifeProc(CParticle::PARTICLE_ATTRIBUTE& particles,float dt)
{
	particles.color.h -= particles.fade;

	// Update size
	particles.size *= 1.00075f;

	// Update colors
	particles.color.x *= 0.99f;
	particles.color.y *= 0.99f;
	particles.color.z *= 0.99f;

	// Update rotation angle
	if ((rand() % 2) == 0)
		particles.angle += 200 * particles.fade;
	else
		particles.angle -= 200 * particles.fade;

	if (particles.angle > 360.0f)
		particles.angle -= 360.0f;
	else if (particles.angle < 0.0f)
		particles.angle += 360.0f;
}


class CubeMapModifier : public CImageModifier
{
public:
    CubeMapModifier():CImageModifier(CModifier::CGL_TIME_CONSTANT) {};
    virtual ~CubeMapModifier() {};

    virtual void onPrepareBuffer(void)
    {
        unsigned char *src = getFrontBuffer();
	    //	add ramdom points
	    for (int j=0;j<20;j++)
	    {
			// select line
			int pos = (128*4) * (rand() % 48 + 40);
			// select column
			pos += 4 * (rand() % 48 + 40);

		    src[pos] = 255;
		    src[pos+1] = 255;
		    src[pos+2] = 255;
		    src[pos+3] = 96 + (rand() % 128);
	    }
    }

	unsigned char * const getBuffer(void) const 
	{ return CImageModifier::getFrontBuffer(); };
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleDisplay::CParticleDisplay():
	m_pFire(NULL),
	m_pSmoke(NULL),
    m_pParticle(NULL),
    m_pSmoke2(NULL),
    m_pParticleManager(NULL),
	m_pModifier(NULL),
	m_pScene(NULL),
	dt(0.0),
	repeat(0),numframe(0)
{
}


//////////////////////////////////////////////////////////////////////

void CParticleDisplay::Init()
{
	CGenericDisplay::Init();

    m_pParticleManager = new CParticleManager();

	// Create particles cluster
	CParticleCompute::CFireParticle *computeFire = new CParticleCompute::CFireParticle();
	CParticle *pFire = m_pParticleManager->glCreateParticle(500,true,CParticle::CGL_PARTICLE_TEXTURE,CParticle::CGL_ALPHA_BLEND,computeFire,"FIRE");
	
	CParticleCompute::CSmokeParticle *computeSmoke = new CParticleCompute::CSmokeParticle();
	CParticle *pSmoke = m_pParticleManager->glCreateParticle(100,true,CParticle::CGL_PARTICLE_TEXTURE,CParticle::CGL_COLOR_BLEND,computeSmoke,"SMOKE");

	MyCompute *myCompute = new MyCompute();
	CParticle *pParticle = m_pParticleManager->glCreateParticle(200, true,CParticle::CGL_PARTICLE_TEXTURE,CParticle::CGL_ALPHA_TRANSPARENT,myCompute,"STATIC");
	
	//pParticle->usePointSprite(false,30.0f);
	CParticleCompute::CSmokeParticle *computeSmoke2 = new CParticleCompute::CSmokeParticle();
    CParticle *pSmoke2 = m_pParticleManager->glCreateParticle(75,true,CParticle::CGL_PARTICLE_VOLUMETRIC,CParticle::CGL_COLOR_BLEND,computeSmoke2,"SMOKE2");
	
	// Load texture
	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CTextureSet* pTextures = (CTextureSet*)CPersistence::FindObject("main_textures");
	ITextureObject* T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_BILINEAR);
	f.glLoadTexture(T, "Datas\\particle.jpg");
	pTextures->addTexture(T);

	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
	f.glLoadTexture(T, "Datas\\Fire.tga");
	pTextures->addTexture(T);
	pFire->setTexture(T);

	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
	f.glSetTransparency(T, 0);
	f.glLoadTexture(T, "Datas\\particle.tga");
	pTextures->addTexture(T);
	pSmoke->setTexture(T);

	pParticle->setTexture(T);

    T = f.glCreateVolumeTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
    T->setSize(128,128,128);
	f.glSetTransparency(T, 255);
	
	smoke.allocatePixels(128, 128, 128);
	pTextures->addTexture(T);
	pSmoke2->setTexture(T);

	m_pModifier = new CubeMapModifier();
    m_pModifier->selectModifierFunction(CImageModifier::CGL_BLOWFADER_MODIFIER,0x05020202);
	m_pModifier->setImage(T);
    
	m_pSmoke2 = new CObject3DInstance(pSmoke2);
	m_pSmoke2->translate(15.0f,-10.0f,-20.0f);
	pSmoke2->getProperties().setVisible(false);	// non visible until texture is computed.
	
	m_pFire = new CObject3DInstance(pFire);
	m_pFire->translate(0.0f, -12.0f, -20.0f);
	m_pSmoke = new CObject3DInstance(pSmoke);
	m_pSmoke->translate(0.0f, -10.0f, -20.0f);
	m_pParticle = new CObject3DInstance(pParticle);
	m_pParticle->translate(-15.0f,-10.0f,-20.0f);

	m_pScene = new C3DScene("PARTICLE_SCENE");
	//m_pScene->useZSort();
	
	m_pScene->addObject(m_pSmoke2);
	m_pScene->addObject(m_pSmoke);
	m_pScene->addObject(m_pFire);
	m_pScene->addObject(m_pParticle);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(m_pScene);
}

//////////////////////////////////////////////////////////////////////

void CParticleDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	// Reset particles
	m_pParticleManager->glInitParticle((CParticle*)m_pParticle->getObject());
	m_pParticleManager->glInitParticle((CParticle*)m_pSmoke->getObject());
	m_pParticleManager->glInitParticle((CParticle*)m_pFire->getObject());
	m_pParticleManager->glInitParticle((CParticle*)m_pSmoke2->getObject());

	m_pParticleManager->animate(true);
	m_pModifier->animate(true);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->selectScene("PARTICLE_SCENE");
}

void CParticleDisplay::UnInit()
{
}

//////////////////////////////////////////////////////////////////////

void CParticleDisplay::Display()
{
	if (reinit)
		ReInit();
	
	CParticle *pParticle = (CParticle*)(m_pParticle->getObject());

	m_pParticle->rotationY(-0.1f);
	pParticle->translate(0.1f * cos(TWO_PI * 2 * dt),0.1f * sin(TWO_PI * dt),0);
	// Compute delta time
	dt += 0.002f;
	if(dt > 1.0)
	{
		dt = 0.0;
		pParticle->translateAbsolute(0.1f,0.0f,0.0f);
	}

	pParticle->getCenter(center);

	numframe++;
	if (numframe > 127)
	{
		numframe = 0;
		repeat ++;
	}

	if (repeat == 3)
	{
		CParticle *pSmoke2 = (CParticle*)(m_pSmoke2->getObject());
		unsigned char *data = smoke.getPixels(numframe);
		unsigned char *src = ((CubeMapModifier*)m_pModifier)->getBuffer();

		memcpy(data,src,128*128*4);
		memset(data,0,128*8); // clear first two lines
		memset(&data[128*126*4],0,128*8); // clear last two lines

		if (numframe == 127)
		{
			pSmoke2->getTexture()->glvkRender();
			CTextureFactory &f = CTextureFactory::getDefaultFactory();
			f.glLoadTexture(pSmoke2->getTexture(), smoke);
			smoke.releasePixels();
			pSmoke2->getProperties().setVisible(true);
		}
	}
}
