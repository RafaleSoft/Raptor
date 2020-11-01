/***************************************************************************/
/*                                                                         */
/*  ServerSession.h                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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

#if !defined(AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_)
	#include "ServerSession.h"
#endif

//! Win32 directories system calls.
#include <direct.h>
#include <io.h>
#include <fcntl.h>

using namespace RaysServer;


CServerSession::CServerSession()
{

}

CServerSession::~CServerSession()
{
	if (m_sessions.size() > 0)
		std::cout << "Warning : deleting Session Manager with existing client sessions !" << std::endl;
}

CServerSession::session CServerSession::getSession(server_base_t::request_handler_t::request_id id)
{
	CRaptorLock lock(m_mutex);

	session return_session = {NULL};

	for (size_t i = 0; i<m_sessions.size(); i++)
		if (m_sessions[i].id == id)
		{
			return_session = m_sessions[i];
			break;
		}

	return return_session;
}

bool CServerSession::saveSessionFile(server_base_t::request_handler_t::request_id id, const std::string& filename, uint8_t* data, size_t size)
{
	//!	Create session private directory to store data.
	char buffer[MAX_PATH];
	if (NULL == _getcwd(buffer, MAX_PATH))
		return false;

	int mode = 0;
	errno_t err = _get_fmode(&mode);
	if (err == EINVAL)
	{
		std::cout << "Invalid parameter: mode" << std::endl;
		return false;
	}
	err = _set_fmode(_O_BINARY);
	if (err == EINVAL)
	{
		std::cout << "Warning : failed to update write mode to binary" << std::endl;
		return 1;
	}

	std::stringstream session_path;
	session_path << "session_" << id << std::ends;

	bool file_written = false;
	int dir_exist = _chdir(session_path.str().c_str());
	if (0 == dir_exist)
	{
		FILE *f = fopen(filename.c_str(), "w");
		if (NULL != f)
		{
			if (size != fwrite(data, 1, size, f))
				std::cout << "Warning : failed to write data file " << filename << " to session " << id << std::endl;
			else
				file_written = true;

			file_written = file_written && (0 == fclose(f));
		}
	}

	if (0 == dir_exist)
		dir_exist = _chdir(buffer);

	return (file_written && (0 == dir_exist));
}

bool CServerSession::createSession(server_base_t::request_handler_t::request_id id)
{
	if (0 == id)
		return false;

	CRaptorLock lock(m_mutex);

	session s;
	s.id = id;

	m_sessions.push_back(s);

	//!	Create session private directory to store data.
	char buffer[MAX_PATH];
	if (NULL == _getcwd(buffer, MAX_PATH))
		return false;

	std::stringstream session_path;
	session_path << "session_" << id << std::ends;
	int dir_exist = _chdir(session_path.str().c_str());
	if ((ENOENT == errno) && (-1 == dir_exist))
	{
		dir_exist = _mkdir(session_path.str().c_str());
		if (0 == dir_exist)
			dir_exist = _chdir(session_path.str().c_str());
	}

	if (0 == dir_exist)
		dir_exist = _chdir(buffer);

	return (0 == dir_exist);
}


bool CServerSession::closeSession(server_base_t::request_handler_t::request_id id)
{
	CRaptorLock lock(m_mutex);

	char buffer[MAX_PATH];
	if (NULL == _getcwd(buffer, MAX_PATH))
		return false;

	for (size_t i = 0; i<m_sessions.size(); i++)
	{
		if (m_sessions[i].id == id)
		{
			m_sessions.erase(m_sessions.begin() + i);

			//! Check if directory is valid.
			std::stringstream session_path;
			session_path << "session_" << id << std::ends;
			int dir_exist = _chdir(session_path.str().c_str());
			if ((ENOENT == errno) && (-1 == dir_exist))
			{
				std::cout << "Invalid session directory " << session_path.str() << " unable to remove ! " << std::endl;
				return false;
			}

			//! Find the first file in the directory.
			std::stringstream session_dir;
			session_dir << buffer << "\\";
			session_dir << "session_" << id;
			session_dir << "\\*" << std::ends;
			WIN32_FIND_DATA ffd;
			HANDLE hFind = FindFirstFile(session_dir.str().c_str(), &ffd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				std::cout << "Unable to list session directory " << session_path.str() << std::endl;
				return false;
			}

			// List all the files in the directory with some info about them.
			std::vector<std::string> files;
			do
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					files.push_back(ffd.cFileName);
			} while (FindNextFile(hFind, &ffd) != 0);

			if (GetLastError() != ERROR_NO_MORE_FILES)
				std::cout << "Some session files could not be accessed in path: " << session_path.str() << std::endl;

			FindClose(hFind);

			for (size_t i = 0; i < files.size(); i++)
			{
				if (-1 == _unlink(files[i].c_str()))
				{
					std::cout << "Unable to remove session file: " << files[i] << std::endl;
					if (ENOENT == errno)
						std::cout << "File not found" << std::endl;
					else if (EACCES == errno)
						std::cout << "Permission denied" << std::endl;
				}
			}

			// Once dir empty, remove dir.
			_chdir(buffer);
			dir_exist = _rmdir(session_path.str().c_str());
			if (-1 == dir_exist)
			{
				std::cout << "Unable to remove session directory " << session_path.str() << std::endl;
				return false;
			}

			return true;
		}
	}

	return false;
}
