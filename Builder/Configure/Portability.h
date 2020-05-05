/***************************************************************************/
/*                                                                         */
/*  Portability.h                                                          */
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

#pragma once


#ifndef __RAPTOR_PORTABILITY_H__
#define __RAPTOR_PORTABILITY_H__

#if defined(WIN32)
	#define LOCALTIME(_Tm,_Time) localtime_s(_Tm,_Time)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_s(_Buf, _SizeInBytes, _Tm)
	#define __FILENAME__(file) (strrchr(file,'\\') ? strrchr(file,'\\') + 1 : file)
#elif defined(LINUX)
	#define LOCALTIME(_Tm,_Time) localtime_r(_Time,_Tm)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_r(_Tm, _Buf)
	#define __FILENAME__(file) (strrchr(file,'/') ? strrchr(file,'/') + 1 : file)
#endif

#endif
