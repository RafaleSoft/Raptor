// MiniClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream.h>	// cout
#include "MiniClient.h"
#include "CltSocket.h"

#include "../Systemdll/Systemdll.h"	//	client server

#include "../Raptor/include/Raptor.h" // glInitRaptor

#include <winbase.h>	// sleep
#include "../Messages.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// global objects
CClient<CClientSocket>	client;
bool					connected = false;
unsigned int			jobID = 0xFFFFFFFF;
CParser					parser;
CAnalyser				analyser(&parser,&client);
unsigned char			*image = NULL;
unsigned int			image_size = 0;
unsigned int			image_height = 0;
unsigned int			image_width = 0;

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;


void showhelp()
{
	cout << "Available commands: " << endl;
	cout << "   - connect <server> < port>: " << endl;
	cout << "       connects to port of server specified by IP address or hostname" << endl;
	cout << "       IP address must be in the format ###.###.###.### and hostname is" << endl;
	cout << "       given by gethostbyname() or is the global defined 'localhost'"<< endl;
	cout << "   - deljob <jobID>: delete job <jobID> from server" << endl;
	cout << "   - disconnect: closes connection to server" << endl;
	cout << "   - getjob <jobID>: receive result of job <jobID>, but job is not removed from server" << endl;
	cout << "   - help: display help" << endl;
	cout << "   - quit: exit this client" << endl;
	cout << "   - render <filename>: " << endl;
	cout << "       parses scene described in filename and sends data to rays server" << endl;
	cout << "   - save <filename>: saves the most recently job received in <filename>" << endl;
	cout << "   - status: display status of server" << endl;
	cout << "   - upload <local-filename> <remote_filename>: uploads a texture file" << endl;
	cout << "       to rays server. Once a texture is uploaded, any job can use it" << endl;
	cout << "       resulting in a faster job start." << endl;
	cout << "   - verbose [on|off]: display full information on most actions" << endl;
	cout << endl;
	cout << "In addition, any system command is accepted ( cd, dir, ... )" << endl;
}

void connect(CString param)
{
	CString server;
	CString port;
	int p;

	param.TrimLeft();
	int pos = param.Find(' ');

	if (pos<0)
	{
		cout << "Missing parameter for connection" << endl;
		return;
	}
	else
	{
		server = param.Left(pos);
		port = param.Right(param.GetLength()-pos);
		port.TrimLeft();
		p = atoi(LPCTSTR(port));
	}

	if (client.ConnectToServer(server,p))
	{
		cout << "connection : " << client.GetPort() << " - " << client.GetSockName() << endl;
		cout << "awaiting data" << endl;
		connected = true;
	}
	else
	{
		cout << "Failed to establish connection to server '" << server << "' on port " << p << endl;
	}
}

void disconnect()
{
	if( jobID < 0xFFFFFFFF)
	{
		cout << "Removing job from server" << endl;
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_REQUEST;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = jobID;
		msg.msg_data[1] = JOB_DELETE;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = client.GetAddr();
		msg.msg_size = 0;
		
		client.Write(&msg,MSGSIZE);
		
		client.Read(&msg,MSGSIZE);
	}

	if (client.DisconnectServer())
		cout << "Connection to rays server is closed successfully" << endl;
	else
		cout << "Failed to close connection to rays server" << endl;
	connected = false;
}

void status()
{
	if (connected)
	{
		cout << endl;
		cout << "Client status:" << endl;
		cout << "   connection : " << client.GetPort() << " - " << client.GetSockName() << endl;
		cout << "   jobID : " << jobID << endl;

		if (jobID < 0xFFFFFFFF)
		{
			cout << "Querying server status ..." << endl;
			MSGSTRUCT msg;
			msg.msg_header = MSG_START;
			msg.msg_id = JOB_STATUS;
			msg.msg_tail = MSG_END;
			msg.msg_data[0] = jobID;
			msg.msg_data[3] = client.GetPort();
			msg.msg_data[4] = client.GetAddr();
			msg.msg_size = 0;
			client.Write(&msg,MSGSIZE);

			client.Read(&msg,MSGSIZE);
			time_t start = (time_t)msg.msg_data[1];
			time_t end = (time_t)msg.msg_data[2];
			struct tm *today = localtime( &start );

			cout << endl;
			cout << "Server status: " << endl;
			cout << "     start time : " << asctime(today) << endl;

			struct tm *finish = localtime( &end );
			if (finish != NULL)
				cout << "     estimated finish time : " << asctime(finish) << endl;
			else
				cout << "     estimated finish time : INFINITE" << endl;
			cout << "     job done yet: " << ((float)(msg.msg_data[4]))/100.0 << " %" <<endl;

			if (msg.msg_size>0)
			{
				cout << endl;
				cout << "There are " << msg.msg_size/sizeof(unsigned int) << " available results with IDs:" << endl;

				unsigned int *buffer = new unsigned int[msg.msg_size];
				client.Read(buffer,msg.msg_size);
				for (int i=0;i<msg.msg_size/sizeof(unsigned int);i++)
					cout << "     Job ID: " << buffer[i] << endl;

				delete [] buffer;
			}

			cout << endl;
		}
		else
		{
			cout << "Job not started or wrong jobID" << endl;
		}
	}
	else
	{
		cout << "You are not connected to a Rays server, use 'connect'" << endl;
	}
}

void render(CString fname)
{
	if (connected)
	{
		analyser.InitAnalyse();

		fname.TrimLeft();
		if (!parser.InitLex(fname))
			cout << "Unable to acces to file " << LPCTSTR(fname) << endl;
		else
		{
			cout << endl;
			cout << "Parsing input file ..." << endl;
			int result = analyser.Analyse();
			if (result>=0)
			{
				if (!parser.FinishLex())
					cout << "Unable to release file " << LPCTSTR(fname) << endl;
				else
				{
					jobID = analyser.jobID;
					cout << "Server job ID is: " << jobID << endl;
				}
			}
			else
			{
				cout << "Syntax error in source file at: line ";
				cout << parser.GetNumLine() << " in position " << parser.GetLinePos() << endl;
				if (!parser.FinishLex())
					cout << "Unable to release file " << LPCTSTR(fname) << endl;
			}
		}
	}
	else
	{
		cout << "You are not connected to a Rays server, use 'connect'" << endl;
	}
}

void getjob(CString strJobID)
{
	if (connected)
	{
		strJobID.TrimLeft();

		cout << "Querying server job ..." << endl;
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_REQUEST;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = atoi(LPCTSTR(strJobID));
		msg.msg_data[1] = IMG_DATA;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = client.GetAddr();
		msg.msg_size = 0;
		client.Write(&msg,MSGSIZE);

		client.Read(&msg,MSGSIZE);
		if (msg.msg_id == ACK_NONE)
			cout << "You are not the owner of job " << msg.msg_data[0] << " or this job does not exist" << endl;
		else
		{
			int pos = 0;
			int nbp = msg.msg_size/PACKET_SIZE;
			
			if (image != NULL)
				delete [] image;
			image = new unsigned char[msg.msg_size];
			image_size = msg.msg_size;
			image_width = msg.msg_data[0];
			image_height = msg.msg_data[1];

			client.Read(image,msg.msg_size);
			
			cout << "Image data successfully received !" << endl;
		}
	}
	else
	{
		cout << "You are not connected to a Rays server, use 'connect'" << endl;
	}
}


void deljob(CString strJobID)
{
	if (connected)
	{
		strJobID.TrimLeft();

		cout << "Querying server job ..." << endl;
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_REQUEST;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = atoi(LPCTSTR(strJobID));
		msg.msg_data[1] = JOB_DELETE;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = client.GetAddr();
		msg.msg_size = 0;
		
		if (msg.msg_data[0]==jobID)
			cout << "You cannot delete your connection" << endl;
		else
		{
			client.Write(&msg,MSGSIZE);

			client.Read(&msg,MSGSIZE);
			if (msg.msg_id == ACK_NONE)
				cout << "You are not the owner of job " << msg.msg_data[0] << " or this job does not exist" << endl;
			else
			{
				if (msg.msg_id == ACK_JOB)
					cout << "Job " << msg.msg_data[0] << " successfully removed from server !" << endl;
			}
		}
	}
	else
	{
		cout << "You are not connected to a Rays server, use 'connect'" << endl;
	}
}

void savejob(CString fname)
{
		
	char	head[18] ;
	long	newcolor = 0;
	FILE	*out_rgb ;

	fname.TrimLeft();
	out_rgb = fopen( LPCTSTR(fname), "wb" ) ;
	if ( ! out_rgb ) 
	{
		cout << "Cannot open %s ." << LPCTSTR(fname) << endl;
		return;
	}

    // --- create the header (TARGA file) ---
	head[0]  = 0 ;		  // ID field lenght.
	head[1]  = 0 ;		  // Color map type.
	head[2]  = 2 ;		  // Image type: true color, uncompressed.
	head[3]  = head[4] = 0 ;  // First color map entry.
	head[5]  = head[6] = 0 ;  // Color map lenght.
	head[7]  = 0 ;		  // Color map entry size.
	head[8]  = head[9] = 0 ;  // Image X origin.
	head[10] = head[11] = 0 ; // Image Y origin.
	head[13] = (char) (image_width >> 8) ;	  // Image width.
	head[12] = (char) (image_width & 0x0FF) ;
	head[15] = (char) (image_height >> 8) ;	  // Image height.
	head[14] = (char) (image_height & 0x0FF) ;
	head[16] = 32 ;		  // Bits per pixel.
	head[17] = 0 ;		  // Image descriptor bits ;

	fwrite( head, sizeof(char), 18, out_rgb ) ;
	for ( int i=0; i < image_size; i+=4 )
	{
		newcolor=	(((long)image[i+3])<<24)+
					(((long)image[i])<<16)+
					(((long)image[i+1])<<8)+
					(((long)image[i+2]));
		// a r g b
		fwrite( &newcolor, 4, 1, out_rgb ) ;
	}
	//fwrite(image,image_size,1,out_rgb);
	fclose( out_rgb ) ;
}

void upload(CString cmd)
{
	cmd.TrimLeft();
	cmd.TrimRight();
	int pos = cmd.Find(' ');
	
	CString local_filename;
	CString remote_filename;

	if (pos<0)
	{
		cout << "Missing remote filename" << endl;
	}
	else
	{
		local_filename = cmd.Left(pos);
		remote_filename = cmd.Right(cmd.GetLength()-pos);
		remote_filename.TrimLeft();

		int pos = local_filename.GetLength() - local_filename.ReverseFind('.');
		CTexture t;
		CString ext = (local_filename.Right(pos-1));
		IMAGELOADERPROC loader = t.GetLoader(ext);

		if (loader == NULL)
		{
			cout << "Unsupported file format: " << local_filename << endl;
			return;
		}
		
		LPGLTEXTUREOBJECT T = t.BindTexture();

		if (!loader(local_filename,T))
		{
			cout << "Unable to open file: " << local_filename << endl;
			return;
		}

		int offset = MSGSIZE;
		int len = remote_filename.GetLength();
		int tsize = T->width*T->height*4;
		unsigned char *buffer = new unsigned char[offset + tsize + len];

		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = OBJ_TEXTURE;
		msg.msg_tail = MSG_DATA;
		msg.msg_data[0] = T->width;
		msg.msg_data[1] = T->height;
		msg.msg_data[2] = len;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = client.GetAddr();
		msg.msg_size = tsize + len;

		memcpy(buffer,&msg,offset);
		memcpy(buffer+offset,LPCTSTR(remote_filename),len);
		memcpy(buffer+offset+len,T->pixels,tsize);
		client.Write(buffer,offset+tsize+len);

		client.Read(&msg,offset);
		if (msg.msg_id == ACK_NONE)
			cout << "File " << local_filename << " could not be uploaded as " << remote_filename << endl;
		else if (msg.msg_id == ACK_IMG)
			cout << "File " << local_filename << " successfully uploaded as " << remote_filename << endl;

		delete [] buffer;
	}
}


void verbose(CString cmd)
{
	cmd.TrimLeft();

	if (cmd == "on")
	{
		analyser.Verbose(true);
		cout << "Client turned to talkative mode" << endl;
	}
	else if(cmd == "off")
	{
		analyser.Verbose(false);
		cout << "Client turned to quiet mode" << endl;
	}
}


void parsecommand(CString cmd)
{
	int pos = cmd.Find(' ');
	CString command;

	if (pos<0)
		command = cmd;
	else
		command = cmd.Left(pos);

	if (command == "help")
		showhelp();
	else if (command == "connect")
		connect(cmd.Right(cmd.GetLength()-pos));
	else if (command == "disconnect")
		disconnect();
	else if (command == "render")
		render(cmd.Right(cmd.GetLength()-pos));
	else if (command == "status")
		status();
	else if (command == "getjob")
		getjob(cmd.Right(cmd.GetLength()-pos));
	else if (command == "deljob")
		deljob(cmd.Right(cmd.GetLength()-pos));
	else if (command == "quit")
		cout << "Bye bye !" << endl;
	else if (command == "save")
		savejob(cmd.Right(cmd.GetLength()-pos));
	else if (command == "upload")
		upload(cmd.Right(cmd.GetLength()-pos));
	else if (command == "verbose")
		verbose(cmd.Right(cmd.GetLength()-pos));
	else
		system(LPCTSTR(cmd));
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	CString	command;
	char	buffer[256];

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else if (!AfxSocketInit())
	{
		cout << "Afx sockets initialisation failed !" << endl;
		nRetCode = 1;
	}
	else if (!glInitRaptor())
	{
		cout << "Raptor initialisation failed !" << endl;
		nRetCode = 1;
	}
	else
	{
		cout << "Rays2 miniclient - By Rafale Soft Inc.(c)" << endl;
		cout << "type 'help' for specific Rays2 miniclient command list" << endl;
		cout << endl;
		
		while (command != "quit")
		{
			cout << endl;
			cout << "Rays2>";
			cin.getline(buffer,255);
		
			command = buffer;
			command.TrimLeft();
			parsecommand(command);
		}

		//	clear memory
		analyser.InitAnalyse();
		if (image != NULL)
				delete [] image;
		if (connected)
			disconnect();
	}

	return nRetCode;
}


