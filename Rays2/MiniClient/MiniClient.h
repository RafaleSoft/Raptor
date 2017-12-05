
#if !defined(AFX_MINICLIENT_H__519257E3_54C3_4640_8153_53A7E61BC259__INCLUDED_)
#define AFX_MINICLIENT_H__519257E3_54C3_4640_8153_53A7E61BC259__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "lexyacc.h"
#include "../Systemdll/Systemdll.h"	//	client server
#include "..\Messages.h"
#include <iostream.h>				// cout
#include <afxtempl.h>				// CArray

class C3DSet;

class CParser : public CLex
{
public:
	CParser() {};
	~CParser() {};

	void OnError(CLex::LexError err)
	{
		switch(err)
		{
		case CLex::LEX_NOERR:
			cout << "No error found." << endl;
			break;
		case CLex::LEX_ERR_SRC:
			cout << "Error reading source file." << endl;
			break;
		case CLex::LEX_ERR_SYN:
			cout << "Syntax error in lex file." << endl;
			break;
		default:
			cout << "Unknown Lex error." << endl;
			break;
		}
	}
};

class CAnalyser : public CYacc
{
public:
	unsigned int	jobID;


public:
	CAnalyser(CParser *p,CClient<CClientSocket> *c);
	~CAnalyser() {};

	void InitAnalyse();
	void Verbose(bool active) { verbose = active; };
	
		
private:
	bool			verbose;
	rays_config_t	config;

	MSGSTRUCT		msg;

	CArray<vertex_t,vertex_t&>
					vertices;
	CArray<triangle_t,triangle_t&>
					triangles;

	//	.3DS file conversion with Raptor
	C3DSet			*set;

	//	data transmission
	CMapStringToPtr	lights;
	CMapStringToPtr	shadings;
	CMapStringToPtr	spheres;
	CMapStringToPtr	textures;
	CMapStringToPtr	compositions;

	CClient<CClientSocket> *client;

private:
	void StartJob();
	void RunJob();
	void SendLights();
	void SendSpheres();
	void SendCompositions();

	//	Helpers
	void __fastcall updateShading(rays_shading_t &r_shading);
	void __fastcall updateTexture(rays_texture_t &r_texture);

	virtual void OnError(CYacc::YaccError err)
	{
		switch(err)
		{
		case CYacc::YACC_NOERR:
			cout << "No error found." << endl;
			break;
		case CYacc::YACC_ERR_LEX:
			cout << "Internal lex error." << endl;
			break;
		case CYacc::YACC_ERR_SYN:
			cout << "Syntax error in lex file." << endl;
			break;
		default:
			cout << "Unknown Lex error." << endl;
			break;
		}
	}

	virtual int	MapTypeinteger(CString toMap) const
		{ return atoi(LPCTSTR(toMap)); };
	virtual float	MapTypefloat(CString toMap) const
		{ return (float)(atof(LPCTSTR(toMap))); };
	virtual CString	MapTypestring(CString toMap) const
		{ return toMap; };

	//	actions for each element parsed
	virtual void Oncoord(UINT);
	virtual void Onvertex(UINT);
	virtual void Ontriangle(UINT);
	virtual void Onshading(UINT);
	virtual void Onlight(UINT);
	virtual void Ontexture(UINT);
	virtual void Onelementname(UINT);
	virtual void Onelement(UINT);
	virtual void Onconfig(UINT) ;
	virtual void Onobjectbase(UINT);
	virtual void Onobject(UINT);
	virtual void Oncomposition(UINT);
	virtual void Onsphere(UINT);
	virtual void Onextern(UINT);
	virtual void Onmain(UINT);
};

#endif // !defined(AFX_MINICLIENT_H__519257E3_54C3_4640_8153_53A7E61BC259__INCLUDED_)
