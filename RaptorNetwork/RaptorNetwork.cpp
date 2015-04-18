// RaptorNetwork.cpp : définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"
#include "RaptorNetwork.h"


const int CRaptorNetwork::PIXEL_SIZE = 3;
const int COMMAND_HEADER = sizeof(CRaptorNetwork::SERVER_COMMAND);

static CRaptorNetwork::SERVER_COMMAND RENDER_COMMAND =
{
	strlen("Render"),
	0,
	sizeof(CRaptorNetwork::SERVER_COMMAND),
	"Render"
};

static CRaptorNetwork::SERVER_COMMAND STATUS_COMMAND =
{
	strlen("Status"),
	0,
	sizeof(CRaptorNetwork::SERVER_COMMAND),
	"Status"
};

static CRaptorNetwork::SESSION_COMMAND OPEN_SESSION_COMMAND =
{
	{
		strlen("OpenSession"),
		0,
		sizeof(CRaptorNetwork::SESSION_COMMAND),
		"OpenSession"
	},
	0,
	0
};

CRaptorNetwork::CRaptorNetwork()
{

}

CRaptorNetwork::~CRaptorNetwork()
{

}

const CRaptorNetwork::SERVER_COMMAND& CRaptorNetwork::getRenderCommand(void)
{
	return RENDER_COMMAND;
}

const CRaptorNetwork::SERVER_COMMAND& CRaptorNetwork::getStatusCommand(void)
{
	return STATUS_COMMAND;
}

const CRaptorNetwork::SESSION_COMMAND& CRaptorNetwork::getOpenSessionCommand(void)
{
	return OPEN_SESSION_COMMAND;
}
