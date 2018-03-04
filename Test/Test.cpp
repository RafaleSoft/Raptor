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
    config.m_uiVertices = 40000;
	config.m_logFile = "Test_Raptor.log";

    Raptor::glInitRaptor(config);

    CImaging::installImagers();

    CRaptorApplication  *app = CRaptorApplication::CreateApplication();
    app->initApplication();

    unsigned long v = Raptor::GetVersion();
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
 
    CRaptorDisplay *pDisplay = NULL;
    RAPTOR_HANDLE wnd = Raptor::glCreateWindow(glcs,pDisplay);

    if (wnd.handle == 0)
    {
        Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : full display config is not supported, trying basic window...");
        glcs.display_mode = CGL_RGBA| CGL_DEPTH;
        wnd = Raptor::glCreateWindow(glcs,pDisplay);
        if (wnd.handle == 0)
        {
            Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL rendering not supported, exiting...");
            return -1;
        }
    }

    app->setRootWindow(wnd);

    CTestDoc *pDoc = new CTestDoc(wnd,pDisplay);

    bool res = pDisplay->glBindDisplay(wnd);
    if (res)
	{
        pDoc->GLInitContext();

        CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->glInit();
        pConsole->showStatus(true);
        pConsole->activateConsole(true);

		//CControllers::createViewpointController(pDisplay->getViewPoint());

		pDisplay->glUnBindDisplay();
	}
	app->grabCursor(false);
    app->run();

    //delete pDoc;

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
