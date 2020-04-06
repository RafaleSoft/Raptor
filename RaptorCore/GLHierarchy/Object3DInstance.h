// Object3DInstance.h: interface for the CObject3DInstance class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
#define AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class IRenderingProperties;
class CShader;



//! This class implements a simple 3D object instance :
//! - the thransform can be overriden ( the paramater of the instance is simply the geometric transform. )
//! - any kind of object can be used.
//! - the shading and the shader used by the current rendering properties can also be overriden
//! The use of this class is highly recommended as geometries are atatic
//! objects, any transform is computationnally very expensive.
class RAPTOR_API CObject3DInstance : public CObject3D
{
public:
	CObject3DInstance(CObject3D * const instance = NULL,const std::string& name = "OBJECT_INSTANCE");
	virtual ~CObject3DInstance();

    //! Changes the instantiated object.
	void instantiate(CObject3D * const instance);

    //! Returns the object instantiated ( or NULL if none )
    CObject3D* const getObject(void) const { return m_pReference; };
	
	//!	Returns the 'OpenGL' compatible transform matrix.
    //! ( i.e. this matrix can be applied directly using glMultMatrixf )
	const GL_MATRIX &getTransform(void) const
	{  return m_transform; }

	//!	Returns the 'Raptor' compatible transform matrix
	//!	( opengl transposed, more convenient for math )
	void getTransform(CGenericMatrix<float> &T) const;

	//!	Initialise object's transform to Identity.
	void resetTransform(void);

    //! CObject3D Implementation:
    //! 
	//!	Manipulators :
	//!	See CObject3D for the list of manipulators.
	//!	CObject3DInstance implements transforms in the
	//! referential of 'self' ( thus they are relative
	//!	to 'ident' matrix from the viewer's viewpoint :
	//!	Translations and Rotations are independant ).
	DECLARE_OBJECT3D_MANIPULATORS

     //! Creates the list of contours for this objet.
    virtual vector<CObject3DContour*> createContours(void);
		
	//! Rendering
    //!
    //! This method is only for use with a 3DEngine, and allows 
    //! a separate clipping processing for full scene effects
	virtual bool RAPTOR_FASTCALL fullClip(const CGenericMatrix<float> &transform);

    //! This methods renders the BBox of the object.
	virtual void glRenderBBox(bool filled = false);

    //! This method renders the object clipped to viewport.
    //! @see CObject3D.
	virtual void glClipRender();

    //! Override Display rendering properties tu use local object specific shading.
	void overrideShading(const IRenderingProperties& override);

    //! Returns the internal shader. A new shader is allocated if necessary.
    //virtual CShader	* const getShader(void);

     //! Assigns a shader to the instance ( and not to the holded object). The previous one is released.
    //virtual void setShader(CShader *shader);


	//!	I/O object serialization is implemented
    //! @see CPersistence.
	DECLARE_IO
	DECLARE_CLASS_ID(CObject3DInstanceClassID,"Object3DInstance",CObject3D)


protected:
	//!	Specific constructor for GLLod, a specialisation of instances
	CObject3DInstance(	const CPersistence::CPersistenceClassID & classID,
						const std::string& name = "OBJECT_INSTANCE");

	//!	Direct access to transformed BBox
	void getBoundingBox(GL_COORD_VERTEX &min,GL_COORD_VERTEX &max) const;

    //!	Inherit from persistence to observe shader life-cycle
	virtual void unLink(const CPersistence* obj);

    //! Implements a notifier handler
    void notifyFromReference(CObject3D* reference);

	//!	The differences between an object and it's instance
	//!	N.B.: to avoid matrix transposition before it is loaded by OpenGL
	//!	operations should produce the transpose of transform
	GL_MATRIX				m_transform;
	
    //! The real object instanciated
	CObject3D				*m_pReference;


private:
    //CShader					*m_pShader;
	IRenderingProperties	*m_pOverride;
    CContainerNotifier<CObject3D*> *m_pObserver;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)

