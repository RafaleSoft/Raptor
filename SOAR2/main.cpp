#include "StdAfx.h"
#include "SOARDoc.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"
#include "Engine/ViewPoint.h"
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
    Raptor::glInitRaptor(config);

    CRaptorApplication  *app = CRaptorApplication::CreateApplication();
    app->initApplication();

	string logname = "SOAR2.log";
    Raptor::GetErrorManager()->logToFile(logname);

    unsigned long v = Raptor::GetVersion();
	ostrstream title;
    title << "SOAR 2 Raptor integration";
	title << Raptor::GetVersionString();
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
 
    CRaptorDisplay *pDisplay = NULL;
    RAPTOR_HANDLE wnd = Raptor::glCreateWindow(glcs,pDisplay);

    if (wnd.handle == 0)
    {
		Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL rendering not supported, exiting...");
        return -1;
    }

    app->setRootWindow(wnd);

    CSOARDoc *pDoc = new CSOARDoc(wnd,pDisplay);

    bool res = pDisplay->glBindDisplay(wnd);
    if (res)
	{
        pDoc->GLInitContext(argc,argv);

        CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->glInit();
        pConsole->showStatus(true);
        pConsole->activateConsole(true);
		//pConsole->setColor(GL_RGBA_COLOR(1.0f,1.0f,1.0f,1.0f));

		pDisplay->glUnBindDisplay();
	}

    app->run();

    //delete pDoc;

    return 0;
}

