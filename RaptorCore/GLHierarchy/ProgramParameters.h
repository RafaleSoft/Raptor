/***************************************************************************/
/*                                                                         */
/*  ProgramParameters.h                                                    */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
#define AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
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
		POSITION = 0,
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
		TEXCOORD7,
		MAX_VERTEX_ATTRIB
	} GL_VERTEX_ATTRIB;


	//!	This class is a base for shader program parameters, it cannot be constructed
	//!	directly, only subclasses are usable.
	class RAPTOR_API CParameterBase
	{
	public:
		//!	Public destructor. 
		virtual ~CParameterBase() {};

		//!	Parameter name property.
		//! @return the parameter name.
		const std::string& name() const { return m_name; };

		//!	Parameter name property.
		//! @param n: the name to set for the parameter.
		void name(const std::string& n) { m_name = n; };

		//!	Assignment operator.
		virtual CParameterBase& operator=(const CParameterBase& p);
		
		//!	Parameter type identifier for comparison
		virtual size_t getTypeId(void) const;
		
		//!	Definition of parameter services:
		//!	- the size of the parameter
		//!	- the address of the parameter (caution: for subclasses, 
		//!	  the addr method must be implemented otherwise the framework
		//!	  will not work with class based parameters.
		//!	- a clone for parameters (deep copy)
		//!	- a copy of the parameter value only.
		virtual uint64_t size(void) const { return 0; };
		virtual const void* addr(void) const { return NULL; };
		virtual CParameterBase* clone(void) const { return NULL; };
		virtual bool copy(const CParameterBase&) { return false; };

		//!	This method defines a comparator for parameter subclasses
		//!	@param t: the parameter type to compare to.
		//! @return true if the parameter is of the tested type.
		template <class T> bool isA(const T &) const
		{
			return getTypeId() == CParameter<T>::TypeId();
		}

		//! Parameter locations can be retrieved once per shader link and reused.
		int			locationIndex;	// -1 if no location has been found
		uint32_t	locationType;
		int			locationSize;

	protected:
		CParameterBase();
		CParameterBase(const CParameterBase& p);

#if (_MSC_VER <= 1500) || defined(LINUX)
		size_t hash_value(const std::string& s);
#endif
		//! The name of the value: it is matched to variables of the program ( @see GLSL )
		std::string	m_name;
	};

	//!	This class is the main class for shader program parameters.
	template <class P>
	class CParameter : public CParameterBase
	{
	public:
		CParameter(const P &param) :p(param) {};
		CParameter(const std::string& n, const P &param) : p(param) { m_name = n; };
		virtual ~CParameter() {};

		static size_t TypeId(void)
		{
#if (_MSC_VER <= 1500) || defined(LINUX)
			const std::string nm = std::string(typeid(P).name());
			static size_t ti = hash_value(nm);
#else
			static size_t ti = typeid(P).hash_code();
#endif
			return ti;
		};
		virtual size_t getTypeId(void) const { return CParameter<P>::TypeId(); };

		virtual uint64_t size(void) const { return sizeof(P); };
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
			//if (param.size() == sizeof(p))
			if (param.isA(p))
			{
				p = ((const CParameter<P>&)param).p;
				return true;
			}
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
			{
				CParameterBase::operator=(param);
				p = ((const CParameter<P>&)param).p;
			}
			return *this;
		}

	public:
		P	p;
	};

	//!	This class is a helper class for shader program array parameters.
	//!	This is because arrays cannot be assigned directly, then they 
	//! need to have a dedicated management.
	template <class P,size_t N>
	class CParameterArray : public CParameterBase
	{
	public:
		typedef P inner_type[N];

	public:
		CParameterArray(const inner_type& param)
		{
			memcpy(&p[0], &param[0], N * sizeof(P));
		};

		CParameterArray(const std::string& n, const inner_type& param)
		{
			if (n[n.length() - 1] != ']')
				m_name = n + "[0]";	// glGetActiveUniform will return a name with array subscript.
			else
				m_name = n;
			memcpy(&p[0], &param[0], N * sizeof(P));
		};

		virtual ~CParameterArray() {};

		static size_t TypeId(void)
		{
#if (_MSC_VER <= 1500) || defined(LINUX)
			const std::string nm = std::string(typeid(inner_type).name());
			static size_t ti = hash_value(nm);
#else
			static size_t ti = typeid(inner_type).hash_code();
#endif
			return ti;
		};
		virtual size_t getTypeId(void) const { return CParameterArray<P,N>::TypeId(); };

		virtual uint64_t size(void) const { return N*sizeof(P); };
		virtual const void* addr(void) const { return &p; };
		virtual CParameterBase* clone(void) const
		{
			CParameterArray<P,N> *param = new CParameterArray<P,N>(p);
			*param = *this;
			return param;
		};
		virtual bool copy(const CParameterBase& param)
		{
			// Should a safe 'isA' be used ? only if fast enough.
			//if (param.size() == sizeof(p))
			if (param.getTypeId() == CParameterArray<P, N>::TypeId())
			{
				const CParameterArray<P, N> &src = (const CParameterArray<P, N>&)param;
				memcpy(&p[0], &(src.p[0]), N * sizeof(P));
				return true;
			}
			return false;
		};

		CParameterArray<P,N>& operator=(const inner_type &_p)
		{
			memcpy(&p[0], &_p[0], N * sizeof(P));
			return *this;
		};

		CParameterArray<P,N>& operator=(const CParameterArray<P,N> &_p)
		{
			CParameterBase::operator=(_p);
			memcpy(&p[0], &(_p.p[0]), N * sizeof(P));
			return *this;
		};

		virtual CParameterBase& operator=(const CParameterBase& param)
		{
			//	Safe assignment
			//if (param.isA(p))
			if (param.getTypeId() == CParameterArray<P, N>::TypeId())
			{
				CParameterBase::operator=(param);
				const CParameterArray<P, N> &src = (const CParameterArray<P, N>&)param;
				memcpy(&p[0], &(src.p[0]), N * sizeof(P));
				
			}
			return *this;
		}

	public:
		inner_type	p;
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
	bool addParameter(const std::string& name, const param& p);

	//! Add a base parameter
	bool addParameter(const CParameterBase& param);

	//! Access to vector parameters.
	CParameterBase& operator[](size_t v);
	const CParameterBase& operator[](size_t v) const;

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
			RAPTOR_WARNING(	CPersistence::CPersistenceClassID::GetClassId(),
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