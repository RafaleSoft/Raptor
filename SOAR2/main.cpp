#include "StdAfx.h"
#include "SOARDoc.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"
#include "Engine/3DEngine.h"

#include <strstream>
using std::ostrstream;

RAPTOR_NAMESPACE

int main(int argc, char* argv[])
{
    CRaptorConfig config;
    config.m_bRelocation = false;
    config.m_uiPolygons = 100000;
    config.m_uiVertices = 800000;
    IRaptor::glInitRaptor(config);

	string logname = "SOAR2.log";
    IRaptor::GetErrorManager()->logToFile(logname);

    unsigned long v = IRaptor::GetVersion();
	ostrstream title;
    title << "SOAR 2 Raptor integration";
	title << IRaptor::GetVersionString();
    title << " Release test";
    title << ends;

	CRaptorDisplayConfig glcs;
	glcs.width = 640;
	glcs.height = 480;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title.str();
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;
 
    
	CRaptorApplication  *app = CRaptorApplication::CreateApplication();
	if (!app->initApplication(glcs))
    {
		IRaptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL rendering not supported, exiting...");
        return -1;
    }

	CRaptorDisplay *pDisplay = app->getRootDisplay();
	CSOARDoc *pDoc = new CSOARDoc(app->getRootWindow(), pDisplay);

	bool res = pDisplay->glvkBindDisplay(app->getRootWindow());
    if (res)
	{
        pDoc->GLInitContext(argc,argv);

        CRaptorConsole *pConsole = IRaptor::GetConsole();
        pConsole->glInit();
        pConsole->showStatus(true);
        pConsole->activateConsole(true);
		//pConsole->setColor(GL_RGBA_COLOR(1.0f,1.0f,1.0f,1.0f));

		pDisplay->glvkUnBindDisplay();
	}

    app->run();

	app->quitApplication();

    return 0;
}

