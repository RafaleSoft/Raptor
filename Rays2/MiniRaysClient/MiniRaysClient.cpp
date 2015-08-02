// MiniRaysClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientDlg.h"
#include "MiniRaysClientStatus.h"
#include "MiniRaysClientUpload.h"
#include "..\RaysCommandLine.h"
#include "..\Raptor\Include\Texture.h"		// création des textures
#include "..\PlugIn.cxx"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientApp Lex/Yacc methods
void CParser::OnError(CLex::LexError err)
{
	if (output != NULL)
	switch(err)
	{
		case CLex::LEX_NOERR:
			output->Output("No error found.");
			break;
		case CLex::LEX_ERR_SRC:
			output->Output("Error reading source file.");
			break;
		case CLex::LEX_ERR_SYN:
			output->Output("Syntax error in lex file.");
			break;
		default:
			output->Output("Unknown Lex error.");
			break;
	}
}

void CAnalyser::OnError(CYacc::YaccError err)
{
	if (output != NULL)
	switch(err)
	{
	case CYacc::YACC_NOERR:
		output->Output("No error found.");
		break;
	case CYacc::YACC_ERR_LEX:
		output->Output("Internal lex error.");
		break;
	case CYacc::YACC_ERR_SYN:
		output->Output("Syntax error in lex file.");
		break;
	default:
		output->Output("Unknown Lex error.");
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientApp

BEGIN_MESSAGE_MAP(CMiniRaysClientApp, CWinApp)
	//{{AFX_MSG_MAP(CMiniRaysClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientApp construction

CMiniRaysClientApp::CMiniRaysClientApp():
	connected(false),jobID(0xFFFFFFFF),
	scene(""),status(NULL),output(NULL),T(NULL),
	image(NULL),image_size(0),image_height(0),image_width(0)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMiniRaysClientApp object

CMiniRaysClientApp miniRaysClient;


/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientApp initialization

BOOL CMiniRaysClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	else if (!Raptor::glInitRaptor())
	{
		AfxMessageBox("Raptor initialisation failed !");
		return FALSE;
	}

	Enable3dControls();			// Call this when using MFC in a shared DLL

	RaysCmdLine cmd;
	ParseCommandLine(cmd);

	CMiniRaysClientDlg dlg;
	if (!cmd.addrStr.IsEmpty())
		dlg.m_IP = cmd.addrStr;
	output = &dlg;

	client = new CClient<CCltSocket>;
	parser = new CParser;
	analyser = new CAnalyser(parser,client);
	parser->output = &dlg;
	analyser->output = &dlg;

	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}

	client->DisconnectServer();

	delete client;
	delete parser;
	delete analyser;

	if (image != NULL)
		delete [] image;

	return FALSE;
}



bool CMiniRaysClientApp::Connect(CString IP,int port)
{
	CString msg;

	if (client->ConnectToServer(IP,port))
	{
		msg.Format("connection : %s - %d",LPCTSTR(client->GetSockName()),client->GetPort());
		output->Output(msg);
		output->Output("awaiting data");
		connected = true;
	}
	else
	{
		connected = false;
		msg.Format("Failed to establish connection to server '%s' on port %d",LPCTSTR(IP),port);
		output->Output(msg);
	}

	return connected;
}


bool CMiniRaysClientApp::Disconnect()
{
	if (client->DisconnectServer())
		output->Output("Connection to rays server is closed");
	else
		output->Output("Failed to close connection to rays server");

	connected = false;

	return connected;
}

void CMiniRaysClientApp::Render()
{
	bool render = false;
	CString msg;
	CString fname = scene_location + scene;

	if (scene.IsEmpty())
		output->Output("No scene is selected !");
	else
	{
		analyser->InitAnalyse();

		if (!parser->InitLex(fname))
		{
			msg = "Unable to acces to file " + fname;
			output->Output(msg);
		}
		else
		{
			output->Output("Parsing input file ...");
			int result = analyser->Analyse();
			if (result>=0)
			{
				if (!parser->FinishLex())
				{
					msg = "Unable to release file " + fname;
					output->Output(msg);
				}
				else
				{
					render = true;
				}
			}
			else
			{
				msg.Format("Syntax error in source file at: line %d in position %d",parser->GetNumLine(),parser->GetLinePos());
				output->Output(msg);
				if (!parser->FinishLex())
				{
					msg = "Unable to release file " + fname;
					output->Output(msg);
				}
			}
		}
	}

	if ((render) && (connected))
	{
		config_sent = false;
		sphere_sent = false;
		light_sent = false;
		composition_sent = false;
		plugin_sent = false;
		frame_sent = false;

		config_ack = false;
		sphere_ack = false;
		light_ack = false;
		composition_ack = false;
		plugin_ack = false;
		frame_ack = false;
		
		analyser->StartJob(12345);
	}
	else
	{
		if (!connected)
			output->Output("You are not connected to a Rays server, use 'connect'");
	}
}


void CMiniRaysClientApp::Status(void)
{
	if (connected)
	{
		output->Output("Querying server status ...");

	}
	else
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}	

	MiniRaysClientStatus	statusDlg;
	status = &statusDlg;

	statusDlg.DoModal();

	status = NULL;
}


void CMiniRaysClientApp::QueryStatus(void)
{
	if (connected)
	{
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_STATUS;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = jobID;
		msg.msg_data[3] = client->GetPort();
		msg.msg_data[4] = client->GetAddr();
		msg.msg_size = 0;

		client->Write(&msg,MSGSIZE);
	}
}


void CMiniRaysClientApp::Getjob(unsigned int jobID)
{
	if (connected)
	{
		output->Output("Querying server job ...");

		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_REQUEST;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = jobID;
		msg.msg_data[1] = IMG_DATA;
		msg.msg_data[3] = client->GetPort();
		msg.msg_data[4] = client->GetAddr();
		msg.msg_size = 0;

		client->Write(&msg,MSGSIZE);
	}
	else
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}
}


bool CMiniRaysClientApp::Savejob(CString fname)
{
		
	char	head[18] ;
	long	newcolor = 0;
	FILE	*out_rgb ;

	fname.TrimLeft();
	out_rgb = fopen( LPCTSTR(fname), "wb" ) ;
	if ( ! out_rgb ) 
	{
		output->Output("Cannot open " + fname);
		return false;
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
	for ( unsigned int i=0; i < image_size; i+=4 )
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

	return true;
}

void CMiniRaysClientApp::Deljob(unsigned int jobID)
{
	if (connected)
	{
		output->Output("Querying server job ...");
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = JOB_REQUEST;
		msg.msg_tail = MSG_END;
		msg.msg_data[0] = jobID;
		msg.msg_data[1] = JOB_DELETE;
		msg.msg_data[3] = client->GetPort();
		msg.msg_data[4] = client->GetAddr();
		msg.msg_size = 0;
		
		if (msg.msg_data[0]== this->jobID)
			output->Output("You cannot delete your connection");
		else
			client->Write(&msg,MSGSIZE);
	}
	else
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}
}


void CMiniRaysClientApp::Upload(void)
{
	if (connected)
	{
		MiniRaysClientUpload uploadDlg;
		uploadDlg.DoModal();
	}
	else
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}
}


CString CMiniRaysClientApp::CheckUpload(void)
{
	if (!connected)
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}

	int pos = upload.GetLength() - upload.ReverseFind('.');
	CString ext = (upload.Right(pos-1));

	if (ext == "dll")
	{
		CString info;
		PLUGIN plugin = OpenPlugin(upload_location+upload);

		if (plugin.moduleInstance == NULL)
		{
			info = "Unable to open file: " + upload;
			output->Output(info);
			correct_module_upload = false;
		}
		else
		{
			if (
					(plugin.pGetModuleAuthor != NULL) && 
					(plugin.pGetModuleDescription != NULL) && 
					(plugin.pGetModuleName != NULL) &&
					(plugin.pGetModuleType != NULL) &&
					(plugin.pGetModuleVersion != NULL) &&
					(plugin.pInitModule != NULL) &&
					(plugin.pGetParameters != NULL) &&
					(plugin.pSetParameters != NULL)
				)
			{
				CString name = plugin.pGetModuleName();
				CString author = plugin.pGetModuleAuthor();
				CString description = plugin.pGetModuleDescription();
				PLUGIN_TYPE ptype = plugin.pGetModuleType();
				union
				{
					unsigned char	b[4];
					unsigned int	i;
				} version;
				version.i = plugin.pGetModuleVersion();

				CString type;
				correct_module_upload = true;

				switch(ptype)
				{
					case UNKNOWN_TYPE : type = "Unknown type";break;
					case AFTER_EFFECT : 
						if (plugin.pAfterEffect != NULL) 
							type = "After Effect";
						else
						{
							correct_module_upload = false;
							type = "Corrupted ( After Effects )";
						}
						break;
					case GEOMETRY : 
						if (plugin.pNormal != NULL) 
							type = "Geometry";
						else
						{
							correct_module_upload = false;
							type = "Corrupted ( Geometry )";
						}
						break;
					case TEXTURE : type = "Texture";break;
					case SHADER : type = "Shader";break;
					case ENGINE : type = "Engine";break;
				}

				info.Format("Rays2 module named: %s\nDevelopped by: %s\nDescription: %s\nType : %s\nVersion : %d.%d.%d.%d",
							LPCTSTR(name),LPCTSTR(author),LPCTSTR(description),LPCTSTR(type),
							version.b[3],version.b[2],version.b[1],version.b[0]);
			}
			else
			{
				info = upload + " is not a valid Rays2 plug-in.";
				output->Output(info);
				correct_module_upload = false;
			}

			AfxFreeLibrary( plugin.moduleInstance );
		}
		return info;
	}
	else
	{
		CTexture t;
		IMAGE_LOADER_PROC loader = t.GetLoader(ext);

		if (loader == NULL)
		{
			CString err = "Unsupported file format: " + upload;
			output->Output(err);
			correct_texture_upload = false;
			return err;
		}
		else
		{
			if (T != NULL)
				delete T;

			T = new GL_TEXTURE_OBJECT;
			T->pixels = NULL;
			T->width = T->height = T->level=0;
			T->alpha = 255;

			if (!loader(upload_location+upload,T))
			{
				CString err = "Unable to open file: " + upload;
				output->Output(err);
				correct_texture_upload = false;
				return err;
			}

			CString info;
			info.Format("Texture file info: \nWidth: %d\nHeight: %d",T->width,T->height);
			correct_texture_upload = true;
			return info;
		}
	}
}

void CMiniRaysClientApp::QueryUpload(void)
{
	if (connected)
	{
		if (correct_texture_upload)
		{
			int offset = MSGSIZE;
			int len = upload.GetLength();
			int tsize = T->width * T->height * 4;
			unsigned char *buffer = new unsigned char[offset + tsize + len];

			MSGSTRUCT msg;
			msg.msg_header = MSG_START;
			msg.msg_id = OBJ_TEXTURE;
			msg.msg_tail = MSG_DATA;
			msg.msg_data[0] = T->width;
			msg.msg_data[1] = T->height;
			msg.msg_data[2] = len;
			msg.msg_data[3] = client->GetPort();
			msg.msg_data[4] = client->GetAddr();
			msg.msg_size = tsize + len;

			memcpy(buffer,&msg,offset);
			memcpy(buffer+offset,LPCTSTR(upload),len);
			memcpy(buffer+offset+len,T->pixels,tsize);
			client->Write(buffer,offset+tsize+len);

			delete [] buffer;
		}
		else if (correct_module_upload)
		{
			int offset = MSGSIZE;
			int len = upload.GetLength();
			CFile plugin(upload_location+upload,CFile::modeRead|CFile::shareExclusive|CFile::typeBinary);
			CFileStatus fs;
			plugin.GetStatus(fs);
			unsigned char *buffer = new unsigned char[offset + fs.m_size + len];

			MSGSTRUCT msg;
			msg.msg_header = MSG_START;
			msg.msg_id = OBJ_PLUGIN;
			msg.msg_tail = MSG_DATA;
			msg.msg_data[0] = fs.m_size;
			msg.msg_data[1] = fs.m_mtime.GetTime();
			msg.msg_data[2] = len;
			msg.msg_data[3] = client->GetPort();
			msg.msg_data[4] = client->GetAddr();
			msg.msg_size = fs.m_size + len;

			memcpy(buffer,&msg,offset);
			memcpy(buffer+offset,LPCTSTR(upload),len);
			plugin.Read(&buffer[offset+len],fs.m_size);
			client->Write(buffer,offset+fs.m_size+len);

			delete [] buffer;
		}
	}
	else
	{
		output->Output("You are not connected to a Rays server, use 'connect'");
	}
}