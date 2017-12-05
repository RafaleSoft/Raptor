// WorkUnit.h : main header file for the WORKUNIT application
//

#if !defined(AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_)
#define AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Messages.h"

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

class CCmdLineParser;
class CRaytracerData;
class CGenericRenderObject;
class CTexture;

class CRaysWorkUnit : public CClient<CClientSocket>
{
public:
	CRaysWorkUnit();
	virtual ~CRaysWorkUnit();

	//!	Request Deamon stop.
	void requestExit() { m_bExit = true; };

	//!	Return exit status.
	bool doExit(void) const { return m_bExit; };

	//!	Starts WorkUnit dialog with deamon.
	//!	Return fals on any connection failure.
	bool start(const CCmdLineParser &cmdline);

	const CRaytracerData* getRaytracerData(void) const
	{ return raytracer_data; };

private:
	void ProcessMsg(MSGSTRUCT& msg, unsigned char raw_data[]);

	bool RunRaytrace(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildSpheres(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildLights(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildCamera(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildGeometries(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildTextures(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildPlugins(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildFrames(MSGSTRUCT& msg,unsigned char raw_data[]);

	//!	Helpers
	bool GetRaytraceData();
	void SetTextures(CGenericRenderObject* obj, rays_objectbase_t& base);
	CTexture *GetTexture(const std::string& tname);

	//!	Exit WorkUnit request
	bool	m_bExit;

	//!	GLobal data
	CRaytracerData*	raytracer_data;
};


#endif // !defined(AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_)
