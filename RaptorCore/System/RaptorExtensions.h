// RaptorExtensions.h: interface for the RaptorExtensions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
#define AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_GLEXT_H__)
	#include "Glext.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class handles all entry points to OGL extensions.
//! An instance of this class is the one and only one access to these entry points.
class RAPTOR_API CRaptorExtensions  
{
public:
	CRaptorExtensions(const std::string &ext);
	virtual ~CRaptorExtensions();

    //! Use static linkage for wgl extensions as they are necessary for
    //! all kind of commands for rendering context management
	DECLARE_WGL_ARB_extensions_string(STATIC_LINKAGE)
	DECLARE_WGL_ARB_pixel_format(STATIC_LINKAGE)
	DECLARE_WGL_ARB_pbuffer(STATIC_LINKAGE)
	DECLARE_WGL_ARB_render_texture(STATIC_LINKAGE)
	DECLARE_WGL_EXT_swap_control(STATIC_LINKAGE)
	
	//!    Vendor specific available extensions will vary with rendering context attributes
	DECLARE_GL_ARB_texture_compression(DEFAULT_LINKAGE)
	DECLARE_GL_NV_vertex_array_range(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_vertex_buffer_object(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_compiled_vertex_array(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_multitexture(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_transpose_matrix(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_vertex_weighting(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_fog_coord(DEFAULT_LINKAGE)
	DECLARE_GL_NV_register_combiners(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_point_parameters(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_secondary_color(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_vertex_program(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_imaging(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_occlusion_query(DEFAULT_LINKAGE)
	DECLARE_GL_NV_point_sprite(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_point_parameters(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_stencil_two_side(DEFAULT_LINKAGE)
    DECLARE_GL_ARB_color_buffer_float(DEFAULT_LINKAGE)
    DECLARE_GL_EXT_texture3D(DEFAULT_LINKAGE)
    DECLARE_GL_ARB_shader_objects(DEFAULT_LINKAGE)
    DECLARE_GL_ARB_vertex_shader(DEFAULT_LINKAGE)
	DECLARE_GL_ARB_geometry_shader4(DEFAULT_LINKAGE)
    DECLARE_GL_ARB_draw_buffers(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_blend_color(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_framebuffer_object(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_framebuffer_blit(DEFAULT_LINKAGE)
	DECLARE_GL_EXT_framebuffer_multisample(DEFAULT_LINKAGE)

	//!	Initialise all extensions on drivers/hardware.
	void glInitExtensions(void);
	
	//! Search for a specific extension string in all available extensions.
	//! Extensions must have been initialized with a valid OpenGL context
	//! from which extensions are extracted.
	//! @return true is ext is found in extensions list, false otherwise.
	bool glIsExtensionSupported(const std::string &ext) const;
	
	//! Same requirements as above.
	//! @return the list of all loaded extensions.
	const std::string& glExtensions(void) const
	{ return extensions; }

private:
	//!	Provides default functions after InitExtensions has been done.
	void defaultInit(void);
	
	//! String buffers containing OGL server capabilities
	string			extensions;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)

