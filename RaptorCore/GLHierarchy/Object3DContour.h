// Object3DContour.h: interface for the CObject3DContour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
#define AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class C3DSet;
class CGeometry;
class CObject3D;
class CContourAttributes;
template <class T> class CContainerNotifier;

class RAPTOR_API CObject3DContour
{
public:
    typedef struct CONTOUR_VOLUME_t
    {
        const GL_COORD_VERTEX * volume;
        unsigned int volumeSize;
	    unsigned int darkCapSize;
	    unsigned int lightCapSize;
	    unsigned int *volumeIndexes;
	    unsigned int *darkCapIndexes;
	    unsigned int *lightCapIndexes;
        bool    darkCapClipped;
        bool    lightCapClipped;
        bool    volumeClipped;
    } CONTOUR_VOLUME;


	//!	The bounding box is initialy the object's bbox.
	//!	It is expanded to fit the shadow volume on query
	//!	( if the contour has been generated )
	void boundingBox(GL_COORD_VERTEX &Min,GL_COORD_VERTEX &Max);

    //! Returns the current BoudingBox
	const CBoundingBox * const getBoundingBox() const;


public:
    //! Public constructor
    //!	An object contour should not be created directly, 
	//! indeed, it should be issued from a CObject3D derived class
	CObject3DContour(const std::string& name = "OBJECT3D-CONTOUR");

	virtual ~CObject3DContour();

	//!	Initialises the contour of a single geometry
	void init(CGeometry *geo);

    //!	Initialises the contour of a set of geometry
    //! Basic checks are performed, but a valid set must be supplied for correct rendering.
	void init(C3DSet *set);

	//!	PrepareContour allocates all necesary emory and data
	//!	structure for real time contour generation. Use only with shadow volumes.
	void prepareContour(void);

    //! Returns the geometry producing the contour.
    //! The geometry returned is different from the one passed at 'Init'
    //! Indeed, it is a result of computations to optimize contour generation
    //! ( at least a minimized form of the init geometry )
    const CGeometry* getGeometry(void) const { return m_pOrigin; }

	//!	Construct a volume from the contour extrusion.
	//!	The volume is built by extruding the vertices from the original geometry.
	//!	@param pos: the point of view (or light position), origin of extrusion.
	//! @param extrusion: the distance to extrude, in total distance from pos.
	void buildVolume(const GL_COORD_VERTEX &pos,float extrusion = 100.0f);

	//!	This method selects the volume parts to be clipped during rendering.
    void clipVolume(bool volumeClipped, bool lightCapClipped, bool darkCapClipped);

    //!    This method returns the coutour volume computed by buildVolume.
    //!    The contour volume provides all necessary data to render the voume.
    const CONTOUR_VOLUME& getContourVolume(void) const { return m_contourVolume; }

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CObject3DContourClassID,"Object3DContour",CPersistence)



private:
    //! Retrive the internal list of faces that are back faces given the light position 'pos'
    virtual void findBackFaces(	const GL_COORD_VERTEX &pos);

    //! Compute the internal contour, based on the current internal list of backfaces
	//!	Currently unimplemented.
	//virtual void findContour(void);

    //! Find the list of edges of the initial geometry
	void findEdges(void);

    //! Copying a contour is forbidden in this version
	CObject3DContour &operator=(const CObject3DContour &r_contour);

    //! A handler to manage origin destruction.
    void notifyFromOrigin(CObject3D* child);

    //! This geometry is the base object for the volume generation.
    //! It results from the sum of set childs/direct geometry copy followed by optimisations/simplifications.
    CGeometry	            *m_pOrigin;

    //! This class holds all internal coutour attributes for calculations & rendering
    CContourAttributes	 *m_pContour;

    //! A notifier observer to update the set when items are updated.
    CContainerNotifier<CObject3D*> *m_pObserver;

    CONTOUR_VOLUME  m_contourVolume;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)

