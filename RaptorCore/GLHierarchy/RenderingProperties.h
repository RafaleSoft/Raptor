// RenderingProperties.h: interface for the CRenderingProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
#define AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

// ! Serialization
class CRaptorIO;



//! This class manages global rendering properties
//! in a scene such as texturing, lighting, fogging, ...
class RAPTOR_API CRenderingProperties
{
public:
    typedef enum
    {
        DISABLE,
        ENABLE,
        IGNORE_PROPERTY
    } PROPERTY_SETTING;


public:
	//! Constructor.
	CRenderingProperties();

    //! Copy Constructor.
	CRenderingProperties(const CRenderingProperties &properties);

    //! Assignment
    CRenderingProperties& operator=(const CRenderingProperties &properties);

	//! Destructor.
	virtual ~CRenderingProperties();

    //! Returns the current rendering properties ( or at least the last rendered )
    static CRenderingProperties *GetCurrentProperties(void);


	//! Applies all the properties.
    //! The current property is made current ( provided it is not already current )
    void glPushProperties(void);

    //! Restore previous properties. 
    //! The property set popped MUST be the last pushed,
    //! otherwise this method has no effects.
    void glPopProperties(void);

	//! Enables / Disables rendering with textures.
    //! Default setting for texturing is IGNORE_PROPERTY.
	void setTexturing(PROPERTY_SETTING texturing) { m_bTexturing = texturing; };
    PROPERTY_SETTING getTexturing(void) const { return m_bTexturing; };
    //! Returns the current global texturing state which should have been set by a previous rendering property.
    PROPERTY_SETTING getCurrentTexturing(void) const;


	//! Enables / Disables rendering with lights.
    //! Default setting for lighting is IGNORE_PROPERTY.
	void setLighting(PROPERTY_SETTING lighting) { m_bLighting = lighting; };
    PROPERTY_SETTING getLighting(void) const { return m_bLighting; };
    //! Returns the current global lighting state which should have been set by a previous rendering property.
    PROPERTY_SETTING getCurrentLighting(void) const;


	//! Enables / Disables wire frame rendering.
    //! Default setting for wireframe is IGNORE_PROPERTY.
	void setWireframe(PROPERTY_SETTING wireframe) { m_bWireframe = wireframe; };
    PROPERTY_SETTING getWireframe(void) const { return m_bWireframe; };

	//! Enables / Disables back face culling.
	//! Back face order is a property of a geometry.
    //! Default setting for culling is ENABLE.
	void setCullFace(PROPERTY_SETTING cullface) { m_bCullFace = cullface; };
    PROPERTY_SETTING getCullFace(void) const { return m_bCullFace; };

	//! Enables / Disables wire frame rendering.
    //! Default setting for depth test is ENABLE.
	void setDepthTest(PROPERTY_SETTING depthtest) { m_bDepthTest = depthtest; };
    PROPERTY_SETTING getDepthTest(void) const { return m_bDepthTest; };

    //! Enables / Disables blending.
    //! Default setting for blending is IGNORE_PROPERTY.
	void setBlending(PROPERTY_SETTING blend) { m_bBlend = blend; };
    PROPERTY_SETTING getBlending(void) const { return m_bBlend; };

    //! Enables / Disables stencil test.
    //! Default setting for stenciling is IGNORE_PROPERTY.
	void setStencilTest(PROPERTY_SETTING stencil) { m_bStencilTest = stencil; };
    PROPERTY_SETTING getStencilTest(void) const { return m_bStencilTest; };

	//! Enables / Disables multisampling.
    //! Default setting for multisampling is IGNORE_PROPERTY.
	void setMultisampling(PROPERTY_SETTING multisample) { m_bMultisample = multisample; };
    PROPERTY_SETTING getMultisampling(void) const { return m_bMultisample; };

	//! Enables / Disables color clamping.
    //! Default setting for color clamping is IGNORE_PROPERTY.
	void setFloatClamping(PROPERTY_SETTING clamping) { m_bClampFloats = clamping; };
    PROPERTY_SETTING getFloatClamping(void) const { return m_bClampFloats; };

    //! Defines the buffers to clear for rendering
    //! ( the value is a combination window buffer configuration: @see CGLTypes.h )
    //! Default setting for clearing is CGL_NULL.
	void clear(unsigned int clear) { m_clear = clear; };
    unsigned int getClear(void) const { return m_clear; };


    //! Rendering properties can be serialized though it is not a persistence.
    //!@see CPersistence
	bool exportObject(CRaptorIO& o);

    //! Same as above.
    //! @see CPersistence
	bool importObject(CRaptorIO& i);



private:
    void glRender(void);

	PROPERTY_SETTING	m_bTexturing;
	PROPERTY_SETTING	m_bLighting;
	PROPERTY_SETTING	m_bWireframe;
	PROPERTY_SETTING	m_bCullFace;
	PROPERTY_SETTING	m_bDepthTest;
    PROPERTY_SETTING	m_bBlend;
    PROPERTY_SETTING	m_bStencilTest;
	PROPERTY_SETTING	m_bMultisample;
	PROPERTY_SETTING	m_bClampFloats;
	unsigned int		m_clear;

    CRenderingProperties *m_pPrevious;
    static CRenderingProperties *m_pCurrent;

    static CRenderingProperties     m_globalProperties;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)

