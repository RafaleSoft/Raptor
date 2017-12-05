//! This header provides GLX specific OpenGL extensions.
//! It is included where needed, depending on the content of 
//! the file CodeGeneration.h. User should never include this file 
//! directly because it is managed by Glext.h
#ifndef __GLX_RAPTOR_GLEXT_H__
#define __GLX_RAPTOR_GLEXT_H__

#define DECLARE_WGL_ARB_extensions_string(LINKAGE)
#define	DECLARE_WGL_ARB_pixel_format(STATIC_LINKAGE)
#define	DECLARE_WGL_ARB_pbuffer(STATIC_LINKAGE)
#define	DECLARE_WGL_ARB_render_texture(STATIC_LINKAGE)
#define	DECLARE_WGL_EXT_swap_control(STATIC_LINKAGE)

#ifdef GLX_EXTENSIONS
    #define GLX_USE_GL				1
    #define GLX_BUFFER_SIZE			2
    #define GLX_LEVEL				3
    #define GLX_RGBA				4
    #define GLX_DOUBLEBUFFER		5
    #define GLX_STEREO				6
    #define GLX_AUX_BUFFERS			7
    #define GLX_RED_SIZE			8
    #define GLX_GREEN_SIZE			9
    #define GLX_BLUE_SIZE			10
    #define GLX_ALPHA_SIZE			11
    #define GLX_DEPTH_SIZE			12
    #define GLX_STENCIL_SIZE		13
    #define GLX_ACCUM_RED_SIZE		14
    #define GLX_ACCUM_GREEN_SIZE	15
    #define GLX_ACCUM_BLUE_SIZE		16
    #define GLX_ACCUM_ALPHA_SIZE	17

    #include <X11/Xlib.h>
    #include <X11/Xutil.h>

    typedef struct __GLXcontextRec *GLXContext;
    typedef XID GLXDrawable;

    Bool glXQueryExtension( Display *dpy, int *errorb, int *event );
    Bool glXMakeCurrent( Display *dpy, GLXDrawable drawable, GLXContext ctx);
    void glXDestroyContext( Display *dpy, GLXContext ctx );
    void glXSwapBuffers( Display *dpy, GLXDrawable drawable );
    XVisualInfo* glXChooseVisual( Display *dpy, int screen, int *attribList );
    GLXContext glXCreateContext (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
#endif



#endif

