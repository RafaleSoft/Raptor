/***************************************************************************/
/*                                                                         */
/*  Win32Glext.h    											           */
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

//! This header provides Win32 specific OpenGL extensions.
//! It is included where needed, depending on the content of 
//! the file CodeGeneration.h. User should never include this file 
//! directly because it is managed by Glext.h

#ifndef __WIN32_RAPTOR_GLEXT_H__
#define __WIN32_RAPTOR_GLEXT_H__


#if defined(WGL_ARB_extensions_string)
	typedef const char * (RAPTOR_APICALL * PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC)(HDC hdc);

	#ifndef DECLARE_WGL_ARB_extensions_string
	#define DECLARE_WGL_ARB_extensions_string(LINKAGE) \
		LINKAGE PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC wglGetExtensionsStringARB;
	#endif
#else
	#define DECLARE_WGL_ARB_extensions_string(LINKAGE)
#endif


#if defined(WGL_ARB_pixel_format)
	#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
	#define WGL_DRAW_TO_WINDOW_ARB				0x2001
	#define WGL_DRAW_TO_BITMAP_ARB				0x2002
	#define WGL_ACCELERATION_ARB				0x2003
	#define WGL_NEED_PALETTE_ARB				0x2004
	#define WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
	#define WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
	#define WGL_SWAP_METHOD_ARB					0x2007
	#define WGL_NUMBER_OVERLAYS_ARB				0x2008
	#define WGL_NUMBER_UNDERLAYS_ARB			0x2009
	#define WGL_TRANSPARENT_ARB					0x200A
	#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
	#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
	#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
	#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
	#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
	#define WGL_SHARE_DEPTH_ARB					0x200C
	#define WGL_SHARE_STENCIL_ARB				0x200D
	#define WGL_SHARE_ACCUM_ARB					0x200E
	#define WGL_SUPPORT_GDI_ARB					0x200F
	#define WGL_SUPPORT_OPENGL_ARB				0x2010
	#define WGL_DOUBLE_BUFFER_ARB				0x2011
	#define WGL_STEREO_ARB						0x2012
	#define WGL_PIXEL_TYPE_ARB					0x2013
	#define WGL_COLOR_BITS_ARB					0x2014
	#define WGL_RED_BITS_ARB					0x2015
	#define WGL_RED_SHIFT_ARB					0x2016
	#define WGL_GREEN_BITS_ARB					0x2017
	#define WGL_GREEN_SHIFT_ARB					0x2018
	#define WGL_BLUE_BITS_ARB					0x2019
	#define WGL_BLUE_SHIFT_ARB					0x201A
	#define WGL_ALPHA_BITS_ARB					0x201B
	#define WGL_ALPHA_SHIFT_ARB					0x201C
	#define WGL_ACCUM_BITS_ARB					0x201D
	#define WGL_ACCUM_RED_BITS_ARB				0x201E
	#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
	#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
	#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
	#define WGL_DEPTH_BITS_ARB					0x2022
	#define WGL_STENCIL_BITS_ARB				0x2023
	#define WGL_AUX_BUFFERS_ARB					0x2024

	#define WGL_NO_ACCELERATION_ARB				0x2025
	#define WGL_GENERIC_ACCELERATION_ARB		0x2026
	#define WGL_FULL_ACCELERATION_ARB			0x2027
	#define WGL_SWAP_EXCHANGE_ARB				0x2028
	#define WGL_SWAP_COPY_ARB					0x2029
	#define WGL_SWAP_UNDEFINED_ARB				0x202A
	#define WGL_TYPE_RGBA_ARB					0x202B
	#define WGL_TYPE_COLORINDEX_ARB				0x202C

	typedef BOOL (RAPTOR_APICALL * PFN_WGL_GET_PIXEL_FORMAT_ATTRIBIV_ARB_PROC)(HDC hdc,int iPixelFormat,int iLayerPlane,UINT nAttributes,const int *piAttributes,int *piValues);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_GET_PIXEL_FORMAT_ATTRIBFV_ARB_PROC)(HDC hdc,int iPixelFormat,int iLayerPlane,UINT nAttributes,const int *piAttributes,FLOAT *pfValues);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_CHOOSE_PIXEL_FORMAT_ARB_PROC)(HDC hdc,const int *piAttribIList,const FLOAT *pfAttribFList,UINT nMaxFormats,int *piFormats,UINT *nNumFormats);

	#ifndef DECLARE_WGL_ARB_pixel_format
	#define DECLARE_WGL_ARB_pixel_format(LINKAGE) \
		LINKAGE PFN_WGL_GET_PIXEL_FORMAT_ATTRIBIV_ARB_PROC wglGetPixelFormatAttribivARB;\
		LINKAGE PFN_WGL_GET_PIXEL_FORMAT_ATTRIBFV_ARB_PROC wglGetPixelFormatAttribfvARB;\
		LINKAGE PFN_WGL_CHOOSE_PIXEL_FORMAT_ARB_PROC wglChoosePixelFormatARB;
	#endif
#else
	#define DECLARE_WGL_ARB_pixel_format(LINKAGE)
#endif


#if defined(WGL_ARB_pbuffer)
    typedef HANDLE HPBUFFERARB;

	#define WGL_DRAW_TO_PBUFFER_ARB				0x202D
	#define WGL_MAX_PBUFFER_PIXELS_ARB			0x202E
	#define WGL_MAX_PBUFFER_WIDTH_ARB			0x202F
	#define WGL_MAX_PBUFFER_HEIGHT_ARB			0x2030
	#define WGL_PBUFFER_LARGEST_ARB				0x2033
	#define WGL_PBUFFER_WIDTH_ARB				0x2034
	#define WGL_PBUFFER_HEIGHT_ARB				0x2035
	#define WGL_PBUFFER_LOST_ARB				0x2036

	typedef HPBUFFERARB (RAPTOR_APICALL * PFN_WGL_CREATE_PBUFFER_ARB_PROC)(HDC hDC,int iPixelFormat,int iWidth,int iHeight,const int *piAttribList);
	typedef HDC (RAPTOR_APICALL * PFN_WGL_GET_PBUFFER_DC_ARB_PROC)(HPBUFFERARB hPbuffer);
	typedef int (RAPTOR_APICALL * PFN_WGL_RELEASE_PBUFFER_DC_ARB_PROC)(HPBUFFERARB hPbuffer,HDC hDC);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_DESTROY_PBUFFER_ARB_PROC)(HPBUFFERARB hPbuffer);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_QUERY_PBUFFER_ARB_PROC)(HPBUFFERARB hPbuffer,int iAttribute,int *piValue);

	#ifndef DECLARE_WGL_ARB_pbuffer
	#define DECLARE_WGL_ARB_pbuffer(LINKAGE) \
		LINKAGE PFN_WGL_CREATE_PBUFFER_ARB_PROC wglCreatePBufferARB;\
		LINKAGE PFN_WGL_GET_PBUFFER_DC_ARB_PROC wglGetPBufferDCARB;\
		LINKAGE PFN_WGL_RELEASE_PBUFFER_DC_ARB_PROC wglReleasePBufferDCARB;\
		LINKAGE PFN_WGL_DESTROY_PBUFFER_ARB_PROC wglDestroyPBufferARB;\
		LINKAGE PFN_WGL_QUERY_PBUFFER_ARB_PROC wglQueryPBufferARB;
	#endif
#else
	#define DECLARE_WGL_ARB_pbuffer(LINKAGE)
#endif


#if defined(WGL_ARB_render_texture)
	#define WGL_BIND_TO_TEXTURE_RGB_ARB			0x2070
	#define WGL_BIND_TO_TEXTURE_RGBA_ARB		0x2071
	#define WGL_TEXTURE_FORMAT_ARB				0x2072
	#define WGL_TEXTURE_TARGET_ARB				0x2073
	#define WGL_MIPMAP_TEXTURE_ARB				0x2074
	#define WGL_TEXTURE_RGB_ARB					0x2075
	#define WGL_TEXTURE_RGBA_ARB				0x2076
	#define WGL_NO_TEXTURE_ARB					0x2077
	#define WGL_TEXTURE_CUBE_MAP_ARB			0x2078
	#define WGL_TEXTURE_1D_ARB					0x2079
	#define WGL_TEXTURE_2D_ARB					0x207A
	#define WGL_NO_TEXTURE_ARB					0x2077
	#define WGL_MIPMAP_LEVEL_ARB				0x207B
	#define WGL_CUBE_MAP_FACE_ARB				0x207C
	#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x207D
	#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
	#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x207F
	#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
	#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x2081
	#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
	#define WGL_FRONT_LEFT_ARB					0x2083
	#define WGL_FRONT_RIGHT_ARB					0x2084
	#define WGL_BACK_LEFT_ARB					0x2085
	#define WGL_BACK_RIGHT_ARB					0x2086
	#define WGL_AUX0_ARB						0x2087
	#define WGL_AUX1_ARB						0x2088
	#define WGL_AUX2_ARB						0x2089
	#define WGL_AUX3_ARB						0x208A
	#define WGL_AUX4_ARB						0x208B
	#define WGL_AUX5_ARB						0x208C
	#define WGL_AUX6_ARB						0x208D
	#define WGL_AUX7_ARB						0x208E
	#define WGL_AUX8_ARB						0x208F
	#define WGL_AUX9_ARB						0x2090

	typedef BOOL (RAPTOR_APICALL * PFN_WGL_BIND_TEX_IMAGE_ARB_PROC)(HPBUFFERARB hPbuffer, int iBuffer);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_RELEASE_TEX_IMAGE_ARB_PROC)(HPBUFFERARB hPbuffer, int iBuffer);
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_SET_PBUFFER_ATTRIB_ARB_PROC)(HPBUFFERARB hPbuffer,const int *piAttribList);

	#ifndef	DECLARE_WGL_ARB_render_texture
	#define DECLARE_WGL_ARB_render_texture(LINKAGE) \
		LINKAGE PFN_WGL_BIND_TEX_IMAGE_ARB_PROC wglBindTexImageARB;\
		LINKAGE PFN_WGL_RELEASE_TEX_IMAGE_ARB_PROC wglReleaseTexImageARB;\
		LINKAGE PFN_WGL_SET_PBUFFER_ATTRIB_ARB_PROC wglSetPbufferAttribARB;
	#endif
#else
	#define DECLARE_WGL_ARB_render_texture(LINKAGE)
#endif


#if defined(WGL_ATI_pixel_format_float)
	#define WGL_RGBA_FLOAT_MODE_ATI             0x8820
	#define WGL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI	0x8835
	#define WGL_TYPE_RGBA_FLOAT_ATI				0x21A0
#endif


/* WGL_EXT_swap_control */
#if defined(WGL_EXT_swap_control)
	typedef BOOL (RAPTOR_APICALL * PFN_WGL_SWAP_INTERVAL_PROC) (int interval);
	typedef int (RAPTOR_APICALL * PFN_WGL_GET_SWAP_INTERVAL_PROC) (void);

	#ifndef DECLARE_WGL_EXT_swap_control
	#define DECLARE_WGL_EXT_swap_control(LINKAGE) \
		LINKAGE PFN_WGL_SWAP_INTERVAL_PROC		wglSwapIntervalEXT; \
		LINKAGE PFN_WGL_GET_SWAP_INTERVAL_PROC	wglGetSwapIntervalEXT;
	#endif
#else
	#define DECLARE_WGL_EXT_swap_control(LINKAGE)
#endif

#if defined(WGL_EXT_swap_control_tear)
	// No new tokens
	// No new functions: only SwapInterval(-1) syncs to monitor actual refresh rate
#endif


#if defined(WGL_ARB_create_context) || defined(WGL_ARB_create_context_profile)
	#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
	#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
	#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
	#define WGL_CONTEXT_FLAGS_ARB                   0x2094
	#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
	#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
	#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
	#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
	#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

	#define ERROR_INVALID_VERSION_ARB               0x2095
	#define ERROR_INVALID_PROFILE_ARB               0x2096

	typedef HGLRC(RAPTOR_APICALL *PFN_WGL_CREATE_CONTEXT_ATTRIBS_PROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

	#ifndef DECLARE_WGL_ARB_create_context
	#define DECLARE_WGL_ARB_create_context(LINKAGE) \
			LINKAGE PFN_WGL_CREATE_CONTEXT_ATTRIBS_PROC		wglCreateContextAttribsARB;
	#endif
#else
	#define DECLARE_WGL_ARB_create_context(LINKAGE)
#endif

#endif

