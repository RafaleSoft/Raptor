// RaptorExtensions.cpp: implementation of the RaptorExtensions class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorGLExtensions.h"
#endif
#ifndef __glext_macros_h_
	#include "GLEXTMacros.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif


RAPTOR_NAMESPACE_BEGIN
#if defined(_WIN32)
	#if defined(WGL_ARB_extensions_string)
		PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC CRaptorGLExtensions::wglGetExtensionsStringARB = NULL;
	#endif
		IMPLEMENT_RAPTOR_WGL_ARB_pixel_format(CRaptorGLExtensions)
		IMPLEMENT_RAPTOR_WGL_ARB_pbuffer(CRaptorGLExtensions)
		IMPLEMENT_RAPTOR_WGL_ARB_render_texture(CRaptorGLExtensions)
		IMPLEMENT_RAPTOR_WGL_EXT_swap_control(CRaptorGLExtensions)
#endif
RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRaptorGLExtensions::CRaptorGLExtensions(const std::string &ext)
{
	extensions = ext;
}

CRaptorGLExtensions::~CRaptorGLExtensions()
{

}

void CRaptorGLExtensions::glInitExtensions(void)
{
#if defined(_WIN32)
	#if defined(WGL_ARB_extensions_string)
		{
			wglGetExtensionsStringARB = (PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC)wglGetProcAddress("wglGetExtensionsStringARB");
		}
	#endif

	#if defined(WGL_ARB_pixel_format)
		wglGetPixelFormatAttribivARB = (PFN_WGL_GET_PIXEL_FORMAT_ATTRIBIV_ARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
		wglGetPixelFormatAttribfvARB = (PFN_WGL_GET_PIXEL_FORMAT_ATTRIBFV_ARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
		wglChoosePixelFormatARB = (PFN_WGL_CHOOSE_PIXEL_FORMAT_ARB_PROC)wglGetProcAddress("wglChoosePixelFormatARB");
	#endif
	
		IMPLEMENT_WGL_ARB_render_texture(this)
	IMPLEMENT_WGL_ARB_pbuffer(this)
#endif

	IMPLEMENT_GL_ARB_texture_compression(this)
	IMPLEMENT_GL_NV_vertex_array_range(this)
	IMPLEMENT_GL_ARB_vertex_buffer_object(this)
	IMPLEMENT_GL_EXT_compiled_vertex_array(this)
	IMPLEMENT_GL_ARB_multitexture(this)
	IMPLEMENT_GL_ARB_transpose_matrix(this)
	IMPLEMENT_GL_EXT_vertex_weighting(this)
	IMPLEMENT_GL_NV_register_combiners(this)
	IMPLEMENT_GL_EXT_fog_coord(this)
	IMPLEMENT_GL_EXT_point_parameters(this)
	IMPLEMENT_GL_EXT_secondary_color(this)
	IMPLEMENT_WGL_EXT_swap_control(this)
	IMPLEMENT_GL_ARB_vertex_program(this)
	IMPLEMENT_GL_ARB_imaging(this)
	IMPLEMENT_GL_ARB_occlusion_query(this)
	IMPLEMENT_GL_NV_point_sprite(this)
	IMPLEMENT_GL_ARB_point_parameters(this)
	IMPLEMENT_GL_EXT_stencil_two_side(this)
	IMPLEMENT_GL_ARB_color_buffer_float(this)
	IMPLEMENT_GL_EXT_texture3D(this)
	IMPLEMENT_GL_ARB_shader_objects(this)
	IMPLEMENT_GL_ARB_vertex_shader(this)
	IMPLEMENT_GL_ARB_geometry_shader4(this)
	IMPLEMENT_GL_ARB_draw_buffers(this)
	IMPLEMENT_GL_EXT_blend_color(this)
	IMPLEMENT_GL_EXT_framebuffer_object(this)
	IMPLEMENT_GL_EXT_framebuffer_blit(this)
	IMPLEMENT_GL_EXT_framebuffer_multisample(this)
	IMPLEMENT_GL_ARB_uniform_buffer_object(this)

    CATCH_GL_ERROR
	
	defaultInit();
}

void RAPTOR_APICALL glActiveTextureARB__default(GLenum target)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Using unavailable extension glActiveTextureARB");
#endif
}

void RAPTOR_APICALL glMultiTexCoord4fvARB__default(GLenum target, const GLfloat *v)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Using unavailable extension glMultiTexCoord4fvARB");
#endif
	glTexCoord4fv(v);
}

void CRaptorGLExtensions::defaultInit(void)
{
#if defined(GL_ARB_multitexture)
	if (glMultiTexCoord4fvARB == NULL)
		glMultiTexCoord4fvARB = glMultiTexCoord4fvARB__default;
	if (glActiveTextureARB == NULL)
		glActiveTextureARB = glActiveTextureARB__default;
#endif
}

bool CRaptorGLExtensions::glIsExtensionSupported(const std::string &ext) const
{	
	bool supported = false;

    supported = (string::npos != extensions.find(ext.data()));
    
	return supported;
}


