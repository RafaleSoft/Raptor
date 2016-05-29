// ContextManager.h: interface for the CContextManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
#define AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "System/RaptorDisplay.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "GLHierarchy/ObjectReference.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(__RAPTOR_VKEXT_H__)
	#include "vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorExtensions;
class CTextureObject;


//!	This class implements OpenGL contexts management.
class CContextManager  
{
public:
	typedef     unsigned long   PIXEL_BUFFER_ID;
	typedef     long            RENDERING_CONTEXT_ID;

public:
	//!	Acces to the unique instance.
	static CContextManager *GetInstance(void);

	//!	Destructor is available, to be able to reinit extensions
	//!	if necessary, but depends on drivers.
	virtual ~CContextManager();

	//!	Retuns the RaptorExtensions class to acces implementation
	//! of openGL extensions.
	virtual const CRaptorExtensions *const glGetExtensions(void) = 0;

	//!	Check display config fields with underlying system capabilities.
	//! If some fields of rdc are not valid, they are adjusted, and false
	//! is returned to indicate that rdc has been modified.
	//! If rdc is properly initialised, it is left unchanged and true is returned.
	virtual bool validateConfig(CRaptorDisplayConfig& rdc);


#if defined(VK_VERSION_1_0)
	//!	Initialise a global Vulkan instance and pysical devices for
	//!	further logical devices queries.
	//! @return false is vulkan library cannot be initialized, true otherwise.
	bool vkInitContext(void);

	RENDERING_CONTEXT_ID vkCreateContext(const RAPTOR_HANDLE& device,int displayMode);

	void vkDestroyContext(RENDERING_CONTEXT_ID ctx);

	void vkMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx);

	void vkSwapBuffers(RENDERING_CONTEXT_ID ctx);

	void vkSwapVSync(unsigned int framerate);
#endif

	//!	This method creates a "default' window for Raptor's internal use.
	//!	It can also be used when the GUI API is used, for example
	//! with full screen applications. This can help to minimise the size and dependencies of application.
    //! Rq : if display_mode of attributes pcs contains CGL_GENERIC, a generic GL context will be created.
    //! This kind of context are limited with regard to available extensions, but Raptor use it to create the very
    //! first GL context, which is necessary to load full driver extensions subsequently. User might never 
    //! need to specify a generic context for the window except on limited hardwares.
    //! Finally, in case of requesting a CGL_GENERIC display_mode, the RENDERING_CONTEXT_ID is returned
    //! in the third parameter. Otherwise, the parameter is left unmodified.
    //! @param pcs : a display configuration for which a window will be created.
    //! @param pDisplay : a reference to a pointer that receive the display created for the window, or NULL if pcs contains CGL_GENERIC.
    //! @param ctx : a reference to a variable what will receive the context id if pcs does not contain CGL_GENERIC
    //! @return : a handle to the window that can be used to bind the display for rendering.
	virtual RAPTOR_HANDLE glCreateWindow(const CRaptorDisplayConfig &pcs,CRaptorDisplay *&pDisplay,RENDERING_CONTEXT_ID &ctx) = 0;

    //! Destroy a window created using the method here above.
    virtual bool glDestroyWindow(const RAPTOR_HANDLE& wnd) = 0;

	//!	This method creates an openGL context.
	//!	@param device : an opaque pointer to a HAL resource object ( window or GC )
	//!	for which the context will be created.
	//!	@param displayMode : the configuration of the requested display ( @see CRaptorDisplay
	//!	and all the types in CGLTypes.h )
	//!    @return : 0 if creation successfull and then GL context is valid, any other value if an error occured.
	virtual RENDERING_CONTEXT_ID glCreateContext(const RAPTOR_HANDLE& device,int displayMode) = 0;

	//!	Same as above, excepted that the context will be 'extended' : i.e. all openGL
	//! extended features will be accessible through wglExt.
	virtual RENDERING_CONTEXT_ID glCreateExtendedContext(const RAPTOR_HANDLE& device,int displayMode) = 0;

	//! Abstraction for rendering context access.
	virtual RENDERING_CONTEXT_ID glGetCurrentContext(void) const = 0;
    
    //! Returns the device for the requested rendering context if it exists.
    //! The returned handle is null if ctx is invalid or if no context ctx is not bound to a device.
	virtual RAPTOR_HANDLE getDevice(CContextManager::RENDERING_CONTEXT_ID ctx) const;

    //! Returns the context for the requested device if it exists.
    //! The returned handle is null if device is invalid or if no context ctx is not bound to the requested device.
    virtual RENDERING_CONTEXT_ID getContext(RAPTOR_HANDLE device) const = 0;

	//! Abstraction for rendering context access.
	virtual void glMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx) = 0;

	//! Abstraction for rendering context access.
	virtual void glDestroyContext(RENDERING_CONTEXT_ID ctx) = 0;

	//! Abstraction for 'swap buffers' rendering.
	//! ( though for some optimize of vsync reasons, this method
	//!	might use bloc copy transfer ).
	virtual void glSwapBuffers(RENDERING_CONTEXT_ID ctx) = 0;

	//! Abstraction for 'swap framerate control'.
	//! This directly method depends on HAL capabilities.
	//!	Returns true if swap control is properly set.
	virtual bool glSwapVSync(unsigned int nbVSync) const = 0;

	//!	Pixel buffers:
	//!	@param pcs : the pixel buffer rendering caps. Same as 
	//! standard rendering context ( except the 'render to texture' spec )
    //! @return : a valid new pixel buffer id
	virtual PIXEL_BUFFER_ID glCreatePBuffer(const CRaptorDisplayConfig &pcs) = 0;

    //! Destroy a pixel buffer
    //! @param pbuffer : a valied pixel buffer id to destroy
    //! @return : true is destruction successfull, false in case of error (e.g. invalid buffer )
	virtual bool glDestroyPBuffer(PIXEL_BUFFER_ID pbuffer) = 0;

	virtual void glBindPBuffer(PIXEL_BUFFER_ID pbuffer,CTextureObject::CUBE_FACE selectBuffer = CTextureObject::CGL_CUBEMAP_NONE) = 0;

	virtual bool glIsPBuffer(PIXEL_BUFFER_ID pbuffer) const = 0;

	virtual bool glBindTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer) = 0;

	virtual bool glReleaseTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer) = 0;


protected:
	CContextManager();

	void glDrawLogo(void);

    void glRemoveLogo(void);


private:
	CTextureObject	*glBuildLogo(void);

	//!	An extensions manager to access Vulkan API.
#if defined(VK_VERSION_1_0)
	CRaptorExtensions	*m_pExtensions;

	VkInstance			m_globalInstance;

	unsigned int				m_nbPhysicalDevices;
	VkPhysicalDevice			*m_pPhysicalDevices;
	VkPhysicalDeviceProperties	*m_pProperties;
	VkPhysicalDeviceFeatures	*m_pFeatures;

	//unsigned int		m_nbDevices;

	typedef struct
	{
		VkDevice			device;
		VkCommandPool		commandPool;
#ifdef VK_KHR_win32_surface
		VkSurfaceKHR		surface;
	#ifdef VK_KHR_swapchain
		VkSwapchainKHR		swapChain;
	#endif
		uint32_t			currentImage;
		VkImage				*pImages;
#endif
	} VK_CONTEXT;
	VK_CONTEXT	*m_pVkContext;
#endif
#ifdef VK_KHR_swapchain
	VkPresentModeKHR	presentMode;
#endif

	RAPTOR_HANDLE				m_logo;
	CReference<CTextureObject>	m_pLogo;

	static CContextManager	*p_manager;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)


