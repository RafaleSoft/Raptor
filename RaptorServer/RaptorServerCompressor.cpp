/***************************************************************************/
/*                                                                         */
/*  RaptorServerCompressor.cpp                                             */
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


#include "stdafx.h"
#include "RaptorServerCompressor.h"

#ifdef LINUX
	#include <string.h>	// for memset
#endif

#include "System/Image.h"
RAPTOR_NAMESPACE


CRaptorServerCompressor::~CRaptorServerCompressor() 
{
	if (m_pData != NULL)
		delete [] m_pData;
}

bool CRaptorServerCompressor::differentialCompress(unsigned char* data,size_t size)
{
	bool ret = false;

	if (m_size != size)
	{
		if (m_pData != NULL)
		{
			delete [] m_pData;
			m_pData = NULL;
		}

	}

	if (m_pData == NULL)
	{
		m_pData = new unsigned char[size];
		m_size = size;
		memset(m_pData,0,size);
	}

	if (data != NULL)
	{
		unsigned int previous = 0;
		unsigned char tmp = 0;
		for (size_t i=0;i<size;i+=4)
		{
			previous = *((unsigned int*)(&data[i]));

			data[i]	  = data[i]	  - m_pData[i];
			data[i+1] = data[i+1] - m_pData[i+1];
			data[i+2] = data[i+2] - m_pData[i+2];

			*((unsigned int*)(&m_pData[i])) = previous;
		}

		//ret = doCompress(data,size);
		//memcpy(m_pData,data,size);
		ret = true;
	}

	return ret;
}

void CRaptorServerCompressor::compress(unsigned char* data,size_t size)
{
	doCompress(data,size);
}

void CRaptorServerCompressor::makeBlocs(unsigned char* data,size_t &size)
{
	size = sizeof(CRaptorNetwork::BLOC_HEADER) * (size / 4) * CRaptorNetwork::PIXEL_SIZE;
}


void CRaptorServerCompressor::removeAlpha(unsigned char* data,size_t size)
{
	int pos = 0;
	for (size_t i=0; i<size; i+=4,pos+=3)
	{
		data[pos + 0] = data[i + 0];
		data[pos + 1] = data[i + 1];
		data[pos + 2] = data[i + 2];
	}
}