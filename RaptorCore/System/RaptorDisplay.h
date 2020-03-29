// RaptorDisplay.h: interface for the CRaptorDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
#define AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
    #include "GLHierarchy/TextureGenerator.h"
#endif

#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "System/RaptorDisplayConfig.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class IViewPoint;
class C3DScene;
class IRenderingProperties;



class RAPTOR_API CRaptorDisplay : public CPersistence, public ITextureGenerator
{
public:
	//! This method returns the current active display, or NULL if none is active.
	static CRaptorDisplay * const GetCurrentDisplay(void);

	//! Returns the global rendering properties.
	virtual IRenderingProperties &getRenderingProperties(void) const;

	//! Creates a view point compatible with this display.
	virtual IRenderingProperties *const createRenderingProperties(void) const;


	//!
	//!	Display management
	//!

	//!	Create and attach the display to a device context.
	//!	For a newly created display, the underlying context is created
	//!	and the bound to the display. For a display already bound at least once
	//! without errors, this method simply makes the current context active for the display.
	//!	Return true if the display is bound without errors, false if the display is
	//! unbounded or if there is an error.
	virtual bool glvkBindDisplay(const RAPTOR_HANDLE& device);

    //! If the display is bound to a device, it returns the current device if it is valid.
    //! Here, the method returns a void device as a generic display cannot be bound, see derived classes
    virtual RAPTOR_HANDLE getCurrentDevice(void) const { RAPTOR_HANDLE noDevice ; return noDevice; }

	//! To unbound the display, simply call this method
	//!	Return true if the display is unbound without errors, false if the display is
	//! unbounded before call or if there is an error.
	virtual bool glvkUnBindDisplay(void);

	//!	Current user ( eye ) viewpoint used to render the current root scene of the display. 
    //! The viewpoint has priority over any user OpenGL transforms in rendering: the initial MODELVIEW
	//!	transform is updated according to the point of view before glRender is called
    //! RQ: If view point is modified, it be passed to this method to "reapply it"
	virtual void setViewPoint(IViewPoint *viewPoint);

	//! Returns the display view point.
	virtual IViewPoint *const getViewPoint(void) const;

	//! Creates a view point compatible with this display.
	virtual IViewPoint *const createViewPoint(void) const;

	//!	Allocates Geometry, Textures and Uniforms resources for this display.
	virtual void glvkAllocateResources(void);

	//!	Release allocated Geometry, Textures and Uniforms resources for this display.
	virtual void glvkReleaseResources(void);


	//!
	//!	Display usage
	//!

	//! Resize the display to handle user interface events.
	//! - sx,sy : define the width and height of the display
	//! - ox,oy : define an offset of the origin of the display within it's container
	virtual void glResize(uint32_t sx, uint32_t sy,
						  uint32_t ox, uint32_t oy) = 0;

	//!	Returns the root scene of the display
	C3DScene *const getRootScene(void) const { return m_pRootScene; };

	//!	Add a scene to the display
	void addScene(C3DScene* const scene );

	//! Select the scene as the current root scene.
	bool selectScene( const std::string& sname);

    //! Adds a sub display : these display displays are rendered before any  rendering
    //! commands are issued for this display and immediatly after binding 
    //! ( ViewPoint, RenderingProperties and Engine are properly set-up / rendered )
    void addSubDisplay(CRaptorDisplay *pDisplay);

	//!	Renders the display, if it has been bound.
	//! Returns true if rendered without errors, false otherwise.
	virtual bool glRender(void) = 0;

	//! Renders a Raptor generic opaque pointer.
	static void glRender(const RAPTOR_HANDLE& handle);

	//! Returns the display core status.
	//! @See CRaptorDisplayConfig
	virtual bool glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const;

	//! Applies a status like one returned here above,
	//! except the basic config which can only be set at creation.
	bool glApplyStatus(CRaptorDisplayConfig &state,unsigned long query);

	//!	Return the number of frames rendered during the last second.
	virtual float getFPS(void) const { return 0.0f; };

	//!	Return the current frame rate : it is the inverse of the rendering time
	//! So it vary a lot but is more likely to be real time.
	virtual float getRTFPS(void) const { return 0.0f; };

	//!	Return the delay of a whole frame, including
	//! buffer swapping and driver workload
	virtual float getFrameTime(void) const { return 0.0f; };

	//!	Return the render time of a single frame (likely to be realtime)
	//! It measures only scene drawing without any deferred tasks
	virtual float getRenderTime(void) const { return 0.0f; };

    //! Grabs a bloc of pixels from the display.
    //! The returned array must be deleted by the user. It contains the pixels data
    //! corresponding to the required display atributes :
    //! - dimensions
    //! - format    ( if supported )
    //! - returns false if format is not supported or if an error occured.
    //! Actual data is returned in data parameter, and data size is returned in size
	virtual bool glGrab(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
						uint8_t* &data,size_t& size) const = 0;

	//!	Blits a bloc of pixels from this display.
	//!	Pixels are transferred to pDst.
	virtual bool glBlit(uint32_t xSrc, uint32_t ySrc, uint32_t widthSrc, uint32_t heightSrc,
						uint32_t xDst, uint32_t yDst, uint32_t widthDst, uint32_t heightDst,
						CRaptorDisplay *pDst) const = 0;

    //! Implements CTextureGenerator.
    //! This generator is not yet implemented.in this version
    virtual ITextureGenerator::GENERATOR_KIND getKind(void) const { return ITextureGenerator::NONE; };

    //! This method returns the width of the generator
	virtual uint32_t getGenerateWidth(void) const;

    //! This method returns the height of the generator
	virtual uint32_t getGenerateHeight(void) const;



    //! @see CPersistence.
    //! This method allows stream display creation
    DECLARE_IO
	DECLARE_CLASS_ID(CRaptorDisplayClassID,"RaptorDisplay",CPersistence)


protected:
	CRaptorDisplay( const CPersistence::CPersistenceClassID& id,
					const std::string& name="RAPTOR_DISPLAY");
	virtual ~CRaptorDisplay();

    //! Implements CPersistence
    virtual void unLink(const CPersistence* obj);

	//! This method enables subclasses to replace rendering properties
	//!	Registration is not implemented, it remains the responsibility of subclasses
	//! (method is protected)
	void setRenderingProperties(IRenderingProperties *properties);

	
private:
	//!	Display factory
	friend class Raptor;
	
	CRaptorDisplay(const CRaptorDisplay&);
	CRaptorDisplay& operator=(const CRaptorDisplay&);

	static CRaptorDisplay	*m_pCurrentDisplay;

	IViewPoint				*m_pViewPoint;
	bool					m_bDeleteViewPoint;
    bool                    m_bApplyViewPointModel;

	IRenderingProperties    *m_pProperties;

    C3DScene				*m_pRootScene;
	vector<C3DScene*>       m_pScenes;
    vector<CRaptorDisplay*> m_pSubDisplays;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)


