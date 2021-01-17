/***************************************************************************/
/*                                                                         */
/*  RaptorNetwork.h                                                        */
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

#ifndef __RAPTOR_PORTABILITY_H__
	#include "Portability.h"
#endif

#if !defined(AFX_RAPTORNETWORK_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
#define AFX_RAPTORNETWORK_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_

#if defined(_WIN32)
	#ifdef EXPORT_RAPTOR_NETWORK
		#define RAPTORNETWORK_API __declspec(dllexport)
	#else
		#define RAPTORNETWORK_API __declspec(dllimport)
	#endif
	#if _MSC_VER > 1200     // 1200 is Visual C++ 6.0
		#define RAPTOR_TYPENAME typename
	#else
		#define RAPTOR_TYPENAME
	#endif
	#define CLOSESOCKET ::closesocket
#else	// Linux environment
	#define RAPTORNETWORK_API
	#define RAPTOR_TYPENAME typename
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define SD_BOTH SHUT_RDWR
	#define CLOSESOCKET ::close
#endif


class RAPTORNETWORK_API CRaptorNetwork 
{
public:
	static const int PIXEL_SIZE;

	typedef struct BLOC_HEADER_t
	{
		uint16_t blocWidth;
		uint16_t blocHeight;
		uint16_t xOffset;
		uint16_t yOffset;
		uint16_t compressionType;
	} BLOC_HEADER;

	typedef struct SERVER_COMMAND_t
	{
		uint8_t	commandLen;
		uint8_t	replyLen;
		uint8_t	requestLen;
		// 16 chars length + 0 to end string ==> align structure to DWORD.
		char	command[17];
	} SERVER_COMMAND;

	typedef struct IMAGE_COMMAND_t
	{
		SERVER_COMMAND	command;
		BLOC_HEADER		header;
		uint8_t			*pData;
	} IMAGE_COMMAND;

	typedef struct SESSION_COMMAND_t
	{
		SERVER_COMMAND	command;
		uint16_t		width;
		uint16_t		height;
		void			*id;
	} SESSION_COMMAND;

	typedef struct DATA_COMMAND_t
	{
		SERVER_COMMAND	command;
		uint32_t		size;
		uint8_t			packnameLen;
		// 32 chars length + 0 to end string ==> align structure to DWORD.
		char			packname[33];
	} DATA_COMMAND;

	static const CRaptorNetwork::SERVER_COMMAND& getRenderCommand(void);
	static const CRaptorNetwork::SERVER_COMMAND& getStatusCommand(void);
	static const CRaptorNetwork::SESSION_COMMAND& getOpenSessionCommand(void);
	static const CRaptorNetwork::DATA_COMMAND& getDataPackageCommand(void);
	static const CRaptorNetwork::IMAGE_COMMAND& getImageCommand(void);

private:
	CRaptorNetwork(void);
	virtual ~CRaptorNetwork(void);
};

#endif	// AFX_RAPTORNETWORK_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_

