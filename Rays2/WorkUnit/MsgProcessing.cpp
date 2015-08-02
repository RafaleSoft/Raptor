#include "stdafx.h"
#include "WorkUnit.h"
#include "WorkUnitDlg.h"
#include <fstream.h>	// ofstream
#include "..\systemdll\systemdll.h"
#include "WUSocket.h"
#include "GenericLight.h"
#include "Quadric.h"
#include "Triangle.h"
#include "Mesh.h"
#include "SSE_CMesh.h"
#include "Texture.h"
#include "Environment.h"
#include "Surfacer.h"
#include "..\PlugIn.cxx"

//	for log/debug
extern CClient<CWUSocket>		client;
extern ofstream			COUT;
extern RAYTRACERDATA	raytracer_data;
extern UINT				Raytrace( LPVOID pParam );
extern unsigned int		ip;
extern unsigned int		ID;

static unsigned int		nbTextureLoad = 0;
static unsigned int		nbPluginLoad = 0;


void CWorkUnitApp::ProcessMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString _msg;
	bool acknowledge = true;
	bool sendacknowledge = true;

	_msg = "Receiving data from server";
	WriteMessage(_msg);
	COUT << "# Processing server Message:" << msg.msg_id << endl;
	switch(msg.msg_id)
	{
		case OBJ_DATA:
			{
				switch (msg.msg_data[1])
				{
					case OBJ_PLUGIN:
						_msg = "Processing plugin objects";
						WriteMessage(_msg);
						acknowledge = BuildPlugins(msg,raw_data);
						break;
					case OBJ_CAMERA:
						_msg = "Processing camera object";
						WriteMessage(_msg);
						acknowledge = BuildCamera(msg,raw_data);
						break;
					case OBJ_LIGHT:
						_msg = "Processing light object";
						WriteMessage(_msg);
						acknowledge = BuildLights(msg,raw_data);
						break;
					case OBJ_SPHERE:
						_msg = "Processing sphere object";
						WriteMessage(_msg);
						acknowledge = BuildSpheres(msg,raw_data);
						break;
					case OBJ_GEOMETRY:
						_msg = "Processing geometry object";
						WriteMessage(_msg);
						acknowledge = BuildGeometries(msg,raw_data);
						break;
					case OBJ_FRAME:
						_msg = "Processing scene frames";
						WriteMessage(_msg);
						acknowledge = BuildFrames(msg,raw_data);
						break;
					default:
						{
							COUT << "Unsupported object id" << msg.msg_data[1] << endl;

							_msg = "Unsupported object id";
							acknowledge = false,
							WriteMessage(_msg);
							_msg.Format("msg.id: %x - data[0]:%x - data[1]:%x",msg.msg_id,msg.msg_data[0],msg.msg_data[1]); 
							WriteMessage(_msg);
						}
				}
				_msg = "Data processed";
				WriteMessage(_msg);
			}
			msg.msg_id = ACK_OBJ;
			break;
		case OBJ_TEXTURE:
			_msg = "Processing texture object";
			WriteMessage(_msg);
			acknowledge = BuildTextures(msg,raw_data);
			sendacknowledge = false;
			break;
		case JOB_RUN:
			acknowledge = RunRaytrace(msg,raw_data);
			msg.msg_data[1] = JOB_RUN;
			msg.msg_id = ACK_JOB;
			break;
		case ACK_NONE:
			if (msg.msg_data[0] == OBJ_TEXTURE)
			{
				COUT << "Texture query failed" << endl;
				_msg = "Server unable to execute request (wrong/none texture ?)";
				WriteMessage(_msg);
				//acknowledge = BuildTextures(msg,raw_data);
				if (nbTextureLoad > 0)
					nbTextureLoad--;
			}
			else if (msg.msg_data[0] == OBJ_PLUGIN)
			{
				COUT << "Plugin query failed" << endl;
				_msg = "Server unable to execute request (wrong/none plugin ?)";
				WriteMessage(_msg);
				//acknowledge = BuildTextures(msg,raw_data);
				if (nbPluginLoad > 0)
					nbPluginLoad--;
			}
			sendacknowledge = false;
			break;
		default:
			acknowledge = false;
			_msg = "Unsupported message id";
			WriteMessage(_msg);
			COUT << "# Unsupported message ID: " << msg.msg_id << endl;
			break;
	}

	if (sendacknowledge)
	{
		COUT << "Sending aknowledgment..." << endl;

		if (!acknowledge)
			msg.msg_id = ACK_NONE;

		msg.msg_size = 0;
		// msg.msg_data[0] should contain jobID
		msg.msg_data[2] = 0;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = ip;
		msg.msg_tail = MSG_END;

		COUT << "Acknowledge datas: " << msg.msg_header << " " << msg.msg_id << " " << msg.msg_size << " " << msg.msg_tail << endl;
		client.Write(&msg,MSGSIZE);

		COUT << "Acknowledge sent !" << endl;
	}

	if (raw_data!=NULL)
		delete [] raw_data;

	COUT << "Data cleaned !" << endl;

	if (ACK_JOB == msg.msg_id)
		GetRaytraceData();
}


//////////////////////////////////////////////////////////////////////
//
//	Job start
//
//////////////////////////////////////////////////////////////////////
bool CWorkUnitApp::RunRaytrace(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString _msg;

	//	is raytracer ready ?
	COUT << "Running raytracer ..." << endl;
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		COUT << "Renderer Busy !!!" << endl;
		_msg.Format("Renderer busy !!!");
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		//	Rendering
		raytracer_data.start = msg.msg_data[1];
		raytracer_data.end = msg.msg_data[2];
		COUT << "Raytrace initialisation... " << endl;
		raytracer = AfxBeginThread(	Raytrace, 
									(void*)&raytracer_data, 
									THREAD_PRIORITY_NORMAL, 
									0, 
									CREATE_SUSPENDED, 
									NULL );
		_msg.Format("Rendering from scanline %d to %d ( %d x %d )",raytracer_data.start,raytracer_data.end,raytracer_data.camera.width,raytracer_data.camera.height);
		WriteMessage(_msg);

		dlg->SetTimer(1,2000,NULL);

		raytracer_data.lock->SetEvent();
		return true;
	}
}

bool CWorkUnitApp::GetRaytraceData()
{
	//	Verifying textures downloading...
	CString tname;
	CTexture *txt;

	CString _msg = "Querying Raytrace additional datas... ";
	WriteMessage(_msg);

	POSITION pos = raytracer_data.textures.GetStartPosition();
	MSGSTRUCT msg;

	if (pos == NULL)
	{
		COUT << "No data needed, jumping to rendering" << endl;
		MSGSTRUCT msg;
		msg.msg_id = ACK_NONE;
		BuildTextures(msg,NULL);
	}
	else
		COUT << "Data needed, querying to server" << endl;

	while (pos != NULL)
	{
		raytracer_data.textures.GetNextAssoc( pos, tname, (void*&)txt );

		//	Query texture to server
		int len = tname.GetLength();
		unsigned char *buffer = new unsigned char[len+MSGSIZE];
		msg.msg_header = MSG_START;
		msg.msg_id = IMG_REQUEST;
		msg.msg_size = len;
		msg.msg_data[0] = OBJ_TEXTURE;
		msg.msg_data[1] = 0;
		msg.msg_data[2] = 0;
		msg.msg_data[3] = client.GetPort();
		msg.msg_data[4] = ip;
		msg.msg_tail = MSG_DATA;

		COUT << "Querying texture: " << LPCTSTR(tname) << endl;

		memcpy(buffer,&msg,MSGSIZE);
		memcpy(buffer+MSGSIZE,LPCTSTR(tname),len);
		client.Write(buffer,MSGSIZE+len);

		delete [] buffer;

		_msg = "Query sent for texture " + tname;
		WriteMessage(_msg);
	}

	//	Verifying plugins...
	pos = raytracer_data.plugins.GetStartPosition();
	while (pos != NULL)
	{
		PLUGIN *p = NULL;
		CString name;

		raytracer_data.plugins.GetNextAssoc(pos,name,(void*&)p);
		int deltaLoad = 0;

		if (p->moduleInstance == NULL)
		{
			deltaLoad++;
			*p = OpenPlugin(name);
		}
		if (p->moduleInstance == NULL)
			COUT << "Unable to load plugin " << LPCTSTR(name) << endl;
		else
		{
			nbPluginLoad -= deltaLoad;
			COUT << "Plugin " << LPCTSTR(name) << " loaded. " << endl;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//	Objects rebuilding
//
//////////////////////////////////////////////////////////////////////
bool CWorkUnitApp::BuildCamera(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		rays_config_t config;
		memcpy(&config,raw_data,msg.msg_size);
							
		raytracer_data.camera.width = config.width;
		raytracer_data.camera.height = config.height;
		raytracer_data.camera.variance = config.variance;
		raytracer_data.camera.reflection_depth = config.deflection;
		raytracer_data.camera.refraction_depth = config.defraction;
		raytracer_data.camera.focale = config.focale;
		raytracer_data.camera.object_plane = config.object_plane;
		raytracer_data.camera.crease = (float)(PI_SUR_180 * config.crease);
		raytracer_data.camera.photon_map = config.photon_map;

		CTexture *txt = NULL;
		CString tname = config.envtexname;
COUT << "Env texture: " << config.envtexname << endl;
		if (!tname.IsEmpty())
			txt = GetTexture(tname);
		CEnvironment::GetInstance()->SetTexture(txt);

		raytracer_data.lock->SetEvent();
		return true;
	}
}


bool CWorkUnitApp::BuildLights(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		rays_light_t tlight;
		unsigned int bufferpos=0;
		unsigned int nblights = msg.msg_data[2];
		CGenericLight *l;
		CString _msg;

		for (unsigned int i=0;i<nblights;i++)
		{
			memcpy(&tlight,raw_data+bufferpos,sizeof(rays_light_t));
			bufferpos+=sizeof(rays_light_t);
			l = new CGenericLight(tlight);
			raytracer_data.lights.Add(l);
			_msg.Format("light: %f-%f-%f-%f",tlight.position.x,tlight.position.y,tlight.position.z,tlight.intensity);
			WriteMessage(_msg);
			COUT << "Light added" << endl;
		}

		raytracer_data.lock->SetEvent();
		return true;
	}
}


bool CWorkUnitApp::BuildSpheres(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		rays_sphere_t tsphere;
		unsigned int bufferpos=0;
		unsigned int nbspheres = msg.msg_data[2];
		CQuadric *q;
		CString _msg;

		for (unsigned int i=0;i<nbspheres;i++)
		{
			memcpy(&tsphere,raw_data+bufferpos,sizeof(rays_sphere_t));
			bufferpos+=sizeof(rays_sphere_t);

			q = new CQuadric(tsphere);
			SetTextures(q,tsphere.base);

			raytracer_data.objects.Add(q);
			_msg.Format("sphere: %f-%f-%f-%f",tsphere.center.x,tsphere.center.y,tsphere.center.z,tsphere.radius);
			WriteMessage(_msg);
			COUT << "Surface added" << endl;
		}

		raytracer_data.lock->SetEvent();
		return true;
	}
}

bool CWorkUnitApp::BuildFrames(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		unsigned int bufferpos=0;
		unsigned int nbframes = msg.msg_data[2];

		CString _msg;

		for (unsigned int i=0;i<nbframes;i++)
		{
			rays_frame_t *pFrame = new rays_frame_t;

			memcpy(pFrame,raw_data+bufferpos,sizeof(rays_frame_t));
			bufferpos+=sizeof(rays_frame_t);

			raytracer_data.frames.SetAt(i,pFrame);

			pFrame->calls = new rays_plugin_t[pFrame->nbCalls];
			for (int j=0;j<pFrame->nbCalls;j++)
			{
				memcpy(&(pFrame->calls[j]),raw_data+bufferpos,sizeof(rays_plugin_t));
				bufferpos += sizeof(rays_plugin_t);
			}

			pFrame->transforms = new rays_transform_t[pFrame->nbTransforms];
			for (j=0;j<pFrame->nbTransforms;j++)
			{
				memcpy(&(pFrame->transforms[j]),raw_data+bufferpos,sizeof(rays_transform_t));
				bufferpos += sizeof(rays_transform_t);
			}

			_msg.Format("frame: %d",i);
			WriteMessage(_msg);
			COUT << "Frame added" << endl;
		}

		raytracer_data.lock->SetEvent();
		return true;
	}
}


bool CWorkUnitApp::BuildGeometries(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		rays_composition_header_t tgeometry;
		unsigned int bufferpos=0;
		unsigned int size = 0;
		unsigned int nbcomposition = msg.msg_data[2];
		
		CString		_msg;
		CMesh		*m = NULL;
		SSE_CMesh	*mm = NULL;

		for (unsigned int i=0;i<nbcomposition;i++)
		{
			memcpy(&tgeometry,raw_data+bufferpos,sizeof(rays_composition_header_t));
			bufferpos+=sizeof(rays_composition_header_t);

			tgeometry.geometry = new rays_vertex_t[tgeometry.geometrySize];
			tgeometry.mesh = new rays_triangle_t[tgeometry.meshSize];

			size = tgeometry.geometrySize * sizeof(rays_vertex_t);
			memcpy(tgeometry.geometry,raw_data+bufferpos,size);
			bufferpos += size;

			size = tgeometry.meshSize * sizeof(rays_triangle_t);
			memcpy(tgeometry.mesh,raw_data+bufferpos,size);
			bufferpos += size;

			if (raytracer_data.use_sse)
			{
				mm = new SSE_CMesh(tgeometry);
				SetTextures(mm,tgeometry.base);
			}
			else
			{
				m = new CMesh(tgeometry);
				SetTextures(m,tgeometry.base);
			}

			COUT << "Geometry size: " << tgeometry.geometrySize << endl;
			COUT << "Mesh size: " << tgeometry.meshSize << endl;

			_msg = "Creating mesh geometry...";
			WriteMessage(_msg);
			if (raytracer_data.use_sse)
			{
				for (int j=0;j<tgeometry.meshSize;j++)
					mm->AddTriangle(tgeometry.mesh[j],tgeometry.geometry);
				raytracer_data.objects.Add(mm);
			}
			else
			{
				for (int j=0;j<tgeometry.meshSize;j++)
					m->AddTriangle(tgeometry.mesh[j],tgeometry.geometry);
				raytracer_data.objects.Add(m);
			}

			COUT << "Mesh geometry added" << endl;

			delete [] tgeometry.geometry;
			delete [] tgeometry.mesh;
		}

		COUT << "Build Geometries done !" << endl;

		raytracer_data.lock->SetEvent();
		return true;
	}
}

bool CWorkUnitApp::BuildTextures(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		raytracer_data.lock->ResetEvent();

		if (msg.msg_id != ACK_NONE)
		{
			COUT << "Building Texture..." << endl;

			int len = msg.msg_data[2];
			char *tname = new char[len+1];

			memcpy(tname,&raw_data[0],len);
			tname[len] = 0;

			CString _msg;
			_msg.Format("Downloading texture: %s",tname);
			WriteMessage(_msg);

			COUT << "Texture name: " << tname << endl;
			COUT << "Texture size: " << msg.msg_size - len << endl;

			CTexture *txt = NULL;
			if (0 != raytracer_data.textures.Lookup( tname, (void*&) txt))
			{
				txt->SetTexture(msg.msg_data[0],msg.msg_data[1],&raw_data[len]);
			}
			else
			{
				_msg = "Internal error : texture object not created !";
				WriteMessage(_msg);
			}

			COUT << "Texture is built !" << endl;
		}

		if (nbTextureLoad > 0)
			nbTextureLoad--;

		COUT << "Still need " << nbTextureLoad << " textures" << endl;
		COUT << "Still need " << nbPluginLoad << " plugins" << endl;

		if (nbTextureLoad == 0)
		{
			COUT << "Raytrace start... " << endl;
			if (nbPluginLoad > 0)
			{
				CString _msg = "WARNING : there still are some missing plugins. Check Rays2 install";
				COUT <<  LPCTSTR(_msg) << endl;
				WriteMessage(_msg);
			}
			raytracer->ResumeThread();
			COUT << "Raytrace running... " << endl;
		}
		else
			raytracer_data.lock->SetEvent();

		return true;
	}
}

void CWorkUnitApp::SetTextures(CGenericRenderObject* obj,rays_objectbase_t& base)
{
	COUT << "Diffuse Texture type: " << base.texture.type << endl;

	CTexture *txt = GetTexture(base.texture.texture.name);
	obj->SetTexture(txt);

	COUT << "Normal Texture type: " << base.normal.type << endl;

	if (base.normal.type == MAP)
	{
		txt = GetTexture(base.normal.texture.name);
		obj->SetNormalMap(txt);
	}
	else
	{
		obj->GetSurfacer(CGenericRenderObject::BUMP_USER)->InitNormalGenerator(base.normal.texture);
	}
}

CTexture *CWorkUnitApp::GetTexture(CString tname)
{
	CTexture *txt = NULL;

	CString _msg = "Querying texture: " + tname;
	WriteMessage(_msg);

	if (tname.IsEmpty())
	{
		COUT << "Texture unavailable ( empty )" << endl;
		_msg = "Texture unavailable ( empty )";
		WriteMessage(_msg);
		return txt;
	}

	if (tname == "none")
	{
		COUT << "Texture unavailable ( none )" << endl;
		_msg = "Texture unavailable ( none )";
		WriteMessage(_msg);
		return txt;
	}

	if (0 == raytracer_data.textures.Lookup( tname, (void*&) txt))
	{
		COUT << "Creating texture: " << LPCTSTR(tname) << endl;
		txt = new CTexture();
		raytracer_data.textures.SetAt(tname,txt);

		nbTextureLoad++;
	}

	return txt;
}


bool CWorkUnitApp::BuildPlugins(MSGSTRUCT& msg,unsigned char raw_data[])
{
	//	is raytracer ready ?
	DWORD wait = WaitForSingleObject(HANDLE(raytracer_data.lock),RAYS_WAIT_TIMEOUT);
	if ((wait == WAIT_TIMEOUT)||(wait == WAIT_ABANDONED))
	{
		CString _msg = "Renderer busy !!!";
		WriteMessage(_msg);
		return false;
	}
	else
	{
		CString _msg;
		raytracer_data.lock->ResetEvent();

		int offset = 0;
		unsigned int nbPlugin = msg.msg_data[2];

		COUT << "Nb plugins: " << nbPlugin << endl;

		for (unsigned int i=0;i<nbPlugin;i++)
		{
			rays_plugin_t plugin;
			memcpy(&plugin,raw_data+offset,sizeof(rays_plugin_t));
			CString pname = plugin.name;
			COUT << "Loading plugin : " << LPCTSTR(pname) << endl;
			COUT << "Plugin nbParams: " << plugin.nbParams << endl;

			if (!LoadModule(pname))
			{
				_msg = "Unable to load module: " + pname;
				WriteMessage(_msg);
				nbPluginLoad++;

				//	Query module to server
				int len = pname.GetLength();
				unsigned char *buffer = new unsigned char[len+MSGSIZE];
				msg.msg_header = MSG_START;
				msg.msg_id = IMG_REQUEST;
				msg.msg_size = len;
				unsigned int jobID = msg.msg_data[0];
				msg.msg_data[0] = OBJ_PLUGIN;
				msg.msg_data[1] = 0;
				msg.msg_data[2] = 0;
				msg.msg_data[3] = client.GetPort();
				msg.msg_data[4] = ip;
				msg.msg_tail = MSG_DATA;

				COUT << "Querying plugin: " << LPCTSTR(pname) << endl;

				memcpy(buffer,&msg,MSGSIZE);
				memcpy(buffer+MSGSIZE,LPCTSTR(pname),len);
				client.Write(buffer,MSGSIZE+len);
				delete [] buffer;

				msg.msg_data[0] = jobID;
			}

			offset += sizeof(rays_plugin_t);
		}

		COUT << "Build Plugins done !" << endl;

		raytracer_data.lock->SetEvent();
		return true;
	}
}

bool CWorkUnitApp::LoadModule(CString name)
{
	PLUGIN *p = NULL;

	if (0 == raytracer_data.plugins.Lookup(LPCTSTR(name),(void*&)p))
	{
		p = new PLUGIN;

		*p = OpenPlugin(name);
		
		raytracer_data.plugins.SetAt(name,p);

		return (p->moduleInstance != NULL);
	}
	else
		return true;
}
