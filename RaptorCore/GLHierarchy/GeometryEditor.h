// GeometryEditor.h: interface for the CGeometryEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
#define AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CGeometry;


class RAPTOR_API CGeometryEditor  
{
public:
	//!	Texture generation model
	typedef enum TEXTURE_MODEL_t
	{
		XY_PLANE,
		XZ_PLANE,
		YZ_PLANE,
		XY_BOX,
		NB_TEXTURE_MODELS
	} TEXTURE_MODEL;


    //!		Geometry operations

    //!
    //!  TexCoords
    //!     apply sx and sy factors to texture coordinates
    void scaleTexCoords(float sx, float sy) const;
	//!
	//!		generates texture coordinates with  appropriate model
	void genTexCoords(	TEXTURE_MODEL model, 
						float xFactor=1.0f, float xOffset=0.0f,
						float yFactor=1.0f, float yOffset=0.0f, 
						float zFactor=1.0f, float zOffset=0.0f) const;

	//!
	//! Normals
	//!		reverse normals to avoid reverse backface culling
	void flipNormals(void) const;
    //!		share a vertex with another Geometry, recomputing the normal
	//!		as necessary. BEWARE : the normals must exist 
	void shareVertex(unsigned int own_vrtx,unsigned int other_vrtx,CGeometry &geo) const;
    //!
    //!     generates normal for each vertex.
	//!		Rq: ambient occlusion parameters are lost and must be re-generated.
	void genNormals(bool rebuild=false) const;
	//!
    //!     generates tangent & binormal for each normal.
    void genBinormals(void) const;
	
	//!
	//! Vertex
	//!		minimizes the number of vertices
	//!		that exists to create rough angles on geometries.
	//!		In many cases, those angles are not visible.
	void minimize(void) const;
	bool isMinimized(void) const { return m_bMinimized; };

    //!
	//!		move a vertex to a specified position, applying a tension 
	//!		to move surrounding vertices.
	void moveVertex(unsigned int numVertex,
					GL_COORD_VERTEX moveTo,
					unsigned int tension);

	//!
	//!		generate the "surface equivalent" value for ambient occlusion
	//!		normals must be generated before this call.
	//!		Normals re-generation require a new call to genSurfaceElements
	void genSurfaceElements(void) const;
	bool hasSurfaceElements(void) const { return m_bSurfaceElements; };

	static void updateSurfaceElements(	const vector<CGeometry*> &geometries,
										const vector<CGenericMatrix<float> > &transforms);

    //!
    //! Polygons
    //!      computes a set of GeometryPrimitives as a set of strips ( triangles ) deduced
    //!      from the original triangle set of the geometry.
    void strip(void) const;


private:
	friend class CGeometry;

    //! Returns the shared 
    CGeometryEditor(CGeometry *pGeometry);

	//! Forbidden destructor
	virtual ~CGeometryEditor();

	//! Helpers
	void computeVertexArea(	unsigned short p1,
							unsigned short p2,
							unsigned short p3) const;

	mutable bool m_bMinimized;
	mutable bool m_bSurfaceElements;
    CGeometry*  m_pGeometry;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)

