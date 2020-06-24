// ParticleCompute.h: interface for the CBasicObjects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLECOMPUTE_H__395259D4_3C60_4233_8D81_1F7B58BED8A5__INCLUDED_)
#define AFX_PARTICLECOMPUTE_H__395259D4_3C60_4233_8D81_1F7B58BED8A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PARTICLEMANAGER_H__687B445A_C015_43DA_95A4_EC8AB1AED606__INCLUDED_)
    #include "Engine/ParticleManager.h"
#endif

RAPTOR_NAMESPACE


class RAPTOR_API CParticleCompute
{
public:
	static bool installComputers();
	static bool installComputer(const std::string& computeModel);

	class RAPTOR_API CRamdomParticle : public CParticleManager::CParticleCompute
	{
	public:
		//! Constructor
		CRamdomParticle();
		virtual ~CRamdomParticle();

		//! Implements a random particle system compute
		virtual std::string getModelName(void) const
		{ return "random"; };
		virtual void compute(CParticle& particle,float dt);
		virtual void initialize(CParticle& particle);

	private:
		void computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
		void computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs);
	};

	class RAPTOR_API CFireParticle : public CParticleManager::CParticleCompute
	{
	public:
		//! Constructor
		CFireParticle();
		virtual ~CFireParticle();

		//! Implements a random particle system compute
		virtual std::string getModelName(void) const
		{ return "fire"; };
		virtual void compute(CParticle& particle,float dt);
		virtual void initialize(CParticle& particle);

	private:
		void computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
		void computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs);
	};

	class RAPTOR_API CSmokeParticle : public CParticleManager::CParticleCompute
	{
	public:
		//! Constructor
		CSmokeParticle();
		virtual ~CSmokeParticle();

		//! Implements a random particle system compute
		virtual std::string getModelName(void) const
		{ return "smoke"; };
		virtual void compute(CParticle& particle,float dt);
		virtual void initialize(CParticle& particle);

	private:
		void computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
		void computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs);
	};

	class RAPTOR_API CStaticParticle : public CParticleManager::CParticleCompute
	{
	public:
		//! Constructor
		CStaticParticle();
		virtual ~CStaticParticle();

		//! Implements a random particle system compute
		virtual std::string getModelName(void) const
		{ return "static"; };
		virtual void compute(CParticle& particle,float dt);
		virtual void initialize(CParticle& particle);

	private:
		void computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
		void computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs);
	};

private:
	CParticleCompute(void) {};
	virtual ~CParticleCompute(void){};
};

#endif // !defined(AFX_PARTICLECOMPUTE_H__395259D4_3C60_4233_8D81_1F7B58BED8A5__INCLUDED_)