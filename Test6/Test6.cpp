// Test.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#include "TestDoc.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"

#include "ToolBox/Imaging.h"
#include "ToolBox/Controllers.h"
#include <sstream>

RAPTOR_NAMESPACE

int main(int argc, char* argv[])
{
    CRaptorConfig config;
    config.m_bRelocation = true;
    config.m_uiPolygons = 10000;
    config.m_uiVertices = 100000;
	config.m_logFile = "Test6_Raptor.log";

    Raptor::glInitRaptor(config);

    CImaging::installImagers();


    unsigned long v = Raptor::GetVersion();
	stringstream title;
    title << "Your empty starting Raptor ";
    title << ((v>>24)&0xFF) << "." << ((v>>16)&0xFF) << "." << ((v>>8)&0xFF);
    title << " Release test6";

	CRaptorDisplayConfig glcs;
	glcs.width = 1024;
	glcs.height = 768;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title.str();
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_4X;
	glcs.renderer = CRaptorDisplayConfig::NATIVE_GL;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.stencil_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;
	glcs.draw_logo = true;
	//glcs.refresh_rate.sync_to_monitor = true;
 
	CRaptorApplication  *app = CRaptorApplication::CreateApplication();
	if (!app->initApplication(glcs))
	{
		Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : full display config is not supported, trying basic window...");
		return -1;
	}

	CRaptorDisplay *pDisplay = app->getRootDisplay();
    CTestDoc *pDoc = new CTestDoc(app->getRootWindow(), pDisplay);

	bool res = pDisplay->glvkBindDisplay(app->getRootWindow());
    if (res)
	{
        pDoc->GLInitContext();

        CRaptorConsole *pConsole = Raptor::GetConsole();
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
