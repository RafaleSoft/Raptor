// 3DSet.h: interface for the C3DSet class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
#define AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class C3DSetItem;


class RAPTOR_API C3DSet : public CObject3D
{
public:
    class RAPTOR_API C3DSetIterator
    {
    public:
		//! Constructor from an arbitrary position
		C3DSetIterator(C3DSetItem  *pos);

        //! Copy constructor necessary to initialise an iterator returned from C3DSet::getIterator
        C3DSetIterator(const C3DSetIterator&);

		//! Acces to iterator's item
		C3DSetItem  * const operator()() const { return m_pItem; };

        //! Destructor does nothing
        virtual ~C3DSetIterator() {};

        //! Move iterator to the next position
        C3DSetIterator& operator++(int);

        //! Move iterator to the previous position
        C3DSetIterator& operator--(int);

        //! Move iterator n-positions forward
        C3DSetIterator& operator+(unsigned int offset);

        //! Move iterator n-positions backward
        C3DSetIterator& operator-(unsigned int offset);

    private:
        C3DSetIterator();
        C3DSetItem  *m_pItem;
    };


public:
	C3DSet(const std::string& name = "3D_SET");

    //! Destructor will delete the whole hierarchy, but not the CObject3D objects.
    //! As they may be used in other structures, it is user's responsibility to destroy them.
    //! ( except if you use purge at exit to automagically delete all objects )
	virtual ~C3DSet();

 
    //!
    //! Set operations
    //!

    //! This method returns an iterator to navigate in the set
    //! and to modifiy the set. The newly created iterator points to the root of the set
    C3DSetIterator  getIterator(void) const;

    //! Returns the object at iterator's position
	 CObject3D *const getChild (const C3DSetIterator& position) const;

    //! Insert an object as a child of iterator's position.
	bool addChild (CObject3D *pObject, const C3DSetIterator& position);

    //! Removes an object from the set. All it's childs are also removed, so user 
    //! must be aware of the way the set is build and manipulated.
    //! @return : true if the object is member of the set and then removed. false otherwise.
    bool removeChild(CObject3D *pObject);


    //!
    //! Rendering : specific rendering methods
    //!

	//! Renders the bbox of each objet in the set.
	virtual void glRenderBBox(bool filled = false);

    //! Clipping each object before it is rendered.
	virtual void glClipRender();


    //!
    //! Base classes:
    //!
    //!    Implements earlyClipping in depth
    virtual bool RAPTOR_FASTCALL fullClip(const CGenericMatrix<float> &transform);

    //! Manipulators : implements base class CObject3D
	DECLARE_OBJECT3D_MANIPULATORS
    
	//!
    //! Creates the list of contours for this objet.
    //!    For testing purposes, the set always returns a single coutour in the result vector;
    //!    Correct implementation will return a variable list of contours. ( depend on optimisation )
    virtual vector<CObject3DContour*> createContours(void);

    //! Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(C3DSetClassID,"Set",CObject3D)


private:
    //! Forbidden operators
    C3DSet(const C3DSet&);
    C3DSet& operator=(const C3DSet&);

    //! Implements CPersitence
	virtual void unLink(const CPersistence*);

    //! Recomputes the bounding box
    void rescanBBox(void);

    void notifyFromChild(CObject3D* child);

    //! The root of the set hierarchy
    C3DSetItem  *m_pRoot;

    //! A notifier observer to update the set when items are updated.
    CContainerNotifier<CObject3D*> *m_pObserver;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)

