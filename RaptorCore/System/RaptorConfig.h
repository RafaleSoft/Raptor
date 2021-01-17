/***************************************************************************/
/*                                                                         */
/*  RaptorConfig.h                                                         */
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


#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
#define AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API CRaptorConfig  
{
public:
	CRaptorConfig();
	virtual ~CRaptorConfig();

	//! Initial log file
	string	m_logFile;

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
	//!	( by default, only 1000 polygons and 16000 vertices are reserved when Raptor is initialized )
	//!
    size_t	m_uiPolygons;
    size_t	m_uiVertices;

    //!
    //!	Reserve memory space at global level for pixel transfer
    //!	( texture loads, buffer reads, ... by default initialized to 1 Mega texels)
    //!
    size_t	m_uiTexels;

	//!
	//!	Reserve memory space at global level for uniform buffers
	//!	( by default initialized to 128 Kbytes)
	//!
	size_t	m_uiUniforms;

	//!
	//!	Reserve memory space for garbaged memory blocs.
	//!	( by default, initialised to 1Mb )
	//!
	size_t	m_uiGarbageSize;

    //!
    //! This factor multiplies the size of the filtered displays to obtain more quality, 
	//!	allowing FSAA and other features.
	//!	@param factor: the filter size factor.
    //!
    void setFilterSizeFactor(float factor);

	//!
	//!	Return the filter size factor.
	//!	@return the silter size factor.
    float getFilterSizeFactor(void) const { return m_fSizeFactor; };

	//!
	//!	This method verifies that all values enable Raptor to 
	//!	initialize without errors. Default values are applied 
	//!	when incorrect fields are encountered.
	//!	@return true is all values are valid.
	//!
	bool checkConfig(void);


private:
    float   m_fSizeFactor;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)

