// MiniRaysClient.h : main header file for the MINIRAYSCLIENT application
//

#if !defined(AFX_MINIRAYSCLIENT_H__3E6D53A2_2BB7_4CCC_AA9D_C28FBD1113E6__INCLUDED_)
#define AFX_MINIRAYSCLIENT_H__3E6D53A2_2BB7_4CCC_AA9D_C28FBD1113E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Rays2_lex.h"
#include "Rays2_yacc.h"
#include "Rays2_tree.h"
#include "../Systemdll/Systemdll.h"	//	client server
#include "..\Messages.h"
#include <iostream.h>				// cout
#include <afxtempl.h>				// CArray
#include "CltSocket.h"
#include "..\Raptor\Include\Raptor.h"

class C3DSet;
class CMiniRaysClientDlg;
class MiniRaysClientStatus;

class CParser : public CLex
{
public:
	CParser():output(NULL) {};
	~CParser() {};

	void OnError(CLex::LexError err);

public:
	CMiniRaysClientDlg	*output;
};

class CAnalyser : public CYacc
{
public:
	CMiniRaysClientDlg	*output;

public:
	CAnalyser(CParser *p,CClient<CCltSocket> *c);
	~CAnalyser() {};

	void InitAnalyse();
	void Verbose(bool active) { verbose = active; };
	

	void StartJob(unsigned int jobID);
	void RunJob(unsigned int jobID);
	void SendLights(unsigned int jobID);
	void SendSpheres(unsigned int jobID);
	void SendCompositions(unsigned int jobID);
	void SendConfiguration(unsigned int jobID);
	void SendPlugins(unsigned int jobID);
	void SendFrames(unsigned int jobID);
		
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
	CMapStringToPtr	plugins;

	rays_frame_t		currentFrame;
	rays_plugin_t		currentPlugins[16];
	rays_transform_t	currentTransforms[16];

	CMapWordToPtr	frames;

	CClient<CCltSocket> *client;

private:
	//	Helpers
	void __fastcall updateShading(rays_shading_t &r_shading);
	void __fastcall updateTexture(rays_texture_t &r_texture);
	void __fastcall updateNormal(rays_texture_t &r_texture);

	virtual void OnError(CYacc::YaccError err);

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
	virtual void Onplugin_call(UINT);
	virtual void Onframe_element(UINT);
	virtual void Onframes(UINT);
	virtual void Onmain(UINT);
};

/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientApp:
// See MiniRaysClient.cpp for the implementation of this class
//

class CMiniRaysClientApp : public CWinApp
{
public:
	CMiniRaysClientApp();

	bool Connect(CString IP,int port);
	bool Disconnect(void);
	void Render(void);
	void Status(void);
	void Upload(void);
	CString CheckUpload(void);

	void Getjob(unsigned int jobID);
	void Deljob(unsigned int jobID);
	void QueryStatus(void);
	void QueryUpload(void);

	void Process_JOB_STATUS(MSGSTRUCT msg,unsigned char raw_data[]);
	void Process_ACKNOWLEDGE(MSGSTRUCT msg,unsigned char raw_data[]);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniRaysClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMiniRaysClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	MiniRaysClientStatus	*status;
	CMiniRaysClientDlg		*output;

	CString					scene_location;
	CString					scene;

	bool					correct_module_upload;
	bool					correct_texture_upload;
	CString					upload_location;
	CString					upload;

private:
	CClient<CCltSocket>	*client;

	CParser				*parser;
	CAnalyser			*analyser;

	bool				connected;
	unsigned int		jobID;

	//	job prepare status
	bool				config_sent;
	bool				sphere_sent;
	bool				light_sent;
	bool				composition_sent;
	bool				plugin_sent;
	bool				frame_sent;

	bool				config_ack;
	bool				sphere_ack;
	bool				light_ack;
	bool				composition_ack;
	bool				plugin_ack;
	bool				frame_ack;

	//	job result ( for saving )
	unsigned char		*image;
	unsigned int		image_size;
	unsigned int		image_height;
	unsigned int		image_width;

	//	uploads
	LP_GL_TEXTURE_OBJECT	T;

	bool Savejob(CString fname);
};

extern CMiniRaysClientApp miniRaysClient;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIRAYSCLIENT_H__3E6D53A2_2BB7_4CCC_AA9D_C28FBD1113E6__INCLUDED_)
