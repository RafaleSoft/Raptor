// TimeObject.h: interface for the CTimeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
#define AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE_BEGIN

class ITImeObjectImpl;

//!	CTimeObject class:
//!	Implements a behaviour of a time dependant object.
//!	Synchronization is managed by the current animator.
class RAPTOR_API CTimeObject  
{
public:
	bool isAnimate(void) const
	{ return m_animate; };

	void animate(bool a)
	{ m_animate = a; };

	//!
	//! Synchronization status.
	//!

	//!	Updates synchronization state of the object.
	//!	Synchronization is compute according to time increment dt.
	//! (not used if synchro is false).
	void synchronize(float dt, bool synchro = true);
	//!	Return true is object has been synchronized by animator.
	bool isSynchronized(void) const { return m_synchronized; };
	//!	Return false if another object had priority.
	bool prioritize(void);

	
	//!
	//! TIme helpers methods.
	//!

	//!	Returns the 'global time' (in seconds) of the current running process.
	static float RAPTOR_FASTCALL GetGlobalTime(void);

	//!	Use this method to scale 'real time' time increments
	//!	received by every CtimeObject derived class.
	//!	This scaled Global Time is returned by GetTime below.
	static void setTimeFactor(float factor);

	//!	Returns time increment since previous call in seconds,
	//!	Global time values are incremented.
	static float deltaTime(void);

	//!	Returns the scaled 'global time' at which the time object
	//!	has last been computed.
	static float RAPTOR_FASTCALL GetTime(void);

	//!	Take a time reference identified by the mark
	//! (the mark must be unique).
	static void markTime(void* mark);

	//!	Returns the time in seconds elapsed since mark was referenced.
	static float deltaMarkTime(void *mark);

	//!	Returns the array of all time objects.
	static const vector<CTimeObject*>& getTimeObjects(void);

	//!	Compute physics resultant after dt time increment
	virtual void RAPTOR_FASTCALL deltaTime(float dt) = 0;


protected:
	CTimeObject();
	virtual ~CTimeObject();

private:
	bool			m_animate;
	bool			m_synchronized;

	static ITImeObjectImpl	*m_impl;

	static float	m_time;
	static float	m_globalTime;
	static float	m_deltat;
	static			vector<CTimeObject*>	m_rootTimeObjects;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)

