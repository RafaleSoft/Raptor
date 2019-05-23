// GLXApplication.cpp: implementation of the CGLXApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_)
    #include "GLXApplication.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_)
    #include "GLXView.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif

#include <stdio.h> // printf

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLXApplication::CGLXApplication()
{

}

CGLXApplication::~CGLXApplication()
{

}

bool CGLXApplication::initApplication()
{
    return CRaptorApplication::initApplication();
}

void CGLXApplication::setRootWindow(const RAPTOR_HANDLE& root)
{
    CRaptorApplication::setRootWindow(root);
}

void CGLXApplication::grabCursor(bool grab)
{

}

typedef struct
{
    int x;
    int y;
} XY;

bool CGLXApplication::run()
{
    if ((m_root.handle == 0) || (m_root.hClass != WINDOW_CLASS))
    {
        Raptor::GetErrorManager()->generateRaptorError(
                Global::COpenGLClassID::GetClassId(),
                CRaptorErrorManager::RAPTOR_ERROR,
                "RaptorApplication has no root window !.");
        return false;
    }

    int done = False;
    int buttonPress = 0;
    XY currentpos;

    CGLXView *pView = (CGLXView*)m_root.handle;
    Display *display = (Display*)(pView->getDisplay()->getCurrentDevice().handle);
    Window window = (Window)(pView->getWindow().handle);

    while (!done)
    {
        XEvent report;
        XNextEvent(display, &report);

        int reDraw = 0;
        unsigned int eventMask = 0;

        cout << "Event type: " << report.type << endl;
        switch (report.type)
        {
            case Expose:
            {
                XWindowAttributes attributes;
                if (XGetWindowAttributes(display,window,&attributes))
                {
                    pView->OnSize(attributes.width,attributes.height);
                    reDraw = 1;
                }
                break;
            }
            case ConfigureNotify:
            {
                reDraw = 1;
                break;
            }
            case MotionNotify:
            {
                XMotionEvent evt = report.xmotion;
                cout << "Pointer move: " << evt.x << " " << evt.y << endl;
                /*
                if (buttonPress == 1)
                {
                    while(XCheckMaskEvent(display, PointerMotionMask, &report));

                    XY tmpPos;
                    tmpPos.x = currentpos.x - evt.x;
                    tmpPos.y = currentpos.y - evt.y;

                    cout << "scroll: " << tmpPos.x << " " << tmpPos.y << endl;

                    currentpos.x = evt.x;
                    currentpos.y = evt.y;
                    reDraw = 1;
                }
                else if (buttonPress == 2)
                {
                    while(XCheckMaskEvent(display, PointerMotionMask, &report));

                    XY tmpPos;
                    XY tmpPos2;
                    tmpPos.x = currentpos.x - 320;
                    tmpPos.y = currentpos.y - 240;
                    tmpPos2.x = evt.x - 320;
                    tmpPos2.y = evt.y - 240;

                    double cs = (tmpPos.x * tmpPos2.x) + (tmpPos.y * tmpPos2.y);
                    double ss = (tmpPos.x * tmpPos2.y) - (tmpPos.y * tmpPos2.x);
                    double n1 = sqrt((double)((tmpPos.x * tmpPos.x) + (tmpPos.y * tmpPos.y)));
                    double n2 = sqrt((double)((tmpPos2.x * tmpPos2.x) + (tmpPos2.y * tmpPos2.y)));

                    cs = cs / ( n1 * n2 );
                    double angle = acos(cs);

                    currentpos.x = evt.x;
                    currentpos.y = evt.y;
                    reDraw = 1;
                }
                else if (buttonPress == 3)
                {
                    while(XCheckMaskEvent(display, PointerMotionMask, &report));

                    XY tmpPos;
                    XY tmpPos2;
                    tmpPos.x = currentpos.x - 320;
                    tmpPos.y = currentpos.y - 240;
                    tmpPos2.x = evt.x - 320;
                    tmpPos2.y = evt.y - 240;

                    double cs = (tmpPos.x * tmpPos2.x) + (tmpPos.y * tmpPos2.y);
                    double ss = (tmpPos.x * tmpPos2.y) - (tmpPos.y * tmpPos2.x);
                    double n1 = sqrt((double)((tmpPos.x * tmpPos.x) + (tmpPos.y * tmpPos.y)));
                    double n2 = sqrt((double)((tmpPos2.x * tmpPos2.x) + (tmpPos2.y * tmpPos2.y)));

                    cs = cs / ( n1 * n2 );
                    double angle = acos(cs);

                    currentpos.x = evt.x;
                    currentpos.y = evt.y;
                    reDraw = 1;
                }
                */
                break;
            }

            case KeyRelease:
            {
                XKeyEvent evt = report.xkey;
                cout << "Key Press:" << evt.keycode << endl;
                
                CRaptorConsole *pConsole = Global::GetInstance().getConsole();
                if (pConsole != NULL)
                {
                    if (evt.keycode == 49)
                        pConsole->activateConsole(!pConsole->isActive());
                    else
                        pConsole->handleInput(evt.keycode);
                }

                reDraw = 1;
                break;
            }

            case KeyPress:
            {
                XKeyEvent evt = report.xkey;
                //cout << "Key Press." << endl;
                break;
            }

            case ButtonPress:
            {
                XButtonEvent evt = report.xbutton;
                currentpos.x = evt.x;
                currentpos.y = evt.y;
                buttonPress = evt.button;
                cout << "ButtonPress: " << evt.x << " " << evt.y << " " << evt.button << endl;
                break;
            }

            case ButtonRelease:
            {
                buttonPress = 0;
                break;
            }

            case ClientMessage:
            {
                done = True;
                break;
            }

            default:
                printf("XEvent non traite: %d\n", report.type); 
                break;
        }

        if (reDraw > 0)
        {
            Raptor::glRender();
        }
    }

    return true;
}

