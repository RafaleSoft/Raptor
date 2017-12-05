// ObjectProperties.h: interface for the CObjectProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTPROPERTIES_H__1D80C74F_6792_4B6E_BF78_613553722315__INCLUDED_)
#define AFX_OBJECTPROPERTIES_H__1D80C74F_6792_4B6E_BF78_613553722315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

// ! Serialization
class CRaptorIO;


class RAPTOR_API CObjectProperties  
{
public:
	typedef enum
	{
		CLIP_AA_BBOX,		// Standard method : Axis Aligned Bounding Box
		CLIP_BSPHERE,		// Clipping using a bounding sphere
		CLIP_BBOX_OCCLUSION	// Use Bounding box occlusion queries
	} CLIPPING_METHOD;

public:
    //! Constructor
	CObjectProperties();

    //! Copy constructor available
    CObjectProperties(const CObjectProperties& );

    //! Destructor
	virtual ~CObjectProperties();

    //! Assignment operator available
    CObjectProperties& operator=(const CObjectProperties& );

    //! Rendering:
	//!	Returns the visibility state of the object
	//!	( by default, object is visible )
	bool RAPTOR_FASTCALL isVisible(void) const { return m_bVisible; }

	//! Rendering:
	//! Set the visibility flag of the object.
	//!	It is the 'User' visibility, not the visibility computed
	//!	by the engine.
	void RAPTOR_FASTCALL setVisible(bool visible) { m_bVisible = visible; };

	//! Rendering:
	//!	Returns the transparency state of the object
	//!	( by default, object is not transparent )
	bool RAPTOR_FASTCALL isTransparent(void) const { return m_bTransparent; }

	//! Rendering:
	//! Set the transparency flag of the object.
	//!	It is the 'User Controlled' transparency, 
	//! not the transparency computed by the engine.
	void RAPTOR_FASTCALL setTransparent(bool transparent) { m_bTransparent = transparent; };

	//! Rendering:
	//!	Returns the shadow receive state of the object
	//!	( by default, object is receiving all shadows )
	bool RAPTOR_FASTCALL isReceiveShadow(void) const { return m_bReceiveShadow; }

	//! Rendering:
	//! Set the receive shadow flag of the object.
	void RAPTOR_FASTCALL setReceiveShadow(bool receive) { m_bReceiveShadow = receive; };

	//! Rendering:
	//!	Returns the shadow cast state of the object
	//!	( by default, object casts its shadow on all objects )
	bool RAPTOR_FASTCALL isCastShadow(void) const { return m_bCastShadow; }

	//! Rendering:
	//! Set the cast shadow flag of the object.
	void RAPTOR_FASTCALL setCastShadow(bool cast) { m_bCastShadow = cast; };


    //! Rendering:
	//!	Returns the mirror state of the object
	//!	( by default, object is not a mirror, it does not reflects its environment )
	bool RAPTOR_FASTCALL isMirror(void) const { return m_bIsMirror; }

	//! Rendering:
	//! Set the mirror flag of the object.
	void RAPTOR_FASTCALL setMirror(bool mirror) { m_bIsMirror = mirror; };

    //! Rendering:
	//!	Returns the mirrored state of the object
	//!	( by default, object is mirrored, it is reflected in any existing visible mirror )
	bool RAPTOR_FASTCALL isMirrored(void) const { return m_bIsMirrored; }

	//! Rendering:
	//! Set the mirror flag of the object.
	void RAPTOR_FASTCALL setMirrored(bool mirrored) { m_bIsMirrored = mirrored; };

	//! Rendering:
	//!	Selects the clipping strategy
	void RAPTOR_FASTCALL setClippingMethod(CLIPPING_METHOD method) { m_eClippingMethod = method; };

	//! Rendering:
	//!	Returns the clipping strategy
	CLIPPING_METHOD RAPTOR_FASTCALL getClippingMethod(void) const { return m_eClippingMethod; };

    //! Object serialization.
    //! @see CPersistence
	virtual bool exportObject(CRaptorIO& o);

    //! Object serialization.
    //! @see CPersistence
	virtual bool importObject(CRaptorIO& i);


private:
   	//!	see remarks of method SetVisible
	bool			m_bVisible;
	//!	see remarks of method SetTransparent
	bool			m_bTransparent;
	//! see remarks of method SetReceiveShadow
	bool			m_bReceiveShadow;
	//! see remarks of method SetCastShadow
	bool			m_bCastShadow;
    //! see remarks of method SetMirror
    bool            m_bIsMirror;
    //! see remarks of method SetMirrored
    bool            m_bIsMirrored;
	//! see remarks of method setClippingMethod
	CLIPPING_METHOD	m_eClippingMethod;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECTPROPERTIES_H__1D80C74F_6792_4B6E_BF78_613553722315__INCLUDED_)

