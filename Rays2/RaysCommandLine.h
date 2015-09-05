#ifndef __RAYS_COMMANd_LINE_H__
#define __RAYS_COMMANd_LINE_H__

class RaysCmdLine : public CCommandLineInfo
{
public:
	RaysCmdLine():foundIP(false) {};
	~RaysCmdLine() {};

	virtual void ParseParam(LPCTSTR lpszParam,BOOL bFlag,BOOL bLast);

	CString	addrStr;
	bool	foundIP;
};

void RaysCmdLine::ParseParam(LPCTSTR lpszParam,BOOL bFlag,BOOL bLast)
{
	if ((bFlag) && (!strcmp(lpszParam,"ip")))
		foundIP = true;
	else if (foundIP)
	{
		addrStr = lpszParam;
		foundIP = false;
	}
	else
		CCommandLineInfo::ParseParam(lpszParam,bFlag,bLast);
	return;
}

#endif