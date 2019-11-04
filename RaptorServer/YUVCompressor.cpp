/***************************************************************************/
/*                                                                         */
/*  YUVCompressor.cpp                                                      */
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
#include "YUVCompressor.h"

#include "System/CGLTypes.h"

RAPTOR_NAMESPACE


CYUVCompressor::CYUVCompressor(void)
{
}

CYUVCompressor::~CYUVCompressor(void)
{
}

bool CYUVCompressor::doCompress(unsigned char* data,size_t size)
{
	//unsigned char *dst = (unsigned char*)(getData());
	unsigned int nbDiffs = 0;
	int maxred = 0;
	int maxgreen = 0;
	int maxblue = 0;

	int maxy = 0;
	int maxu = 0;
	int maxv = 0;
	int miny = 0;
	int minu = 0;
	int minv = 0;

	for (size_t i=0;i<size;i+=4)
	{
		int red = (int)data[i];// - (int)dst[i];
		int green = (int)data[i+1];// - (int)dst[i+1];
		int blue = (int)data[i+2];// - (int)dst[i+2];

		CColor::RGBA color(red,green,blue,1.0f);
		//CColor::RGBA color(data[i],data[i+1],data[i+2],1.0f);
		CColor::YUVA yuv = color;

		int y = yuv.y;
		int u = yuv.u;
		int v = yuv.v;

		int delta = y + u + v;
		if (delta != 0)
			nbDiffs++;

		if (y > maxy)
			maxy = y;
		else if (y < miny)
			miny = y;
		if (u > maxu)
			maxu = u;
		else if (u < minu)
			minu = u;
		if (v > maxv)
			maxv = v;
		else if (v < minv)
			minv = v;

		data[i] = y;
		data[i+1] = u;
		data[i+2] = v;
	}

	return false;
}

