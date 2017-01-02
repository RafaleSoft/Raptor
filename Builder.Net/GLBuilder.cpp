// GLBuilder.cpp: implementation of the CGLBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "GLBuilder.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
using std::ofstream;

//////////////////////////////////////////////////////////////////////
//
//	Code generation
//
static const char *HEADER =
"//////////////////////////////////////////////////////////////////////////\n\
//	This file is generated by Raptor Builder application from \n\
//	Rafale Soft Inc. (c) and is necessary to compile Raptor.\n\
//\n\
//	Do not modify this file, your changes would be lost on next generation\n\
//	This file defines how Raptor code is optimized according to the \n\
//	extensions selected in Raptor Builder\n\
///////////////////////////////////////////////////////////////////////////\n\
\n\n\
#if !defined(__CODE_GENERATION_H__)\n\
#define __CODE_GENERATION_H__\n\n";

static const char *OGL = "//\n//	OpenGL extensions\n//\n\
//	Mandatory extensions \n\
#define GL_EXT_vertex_array                 1\n\n";

static const char *CODE =
"\n\n\
//	CPU code generation\n\n";

static const char *STL =
"\n\n\
//	Default entry point\n\
#if defined(_WIN32)\n\
	#include \"stdafx.h\" \n\
	#if !defined(WINAPI)\n\
        #define WINAPI	__stdcall\n\
	#endif\n\
	#ifdef EXPORT_RAPTOR_CORE\n\
		#define RAPTOR_API __declspec(dllexport)\n\
	#else\n\
		#define RAPTOR_API _declspec(dllimport)\n\
	#endif\n\
    #define RAPTOR_FASTCALL __fastcall\n\
    #define RAPTOR_CCALL __cdecl\n\
    #define RAPTOR_APICALL WINAPI\n\
    #if _MSC_VER > 1200     // 1200 is Visual C++ 6.0\n\
        #define RAPTOR_TYPENAME typename\n\
    #else\n\
        #define RAPTOR_TYPENAME\n\
    #endif\n\
#else\n\
	#include <stdlib.h>\n\
	#define RAPTOR_API\n\
    #define RAPTOR_FASTCALL\n\
    #define RAPTOR_CCALL\n\
    #define RAPTOR_APICALL\n\
    #define RAPTOR_TYPENAME typename\n\
#endif\n\n\
#if defined(_WIN32) \n\
    #pragma warning(disable: 4786)    //  dbug info too large \n\
    #pragma warning(disable: 4100)  \n\
	#pragma warning(disable: 4244)	\n\
    #pragma warning(disable: 4511)    // copy ctor not generated \n\
    #pragma warning(disable: 4512)    // assign operator not generated \n\
    #pragma warning(disable: 4663)    // C++ language changes \n\
	#pragma warning(disable: 4251)    // DLL interface required for STL exports \n\
	#pragma warning(disable: 4275)    // deriving exported class from non-exported \n\
#endif \n\
\n\n\
//	Standard sized int types \n\
#if defined(_MSC_VER) && (_MSC_VER < 1600) \n\
	typedef signed   __int8  int8_t;	\n\
	typedef unsigned __int8  uint8_t;	\n\
	typedef signed   __int16 int16_t;	\n\
	typedef unsigned __int16 uint16_t;	\n\
	typedef signed   __int32 int32_t;	\n\
	typedef unsigned __int32 uint32_t;	\n\
	typedef signed   __int64 int64_t;	\n\
	typedef unsigned __int64 uint64_t;	\n\
#else	\n\
	#include <stdint.h>	\n\
#endif\n\n\
//	Standard Template Library Headers used for Raptor\n\
#include <string> \n\
#include <vector> \n\
#include <map> \n\
#include <sstream> \n\
using namespace std;\n\
\n\
RAPTOR_NAMESPACE_BEGIN\n\
typedef struct lessString\n\
{\n\
	bool operator()(const string& x,const string &y) const\n\
	{\n\
		return (x.compare(y) < 0);\n\
	}\n\
} lessString;\n\
typedef map<string,void*,lessString> MapStringToPtr;\n\
RAPTOR_NAMESPACE_END\n\
\n\
// define linkage specifier for declarators \n\
#define DEFAULT_LINKAGE \n\
#define STATIC_LINKAGE        static \n\
#define EXTERN_LINKAGE      extern \n\
\n";

static const char *NAMESPACE =
"\n\n\
#define RAPTOR_NAMESPACE_BEGIN namespace raptor { \n\
#define RAPTOR_NAMESPACE_END	} \n\
#define RAPTOR_NAMESPACE using namespace raptor; \n\
#define RAPTOR(name) raptor::name \n\
\n";

static const char *END =
"\n\n\
#endif\n";



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLBuilder::CGLBuilder():
    m_buildDebug(false),m_buildRedist(false),m_buildCompute(false)
{
	majorVersion = 1;
	minorVersion = 0;
	releaseNumber = 0;

	EXTENSION extension;

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = CPU;	extension.extensionName = "RAPTOR_SMP_CODE_GENERATION";
	extensions.push_back(extension); 

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = CPU;	extension.extensionName = "RAPTOR_SSE_CODE_GENERATION";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = CPU;	extension.extensionName = "RAPTOR_SSE2_CODE_GENERATION";
    extension.dependencies.push_back("RAPTOR_SSE_CODE_GENERATION");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_0";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_1";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_2";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_3";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_4";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_1_5";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_2_0";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_2_1";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_3_0";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_3_1";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_3_2";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_3_3";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_0";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_1";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_2";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_3";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_4";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREGL;	extension.extensionName = "GL_VERSION_4_5";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = COREVK;	extension.extensionName = "VK_VERSION_1_0";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_multitexture";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_compression";
    extension.dependencies.push_back("GL_ARB_texture_cube_map");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_mirrored_repeat";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_env_combine";
    extension.dependencies.push_back("GL_ARB_multitexture");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_transpose_matrix";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_imaging";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_multisample";
#ifdef WIN32
    extension.dependencies.push_back("WGL_ARB_extensions_string");
    extension.dependencies.push_back("WGL_ARB_pixel_format");
#endif
	extensions.push_back(extension);

    extension.dependencies.clear();
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_env_dot3";
    extension.dependencies.push_back("GL_ARB_multitexture");
    extension.dependencies.push_back("GL_ARB_texture_env_combine");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_cube_map";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_vertex_buffer_object";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_fragment_program";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_fragment_program_shadow";
    extension.dependencies.push_back("GL_ARB_fragment_program");
    extension.dependencies.push_back("GL_ARB_shadow");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_occlusion_query";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_point_sprite";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_point_parameters";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_shadow";
    extension.dependencies.push_back("GL_ARB_depth_texture");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_shadow_ambient";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_depth_texture";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_vertex_program";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_non_power_of_two";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_color_buffer_float";
#ifdef WIN32
    extension.dependencies.push_back("WGL_ARB_pixel_format");
#endif
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_float";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_border_clamp";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_pixel_buffer_object";
    extension.dependencies.push_back("GL_ARB_vertex_buffer_object");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_rectangle";
	extension.dependencies.push_back("GL_VERSION_1_1");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_shading_language_100";
    extension.dependencies.push_back("GL_ARB_shader_objects");
    extension.dependencies.push_back("GL_ARB_fragment_shader");
    extension.dependencies.push_back("GL_ARB_vertex_shader");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_shader_objects";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_vertex_shader";
    extension.dependencies.push_back("GL_ARB_shader_objects");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_fragment_shader";
    extension.dependencies.push_back("GL_ARB_shader_objects");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_draw_buffers";
	extension.dependencies.push_back("GL_VERSION_1_3");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_half_float_pixel";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_window_pos";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_vertex_array_object";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_compression_rgtc";
	extension.dependencies.push_back("GL_ARB_texture_compression");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_compression_bptc";
	extension.dependencies.push_back("GL_ARB_texture_compression");
	extension.dependencies.push_back("GL_VERSION_3_1");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_texture_rg";
	extension.dependencies.push_back("GL_VERSION_1_1");
	extensions.push_back(extension);

	extension.dependencies.clear();
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_fragment_coord_conventions";
	extension.dependencies.push_back("GL_VERSION_3_1");
	extensions.push_back(extension);

	extension.dependencies.clear();
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_geometry_shader4";
	extension.dependencies.push_back("GL_VERSION_2_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_swapchain";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_display";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_display_swapchain";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_xlib_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_xcb_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_wayland_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_mir_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_android_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_KHR_win32_surface";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = VK;	extension.extensionName = "VK_EXT_debug_report";
	extension.dependencies.push_back("VK_VERSION_1_0");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = _3DFX;	extension.extensionName = "GL_3DFX_texture_compression_FXT1";
	extension.dependencies.push_back("GL_ARB_texture_compression");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_compression_latc";
	extension.dependencies.push_back("GL_ARB_texture_compression");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_abgr";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_bgra";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_cmyka";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_clip_volume_hint";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_compiled_vertex_array";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_cull_vertex";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_packed_pixels";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_point_parameters";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_stencil_wrap";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_stencil_two_side";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_compression_s3tc";
    extension.dependencies.push_back("GL_ARB_texture_compression");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_env_add";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_secondary_color";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_separate_specular_color";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_vertex_weighting";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_filter_anisotropic";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_fog_coord";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture3D";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_edge_clamp";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_blend_color";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_framebuffer_object";
    extension.dependencies.push_back("GL_VERSION_1_1");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_framebuffer_blit";
    extension.dependencies.push_back("GL_VERSION_1_1");
	extension.dependencies.push_back("GL_EXT_framebuffer_object");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_framebuffer_multisample";
    extension.dependencies.push_back("GL_EXT_framebuffer_blit");
	extension.dependencies.push_back("GL_EXT_framebuffer_object");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_packed_depth_stencil";
    extension.dependencies.push_back("GL_VERSION_1_1");
	extension.dependencies.push_back("GL_EXT_framebuffer_object");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_fragment_program";
    extension.dependencies.push_back("GL_VERSION_1_2");
    extension.dependencies.push_back("GL_ARB_multitexture");
	extensions.push_back(extension);
    
	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_float_buffer";
	extension.dependencies.push_back("GL_NV_fragment_program");
	extension.dependencies.push_back("GL_NV_texture_rectangle");
#ifdef WIN32
	extension.dependencies.push_back("WGL_ARB_pixel_format");
	extension.dependencies.push_back("WGL_ARB_render_texture");
	extension.dependencies.push_back("WGL_NV_render_texture_rectangle");
#endif
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_texgen_reflection";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_texture_env_combine4";
    extension.dependencies.push_back("GL_ARB_texture_env_combine");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_texture_rectangle";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_texture_shader";
    extension.dependencies.push_back("GL_ARB_multitexture");
    extension.dependencies.push_back("GL_ARB_texture_cube_map");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_vertex_array_range";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_point_sprite";
    extension.dependencies.push_back("GL_EXT_point_parameters");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_register_combiners";
    extension.dependencies.push_back("GL_ARB_multitexture");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_EXT_swap_control";
    extension.dependencies.push_back("WGL_ARB_extensions_string");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "GL_WIN_swap_hint";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_ARB_extensions_string";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_ARB_pixel_format";
    extension.dependencies.push_back("WGL_ARB_extensions_string");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
	extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_ARB_pbuffer";
    extension.dependencies.push_back("WGL_ARB_extensions_string");
    extension.dependencies.push_back("WGL_ARB_pixel_format");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_ARB_render_texture";
    extension.dependencies.push_back("WGL_ARB_extensions_string");
    extension.dependencies.push_back("WGL_ARB_pixel_format");
    extension.dependencies.push_back("WGL_ARB_pbuffer");
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_NV_render_depth_texture";
    extension.dependencies.push_back("WGL_ARB_render_texture");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_NV_render_texture_rectangle";
	extensions.push_back(extension);

    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = GLX;	extension.extensionName = "GLX_EXTENSIONS";
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ATI;	extension.extensionName = "WGL_ATI_pixel_format_float";
	extension.dependencies.push_back("WGL_ARB_pixel_format");
	extensions.push_back(extension);

	extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_EXT_swap_control_tear";
	extension.dependencies.push_back("WGL_EXT_swap_control");
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = KHR;	extension.extensionName = "GL_KHR_texture_compression_astc_hdr";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = KHR;	extension.extensionName = "GL_KHR_texture_compression_astc_ldr";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = NV;	extension.extensionName = "GL_NV_texture_compression_vtc";
	extension.dependencies.push_back("GL_ARB_texture_compression");
	extension.dependencies.push_back("GL_EXT_texture_compression_s3tc");
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = EXT;	extension.extensionName = "GL_EXT_texture_compression_dxt1";
	extension.dependencies.push_back("GL_VERSION_1_3");
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_compressed_texture_pixel_storage";
	extension.dependencies.push_back("GL_VERSION_1_2");
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = OES;	extension.extensionName = "GL_OES_compressed_paletted_texture";
	extensions.push_back(extension);

	extension.dependencies.clear();
	extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_uniform_buffer_object";
	extension.dependencies.push_back("GL_ARB_shader_objects");
	extension.dependencies.push_back("GL_ARB_vertex_buffer_object");
	extensions.push_back(extension);
}

CGLBuilder::~CGLBuilder()
{

}


void CGLBuilder::activateAll(void)
{
	for (unsigned int i=0;i<extensions.size();i++)
	{
		EXTENSION &extension = extensions[i];
		extension.active = true;
	}

    m_buildDebug = true;
	m_buildRedist = true;
    m_buildCompute = true;
}

void CGLBuilder::activateNone(void)
{
	for (unsigned int i=0;i<extensions.size();i++)
	{
		EXTENSION &extension = extensions[i];
		extension.active = false;
	}

    m_buildDebug = false;
	m_buildRedist = false;
    m_buildCompute = false;
}

bool CGLBuilder::activateExtension(const string &extension,bool activate)
{
	bool res = false;

	for (unsigned int i=0;i<extensions.size();i++)
	{
		EXTENSION& ext = extensions[i];

		if ( 0 == ext.extensionName.compare(extension) )
		{
			ext.active = activate;
			res = true;
			break;
		}
	}

	return res;
}

bool CGLBuilder::isExtensionActive(const string &extension) const
{
	bool res = false;

	for (unsigned int i=0;i<extensions.size();i++)
	{
		const EXTENSION& ext = extensions[i];

		if ( 0 == ext.extensionName.compare(extension) )
		{
			res = ext.active;
			break;
		}
	}

	return res;
}

void CGLBuilder::glQueryExtensions(const string &strExtensions)
{
	for (unsigned int i=0;i<extensions.size();i++)
	{
		const EXTENSION& extension = extensions[i];
		activateExtension(extension.extensionName, (strExtensions.find(extension.extensionName) != string::npos));
	}
}

string CGLBuilder::setVersion(const string &strVersion)
{
	string strResult;

	string::size_type pos = strVersion.find('.');
	if (pos != string::npos)
	{
		string major = strVersion.substr(0,pos);
		majorVersion = atoi(major.c_str());

		if ((pos+1) < strVersion.length())
		{
			string::size_type pos2 = strVersion.find('.',pos+1);
			string minor = strVersion.substr(pos+1,pos2-pos-1);
			minorVersion = atoi(minor.c_str());

			if ((pos2+1) < strVersion.length())
			{
				string release = strVersion.substr(pos2+1,string::npos);
				releaseNumber = atoi(release.c_str());
			}
		}
	}

	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 0)))
		strResult += " GL_VERSION_1_0";
	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 1)))
		strResult += " GL_VERSION_1_1";
	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 2)))
		strResult += " GL_VERSION_1_2";
	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 3)))
		strResult += " GL_VERSION_1_3";
	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 4)))
		strResult += " GL_VERSION_1_4";
	if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 5)))
		strResult += " GL_VERSION_1_5";
	if ((majorVersion > 2) || ((majorVersion == 2) && (minorVersion >= 0)))
		strResult += " GL_VERSION_2_0";
	if ((majorVersion > 2) || ((majorVersion == 2) && (minorVersion >= 1)))
		strResult += " GL_VERSION_2_1";
	if ((majorVersion > 3) || ((majorVersion == 3) && (minorVersion >= 0)))
		strResult += " GL_VERSION_3_0";
	if ((majorVersion > 3) || ((majorVersion == 3) && (minorVersion >= 1)))
		strResult += " GL_VERSION_3_1";
	if ((majorVersion > 3) || ((majorVersion == 3) && (minorVersion >= 2)))
		strResult += " GL_VERSION_3_2";
	if ((majorVersion > 3) || ((majorVersion == 3) && (minorVersion >= 3)))
		strResult += " GL_VERSION_3_3";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 0)))
		strResult += " GL_VERSION_4_0";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 1)))
		strResult += " GL_VERSION_4_1";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 2)))
		strResult += " GL_VERSION_4_2";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 3)))
		strResult += " GL_VERSION_4_3";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 4)))
		strResult += " GL_VERSION_4_4";
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 5)))
		strResult += " GL_VERSION_4_5";

	return strResult;
}


bool CGLBuilder::checkConsistency(bool forceDependency)
{
    bool res = true;

    for (unsigned int i=0;i<extensions.size();i++)
	{
		const EXTENSION& extension = extensions[i];
		if (extension.active)
        {
			for (unsigned int j=0;j<extension.dependencies.size();j++)
			{
				if (forceDependency)
					activateExtension(extension.dependencies[j],true);
				else if (!isExtensionActive(extension.dependencies[j]))
				{
					printf(	"Error: extension %s is missing %s\n",
							extension.extensionName.c_str(),
							extension.dependencies[j].c_str());
					res = false;
				}
			}
		}
    }

    return res;
}

bool CGLBuilder::writeHeader(const string& filename)
{
	ofstream header(filename.data());
	if (!header.is_open())
		return false;

	header.write(HEADER,strlen(HEADER));
    header.write(NAMESPACE,strlen(NAMESPACE));
    header.write(STL,strlen(STL));

	//	Then write back code generation
	if (m_buildRedist)
	{
		header << "// Redistribuable package build\n#define BUILD_REDIST\n\n";
	}

	if (m_buildDebug)
	{
		header << "// Raptor Debug build\n#define RAPTOR_DEBUG_MODE_GENERATION			";
		header << m_buildDebug << "\n\n";
	}

    if (m_buildCompute)
	{
		header << "// Activate event framework\n#define BUILD_COMPUTE\n\n";
	}

	header.write(OGL,strlen(OGL));
	
    unsigned int i=0;
	header << "// OpenGL versions\n";
	for (i=0;i<extensions.size();i++)
	{
		const EXTENSION& extension = extensions[i];
		if ((extension.active) && ((extension.kind == COREGL) || (extension.kind == COREVK)))
		{
			header << "#ifndef " << extension.extensionName << "\n";
			header << "	#define	" << extension.extensionName << "		1\n";
			header << "#endif\n"; 
		}
	}
	header << "\n";

	for (i=0;i<extensions.size();i++)
	{
		const EXTENSION& extension = extensions[i];
		if ((extension.active) && (extension.kind != CPU) && (extension.kind != COREGL) && (extension.kind != COREVK))
		{
			header << "#define ";
			header << extension.extensionName;
			header << "				1\n";
		}
	}
	
	header.write(CODE,strlen(CODE));

	for (i=0;i<extensions.size();i++)
	{
		const EXTENSION& extension = extensions[i];
		if ((extension.active) && (extension.kind == CPU))
		{
			header << "#define ";
			header << extension.extensionName;
			header << "				1\n";
		}
	}
	
	header.write(END,strlen(END));
	header.close();

	return true;
}


