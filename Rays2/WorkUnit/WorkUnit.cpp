// WorkUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WorkUnit.h"
#include "WorkUnitDlg.h"
#include "..\systemdll\systemdll.h"
#include "WUSocket.h"
#include "GenericLight.h"
#include "Quadric.h"
#include "Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWorkUnitApp

BEGIN_MESSAGE_MAP(CWorkUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CWorkUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkUnitApp construction

CWorkUnitApp::CWorkUnitApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

#include "SSE_BoundingBox.h"
#include "BoundingBox.h"

void test()
{
	SSE_CBoundingBox b;
	b.Set(	-118.131f,-97.752f,-414.663f,
			115.294f,78.5945f,-177.914f);

	CGenericRay r;
	r.origin.Set(-3.38269f,-96.201f,-277.395f,1.0f);
	r.direction.Set(0.0745249f,-0.988922f,0.128369f,1.0f);

	CBoundingBox bb;
	bb.Set(	-118.131f,-97.752f,-414.663f,
			115.294f,78.5945f,-177.914f);

	float hit = b.Intersect(r);
}

/////////////////////////////////////////////////////////////////////////////
// Global datas
CWorkUnitApp			theApp;
CClient<CWUSocket>		client;
unsigned int			ip;
unsigned int			ID;
extern RAYTRACERDATA	raytracer_data;
extern UINT				Raytrace( LPVOID pParam );

//	for log/debug
ofstream				COUT;
/////////////////////////////////////////////////////////////////////////////
// CWorkUnitApp initialization

BOOL CWorkUnitApp::InitInstance()
{
//	test();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	Enable3dControls();			// Call this when using MFC in a shared DLL

	//	initialise rendering datas
	raytracer_data.lock = new CEvent(FALSE,FALSE,"Rays lock",NULL);
	raytracer_data.lock->SetEvent();	//	ressource free
	raytracer_data.start = 0;
	raytracer_data.end = 0;
	raytracer_data.percent = 0;
	raytracer_data.camera.focale = 50.0f;	// en millimètres, 50 correspond ~ à la vision humaine 
											// longueur focale ou distance focale ?
	raytracer_data.camera.object_plane = 10.0f;	// plan objet, en mètres
	raytracer_data.camera.aperture = 2.8f;
	raytracer_data.camera.reflection_depth = 5;
	raytracer_data.camera.refraction_depth = 5;
	raytracer_data.camera.origin.Set(0.0,0.0,20.0,1.0);	// centre optique
	raytracer_data.camera.height = 240;
	raytracer_data.camera.width = 320;
	raytracer_data.camera.variance = 5;
	raytracer_data.camera.photon_map = 200000;
	raytracer_data.image = NULL;
	raytracer_data.lights.RemoveAll();
	raytracer_data.objects.RemoveAll();
	raytracer_data.textures.RemoveAll();
	raytracer_data.plugins.RemoveAll();
	raytracer_data.currentFrame = 0;
	raytracer_data.frames.RemoveAll();
	raytracer_data.use_sse = false;
	raytracer_data.use_cmov = false;
	raytracer_data.use_mmx = false;

	//	receive ID given by server
	CString cmdLine = GetCommandLine();

	CString p1,p2,p3;
	int pos = cmdLine.Find(' ');

	// for profiling, extract first argument = program name
	if (cmdLine.Find(".exe") >= 0)
	{
		cmdLine = cmdLine.Right(cmdLine.GetLength()-pos-1);
		pos = cmdLine.Find(' ');
	}

	p1 = cmdLine.Left(pos);
	p2 = cmdLine.Right(cmdLine.GetLength()-pos-1);
	pos = p2.Find(' ');
	p3 = p2.Right(p2.GetLength()-pos-1);
	p2 = p2.Left(pos);

	ID = atoi(p1);
	int	port = atoi(p2);
	int IP = atoi(p3);

	__asm mov eax,IP
	__asm bswap eax
	__asm mov IP,eax
	
	//	Prepare log file
	CString LogName;
	LogName.Format("WorkUnit%d.log",ID);
	COUT.open(LPCTSTR(LogName));
	COUT << "Starting Work unit..." << endl;

	in_addr addr;
	addr.S_un.S_addr = IP;
	char *address = inet_ntoa(addr);

	//	establish connection
	if (client.ConnectToServer(address,port))
	{
		//	write back ID to acknowledge Workunit availability
		CString IP = client.GetSockName();
		ip = client.GetAddr();

		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_WUNIT;
		msg.msg_size = 0;
		msg.msg_data[0] = ID;
		msg.msg_data[1] = 0;
		msg.msg_data[2] = 0;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = ip;
		msg.msg_tail = MSG_END;
		client.Write(&msg,MSGSIZE);

		//	create user interface with connection
		dlg = new CWorkUnitDlg(client.GetSockName(),client.GetPort(),ID);
		raytracer_data.app = this;

		dlg->data = &raytracer_data;
		m_pMainWnd = dlg;
		dlg->DoModal();

		// dialog is finished : job complete
		msg.msg_id = JOB_STOP;
		msg.msg_size = (raytracer_data.end-raytracer_data.start)*raytracer_data.camera.width*4;
		msg.msg_size *= 2;	// add z-buffer
		msg.msg_data[1] = raytracer_data.start;
		msg.msg_data[2] = raytracer_data.end;
		msg.msg_tail = MSG_DATA;
		client.Write(&msg,MSGSIZE);

		//	send job result
		client.Write(raytracer_data.image,msg.msg_size/2);
		client.Write(raytracer_data.ZBuffer,msg.msg_size/2);
	//	SaveZBuffer();

		Sleep(2000);
		client.DisconnectServer();

		//	clear data
		delete [] raytracer_data.image;
		for (int i=0;i<raytracer_data.lights.GetSize();i++)
			delete raytracer_data.lights[i];
		raytracer_data.lights.RemoveAll();
		for (i=0;i<raytracer_data.objects.GetSize();i++)
			delete raytracer_data.objects[i];
		raytracer_data.objects.RemoveAll();

		POSITION pos = raytracer_data.textures.GetStartPosition();
		CTexture *txt = NULL;
		CString key;
		while (pos != NULL)
		{
			raytracer_data.textures.GetNextAssoc( pos, key,(void*&)txt);
			delete txt;
		}
		raytracer_data.textures.RemoveAll();
	}
	else
		AfxMessageBox("Unable to connect to server request !");

	COUT << "Exiting Workunit..." << endl;
	COUT.close();

	return FALSE;
}

void CWorkUnitApp::WriteMessage(CString &msg)
{
	if (NULL != m_pMainWnd)
	{
		CListBox *lb = ((CWorkUnitDlg*)m_pMainWnd)->lb;
		lb->AddString(msg);
		int nb = lb->GetCount();
		lb->SetCurSel(nb-1);
	}
	else
	{
		AfxMessageBox(msg);
	}
}




int CWorkUnitApp::ExitInstance() 
{
	delete raytracer_data.lock;

	return CWinApp::ExitInstance();
}

void CWorkUnitApp::Notify(void)
{
	if (raytracer_data.percent > 0)
		SendPercentage(raytracer_data.percent);
	if (raytracer_data.percent == 9999)
	{
		CString msg = "Rendering finished. Post-processing phase.";
		WriteMessage(msg);
	}
	else if (raytracer_data.percent == 10000)
	{
		SendPercentage(raytracer_data.percent);
		dlg->KillTimer(1);
		dlg->EndDialog(IDOK);
	}
}

void CWorkUnitApp::SendPercentage(UINT percent) const
{
	((CWorkUnitDlg*)dlg)->SetProgress(percent/100);

	MSGSTRUCT msg;
	msg.msg_header = MSG_START;
	msg.msg_id = JOB_PERCENT;
	msg.msg_size = 0;
	msg.msg_data[0] = ID;
	msg.msg_data[1] = 0;
	msg.msg_data[2] = 0;
	msg.msg_data[3] = 0;
	msg.msg_data[4] = percent;
	msg.msg_tail = MSG_END;
	client.Write(&msg,MSGSIZE);
}

void CWorkUnitApp::SaveZBuffer(void)
{
	CFile zbuffer("zbuffer.dat",CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive);

	int height = raytracer_data.end-raytracer_data.start;
	
	CString str;
	for (int j=0;j<height;j++)
	{
		for (int i=0;i<raytracer_data.camera.width;i++)
		{
			str.Format("%f ",raytracer_data.ZBuffer[i+j*raytracer_data.camera.width]);
			zbuffer.Write(LPCTSTR(str),str.GetLength());
		}
		str.Format("\n");
		zbuffer.Write(LPCTSTR(str),str.GetLength());
	}

	zbuffer.Close();
}

