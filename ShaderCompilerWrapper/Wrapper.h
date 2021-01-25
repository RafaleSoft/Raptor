#pragma once

#include "pch.h"
#include "Portability.h"
#include "System/RaptorDisplayConfig.h"
#include "System/RaptorDisplay.h"
#include "System/RenderEntryPoint.h"

RAPTOR_NAMESPACE

struct RAPTOR_WRAPPER_API CRaptorConfig_t
{
	//! Initial log file
	char	*m_logFile;

	//! This boolean set to true ( default ) indicates Raptor to destroy all Raptor objects when
	//! application exit request is detected. Is set to false, the user needs to call glQuitRaptor
	bool	m_bAutoDestroy;

	//!
	//!	Use geometry rellocation to use fast AGP buffers. It must be called before
	//!	data allocation or copy. Returns false if rellocation is not possible.
	//! CAUTION : user data acces is much slower when relocated because it needs data copy.
	//!
	bool	m_bRelocation;

	//!
	//! Use Raptor Compute interface to openCL in Raptor Engine.
	//!
	bool	m_bCompute;

	//!
	//! Initialise Raptor Vulkan interface in Raptor Engine.
	//!
	bool	m_bVulkan;

	//!
	//!	Reserve memory space at global level for geometries.
	//!	( by default, only 1000 polygons and 4000 vertices are reserved when Raptor is initialized )
	//!
	size_t	m_uiPolygons;
	size_t	m_uiVertices;

	//!
	//!	Reserve memory space at global level for pixel transfer
	//!	( texture loads, buffer reads, ... by default initialized to 16384 texels)
	//!
	size_t	m_uiTexels;

	//!
	//!	Reserve memory space at global level for uniform buffers
	//!	( by default initialized to 1024 bytes)
	//!
	size_t	m_uiUniforms;

	//!
	//!	Reserve memory space for garbaged memory blocs.
	//!	( by default, initialised to 1Mb )
	//!
	size_t	m_uiGarbageSize;

	//!
	//! This factor multiplies the size of the filtered displays to obtain more quality, allowing FSAA and other features.
	//!
	float   m_fSizeFactor;
} SRaptorConfig;


struct RAPTOR_WRAPPER_API CRaptorDisplayConfig_t
{
	int				x;
	int				y;
	int				width;
	int				height;
	bool			draw_logo;
	bool			status_bar;
	unsigned int	display_mode;
	CRaptorDisplayConfig::GL_ACCELERATION		acceleration;
	CRaptorDisplayConfig::GL_ANTIALIAS			antialias;
	CRaptorDisplayConfig::GL_SWAPBUFFER			swap_buffer;
	CRaptorDisplayConfig::GL_DISPLAY_RENDERER	renderer;
	bool			bind_to_texture;
	bool			overlay;
	bool			double_buffer;
	bool			depth_buffer;
	bool			stencil_buffer;
	bool			accumulator_buffer;
	unsigned char	fps;
	bool			sync_to_monitor;
	char*			caption;
} SRaptorDisplayConfig;


extern "C"
bool RAPTOR_WRAPPER_API glInitRaptor(CRaptorConfig_t &config);

extern "C"
bool RAPTOR_WRAPPER_API glQuitRaptor(void);

extern "C"
bool RAPTOR_WRAPPER_API glCreateDisplay(CRaptorDisplayConfig_t &config, CRaptorDisplay* &display);

extern "C"
bool RAPTOR_WRAPPER_API glDestroyDisplay(CRaptorDisplay* display, HDC dc);

extern "C"
bool RAPTOR_WRAPPER_API glBindDisplay(CRaptorDisplay* display, HDC dc);

extern "C"
bool RAPTOR_WRAPPER_API glUnBindDisplay(CRaptorDisplay* display);

extern "C"
bool RAPTOR_WRAPPER_API glRender(void);

extern "C"
bool RAPTOR_WRAPPER_API glCreateRaptorRenderer(CRaptorDisplay* display, CRenderEntryPoint* &renderer);

extern "C"
bool RAPTOR_WRAPPER_API glDiag(void);