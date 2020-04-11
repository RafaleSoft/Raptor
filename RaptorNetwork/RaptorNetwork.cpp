// RaptorNetwork.cpp : définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"
#include "RaptorNetwork.h"

const int CRaptorNetwork::PIXEL_SIZE = 3;

static CRaptorNetwork::SERVER_COMMAND RENDER_COMMAND =
{
	(uint8_t)strlen("Render"),
	0,
	sizeof(CRaptorNetwork::SERVER_COMMAND),
	"Render"
};

static CRaptorNetwork::SERVER_COMMAND STATUS_COMMAND =
{
	(uint8_t)strlen("Status"),
	0,
	sizeof(CRaptorNetwork::SERVER_COMMAND),
	"Status"
};

static CRaptorNetwork::SESSION_COMMAND OPEN_SESSION_COMMAND =
{
	{
		(uint8_t)strlen("OpenSession"),
		0,
		sizeof(CRaptorNetwork::SESSION_COMMAND),
		"OpenSession"
	},
	0,
	0
};

static CRaptorNetwork::DATA_COMMAND DATA_PACKAGE_COMMAND =
{
	{
		(uint8_t)strlen("DataPackage"),
		0,
		sizeof(CRaptorNetwork::DATA_COMMAND),
		"DataPackage"
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

const CRaptorNetwork::DATA_COMMAND& CRaptorNetwork::getDataPackageCommand(void)
{
	return DATA_PACKAGE_COMMAND;
}
