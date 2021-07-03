/***************************************************************************/
/*                                                                         */
/*  Win32Application.cpp                                                   */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)
    #include "Win32Application.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif



RAPTOR_NAMESPACE

bool CWin32Application::destroyWindow = true;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWin32Application::CWin32Application():
m_VMouseXPos(0), m_VMouseYPos(0), m_bGrab(true)
{
}

CWin32Application::~CWin32Application()
{
}

void CWin32Application::grabCursor(bool grab)
{
	m_bGrab = grab;
}

bool CWin32Application::run(void)
{
    if (m_root.handle() == NULL)
    {
        RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
						"RaptorApplication has no root window !.");
        return false;
    }

	HWND wnd = getRootWindow().ptr<HWND__>();
	SetForegroundWindow(wnd);	// Slightly Higher Priority
	SetFocus(wnd);
	
	RECT rect;
	GetWindowRect(wnd,&rect);
	LONG xCenter = 0.5*(rect.left + rect.right);
	LONG yCenter = 0.5*(rect.bottom + rect.top);
	
	if (m_bGrab)
	{
		ShowCursor(FALSE);

		m_VMouseXPos = xCenter;
		m_VMouseYPos = yCenter; 
		SetCursorPos(m_VMouseXPos,m_VMouseYPos);
	}

	m_bRunning = true;

    MSG msg;
	bool exitMainloop = false;
    while(!exitMainloop)
    {
        // test if there is a message in queue, if so get it
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        { 
            // test if this is a quit
            if (msg.message == WM_QUIT)
                exitMainloop = true;

            // translate any accelerator keys
            TranslateMessage(&msg);

            // send the message to the window proc
            DispatchMessage(&msg);
        }

		if (!exitMainloop)
		{
			asyncWindowProc(xCenter, yCenter);
			Raptor::glRender();
		}
    } // end while

	if (m_bGrab)
		ShowCursor(TRUE);

	m_bRunning = false;
    return true;
}

void CWin32Application::asyncWindowProc(LONG xCenter,LONG yCenter)
{
	int mouseButton = 0;
	BYTE lpKeyState[256];
	if (0 != GetKeyboardState(lpKeyState))
	{
		bool shift = (0 != (lpKeyState[VK_SHIFT] & 0x80));
		bool ctrl = (0 != (lpKeyState[VK_CONTROL] & 0x80));

		for (unsigned int i=0;i<256;i++)
		{
			if (lpKeyState[i] & 0x80)
			{
				UINT code = MapVirtualKey(i,MAPVK_VK_TO_CHAR);
				if (code != 0)
				{
					char key = (char)(code & 0xff);
					if ((i >= 'A') &&	// VK_A
						(i <= 'Z'))		// VK_Z
						if (!shift) key = key - 'A' + 'a';
					CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(key);
				}
				else if (i == VK_LBUTTON)
					mouseButton = 1;
				else if (i == VK_MBUTTON)
					mouseButton = 2;
				else if (i == VK_RBUTTON)
					mouseButton = 3;
				else if (i == VK_XBUTTON1)
					mouseButton = 4;
				else if (i == VK_XBUTTON1)
					mouseButton = 5;
				else
				{
					CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(i);
				}
			}
		}
	}
/*
	Implement a configuration based keyboard mask to avoid full buffered read

	if (0 != GetAsyncKeyState(VK_LEFT))
		CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(VK_LEFT);
	else if (0 != GetAsyncKeyState(VK_RIGHT))
		CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(VK_RIGHT);
	else if (0 != GetAsyncKeyState(VK_UP))
		CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(VK_UP);
	else if (0 != GetAsyncKeyState(VK_DOWN))
		CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(VK_DOWN);
*/
	POINT p;
	GetCursorPos(&p);
	if ((p.x != xCenter) || (p.y != yCenter))
	{
		m_VMouseXPos += (p.x - xCenter);
		m_VMouseYPos += (p.y - yCenter);

		CRaptorConsole::CInputCollectorBase::broadcastMouseInput(mouseButton,m_VMouseXPos,m_VMouseYPos);
		if (m_bGrab)
			SetCursorPos(xCenter,yCenter);
	}
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	CRaptorApplication *pApp = CRaptorApplication::GetInstance();
	if (NULL == pApp)
		return (DefWindowProc(hwnd, msg, wparam, lparam));
	if (!pApp->isRunning())
		return (DefWindowProc(hwnd, msg, wparam, lparam));

    switch(msg)
	{
		case WM_PAINT:
		{
			Raptor::glRender();
			return 0;
			break;
		}
        case WM_ERASEBKGND:
        {
			Raptor::glRender();
            return 0;
            break;
        }
		case WM_CHAR:
		{
			Raptor::GetConsole()->handleInput(wparam);
			return 0;
			break;
		}
		case WM_CLOSE:
	    case WM_DESTROY:
		{
		    // kill the application, this sends a WM_QUIT message 
            if (pApp == NULL)
                return (DefWindowProc(hwnd, msg, wparam, lparam));

			if (pApp->getRootWindow().ptr<HWND__>() == hwnd)
			{
				//! Temporary !
				CWin32Application::destroyWindow = false;

				//! TODO: Destroy display and resources here ! 
				pApp->quitApplication();
				//! This is the last point where device context is still usable.
				PostQuitMessage(0);
			}

		    return(0);
            break;
		}
		/*
		case WM_INPUT: 
		{
			UINT dwSize = 0;
			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL) 
				return 0;

			if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
				 return 0;

			RAWINPUT* raw = (RAWINPUT*)lpb;
		
			if (raw->header.dwType == RIM_TYPEKEYBOARD) 
			{
				if ((raw->data.keyboard.Flags & RI_KEY_BREAK) == RI_KEY_BREAK)
				{
					UINT code = MapVirtualKey(raw->data.keyboard.VKey,MAPVK_VK_TO_CHAR);
					if (code != 0)
					{
						bool shift = false;
						char key = (char)(code & 0xff);
						if ((raw->data.keyboard.VKey >= 'A') && //VK_A) && 
							(raw->data.keyboard.VKey <= 'Z')) //VK_Z
							if (!shift) key = key - 'A' + 'a';
						CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(key);
					}
				}
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE) 
			{
				if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) == RI_MOUSE_LEFT_BUTTON_DOWN)
					CRaptorConsole::CInputCollectorBase::broadcastCharacterInput('c');
				if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) == RI_MOUSE_RIGHT_BUTTON_DOWN)
					CRaptorConsole::CInputCollectorBase::broadcastCharacterInput('C');
			} 

			delete[] lpb; 
			return 0;
		}
		*/
	    default:
            break;
    } // end switch

    // process any messages that we didn't take care of 
    return (DefWindowProc(hwnd, msg, wparam, lparam));
} // end WinProc

CWin32Application::windowProc CWin32Application::getWindowProc(void)
{
	return &WindowProc;
}


/*
UINT nDevices = 0;
PRAWINPUTDEVICELIST pRawInputDeviceList;
if (res)
res &= (0 == GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)));

if (res)
res &= (NULL != (pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)));

if (res)
res &= (nDevices == GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)));

if (res)
{
RAWINPUTDEVICE Rid[4];

Rid[0].usUsagePage = 0x01;
Rid[0].usUsage = 0x05;
Rid[0].dwFlags = 0;                 // adds game pad
Rid[0].hwndTarget = 0;

Rid[1].usUsagePage = 0x01;
Rid[1].usUsage = 0x04;
Rid[1].dwFlags = 0;                 // adds joystick
Rid[1].hwndTarget = 0;

// This would support multi-mouse input, but
// still there is keyboard latency.
// May be enabled in future versions.
Rid[2].usUsagePage = 0x01;
Rid[2].usUsage = 0x02;
Rid[2].dwFlags = 0;//RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
Rid[2].hwndTarget = 0;

Rid[3].usUsagePage = 0x01;
Rid[3].usUsage = 0x06;
Rid[3].dwFlags = 0;//RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
Rid[3].hwndTarget = 0;

res = (TRUE == RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])));
}

// after the job, free the RAWINPUTDEVICELIST
free(pRawInputDeviceList);
*/
