/***************************************************************************/
/*                                                                         */
/*  RaptorServerInstance.cpp                                               */
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


#include "stdafx.h"

#if !defined(AFX_RAPTORSERVERINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
    #include "RaptorServerInstance.h"
#endif
#if !defined(AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_)
	#include "ServerSession.h"
#endif

#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
	#include "System/RaptorApplication.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
	#include "ToolBox/Imaging.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif

#include "YUVCompressor.h"
#include "ToolBox/RaptorToolBox.h"


CRaptorServerInstance* CRaptorServerInstance::m_pInstance = NULL;


#if defined(WIN32)
	#define SLEEP(s)		Sleep(s)
	#include <direct.h>	//! Win32 directories system calls.
#else	// Linux environment
	#include <unistd.h>
	// For identical behavior when compared to windows : 1 ms = 1000 µs
	#define SLEEP(s)		usleep(1000*s)
	#define MAX_PATH		260
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorServerInstance::CRaptorServerInstance()
{
    m_pDisplay = NULL;
	m_pApplication = NULL;
	m_pCompressor = NULL;
	m_bStarted = false;
	m_pSessionManager = new CServerSession();
}

CRaptorServerInstance::~CRaptorServerInstance()
{
    m_pInstance = NULL;
}

CRaptorServerInstance* CRaptorServerInstance::GetInstance(void)
{
    if (m_pInstance == NULL)
		m_pInstance = new CRaptorServerInstance();

    return m_pInstance;
}

void CRaptorServerInstance::glRender()
{
	if (m_pWindow.handle() != 0)
	{
		request r;
		size_t nbr = 0;
		{
			CRaptorLock lock(m_mutex);
			nbr = m_requests.size();
			if (nbr > 0)
			{
				r = m_requests.front();
				m_requests.erase(m_requests.begin());
			}
		}

		if (nbr > 0)
		{
			CRaptorConsole *pConsole = Raptor::GetConsole();
			CGLFont::FONT_TEXT_ITEM item;
			stringstream str;
			str << "Pending requests: ";
			str << nbr;
			item.text = str.str();
			pConsole->updateItem(item,0);
			stringstream str2;
			str2 << "Pending replies: ";
			str2 << m_replies.size();
			item.text = str2.str();
			pConsole->updateItem(item,1);

			m_pDisplay->glvkBindDisplay(m_pWindow);

			CServerSession::session request_session = m_pSessionManager->getSession(r.id);
			r.display = request_session.display;

			executeRequest(r);
			delete [] (unsigned char*)r.data;

			if (r.render)
			{
				CRaptorDisplay *pDisplay = m_pDisplay;
				if (r.display != 0)
				{
					pDisplay = r.display;
					RAPTOR_HANDLE handle;
					pDisplay->glvkBindDisplay(handle);
				}
				
				pDisplay->glRender();
								
				CRaptorDisplayConfig state;
				pDisplay->glQueryStatus(state,GL_CONFIG_STATE_QUERY);

				size_t size = state.width*state.height*4 + sizeof(CRaptorNetwork::IMAGE_COMMAND);
				r.data = new unsigned char[size];
				CRaptorNetwork::IMAGE_COMMAND *image_command = (CRaptorNetwork::IMAGE_COMMAND*)r.data;
				image_command->command = CRaptorNetwork::getImageCommand().command;
				image_command->header.blocHeight = state.height;
				image_command->header.blocWidth = state.width;
				image_command->header.xOffset = 0;
				image_command->header.yOffset = 0;
				uint8_t* image = (uint8_t*)&(image_command->pData);

				pDisplay->glGrab(0,0,state.width,state.height,image,r.size);

				CRaptorLock lock(m_rmutex);
				m_replies.push_back(r);
				m_prodCons.P();

				if (r.display != 0)
					pDisplay->glvkUnBindDisplay();
				m_pDisplay->glRender();
			}

			m_pDisplay->glvkUnBindDisplay();
		}
		else
		{
			if (!m_recycledDisplays.empty())
			{
				m_pDisplay->glvkBindDisplay(m_pWindow);
				// need a mutex here.
				for (size_t i=0;i<m_recycledDisplays.size();i++)
					Raptor::glDestroyDisplay(m_recycledDisplays[i]);
				
				m_pDisplay->glvkUnBindDisplay();

				m_recycledDisplays.clear();
			}
			SLEEP(1);
		}
	}
}

void CRaptorServerInstance::processOutputFrame(request &r)
{
	CRaptorNetwork::SERVER_COMMAND *command = (CRaptorNetwork::SERVER_COMMAND *)r.data;
	
	const CRaptorNetwork::SESSION_COMMAND &cmd = CRaptorNetwork::getOpenSessionCommand();
	if (!strncmp(command->command, cmd.command.command, cmd.command.commandLen))
	{
		CRaptorNetwork::SESSION_COMMAND *session = (CRaptorNetwork::SESSION_COMMAND*)command;
		session->id = r.id;

		return;
	}

	const CRaptorNetwork::IMAGE_COMMAND& cmd2 = CRaptorNetwork::getImageCommand();
	if (!strncmp(command->command, cmd2.command.command, cmd2.command.commandLen))
	{
		CRaptorNetwork::IMAGE_COMMAND *image_command = (CRaptorNetwork::IMAGE_COMMAND*)r.data;
		image_command->header.compressionType = 0;

		uint8_t* image = (uint8_t*)&(image_command->pData);
		//m_pCompressor->differentialCompress(image,r.size);
		//m_pCompressor->compress(image,r.size);
		m_pCompressor->removeAlpha(image, r.size);
		r.size = (r.size / 4) * CRaptorNetwork::PIXEL_SIZE;
		r.size += sizeof(CRaptorNetwork::IMAGE_COMMAND);

		return;
	}
}

bool CRaptorServerInstance::closeSession(request_handler_t::request_id id)
{
	std::cout << "Closing session: " << id << std::endl;

	CServerSession::session request_session = m_pSessionManager->getSession(id);
	m_recycledDisplays.push_back(request_session.display);

	return m_pSessionManager->closeSession(id);
}

bool CRaptorServerInstance::executeRequest(request &r)
{
	CRaptorNetwork::SERVER_COMMAND *command = (CRaptorNetwork::SERVER_COMMAND *)r.data;
	// on the fly XML parsing is too low !!!
	const CRaptorNetwork::SERVER_COMMAND& cmd = CRaptorNetwork::getRenderCommand();
	if (!strncmp(command->command,cmd.command,cmd.commandLen))
	{
		r.render = true;
		r.reply = true;
		return true;
	}

	const CRaptorNetwork::SERVER_COMMAND& cmd2 = CRaptorNetwork::getStatusCommand();
	if (!strncmp(command->command,cmd2.command,cmd2.commandLen))
	{
		r.reply = true;
		// Not yet implemented
		return false;
	}

	const CRaptorNetwork::DATA_COMMAND& cmd3 = CRaptorNetwork::getDataPackageCommand();
	if (!strncmp(command->command, cmd3.command.command, cmd3.command.commandLen))
	{
		r.reply = false;
		CRaptorNetwork::DATA_COMMAND *data = (CRaptorNetwork::DATA_COMMAND*)r.data;
		return loadPackage(*data, r.id);
	}

	const CRaptorNetwork::SESSION_COMMAND& cmd4 = CRaptorNetwork::getOpenSessionCommand();
	if (!strncmp(command->command,cmd4.command.command,cmd4.command.commandLen))
	{
		r.reply = true;
		r.render = false;
		r.display = NULL;
		
		CRaptorNetwork::SESSION_COMMAND *session_command = (CRaptorNetwork::SESSION_COMMAND *)r.data;
		CRaptorDisplayConfig glcs;
		glcs.width = session_command->width;
		glcs.height = session_command->height;
		glcs.x = 0;
		glcs.y = 0;
		std::stringstream dname;
		dname << "Session";
		dname << r.id;
		dname << "_display";
		glcs.caption = dname.str();
		glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
		glcs.display_mode = CGL_RGBA | CGL_DEPTH;
		glcs.renderer = CRaptorDisplayConfig::RENDER_BUFFER;
		glcs.refresh_rate.fps = CGL_MAXREFRESHRATE;
		CRaptorDisplay *pDisplay = Raptor::glCreateDisplay(glcs);
		if (pDisplay == 0)
		{
			std::cout << "Raptor Render Server has no resources: hardware OpenGL rendering not supported, exiting..." << std::endl;
			return false;
		}

		RAPTOR_HANDLE handle;
		pDisplay->glvkBindDisplay(handle);
			IRenderingProperties &props = pDisplay->getRenderingProperties();
			props.setMultisampling(IRenderingProperties::ENABLE);
			props.setTexturing(IRenderingProperties::ENABLE);
			props.setLighting(IRenderingProperties::ENABLE);
			props.clear(CGL_RGBA|CGL_DEPTH);
		pDisplay->glvkUnBindDisplay();

		if (m_pSessionManager->createSession(r.id, pDisplay))
			std::cout << "New session created: " << r.id << std::endl;
		else
			std::cout << "Failed to create new session " << r.id << std::endl;
		
		CRaptorLock lock(m_rmutex);

		request reply;
		reply.display = NULL;
		reply.id = r.id;
		reply.render = false;

		session_command = new CRaptorNetwork::SESSION_COMMAND;
		session_command->id = r.id;
		session_command->width = glcs.width;
		session_command->height = glcs.height;
		session_command->command = cmd4.command;
		reply.data = session_command;
		reply.size = sizeof(CRaptorNetwork::SESSION_COMMAND);

		m_replies.push_back(reply);
		m_prodCons.P();

		return true;
	}

	return false;
}

bool CRaptorServerInstance::loadPackage(const CRaptorNetwork::DATA_COMMAND& data, request_handler_t::request_id id)
{
	std::cout << "Storing data package file to server." << std::endl;

	//!	Create session private directory to store data.
	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);
	std::stringstream session_path;
	session_path << "session_" << id;
	int dir_exist = _chdir(session_path.str().c_str());
	if ((ENOENT == errno) && (-1 == dir_exist))
	{
		_mkdir(session_path.str().c_str());
		_chdir(session_path.str().c_str());
	}

	//!	Grab package file name.
	std::streampos fsize = data.size;
	const unsigned char *pData = (unsigned char*)&data;
	std::string fname = data.packname;

	//!	Create package file.
	CRaptorIO *io = CRaptorIO::Create(fname, CRaptorIO::DISK_WRITE, CRaptorIO::BINARY);

	//!	Cannot open file
	if (io->getStatus() != CRaptorIO::IO_OK)
	{
		std::cout << "Unable to create server file " << fname << " !" << std::endl;
		return false;
	}

	io->write(&pData[data.command.requestLen], fsize);
	delete io;

	//!	Export package data.
	CRaptorDataManager *datamanager = CRaptorDataManager::GetInstance();
	datamanager->managePackage(fname);
	std::vector<std::string> files = datamanager->getManagedFiles(fname);
	//!	Package is empty
	if (0 == files.size())
	{
		_chdir(buffer);
		return false;
	}

	//!	Export all fils in package and find scene descriptor file.
	std::string scene_file = "";
	for (size_t i = 0; i < files.size(); i++)
	{
		fname = datamanager->exportFile(files[i], ".");
		size_t ext_pos = files[i].find(".xml", 0);
		if (ext_pos != std::string::npos)
			scene_file = fname;
	}
	
	CRaptorDisplay *pDisplay = NULL;
	CServerSession::session request_session = m_pSessionManager->getSession(id);
	pDisplay = request_session.display;

	//!	If no scene descriptor file found, nothing can be done, return an error.
	bool res = false;
	if ((NULL != pDisplay) && (!scene_file.empty()))
	{
		// Set a temporary name for display update
		pDisplay->setName("RaptorRenderSession");
		if (CRaptorToolBox::loadRaptorData(scene_file))
		{
			std::stringstream dname;
			dname << "Session";
			dname << id;
			dname << "_display";
			pDisplay->setName(dname.str());
			std::cout << "Session rendering scene updated." << std::endl;
			res = true;
		}
	}
	else
	{
		std::cout << "Invalid session identifier " << id << std::endl;
	}

	//!	Release package once files are obtained.
	fname = data.packname;
	datamanager->unManagePackage(fname);

	_chdir(buffer);
	return res;
}


bool CRaptorServerInstance::handleRequest(request_handler_t::request_id id, const void *data, size_t size)
{
	if (m_bStarted)
	{
		request r;
		r.id = id;
		r.reply = false;
		r.render = false;
		r.data = new uint8_t[size];
		r.size = size;
		r.display = NULL;
		memcpy(r.data, data, size);

		CRaptorLock lock(m_mutex);
		m_requests.push_back(r);
	}

	return m_bStarted;
}

bool CRaptorServerInstance::handleReply(request_handler_t::request_id id, const void *&data, size_t &size)
{
	request r;
	r.id = 0;

	if (!m_bStarted)
	{
		//	Server is stopping...
		//	... log warning/error message ?
		SLEEP(1);
		return false;
	}

	m_prodCons.V();
	CRaptorLock lock(m_rmutex);

	if (m_replies.size() < 1)
		return false;

#ifdef WIN32
	std::vector<request>::const_iterator it = m_replies.begin();
#else
	std::vector<request>::iterator it = m_replies.begin();
#endif
	do
	{
		r = *it++;
	} while ((it != m_replies.end()) && (r.id != id));

	if (r.id == id)
	{
		it--;
		m_replies.erase(it);
	}
	else
		return false;

	processOutputFrame(r);

	data = r.data;
	size = r.size;

	return true;
}

bool CRaptorServerInstance::run(void)
{
	if (m_pApplication != NULL)
	{
		return m_pApplication->run();
	}
	else
		return true;
}

bool CRaptorServerInstance::start(unsigned int width, unsigned int height)
{
	CRaptorConfig config;
	config.m_logFile = "Raptor_Server.log";
	config.m_bRelocation = true;
	config.m_uiVertices = 1000000;
	config.m_uiPolygons = 1000000;
	config.m_uiTexels = 1000000;
	config.m_uiUniforms = 100000;
	
	if (Raptor::glInitRaptor(config))
		CAnimator::SetAnimator(new CAnimator());
	else
	{
		std::cout << "Failed to initialize Raptor layer." << std::endl;
		return false;
	}

	CImaging::installImagers();
	std::cout << "Raptor Toolbox initialized. " << std::endl;

	CRaptorDisplayConfig glcs;
	glcs.width = width;
	glcs.height = height;
	glcs.x = 750;
	glcs.y = 10;
	glcs.caption = "RaptorRenderServer";
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_4X;
	glcs.swap_buffer = CRaptorDisplayConfig::SWAP_EXCHANGE;
	glcs.renderer = CRaptorDisplayConfig::NATIVE_GL;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.stencil_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;
	/*
	if (!Raptor::glCheckDisplayConfig(glcs))
	{
		std::cout << "Some hardware features are missing. Will use lower config, disabling some effects" << std::endl;
		glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_NONE;
		if (!Raptor::glCheckDisplayConfig(glcs))
		{
			std::cout << "Some hardware features are missing. Will use minimal config, disabling all advanced effects" << std::endl;
			glcs.renderer = CRaptorDisplayConfig::NATIVE_GL;
			if (!Raptor::glCheckDisplayConfig(glcs))
			{
				std::cout << "Minimum required display config cannot be created. Sorry, demo will abort. Bye." << std::endl;
				return false;
			}
		}
	}
	*/
	std::cout << "Raptor Display configuration checked, creating raptor application. " << std::endl;
	m_pApplication = CRaptorApplication::CreateApplication();
	m_pApplication->grabCursor(false);
	
	glcs.refresh_rate.fps = CGL_MAXREFRESHRATE;
	if (!m_pApplication->initApplication(glcs))
	{
		std::cout << "Raptor Render Server has no resources: hardware OpenGL rendering not supported, exiting..." << std::endl;
		return false;
	}

	m_pDisplay = m_pApplication->getRootDisplay();
	m_pWindow = m_pApplication->getRootWindow();

	std::cout << "Creating Raptor Main Display. " << std::endl;
	m_pDisplay->glvkBindDisplay(m_pWindow);

		CRaptorConsole *pConsole = Raptor::GetConsole();
		pConsole->glInit();

		pConsole->showStatus(true);
		pConsole->showFPS(true);
		pConsole->showFrameTime(true);
		pConsole->activateConsole(true);
		CGLFont::FONT_TEXT_ITEM item;
		item.text = "Pending requests: 0";
		pConsole->addItem(item);
		item.text = "Pending replies: 0";
		pConsole->addItem(item);

		IRenderingProperties &props = m_pDisplay->getRenderingProperties();
		props.setTexturing(IRenderingProperties::ENABLE);
		props.setLighting(IRenderingProperties::DISABLE);
		props.setDepthTest(IRenderingProperties::DISABLE);
		props.setCullFace(IRenderingProperties::DISABLE);
		props.clear(CGL_RGBA);
	m_pDisplay->glvkUnBindDisplay();

	if (m_pCompressor == NULL)
		m_pCompressor = new CYUVCompressor();

	std::cout << "Raptor Instance Ready. " << std::endl;
	m_bStarted = true;
	return true;
}

bool CRaptorServerInstance::closeWindow()
{
	if ((NULL != m_pApplication) && (m_pApplication->isRunning()))
	{
		std::cout << "Raptor Instance requested to close main window and quit application... " << std::endl;

		HWND wnd;
		if (NULL != m_pApplication)
			wnd = m_pApplication->getRootWindow().ptr<HWND__>();
		if (NULL != wnd)
		{
			PostMessage(wnd, WM_CLOSE, 0, 0);
			return true;
		}
		else
			return true;
	}
	else
		return false;
}

bool CRaptorServerInstance::stop(void)
{
	bool stopped = m_bStarted;

	if (m_bStarted)
	{
		m_bStarted = false;

		//! Raptor will destroy displays with appropriate context
		delete m_pSessionManager;
		m_pSessionManager = NULL;

		//! should we wait until all pending requests are satisfied ?
		CRaptorLock lock(m_mutex);
		m_requests.clear();

		CRaptorLock lock2(m_rmutex);
		m_prodCons.P();

		if (NULL != m_pApplication)
		{
			stopped = m_pApplication->quitApplication();
			delete m_pApplication;
			m_pApplication = NULL;
		}

		if (stopped)
			std::cout << "Raptor Server Instance has stopped successfully." << std::endl;
		else
			std::cout << "Raptor Server Instance has stopped with errors." << std::endl;
	}

	return stopped;
}
