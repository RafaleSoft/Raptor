/***************************************************************************/
/*                                                                         */
/*  GLEXTMacros.h    											           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __glext_macros_h_
#define __glext_macros_h_

#ifndef __RAPTOR_GLEXT_H__
	#include "Glext.h"
#endif


//!
//! Platform specific extension loader
//!
#if defined(WIN32)
	#if defined RAPTOR_DEBUG_MODE_GENERATION
		extern "C" PROC glGetProcAddress(const char *name, const std::string& file, int line);
		#define GET_PROC_ADDRESS(name) glGetProcAddress(name, __FILE__, __LINE__)
	#else
		#define GET_PROC_ADDRESS(name) wglGetProcAddress(name)
	#endif
#elif defined(LINUX)
	extern "C" void (*glXGetProcAddress(const GLubyte *name))(void);
	#define GET_PROC_ADDRESS(name) glXGetProcAddress((GLubyte*)name)
#endif
//!
//!
//!     Macro definition for extensions declaration
//!
//!

#ifdef GL_ARB_multitexture
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_multitexture
	#define IMPLEMENT_RAPTOR_GL_ARB_multitexture(target) \
		PFN_GL_MULTI_TEX_COORD_1D_ARB_PROC		target::glMultiTexCoord1dARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_1DV_ARB_PROC		target::glMultiTexCoord1dvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_1F_ARB_PROC		target::glMultiTexCoord1fARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_1FV_ARB_PROC		target::glMultiTexCoord1fvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_2D_ARB_PROC		target::glMultiTexCoord2dARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_2DV_ARB_PROC		target::glMultiTexCoord2dvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_2F_ARB_PROC		target::glMultiTexCoord2fARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_2FV_ARB_PROC		target::glMultiTexCoord2fvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_3D_ARB_PROC		target::glMultiTexCoord3dARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_3DV_ARB_PROC		target::glMultiTexCoord3dvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_3F_ARB_PROC		target::glMultiTexCoord3fARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_3FV_ARB_PROC		target::glMultiTexCoord3fvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_4D_ARB_PROC		target::glMultiTexCoord4dARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_4DV_ARB_PROC		target::glMultiTexCoord4dvARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_4F_ARB_PROC		target::glMultiTexCoord4fARB = NULL; \
		PFN_GL_MULTI_TEX_COORD_4FV_ARB_PROC		target::glMultiTexCoord4fvARB = NULL; \
		PFN_GL_ACTIVE_TEXTURE_ARB_PROC			target::glActiveTextureARB = NULL; \
		PFN_GL_CLIENT_ACTIVE_TEXTURE_ARB_PROC	target::glClientActiveTextureARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_multitexture(target)
#endif

#ifdef GL_ARB_transpose_matrix
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_transpose_matrix
	#define IMPLEMENT_RAPTOR_GL_ARB_transpose_matrix(target) \
		PFN_GL_LOAD_TRANSPOSE_MATRIX_F_ARB_PROC	target::glLoadTransposeMatrixfARB = NULL; \
		PFN_GL_LOAD_TRANSPOSE_MATRIX_D_ARB_PROC	target::glLoadTransposeMatrixdARB = NULL; \
		PFN_GL_MULT_TRANSPOSE_MATRIX_F_ARB_PROC	target::glMultTransposeMatrixfARB = NULL; \
		PFN_GL_MULT_TRANSPOSE_MATRIX_D_ARB_PROC	target::glMultTransposeMatrixdARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_transpose_matrix(target)
#endif

#ifdef GL_EXT_vertex_weighting
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_vertex_weighting
	#define IMPLEMENT_RAPTOR_GL_EXT_vertex_weighting(target) \
		PFN_GL_VERTEX_WEIGHT_F_EXT_PROC			target::glVertexWeightfEXT = NULL; \
		PFN_GL_VERTEX_WEIGHT_FV_EXT_PROC		target::glVertexWeightfvEXT = NULL; \
		PFN_GL_VERTEX_WEIGHT_POINTER_EXT_PROC	target::glVertexWeightPointerEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_vertex_weighting(target)
#endif

#ifdef GL_EXT_point_parameters
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_point_parameters
	#define IMPLEMENT_RAPTOR_GL_EXT_point_parameters(target) \
		PFN_GL_POINT_PARAMETER_F_EXT_PROC 	target::glPointParameterfEXT = NULL; \
		PFN_GL_POINT_PARAMETER_FV_EXT_PROC	target::glPointParameterfvEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_point_parameters(target)
#endif

#ifdef GL_ARB_point_parameters
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_point_parameters
	#define IMPLEMENT_RAPTOR_GL_ARB_point_parameters(target) \
		PFN_GL_POINT_PARAMETER_F_ARB_PROC 	target::glPointParameterfARB = NULL; \
		PFN_GL_POINT_PARAMETER_FV_ARB_PROC	target::glPointParameterfvARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_point_parameters(target)
#endif

#ifdef WGL_EXT_swap_control
	#ifndef IMPLEMENT_RAPTOR_WGL_EXT_swap_control
	#define IMPLEMENT_RAPTOR_WGL_EXT_swap_control(target) \
		PFN_WGL_SWAP_INTERVAL_PROC		target::wglSwapIntervalEXT = NULL; \
		PFN_WGL_GET_SWAP_INTERVAL_PROC	target::wglGetSwapIntervalEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_WGL_EXT_swap_control(target)
#endif

#ifdef GL_EXT_secondary_color
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_secondary_color
	#define IMPLEMENT_RAPTOR_GL_EXT_secondary_color(target) \
		PFN_GL_SECONDARY_COLOR_3F_EXT_PROC		target::glSecondaryColor3fEXT = NULL; \
		PFN_GL_SECONDARY_COLOR_3D_EXT_PROC		target::glSecondaryColor3dEXT = NULL; \
		PFN_GL_SECONDARY_COLOR_3FV_EXT_PROC		target::glSecondaryColor3fvEXT = NULL; \
		PFN_GL_SECONDARY_COLOR_3DV_EXT_PROC		target::glSecondaryColor3dvEXT = NULL; \
		PFN_GL_SECONDARY_COLOR_POINTER_EXT_PROC	target::glSecondaryColorPointerEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_secondary_color(target)
#endif

#ifdef GL_ARB_texture_compression
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_texture_compression
	#define IMPLEMENT_RAPTOR_GL_ARB_texture_compression(target) \
		PFN_GL_COMPRESSED_TEX_IMAGE_3D_ARB_PROC		target::glCompressedTexImage3DARB = NULL; \
		PFN_GL_COMPRESSED_TEX_IMAGE_2D_ARB_PROC		target::glCompressedTexImage2DARB = NULL; \
		PFN_GL_COMPRESSED_TEX_IMAGE_1D_ARB_PROC		target::glCompressedTexImage1DARB = NULL; \
		PFN_GL_COMPRESSED_TEX_SUB_IMAGE_3D_ARB_PROC	target::glCompressedTexSubImage3DARB = NULL; \
		PFN_GL_COMPRESSED_TEX_SUB_IMAGE_2D_ARB_PROC	target::glCompressedTexSubImage2DARB = NULL; \
		PFN_GL_COMPRESSED_TEX_SUB_IMAGE_1D_ARB_PROC	target::glCompressedTexSubImage1DARB = NULL; \
		PFN_GL_GET_COMPRESSED_TEX_IMAGE_ARB_PROC	target::glGetCompressedTexImageARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_texture_compression(target)
#endif

#ifdef GL_EXT_compiled_vertex_array
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_compiled_vertex_array
	#define IMPLEMENT_RAPTOR_GL_EXT_compiled_vertex_array(target) \
		PFN_GL_LOCK_ARRAYS_EXT_PROC		target::glLockArraysEXT = NULL; \
		PFN_GL_UNLOCK_ARRAYS_EXT_PROC	target::glUnlockArraysEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_compiled_vertex_array(target)
#endif

#ifdef GL_NV_register_combiners
	#ifndef IMPLEMENT_RAPTOR_GL_NV_register_combiners
	#define IMPLEMENT_RAPTOR_GL_NV_register_combiners(target) \
		PFN_GL_COMBINER_PARAMETER_FV_NV_PROC					target::glCombinerParameterfvNV = NULL; \
		PFN_GL_COMBINER_PARAMETER_IV_NV_PROC					target::glCombinerParameterivNV = NULL; \
		PFN_GL_COMBINER_PARAMETER_F_NV_PROC						target::glCombinerParameterfNV = NULL; \
		PFN_GL_COMBINER_PARAMETER_I_NV_PROC						target::glCombinerParameteriNV = NULL; \
		PFN_GL_COMBINER_INPUT_NV_PROC							target::glCombinerInputNV = NULL; \
		PFN_GL_COMBINER_OUTPUT_NV_PROC							target::glCombinerOutputNV = NULL; \
		PFN_GL_FINAL_COMBINER_INPUT_NV							target::glFinalCombinerInputNV = NULL; \
		PFN_GL_GET_COMBINER_INPUT_PARAMETER_FV_NV_PROC			target::glGetCombinerInputParameterfvNV = NULL; \
		PFN_GL_GET_COMBINER_INPUT_PARAMETER_IV_NV_PROC			target::glGetCombinerInputParameterivNV = NULL; \
		PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_FV_NV_PROC			target::glGetCombinerOutputParameterfvNV = NULL; \
		PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_IV_NV_PROC			target::glGetCombinerOutputParameterivNV = NULL; \
		PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_FV_NV_PROC	target::glGetFinalCombinerInputParameterfvNV = NULL; \
		PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_IV_NV_PROC	target::glGetFinalCombinerInputParameterivNV = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_NV_register_combiners(target)
#endif

#ifdef GL_ARB_imaging
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_imaging
	#define IMPLEMENT_RAPTOR_GL_ARB_imaging(target) \
		PFN_GL_BLEND_COLOR_ARB_PROC		target::glBlendColorARB = NULL; \
		PFN_GL_BLEND_EQUATION_ARB_PROC	target::glBlendEquationARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_imaging(target)
#endif

#ifdef GL_EXT_fog_coord
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_fog_coord
	#define IMPLEMENT_RAPTOR_GL_EXT_fog_coord(target) \
		PFN_GL_FOG_COORD_F_EXT_PROC			target::glFogCoordfEXT = NULL; \
		PFN_GL_FOG_COORD_D_EXT_PROC			target::glFogCoorddEXT = NULL; \
		PFN_GL_FOG_COORD_FV_EXT_PROC		target::glFogCoordfvEXT = NULL; \
		PFN_GL_FOG_COORD_DV_EXT_PROC		target::glFogCoorddvEXT = NULL; \
		PFN_GL_FOG_COORD_POINTER_EXT_PROC	target::glFogCoordPointerEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_fog_coord(target)
#endif

#ifdef GL_NV_vertex_array_range
	#ifndef IMPLEMENT_RAPTOR_GL_NV_vertex_array_range
	#define IMPLEMENT_RAPTOR_GL_NV_vertex_array_range(target) \
		PFN_GL_VERTEX_ARRAY_RANGE_NV_PROC		target::glVertexArrayRangeNV = NULL;\
		PFN_GL_FLUSH_VERTEX_ARRAY_RANGE_NV_PROC	target::glFlushVertexArrayRangeNV = NULL;\
		PFN_WGL_ALLOCATE_MEMORY_NV_PROC			target::wglAllocateMemoryNV = NULL;\
		PFN_WGL_FREE_MEMORY_NV_PROC				target::wglFreeMemoryNV = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_NV_vertex_array_range(target)
#endif

#ifdef GL_NV_point_sprite
	#ifndef IMPLEMENT_RAPTOR_GL_NV_point_sprite
	#define IMPLEMENT_RAPTOR_GL_NV_point_sprite(target) \
		PFN_GL_POINT_PARAMETER_I_NV_PROC 	target::glPointParameteriNV = NULL; \
		PFN_GL_POINT_PARAMETER_IV_NV_PROC	target::glPointParameterivNV = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_NV_point_sprite(target)
#endif

#ifdef WGL_ARB_pbuffer
	#ifndef IMPLEMENT_RAPTOR_WGL_ARB_pbuffer
	#define IMPLEMENT_RAPTOR_WGL_ARB_pbuffer(target) \
		PFN_WGL_CREATE_PBUFFER_ARB_PROC target::wglCreatePBufferARB = NULL;\
		PFN_WGL_GET_PBUFFER_DC_ARB_PROC target::wglGetPBufferDCARB = NULL;\
		PFN_WGL_RELEASE_PBUFFER_DC_ARB_PROC target::wglReleasePBufferDCARB = NULL;\
		PFN_WGL_DESTROY_PBUFFER_ARB_PROC target::wglDestroyPBufferARB = NULL;\
		PFN_WGL_QUERY_PBUFFER_ARB_PROC target::wglQueryPBufferARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_WGL_ARB_pbuffer(target)
#endif

#ifdef WGL_ARB_render_texture
	#ifndef	IMPLEMENT_RAPTOR_WGL_ARB_render_texture
	#define IMPLEMENT_RAPTOR_WGL_ARB_render_texture(target)\
		PFN_WGL_BIND_TEX_IMAGE_ARB_PROC target::wglBindTexImageARB = NULL;\
		PFN_WGL_RELEASE_TEX_IMAGE_ARB_PROC target::wglReleaseTexImageARB = NULL;\
		PFN_WGL_SET_PBUFFER_ATTRIB_ARB_PROC target::wglSetPbufferAttribARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_WGL_ARB_render_texture(target)
#endif

#ifdef WGL_ARB_pixel_format
	#ifndef IMPLEMENT_RAPTOR_WGL_ARB_pixel_format
	#define IMPLEMENT_RAPTOR_WGL_ARB_pixel_format(target)\
		PFN_WGL_GET_PIXEL_FORMAT_ATTRIBIV_ARB_PROC target::wglGetPixelFormatAttribivARB = NULL;\
		PFN_WGL_GET_PIXEL_FORMAT_ATTRIBFV_ARB_PROC target::wglGetPixelFormatAttribfvARB = NULL;\
		PFN_WGL_CHOOSE_PIXEL_FORMAT_ARB_PROC target::wglChoosePixelFormatARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_WGL_ARB_pixel_format(target)
#endif

#ifdef GL_ARB_vertex_buffer_object
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_vertex_buffer_object
	#define IMPLEMENT_RAPTOR_GL_ARB_vertex_buffer_object(target) \
		PFN_BIND_BUFFER_ARB_PROC		target::glBindBufferARB = NULL;\
		PFN_DELETE_BUFFERS_ARB_PROC	target::glDeleteBuffersARB = NULL;\
		PFN_GEN_BUFFERS_ARB_PROC		target::glGenBuffersARB = NULL;\
		PFN_IS_BUFFERS_ARB_PROC		target::glIsBufferARB = NULL; \
		PFN_BUFFER_DATA_ARB_PROC		target::glBufferDataARB = NULL; \
		PFN_BUFFER_SUB_DATA_ARB_PROC target::glBufferSubDataARB = NULL; \
		PFN_GET_BUFFER_SUB_DATA_ARB_PROC target::glGetBufferSubDataARB = NULL; \
		PFN_MAP_BUFFER_ARB_PROC		target::glMapBufferARB = NULL; \
		PFN_UNMAP_BUFFER_ARB_PROC	target::glUnmapBufferARB = NULL; \
		PFN_GET_BUFFER_PARAMETER_IV_ARB_PROC target::glGetBufferParameterivARB = NULL; \
		PFN_GET_BUFFER_POINTER_V_ARB_PROC target::glGetBufferPointervARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_vertex_buffer_object(target)
#endif


#if defined(GL_ARB_vertex_program)
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_vertex_program
	#define IMPLEMENT_RAPTOR_GL_ARB_vertex_program(target) \
		PFN_VERTEX_ATTRIB_1S_ARB_PROC				target::glVertexAttrib1sARB = NULL; \
		PFN_VERTEX_ATTRIB_1F_ARB_PROC				target::glVertexAttrib1fARB = NULL; \
		PFN_VERTEX_ATTRIB_1D_ARB_PROC				target::glVertexAttrib1dARB = NULL; \
		PFN_VERTEX_ATTRIB_2S_ARB_PROC				target::glVertexAttrib2sARB = NULL; \
		PFN_VERTEX_ATTRIB_2F_ARB_PROC				target::glVertexAttrib2fARB = NULL; \
		PFN_VERTEX_ATTRIB_2D_ARB_PROC				target::glVertexAttrib2dARB = NULL; \
		PFN_VERTEX_ATTRIB_3S_ARB_PROC				target::glVertexAttrib3sARB = NULL; \
		PFN_VERTEX_ATTRIB_3F_ARB_PROC				target::glVertexAttrib3fARB = NULL; \
		PFN_VERTEX_ATTRIB_3D_ARB_PROC				target::glVertexAttrib3dARB = NULL; \
		PFN_VERTEX_ATTRIB_4S_ARB_PROC				target::glVertexAttrib4sARB = NULL; \
		PFN_VERTEX_ATTRIB_4F_ARB_PROC				target::glVertexAttrib4fARB = NULL; \
		PFN_VERTEX_ATTRIB_4D_ARB_PROC				target::glVertexAttrib4dARB = NULL; \
		PFN_VERTEX_ATTRIB_4NUB_ARB_PROC				target::glVertexAttrib4NubARB = NULL; \
		PFN_VERTEX_ATTRIB_1SV_ARB_PROC				target::glVertexAttrib1svARB = NULL; \
		PFN_VERTEX_ATTRIB_1FV_ARB_PROC				target::glVertexAttrib1fvARB = NULL; \
		PFN_VERTEX_ATTRIB_1DV_ARB_PROC				target::glVertexAttrib1dvARB = NULL; \
		PFN_VERTEX_ATTRIB_2SV_ARB_PROC				target::glVertexAttrib2svARB = NULL; \
		PFN_VERTEX_ATTRIB_2FV_ARB_PROC				target::glVertexAttrib2fvARB = NULL; \
		PFN_VERTEX_ATTRIB_2DV_ARB_PROC				target::glVertexAttrib2dvARB = NULL; \
		PFN_VERTEX_ATTRIB_3SV_ARB_PROC				target::glVertexAttrib3svARB = NULL; \
		PFN_VERTEX_ATTRIB_3FV_ARB_PROC				target::glVertexAttrib3fvARB = NULL; \
		PFN_VERTEX_ATTRIB_3DV_ARB_PROC				target::glVertexAttrib3dvARB = NULL; \
		PFN_VERTEX_ATTRIB_4BV_ARB_PROC				target::glVertexAttrib4bvARB = NULL; \
		PFN_VERTEX_ATTRIB_4SV_ARB_PROC				target::glVertexAttrib4svARB = NULL; \
		PFN_VERTEX_ATTRIB_4IV_ARB_PROC				target::glVertexAttrib4ivARB = NULL; \
		PFN_VERTEX_ATTRIB_4UBV_ARB_PROC				target::glVertexAttrib4ubvARB = NULL; \
		PFN_VERTEX_ATTRIB_4USV_ARB_PROC				target::glVertexAttrib4usvARB = NULL; \
		PFN_VERTEX_ATTRIB_4UIV_ARB_PROC				target::glVertexAttrib4uivARB = NULL; \
		PFN_VERTEX_ATTRIB_4FV_ARB_PROC				target::glVertexAttrib4fvARB = NULL; \
		PFN_VERTEX_ATTRIB_4DV_ARB_PROC				target::glVertexAttrib4dvARB = NULL; \
		PFN_VERTEX_ATTRIB_4NBV_ARB_PROC				target::glVertexAttrib4NbvARB = NULL; \
		PFN_VERTEX_ATTRIB_4NSV_ARB_PROC				target::glVertexAttrib4NsvARB = NULL; \
		PFN_VERTEX_ATTRIB_4NIV_ARB_PROC				target::glVertexAttrib4NivARB = NULL; \
		PFN_VERTEX_ATTRIB_4NUBV_ARB_PROC			target::glVertexAttrib4NubvARB = NULL; \
		PFN_VERTEX_ATTRIB_4NUSV_ARB_PROC			target::glVertexAttrib4NusvARB = NULL; \
		PFN_VERTEX_ATTRIB_4NUIV_ARB_PROC			target::glVertexAttrib4NuivARB = NULL; \
		PFN_VERTEX_ATTRIB_POINTER_ARB_PROC			target::glVertexAttribPointerARB = NULL; \
		PFN_ENABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC		target::glEnableVertexAttribArrayARB = NULL; \
		PFN_DISABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC	target::glDisableVertexAttribArrayARB = NULL; \
		PFN_PROGRAM_STRING_ARB_PROC					target::glProgramStringARB = NULL; \
		PFN_BIND_PROGRAM_ARB_PROC					target::glBindProgramARB = NULL; \
		PFN_DELETE_PROGRAMS_ARB_PROC				target::glDeleteProgramsARB = NULL; \
		PFN_GEN_PROGRAMS_ARB_PROC					target::glGenProgramsARB = NULL; \
		PFN_PROGRAM_ENV_PARAMETER_4F_ARB_PROC		target::glProgramEnvParameter4fARB = NULL; \
		PFN_PROGRAM_ENV_PARAMETER_4D_ARB_PROC		target::glProgramEnvParameter4dARB = NULL; \
		PFN_PROGRAM_ENV_PARAMETER_4DV_ARB_PROC		target::glProgramEnvParameter4dvARB = NULL; \
		PFN_PROGRAM_ENV_PARAMETER_4FV_ARB_PROC		target::glProgramEnvParameter4fvARB = NULL; \
		PFN_PROGRAM_LOCAL_PARAMETER_4F_ARB_PROC		target::glProgramLocalParameter4fARB = NULL; \
		PFN_PROGRAM_LOCAL_PARAMETER_4D_ARB_PROC		target::glProgramLocalParameter4dARB = NULL; \
		PFN_PROGRAM_LOCAL_PARAMETER_4DV_ARB_PROC	target::glProgramLocalParameter4dvARB = NULL; \
		PFN_PROGRAM_LOCAL_PARAMETER_4FV_ARB_PROC	target::glProgramLocalParameter4fvARB = NULL; \
		PFN_GET_PROGRAM_ENV_PARAMETER_FV_ARB_PROC	target::glGetProgramEnvParameterfvARB = NULL; \
		PFN_GET_PROGRAM_ENV_PARAMETER_DV_ARB_PROC	target::glGetProgramEnvParameterdvARB = NULL; \
		PFN_GET_PROGRAM_LOCAL_PARAMETER_FV_ARB_PROC	target::glGetProgramLocalParameterfvARB = NULL; \
		PFN_GET_PROGRAM_LOCAL_PARAMETER_DV_ARB_PROC	target::glGetProgramLocalParameterdvARB = NULL; \
		PFN_GET_PROGRAM_IV_ARB_PROC					target::glGetProgramivARB = NULL; \
		PFN_GET_PROGRAM_STRING_ARB_PROC				target::glGetProgramStringARB = NULL; \
		PFN_GET_VERTEX_ATTRIB_DV_ARB_PROC			target::glGetVertexAttribdvARB = NULL; \
		PFN_GET_VERTEX_ATTRIB_FV_ARB_PROC			target::glGetVertexAttribfvARB = NULL; \
		PFN_GET_VERTEX_ATTRIB_IV_ARB_PROC			target::glGetVertexAttribivARB = NULL; \
		PFN_GET_VERTEX_ATTRIB_POINTERV_ARB_PROC		target::glGetVertexAttribPointervARB = NULL; \
		PFN_IS_PROGRAM_ARB_PROC						target::glIsProgramARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_vertex_program(target)
#endif


#if defined(GL_ARB_occlusion_query)
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_occlusion_query
	#define IMPLEMENT_RAPTOR_GL_ARB_occlusion_query(target) \
		PFN_GEN_QUERIES_ARB_PROC			target::glGenQueriesARB = NULL;\
		PFN_DELETE_QUERIES_ARB_PROC			target::glDeleteQueriesARB = NULL;\
		PFN_IS_QUERY_ARB_PROC				target::glIsQueryARB = NULL;\
		PFN_BEGIN_QUERY_ARB_PROC			target::glBeginQueryARB = NULL;\
		PFN_END_QUERY_ARB_PROC				target::glEndQueryARB = NULL;\
		PFN_GET_QUERY_IV_ARB_PROC			target::glGetQueryivARB = NULL;\
		PFN_GET_QUERY_OBJECT_IV_ARB_PROC	target::glGetQueryObjectivARB = NULL;\
		PFN_GET_QUERY_OBJECT_UIV_ARB_PROC	target::glGetQueryObjectuivARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_occlusion_query(target)
#endif

#if defined(GL_EXT_stencil_two_side)
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_stencil_two_side
	#define IMPLEMENT_RAPTOR_GL_EXT_stencil_two_side(target) \
		PFN_GL_ACTIVE_STENCIL_FACE_EXT_PROC	target::glActiveStencilFaceEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_stencil_two_side(target)
#endif

#if defined(GL_ARB_color_buffer_float)
	#ifndef IMPLEMENT_RAPTOR_GL_ARB_color_buffer_float
	#define IMPLEMENT_RAPTOR_GL_ARB_color_buffer_float(target) \
		PFN_CLAMP_COLOR_ARB_PROC	target::glClampColorARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_ARB_color_buffer_float(target)
#endif

#if defined(GL_EXT_texture3D)
	#ifndef IMPLEMENT_RAPTOR_GL_EXT_texture3D
	#define IMPLEMENT_RAPTOR_GL_EXT_texture3D(target) \
		PFN_GL_TEX_IMAGE3D_EXT_PROC	target::glTexImage3DEXT = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_GL_EXT_texture3D(target)
#endif

#if defined(GL_ARB_shader_objects)
    #ifndef IMPLEMENT_RAPTOR_GL_ARB_shader_objects
    #define IMPLEMENT_RAPTOR_GL_ARB_shader_objects(target) \
        PFN_DELETE_OBJECT_ARB_PROC				target::glDeleteObjectARB = NULL;\
        PFN_GET_HANDLE_ARB_PROC					target::glGetHandleARB = NULL;\
        PFN_DETACH_OBJECT_ARB_PROC				target::glDetachObjectARB = NULL;\
        PFN_CREATE_SHADER_OBJECT_ARB_PROC		target::glCreateShaderObjectARB = NULL;\
        PFN_SHADER_SOURCE_ARB_PROC				target::glShaderSourceARB = NULL;\
        PFN_COMPILE_SHADER_ARB_PROC				target::glCompileShaderARB = NULL;\
        PFN_CREATE_PROGRAM_OBJECT_ARB_PROC		target::glCreateProgramObjectARB = NULL;\
        PFN_ATTACH_OBJECT_ARB_PROC				target::glAttachObjectARB = NULL;\
        PFN_LINK_PROGRAM_ARB_PROC				target::glLinkProgramARB = NULL;\
        PFN_USE_PROGRAM_OBJECT_ARB_PROC			target::glUseProgramObjectARB = NULL;\
        PFN_VALIDATE_PROGRAM_ARB_PROC			target::glValidateProgramARB = NULL;\
        PFN_UNIFORM_1F_ARB_PROC					target::glUniform1fARB = NULL;\
        PFN_UNIFORM_2F_ARB_PROC					target::glUniform2fARB = NULL;\
        PFN_UNIFORM_3F_ARB_PROC					target::glUniform3fARB = NULL;\
        PFN_UNIFORM_4F_ARB_PROC					target::glUniform4fARB = NULL;\
        PFN_UNIFORM_1I_ARB_PROC					target::glUniform1iARB = NULL;\
        PFN_UNIFORM_2I_ARB_PROC					target::glUniform2iARB = NULL;\
        PFN_UNIFORM_3I_ARB_PROC					target::glUniform3iARB = NULL;\
        PFN_UNIFORM_4I_ARB_PROC					target::glUniform4iARB = NULL;\
        PFN_UNIFORM_1FV_ARB_PROC				target::glUniform1fvARB = NULL;\
        PFN_UNIFORM_2FV_ARB_PROC				target::glUniform2fvARB = NULL;\
        PFN_UNIFORM_3FV_ARB_PROC				target::glUniform3fvARB = NULL;\
        PFN_UNIFORM_4FV_ARB_PROC				target::glUniform4fvARB = NULL;\
        PFN_UNIFORM_1IV_ARB_PROC				target::glUniform1ivARB = NULL;\
        PFN_UNIFORM_2IV_ARB_PROC				target::glUniform2ivARB = NULL;\
        PFN_UNIFORM_3IV_ARB_PROC				target::glUniform3ivARB = NULL;\
        PFN_UNIFORM_4IV_ARB_PROC				target::glUniform4ivARB = NULL;\
        PFN_UNIFORM_MATRIX_2FV_ARB_PROC			target::glUniformMatrix2fvARB = NULL;\
        PFN_UNIFORM_MATRIX_3FV_ARB_PROC			target::glUniformMatrix3fvARB = NULL;\
        PFN_UNIFORM_MATRIX_4FV_ARB_PROC			target::glUniformMatrix4fvARB = NULL;\
        PFN_GET_OBJECT_PARAMETER_FV_ARB_PROC	target::glGetObjectParameterfvARB = NULL;\
        PFN_GET_OBJECT_PARAMETER_IV_ARB_PROC	target::glGetObjectParameterivARB = NULL;\
        PFN_GET_INFO_LOG_ARB_PROC				target::glGetInfoLogARB = NULL;\
        PFN_GET_ATTACHED_OBJECTS_ARB_PROC		target::glGetAttachedObjectsARB = NULL;\
        PFN_GET_UNIFORM_LOCATION_ARB_PROC		target::glGetUniformLocationARB = NULL;\
        PFN_GET_ACTIVE_UNIFORM_ARB_PROC			target::glGetActiveUniformARB = NULL;\
        PFN_GET_UNIFORM_FV_ARB_PROC				target::glGetUniformfvARB = NULL;\
        PFN_GET_UNIFORM_IV_ARB_PROC				target::glGetUniformivARB = NULL;\
        PFN_GET_SHADER_SOURCE_ARB_PROC			target::glGetShaderSourceARB = NULL;
    #endif
#else
    #define IMPLEMENT_RAPTOR_GL_ARB_shader_objects(target)
#endif

#if defined(GL_ARB_draw_buffers)
    #ifndef IMPLEMENT_RAPTOR_GL_ARB_draw_buffers
    #define IMPLEMENT_RAPTOR_GL_ARB_draw_buffers(target) \
        PFN_DRAW_BUFFERS_ARB	target::glDrawBuffersARB = NULL;
    #endif
#else
    #define IMPLEMENT_RAPTOR_GL_ARB_draw_buffers(target)
#endif

#if defined(DECLARE_GL_EXT_framebuffer_object)
    #ifndef IMPLEMENT_RAPTOR_DECLARE_GL_EXT_framebuffer_object
    #define IMPLEMENT_RAPTOR_DECLARE_GL_EXT_framebuffer_object(target) \
		PFN_GL_IS_RENDER_BUFFER_EXT_PROC							target::glIsRenderbufferEXT = NULL; \
		PFN_GL_BIND_RENDER_BUFFER_EXT_PROC							target::glBindRenderbufferEXT = NULL; \
		PFN_GL_DELETE_RENDER_BUFFERS_EXT_PROC						target::glDeleteRenderbuffersEXT = NULL; \
		PFN_GL_GEN_RENDER_BUFFERS_EXT_PROC							target::glGenRenderbuffersEXT = NULL; \
		PFN_GL_RENDER_BUFFER_STORAGE_EXT_PROC						target::glRenderbufferStorageEXT = NULL; \
		PFN_GL_GET_RENDER_BUFFER_PARAMETER_IV_EXT_PROC				target::glGetRenderbufferParameterivEXT = NULL; \
		PFN_GL_IS_FRAME_BUFFER_EXT_PROC								target::glIsFramebufferEXT = NULL; \
		PFN_GL_BIND_FRAME_BUFFER_EXT_PROC							target::glBindFramebufferEXT = NULL; \
		PFN_GL_DELETE_FRAME_BUFFERS_EXT_PROC						target::glDeleteFramebuffersEXT = NULL; \
		PFN_GL_GEN_FRAME_BUFFERS_EXT_PROC							target::glGenFramebuffersEXT = NULL; \
		PFN_GL_CHECK_FRAME_BUFFER_STATUS_EXT_PROC					target::glCheckFramebufferStatusEXT = NULL; \
		PFN_GL_FRAME_BUFFER_TEXTURE_1D_EXT_PROC						target::glFramebufferTexture1DEXT = NULL; \
		PFN_GL_FRAME_BUFFER_TEXTURE_2D_EXT_PROC						target::glFramebufferTexture2DEXT = NULL; \
		PFN_GL_FRAME_BUFFER_TEXTURE_3D_EXT_PROC						target::glFramebufferTexture3DEXT = NULL; \
		PFN_GL_FRAME_BUFFER_RENDER_BUFFER_EXT_PROC					target::glFramebufferRenderbufferEXT = NULL; \
		PFN_GL_GET_FRAME_BUFFER_ATTACHMENT_PARAMETER_IV_EXT_PROC	target::glGetFramebufferAttachmentParameterivEXT = NULL; \
		PFN_GL_GENERATE_MIPMAP_EXT_PROC								target::glGenerateMipmapEXT = NULL;
    #endif
#else
    #define IMPLEMENT_RAPTOR_DECLARE_GL_EXT_framebuffer_object(target)
#endif

#if defined(GL_EXT_framebuffer_blit)
    #ifndef IMPLEMENT_RAPTOR_GL_EXT_framebuffer_blit
    #define IMPLEMENT_RAPTOR_GL_EXT_framebuffer_blit(target) \
        PFN_GL_BLIT_FRAME_BUFFER_EXT_PROC	target::glBlitFrameBufferEXT = NULL;
    #endif
#else
    #define IMPLEMENT_RAPTOR_GL_EXT_framebuffer_blit(target)
#endif

#if defined(GL_EXT_framebuffer_multisample)
    #ifndef IMPLEMENT_RAPTOR_GL_EXT_framebuffer_multisample
    #define IMPLEMENT_RAPTOR_GL_EXT_framebuffer_multisample(target) \
        PFN_GL_RENDER_BUFFER_STORAGE_MULTISAMPLE_EXT_PROC	target::glRenderbufferStorageMultisampleEXT = NULL;
    #endif
#else
    #define IMPLEMENT_RAPTOR_GL_EXT_framebuffer_multisample(target)
#endif


#if defined(WGL_ARB_create_context)
	#ifndef IMPLEMENT_RAPTOR_WGL_ARB_create_context
	#define IMPLEMENT_RAPTOR_WGL_ARB_create_context(target) \
			PFN_WGL_CREATE_CONTEXT_ATTRIBS_PROC	target::wglCreateContextAttribsARB = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_WGL_ARB_create_context(target)
#endif




//!
//!
//!     Macro definition for extensions initialisation
//!
//!

#ifndef IMPLEMENT_GL_EXT_point_parameters
	#ifdef GL_EXT_point_parameters
		#define IMPLEMENT_GL_EXT_point_parameters(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_POINT_PARAMETERS_EXTENSION_NAME))\
		{\
			target->glPointParameterfEXT = (PFN_GL_POINT_PARAMETER_F_EXT_PROC)GET_PROC_ADDRESS("glPointParameterfEXT");\
			target->glPointParameterfvEXT = (PFN_GL_POINT_PARAMETER_FV_EXT_PROC)GET_PROC_ADDRESS("glPointParameterfvEXT");\
		}\
		else\
		{\
			target->glPointParameterfEXT = NULL;\
			target->glPointParameterfvEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_point_parameters(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_point_parameters
	#ifdef GL_ARB_point_parameters
		#define IMPLEMENT_GL_ARB_point_parameters(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_POINT_PARAMETERS_EXTENSION_NAME))\
		{\
			target->glPointParameterfARB = (PFN_GL_POINT_PARAMETER_F_ARB_PROC)GET_PROC_ADDRESS("glPointParameterfARB");\
			target->glPointParameterfvARB = (PFN_GL_POINT_PARAMETER_FV_ARB_PROC)GET_PROC_ADDRESS("glPointParameterfvARB");\
		}\
		else\
		{\
			target->glPointParameterfARB = NULL;\
			target->glPointParameterfvARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_point_parameters(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_multitexture
	#ifdef GL_ARB_multitexture
		#define IMPLEMENT_GL_ARB_multitexture(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_MULTITEXTURE_EXTENSION_NAME))\
		{\
			target->glActiveTextureARB = (PFN_GL_ACTIVE_TEXTURE_ARB_PROC)GET_PROC_ADDRESS("glActiveTextureARB");\
			target->glClientActiveTextureARB = (PFN_GL_CLIENT_ACTIVE_TEXTURE_ARB_PROC)GET_PROC_ADDRESS("glClientActiveTextureARB");\
			target->glMultiTexCoord1dARB = (PFN_GL_MULTI_TEX_COORD_1D_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord1dARB");\
			target->glMultiTexCoord1dvARB = (PFN_GL_MULTI_TEX_COORD_1DV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord1dvARB");\
			target->glMultiTexCoord1fARB = (PFN_GL_MULTI_TEX_COORD_1F_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord1fARB");\
			target->glMultiTexCoord1fvARB = (PFN_GL_MULTI_TEX_COORD_1FV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord1fvARB");\
			target->glMultiTexCoord2dARB = (PFN_GL_MULTI_TEX_COORD_2D_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord2dARB");\
			target->glMultiTexCoord2fARB = (PFN_GL_MULTI_TEX_COORD_2F_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord2fARB");\
			target->glMultiTexCoord2dvARB = (PFN_GL_MULTI_TEX_COORD_2DV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord2dvARB");\
			target->glMultiTexCoord2fvARB = (PFN_GL_MULTI_TEX_COORD_2FV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord2fvARB");\
			target->glMultiTexCoord3dARB = (PFN_GL_MULTI_TEX_COORD_3D_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord3dARB");\
			target->glMultiTexCoord3fARB = (PFN_GL_MULTI_TEX_COORD_3F_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord3fARB");\
			target->glMultiTexCoord3dvARB = (PFN_GL_MULTI_TEX_COORD_3DV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord3dvARB");\
			target->glMultiTexCoord3fvARB = (PFN_GL_MULTI_TEX_COORD_3FV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord3fvARB");\
			target->glMultiTexCoord4dARB = (PFN_GL_MULTI_TEX_COORD_4D_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord4dARB");\
			target->glMultiTexCoord4fARB = (PFN_GL_MULTI_TEX_COORD_4F_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord4fARB");\
			target->glMultiTexCoord4dvARB = (PFN_GL_MULTI_TEX_COORD_4DV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord4dvARB");\
			target->glMultiTexCoord4fvARB = (PFN_GL_MULTI_TEX_COORD_4FV_ARB_PROC)GET_PROC_ADDRESS("glMultiTexCoord4fvARB");\
		} \
		else\
		{ \
			target->glActiveTextureARB = NULL;\
			target->glClientActiveTextureARB = NULL;\
			target->glMultiTexCoord1dARB = NULL;\
			target->glMultiTexCoord1dvARB = NULL;\
			target->glMultiTexCoord1fARB = NULL;\
			target->glMultiTexCoord1fvARB = NULL;\
			target->glMultiTexCoord2dARB = NULL;\
			target->glMultiTexCoord2fARB = NULL;\
			target->glMultiTexCoord2dvARB = NULL;\
			target->glMultiTexCoord2fvARB = NULL;\
			target->glMultiTexCoord3dARB = NULL;\
			target->glMultiTexCoord3fARB = NULL;\
			target->glMultiTexCoord3dvARB = NULL;\
			target->glMultiTexCoord3fvARB = NULL;\
			target->glMultiTexCoord4dARB = NULL;\
			target->glMultiTexCoord4fARB = NULL;\
			target->glMultiTexCoord4dvARB = NULL;\
			target->glMultiTexCoord4fvARB = NULL;\
		} 
	#else
		#define IMPLEMENT_GL_ARB_multitexture(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_EXT_secondary_color
	#ifdef GL_EXT_secondary_color
		#define IMPLEMENT_GL_EXT_secondary_color(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_SECONDARY_COLOR_EXTENSION_NAME))\
		{\
			target->glSecondaryColor3fEXT = (PFN_GL_SECONDARY_COLOR_3F_EXT_PROC)GET_PROC_ADDRESS("glSecondaryColor3fEXT");\
			target->glSecondaryColor3fvEXT = (PFN_GL_SECONDARY_COLOR_3FV_EXT_PROC)GET_PROC_ADDRESS("glSecondaryColor3fvEXT");\
			target->glSecondaryColor3dEXT = (PFN_GL_SECONDARY_COLOR_3D_EXT_PROC)GET_PROC_ADDRESS("glSecondaryColor3dEXT");\
			target->glSecondaryColor3dvEXT = (PFN_GL_SECONDARY_COLOR_3DV_EXT_PROC)GET_PROC_ADDRESS("glSecondaryColor3dvEXT");\
			target->glSecondaryColorPointerEXT = (PFN_GL_SECONDARY_COLOR_POINTER_EXT_PROC)GET_PROC_ADDRESS("glSecondaryColorPointerEXT");\
		}\
		else\
		{\
			target->glSecondaryColor3fEXT = NULL;\
			target->glSecondaryColor3fvEXT = NULL;\
			target->glSecondaryColor3dEXT = NULL;\
			target->glSecondaryColor3dvEXT = NULL;\
			target->glSecondaryColorPointerEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_secondary_color(target)
	#endif
#endif

#ifndef IMPLEMENT_WGL_EXT_swap_control
	#ifdef WGL_EXT_swap_control
		#define IMPLEMENT_WGL_EXT_swap_control(target)\
		if (IRaptor::glIsExtensionSupported(WGL_EXT_SWAP_CONTROL_EXTENSION_NAME))\
		{\
			target->wglSwapIntervalEXT = (PFN_WGL_SWAP_INTERVAL_PROC)GET_PROC_ADDRESS("wglSwapIntervalEXT");\
			target->wglGetSwapIntervalEXT = (PFN_WGL_GET_SWAP_INTERVAL_PROC)GET_PROC_ADDRESS("wglGetSwapIntervalEXT");\
		}\
		else\
		{\
			target->wglSwapIntervalEXT = NULL;\
			target->wglGetSwapIntervalEXT = NULL;\
		}
	#else
		#define IMPLEMENT_WGL_EXT_swap_control(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_transpose_matrix
	#ifdef GL_ARB_transpose_matrix
		#define IMPLEMENT_GL_ARB_transpose_matrix(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_TRANSPOSE_MATRIX_EXTENSION_NAME))\
		{\
			target->glLoadTransposeMatrixfARB = (PFN_GL_LOAD_TRANSPOSE_MATRIX_F_ARB_PROC)GET_PROC_ADDRESS("glLoadTransposeMatrixfARB");\
			target->glLoadTransposeMatrixdARB = (PFN_GL_LOAD_TRANSPOSE_MATRIX_D_ARB_PROC)GET_PROC_ADDRESS("glLoadTransposeMatrixdARB");\
			target->glMultTransposeMatrixfARB = (PFN_GL_MULT_TRANSPOSE_MATRIX_F_ARB_PROC)GET_PROC_ADDRESS("glMultTransposeMatrixfARB");\
			target->glMultTransposeMatrixdARB = (PFN_GL_MULT_TRANSPOSE_MATRIX_D_ARB_PROC)GET_PROC_ADDRESS("glMultTransposeMatrixdARB");\
		}\
		else\
		{\
			target->glLoadTransposeMatrixfARB = NULL;\
			target->glLoadTransposeMatrixdARB = NULL;\
			target->glMultTransposeMatrixfARB = NULL;\
			target->glMultTransposeMatrixdARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_transpose_matrix(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_EXT_vertex_weighting
	#ifdef GL_EXT_vertex_weighting
		#define IMPLEMENT_GL_EXT_vertex_weighting(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_VERTEX_WEIGHTING_EXTENSION_NAME))\
		{\
			target->glVertexWeightfEXT = (PFN_GL_VERTEX_WEIGHT_F_EXT_PROC)GET_PROC_ADDRESS("glVertexWeightfEXT");\
			target->glVertexWeightfvEXT = (PFN_GL_VERTEX_WEIGHT_FV_EXT_PROC)GET_PROC_ADDRESS("glVertexWeightfvEXT");\
			target->glVertexWeightPointerEXT = (PFN_GL_VERTEX_WEIGHT_POINTER_EXT_PROC)GET_PROC_ADDRESS("glVertexWeightPointerEXT");\
		}\
		else\
		{\
			target->glVertexWeightfEXT = NULL;\
			target->glVertexWeightfvEXT = NULL;\
			target->glVertexWeightPointerEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_vertex_weighting(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_texture_compression
	#ifdef GL_ARB_texture_compression
		#define IMPLEMENT_GL_ARB_texture_compression(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_TEXTURE_COMPRESSION_EXTENSION_NAME))\
		{\
			target->glCompressedTexImage3DARB = (PFN_GL_COMPRESSED_TEX_IMAGE_3D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexImage3DARB"); \
			target->glCompressedTexImage2DARB = (PFN_GL_COMPRESSED_TEX_IMAGE_2D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexImage2DARB"); \
			target->glCompressedTexImage1DARB = (PFN_GL_COMPRESSED_TEX_IMAGE_1D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexImage1DARB"); \
			target->glCompressedTexSubImage3DARB = (PFN_GL_COMPRESSED_TEX_SUB_IMAGE_3D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexSubImage3DARB"); \
			target->glCompressedTexSubImage2DARB = (PFN_GL_COMPRESSED_TEX_SUB_IMAGE_2D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexSubImage2DARB"); \
			target->glCompressedTexSubImage1DARB = (PFN_GL_COMPRESSED_TEX_SUB_IMAGE_1D_ARB_PROC)GET_PROC_ADDRESS("glCompressedTexSubImage1DARB"); \
			target->glGetCompressedTexImageARB = (PFN_GL_GET_COMPRESSED_TEX_IMAGE_ARB_PROC)GET_PROC_ADDRESS("glGetCompressedTexImageARB"); \
		}\
		else\
		{\
			target->glCompressedTexImage3DARB = NULL; \
			target->glCompressedTexImage2DARB = NULL; \
			target->glCompressedTexImage1DARB = NULL; \
			target->glCompressedTexSubImage3DARB = NULL; \
			target->glCompressedTexSubImage2DARB = NULL; \
			target->glCompressedTexSubImage1DARB = NULL; \
			target->glGetCompressedTexImageARB = NULL; \
		}
	#else
		#define IMPLEMENT_GL_ARB_texture_compression(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_compiled_vertex_array
	#ifdef GL_EXT_compiled_vertex_array
		#define IMPLEMENT_GL_EXT_compiled_vertex_array(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_COMPILED_VERTEX_ARRAY_EXTENSION_NAME))\
		{\
			target->glLockArraysEXT = (PFN_GL_LOCK_ARRAYS_EXT_PROC)GET_PROC_ADDRESS("glLockArraysEXT");\
			target->glUnlockArraysEXT = (PFN_GL_UNLOCK_ARRAYS_EXT_PROC)GET_PROC_ADDRESS("glUnlockArraysEXT");\
		}\
		else\
		{\
			target->glLockArraysEXT = NULL;\
			target->glUnlockArraysEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_compiled_vertex_array(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_imaging
	#ifdef GL_ARB_imaging
		#define IMPLEMENT_GL_ARB_imaging(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_IMAGING_EXTENSION_NAME))\
		{\
			target->glBlendColorARB = (PFN_GL_BLEND_COLOR_ARB_PROC)GET_PROC_ADDRESS("glBlendColor");\
			target->glBlendEquationARB = (PFN_GL_BLEND_EQUATION_ARB_PROC)GET_PROC_ADDRESS("glBlendEquation");\
		}\
		else\
		{\
			target->glBlendColorARB = NULL;\
			target->glBlendEquationARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_imaging(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_NV_register_combiners
	#ifdef GL_NV_register_combiners
	#define IMPLEMENT_GL_NV_register_combiners(target) \
		if (IRaptor::glIsExtensionSupported(GL_NV_REGISTER_COMBINERS_EXTENSION_NAME))\
		{\
			target->glCombinerParameterfvNV = (PFN_GL_COMBINER_PARAMETER_FV_NV_PROC)GET_PROC_ADDRESS("glCombinerParameterfvNV"); \
			target->glCombinerParameterivNV  = (PFN_GL_COMBINER_PARAMETER_IV_NV_PROC)GET_PROC_ADDRESS("glCombinerParameterivNV"); \
			target->glCombinerParameterfNV = (PFN_GL_COMBINER_PARAMETER_F_NV_PROC)GET_PROC_ADDRESS("glCombinerParameterfNV"); \
			target->glCombinerParameteriNV = (PFN_GL_COMBINER_PARAMETER_I_NV_PROC)GET_PROC_ADDRESS("glCombinerParameteriNV"); \
			target->glCombinerInputNV = (PFN_GL_COMBINER_INPUT_NV_PROC)GET_PROC_ADDRESS("glCombinerInputNV"); \
			target->glCombinerOutputNV = (PFN_GL_COMBINER_OUTPUT_NV_PROC)GET_PROC_ADDRESS("glCombinerOutputNV"); \
			target->glFinalCombinerInputNV = (PFN_GL_FINAL_COMBINER_INPUT_NV)GET_PROC_ADDRESS("glFinalCombinerInputNV"); \
			target->glGetCombinerInputParameterfvNV = (PFN_GL_GET_COMBINER_INPUT_PARAMETER_FV_NV_PROC)GET_PROC_ADDRESS("glGetCombinerInputParameterfvNV"); \
			target->glGetCombinerInputParameterivNV = (PFN_GL_GET_COMBINER_INPUT_PARAMETER_IV_NV_PROC)GET_PROC_ADDRESS("glGetCombinerInputParameterivNV"); \
			target->glGetCombinerOutputParameterfvNV = (PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_FV_NV_PROC)GET_PROC_ADDRESS("glGetCombinerOutputParameterfvNV"); \
			target->glGetCombinerOutputParameterivNV = (PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_IV_NV_PROC)GET_PROC_ADDRESS("glGetCombinerOutputParameterivNV"); \
			target->glGetFinalCombinerInputParameterfvNV = (PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_FV_NV_PROC)GET_PROC_ADDRESS("glGetFinalCombinerInputParameterfvNV"); \
			target->glGetFinalCombinerInputParameterivNV = (PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_IV_NV_PROC)GET_PROC_ADDRESS("glGetFinalCombinerInputParameterivNV"); \
		}\
		else\
		{\
			target->glCombinerParameterfvNV = NULL; \
			target->glCombinerParameterivNV  = NULL; \
			target->glCombinerParameterfNV = NULL; \
			target->glCombinerParameteriNV = NULL; \
			target->glCombinerInputNV = NULL; \
			target->glCombinerOutputNV = NULL; \
			target->glFinalCombinerInputNV = NULL; \
			target->glGetCombinerInputParameterfvNV = NULL; \
			target->glGetCombinerInputParameterivNV = NULL; \
			target->glGetCombinerOutputParameterfvNV = NULL; \
			target->glGetCombinerOutputParameterivNV = NULL; \
			target->glGetFinalCombinerInputParameterfvNV = NULL; \
			target->glGetFinalCombinerInputParameterivNV = NULL; \
		}
	#else
		#define IMPLEMENT_GL_NV_register_combiners(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_fog_coord
	#ifdef GL_EXT_fog_coord
	#define IMPLEMENT_GL_EXT_fog_coord(target) \
		if (IRaptor::glIsExtensionSupported(GL_EXT_FOG_COORD_EXTENSION_NAME))\
		{\
			target->glFogCoordfEXT = (PFN_GL_FOG_COORD_F_EXT_PROC)GET_PROC_ADDRESS("glFogCoordfEXT"); \
			target->glFogCoorddEXT = (PFN_GL_FOG_COORD_D_EXT_PROC)GET_PROC_ADDRESS("glFogCoorddEXT"); \
			target->glFogCoordfvEXT = (PFN_GL_FOG_COORD_FV_EXT_PROC)GET_PROC_ADDRESS("glFogCoordfvEXT"); \
			target->glFogCoorddvEXT = (PFN_GL_FOG_COORD_DV_EXT_PROC)GET_PROC_ADDRESS("glFogCoorddvEXT"); \
			target->glFogCoordPointerEXT = (PFN_GL_FOG_COORD_POINTER_EXT_PROC)GET_PROC_ADDRESS("glFogCoordPointerEXT"); \
		}\
		else\
		{\
			target->glFogCoordfEXT = NULL; \
			target->glFogCoorddEXT = NULL; \
			target->glFogCoordfvEXT = NULL; \
			target->glFogCoorddvEXT = NULL; \
			target->glFogCoordPointerEXT = NULL; \
		}
	#else
		#define IMPLEMENT_GL_EXT_fog_coord(target)
	#endif
#endif
	
#ifndef IMPLEMENT_GL_NV_vertex_array_range
	#ifdef GL_NV_vertex_array_range
	#define IMPLEMENT_GL_NV_vertex_array_range(target) \
		if (IRaptor::glIsExtensionSupported(GL_NV_VERTEX_ARRAY_RANGE_EXTENSION_NAME))\
		{\
			target->glVertexArrayRangeNV = (PFN_GL_VERTEX_ARRAY_RANGE_NV_PROC)GET_PROC_ADDRESS("glVertexArrayRangeNV");\
			target->glFlushVertexArrayRangeNV = (PFN_GL_FLUSH_VERTEX_ARRAY_RANGE_NV_PROC)GET_PROC_ADDRESS("glFlushVertexArrayRangeNV");\
			target->wglAllocateMemoryNV = (PFN_WGL_ALLOCATE_MEMORY_NV_PROC)GET_PROC_ADDRESS("wglAllocateMemoryNV");\
			target->wglFreeMemoryNV = (PFN_WGL_FREE_MEMORY_NV_PROC)GET_PROC_ADDRESS("wglFreeMemoryNV");\
		}\
		else\
		{\
			target->glVertexArrayRangeNV = NULL;\
			target->glFlushVertexArrayRangeNV = NULL;\
			target->wglAllocateMemoryNV = NULL;\
			target->wglFreeMemoryNV = NULL;\
		}
	#else
		#define IMPLEMENT_GL_NV_vertex_array_range(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_vertex_buffer_object
	#ifdef GL_ARB_vertex_buffer_object
	#define IMPLEMENT_GL_ARB_vertex_buffer_object(target) \
		if (IRaptor::glIsExtensionSupported(GL_ARB_VERTEX_BUFFER_OBJECT_EXTENSION_NAME))\
		{\
			target->glBindBufferARB = (PFN_BIND_BUFFER_ARB_PROC)GET_PROC_ADDRESS("glBindBufferARB");\
			target->glDeleteBuffersARB = (PFN_DELETE_BUFFERS_ARB_PROC)GET_PROC_ADDRESS("glDeleteBuffersARB");\
			target->glGenBuffersARB = (PFN_GEN_BUFFERS_ARB_PROC)GET_PROC_ADDRESS("glGenBuffersARB");\
			target->glIsBufferARB = (PFN_IS_BUFFERS_ARB_PROC)GET_PROC_ADDRESS("glIsBufferARB"); \
			target->glBufferDataARB = (PFN_BUFFER_DATA_ARB_PROC)GET_PROC_ADDRESS("glBufferDataARB"); \
			target->glBufferSubDataARB = (PFN_BUFFER_SUB_DATA_ARB_PROC)GET_PROC_ADDRESS("glBufferSubDataARB"); \
			target->glGetBufferSubDataARB = (PFN_GET_BUFFER_SUB_DATA_ARB_PROC)GET_PROC_ADDRESS("glGetBufferSubDataARB"); \
			target->glMapBufferARB = (PFN_MAP_BUFFER_ARB_PROC)GET_PROC_ADDRESS("glMapBufferARB"); \
			target->glUnmapBufferARB = (PFN_UNMAP_BUFFER_ARB_PROC)GET_PROC_ADDRESS("glUnmapBufferARB"); \
			target->glGetBufferParameterivARB = (PFN_GET_BUFFER_PARAMETER_IV_ARB_PROC)GET_PROC_ADDRESS("glGetBufferParameterivARB"); \
			target->glGetBufferPointervARB = (PFN_GET_BUFFER_POINTER_V_ARB_PROC)GET_PROC_ADDRESS("glGetBufferPointervARB");\
		}\
		else\
		{\
			target->glBindBufferARB = NULL;\
			target->glDeleteBuffersARB = NULL;\
			target->glGenBuffersARB = NULL;\
			target->glIsBufferARB = NULL; \
			target->glBufferDataARB = NULL; \
			target->glBufferSubDataARB = NULL; \
			target->glGetBufferSubDataARB = NULL; \
			target->glMapBufferARB = NULL; \
			target->glUnmapBufferARB = NULL; \
			target->glGetBufferParameterivARB = NULL; \
			target->glGetBufferPointervARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_vertex_buffer_object(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_NV_point_sprite
	#ifdef GL_NV_point_sprite
		#define IMPLEMENT_GL_NV_point_sprite(target)\
		if (IRaptor::glIsExtensionSupported(GL_NV_POINT_SPRITE_EXTENSION_NAME))\
		{\
			target->glPointParameteriNV = (PFN_GL_POINT_PARAMETER_I_NV_PROC)GET_PROC_ADDRESS("glPointParameteriNV");\
			target->glPointParameterivNV = (PFN_GL_POINT_PARAMETER_IV_NV_PROC)GET_PROC_ADDRESS("glPointParameterivNV");\
		}\
		else\
		{\
			target->glPointParameteriNV = NULL;\
			target->glPointParameterivNV = NULL;\
		}
	#else
		#define IMPLEMENT_GL_NV_point_sprite(target)
	#endif
#endif

#ifndef IMPLEMENT_WGL_ARB_pbuffer
	#ifdef WGL_ARB_pbuffer
		#define IMPLEMENT_WGL_ARB_pbuffer(target) \
		if (IRaptor::glIsExtensionSupported(WGL_ARB_PBUFFER_EXTENSION_NAME))\
		{\
			target->wglCreatePBufferARB = (PFN_WGL_CREATE_PBUFFER_ARB_PROC)GET_PROC_ADDRESS("wglCreatePbufferARB");\
			target->wglGetPBufferDCARB = (PFN_WGL_GET_PBUFFER_DC_ARB_PROC)GET_PROC_ADDRESS("wglGetPbufferDCARB");\
			target->wglReleasePBufferDCARB = (PFN_WGL_RELEASE_PBUFFER_DC_ARB_PROC)GET_PROC_ADDRESS("wglReleasePbufferDCARB");\
			target->wglDestroyPBufferARB = (PFN_WGL_DESTROY_PBUFFER_ARB_PROC)GET_PROC_ADDRESS("wglDestroyPbufferARB");\
			target->wglQueryPBufferARB = (PFN_WGL_QUERY_PBUFFER_ARB_PROC)GET_PROC_ADDRESS("wglQueryPbufferARB");\
		}\
		else\
		{\
			target->wglCreatePBufferARB = NULL;\
			target->wglGetPBufferDCARB = NULL;\
			target->wglReleasePBufferDCARB = NULL;\
			target->wglDestroyPBufferARB = NULL;\
			target->wglQueryPBufferARB = NULL;\
		}
	#else
		#define IMPLEMENT_WGL_ARB_pbuffer(target)
	#endif
#endif

#ifndef	IMPLEMENT_WGL_ARB_render_texture
	#ifdef WGL_ARB_render_texture
		#define IMPLEMENT_WGL_ARB_render_texture(target)\
		if (IRaptor::glIsExtensionSupported(WGL_ARB_RENDER_TEXTURE_EXTENSION_NAME))\
		{\
			target->wglBindTexImageARB = (PFN_WGL_BIND_TEX_IMAGE_ARB_PROC)GET_PROC_ADDRESS("wglBindTexImageARB");\
			target->wglReleaseTexImageARB = (PFN_WGL_RELEASE_TEX_IMAGE_ARB_PROC)GET_PROC_ADDRESS("wglReleaseTexImageARB");\
			target->wglSetPbufferAttribARB = (PFN_WGL_SET_PBUFFER_ATTRIB_ARB_PROC)GET_PROC_ADDRESS("wglSetPbufferAttribARB");\
		}\
		else\
		{\
			target->wglBindTexImageARB = NULL;\
			target->wglReleaseTexImageARB = NULL;\
			target->wglSetPbufferAttribARB = NULL;\
		}
	#else
		#define IMPLEMENT_WGL_ARB_render_texture(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_ARB_vertex_program
	#ifdef GL_ARB_vertex_program
		#define IMPLEMENT_GL_ARB_vertex_program(target)\
		{\
			if (IRaptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME))\
			{\
				target->glVertexAttrib1sARB = (PFN_VERTEX_ATTRIB_1S_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1sARB");\
				target->glVertexAttrib1fARB = (PFN_VERTEX_ATTRIB_1F_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1fARB");\
				target->glVertexAttrib1dARB = (PFN_VERTEX_ATTRIB_1D_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1dARB");\
				target->glVertexAttrib2sARB = (PFN_VERTEX_ATTRIB_2S_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2sARB");\
				target->glVertexAttrib2fARB = (PFN_VERTEX_ATTRIB_2F_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2fARB");\
				target->glVertexAttrib2dARB = (PFN_VERTEX_ATTRIB_2D_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2dARB");\
				target->glVertexAttrib3sARB = (PFN_VERTEX_ATTRIB_3S_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3sARB");\
				target->glVertexAttrib3fARB = (PFN_VERTEX_ATTRIB_3F_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3fARB");\
				target->glVertexAttrib3dARB = (PFN_VERTEX_ATTRIB_3D_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3dARB");\
				target->glVertexAttrib4sARB = (PFN_VERTEX_ATTRIB_4S_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4sARB");\
				target->glVertexAttrib4fARB = (PFN_VERTEX_ATTRIB_4F_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4fARB");\
				target->glVertexAttrib4dARB = (PFN_VERTEX_ATTRIB_4D_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4dARB");\
				target->glVertexAttrib4NubARB = (PFN_VERTEX_ATTRIB_4NUB_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NubARB");\
				target->glVertexAttrib1svARB = (PFN_VERTEX_ATTRIB_1SV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1svARB");\
				target->glVertexAttrib1fvARB = (PFN_VERTEX_ATTRIB_1FV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1fvARB");\
				target->glVertexAttrib1dvARB = (PFN_VERTEX_ATTRIB_1DV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib1dvARB");\
				target->glVertexAttrib2svARB = (PFN_VERTEX_ATTRIB_2SV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2svARB");\
				target->glVertexAttrib2fvARB = (PFN_VERTEX_ATTRIB_2FV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2fvARB");\
				target->glVertexAttrib2dvARB = (PFN_VERTEX_ATTRIB_2DV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib2dvARB");\
				target->glVertexAttrib3svARB = (PFN_VERTEX_ATTRIB_3SV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3svARB");\
				target->glVertexAttrib3fvARB = (PFN_VERTEX_ATTRIB_3FV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3fvARB");\
				target->glVertexAttrib3dvARB = (PFN_VERTEX_ATTRIB_3DV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib3dvARB");\
				target->glVertexAttrib4bvARB = (PFN_VERTEX_ATTRIB_4BV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4bvARB");\
				target->glVertexAttrib4svARB = (PFN_VERTEX_ATTRIB_4SV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4svARB");\
				target->glVertexAttrib4ivARB = (PFN_VERTEX_ATTRIB_4IV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4ivARB");\
				target->glVertexAttrib4ubvARB = (PFN_VERTEX_ATTRIB_4UBV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4ubvARB");\
				target->glVertexAttrib4usvARB = (PFN_VERTEX_ATTRIB_4USV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4usvARB");\
				target->glVertexAttrib4uivARB = (PFN_VERTEX_ATTRIB_4UIV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4uivARB");\
				target->glVertexAttrib4fvARB = (PFN_VERTEX_ATTRIB_4FV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4fvARB");\
				target->glVertexAttrib4dvARB = (PFN_VERTEX_ATTRIB_4DV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4dvARB");\
				target->glVertexAttrib4NbvARB = (PFN_VERTEX_ATTRIB_4NBV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NbvARB");\
				target->glVertexAttrib4NsvARB = (PFN_VERTEX_ATTRIB_4NSV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NsvARB");\
				target->glVertexAttrib4NivARB = (PFN_VERTEX_ATTRIB_4NIV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NivARB");\
				target->glVertexAttrib4NubvARB = (PFN_VERTEX_ATTRIB_4NUBV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NubvARB");\
				target->glVertexAttrib4NusvARB = (PFN_VERTEX_ATTRIB_4NUSV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NusvARB");\
				target->glVertexAttrib4NuivARB = (PFN_VERTEX_ATTRIB_4NUIV_ARB_PROC)GET_PROC_ADDRESS("glVertexAttrib4NuivARB");\
				target->glVertexAttribPointerARB = (PFN_VERTEX_ATTRIB_POINTER_ARB_PROC)GET_PROC_ADDRESS("glVertexAttribPointerARB");\
				target->glEnableVertexAttribArrayARB = (PFN_ENABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC)GET_PROC_ADDRESS("glEnableVertexAttribArrayARB");\
				target->glDisableVertexAttribArrayARB = (PFN_DISABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC)GET_PROC_ADDRESS("glDisableVertexAttribArrayARB");\
				target->glProgramStringARB = (PFN_PROGRAM_STRING_ARB_PROC)GET_PROC_ADDRESS("glProgramStringARB");\
				target->glBindProgramARB = (PFN_BIND_PROGRAM_ARB_PROC)GET_PROC_ADDRESS("glBindProgramARB");\
				target->glDeleteProgramsARB = (PFN_DELETE_PROGRAMS_ARB_PROC)GET_PROC_ADDRESS("glDeleteProgramsARB");\
				target->glGenProgramsARB = (PFN_GEN_PROGRAMS_ARB_PROC)GET_PROC_ADDRESS("glGenProgramsARB");\
				target->glProgramEnvParameter4fARB = (PFN_PROGRAM_ENV_PARAMETER_4F_ARB_PROC)GET_PROC_ADDRESS("glProgramEnvParameter4fARB");\
				target->glProgramEnvParameter4dARB = (PFN_PROGRAM_ENV_PARAMETER_4D_ARB_PROC)GET_PROC_ADDRESS("glProgramEnvParameter4dARB");\
				target->glProgramEnvParameter4dvARB = (PFN_PROGRAM_ENV_PARAMETER_4DV_ARB_PROC)GET_PROC_ADDRESS("glProgramEnvParameter4dvARB");\
				target->glProgramEnvParameter4fvARB = (PFN_PROGRAM_ENV_PARAMETER_4FV_ARB_PROC)GET_PROC_ADDRESS("glProgramEnvParameter4fvARB");\
				target->glProgramLocalParameter4fARB = (PFN_PROGRAM_LOCAL_PARAMETER_4F_ARB_PROC)GET_PROC_ADDRESS("glProgramLocalParameter4fARB");\
				target->glProgramLocalParameter4dARB = (PFN_PROGRAM_LOCAL_PARAMETER_4D_ARB_PROC)GET_PROC_ADDRESS("glProgramLocalParameter4dARB");\
				target->glProgramLocalParameter4dvARB = (PFN_PROGRAM_LOCAL_PARAMETER_4DV_ARB_PROC)GET_PROC_ADDRESS("glProgramLocalParameter4dvARB");\
				target->glProgramLocalParameter4fvARB = (PFN_PROGRAM_LOCAL_PARAMETER_4FV_ARB_PROC)GET_PROC_ADDRESS("glProgramLocalParameter4fvARB");\
				target->glGetProgramEnvParameterfvARB = (PFN_GET_PROGRAM_ENV_PARAMETER_FV_ARB_PROC)GET_PROC_ADDRESS("glGetProgramEnvParameterfvARB");\
				target->glGetProgramEnvParameterdvARB = (PFN_GET_PROGRAM_ENV_PARAMETER_DV_ARB_PROC)GET_PROC_ADDRESS("glGetProgramEnvParameterdvARB");\
				target->glGetProgramLocalParameterfvARB = (PFN_GET_PROGRAM_LOCAL_PARAMETER_FV_ARB_PROC)GET_PROC_ADDRESS("glGetProgramLocalParameterfvARB");\
				target->glGetProgramLocalParameterdvARB = (PFN_GET_PROGRAM_LOCAL_PARAMETER_DV_ARB_PROC)GET_PROC_ADDRESS("glGetProgramLocalParameterdvARB");\
				target->glGetProgramivARB = (PFN_GET_PROGRAM_IV_ARB_PROC)GET_PROC_ADDRESS("glGetProgramivARB");\
				target->glGetProgramStringARB = (PFN_GET_PROGRAM_STRING_ARB_PROC)GET_PROC_ADDRESS("glGetProgramStringARB");\
				target->glGetVertexAttribdvARB = (PFN_GET_VERTEX_ATTRIB_DV_ARB_PROC)GET_PROC_ADDRESS("glGetVertexAttribdvARB");\
				target->glGetVertexAttribfvARB = (PFN_GET_VERTEX_ATTRIB_FV_ARB_PROC)GET_PROC_ADDRESS("glGetVertexAttribfvARB");\
				target->glGetVertexAttribivARB = (PFN_GET_VERTEX_ATTRIB_IV_ARB_PROC)GET_PROC_ADDRESS("glGetVertexAttribivARB");\
				target->glGetVertexAttribPointervARB = (PFN_GET_VERTEX_ATTRIB_POINTERV_ARB_PROC)GET_PROC_ADDRESS("glGetVertexAttribPointervARB");\
				target->glIsProgramARB = (PFN_IS_PROGRAM_ARB_PROC)GET_PROC_ADDRESS("glIsProgramARB");\
			}\
			else\
			{\
				target->glVertexAttrib1sARB = NULL;\
				target->glVertexAttrib1fARB = NULL;\
				target->glVertexAttrib1dARB = NULL;\
				target->glVertexAttrib2sARB = NULL;\
				target->glVertexAttrib2fARB = NULL;\
				target->glVertexAttrib2dARB = NULL;\
				target->glVertexAttrib3sARB = NULL;\
				target->glVertexAttrib3fARB = NULL;\
				target->glVertexAttrib3dARB = NULL;\
				target->glVertexAttrib4sARB = NULL;\
				target->glVertexAttrib4fARB = NULL;\
				target->glVertexAttrib4dARB = NULL;\
				target->glVertexAttrib4NubARB = NULL;\
				target->glVertexAttrib1svARB = NULL;\
				target->glVertexAttrib1fvARB = NULL;\
				target->glVertexAttrib1dvARB = NULL;\
				target->glVertexAttrib2svARB = NULL;\
				target->glVertexAttrib2fvARB = NULL;\
				target->glVertexAttrib2dvARB = NULL;\
				target->glVertexAttrib3svARB = NULL;\
				target->glVertexAttrib3fvARB = NULL;\
				target->glVertexAttrib3dvARB = NULL;\
				target->glVertexAttrib4bvARB = NULL;\
				target->glVertexAttrib4svARB = NULL;\
				target->glVertexAttrib4ivARB = NULL;\
				target->glVertexAttrib4ubvARB = NULL;\
				target->glVertexAttrib4usvARB = NULL;\
				target->glVertexAttrib4uivARB = NULL;\
				target->glVertexAttrib4fvARB = NULL;\
				target->glVertexAttrib4dvARB = NULL;\
				target->glVertexAttrib4NbvARB = NULL;\
				target->glVertexAttrib4NsvARB = NULL;\
				target->glVertexAttrib4NivARB = NULL;\
				target->glVertexAttrib4NubvARB = NULL;\
				target->glVertexAttrib4NusvARB = NULL;\
				target->glVertexAttrib4NuivARB = NULL;\
				target->glVertexAttribPointerARB = NULL;\
				target->glEnableVertexAttribArrayARB = NULL;\
				target->glDisableVertexAttribArrayARB = NULL;\
				target->glProgramStringARB = NULL;\
				target->glBindProgramARB = NULL;\
				target->glDeleteProgramsARB = NULL;\
				target->glGenProgramsARB = NULL;\
				target->glProgramEnvParameter4fARB = NULL;\
				target->glProgramEnvParameter4dARB = NULL;\
				target->glProgramEnvParameter4dvARB = NULL;\
				target->glProgramEnvParameter4fvARB = NULL;\
				target->glProgramLocalParameter4fARB = NULL;\
				target->glProgramLocalParameter4dARB = NULL;\
				target->glProgramLocalParameter4dvARB = NULL;\
				target->glProgramLocalParameter4fvARB = NULL;\
				target->glGetProgramEnvParameterfvARB = NULL;\
				target->glGetProgramEnvParameterdvARB = NULL;\
				target->glGetProgramLocalParameterfvARB = NULL;\
				target->glGetProgramLocalParameterdvARB = NULL;\
				target->glGetProgramivARB = NULL;\
				target->glGetProgramStringARB = NULL;\
				target->glGetVertexAttribdvARB = NULL;\
				target->glGetVertexAttribfvARB = NULL;\
				target->glGetVertexAttribivARB = NULL;\
				target->glGetVertexAttribPointervARB = NULL;\
				target->glIsProgramARB = NULL;\
			}\
		}
	#else
		#define IMPLEMENT_GL_ARB_vertex_program(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_occlusion_query
	#ifdef GL_ARB_occlusion_query
		#define IMPLEMENT_GL_ARB_occlusion_query(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_OCCLUSION_QUERY_EXTENSION_NAME))\
		{\
			target->glGenQueriesARB = (PFN_GEN_QUERIES_ARB_PROC)GET_PROC_ADDRESS("glGenQueriesARB");\
			target->glDeleteQueriesARB = (PFN_DELETE_QUERIES_ARB_PROC)GET_PROC_ADDRESS("glDeleteQueriesARB");\
			target->glIsQueryARB = (PFN_IS_QUERY_ARB_PROC)GET_PROC_ADDRESS("glIsQueryARB");\
			target->glBeginQueryARB = (PFN_BEGIN_QUERY_ARB_PROC)GET_PROC_ADDRESS("glBeginQueryARB");\
			target->glEndQueryARB = (PFN_END_QUERY_ARB_PROC)GET_PROC_ADDRESS("glEndQueryARB");\
			target->glGetQueryivARB = (PFN_GET_QUERY_IV_ARB_PROC)GET_PROC_ADDRESS("glGetQueryivARB");\
			target->glGetQueryObjectivARB = (PFN_GET_QUERY_OBJECT_IV_ARB_PROC)GET_PROC_ADDRESS("glGetQueryObjectivARB");\
			target->glGetQueryObjectuivARB = (PFN_GET_QUERY_OBJECT_UIV_ARB_PROC)GET_PROC_ADDRESS("glGetQueryObjectuivARB");\
		}\
		else\
		{\
			target->glGenQueriesARB = NULL;\
			target->glDeleteQueriesARB = NULL;\
			target->glIsQueryARB = NULL;\
			target->glBeginQueryARB = NULL;\
			target->glEndQueryARB = NULL;\
			target->glGetQueryivARB = NULL;\
			target->glGetQueryObjectivARB = NULL;\
			target->glGetQueryObjectuivARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_occlusion_query(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_stencil_two_side
	#ifdef GL_EXT_stencil_two_side
		#define IMPLEMENT_GL_EXT_stencil_two_side(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_STENCIL_TWO_SIDE_EXTENSION_NAME))\
		{\
			target->glActiveStencilFaceEXT = (PFN_GL_ACTIVE_STENCIL_FACE_EXT_PROC)GET_PROC_ADDRESS("glActiveStencilFaceEXT");\
		}\
		else\
		{\
			target->glActiveStencilFaceEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_stencil_two_side(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_ARB_color_buffer_float
	#ifdef GL_ARB_color_buffer_float
		#define IMPLEMENT_GL_ARB_color_buffer_float(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_COLOR_BUFFER_FLOAT_EXTENSION_NAME))\
		{\
			target->glClampColorARB = (PFN_CLAMP_COLOR_ARB_PROC)GET_PROC_ADDRESS("glClampColorARB");\
		}\
		else\
		{\
			target->glClampColorARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_color_buffer_float(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_texture3D
	#ifdef GL_EXT_texture3D
		#define IMPLEMENT_GL_EXT_texture3D(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_TEXTURE3D_EXTENSION_NAME))\
		{\
			target->glTexImage3DEXT = (PFN_GL_TEX_IMAGE3D_EXT_PROC)GET_PROC_ADDRESS("glTexImage3DEXT");\
		}\
		else\
		{\
			target->glTexImage3DEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_texture3D(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_shader_objets
    #ifdef GL_ARB_shader_objects
        #define IMPLEMENT_GL_ARB_shader_objects(target) \
        if (IRaptor::glIsExtensionSupported(GL_ARB_SHADER_OBJECTS_EXTENSION_NAME))\
		{\
			target->glDeleteObjectARB = (PFN_DELETE_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glDeleteObjectARB");\
            target->glGetHandleARB = (PFN_GET_HANDLE_ARB_PROC)GET_PROC_ADDRESS("glGetHandleARB");\
            target->glDetachObjectARB = (PFN_DETACH_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glDetachObjectARB");\
            target->glCreateShaderObjectARB = (PFN_CREATE_SHADER_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glCreateShaderObjectARB");\
            target->glShaderSourceARB = (PFN_SHADER_SOURCE_ARB_PROC)GET_PROC_ADDRESS("glShaderSourceARB");\
            target->glCompileShaderARB = (PFN_COMPILE_SHADER_ARB_PROC)GET_PROC_ADDRESS("glCompileShaderARB");\
            target->glCreateProgramObjectARB = (PFN_CREATE_PROGRAM_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glCreateProgramObjectARB");\
            target->glAttachObjectARB = (PFN_ATTACH_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glAttachObjectARB");\
            target->glLinkProgramARB = (PFN_LINK_PROGRAM_ARB_PROC)GET_PROC_ADDRESS("glLinkProgramARB");\
            target->glUseProgramObjectARB = (PFN_USE_PROGRAM_OBJECT_ARB_PROC)GET_PROC_ADDRESS("glUseProgramObjectARB");\
            target->glValidateProgramARB = (PFN_VALIDATE_PROGRAM_ARB_PROC)GET_PROC_ADDRESS("glValidateProgramARB");\
            target->glUniform1fARB = (PFN_UNIFORM_1F_ARB_PROC)GET_PROC_ADDRESS("glUniform1fARB");\
            target->glUniform2fARB = (PFN_UNIFORM_2F_ARB_PROC)GET_PROC_ADDRESS("glUniform2fARB");\
            target->glUniform3fARB = (PFN_UNIFORM_3F_ARB_PROC)GET_PROC_ADDRESS("glUniform3fARB");\
            target->glUniform4fARB = (PFN_UNIFORM_4F_ARB_PROC)GET_PROC_ADDRESS("glUniform4fARB");\
            target->glUniform1iARB = (PFN_UNIFORM_1I_ARB_PROC)GET_PROC_ADDRESS("glUniform1iARB");\
            target->glUniform2iARB = (PFN_UNIFORM_2I_ARB_PROC)GET_PROC_ADDRESS("glUniform2iARB");\
            target->glUniform3iARB = (PFN_UNIFORM_3I_ARB_PROC)GET_PROC_ADDRESS("glUniform3iARB");\
            target->glUniform4iARB = (PFN_UNIFORM_4I_ARB_PROC)GET_PROC_ADDRESS("glUniform4iARB");\
            target->glUniform1fvARB = (PFN_UNIFORM_1FV_ARB_PROC)GET_PROC_ADDRESS("glUniform1fvARB");\
            target->glUniform2fvARB = (PFN_UNIFORM_2FV_ARB_PROC)GET_PROC_ADDRESS("glUniform2fvARB");\
            target->glUniform3fvARB = (PFN_UNIFORM_3FV_ARB_PROC)GET_PROC_ADDRESS("glUniform3fvARB");\
            target->glUniform4fvARB = (PFN_UNIFORM_4FV_ARB_PROC)GET_PROC_ADDRESS("glUniform4fvARB");\
            target->glUniform1ivARB = (PFN_UNIFORM_1IV_ARB_PROC)GET_PROC_ADDRESS("glUniform1ivARB");\
            target->glUniform2ivARB = (PFN_UNIFORM_2IV_ARB_PROC)GET_PROC_ADDRESS("glUniform2ivARB");\
            target->glUniform3ivARB = (PFN_UNIFORM_3IV_ARB_PROC)GET_PROC_ADDRESS("glUniform3ivARB");\
            target->glUniform4ivARB = (PFN_UNIFORM_4IV_ARB_PROC)GET_PROC_ADDRESS("glUniform4ivARB");\
            target->glUniformMatrix2fvARB = (PFN_UNIFORM_MATRIX_2FV_ARB_PROC)GET_PROC_ADDRESS("glUniformMatrix2fvARB");\
            target->glUniformMatrix3fvARB = (PFN_UNIFORM_MATRIX_3FV_ARB_PROC)GET_PROC_ADDRESS("glUniformMatrix3fvARB");\
            target->glUniformMatrix4fvARB = (PFN_UNIFORM_MATRIX_4FV_ARB_PROC)GET_PROC_ADDRESS("glUniformMatrix4fvARB");\
            target->glGetObjectParameterfvARB = (PFN_GET_OBJECT_PARAMETER_FV_ARB_PROC)GET_PROC_ADDRESS("glGetObjectParameterfvARB");\
            target->glGetObjectParameterivARB = (PFN_GET_OBJECT_PARAMETER_IV_ARB_PROC)GET_PROC_ADDRESS("glGetObjectParameterivARB");\
            target->glGetInfoLogARB = (PFN_GET_INFO_LOG_ARB_PROC)GET_PROC_ADDRESS("glGetInfoLogARB");\
            target->glGetAttachedObjectsARB = (PFN_GET_ATTACHED_OBJECTS_ARB_PROC)GET_PROC_ADDRESS("glGetAttachedObjectsARB");\
            target->glGetUniformLocationARB = (PFN_GET_UNIFORM_LOCATION_ARB_PROC)GET_PROC_ADDRESS("glGetUniformLocationARB");\
            target->glGetActiveUniformARB = (PFN_GET_ACTIVE_UNIFORM_ARB_PROC)GET_PROC_ADDRESS("glGetActiveUniformARB");\
            target->glGetUniformfvARB = (PFN_GET_UNIFORM_FV_ARB_PROC)GET_PROC_ADDRESS("glGetUniformfvARB");\
            target->glGetUniformivARB = (PFN_GET_UNIFORM_IV_ARB_PROC)GET_PROC_ADDRESS("glGetUniformivARB");\
            target->glGetShaderSourceARB = (PFN_GET_SHADER_SOURCE_ARB_PROC)GET_PROC_ADDRESS("glGetShaderSourceARB");\
        }\
		else\
		{\
			target->glDeleteObjectARB = NULL;\
            target->glGetHandleARB = NULL;\
            target->glDetachObjectARB = NULL;\
            target->glCreateShaderObjectARB = NULL;\
            target->glShaderSourceARB = NULL;\
            target->glCompileShaderARB = NULL;\
            target->glCreateProgramObjectARB = NULL;\
            target->glAttachObjectARB = NULL;\
            target->glLinkProgramARB = NULL;\
            target->glUseProgramObjectARB = NULL;\
            target->glValidateProgramARB = NULL;\
            target->glUniform1fARB = NULL;\
            target->glUniform2fARB = NULL;\
            target->glUniform3fARB = NULL;\
            target->glUniform4fARB = NULL;\
            target->glUniform1iARB = NULL;\
            target->glUniform2iARB = NULL;\
            target->glUniform3iARB = NULL;\
            target->glUniform4iARB = NULL;\
            target->glUniform1fvARB = NULL;\
            target->glUniform2fvARB = NULL;\
            target->glUniform3fvARB = NULL;\
            target->glUniform4fvARB = NULL;\
            target->glUniform1ivARB = NULL;\
            target->glUniform2ivARB = NULL;\
            target->glUniform3ivARB = NULL;\
            target->glUniform4ivARB = NULL;\
            target->glUniformMatrix2fvARB = NULL;\
            target->glUniformMatrix3fvARB = NULL;\
            target->glUniformMatrix4fvARB = NULL;\
            target->glGetObjectParameterfvARB = NULL;\
            target->glGetObjectParameterivARB = NULL;\
            target->glGetInfoLogARB = NULL;\
            target->glGetAttachedObjectsARB = NULL;\
            target->glGetUniformLocationARB = NULL;\
            target->glGetActiveUniformARB = NULL;\
            target->glGetUniformfvARB = NULL;\
            target->glGetUniformivARB = NULL;\
            target->glGetShaderSourceARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_shader_objects(target)
	#endif        
#endif


#ifndef IMPLEMENT_GL_ARB_vertex_shader
	#ifdef GL_ARB_vertex_shader
		#define IMPLEMENT_GL_ARB_vertex_shader(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_VERTEX_SHADER_EXTENSION_NAME))\
		{\
			target->glBindAttribLocationARB = (PFN_BIND_ATTRIB_LOCATION_ARB_PROC)GET_PROC_ADDRESS("glBindAttribLocationARB");\
            target->glGetActiveAttribARB = (PFN_GET_ACTIVE_ATTRIB_ARB_PROC)GET_PROC_ADDRESS("glGetActiveAttribARB");\
            target->glGetAttribLocationARB = (PFN_GET_ATTRIB_LOCATION_ARB_PROC)GET_PROC_ADDRESS("glGetAttribLocationARB");\
		}\
		else\
		{\
			target->glBindAttribLocationARB = NULL;\
            target->glGetActiveAttribARB = NULL;\
            target->glGetAttribLocationARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_vertex_shader(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_ARB_geometry_shader4
	#ifdef GL_ARB_geometry_shader4
		#define IMPLEMENT_GL_ARB_geometry_shader4(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_GEOMETRY_SHADER4_EXTENSION_NAME))\
		{\
			target->glProgramParameteriARB = (PFN_PROGRAM_PARAMETER_I_ARB_PROC)GET_PROC_ADDRESS("glProgramParameteriARB");\
            target->glFramebufferTextureARB = (PFN_FRAMEBUFFER_TEXTURE_ARB_PROC)GET_PROC_ADDRESS("glFramebufferTextureARB");\
            target->glFramebufferTextureLayerARB = (PFN_FRAMEBUFFER_TEXTURE_LAYER_ARB_PROC)GET_PROC_ADDRESS("glFramebufferTextureLayerARB");\
			target->glFramebufferTextureFaceARB = (PFN_FRAMEBUFFER_TEXTURE_FACE_ARB_PROC)GET_PROC_ADDRESS("glFramebufferTextureFaceARB");\
		}\
		else\
		{\
			target->glProgramParameteriARB = NULL;\
            target->glFramebufferTextureARB = NULL;\
            target->glFramebufferTextureLayerARB = NULL;\
			target->glFramebufferTextureFaceARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_geometry_shader4(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_ARB_draw_buffers
	#ifdef GL_ARB_draw_buffers
		#define IMPLEMENT_GL_ARB_draw_buffers(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_DRAW_BUFFERS_EXTENSION_NAME))\
		{\
			target->glDrawBuffersARB = (PFN_DRAW_BUFFERS_ARB)GET_PROC_ADDRESS("glDrawBuffersARB");\
		}\
		else\
		{\
			target->glDrawBuffersARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_draw_buffers(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_blend_color
	#ifdef GL_EXT_blend_color
		#define IMPLEMENT_GL_EXT_blend_color(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_BLEND_COLOR_EXTENSION_NAME))\
		{\
			target->glBlendColorEXT = (PFN_GL_BLEND_COLOR_EXT_PROC)GET_PROC_ADDRESS("glBlendColorEXT");\
		}\
		else\
		{\
			target->glBlendColorEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_blend_color(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_framebuffer_object
    #ifdef GL_EXT_framebuffer_object
		#define IMPLEMENT_GL_EXT_framebuffer_object(target) \
		if (IRaptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_OBJECT_EXTENSION_NAME))\
		{\
			target->glIsRenderbufferEXT = (PFN_GL_IS_RENDER_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glIsRenderbufferEXT");\
			target->glBindRenderbufferEXT = (PFN_GL_BIND_RENDER_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glBindRenderbufferEXT");\
			target->glDeleteRenderbuffersEXT = (PFN_GL_DELETE_RENDER_BUFFERS_EXT_PROC)GET_PROC_ADDRESS("glDeleteRenderbuffersEXT");\
			target->glGenRenderbuffersEXT = (PFN_GL_GEN_RENDER_BUFFERS_EXT_PROC)GET_PROC_ADDRESS("glGenRenderbuffersEXT");\
			target->glRenderbufferStorageEXT = (PFN_GL_RENDER_BUFFER_STORAGE_EXT_PROC)GET_PROC_ADDRESS("glRenderbufferStorageEXT");\
			target->glGetRenderbufferParameterivEXT = (PFN_GL_GET_RENDER_BUFFER_PARAMETER_IV_EXT_PROC)GET_PROC_ADDRESS("glGetRenderbufferParameterivEXT");\
			target->glIsFramebufferEXT = (PFN_GL_IS_FRAME_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glIsFramebufferEXT");\
			target->glBindFramebufferEXT = (PFN_GL_BIND_FRAME_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glBindFramebufferEXT");\
			target->glDeleteFramebuffersEXT = (PFN_GL_DELETE_FRAME_BUFFERS_EXT_PROC)GET_PROC_ADDRESS("glDeleteFramebuffersEXT");\
			target->glGenFramebuffersEXT = (PFN_GL_GEN_FRAME_BUFFERS_EXT_PROC)GET_PROC_ADDRESS("glGenFramebuffersEXT");\
			target->glCheckFramebufferStatusEXT = (PFN_GL_CHECK_FRAME_BUFFER_STATUS_EXT_PROC)GET_PROC_ADDRESS("glCheckFramebufferStatusEXT");\
			target->glFramebufferTexture1DEXT = (PFN_GL_FRAME_BUFFER_TEXTURE_1D_EXT_PROC)GET_PROC_ADDRESS("glFramebufferTexture1DEXT");\
			target->glFramebufferTexture2DEXT = (PFN_GL_FRAME_BUFFER_TEXTURE_2D_EXT_PROC)GET_PROC_ADDRESS("glFramebufferTexture2DEXT");\
			target->glFramebufferTexture3DEXT = (PFN_GL_FRAME_BUFFER_TEXTURE_3D_EXT_PROC)GET_PROC_ADDRESS("glFramebufferTexture3DEXT");\
			target->glFramebufferRenderbufferEXT = (PFN_GL_FRAME_BUFFER_RENDER_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glFramebufferRenderbufferEXT");\
			target->glGetFramebufferAttachmentParameterivEXT = (PFN_GL_GET_FRAME_BUFFER_ATTACHMENT_PARAMETER_IV_EXT_PROC)GET_PROC_ADDRESS("glGetFramebufferAttachmentParameterivEXT");\
			target->glGenerateMipmapEXT = (PFN_GL_GENERATE_MIPMAP_EXT_PROC)GET_PROC_ADDRESS("glGenerateMipmapEXT");\
		}\
		else\
		{\
			target->glIsRenderbufferEXT = NULL;\
			target->glBindRenderbufferEXT = NULL;\
			target->glDeleteRenderbuffersEXT = NULL;\
			target->glGenRenderbuffersEXT = NULL;\
			target->glRenderbufferStorageEXT = NULL;\
			target->glGetRenderbufferParameterivEXT = NULL;\
			target->glIsFramebufferEXT = NULL;\
			target->glBindFramebufferEXT = NULL;\
			target->glDeleteFramebuffersEXT = NULL;\
			target->glGenFramebuffersEXT = NULL;\
			target->glCheckFramebufferStatusEXT = NULL;\
			target->glFramebufferTexture1DEXT = NULL;\
			target->glFramebufferTexture2DEXT = NULL;\
			target->glFramebufferTexture3DEXT = NULL;\
			target->glFramebufferRenderbufferEXT = NULL;\
			target->glGetFramebufferAttachmentParameterivEXT = NULL;\
			target->glGenerateMipmapEXT = NULL;\
		}
    #endif
#else
    #define IMPLEMENT_RAPTOR_DECLARE_GL_EXT_framebuffer_object(target)
#endif


#ifndef IMPLEMENT_GL_EXT_framebuffer_blit
	#ifdef GL_EXT_framebuffer_blit
		#define IMPLEMENT_GL_EXT_framebuffer_blit(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_BLIT_EXTENSION_NAME))\
		{\
			target->glBlitFrameBufferEXT = (PFN_GL_BLIT_FRAME_BUFFER_EXT_PROC)GET_PROC_ADDRESS("glBlitFramebufferEXT");\
		}\
		else\
		{\
			target->glBlitFrameBufferEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_framebuffer_blit(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_EXT_framebuffer_multisample
	#ifdef GL_EXT_framebuffer_multisample
		#define IMPLEMENT_GL_EXT_framebuffer_multisample(target)\
		if (IRaptor::glIsExtensionSupported(GL_EXT_BLEND_COLOR_EXTENSION_NAME))\
		{\
			target->glRenderbufferStorageMultisampleEXT = (PFN_GL_RENDER_BUFFER_STORAGE_MULTISAMPLE_EXT_PROC)GET_PROC_ADDRESS("glRenderbufferStorageMultisampleEXT");\
		}\
		else\
		{\
			target->glRenderbufferStorageMultisampleEXT = NULL;\
		}
	#else
		#define IMPLEMENT_GL_EXT_framebuffer_multisample(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_uniform_buffer_object
	#ifdef GL_ARB_uniform_buffer_object
	#define IMPLEMENT_GL_ARB_uniform_buffer_object(target)\
		if (IRaptor::glIsExtensionSupported(GL_ARB_UNIFORM_BUFFER_OBJECT_EXTENSION_NAME))\
		{\
			target->glGetUniformIndicesARB = (PFN_GL_GET_UNIFORM_INDICES_ARB_PROC)GET_PROC_ADDRESS("glGetUniformIndices");\
			target->glGetActiveUniformsivARB = (PFN_GL_GET_ACTIVE_UNIFORMS_IV_ARB_PROC)GET_PROC_ADDRESS("glGetActiveUniformsiv");\
			target->glGetActiveUniformNameARB = (PFN_GL_GET_ACTIVE_UNIFORM_NAME_ARB_PROC)GET_PROC_ADDRESS("glGetActiveUniformName");\
			target->glGetUniformBlockIndexARB = (PFN_GL_GET_UNIFORM_BLOCK_INDEX_ARB_PROC)GET_PROC_ADDRESS("glGetUniformBlockIndex");\
			target->glGetActiveUniformBlockivARB = (PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_IV_ARB_PROC)GET_PROC_ADDRESS("glGetActiveUniformBlockiv");\
			target->glGetActiveUniformBlockNameARB = (PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_NAME_ARB_PROC)GET_PROC_ADDRESS("glGetActiveUniformBlockName");\
			target->glBindBufferRangeARB = (PFN_GL_BIND_BUFFER_RANGE_ARB_PROC)GET_PROC_ADDRESS("glBindBufferRange");\
			target->glBindBufferBaseARB = (PFN_GL_BIND_BUFFER_BASE_ARB_PROC)GET_PROC_ADDRESS("glBindBufferBase");\
			target->glGetIntegeri_vARB = (PFN_GL_GET_INTEGERI_V_ARB_PROC)GET_PROC_ADDRESS("glGetIntegeri_v");\
			target->glUniformBlockBindingARB = (PFN_GL_UNIFORM_BLOCK_BINDING_ARB_PROC)GET_PROC_ADDRESS("glUniformBlockBinding");\
		}\
		else\
		{\
			target->glGetUniformIndicesARB = NULL;\
			target->glGetActiveUniformsivARB = NULL;\
			target->glGetActiveUniformNameARB = NULL;\
			target->glGetUniformBlockIndexARB = NULL;\
			target->glGetActiveUniformBlockivARB = NULL;\
			target->glGetActiveUniformBlockNameARB = NULL;\
			target->glBindBufferRangeARB = NULL;\
			target->glBindBufferBaseARB = NULL;\
			target->glGetIntegeri_vARB = NULL;\
			target->glUniformBlockBindingARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_uniform_buffer_object(target)
	#endif
#endif

#ifndef	IMPLEMENT_WGL_ARB_create_context
	#ifdef WGL_ARB_create_context
	#define IMPLEMENT_WGL_ARB_create_context(target)\
		if (IRaptor::glIsExtensionSupported(WGL_ARB_RENDER_TEXTURE_EXTENSION_NAME))\
		{\
			target->wglCreateContextAttribsARB = (PFN_WGL_CREATE_CONTEXT_ATTRIBS_PROC)GET_PROC_ADDRESS("wglCreateContextAttribsARB");\
		}\
		else\
		{\
			target->wglCreateContextAttribsARB = NULL; \
		}
	#else
		#define IMPLEMENT_WGL_ARB_create_context(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_ARB_vertex_array_object
	#ifdef GL_ARB_vertex_array_object
	#define IMPLEMENT_GL_ARB_vertex_array_object(target) \
		if (IRaptor::glIsExtensionSupported(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME))\
		{\
			target->glBindVertexArrayARB = (PFN_GL_BIND_VERTEX_ARRAY_ARB_PROC)GET_PROC_ADDRESS("glBindVertexArray");\
			target->glDeleteVertexArraysARB = (PFN_GL_DELETE_VERTEX_ARRAYS_ARB_PROC)GET_PROC_ADDRESS("glDeleteVertexArrays");\
			target->glGenVertexArraysARB = (PFN_GL_GEN_VERTEX_ARRAYS_ARB_PROC)GET_PROC_ADDRESS("glGenVertexArrays");\
			target->glIsVertexArrayARB = (PFN_GL_IS_VERTEX_ARRAY_ARB_PROC)GET_PROC_ADDRESS("glIsVertexArray");\
		}\
		else\
		{\
			target->glBindVertexArrayARB = NULL;\
			target->glDeleteVertexArraysARB = NULL;\
			target->glGenVertexArraysARB = NULL;\
			target->glIsVertexArrayARB = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_vertex_array_object(target)
	#endif
#endif


#ifndef IMPLEMENT_GL_ARB_debug_output
	#ifdef GL_ARB_debug_output
	#define IMPLEMENT_GL_ARB_debug_output(target) \
		if (IRaptor::glIsExtensionSupported(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME))\
		{\
			target->glDebugMessageControlARB = (PFN_GL_DEBUG_MESSAGE_CONTROL_ARB)GET_PROC_ADDRESS("glDebugMessageControlARB");\
			target->glDebugMessageInsertARB = (PFN_GL_DEBUG_MESSAGE_INSERT_ARB)GET_PROC_ADDRESS("glDebugMessageInsertARB");\
			target->glDebugMessageCallbackARB = (PFN_GL_DEBUG_MESSAGE_CALLBACK_ARB)GET_PROC_ADDRESS("glDebugMessageCallbackARB");\
			target->glGetDebugMessageLogARB = (PFN_GL_GET_DEBUG_MESSAGE_LOG_ARB)GET_PROC_ADDRESS("glGetDebugMessageLogARB");\
			target->glGetPointerv = (PFN_GL_GET_POINTERV)GET_PROC_ADDRESS("glGetPointerv");\
		}\
		else\
		{\
			target->glDebugMessageControlARB = NULL;\
			target->glDebugMessageInsertARB = NULL;\
			target->glDebugMessageCallbackARB = NULL;\
			target->glGetDebugMessageLogARB = NULL;\
			target->glGetPointerv = NULL;\
		}
	#else
		#define IMPLEMENT_GL_ARB_debug_output(target)
	#endif
#endif



#ifndef IMPLEMENT_WGL_ARB_create_context
	#ifdef WGL_ARB_create_context
	#define IMPLEMENT_WGL_ARB_create_context(target)\
			if (IRaptor::glIsExtensionSupported(WGL_ARB_CREATE_CONTEXT_EXTENSION_NAME))\
			{\
				target->wglCreateContextAttribsARB = (PFN_WGL_CREATE_CONTEXT_ATTRIBS_PROC)GET_PROC_ADDRESS("wglCreateContextAttribsARB");\
			}\
			else\
			{\
				target->glTexImage3DEXT = NULL;\
			}
	#else
		#define IMPLEMENT_WGL_ARB_create_context(target)
	#endif
#endif

#ifndef IMPLEMENT_GL_VERSION_3_1
	#ifdef GL_VERSION_3_1
		#define IMPLEMENT_GL_VERSION_3_1(target) \
			target->glDrawArraysInstanced = (PFN_GL_DRAW_ARRAYS_INSTANCED_PROC)GET_PROC_ADDRESS("glDrawArraysInstanced"); \
			target->glDrawElementsInstanced = (PFN_GL_DRAW_ELEMENTS_INSTANCED_PROC)GET_PROC_ADDRESS("glDrawElementsInstanced"); \
			target->glTexBuffer = (PFN_GL_TEX_BUFFER_PROC)GET_PROC_ADDRESS("glTexBuffer"); \
			target->glPrimitiveRestartIndex = (PFN_GL_PRIMITIVERE_START_INDEX_PROC)GET_PROC_ADDRESS(" glPrimitiveRestartIndex"); \
			target->glCopyBufferSubData = (PFN_GL_COPY_BUFFER_SUB_DATA_PROC)GET_PROC_ADDRESS("glCopyBufferSubData"); \
			target->glGetUniformIndices = (PFN_GL_GET_UNIFORM_INDICES_PROC)GET_PROC_ADDRESS("glGetUniformIndices"); \
			target->glGetActiveUniformsiv = (PFN_GL_GET_ACTIVE_UNIFORMS_IV_PROC)GET_PROC_ADDRESS("glGetActiveUniformsiv"); \
			target->glGetActiveUniformName = (PFN_GL_GET_ACTIVE_UNIFORM_NAME_PROC)GET_PROC_ADDRESS("glGetActiveUniformName"); \
			target->glGetUniformBlockIndex = (PFN_GL_GET_UNIFORM_BLOCK_INDEX_PROC)GET_PROC_ADDRESS("glGetUniformBlockIndex"); \
			target->glGetActiveUniformBlockiv = (PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_IV_PROC)GET_PROC_ADDRESS("glGetActiveUniformBlockiv"); \
			target->glGetActiveUniformBlockName = (PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_NAME_PROC)GET_PROC_ADDRESS("glGetActiveUniformBlockName"); \
			target->glUniformBlockBinding = (PFN_GL_UNIFORM_BLOCK_BINDING_PROC)GET_PROC_ADDRESS("glUniformBlockBinding");
	#else
		#define IMPLEMENT_GL_VERSION_3_1(target)\
			target->glDrawArraysInstanced = NULL;\
			target->glDrawElementsInstanced = NULL;\
			target->glTexBuffer = NULL;\
			target->glPrimitiveRestartIndex = NULL;\
			target->glCopyBufferSubData = NULL; \
			target->glGetUniformIndices = NULL; \
			target->glGetActiveUniformsiv = NULL; \
			target->glGetActiveUniformName = NULL; \
			target->glGetUniformBlockIndex = NULL; \
			target->glGetActiveUniformBlockiv = NULL; \
			target->glGetActiveUniformBlockName = NULL; \
			target->glUniformBlockBinding = NULL;
	#endif
#endif

#ifndef IMPLEMENT_GL_VERSION_3_0
	#ifdef GL_VERSION_3_0
		#define IMPLEMENT_GL_VERSION_3_0(target) \
			target->glMapBufferRange = (PFN_GL_MAP_BUFFER_RANGE_PROC)GET_PROC_ADDRESS("glMapBufferRange");
	#else
		#define IMPLEMENT_GL_VERSION_3_0(target)\
			target->glMapBufferRange = NULL;
	#endif
#endif

#ifndef IMPLEMENT_GL_VERSION_2_0
	#ifdef GL_VERSION_2_0
		#define IMPLEMENT_GL_VERSION_2_0(target) \
			target->glCreateProgram = (PFN_GL_CREATE_PROGRAM_PROC)GET_PROC_ADDRESS("glCreateProgram"); \
			target->glDeleteProgram = (PFN_GL_DELETE_PROGRAM_PROC)GET_PROC_ADDRESS("glDeleteProgram"); \
			target->glIsProgram = (PFN_GL_IS_PROGRAM_PROC)GET_PROC_ADDRESS("glIsProgram"); \
			target->glCreateShader = (PFN_GL_CREATE_SHADER_PROC)GET_PROC_ADDRESS("glCreateShader"); \
			target->glDeleteShader = (PFN_GL_DELETE_SHADER_PROC)GET_PROC_ADDRESS("glDeleteShader"); \
			target->glIsShader = (PFN_GL_IS_SHADER_PROC)GET_PROC_ADDRESS("glIsShader"); \
			target->glAttachShader = (PFN_GL_ATTACH_SHADER_PROC)GET_PROC_ADDRESS("glAttachShader"); \
			target->glDetachShader = (PFN_GL_DETACH_SHADER_PROC)GET_PROC_ADDRESS("glDetachShader"); \
			target->glGetAttachedShaders = (PFN_GL_GET_ATTACHED_SHADERS_PROC)GET_PROC_ADDRESS("glGetAttachedShaders"); \
			target->glBindAttribLocation = (PFN_GL_BIND_ATTRIB_LOCATION_PROC)GET_PROC_ADDRESS("glBindAttribLocation"); \
			target->glLinkProgram = (PFN_GL_LINK_PROGRAM_PROC)GET_PROC_ADDRESS("glLinkProgram"); \
			target->glValidateProgram = (PFN_GL_VALIDATE_PROGRAM_PROC)GET_PROC_ADDRESS("glValidateProgram"); \
			target->glGetProgramiv = (PFN_GL_GET_PROGRAM_IV_PROC)GET_PROC_ADDRESS("glGetProgramiv"); \
			target->glGetProgramInfoLog = (PFN_GL_GET_PROGRAM_INFO_LOG_PROC)GET_PROC_ADDRESS("glGetProgramInfoLog"); \
			target->glUseProgram = (PFN_GL_USE_PROGRAM_PROC)GET_PROC_ADDRESS("glUseProgram"); \
			target->glCompileShader = (PFN_GL_COMPILE_SHADER_PROC)GET_PROC_ADDRESS("glCompileShader"); \
			target->glShaderSource = (PFN_GL_SHADER_SOURCE_PROC)GET_PROC_ADDRESS("glShaderSource"); \
			target->glGetShaderiv = (PFN_GL_GET_SHADER_IV_PROC)GET_PROC_ADDRESS("glGetShaderiv"); \
			target->glGetShaderInfoLog = (PFN_GL_GET_SHADER_INFO_LOG_PROC)GET_PROC_ADDRESS("glGetShaderInfoLog"); \
			target->glGetShaderSource = (PFN_GL_GET_SHADER_SOURCE_PROC)GET_PROC_ADDRESS("glGetShaderSource"); \
			target->glUniform1i = (PFN_GL_UNIFORM_1I_PROC)GET_PROC_ADDRESS("glUniform1i"); \
			target->glUniform2i = (PFN_GL_UNIFORM_2I_PROC)GET_PROC_ADDRESS("glUniform2i"); \
			target->glUniform3i = (PFN_GL_UNIFORM_3I_PROC)GET_PROC_ADDRESS("glUniform3i"); \
			target->glUniform4i = (PFN_GL_UNIFORM_4I_PROC)GET_PROC_ADDRESS("glUniform4i"); \
			target->glUniform1f = (PFN_GL_UNIFORM_1F_PROC)GET_PROC_ADDRESS("glUniform1f"); \
			target->glUniform2f = (PFN_GL_UNIFORM_2F_PROC)GET_PROC_ADDRESS("glUniform2f"); \
			target->glUniform3f = (PFN_GL_UNIFORM_3F_PROC)GET_PROC_ADDRESS("glUniform3f"); \
			target->glUniform4f = (PFN_GL_UNIFORM_4F_PROC)GET_PROC_ADDRESS("glUniform4f"); \
			target->glUniform1iv = (PFN_GL_UNIFORM_1IV_PROC)GET_PROC_ADDRESS("glUniform1iv"); \
			target->glUniform2iv = (PFN_GL_UNIFORM_1IV_PROC)GET_PROC_ADDRESS("glUniform2iv"); \
			target->glUniform3iv = (PFN_GL_UNIFORM_1IV_PROC)GET_PROC_ADDRESS("glUniform3iv"); \
			target->glUniform4iv = (PFN_GL_UNIFORM_1IV_PROC)GET_PROC_ADDRESS("glUniform4iv"); \
			target->glUniform1fv = (PFN_GL_UNIFORM_1FV_PROC)GET_PROC_ADDRESS("glUniform1fv"); \
			target->glUniform2fv = (PFN_GL_UNIFORM_2FV_PROC)GET_PROC_ADDRESS("glUniform2fv"); \
			target->glUniform3fv = (PFN_GL_UNIFORM_3FV_PROC)GET_PROC_ADDRESS("glUniform3fv"); \
			target->glUniform4fv = (PFN_GL_UNIFORM_4FV_PROC)GET_PROC_ADDRESS("glUniform4fv"); \
			target->glGetActiveAttrib = (PFN_GL_GET_ACTIVE_ATTRIB_PROC)GET_PROC_ADDRESS("glGetActiveAttrib"); \
			target->glGetActiveUniform = (PFN_GL_GET_ACTIVE_UNIFORM_PROC)GET_PROC_ADDRESS("glGetActiveUniform"); \
			target->glGetAttribLocation = (PFN_GL_GET_ATTRIB_LOCATION_PROC)GET_PROC_ADDRESS("glGetAttribLocation"); \
			target->glGetUniformLocation = (PFN_GL_GET_UNIFORM_LOCATION_PROC)GET_PROC_ADDRESS("glGetUniformLocation");
	#else
		#define IMPLEMENT_GL_VERSION_2_0(target)\
			target->glCreateProgram = NULL; \
			target->glDeleteProgram = NULL; \
			target->glIsProgram = NULL; \
			target->glCreateShader = NULL; \
			target->glDeleteShader = NULL; \
			target->glIsShader = NULL; \
			target->glAttachShader = NULL; \
			target->glDetachShader = NULL; \
			target->glGetAttachedShaders = NULL; \
			target->glBindAttribLocation = NULL; \
			target->glLinkProgram = NULL; \
			target->glValidateProgram = NULL; \
			target->glGetProgramiv = NULL; \
			target->glGetProgramInfoLog = NULL; \
			target->glUseProgram = NULL; \
			target->glCompileShader = NULL; \
			target->glShaderSource = NULL; \
			target->glGetShaderiv = NULL; \
			target->glGetShaderInfoLog = NULL; \
			target->glGetShaderSource = NULL; \
			target->glUniform1i = NULL; \
			target->glUniform2i = NULL; \
			target->glUniform3i = NULL; \
			target->glUniform4i = NULL; \
			target->glUniform1f = NULL; \
			target->glUniform2f = NULL; \
			target->glUniform3f = NULL; \
			target->glUniform4f = NULL; \
			target->glUniform1iv = NULL; \
			target->glUniform2iv = NULL; \
			target->glUniform3iv = NULL; \
			target->glUniform4iv = NULL; \
			target->glUniform1fv = NULL; \
			target->glUniform2fv = NULL; \
			target->glUniform3fv = NULL; \
			target->glUniform4fv = NULL; \
			target->glGetActiveAttrib = NULL; \
			target->glGetActiveUniform = NULL; \
			target->glGetAttribLocation = NULL; \
			target->glGetUniformLocation = NULL;
	#endif
#endif

#endif	// __glext_macros_h_

