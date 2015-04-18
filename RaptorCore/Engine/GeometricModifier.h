// GeometricModifier.h: interface for the CGeometricModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRICMODIFIER_H__5C0729A2_6F0C_42CA_9033_CAFB26247761__INCLUDED_)
#define AFX_GEOMETRICMODIFIER_H__5C0729A2_6F0C_42CA_9033_CAFB26247761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3D;
class CGeometry;
class CGeometryWrapper;

class RAPTOR_API CGeometricModifier : public CModifier  
{
public:
	CGeometricModifier(	TIME_FUNCTION_TYPE timeFunction,
						float			timeArg0,
						float			timeArg1,
						float			timeArg2,
						float			timeArg3,
						USER_FUNCTION   pUserFunction,
						const CPersistence::CPersistenceClassID& id,
						const std::string& name = "GL_GEOMETRICMODIFIER");
	virtual ~CGeometricModifier();

	//!	Base implementation
    virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_GEOMETRIC_MODIFIER; };

	//!	Returns the Geometry object updated by the modifier
	void setGeometry(CGeometry *geometry);

	//!	Returns the Geometry object updated by the modifier
	CObject3D *getObject() const;


protected:
	//!	Implements CPersistence
	virtual void unLink(const CPersistence*);


private:
	//!	Handler to be overloaded to declare an update
	//!	of the managed geometry vertices
	virtual void RAPTOR_FASTCALL updateVertices(float dt,GL_COORD_VERTEX *vertices);

	//!	Handler to be overloaded to declare an update
	//!	of the managed geometry normals
	virtual void RAPTOR_FASTCALL updateNormals(float dt,GL_COORD_VERTEX	*normals);

	//!	Handler to be overloaded to declare an update
	//!	of the managed geometry texture coordinates
	virtual void RAPTOR_FASTCALL updateTexCoords(float dt,GL_TEX_VERTEX	*texcoords);

	//!	Compute physics resultant after dt time increment
	//!	This method should not be called directly.
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

	//!	Helper to clean all used resources.
	void RAPTOR_FASTCALL cleanWrapper();

	//!	A geometry wrapper to intercept rendering
	//! and perform a geometry update in geometry buffers
	CGeometryWrapper	*m_pWrapper;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRICMODIFIER_H__5C0729A2_6F0C_42CA_9033_CAFB26247761__INCLUDED_)

