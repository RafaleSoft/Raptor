/***************************************************************************/
/*                                                                         */
/*  GLBuilder.cpp                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
"\n\
/***************************************************************************/\n\
/*                                                                         */\n\
/*	CodeGeneration.h                                                       */\n\
/*                                                                         */\n\
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */\n\
/*                                                                         */\n\
/*   This file is generated by Raptor Builder application from             */\n\
/*   Rafale Soft Inc. (c) and is necessary to compile Raptor.              */\n\
/*   Do not modify this file, your changes would be lost on next generation*/\n\
/*   This file defines how Raptor code is optimized according to the       */\n\
/*	 extensions selected in Raptor Builder.                                */\n\
/*                                                                         */\n\
/*  Copyright 1998-2019 by                                                 */\n\
/*  Fabrice FERRAND.                                                       */\n\
/*                                                                         */\n\
/*  This file is part of the Raptor project, and may only be used,         */\n\
/*  modified, and distributed under the terms of the Raptor project        */\n\
/*  license, LICENSE.  By continuing to use, modify, or distribute         */\n\
/*  this file you indicate that you have read the license and              */\n\
/*  understand and accept it fully.                                        */\n\
/*                                                                         */\n\
/***************************************************************************/\n\
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
	#define RAPTOR_INTERFACE __interface\n\
#else // Linux environment \n\
	#include <stdlib.h>\n\
	#include <typeinfo>\n\
	#define RAPTOR_API\n\
    #define RAPTOR_FASTCALL\n\
    #define RAPTOR_CCALL\n\
    #define RAPTOR_APICALL\n\
    #define RAPTOR_TYPENAME typename\n\
	#define RAPTOR_INTERFACE class\n\
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
\n\n\
// define types for compatibility with Android OpenGLES \n\
#if defined(_ANDROID) \n\
#define GLdouble double \n\
#endif\n\n\
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

const char* CGLBuilder::core_profile = "GL_CORE_profile";
const char* CGLBuilder::compatibility_profile = "GL_COMPATIBILITY_profile";
const char* CGLBuilder::full_profile = "GL_FULL_profile";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLBuilder::CGLBuilder():
    m_buildDebug(false),m_buildRedist(false),m_buildCompute(false),
	majorVersion(1), minorVersion(0), releaseNumber(0)
{
	addExtension(CPU, "RAPTOR_SMP_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSE_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSE2_CODE_GENERATION", "RAPTOR_SSE_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSE3_CODE_GENERATION", "RAPTOR_SSE_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSSE3_CODE_GENERATION", "RAPTOR_SSE3_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSE41_CODE_GENERATION", "RAPTOR_SSE_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_SSE42_CODE_GENERATION", "RAPTOR_SSE_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_AES_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_AVX_CODE_GENERATION");
	addExtension(CPU, "RAPTOR_FMA_CODE_GENERATION");

	addExtension(COREGL, "GL_VERSION_1_0");
	addExtension(COREGL, "GL_VERSION_1_1");
	addExtension(COREGL, "GL_VERSION_1_2");
	addExtension(COREGL, "GL_VERSION_1_3");
	addExtension(COREGL, "GL_VERSION_1_4");
	addExtension(COREGL, "GL_VERSION_1_5");
	addExtension(COREGL, "GL_VERSION_2_0");
	addExtension(COREGL, "GL_VERSION_2_1");
	addExtension(COREGL, "GL_VERSION_3_0");
	addExtension(COREGL, "GL_VERSION_3_1");
	addExtension(COREGL, "GL_VERSION_3_2");
	addExtension(COREGL, "GL_VERSION_3_3");
	addExtension(COREGL, "GL_VERSION_4_0");
	addExtension(COREGL, "GL_VERSION_4_1");
	addExtension(COREGL, "GL_VERSION_4_2");
	addExtension(COREGL, "GL_VERSION_4_3");
	addExtension(COREGL, "GL_VERSION_4_4");
	addExtension(COREGL, "GL_VERSION_4_5");
	addExtension(COREGL, "GL_VERSION_4_6");

	addExtension(GLPROFILE, CGLBuilder::core_profile);
	addExtension(GLPROFILE, CGLBuilder::compatibility_profile);
	addExtension(GLPROFILE, CGLBuilder::full_profile);

	addExtension(ARB, "GL_ARB_multitexture");
	addExtension(ARB, "GL_ARB_texture_mirrored_repeat");
	addExtension(ARB, "GL_ARB_transpose_matrix");
	addExtension(ARB, "GL_ARB_imaging");
	addExtension(ARB, "GL_ARB_texture_cube_map");
	addExtension(ARB, "GL_ARB_vertex_buffer_object");
	addExtension(ARB, "GL_ARB_fragment_program");
	addExtension(ARB, "GL_ARB_occlusion_query");
	addExtension(ARB, "GL_ARB_point_sprite");
	addExtension(ARB, "GL_ARB_point_parameters");
	addExtension(ARB, "GL_ARB_shadow_ambient");
	addExtension(ARB, "GL_ARB_depth_texture");
	addExtension(ARB, "GL_ARB_vertex_program");
	addExtension(ARB, "GL_ARB_texture_non_power_of_two");
	addExtension(ARB, "GL_ARB_texture_float");
	addExtension(ARB, "GL_ARB_texture_border_clamp");
	addExtension(ARB, "GL_ARB_shader_objects");
	addExtension(ARB, "GL_ARB_half_float_pixel");
	addExtension(ARB, "GL_ARB_window_pos");
	addExtension(ARB, "GL_ARB_vertex_array_object");
	addExtension(ARB, "GL_ARB_texture_compression", "GL_ARB_texture_cube_map");
	addExtension(ARB, "GL_ARB_texture_mirror_clamp_to_edge", "GL_VERSION_1_4");
	addExtension(ARB, "GL_ARB_texture_env_combine", "GL_ARB_multitexture");
	addExtension(ARB, "GL_ARB_shadow", "GL_ARB_depth_texture");
	addExtension(ARB, "GL_ARB_pixel_buffer_object", "GL_ARB_vertex_buffer_object");
	addExtension(ARB, "GL_ARB_texture_rectangle", "GL_VERSION_1_1");
	addExtension(ARB, "GL_ARB_vertex_shader", "GL_ARB_shader_objects");
	addExtension(ARB, "GL_ARB_fragment_shader", "GL_ARB_shader_objects");
	addExtension(ARB, "GL_ARB_texture_compression_rgtc", "GL_ARB_texture_compression");
	addExtension(ARB, "GL_ARB_texture_rg", "GL_VERSION_1_1");
	addExtension(ARB, "GL_ARB_fragment_coord_conventions", "GL_VERSION_3_1");
	addExtension(ARB, "GL_ARB_geometry_shader4", "GL_VERSION_2_0");
	addExtension(ARB, "GL_ARB_compressed_texture_pixel_storage", "GL_VERSION_1_2");
	addExtension(ARB, "GL_ARB_draw_buffers", "GL_VERSION_1_3");
	addExtension(ARB, "GL_ARB_color_buffer_float", "WGL_ARB_pixel_format");
	addExtension(ARB, "GL_ARB_multisample", "WGL_ARB_extensions_string", "WGL_ARB_pixel_format");
	addExtension(ARB, "GL_ARB_texture_env_dot3", "GL_ARB_multitexture", "GL_ARB_texture_env_combine");
	addExtension(ARB, "GL_ARB_fragment_program_shadow", "GL_ARB_fragment_program", "GL_ARB_shadow");
	addExtension(ARB, "GL_ARB_texture_compression_bptc", "GL_ARB_texture_compression", "GL_VERSION_3_1");
	addExtension(ARB, "GL_ARB_uniform_buffer_object", "GL_ARB_shader_objects", "GL_ARB_vertex_buffer_object");
	addExtension(ARB, "GL_ARB_compatibility");

	EXTENSION extension;
    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = ARB;	extension.extensionName = "GL_ARB_shading_language_100";
    extension.dependencies.push_back("GL_ARB_shader_objects");
    extension.dependencies.push_back("GL_ARB_fragment_shader");
    extension.dependencies.push_back("GL_ARB_vertex_shader");
	extensions.push_back(extension);


	addExtension(COREVK, "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_swapchain", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_display", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_display_swapchain", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_xlib_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_xcb_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_wayland_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_mir_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_android_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_win32_surface", "VK_VERSION_1_0");
	addExtension(VK, "VK_EXT_debug_report", "VK_VERSION_1_0");
	addExtension(VK, "VK_KHR_sampler_mirror_clamp_to_edge", "VK_VERSION_1_0");

	addExtension(_3DFX, "GL_3DFX_texture_compression_FXT1", "GL_ARB_texture_compression");

	addExtension(EXT, "GL_EXT_abgr");
	addExtension(EXT, "GL_EXT_bgra");
	addExtension(EXT, "GL_EXT_cmyka");
	addExtension(EXT, "GL_EXT_clip_volume_hint");
	addExtension(EXT, "GL_EXT_compiled_vertex_array");
	addExtension(EXT, "GL_EXT_cull_vertex");
	addExtension(EXT, "GL_EXT_packed_pixels");
	addExtension(EXT, "GL_EXT_point_parameters");
	addExtension(EXT, "GL_EXT_stencil_wrap");
	addExtension(EXT, "GL_EXT_stencil_two_side");
	addExtension(EXT, "GL_EXT_texture_env_add");
	addExtension(EXT, "GL_EXT_secondary_color");
	addExtension(EXT, "GL_EXT_separate_specular_color");
	addExtension(EXT, "GL_EXT_vertex_weighting");
	addExtension(EXT, "GL_EXT_texture_filter_anisotropic");
	addExtension(EXT, "GL_EXT_fog_coord");
	addExtension(EXT, "GL_EXT_texture3D");
	addExtension(EXT, "GL_EXT_texture_edge_clamp");
	addExtension(EXT, "GL_EXT_blend_color");
	addExtension(EXT, "GL_EXT_texture_compression_latc", "GL_ARB_texture_compression");
	addExtension(EXT, "GL_EXT_texture_compression_s3tc", "GL_ARB_texture_compression");
	addExtension(EXT, "GL_EXT_texture_compression_dxt1", "GL_VERSION_1_3");
	addExtension(EXT, "GL_EXT_framebuffer_object", "GL_VERSION_1_1");
	addExtension(EXT, "GL_EXT_framebuffer_blit", "GL_VERSION_1_1", "GL_EXT_framebuffer_object");
	addExtension(EXT, "GL_EXT_framebuffer_multisample", "GL_EXT_framebuffer_blit", "GL_EXT_framebuffer_object");
	addExtension(EXT, "GL_EXT_packed_depth_stencil", "GL_VERSION_1_1", "GL_EXT_framebuffer_object");

	addExtension(NV, "GL_NV_texgen_reflection");
	addExtension(NV, "GL_NV_texture_rectangle");
	addExtension(NV, "GL_NV_vertex_array_range");
	addExtension(NV, "GL_NV_point_sprite", "GL_EXT_point_parameters");
	addExtension(NV, "GL_NV_register_combiners", "GL_ARB_multitexture");
	addExtension(NV, "GL_NV_texture_env_combine4", "GL_ARB_texture_env_combine");
	addExtension(NV, "GL_NV_fragment_program", "GL_VERSION_1_2", "GL_ARB_multitexture");
	addExtension(NV, "GL_NV_texture_shader", "GL_ARB_multitexture", "GL_ARB_texture_cube_map");
	addExtension(NV, "GL_NV_texture_compression_vtc", "GL_ARB_texture_compression", "GL_EXT_texture_compression_s3tc");

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


	addExtension(WGL, "GL_WIN_swap_hint");
	addExtension(WGL, "WGL_ARB_extensions_string");
	addExtension(WGL, "WGL_NV_render_texture_rectangle");
	addExtension(WGL, "WGL_EXT_swap_control", "WGL_ARB_extensions_string");
	addExtension(WGL, "WGL_ARB_pixel_format", "WGL_ARB_extensions_string");
	addExtension(WGL, "WGL_NV_render_depth_texture", "WGL_ARB_render_texture");
	addExtension(WGL, "WGL_EXT_swap_control_tear", "WGL_EXT_swap_control");
	addExtension(WGL, "WGL_ARB_pbuffer", "WGL_ARB_extensions_string", "WGL_ARB_pixel_format");
	addExtension(WGL, "WGL_ARB_create_context", "WGL_ARB_extensions_string", "WGL_ARB_create_context_profile");
	addExtension(WGL, "WGL_ARB_create_context_profile", "WGL_ARB_extensions_string", "WGL_ARB_create_context");
	
    extension.dependencies.clear(); 
    extension.active = false;	extension.kind = WGL;	extension.extensionName = "WGL_ARB_render_texture";
    extension.dependencies.push_back("WGL_ARB_extensions_string");
    extension.dependencies.push_back("WGL_ARB_pixel_format");
    extension.dependencies.push_back("WGL_ARB_pbuffer");
	extensions.push_back(extension);

    
	addExtension(GLX, "GLX_EXTENSIONS");

	addExtension(ATI, "WGL_ATI_pixel_format_float", "WGL_ARB_pixel_format");

	addExtension(KHR, "GL_KHR_texture_compression_astc_hdr");
	addExtension(KHR, "GL_KHR_texture_compression_astc_ldr");

	addExtension(OES, "GL_OES_compressed_paletted_texture");
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
	if ((majorVersion > 4) || ((majorVersion == 4) && (minorVersion >= 6)))
		strResult += " GL_VERSION_4_6";

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

		std::string extension_name = "";
		std::string::const_iterator it = extension.extensionName.begin();
		while (it != extension.extensionName.end())
			extension_name += (char)::toupper(*it++);
		header << "#define	" << extension_name << "_EXTENSION_NAME \"" << extension.extensionName << "\"\n";

		if ((extension.active) && (extension.kind != CPU) && (extension.kind != COREGL) && (extension.kind != COREVK))
		{
			header << "#ifndef " << extension.extensionName << "\n";
			header << "	#define ";
			header << extension.extensionName;
			header << "				1\n";
			header << "#endif\n";
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


void CGLBuilder::addExtension(EXTENSION_KIND kind, const std::string& extensionName)
{
	EXTENSION extension;

	extension.active = false;
	extension.kind = kind;
	extension.extensionName = extensionName;

	extensions.push_back(extension);
}

void CGLBuilder::addExtension(EXTENSION_KIND kind, const std::string& extensionName, const std::string& extensionDependency)
{
	EXTENSION extension;

	extension.active = false;
	extension.kind = kind;
	extension.extensionName = extensionName;
	extension.dependencies.push_back(extensionDependency);

	extensions.push_back(extension);
}

void CGLBuilder::addExtension(EXTENSION_KIND kind, const std::string& extensionName, const std::string& extensionDependency, const std::string& extensionDependency2)
{
	EXTENSION extension;

	extension.active = false;
	extension.kind = kind;
	extension.extensionName = extensionName;
	extension.dependencies.push_back(extensionDependency);
	extension.dependencies.push_back(extensionDependency2);

	extensions.push_back(extension);
}