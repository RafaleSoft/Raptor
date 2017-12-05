// ParticleManager.h: interface for the CParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLEMANAGER_H__687B445A_C015_43DA_95A4_EC8AB1AED606__INCLUDED_)
#define AFX_PARTICLEMANAGER_H__687B445A_C015_43DA_95A4_EC8AB1AED606__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GEOMETRICMODIFIER_H__5C0729A2_6F0C_42CA_9033_CAFB26247761__INCLUDED_)
	#include "Engine/Modifier.h"
#endif

#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
	#include "GLHierarchy/Particle.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;


//! This class handles particle objects and ther behaviour.
//! Rendering is delegated to particle objects
class RAPTOR_API CParticleManager : public CModifier  
{
public:
	//!
	//!	Particle behaviour is implemented through a compute object.
	//!	This class is used for birth/life/death events for each particle.
	//! This class must be derived and provided to this manager to create a particle system.
	//!
	class RAPTOR_API CParticleCompute
	{
	public:
		virtual ~CParticleCompute() {};

		//! Returns the name of the compute model to find it
		virtual std::string getModelName(void) const = 0;

		//!	Entry point of particle compute object
		virtual void compute(CParticle& particle,float dt) = 0;

		//!	Initialize the particle compute object
		virtual void initialize(CParticle& particle) = 0;

		//! If this flag is set, the compute model revives dead particles.
		void setRevive(bool revive)
		{ m_bRevive = revive; }

		//!	Defines scale and fading factors of the computing model
		//! (applied identically to each particle)
		void setFactors(const GL_COORD_VERTEX &fadeFactors,float scaleFactor)
		{
			m_fadeFactors = fadeFactors;
			m_scaleFactor = scaleFactor;
		}

	protected:
		CParticleCompute()
			:m_scaleFactor(0.0),m_fadeFactors(1.0,1.0,1.0,1.0),m_bRevive(true)
		{};

		GL_COORD_VERTEX m_fadeFactors;
		float			m_scaleFactor;
		bool			m_bRevive;
	};

	//! Constructor.
	CParticleManager(const std::string& name = "GL_PARTICLE_MANAGER");

	//! Destructor.
	virtual ~CParticleManager();

	//!	Base implementation
    virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_GEOMETRIC_MODIFIER; };


    //! Create a managed particle object
    CParticle *glCreateParticle(unsigned int number, bool revive, 
								CParticle::PARTICLE_RENDERING type,
								CParticle::PARTICLE_BLENDING blend, 
								CParticleCompute *compute,
								const std::string& name = "GL_PARTICLE");

	//! Create a managed particle object
    CParticle *glCreateParticle(unsigned int number, bool revive,
								CParticle::PARTICLE_RENDERING type,
								CParticle::PARTICLE_BLENDING blend,
								const std::string& computeModel,
								const std::string& name = "GL_PARTICLE");

    //! Initialize or re-initialize a particle object
    void glInitParticle(CParticle *p);

	//!	Set a particle computing model
	static void setComputeModel(CParticleCompute* compute,
								const std::string& model);


    //! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CParticleManagerClassID,"ParticleManager",CPersistence)



protected:
	//!	Compute physics resultant after dt time increment
	//!	This method should not be called directly.
	virtual void RAPTOR_FASTCALL deltaTime(float dt);


private:
    //!	A helper to import managed particle systems.
    bool importParticle(CRaptorIO& io);
	//!	A helper to import managed particle systems.
	CTextureObject* importTexture(CRaptorIO &io);

	//!	Notifies observers that this persistence is about to be destroyed.
	virtual void unLink(const CPersistence*);

	//!	The number of tasks to usr more efficiently available processors.
	static size_t	avgParallelTasks;

    //!    The structure of a particle system.
    typedef struct 
    {
        //! Particle computer
		CParticleCompute *compute;
        //!  Point datas
        CParticle*  system;
    } PARTICLE_SYSTEM;
    vector<PARTICLE_SYSTEM>  m_pParticles;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PARTICLEMANAGER_H__687B445A_C015_43DA_95A4_EC8AB1AED606__INCLUDED_)

