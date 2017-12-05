// FileManager.h: interface for the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMANAGER_H__53C9C658_2ECF_4C22_B392_A1F2AB6EEEB4__INCLUDED_)
#define AFX_FILEMANAGER_H__53C9C658_2ECF_4C22_B392_A1F2AB6EEEB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	10Mb default reserved cache size in memory
const unsigned int DEFAULT_CACHE_SIZE = 10000000;

class CFileManager
{
public:

	typedef enum
	{
		TEXTURE,
		PLUGIN
	} RAYSFILETYPE;

	typedef struct RAYSFILE
	{
		CString			url;		// file origin
		CString			name;		// file name for string map
		CString			storage;	// server file storage
		RAYSFILETYPE	fileType;
		time_t			fileDate;	// file modify date ( time_t )
		bool			isLocked;
		float			priority;
		UINT			pos;
		UINT			size;
		UCHAR			*cache;		// near file data
	} rays_file_t;

	typedef RAYSFILE* LPRAYSFILE;

public:
	CFileManager();
	virtual ~CFileManager();

	UINT	GetNbFiles(void) const { return m_files.GetCount(); };
	UINT	GetNbFiles(RAYSFILETYPE type) const;

	CFileManager::LPRAYSFILE	GetFile(UINT numFile,CFileManager::RAYSFILETYPE type) const;
	CFileManager::LPRAYSFILE	GetFile(CString fname) const;

	UINT	GetCacheSize(void) const { return m_totalCacheSize; };
	bool	SetCacheSize(UINT /*maxSize*/) { return false; };

	bool	CreateFile(CString filename,RAYSFILETYPE type,UINT image_width, UINT image_height,void *data);
	bool	CreateFile(CString filename,RAYSFILETYPE type,UINT size,void *data);

	bool	GetFile(CString filename,UINT &image_width,UINT &image_height,unsigned char *&buffer);

	bool	RemoveFile(CString filename);

private:


	// File System 
	CSystem				*m_fs;

	// The real file storage
	CMapStringToPtr		m_files;

	UINT				m_totalCacheSize;
	CString				m_ServerStorage;

};

#endif // !defined(AFX_FILEMANAGER_H__53C9C658_2ECF_4C22_B392_A1F2AB6EEEB4__INCLUDED_)
