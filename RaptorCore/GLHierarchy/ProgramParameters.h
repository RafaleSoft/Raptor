// ProgramParameters.h: interface for the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
#define AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif

RAPTOR_NAMESPACE_BEGIN



class RAPTOR_API CProgramParameters
{
public:
	//!  Fixed pipeline attributes binding shortcut that can help switching
	//! easily between the asm shaders and conventional pipelines which are exclusive.
	//! Nevertheless, GLSL bindings can coexist with conventionnal mapping.
	typedef enum GL_VERTEX_ATTRIB_t
	{
		POSITION,
		WEIGHTS,
		NORMAL,
		PRIMARY_COLOR,
		SECONDARY_COLOR,
		FOG_COORDINATE,
		ADDITIONAL_PARAM1,
		ADDITIONAL_PARAM2,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7
	} GL_VERTEX_ATTRIB;

	class CParameterBase
	{
	public:
		virtual ~CParameterBase() {};

		virtual uint64_t size(void) const { return 0; };
		virtual const void* addr(void) const { return NULL; };
		virtual CParameterBase* clone(void) const { return NULL; };
		virtual bool copy(const CParameterBase&) { return false; };


		const std::string& name() const { return m_name; };
		void name(const std::string& n) { m_name = n; };

		virtual CParameterBase& operator=(const CParameterBase& p)
		{
			m_name = p.m_name;
			locationIndex = p.locationIndex;
			locationType = p.locationType;
			return *this;
		}

#if _MSC_VER == 1500
		static size_t hash_value(const std::string& s)
		{	
			const char *ptr = s.c_str();
			const char *end = ptr + s.size();
			size_t hash = 2166136261U;
			while(end != end)
				hash = 16777619U * hash ^ (size_t)*ptr++;
			return (hash);
		}
#endif
		virtual size_t getTypeId(void) const
		{
#if _MSC_VER > 1500
			static size_t ti = typeid(void*).hash_code();
#else
			const std::string nm = std::string(typeid(void*).name());
			static size_t ti = hash_value(nm);
#endif		
			return ti;
		}

		template <class T> bool isA(const T &t) const
		{
			return getTypeId() == CParameter<T>::TypeId();
		}

		//! Parameter locations can be retrieved once per link and reused.
		int				locationIndex;
		unsigned int	locationType;

	protected:
		CParameterBase() :m_name(""), locationIndex(-1), locationType(0) {};
		CParameterBase(const CParameterBase& p)
			:m_name(p.m_name),
			locationIndex(p.locationIndex),
			locationType(p.locationType) {};

		//! The name of the value: it is matched to variables of the program ( @see GLSL )
		string				m_name;
	};

	template <class P>
	class CParameter : public CParameterBase
	{
	public:
		CParameter(const P &param) :p(param) {};
		virtual ~CParameter() {};

		static size_t TypeId(void)
		{
#if _MSC_VER > 1500
			static size_t ti = typeid(P).hash_code();
#else
			const std::string nm = std::string(typeid(P).name());
			static size_t ti = hash_value(nm);
#endif
			return ti;
		};
		virtual size_t getTypeId(void) const { return CParameter<P>::TypeId(); };

		virtual uint64_t size(void) const { return sizeof(p); };
		virtual const void* addr(void) const { return &p; };
		virtual CParameterBase* clone(void) const
		{
			CParameter<P> *param = new CParameter<P>(p);
			*param = *this;
			return param;
		};
		virtual bool copy(const CParameterBase& param)
		{
			// Should a safe 'isA' be used ? only if fast enough.
			if (param.size() == sizeof(p))
			{
				p = ((const CParameter<P>&)param).p;
				return true;
			}
			else
				return false;
		};

		CParameter<P>& operator=(const P &_p)
		{
			p = _p;
			return *this;
		};

		CParameter<P>& operator=(const CParameter<P> &_p)
		{
			CParameterBase::operator=(_p);
			p = _p.p;
			return *this;
		};

		virtual CParameterBase& operator=(const CParameterBase& param)
		{
			//	Safe assignment
			if (param.isA(p))
				this->operator=((const CParameter<P>&)param);
			return *this;
		}

	public:
		P	p;
	};


public:
	//! Default constructor. The parameter set is uninitialised, user
	//! must call the proper init method from the relevant shader.
	CProgramParameters():m_parameters() {};
	virtual ~CProgramParameters() {	clear(); };

	//!	Remove all parameters
	void clear(void);

	//! Number of paremeters
	size_t getNbParameters(void) const { return m_parameters.size(); };

	//! Add a generic parameter
	template <class param>
	bool addParameter(const std::string& name, const param& param);

	//! Add a base parameter
	bool addParameter(const CParameterBase& param);

	//! Access to vector parameters.
	CParameterBase& operator[](unsigned int v);
	const CParameterBase& operator[](unsigned int v) const;

	//! Parameters assignment: only typed value is assigned, type and index are left unchanged
	//!	Source and destination parameter set must have the same dimension, otherwise
	//!	this set is reinitialised
	CProgramParameters& operator=(const CProgramParameters& params);

	//! Parameters assignment: updates typed values, based on identical parameter names
	bool updateParameters(const CProgramParameters& params);


private:
	CProgramParameters(const CProgramParameters&) {};

	//! Values of this parameter set.
	std::vector<CProgramParameters::CParameterBase*>	m_parameters;
};

template <class P>
bool CProgramParameters::addParameter(const std::string& name, const P& param)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < m_parameters.size(); i++)
	{
		if (m_parameters[i]->name() == name)
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Duplicate parameter name");
			return false;
		}
	}
#endif

	CProgramParameters::CParameter<P> *p = new CProgramParameters::CParameter<P>(param);
	p->name(name);
	m_parameters.push_back(p);

	return false;
}


RAPTOR_NAMESPACE_END

#endif	//	!defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)