// GeometryPrimitive.h: interface for the CGeometryPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYPRIMITIVE_H__890D2E18_2DAC_4094_AE20_BDF7D6FA5DBF__INCLUDED_)
#define AFX_GEOMETRYPRIMITIVE_H__890D2E18_2DAC_4094_AE20_BDF7D6FA5DBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CGeometry;


//!	This class defines the model of primitive
//!	from witch a geometry is rendered.
class RAPTOR_API CGeometryPrimitive : public CObjectReference
{
public:
	//!	The kind of primitives
	typedef enum
	{
		POINT,
		LINE,
		TRIANGLE,
		QUAD,		//! Deprecated in core profile
		LINE_STRIP,
		TRIANGLE_STRIP,
		QUAD_STRIP,	//! Deprecated in core profile
		LINE_LOOP,
		TRIANGLE_FAN,
		POLYGON,
		HIGH_ORDER_SURFACE
	} PRIMITIVE_KIND;

	//!	Primitive constructor
	CGeometryPrimitive(PRIMITIVE_KIND kind);

	//! Renders the geometry
	void glRender(void);

	//!	Returns one of the above kind.
	PRIMITIVE_KIND getKind(void) const { return m_kind; };

    //! Returns the size of the primitive, i.e. the number of indexes
	uint32_t getSize(void) const { return m_size; };

    //! Returns the faces of the primitive.
    //! The user is responsible for allocating enough storage to receive data.
    void getIndexes(unsigned short *faces);

	//!	Getter specific for polygon description.
	void getIndexes(vector<unsigned short> &polygonSizes,vector<unsigned short> &polygonIndexes);

	//!	Defines indexes of the primitive.
	//!	Rendering will use vertices of geometry pointed by indexes to render basic primitives.
	//!	Unused when specifying an HOS primitive.
	void setIndexes(unsigned short size,unsigned short* faces);
	void setIndexes(const vector<unsigned short> &faces);

	//!	Setter specific to polygon descriptions.
	void setIndexes(const vector<unsigned short> &polygonSizes,const vector<unsigned short> &polygonIndexes);


	//!	Defines the orders and precision of evaluation of
	//!	an HOS primitive
	void setOrder(unsigned int uorder, unsigned int vorder, unsigned int uprecision, unsigned int vprecision);


private:
	virtual ~CGeometryPrimitive();

	PRIMITIVE_KIND	m_kind;
	uint32_t		m_size;
	uint32_t		m_uorder;
	uint32_t		m_vorder;
	uint32_t		m_uprecision;
	uint32_t		m_vprecision;

	uint32_t		m_polygonsSize;
    unsigned short	*m_faces;
	unsigned short	*m_polygons;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRYPRIMITIVE_H__890D2E18_2DAC_4094_AE20_BDF7D6FA5DBF__INCLUDED_)

