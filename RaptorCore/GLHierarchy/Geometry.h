// Geometry.h: interface for the CGeometry class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
#define AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif
#if !defined(AFX_GEOMETRYPRIMITIVE_H__890D2E18_2DAC_4094_AE20_BDF7D6FA5DBF__INCLUDED_)
	#include "GeometryPrimitive.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CShader;
class CTextureFactory;
class CMaterial;
class CTextureUnitSetup;
class CShadedGeometryInstance;
class CGeometryEditor;

//#define DATA_EXTENDED 1
#define DATA_PACKED   1


class RAPTOR_API CGeometry : public CObject3D
{
public:
    //////////////////////////////////////////////////////////////////////
	//	Geometry rendering management
	class RAPTOR_API CRenderingModel
	{
	public:
		typedef enum
		{
			CGL_FRONT_GEOMETRY	=0x1,
			CGL_BACK_GEOMETRY	=0x2,
			CGL_NORMALS			=0x4,
            CGL_TANGENTS		=0x8,
			CGL_TEXTURE			=0x10,
			CGL_WEIGHT			=0x20,
			CGL_COLORS			=0x40,
			CGL_FOG				=0x80,
			CGL_FULLRENDER		=0xFF,	// CGL_FRONT_GEOMETRY|CGL_BACK_GEOMETRY|CGL_MATERIAL|
										// CGL_TEXTURE|CGL_WIREFRAME|CGL_COLORS|CGL_FOG
		} MODEL;

		CRenderingModel(long);

		//! Returns true if the model feature is set
		bool RAPTOR_FASTCALL hasModel(MODEL model) const { return ((m_renderingModel & model) == model); };

		//!	appends a rendering model feature
		void addModel(MODEL);

		//!	removes a rendering model feature
		void removeModel(MODEL);

		//!	assignment operator
		const CRenderingModel& operator=(const CRenderingModel& model);

	private:
		CRenderingModel();
		long	m_renderingModel;
	};


public:
	//!
	//! Construction/Destruction
	//!
	CGeometry(const std::string& name = "GEOMETRY");
	virtual ~CGeometry(void);

	//!
	//! Manage rendered elements of object
	//!
public:
	virtual void setRenderingModel(CRenderingModel::MODEL model);
	void addModel(CRenderingModel::MODEL model);
	virtual void removeModel(CRenderingModel::MODEL model);
	bool RAPTOR_FASTCALL hasModel(CRenderingModel::MODEL model) const { return m_renderingModel.hasModel(model); };
	
	//!
	//!	Geometry creation
	//!
	//! Operators
	CGeometry& operator=(const CGeometry &geo);
		

    //! Creates a geometry primitive. A primitive manages indexes of elementary
    //! geometric structure ( triangles, quads, strips ... ). A primitive is used to actually
    //! draw the geometry, but it is not necessary. In this case, the CGeometry is only
    //! drawn with a set of triangles whose properties are defined here under.
    CGeometryPrimitive*	createPrimitive(CGeometryPrimitive::PRIMITIVE_KIND kind);

	//!	Return the list of primitives currently defining the geometry
	const std::vector<CGeometryPrimitive*>	getPrimitives(void) const { return m_pPrimitives; };

	//!	Removes a specific GeometryPrimitive.
	//!	@param priimitive : a pointer to a primitive to delete. If NULL,
	//!	all primitives will be deleted.
	//! @return : true is the requested primitieve has been removed.
	//! Rq : if all primitives are removed, the geometry may be invalid as
	//!	no polygon data is provided for rendering.
	bool removePrimitive(CGeometryPrimitive *primitive);


	//!	Bulk data initialisers. 
	//!	If 2nd parameter pointer argument is null, these methods reserve memory space for geometry data
	//!	If 2nd parameter pointer argument is not null, geometry data is updated, memory must have been reserved, 
	//!		and the number of vertex attributes is not changed.
	//!	In all methods, the number of items (nbV, nbN, nbT, ... ) is a multiple of the second parameter type.
	void glSetVertices(size_t nbV, GL_COORD_VERTEX* vertices = NULL);
	void glSetNormals(size_t nbN, GL_COORD_VERTEX* normals = NULL);
	void glSetTexCoords(size_t nbT, GL_TEX_VERTEX* texcoords = NULL);
	void glSetTexCoords2(size_t nbT, GL_TEX_VERTEX* texcoords = NULL);
	void glSetWeights(size_t nbW, float* weights = NULL);
	void glSetFogs(size_t nbW, float* fogs = NULL);
	void glSetColors(size_t nbC, CColor::RGBA* rgbaColors = NULL);
	void glSetPolygons(size_t nbP, uint16_t* polygons = NULL);

	//!
	//!		After allocs, data can be set ...
	//!
    //! If relocation mode is used, any data acces must be preceded by a lock and 
    //! followed by a matching unlock. This enables client memory data access.
	//! @return false if relocation is not enabled or not supported or if data is already locked for editing,
	//!	or if access to memory location cannot be locked or forced.
    bool glLockData();
	//! This method also recomputes the BBox if necessary, because it is usually used
	//!	to update geometry's data
    //! @return false if relocation is not enabled or not supported or if data is already unlocked,
	//!	or if access to memory location cannot be locked or forced.
    bool glUnLockData();
    //! 
    //! Setters:
    //!
	void addVertex(float x,float y,float z,float h);
	void addFace(int p1,int p2,int p3);
	void setCoord(size_t numvtx,float x,float y,float z,float h);
	void setTexCoord(size_t numvtx,float u,float v);
	void setWeight(size_t numvtx,float w);
	void setColor(size_t numvtx,float r,float g,float b,float a);
	void setFogCoord(size_t numvtx,float f);
	void setNormal(size_t numvtx,float x,float y,float z,float h);
    void updateBBox(void);
	//!
	//!	Getters:
	//!
	unsigned int nbVertex(void) const { return m_nbVertex; };
	unsigned int nbFace(void) const { return m_nbPolys; };
	void getVertex(size_t numvtx,GL_VERTEX_DATA &v) const;
	float getWeight(size_t numvtx) const;
	void getColor(size_t numvtx,CColor::RGBA &v) const;
	float getFogCoord(size_t numvtx) const;
	void getFace(size_t numface,unsigned int &p1,unsigned int &p2,unsigned int &p3) const;
	void getCoord(size_t numvtx,GL_COORD_VERTEX &v) const;
	void getNormal(size_t numvtx,GL_COORD_VERTEX &v) const;
    void getTangent(size_t numvtx,GL_COORD_VERTEX &v) const;
    void getBiNormal(size_t numvtx,GL_COORD_VERTEX &v) const;
	
    //!
    //!  Geometry operations are performed in a GeometryEditor
    //!
    const CGeometryEditor& getEditor(void);

    //! CObject3D Implementation:
	//!
	//! Manipulators
	DECLARE_OBJECT3D_MANIPULATORS
	virtual void vkRender(CVulkanCommandBuffer& commandBuffer,
						  VkBuffer vertexBinding,
						  VkBuffer indexBinding);

	bool getVertexInputState(std::vector<VkVertexInputBindingDescription>& bindings,
							 std::vector<VkVertexInputAttributeDescription>& vertexInput) const;

    //!
    //! Creates the list of contours for this objet.
    virtual vector<CObject3DContour*> createContours(void);


	//!	Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CGeometryClassID,"Geometry",CObject3D)


protected:
	//!	Pure geometric rendering
	virtual void glRenderGeometry();

	//! Specific constructor for derived classes
	CGeometry(const std::string& name, const CPersistence::CPersistenceClassID &classID);

    friend class CGeometryEditor;

	unsigned int	m_nbVertex;
	unsigned int	m_nbPolys;
    
	
#if defined(DATA_EXTENDED)

    #define VERTEX(i)		geometry[i].vertex
    #define NORMAL(i)		geometry[i].normal
    #define TANGENT(i)		geometry[i].tangent
    #define BINORMAL(i)		geometry[i].binormal
    #define COLOR(i)		geometry[i].color
    #define TEXCOORD0(i)	geometry[i].texCoord0
	#define TEXCOORD1(i)	geometry[i].texCoord1
    #define WEIGHT(i)		geometry[i].weight
    #define FOGCOORD(i)		geometry[i].fog

    GL_VERTEX_DATA  *geometry;

#elif defined(DATA_PACKED)

    #define VERTEX(i)		vertex[i]
    #define NORMAL(i)		normals[i]
    #define TANGENT(i)		tangents[i]
    #define BINORMAL(i)		binormals[i]
    #define COLOR(i)		colors[i]
    #define TEXCOORD0(i)	texcoords[i]
	#define TEXCOORD1(i)	texcoords2[i]
    #define WEIGHT(i)		weightcoords[i]
    #define FOGCOORD(i)		fogcoords[i]

	GL_COORD_VERTEX	*vertex;		// world raw vertex (point)
	GL_TEX_VERTEX	*texcoords;		// texture coordinates : depend on nb vertex
	GL_TEX_VERTEX	*texcoords2;	// second set of texture coordinates : optional usage for custom data of multiple texture targets
	GL_COORD_VERTEX	*normals;		// vertex normals : depend on nb vertex
	CColor::RGBA	*colors;		// vertex color : as they might be generated by lighting, it is not a member of vertex to reduce data transfer
	GL_COORD_VERTEX	*tangents;		//	Complement to geometry : tangent vectors
	GL_COORD_VERTEX	*binormals;		//	Complement to geometry : binormal vectors
   	float			*fogcoords;		// fog coordinates : depend on nb vertex
	float			*weightcoords;	// weight coordinates : depend on nb vertex

#endif
		
    //! Default primitive : triangles when the geometry has no primitive list.
	unsigned short	*polys;	

	//!	Vertex Input State Resource binder
	void				*m_pBinder;



private:
    //! Helper for IO.
    bool importRenderingModel(CRaptorIO& io);

    //! Data is locked for relocation
    bool                m_bDataLocked;

    //! BBox needs update because vertices have been modified.
    bool                m_bUpdateBBox;

    //! The geometry editor. Further developments might specialize it
    CGeometryEditor*    m_pEditor;

    //! The rendering model of the geometry
    CRenderingModel		m_renderingModel;

    //! The list of base primitives
	vector<CGeometryPrimitive*>	m_pPrimitives;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)

