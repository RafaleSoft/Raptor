// WorkUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

#include "RaysWorkUnit.h"
#include "Raytracer.h"

/////////////////////////////////////////////////////////////////////////////
// Const and globals
unsigned int			ID;
static CRaysWorkUnit	*p_WU = NULL;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch(fdwCtrlType)
	{ 
		// Handle the CTRL-C signal.
		case CTRL_C_EVENT:
		{
			std::cout << "WorkUnit user exit requested by Ctrl-C. Exiting, bye!" << std::endl;
			p_WU->requestExit();
			return(TRUE);
			break;
		}
		// CTRL-CLOSE: confirm that the user wants to exit.
		case CTRL_CLOSE_EVENT:
		{
			std::cout << "WorkUnit user exit requested by Ctrl-close. Exiting, bye!" << std::endl;
			p_WU->requestExit();
			return(TRUE);
			break;
		}
		// Pass other signals to the next handler.
		case CTRL_BREAK_EVENT: 
			return FALSE; 
		case CTRL_LOGOFF_EVENT: 
			return FALSE; 
		case CTRL_SHUTDOWN_EVENT: 
			return FALSE; 
		default: 
			return FALSE; 
	} 
} 

CRaysWorkUnit::CRaysWorkUnit():
	m_bExit(false),raytracer_data(NULL)
{
	raytracer_data = new CRaytracerData();
}

CRaysWorkUnit::~CRaysWorkUnit()
{
	delete raytracer_data;
}

/////////////////////////////////////////////////////////////////////////////
//	Entry point
int main(int argc, char* argv[])
{
	std::cout << std::endl;
	std::cout << "        Rays Workunit.      " << std::endl;
	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;

#ifdef WIN32
	if (!Network::initSocketLayer())
		return 1;
#endif

	CCmdLineParser parser;
	parser.addOption("id","i",(unsigned short)0);
	parser.addOption("port","p",(unsigned short)2048);
	parser.addOption("width","w",(unsigned short)256);
	parser.addOption("height","h",(unsigned short)256);
	parser.addOption("host_addr","a","127.0.0.1");

	if (!parser.parse(argc,argv))
	{
		std::cout << "Rays Workunit failed to parse command line. Exiting, bye!" << std::endl;
		return -1;
	}

	p_WU = new CRaysWorkUnit();
	if (p_WU->start(parser))
	{
		// TODO: Run Raytracer !

		const CRaytracerData* raytracer_data = p_WU->getRaytracerData();
		
		// job complete, send back results
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_STOP;
		msg.msg_size = (raytracer_data->getEnd()-raytracer_data->getStart())*raytracer_data->getCamera().width*4;
		msg.msg_size *= 2;	// add z-buffer
		msg.msg_data[1] = raytracer_data->getStart();
		msg.msg_data[2] = raytracer_data->getEnd();
		msg.msg_tail = MSG_DATA;
		p_WU->write(&msg,MSGSIZE);

		//	send job result
		p_WU->write(raytracer_data->getImage(),msg.msg_size/2);
		p_WU->write(raytracer_data->getZBuffer(),msg.msg_size/2);
	}

	std::cout << "Exiting work unit ... bye!" << std::endl;
	p_WU->disconnectServer();
	delete p_WU;

	return 0;
}



bool CRaysWorkUnit::start(const CCmdLineParser &cmdline)
{
	unsigned short id = 0;
	unsigned short port = 2049;
	const char* address = "127.0.0.1";
	cmdline.getValue("host_addr",address);
	cmdline.getValue("port",port);
	cmdline.getValue("id",id);

	if (connectToServer(address,port))
	{
		//	write back ID to acknowledge Workunit availability
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_WUNIT;
		msg.msg_size = 0;
		msg.msg_data[0] = id;
		msg.msg_data[1] = 0;
		msg.msg_data[2] = 0;
		msg.msg_data[3] = getPort();
		msg.msg_data[4] = getAddr();
		msg.msg_tail = MSG_END;
		write(&msg,MSGSIZE);
	}
	else
		std::cout << "Unable to connect to server request !" << std::endl;

	return false;
}

/*
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
*/