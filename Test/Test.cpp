/***************************************************************************/
/*                                                                         */
/*  Test.cpp                                                               */
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


#include "StdAfx.h"

#include "TestDoc.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"

#include "ToolBox/Imaging.h"
#include "ToolBox/Controllers.h"
#include "ToolBox/Filters.h"
#include <sstream>

RAPTOR_NAMESPACE

int main(int argc, char* argv[])
{
    CRaptorConfig config;
    config.m_bRelocation = true;
    config.m_uiPolygons = 10000;
    config.m_uiVertices = 40000;
	config.m_logFile = "Test_Raptor.log";

    IRaptor::glInitRaptor(config);

    CImaging::installImagers();


    unsigned long v = IRaptor::GetVersion();
	stringstream title;
    title << "Your empty starting Raptor ";
    title << ((v>>24)&0xFF) << "." << ((v>>16)&0xFF) << "." << ((v>>8)&0xFF);
    title << " Release test";

	CRaptorDisplayConfig glcs;
	glcs.width = 1024;
	glcs.height = 768;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title.str();
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_4X;
	glcs.renderer = CRaptorDisplayConfig::RENDER_BUFFER_FILTER_CHAIN;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.stencil_buffer = true;
	glcs.display_mode = CGL_FLOAT_16 | CGL_DEPTH;
	//glcs.refresh_rate.sync_to_monitor = true;
 
    CRaptorApplication  *app = CRaptorApplication::CreateApplication();
	if (!app->initApplication(glcs))
    {
        IRaptor::GetMessages()->displayMessage("Sorry: Test cannot run : full display config is not supported, trying basic window...");
        glcs.display_mode = CGL_RGBA| CGL_DEPTH;
		if (!app->initApplication(glcs))
        {
            IRaptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL rendering not supported, exiting...");
            return -1;
        }
    }

	CRaptorDisplay *pDisplay = app->getRootDisplay();
    CTestDoc *pDoc = new CTestDoc(app->getRootWindow(),pDisplay);

	bool res = pDisplay->glvkBindDisplay(app->getRootWindow());
    if (res)
	{
		CFilters::glInstallFilters();

        pDoc->GLInitContext();

        CRaptorConsole *pConsole = IRaptor::GetConsole();
        pConsole->glInit();
        pConsole->showStatus(true);
        pConsole->activateConsole(true);

		pDisplay->glvkUnBindDisplay();
	}

	app->grabCursor(false);
    app->run();

	app->quitApplication();

    return 0;
}

#ifdef WIN32
int WINAPI WinMain(	HINSTANCE hInstance,  // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
                    LPSTR lpCmdLine,      // pointer to command line
                    int nCmdShow          // show state of window
)
{
    int res = main(0,NULL);
    return res;
}
#endif
