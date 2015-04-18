#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_

#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
	#include "RaptorComputeMemory.h"
#endif

// Cette classe est exportée de RaptorCompute.dll
class RAPTORCOMPUTE_API CRaptorComputeTask
{
public:
	class ParameterBase
	{
	public:
		virtual ~ParameterBase() {};
		virtual size_t size(void) const { return 0; };
		virtual const void* addr(void) const { return NULL; };
		virtual ParameterBase* clone(void) const { return NULL; };

		template <class T> bool isA(const T &t) const
		{ return getTypeId() == Parameter<T>::TypeId(); };

	protected:
		ParameterBase() {};
		ParameterBase(const ParameterBase&) {};
		virtual const type_info& getTypeId(void) const { return typeid(void*); };
	};

	template <class P>
	class Parameter : public ParameterBase
	{
	public:
		Parameter(const P &param):p(param) {};
		virtual ~Parameter() {};

		static const type_info& TypeId(void) { static P _p; return typeid(_p); };
		virtual const type_info& getTypeId(void) const { return typeid(p); };

		virtual size_t size(void) const { return sizeof(p); };
		virtual const void* addr(void) const { return &p; };
		virtual ParameterBase* clone(void) const
		{ return new Parameter<P>(p); };

		Parameter<P>& operator=(const P &_p)
		{ p = _p; return *this; };

	public:
		P	p;
	};


public:
	//!	Constructor
	CRaptorComputeTask(	const std::string& name,
						size_t globalSize,
						size_t localSize);

	//!	Copy constructor
	CRaptorComputeTask(const CRaptorComputeTask& task);

	//!	Destructor
	virtual ~CRaptorComputeTask(void);

	//!	The name of the job: must match the name of a kernel
	//!	@return the actual name
	const std::string &getName(void) const 
	{ return m_name; };

	//!	@return the global work size of the task
	const size_t* getGlobalSize(void) const
	{ return m_globalSize; };

	//!	Defines the global size of the task
	//!	@param gs : the global work size of the task
	void setGlobalSize(const size_t gs);

	//!	@return the local work size of the task
	const size_t* getLocalSize(void) const
	{
		if (m_localSize[0] == 0) return NULL;
		else return m_localSize; 
	};

	//! Defines the local size of the task
	//!	@param ls : the local work size of the task
	void setLocalSize(const size_t ls);

	//! @return the number of parameters of this task
	size_t getNbParameters(void) const { return m_parameters.size(); };

	//!	Add a parameter for the next kernel invocation
	//!	@param c : the parameter value to add
	template <class C>
	void addParameter(C c)
	{ m_parameters.push_back(new Parameter<C>(c)); };
	
	//!	Specific handling to access to the buffer object pointer.
	//! @param bo : a reference to a compute buffer object
	void addParameter(const CRaptorComputeMemory::CBufferObject &bo);

	//! @return the list of parameters of this task.
	const std::vector<ParameterBase*>& getParameters(void) const
	{ return m_parameters; };

	//!	operator [] overload
	//! @param pos : the index of the parameter to retrieve
	//! @return the requested parameter, of nullParameter if invalid index.
	template <class C>
	Parameter<C>& operator[](size_t pos) const
	{ if (pos < m_parameters.size()) return *m_parameters[pos]; else return nullParameter; };

	template <class C>
	void setParameter(size_t pos,const C &c)
	{	
		if (pos < m_parameters.size())
		{
			if (m_parameters[pos]->isA(c))
				*((Parameter<C>*)m_parameters[pos]) = c; 
		}
	};

	//!	Specific handling to access to the buffer object pointer.
	//! @param bo : a reference to a compute buffer object
	void setParameter(size_t pos,const CRaptorComputeMemory::CBufferObject &bo);


private:
	static ParameterBase	nullParameter;

	const std::string m_name;
	size_t	m_globalSize[4];
	size_t	m_localSize[4];
	std::vector<ParameterBase*>	m_parameters;
};


#endif	// defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)


