// GLXView.h: interface for the CGLXView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_)
#define AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
    #include "System/RenderEntryPoint.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
    #include "System/RaptorDisplayConfig.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplay;


class CGLXView  : public CRenderEntryPoint
{
public:
	CGLXView();
	virtual ~CGLXView();


    //! Rendering : Implements RenderEntryPoint
    virtual void glRender();

    //! Creates a valid CGLWnd to display openGL.
    //! This is the only method that should be called to
    //! create the CGLWnd object as a GL display.
    bool GLCreateWindow ( const string &name, 
                          const CRaptorDisplayConfig& glCreateStruct);

    //!     Initialises the GL context owned by this object.
    //!     Ensures that the current context is valid during initialisation
    //!     Other inits ( texture, extensions, ... ) in other functions might
    //!     not be done correctly if the current context is not selected ( glMakeCurrent )
    virtual void GLInitContext(void);

    //! This method is used to notify this view that it is about to be resized
    //! ( GLXApplication handles Window events and dispatch then here )
    virtual void OnSize(int sx, int sz);


    //!
    //! Accessors:
    //!
    
    //! This operator creates a raptor handle from this to easily
    //! pass a GLXView to a RaptorApplication
    operator RAPTOR_HANDLE() const;

    //! Returns the display associated with this view.
    //! The display is only valid if the window has properly
    //! been created.
    const CRaptorDisplay* const getDisplay(void) const { return m_pDisplay; };


    //! Returns the handle to the window created here above.
    RAPTOR_HANDLE getWindow(void) const { return m_wnd; };




private:
    friend class CGLXContextManager;
    CRaptorDisplay *m_pDisplay;
    RAPTOR_HANDLE    m_wnd;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_)
