// Demo.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"

#include "ToolBox/Imaging.h"
#include "ToolBox/Controllers.h"
#include "ToolBox/ParticleCompute.h"

#include "DemoDoc.h"


int main(int argc, char* argv[])
{
    CRaptorConfig config;
	config.m_logFile = "Demo_Raptor.log";
    config.m_bRelocation = true;
    config.m_uiPolygons = 1000000;
    config.m_uiVertices = 2000000;
	config.m_uiTexels = 2000000;

	CRaptorDisplayConfig glcs;
	glcs.width = 1024;
	glcs.height = 768;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = "Raptor Demo";
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_4X;
	glcs.swap_buffer = CRaptorDisplayConfig::SWAP_EXCHANGE;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.stencil_buffer = true;
    glcs.display_mode = CGL_FLOAT_16 |//*/ CGL_RGBA | 
						CGL_DEPTH | 
						CGL_RENDER_FILTERED |
						CGL_RENDER_BUFFER;
	glcs.draw_logo = true;
	glcs.lightingState.lightModelSeparateSpecular = true;

    //	initialize Raptor classes and settings
	Raptor::glInitRaptor(config);

	CImaging::installImagers(CTextureFactory::getDefaultFactory());
	CParticleCompute::installComputers();

    if (!Raptor::glCheckDisplayConfig(glcs))
    {
        Raptor::GetMessages()->displayMessage("Some hardware features are missing. Will use lower config, disabling some effects");
        glcs.display_mode = CGL_RGBA | CGL_DEPTH | CGL_RENDER_FILTERED ;
        if (!Raptor::glCheckDisplayConfig(glcs))
        {
            Raptor::GetMessages()->displayMessage("Some hardware features are missing. Will use minimal config, disabling all advanced effects");
            glcs.display_mode = CGL_RGBA | CGL_DEPTH;
            if (!Raptor::glCheckDisplayConfig(glcs))
            {
                Raptor::GetMessages()->displayMessage("Minimum required display config cannot be created. Sorry, demo will abort. Bye.");
                return FALSE;
            }
        }
    }

	CRaptorApplication  *app = CRaptorApplication::CreateApplication();
    app->initApplication();

	CRaptorDisplay *pDisplay = NULL;
    RAPTOR_HANDLE wnd = Raptor::glCreateWindow(glcs,pDisplay);

    app->setRootWindow(wnd);

    CDemoDoc *pDoc = new CDemoDoc(wnd,pDisplay);

    bool res = pDisplay->glBindDisplay(wnd);
    if (res)
	{
        pDoc->GLInitContext();

		CControllers::createViewpointController(pDisplay->getViewPoint());

		pDisplay->glUnBindDisplay();
	}

    app->run();

	char msg[1024];
	float avgfps = ((float)(1000 * (pDoc->nbFrames-10))) / (pDoc->finishTime - pDoc->startTime);
	sprintf(msg,"Bench results :\nMin fps : %f\nMax fps : %f\nAvg fps : %f",pDoc->minfps,pDoc->maxfps,avgfps);
	Raptor::GetMessages()->displayMessage(msg);

    Raptor::glQuitRaptor();

	return 0;
}

#ifdef WIN32
int WINAPI WinMain(	HINSTANCE hInstance,  // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
                    LPSTR lpCmdLine,      // pointer to command line
                    int nCmdShow          // show state of window
)
{
    int res = main(__argc,__argv);
    return res;
}
#endif

