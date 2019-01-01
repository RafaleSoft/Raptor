// BasicObjects.cpp: implementation of the CBasicObjects class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PARTICLECOMPUTE_H__395259D4_3C60_4233_8D81_1F7B58BED8A5__INCLUDED_)
	#include "ParticleCompute.h"
#endif


#include <stdlib.h>	// rand

#define NB_RANDVALUES 2048
static long LOCALRAND[NB_RANDVALUES];
static int T = NB_RANDVALUES;
long myRand(void)
{
    if (T >= NB_RANDVALUES)
    {
        for (unsigned int i=0;i<NB_RANDVALUES;i++)
            LOCALRAND[i] = rand();
        T = 1;
        return LOCALRAND[0];
    }
    else
    {
        if (T == NB_RANDVALUES)
            T = 0;
        return LOCALRAND[T++];
    }
}

bool CParticleCompute::installComputers()
{
	CParticleManager::CParticleCompute *p = NULL;

	p = new CRamdomParticle();
	CParticleManager::setComputeModel(p,p->getModelName());
	p = new CFireParticle();
	CParticleManager::setComputeModel(p,p->getModelName());
	p = new CSmokeParticle();
	CParticleManager::setComputeModel(p,p->getModelName());
	p = new CStaticParticle();
	CParticleManager::setComputeModel(p,p->getModelName());

	return true;
}

bool CParticleCompute::installComputer(const std::string& computeModel)
{
	bool ret = true;

	// TODO : use getModelName made static instead of hard coded names
	if (computeModel == "random")
		CParticleManager::setComputeModel(new CRamdomParticle(), computeModel);
	else if (computeModel == "fire")
		CParticleManager::setComputeModel(new CFireParticle(), computeModel);
	else if (computeModel == "smoke")
		CParticleManager::setComputeModel(new CSmokeParticle(), computeModel);
	else if (computeModel == "static")
		CParticleManager::setComputeModel(new CStaticParticle(), computeModel);
	else
		ret = false;

	return ret;
}


//////////////////////////////////////////////////////////////////////
//
//	CRamdomParticle
//
//////////////////////////////////////////////////////////////////////
CParticleCompute::CRamdomParticle::CRamdomParticle(void)
	:CParticleManager::CParticleCompute()
{
	m_fadeFactors = GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f);
    m_scaleFactor = 0.0f;
}

CParticleCompute::CRamdomParticle::~CRamdomParticle(void)
{
}

void CParticleCompute::CRamdomParticle::compute(CParticle& particle,float dt)
{
	// Life : compute only alive particules
	unsigned int nbDead = 0;
	size_t nbParticle = particle.getQuantity();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeLife(attrs, dt);
		if(attrs.color.h < 0.0f)
	    {
			if(m_bRevive)
			{
				computeBirth(attrs);
				if (nbDead > 0)
					particle.getAttribute(i-nbDead) = attrs;
			}
			else
				nbDead++;
		}
	}
	particle.setQuantity(particle.getSize() - nbDead);
}

void CParticleCompute::CRamdomParticle::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeBirth(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void CParticleCompute::CRamdomParticle::computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt)
{
	// Compute displacement : pull is added to speed (acceleration)
#ifdef RAPTOR_SSE_CODE_GENERATION
	CSSERegister0 sse0;
	CSSERegister1 sse1;
    CSSERegister2 sse2;

	float *s = attrs.speed;
	float *p = attrs.position;
    float DT[4] = { dt, dt, dt, 1.0f };
	sse1 = attrs.pull;
	sse0 = s;
    sse2 = DT;
    sse1 *= sse2;
	sse0 += sse1;
	sse0.Save(s);
	sse1 = p;
    sse0 *= sse2;
	sse0 += sse1;
	sse0.Save(p);
#else
	attrs.speed.x += dt * attrs.pull.x;
	attrs.speed.y += dt * attrs.pull.y;
	attrs.speed.z += dt * attrs.pull.z;
	attrs.position.x += dt * attrs.speed.x;
	attrs.position.y += dt * attrs.speed.y;
	attrs.position.z += dt * attrs.speed.z;
#endif

	// Fade particule brigthness
	float f = fabs(dt * attrs.fade);
	attrs.color.x -= f * m_fadeFactors.x;
	attrs.color.y -= f * m_fadeFactors.y;
	attrs.color.z -= f * m_fadeFactors.z;
	attrs.color.h -= f * m_fadeFactors.h;

	// Enlarge smoke particle
	attrs.size += dt * m_scaleFactor;

	// Update rotation angle
	attrs.angle += dt * attrs.fade * 360.0f;
	if (attrs.angle >= 360.0f)
		attrs.angle -= 360.0f;
}

void CParticleCompute::CRamdomParticle::computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs)
{
	// Set initial position
	attrs.position.x = 0.0f;
	attrs.position.y = 0.0f;
	attrs.position.z = 0.0f;
	attrs.position.h = 1.0f;

	// Set speed
	attrs.speed.x = ((myRand() & 0x1F) - 16) * 0.0001f;
	attrs.speed.y = ((myRand() & 0x1F) - 16) * 0.0001f;
	attrs.speed.z = ((myRand() & 0x1F) - 16) * 0.0001f;
	attrs.speed.h = 0.0f;

	// Set pull
	attrs.pull.x = ((myRand() & 0x1F) - 16) * 0.00001f;
	attrs.pull.y = ((myRand() & 0x1F) - 16) * 0.00001f;
	attrs.pull.z = ((myRand() & 0x1F) - 16) * 0.00001f;
	attrs.pull.h = 0.0f;

	// Set color
	attrs.color.x = (myRand() & 0x0FF) * ONE_OVER_256_F;	// red
	attrs.color.y = (myRand() & 0x0FF) * ONE_OVER_256_F;	// green
	attrs.color.z = (myRand() & 0x0FF) * ONE_OVER_256_F;	// blue
	attrs.color.h = 1.0f;									// alpha
	
	// Set size
	attrs.size = 1.0f;
	
	// Set fading rate
	attrs.fade = float(myRand() & 0x7F) * 0.001f + 0.001f;

	// Set rotation angle
	attrs.angle = float(myRand() % 360);
}



//////////////////////////////////////////////////////////////////////
//
//	CFireParticle
//
//////////////////////////////////////////////////////////////////////
CParticleCompute::CFireParticle::CFireParticle(void)
	:CParticleManager::CParticleCompute()
{
	m_fadeFactors = GL_COORD_VERTEX(1.2f,2.0f,1.0f,1.0f);
    m_scaleFactor = 6.0f;
}

CParticleCompute::CFireParticle::~CFireParticle(void)
{
}

void CParticleCompute::CFireParticle::compute(CParticle& particle,float dt)
{
	// Life : compute only alive particules
	unsigned int nbDead = 0;
	size_t nbParticle = particle.getQuantity();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeLife(attrs, dt);
		if(attrs.color.h < 0.0f)
	    {
			if(m_bRevive)
			{
				computeBirth(attrs);
				if (nbDead > 0)
					particle.getAttribute(i-nbDead) = attrs;
			}
			else
				nbDead++;
		}
	}
	particle.setQuantity(particle.getSize() - nbDead);
}

void CParticleCompute::CFireParticle::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeBirth(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void CParticleCompute::CFireParticle::computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt)
{
	// Compute displacement : pull is added to speed (acceleration)
#ifdef RAPTOR_SSE_CODE_GENERATION
	CSSERegister0 sse0;
	CSSERegister1 sse1;
    CSSERegister2 sse2;

	float *s = attrs.speed;
	float *p = attrs.position;
    float DT[4] = { dt, dt, dt, 1.0f };
	sse1 = attrs.pull;
	sse0 = s;
    sse2 = DT;
    sse1 *= sse2;
	sse0 += sse1;
	sse0.Save(s);
	sse1 = p;
    sse0 *= sse2;
	sse0 += sse1;
	sse0.Save(p);
#else
	attrs.speed.x += dt * attrs.pull.x;
	attrs.speed.y += dt * attrs.pull.y;
	attrs.speed.z += dt * attrs.pull.z;
	attrs.position.x += dt * attrs.speed.x;
	attrs.position.y += dt * attrs.speed.y;
	attrs.position.z += dt * attrs.speed.z;
#endif

	// Fade particule brigthness
	float f = fabs(dt * attrs.fade);
	attrs.color.x -= f * m_fadeFactors.x;
	attrs.color.y -= f * m_fadeFactors.y;
	attrs.color.z -= f * m_fadeFactors.z;
	attrs.color.h -= f * m_fadeFactors.h;

	// Enlarge smoke particle
	attrs.size += dt * m_scaleFactor;

	// Update rotation angle
	attrs.angle += dt * attrs.fade * 360.0f;
	if (attrs.angle >= 360.0f)
		attrs.angle -= 360.0f;
}

void CParticleCompute::CFireParticle::computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs)
{
	// Set initial position
	attrs.position.x = ((myRand() & 0x1F) - 16) * 0.05f;
	attrs.position.y = ((myRand() & 0x3F) - 32) * 0.05f;
	attrs.position.z = ((myRand() & 0x1F) - 16) * 0.05f;
	attrs.position.h = 1.0f;

	// Set speed
	attrs.speed.x = ((myRand() & 0x3F) - 32) * 0.01f;
	attrs.speed.y = 20.0f;//((myRand() & 0x1F)) * 0.002f;
	attrs.speed.z = ((myRand() & 0x1F) - 16) * 0.01f;
	attrs.speed.h = 0.0f;

	// Set pull
	attrs.pull.x = 0.0f;//((myRand() & 0x3F) - 32) * 0.000001f;
	attrs.pull.y = 0.0f;//((myRand() & 0x3F)) * 0.00001f;
	attrs.pull.z = 0.0f;
	attrs.pull.h = 0.0f;

	// Set color
	attrs.color.x = 1.0f;	// red
	attrs.color.y = 1.0f;	// green
	attrs.color.z = 1.0f;	// blue
	attrs.color.h = 1.0f;	// alpha
	
	// Set size
    attrs.size = (myRand() & 0x1F) * 0.12f;
	
	// Set fading rate
    attrs.fade = float(myRand() & 0x7F) * 0.4f + 1.0f;

	// Set rotation angle
    attrs.angle = float(myRand() % 360);
}


//////////////////////////////////////////////////////////////////////
//
//	CSmokeParticle
//
//////////////////////////////////////////////////////////////////////
CParticleCompute::CSmokeParticle::CSmokeParticle(void)
	:CParticleManager::CParticleCompute()
{
	m_fadeFactors = GL_COORD_VERTEX(1.0f,1.0f,1.0f,1.0f);
    m_scaleFactor = 3.0f;
}

CParticleCompute::CSmokeParticle::~CSmokeParticle(void)
{
}

void CParticleCompute::CSmokeParticle::compute(CParticle& particle,float dt)
{
	// Life : compute only alive particules
	unsigned int nbDead = 0;
	size_t nbParticle = particle.getQuantity();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeLife(attrs, dt);
		if(attrs.color.h < 0.0f)
	    {
			if(m_bRevive)
			{
				computeBirth(attrs);
				if (nbDead > 0)
					particle.getAttribute(i-nbDead) = attrs;
			}
			else
				nbDead++;
		}
	}
	particle.setQuantity(particle.getSize() - nbDead);
}

void CParticleCompute::CSmokeParticle::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeBirth(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void CParticleCompute::CSmokeParticle::computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt)
{
	// Compute displacement : pull is added to speed (acceleration)
#ifdef RAPTOR_SSE_CODE_GENERATION
	CSSERegister0 sse0;
	CSSERegister1 sse1;
    CSSERegister2 sse2;

	float *s = attrs.speed;
	float *p = attrs.position;
    float DT[4] = { dt, dt, dt, 1.0f };
	sse1 = attrs.pull;
	sse0 = s;
    sse2 = DT;
    sse1 *= sse2;
	sse0 += sse1;
	sse0.Save(s);
	sse1 = p;
    sse0 *= sse2;
	sse0 += sse1;
	sse0.Save(p);
#else
	attrs.speed.x += dt * attrs.pull.x;
	attrs.speed.y += dt * attrs.pull.y;
	attrs.speed.z += dt * attrs.pull.z;
	attrs.position.x += dt * attrs.speed.x;
	attrs.position.y += dt * attrs.speed.y;
	attrs.position.z += dt * attrs.speed.z;
#endif

	// Fade particule brigthness
	float f = fabs(dt * attrs.fade);
	attrs.color.x -= f * m_fadeFactors.x;
	attrs.color.y -= f * m_fadeFactors.y;
	attrs.color.z -= f * m_fadeFactors.z;
	attrs.color.h -= f * m_fadeFactors.h;

	// Enlarge smoke particle
	attrs.size += dt * m_scaleFactor;

	// Update rotation angle
	attrs.angle += dt * attrs.fade * 360.0f;
	if (attrs.angle >= 360.0f)
		attrs.angle -= 360.0f;
}

void CParticleCompute::CSmokeParticle::computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs)
{
	// Set initial position
	attrs.position.x = ((myRand() & 0x1F) - 16) * 0.001f;
	attrs.position.y = ((myRand() & 0x3F)) * 0.001f;
	attrs.position.z = ((myRand() & 0x1F) - 16) * 0.001f;
	attrs.position.h = 1.0f;

	// Set speed
	attrs.speed.x = ((myRand() & 0x1F) - 16 ) * 0.05f;
	attrs.speed.y = 6.0f;//((myRand() & 0x1F) ) * 0.001f + 0.05f;
	attrs.speed.z = 0.0f;//((myRand() & 0x1F) ) * 0.001f;
	attrs.speed.h = 0.0f;

	// Set pull
	attrs.pull.x = 0.0f;
	attrs.pull.y = -(myRand() & 0x1F) * 0.00005f;
	attrs.pull.z = 0.0f;
	attrs.pull.h = 0.0f;

	// Set color
	float f = (myRand() % 50) * 0.01f + 0.5f;
	attrs.color.x = f;	// red
	attrs.color.y = f;	// green
	attrs.color.z = f;	// blue
	attrs.color.h = f;		// alpha
	
	// Set size
	attrs.size = ((myRand() & 0x1F) - 16 ) * 0.05f + 4.0f;
	
	// Set fading rate
	attrs.fade = float((myRand() & 0x1F) - 16) * 0.2f + 0.1f;

	// Set rotation angle
	attrs.angle = float(myRand() % 360);
}

//////////////////////////////////////////////////////////////////////
//
//	CStaticParticle
//
//////////////////////////////////////////////////////////////////////
CParticleCompute::CStaticParticle::CStaticParticle(void)
	:CParticleManager::CParticleCompute()
{
	m_fadeFactors = GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f);
    m_scaleFactor = 0.0f;
}

CParticleCompute::CStaticParticle::~CStaticParticle(void)
{
}

void CParticleCompute::CStaticParticle::compute(CParticle& particle,float dt)
{
	// Life : compute only alive particules
	unsigned int nbDead = 0;
	size_t nbParticle = particle.getQuantity();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeLife(attrs, dt);
		if(attrs.color.h < 0.0f)
	    {
			if(m_bRevive)
			{
				computeBirth(attrs);
				if (nbDead > 0)
					particle.getAttribute(i-nbDead) = attrs;
			}
			else
				nbDead++;
		}
	}
	particle.setQuantity(particle.getSize() - nbDead);
}

void CParticleCompute::CStaticParticle::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeBirth(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void CParticleCompute::CStaticParticle::computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt)
{
	// Compute displacement : pull is added to speed (acceleration)
#ifdef RAPTOR_SSE_CODE_GENERATION
	CSSERegister0 sse0;
	CSSERegister1 sse1;
    CSSERegister2 sse2;

	float *s = attrs.speed;
	float *p = attrs.position;
    float DT[4] = { dt, dt, dt, 1.0f };
	sse1 = attrs.pull;
	sse0 = s;
    sse2 = DT;
    sse1 *= sse2;
	sse0 += sse1;
	sse0.Save(s);
	sse1 = p;
    sse0 *= sse2;
	sse0 += sse1;
	sse0.Save(p);
#else
	attrs.speed.x += dt * attrs.pull.x;
	attrs.speed.y += dt * attrs.pull.y;
	attrs.speed.z += dt * attrs.pull.z;
	attrs.position.x += dt * attrs.speed.x;
	attrs.position.y += dt * attrs.speed.y;
	attrs.position.z += dt * attrs.speed.z;
#endif

	// Fade particule brigthness
	float f = fabs(dt * attrs.fade);
	attrs.color.x -= f * m_fadeFactors.x;
	attrs.color.y -= f * m_fadeFactors.y;
	attrs.color.z -= f * m_fadeFactors.z;
	attrs.color.h -= f * m_fadeFactors.h;

	// Enlarge smoke particle
	attrs.size += dt * m_scaleFactor;

	// Update rotation angle
	attrs.angle += dt * attrs.fade * 360.0f;
	if (attrs.angle >= 360.0f)
		attrs.angle -= 360.0f;
}

void CParticleCompute::CStaticParticle::computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs)
{
	// Set initial position
	attrs.position.x = ((myRand() & 0x1F) - 16) * 0.0001f;
	attrs.position.y = ((myRand() & 0x3F)) * 0.0001f;
	attrs.position.z = ((myRand() & 0x1F) - 16) * 0.0001f;
	attrs.position.h = 1.0f;

	// Set speed
	attrs.speed.x = 0.0f;
	attrs.speed.y = 0.0f;
	attrs.speed.z = 0.0f;
	attrs.speed.h = 0.0f;

	// Set pull
	attrs.pull.x = 0.0f;
	attrs.pull.y = 0.0f;
	attrs.pull.z = 0.0f;
	attrs.pull.h = 0.0f;

	// Set color
	float f = (myRand() % 100) * 0.01f;
	attrs.color.x = f;	// red
	attrs.color.y = f;	// green
	attrs.color.z = f;	// blue
	attrs.color.h = f;	// alpha
	
	// Set size
	attrs.size = ((myRand() & 0x1F) - 16 ) * 0.05f + 4.0f;
	
	// Set fading rate
	attrs.fade = float(myRand() & 0x1F) * 0.0005f + 0.0005f;

	// Set rotation angle
	attrs.angle = float(myRand() % 360);
}

