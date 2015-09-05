// FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "FileManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileManager::CFileManager()
{
	m_totalCacheSize = DEFAULT_CACHE_SIZE;
	m_ServerStorage = "RaysFileMgr/";

	m_fs = new CSystem();

	if (0 == m_fs->SetCurrentDirectory(LPCTSTR(m_ServerStorage), NULL))
	{
		if (0 == m_fs->CreateDirectory( LPCTSTR(m_ServerStorage), NULL,NULL))
			AfxMessageBox("Rays server is unable to manage local files");

		m_fs->SetCurrentDirectory(LPCTSTR(m_ServerStorage), NULL);
		m_fs->ScanCurrentDirectory();
	}
	else
	{
		m_fs->ScanCurrentDirectory();

		CEntity *ent = m_fs->DirList.GetNextEntity(true);

		for (int i=0;i<m_fs->DirList.NbEnt();i++)
		{
			if ((ent->Info->attrib & _A_ARCH) && (ent->Name.Right(4) == ".ray"))
			{
				CFile _file(LPCTSTR(ent->Name),CFile::modeRead);
				UINT uint = 0;
				RAYSFILETYPE type;
				_file.Read(&uint,sizeof(UINT));
				_file.Read(&uint,sizeof(UINT));	
				_file.Read(&type,sizeof(UINT));
				_file.Close();

				LPRAYSFILE rays_file = new RAYSFILE;
				rays_file->url = "";
				rays_file->storage = "RaysFileMgr/";
				rays_file->name = ent->Name.Left(ent->Name.GetLength()-4);
				rays_file->fileType = type;
				rays_file->fileDate = ent->Info->time_write;
				rays_file->isLocked = false;
				rays_file->priority = 0;
				rays_file->pos = 0;
				rays_file->size = ent->Info->size - 12;
				rays_file->cache = NULL;

				m_files.SetAt( rays_file->name, rays_file );
			}

			ent = m_fs->DirList.GetNextEntity();
		}
	}
}

CFileManager::~CFileManager()
{

}

bool CFileManager::RemoveFile(CString filename)
{
	bool res = false;

	if ((TRUE == m_fs->DeleteFile(LPCTSTR(filename))) &&
		TRUE == m_fs->DeleteFile(LPCTSTR(filename+".ray")))
	{
		m_fs->DirList.Scan(NULL,NULL);

		LPRAYSFILE rays_file = NULL;
		res = (TRUE == m_files.Lookup( filename, (void*&)rays_file ));
		if (res)
			m_files.RemoveKey( filename );

		if (rays_file != NULL)
			delete rays_file;
		res = true;
	}

	return res;
}


bool CFileManager::CreateFile(CString filename,RAYSFILETYPE type,UINT size, void *data)
{
	return CreateFile(filename,type,size,1,data);
}

bool CFileManager::CreateFile(CString filename,RAYSFILETYPE type,UINT image_width,UINT image_height, void *data)
{
	CEntity *ent = m_fs->DirList.Find(filename);
	bool success = true;

	if (ent != NULL)
		return false;

	try
	{
		CFile _rayfile(LPCTSTR(filename+".ray"),CFile::modeCreate|CFile::modeWrite);

		UINT filesize = image_width * image_height;
		_rayfile.Write(&image_width,sizeof(UINT));
		_rayfile.Write(&image_height,sizeof(UINT));
		_rayfile.Write(&type,sizeof(UINT));
		_rayfile.Flush();
		_rayfile.Close();

		CFile _file(LPCTSTR(filename),CFile::modeCreate|CFile::modeWrite);
		_file.Write(data,filesize);
		_file.Flush();
		_file.Close();

		LPRAYSFILE rays_file = new RAYSFILE;
		rays_file->url = "";
		rays_file->storage = "RaysFileMgr/";
		rays_file->name = filename;
		rays_file->fileType = type;
		rays_file->fileDate = CTime::GetCurrentTime().GetTime();
		rays_file->isLocked = false;
		rays_file->priority = 0;
		rays_file->pos = 0;
		rays_file->size = filesize;
		rays_file->cache = NULL;

		m_files.SetAt( filename, rays_file );
	}
	catch (...)
	{
		success = false;
	}

	m_fs->DirList.Scan(NULL,NULL);

	return success; 
}

bool CFileManager::GetFile(CString filename,UINT &image_width,UINT &image_height,unsigned char *&buffer)
{
	CEntity *ent = m_fs->DirList.Find(filename+".ray");
	bool success = true;

	if (ent == NULL)
	{
		LPRAYSFILE rays_file = NULL;

		BOOL res = m_files.Lookup( filename, (void*&)rays_file );
		if (res == TRUE)
			m_files.RemoveKey( filename );

		if (rays_file != NULL)
			delete rays_file;

		return false;
	}

	try
	{
		CFile _rayfile(LPCTSTR(filename+".ray"),CFile::modeRead);
		RAYSFILETYPE type;
		_rayfile.Read(&image_width,sizeof(UINT));
		_rayfile.Read(&image_height,sizeof(UINT));
		_rayfile.Read(&type,sizeof(UINT));
		_rayfile.Close();

		CFileStatus status;
		CFile _file(LPCTSTR(filename),CFile::modeRead);
		_file.GetStatus(status);
		UINT filesize = status.m_size;
		//RAYSFILETYPE type;
		//_file.Read(&image_width,sizeof(UINT));
		//_file.Read(&image_height,sizeof(UINT));
		//_file.Read(&type,sizeof(UINT));

		//filesize = image_width * image_height;
		buffer = new unsigned char [filesize];
		_file.Read(buffer,filesize);
		_file.Close();
	}
	catch (...)
	{
		success = false;
	}

	return success;
}

CFileManager::LPRAYSFILE CFileManager::GetFile(UINT numFile,CFileManager::RAYSFILETYPE type) const
{
	LPRAYSFILE	rays_file = NULL;
	CString rKey;
	POSITION pos = m_files.GetStartPosition( );

	bool found = false;
	UINT nFile = 0;

	while ((!found)&&(pos != NULL))
	{
		m_files.GetNextAssoc( pos, rKey, (void*&) rays_file );
		if (rays_file != NULL)
			if (rays_file->fileType == type)
			{
				found = (nFile == numFile);
				nFile++;
			}
	}

	if (found)
		return rays_file;
	else
		return NULL;
}

CFileManager::LPRAYSFILE CFileManager::GetFile(CString filename) const
{
	LPRAYSFILE	rays_file = NULL;
	CString rKey;
	POSITION pos = m_files.GetStartPosition( );

	bool found = false;
	UINT nFile = 0;

	while ((!found)&&(pos != NULL))
	{
		m_files.GetNextAssoc( pos, rKey, (void*&) rays_file );
		if (rays_file != NULL)
		{
			found = (rays_file->name == filename);
		}
	}

	if (found)
		return rays_file;
	else
		return NULL;
}

UINT CFileManager::GetNbFiles(RAYSFILETYPE type) const
{
	UINT res = 0;

	POSITION pos = m_files.GetStartPosition( );
	LPRAYSFILE rays_file = NULL;
	CString rKey;

	while (pos != NULL)
	{
		m_files.GetNextAssoc( pos, rKey, (void*&) rays_file );
		if (rays_file != NULL)
			if (rays_file->fileType == type)
				res++;
	}

	return res;
}