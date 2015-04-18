// Object3DContainerNotifier.h: interface for the CObject3DContainerNotifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
#define AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

RAPTOR_NAMESPACE_BEGIN


//!
//! This class is only an abstraction of the Object3DContainerNotifier
//! to allow public classes to expose opaque pointers.
//!
template <class USERDATA_t=CObject3D*>
class CContainerNotifier
{
public:
    typedef enum
    {
        DESTRUCTION,    // This is redundant with a CPersistence
        UPDATEBBOX,
        UPDATEPROPERTIES,
        USEREVENT
    } ContainerEvent;

public:
    CContainerNotifier():m_Evt(USEREVENT),m_bEnabled(true) {};
    virtual  ~CContainerNotifier() {};

    //! Return the event passed at notification
    ContainerEvent getEvent(void) const { return m_Evt; };

    //! Enables or disables the notifier.
    //! @return false if the current state is already the requested one, true otherwise
    bool enable(bool bEnable)
    {
        if (bEnable == m_bEnabled)
            return false;
        m_bEnabled = bEnable;
        return true;
    };

    //! Provokes the notification.
    virtual void notify(ContainerEvent evt,USERDATA_t) { m_Evt = evt; };

private:
    //! Store the event in case of need
    ContainerEvent  m_Evt;
    //! Retain the enable status
    bool			m_bEnabled;
};



//!
//! This class defines a notification protocol from an object to its container.
//! An instance of this class is holded by a CObject3D, which enables the notification
//!
template <class CONTAINER_t, class USERDATA_t>
class CObject3DContainerNotifier : public CContainerNotifier<USERDATA_t>
{
public:
    //! Definition of a callback to call on the container
    typedef void (CONTAINER_t::*Notifier)(USERDATA_t data);
	typedef typename CContainerNotifier<USERDATA_t>::ContainerEvent Object3DEvent;
    

public:
    //! Constructor must initialize notifier
    CObject3DContainerNotifier(CONTAINER_t &pContainer,Notifier notifier/*,USERDATA_t pData*/);

    //! Destructor does nothing.
    virtual ~CObject3DContainerNotifier();

    //! Implements base class
    virtual void notify(Object3DEvent ,USERDATA_t);


private:
    //! Forbidden operators
    CObject3DContainerNotifier();
    CObject3DContainerNotifier(const CObject3DContainerNotifier& );
    CObject3DContainerNotifier& operator=(const CObject3DContainerNotifier& );

    CONTAINER_t		&m_pContainer;
    Notifier		m_fnNotifier;
//    USERDATA_t      m_pData;
};


template <class CONTAINER_t, class USERDATA_t>
CObject3DContainerNotifier<CONTAINER_t, USERDATA_t>::~CObject3DContainerNotifier()
{
}


template <class CONTAINER_t, class USERDATA_t>
CObject3DContainerNotifier<CONTAINER_t, USERDATA_t>::CObject3DContainerNotifier(CONTAINER_t &pContainer,Notifier notifier/*,USERDATA_t pData*/) 
        :m_pContainer(pContainer),m_fnNotifier(notifier)//,m_pData(pData)
{
}

template <class CONTAINER_t, class USERDATA_t>
void CObject3DContainerNotifier<CONTAINER_t, USERDATA_t>::notify(Object3DEvent evt,USERDATA_t pData)
{
    //! Is the notifier already locked ?
	if (!CContainerNotifier<USERDATA_t>::enable(false))
        return;

    CContainerNotifier<USERDATA_t>::notify(evt,pData);

    if (m_fnNotifier != NULL)
        (m_pContainer.*m_fnNotifier)(/*m_*/pData);

    CContainerNotifier<USERDATA_t>::enable(true);
}

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)


