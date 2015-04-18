// ServerCmdLine.h: interface for the ServerCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERCMDLINE_H__3FAF402E_EBC9_4BEF_B369_8D2CBD677141__INCLUDED_)
#define AFX_SERVERCMDLINE_H__3FAF402E_EBC9_4BEF_B369_8D2CBD677141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using std::string;

class ServerCmdLine
{
public:
	ServerCmdLine();
	virtual ~ServerCmdLine();

    virtual void Parse(int argc, char *argv[]);

	string	addrStr;
    unsigned int port;
	unsigned int width;
	unsigned int height;

private:
    bool parseIP;
    bool parsePort;
	bool parseWidth;
	bool parseHeight;
};

#endif // !defined(AFX_SERVERCMDLINE_H__3FAF402E_EBC9_4BEF_B369_8D2CBD677141__INCLUDED_)
