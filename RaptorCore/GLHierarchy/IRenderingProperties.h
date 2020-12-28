/***************************************************************************/
/*                                                                         */
/*  IRenderingProperties.h                                                 */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
#define AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

// ! Serialization
class CRaptorIO;



//! This class manages global rendering properties
//! in a scene such as texturing, lighting, fogging, ...
class RAPTOR_API IRenderingProperties
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
	IRenderingProperties();

    //! Copy Constructor.
	IRenderingProperties(const IRenderingProperties &properties);

    //! Assignment
	IRenderingProperties& operator=(const IRenderingProperties &properties);

	//! Destructor.
	virtual ~IRenderingProperties();

	//!	Read all properties from the GL
	virtual void glGrabProperties(void) = 0;

	//! Applies all the properties.
    //! The current property is made current ( provided it is not already current )
	virtual void glPushProperties(void) = 0;

    //! Restore previous properties. 
    //! The property set popped MUST be the last pushed,
    //! otherwise this method has no effects.
	virtual void glPopProperties(void) = 0;

	//! Enables / Disables rendering with textures.
    //! Default setting for texturing is IGNORE_PROPERTY.
	IRenderingProperties& setTexturing(PROPERTY_SETTING texturing) { m_bTexturing = texturing; return *this;  };
    PROPERTY_SETTING getTexturing(void) const { return m_bTexturing; };
#define enableTexturing setTexturing(IRenderingProperties::ENABLE)
#define disableTexturing setTexturing(IRenderingProperties::DISABLE)
#define ignoreTexturing setTexturing(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables rendering with lights.
    //! Default setting for lighting is IGNORE_PROPERTY.
	IRenderingProperties& setLighting(PROPERTY_SETTING lighting) { m_bLighting = lighting; return *this;  };
    PROPERTY_SETTING getLighting(void) const { return m_bLighting; };
#define enableLighting setLighting(IRenderingProperties::ENABLE)
#define disableLighting setLighting(IRenderingProperties::DISABLE)
#define ignoreLighting setLighting(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables wire frame rendering.
    //! Default setting for wireframe is IGNORE_PROPERTY.
	IRenderingProperties& setWireframe(PROPERTY_SETTING wireframe) { m_bWireframe = wireframe; return *this;  };
    PROPERTY_SETTING getWireframe(void) const { return m_bWireframe; };
#define enableWireframe setWireframe(IRenderingProperties::ENABLE)
#define disableWireframe setWireframe(IRenderingProperties::DISABLE)
#define ignoreWireframe setWireframe(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables back face culling.
	//! Back face order is a property of a geometry.
    //! Default setting for culling is ENABLE.
	IRenderingProperties& setCullFace(PROPERTY_SETTING cullface) { m_bCullFace = cullface; return *this;  };
    PROPERTY_SETTING getCullFace(void) const { return m_bCullFace; };
#define enableCullFace setCullFace(IRenderingProperties::ENABLE)
#define disableCullFace setCullFace(IRenderingProperties::DISABLE)
#define ignoreCullFace setCullFace(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables wire frame rendering.
    //! Default setting for depth test is ENABLE.
	IRenderingProperties& setDepthTest(PROPERTY_SETTING depthtest) { m_bDepthTest = depthtest; return *this;  };
    PROPERTY_SETTING getDepthTest(void) const { return m_bDepthTest; };
#define enableDepthTest setDepthTest(IRenderingProperties::ENABLE)
#define disableDepthTest setDepthTest(IRenderingProperties::DISABLE)
#define ignoreDepthTest setDepthTest(IRenderingProperties::IGNORE_PROPERTY)

    //! Enables / Disables blending.
    //! Default setting for blending is IGNORE_PROPERTY.
	IRenderingProperties& setBlending(PROPERTY_SETTING blend) { m_bBlend = blend; return *this; };
    PROPERTY_SETTING getBlending(void) const { return m_bBlend; };
#define enableBlending setBlending(IRenderingProperties::ENABLE)
#define disableBlending setBlending(IRenderingProperties::DISABLE)
#define ignoreBlending setBlending(IRenderingProperties::IGNORE_PROPERTY)

    //! Enables / Disables stencil test.
    //! Default setting for stenciling is IGNORE_PROPERTY.
	IRenderingProperties& setStencilTest(PROPERTY_SETTING stencil) { m_bStencilTest = stencil; return *this; };
    PROPERTY_SETTING getStencilTest(void) const { return m_bStencilTest; };
#define enableStencilTest setStencilTest(IRenderingProperties::ENABLE)
#define disableStencilTest setStencilTest(IRenderingProperties::DISABLE)
#define ignoreStencilTest setStencilTest(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables multisampling.
    //! Default setting for multisampling is IGNORE_PROPERTY.
	IRenderingProperties& setMultisampling(PROPERTY_SETTING multisample) { m_bMultisample = multisample; return *this; };
    PROPERTY_SETTING getMultisampling(void) const { return m_bMultisample; };
#define enableMultisampling setMultisampling(IRenderingProperties::ENABLE)
#define disableMultisampling setMultisampling(IRenderingProperties::DISABLE)
#define ignoreMultisampling setMultisampling(IRenderingProperties::IGNORE_PROPERTY)

	//! Enables / Disables color clamping.
    //! Default setting for color clamping is IGNORE_PROPERTY.
	IRenderingProperties& setFloatClamping(PROPERTY_SETTING clamping) { m_bClampFloats = clamping; return *this; };
    PROPERTY_SETTING getFloatClamping(void) const { return m_bClampFloats; };
#define enableFloatClamping setFloatClamping(IRenderingProperties::ENABLE)
#define disableFloatClamping setFloatClamping(IRenderingProperties::DISABLE)
#define ignoreFloatClamping setFloatClamping(IRenderingProperties::IGNORE_PROPERTY)

    //! Defines the buffers to clear for rendering
    //! ( the value is a combination window buffer configuration: @see CGLTypes.h )
    //! Default setting for clearing is CGL_NULL.
	IRenderingProperties& clear(unsigned int clear) { m_clear = clear; return *this; };
    unsigned int getClear(void) const { return m_clear; };


    //! Rendering properties can be serialized though it is not a persistence.
    //!@see CPersistence
	bool exportObject(CRaptorIO& o);

    //! Same as above.
    //! @see CPersistence
	bool importObject(CRaptorIO& i);



protected:
	PROPERTY_SETTING	m_bTexturing;
	PROPERTY_SETTING	m_bLighting;
	PROPERTY_SETTING	m_bWireframe;
	PROPERTY_SETTING	m_bCullFace;
	PROPERTY_SETTING	m_bDepthTest;
    PROPERTY_SETTING	m_bBlend;
    PROPERTY_SETTING	m_bStencilTest;
	PROPERTY_SETTING	m_bMultisample;
	PROPERTY_SETTING	m_bClampFloats;
	uint32_t			m_clear;

	IRenderingProperties	*m_pPrevious;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)

