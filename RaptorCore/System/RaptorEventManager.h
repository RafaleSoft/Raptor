// RaptorEventManager.h: interface for the CRaptorEventManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTOREVENTMANAGER_H__DBC94306_4406_4A12_9C54_EFFDF30C4A4C__INCLUDED_)
#define AFX_RAPTOREVENTMANAGER_H__DBC94306_4406_4A12_9C54_EFFDF30C4A4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN


class CRaptorEventManager  
{
public:
    class CRaptorEvent
    {
     public:
         CRaptorEvent() {};
         virtual ~CRaptorEvent() {};

         virtual void raiseEvent();
    };


public:
	CRaptorEventManager();
	virtual ~CRaptorEventManager();

};


RAPTOR_NAMESPACE_END


#endif // !defined(AFX_RAPTOREVENTMANAGER_H__DBC94306_4406_4A12_9C54_EFFDF30C4A4C__INCLUDED_)
