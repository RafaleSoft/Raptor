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

class CViewPoint;
class C3DScene;
class CRenderingProperties;



class RAPTOR_API CRaptorDisplay : public CPersistence, public ITextureGenerator
{
public:
	//! This method returns the current active display, or NULL if none is active.
	static CRaptorDisplay * const GetCurrentDisplay(void);

	//! Returns the global rendering properties.
	virtual CRenderingProperties *getRenderingProperties(void) const { return m_pProperties; };

	//!	Create and attach the display to a device context.
	//!	For a newly created display, the underlying context is created
	//!	and the bound to the display. For a display already bound at least once
	//! without errors, this method simply makes the current context active for the display.
	//!	Return true if the display is bound without errors, false if the display is
	//! unbounded or if there is an error.
	virtual bool glBindDisplay(const RAPTOR_HANDLE& device);

    //! If the display is bound to a device, it returns the current device if it is valid.
    //! Here, the method returns a void device as a generic display cannot be bound, see derived classes
    virtual RAPTOR_HANDLE getCurrentDevice(void) const { RAPTOR_HANDLE noDevice ; return noDevice; }

	//! To unbound the display, simply call this method
	//!	Return true if the display is unbound without errors, false if the display is
	//! unbounded before call or if there is an error.
	virtual bool glUnBindDisplay(void);

	//!	Current user ( eye ) viewpoint used to render the current root scene of the display. 
    //! The viewpoint has priority over any user OpenGL transforms in rendering: the initial MODELVIEW
	//!	transform is updated according to the point of view before glRender is called
    //! RQ: If view point is modified, it be passed to this method to "reapply it"
	virtual void setViewPoint(CViewPoint *viewPoint);

	//! Returns the display view point.
	virtual CViewPoint *const getViewPoint(void) const;

	//! Resize the display to handle user interface events.
	//! - sx,sy : define the width and height of the display
	//! - ox,oy : define an offset of the origin of the display within it's container
	virtual void glResize(	unsigned int sx,unsigned int sy,
							unsigned int ox, unsigned int oy) = 0;

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
	bool glApplyStatus(const CRaptorDisplayConfig &state,unsigned long query);

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
    virtual bool glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
						unsigned char* &data,unsigned int& size) const = 0;

	//!	Blits a bloc of pixels from this display.
	//!	Pixels are transferred to pDst.
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const = 0;

    //! Implements CTextureGenerator.
    //! This generator is not yet implemented.in this version
    virtual ITextureGenerator::GENERATOR_KIND getKind(void) const { return ITextureGenerator::NONE; };

    //! Implements CTextureGenerator
    //! This class is virtual, generation is only implemented in subclasses
    //virtual void glGenerate(CTextureObject* );

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const;



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

    //! This method is for internal use only. Raptor call it to
    //! free all possible ressources while the display is still usable, 
    //! and before the purge is effective.
    virtual void glReleaseResources(void);

	
private:
	friend class Raptor;
	
	CRaptorDisplay(const CRaptorDisplay&);
	CRaptorDisplay& operator=(const CRaptorDisplay&);

	static CRaptorDisplay	*m_pCurrentDisplay;

	CViewPoint				*m_pViewPoint;
	bool					m_bDeleteViewPoint;
    bool                    m_bApplyViewPointModel;

	CRenderingProperties    *m_pProperties;

    C3DScene				*m_pRootScene;
	vector<C3DScene*>       m_pScenes;
    vector<CRaptorDisplay*> m_pSubDisplays;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)


