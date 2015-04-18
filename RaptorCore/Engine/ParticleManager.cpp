// ParticleManager.cpp: implementation of the CParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PARTICLEMANAGER_H__687B445A_C015_43DA_95A4_EC8AB1AED606__INCLUDED_)
    #include "ParticleManager.h"
#endif
#ifndef __INTERNAL_PROCS_H__
	#include "Subsys/InternalProcs.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static CParticleManager::CParticleManagerClassID particleId;
const CPersistence::CPersistenceClassID& CParticleManager::CParticleManagerClassID::GetClassId(void)
{
	return particleId;
}

static CPersistentType<CParticleManager> particleFactory(particleId);
static map<std::string,CParticleManager::CParticleCompute*> m_ComputeModels;

size_t	CParticleManager::avgParallelTasks = 0;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleManager::CParticleManager(const std::string& name)
    :CModifier(CGL_TIME_CONSTANT, 0, 0, 0, 0, NULL,particleId,name)
{
#if defined(RAPTOR_SSE_CODE_GENERATION)
	// Request at least one task per processor.
	if (avgParallelTasks == 0)
		avgParallelTasks = getCPUINFO().numCores;
#endif
}

CParticleManager::~CParticleManager()
{
    for (unsigned int i=0;i<m_pParticles.size();i++)
    {
        PARTICLE_SYSTEM ps = m_pParticles[i];
		ps.system->unregisterDestruction(this);
        delete ps.system;
    }
    m_pParticles.clear();
}

void CParticleManager::unLink(const CPersistence* obj)
{
	for (size_t i=0;i<m_pParticles.size();i++)
	{
		PARTICLE_SYSTEM ps = m_pParticles[i];
		if (obj == static_cast<CPersistence*>(ps.system))
		{
			m_pParticles.erase(m_pParticles.begin() + i);
		}
	}
}

void CParticleManager::setComputeModel(CParticleCompute* compute,const std::string& model)
{
	if ((compute != NULL) && !model.empty())
	{
		m_ComputeModels[model] = compute;
	}
}

void CParticleManager::glInitParticle(CParticle *p)
{
    for (unsigned int i=0;i<m_pParticles.size();i++)
    {
        const PARTICLE_SYSTEM &ps = m_pParticles[i];

        if (ps.system == p)
        {
			ps.compute->initialize(*p);

			//	create caches and shared data
			p->glInitParticle();

			CATCH_GL_ERROR
        }
    }
}

CParticle *CParticleManager::glCreateParticle(unsigned int number,
											  bool revive,
											  CParticle::PARTICLE_RENDERING type,
											  CParticle::PARTICLE_BLENDING blend,
											  const std::string& computeModel,
											  const std::string& name)
{
	map<std::string,CParticleManager::CParticleCompute*>::iterator it = 
		m_ComputeModels.find(computeModel);

	if (it != m_ComputeModels.end())
	{
		return glCreateParticle(number,revive,type,blend,(*it).second,name);
	}
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CParticleManager::CParticleManagerClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_PROCEDURE_FAILED);
#endif
		return NULL;
	}
}

CParticle *CParticleManager::glCreateParticle(unsigned int number, 
											  bool revive, 
											  CParticle::PARTICLE_RENDERING type,
											  CParticle::PARTICLE_BLENDING blend,
											  CParticleCompute *compute,
                                              const std::string& name)
{
	if (0 != compute)
	{
		PARTICLE_SYSTEM ps;
		ps.system = new CParticle(number,type,blend,name);
		ps.system->registerDestruction(this);
		ps.compute = compute;

		ps.compute->setRevive(revive);
		ps.compute->initialize(*ps.system);

		//	create caches and shared data
		ps.system->glInitParticle();
		m_pParticles.push_back(ps);

		CATCH_GL_ERROR
		return ps.system;
	}
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CParticleManager::CParticleManagerClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_PROCEDURE_FAILED);
#endif
		return NULL;
	}
}

void RAPTOR_FASTCALL CParticleManager::deltaTime(float dt)
{
	CModifier::deltaTime(dt);

	int j = 0;
	size_t jMax = m_pParticles.size();
	// depends on compute time & nb processors available.
	if (jMax > avgParallelTasks)
	{
#pragma omp parallel for default(none) private (j) firstprivate(dt,jMax)
		for (j=0;j<jMax;j++)
		{
			const PARTICLE_SYSTEM &ps = m_pParticles[j];

			if (ps.compute != NULL)
				ps.compute->compute(*ps.system,dt);
		}
	}
	else for (j=0;j<jMax;j++)
		{
			const PARTICLE_SYSTEM &ps = m_pParticles[j];

			if (ps.compute != NULL)
				ps.compute->compute(*ps.system,dt);
		}
}

bool CParticleManager::exportObject(CRaptorIO& o)
{
    return false;
}

CTextureObject* CParticleManager::importTexture(CRaptorIO &io)
{
    string name;
    io >> name;

	CTextureObject *t = NULL;
    string setName = "";
    string textureName = "";

	string data = io.getValueName();
    while (!data.empty())
    {
        if (data == "set")
            io >> setName;
        else if (data == "texname")
            io >> textureName;
        else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

    if (!setName.empty() && !textureName.empty())
    {
        CPersistence *p = CPersistence::FindObject(setName);
        if ((p != NULL) && 
			(p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId())))
        {
            CTextureSet *tset = (CTextureSet*)p;
            t = tset->getTexture(textureName);
        }
    }

    return t;
}

bool CParticleManager::importParticle(CRaptorIO& io)
{
    string name;
    io >> name;

    CParticle *particle = NULL;
    string particleName = "GL_PARTICLE";
    unsigned int size = 100;
    bool revive = true;
	bool sprite = false;
	float sFactor = 1.0f;
	float pSize = 1.0f;
	GL_COORD_VERTEX fFactors(1.0f,1.0f,1.0f,1.0f);
    CParticle::PARTICLE_RENDERING type = CParticle::CGL_PARTICLE_POINT;
    CParticle::PARTICLE_BLENDING blend = CParticle::CGL_ALPHA_TRANSPARENT;
	string computeModel = "";
	CTextureObject *t = NULL;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			io >> particleName;
        else if (data == "systemSize")
            io >> size;
		else if (data == "scaleFactor")
            io >> sFactor;
		else if (data == "FadeFactors")
            io >> fFactors;
        else if (data == "revive")
        {
            int bVal = 0;
            io >> bVal;
            revive = (bVal > 0);
        }
		else if (data == "sprite")
        {
            int bVal = 0;
            io >> bVal;
            sprite = (bVal > 0);
        }
        else if (data == "pointSize")
            io >> pSize;
        else if (data == "type")
        {
            io >> name;
            if (name == "point")
                type = CParticle::CGL_PARTICLE_POINT;
            else if (name == "line")
                type = CParticle::CGL_PARTICLE_LINE;
            else if (name == "texture")
                type = CParticle::CGL_PARTICLE_TEXTURE;
            else if (name == "volumetric")
                type = CParticle::CGL_PARTICLE_VOLUMETRIC;
        }
        else if (data == "model")
        {
            io >> name;
            if (name == "fire")
				blend = CParticle::CGL_ALPHA_BLEND;
            else if (name == "smoke")
                blend = CParticle::CGL_COLOR_BLEND;
			else if (name == "static")
                blend = CParticle::CGL_ALPHA_TRANSPARENT;
			computeModel = name;
        }
		else if (data == "Sprite")
            t = importTexture(io);
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

    if (particle == NULL)
        particle = glCreateParticle(size,revive,type,blend,computeModel,particleName);
	particle->usePointSprite(sprite,pSize);
	particle->setTexture(t);

	return true;
}

bool CParticleManager::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Particle")
                importParticle(io);
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}


