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
	#include <malloc.h>
	
	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;
	
	#include <GL/glx.h>
#endif



#endif

