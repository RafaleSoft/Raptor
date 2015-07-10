// ClientCmdLine.h: interface for the ClientCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTCMDLINE_H__D2547634_67C1_4608_8A82_6360CFBF4A42__INCLUDED_)
#define AFX_CLIENTCMDLINE_H__D2547634_67C1_4608_8A82_6360CFBF4A42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using std::string;

class ClientCmdLine
{
public:
	ClientCmdLine();
	virtual ~ClientCmdLine();

    virtual void Parse(int argc, char *argv[]);

	string	addrStr;
    unsigned short port;
	unsigned short width;
	unsigned short height;
	unsigned short r_width;
	unsigned short r_height;
	string	file;
};

#endif // !defined(AFX_CLIENTCMDLINE_H__D2547634_67C1_4608_8A82_6360CFBF4A42__INCLUDED_)

