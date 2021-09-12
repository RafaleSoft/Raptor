#include "StdAfx.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorApplication.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorConsole.h"
#include "Engine/Animator.h"
#include "Engine/TimeObject.h"

RAPTOR_NAMESPACE

#include "ToolBox/Imaging.h"
#include "ToolBox/Streaming.h"
#include <sstream>
#include "TestDoc.h"



CRaptorDisplay *pDisplay = NULL;
CTestDoc *pDoc = NULL;

long lastx = 0;
long lasty = 0;

LRESULT CALLBACK WindowProc(  HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
	{	
        case WM_ERASEBKGND:
        {
            return 0;
            break;
        }

        case WM_LBUTTONUP:
        {
            pDoc->highLight(false);

            return 0;
            break;
        }

        case WM_LBUTTONDOWN:
        {
            lastx = LOWORD(lparam); 
            lasty = HIWORD(lparam);

			//	A translate of 0.0f will stop the menu
			pDoc->translateMenu(0.0f);
            pDoc->highLight(true);

            return 0;
            break;
        }

        case WM_CHAR:
        {
			CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(wparam);

            return 0;
            break;
        }

        case WM_MOUSEMOVE:
        {
            if ((wparam & MK_LBUTTON) == MK_LBUTTON)
            {
                long xPos = LOWORD(lparam); 
                long yPos = HIWORD(lparam);

                pDoc->translateMenu(0.0002f * (xPos - lastx));
                lasty = yPos;
                lastx = xPos;
            }
            return 0;
            break;
        }

	    case WM_CREATE: 
        {
            CRaptorDisplayConfig glcs;
	        glcs.width = 1024;
	        glcs.height = 768;
	        glcs.x = 0;
	        glcs.y = 0;
	        glcs.caption = "Raptor";
			glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
			glcs.renderer = CRaptorDisplayConfig::PIXEL_BUFFER_FILTER_CHAIN;
			glcs.double_buffer = true;
			glcs.depth_buffer = true;
	        glcs.display_mode = CGL_FLOAT_32 /*CGL_RGBA*/ | CGL_DEPTH;
			glcs.draw_logo = true;
            pDisplay = IRaptor::glCreateDisplay(glcs);

            if (pDisplay == 0)
            {
                IRaptor::GetMessages()->displayMessage("Sorry: Test cannot run : a display for this window cannot be created...");
                return -1;
            }
            else
            {
                // get a graphics context
                HDC hdc = GetDC(hwnd);

                RAPTOR_HANDLE display;
	            display.ptr(hdc);
	            display.hClass(CLIENT_HANDLE_CLASS);
				if (pDisplay->glvkBindDisplay(display))
	            {
                    CRaptorConsole *pConsole = IRaptor::GetConsole();
                    pConsole->glInit();
                    pConsole->showStatus(true);
					pConsole->showFrameTime(true);
                    pConsole->activateConsole(true);

					pDisplay->glvkUnBindDisplay();

                    pDoc = new CTestDoc(hwnd,pDisplay);
                    pDoc->GLInitContext(hdc);

                    // release the dc back
                    ReleaseDC(hwnd, hdc);
	            }
                else
                {
                    IRaptor::GetMessages()->displayMessage("Sorry: Test cannot run : full hardware OpenGL rendering not supported, exiting...");
                    return -1;
                }
            }

            return 0;
            break;
		}

        case WM_SIZE:
        {
            HDC hdc = GetDC(hwnd);
            unsigned int nWidth = LOWORD(lparam);  // width of client area 
            unsigned int nHeight = HIWORD(lparam); // height of client area 

            RAPTOR_HANDLE display;
	        display.ptr(hdc);
	        display.hClass(CLIENT_HANDLE_CLASS);
			if (pDisplay->glvkBindDisplay(display))
	        {
                pDisplay->glResize(nWidth,nHeight,0,0);
				pDisplay->glvkUnBindDisplay();
	        }

            ReleaseDC(hwnd, hdc);
            return 0;
            break;
        }

	    case WM_PAINT: 
		{
            PAINTSTRUCT	ps;          // used in WM_PAINT
		    
   	        HDC hdc = BeginPaint(hwnd,&ps);
            pDoc->glDisplay(hdc);
            EndPaint(hwnd,&ps);

		    return(0);
            break;
   		} 

	    case WM_DESTROY: 
		{
			HDC hdc = GetDC(hwnd);
			RAPTOR_HANDLE display;
			display.ptr(hdc);
			display.hClass(CLIENT_HANDLE_CLASS);
			if (pDisplay->glvkBindDisplay(display))
			{
				pDisplay->glvkReleaseResources();
				pDisplay->glvkUnBindDisplay();
			}

			IRaptor::glDestroyDisplay(pDisplay);
			pDisplay = NULL;

		    // kill the application, this sends a WM_QUIT message 
		    PostQuitMessage(0);
		    return(0);
            break;
		}

	    default:
            break;
    } // end switch

    // process any messages that we didn't take care of 
    return (DefWindowProc(hwnd, msg, wparam, lparam));
} // end WinProc


#ifdef WIN32
int WINAPI WinMain(  HINSTANCE hinstance,
		             HINSTANCE hprevinstance,
		             LPSTR lpcmdline,
		             int ncmdshow)
{
    CRaptorConfig config;
    config.m_bRelocation = true;
    config.m_uiPolygons = 10000;
    config.m_uiVertices = 64000;
    config.m_uiTexels = 3000000;
	config.m_logFile = "Test4_Raptor.log";
    IRaptor::glInitRaptor(config);

	CTimeObject::setTimeFactor(1.0f);
    CAnimator *pAnimator = new CAnimator();
    CAnimator::SetAnimator(pAnimator);
    CImaging::installImagers();
    CStreaming::installStreamers();


    unsigned long v = IRaptor::GetVersion();
	stringstream title;
    title << "Raptor ";
    title << ((v>>24)&0xFF) << "." << ((v>>16)&0xFF) << "." << ((v>>8)&0xFF);
    title << " Release test";


    WNDCLASSEX   winclass; // this will hold the class we create
    HWND	     hwnd;	 // generic window handle
    MSG		     msg;		 // generic message

    // first fill in the window class stucture
    winclass.cbSize = sizeof(WNDCLASSEX);
    winclass.style	= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    winclass.lpfnWndProc	= WindowProc;               //set event handler
    winclass.cbClsExtra	= 0;                        //reserve data space
    winclass.cbWndExtra	= 0;                        //
    winclass.hInstance	= hinstance;                //set instance of application
    winclass.hIcon		= LoadIcon(NULL, IDI_APPLICATION);     //load program icon
    winclass.hCursor	= LoadCursor(NULL, IDC_ARROW);         //load cursor type
    winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); //set background brush
    winclass.lpszMenuName	= NULL;                                //arrach resource menu
    winclass.lpszClassName	= "Raptor";                   //set Windows class name
    winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

    // register the window class
    if (!RegisterClassEx(&winclass))
	    return(0);

    // create the window
    if (!(hwnd = CreateWindowEx(NULL,                  // extended style
                                "Raptor",     // class
								title.str().c_str(), // title
						        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					 	        0,0,	  // initial x,y
						        1024,768,  // initial width, height
						        NULL,	  // handle to parent 
						        NULL,	  // handle to menu
						        hinstance,// instance of this application
						        NULL)))	// extra creation parms
        return(0);

    // enter main event loop, but this time we use PeekMessage()
    // instead of GetMessage() to retrieve messages
    while(TRUE)
    {
        // test if there is a message in queue, if so get it
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        { 
            // test if this is a quit
            if (msg.message == WM_QUIT)
               break;

            // translate any accelerator keys
            TranslateMessage(&msg);

            // send the message to the window proc
            DispatchMessage(&msg);
        } // end if

        IRaptor::glRender();
    } // end while

    delete pDoc;
    
	DestroyWindow(hwnd);

	if (IRaptor::GetConfig().m_bAutoDestroy)
		IRaptor::glQuitRaptor();

    // return to Windows like this
    return(msg.wParam);

} // end WinMain

#endif

