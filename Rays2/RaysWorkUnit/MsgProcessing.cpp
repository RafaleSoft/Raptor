#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#include "RaysWorkUnit.h"
#include "GenericLight.h"
#include "Quadric.h"
#include "Triangle.h"
#include "Mesh.h"
#include "SSE_CMesh.h"
#include "Texture.h"
#include "Environment.h"
#include "Surfacer.h"
#include "..\PlugIn.h"

extern DWORD			__stdcall Raytrace(LPVOID pParam);
extern unsigned int		ID;

static unsigned int		nbTextureLoad = 0;
static unsigned int		nbPluginLoad = 0;


void CRaysWorkUnit::ProcessMsg(MSGSTRUCT& msg, unsigned char raw_data[])
{
	bool acknowledge = true;
	bool sendacknowledge = true;

	std::cout << "Receiving data from server" << std::endl;
	std::cout << "# Processing server Message:" << msg.msg_id << endl;
	switch(msg.msg_id)
	{
		case OBJ_DATA:
		{
			switch (msg.msg_data[1])
			{
				case OBJ_PLUGIN:
				{
					std::cout << "Processing plugin objects" << std::endl;
					acknowledge = BuildPlugins(msg, raw_data);
					break;
				}
				case OBJ_CAMERA:
				{
					std::cout << "Processing camera object" << std::endl;
					acknowledge = BuildCamera(msg, raw_data);
					break;
				}
				case OBJ_LIGHT:
				{
					std::cout << "Processing light object" << std::endl;
					acknowledge = BuildLights(msg, raw_data);
					break;
				}
				case OBJ_SPHERE:
				{
					std::cout << "Processing sphere object" << std::endl;
					acknowledge = BuildSpheres(msg, raw_data);
					break;
				}
				case OBJ_GEOMETRY:
				{
					std::cout << "Processing geometry object" << std::endl;
					acknowledge = BuildGeometries(msg, raw_data);
					break;
				}
				case OBJ_FRAME:
				{
					std::cout << "Processing scene frames" << std::endl;
					acknowledge = BuildFrames(msg, raw_data);
					break;
				}
				default:
				{
					std::cout << "Unsupported object id" << msg.msg_data[1] << endl;
					acknowledge = false;
					std::cout << "Unsupported object id";
					std::cout << " msg.id: " << msg.msg_id;
					std::cout << " - data[0]: " << msg.msg_data[0];
					std::cout << " - data[1]: " << msg.msg_data[1] << std::endl;
					break;
				}
			}
			std::cout << "Data processed" << std::endl;
			msg.msg_id = ACK_OBJ;
			break;
		}
		case OBJ_TEXTURE:
		{
			std::cout << "Processing texture object" << std::endl;
			acknowledge = BuildTextures(msg, raw_data);
			sendacknowledge = false;
			break;
		}
		case JOB_RUN:
		{
			acknowledge = RunRaytrace(msg, raw_data);
			msg.msg_data[1] = JOB_RUN;
			msg.msg_id = ACK_JOB;
			break;
		}
		case ACK_NONE:
		{
			if (msg.msg_data[0] == OBJ_TEXTURE)
			{
				std::cout << "Texture query failed" << std::endl;
				std::cout << "Server unable to execute request (wrong/none texture ?)" << std::endl;
				if (nbTextureLoad > 0)
					nbTextureLoad--;
			}
			else if (msg.msg_data[0] == OBJ_PLUGIN)
			{
				std::cout << "Plugin query failed" << std::endl;
				std::cout << "Server unable to execute request (wrong/none plugin ?)" << std::endl;
				if (nbPluginLoad > 0)
					nbPluginLoad--;
			}
			sendacknowledge = false;
			break;
		}
		default:
		{
			acknowledge = false;
			std::cout << "Unsupported message id" << std::endl;
			std::cout << "# Unsupported message ID: " << msg.msg_id << std::endl;
			break;
		}
	}

	if (sendacknowledge)
	{
		std::cout << "Sending aknowledgment..." << std::endl;

		if (!acknowledge)
			msg.msg_id = ACK_NONE;

		//msg.msg_size = 0;
		// msg.msg_data[0] should contain jobID
		msg.msg_data[2] = 0;
		msg.msg_data[3] = getPort();
		msg.msg_data[4] = getAddr();
		//msg.msg_tail = MSG_END;

		std::cout << "Acknowledge datas: " << msg.msg_id << std::endl;
		write(&msg,MSGSIZE);

		std::cout << "Acknowledge sent !" << std::endl;
	}

	if (raw_data!=NULL)
		delete [] raw_data;

	std::cout << "Data cleaned !" << std::endl;

	if (ACK_JOB == msg.msg_id)
		GetRaytraceData();
}


bool CRaysWorkUnit::GetRaytraceData()
{
	std::cout << "Querying Raytrace additional datas... " << std::endl;

	if (0 == raytracer_data->getNbTextures())
		std::cout << "No data needed, rendering can start" << endl;
	else
	{
		std::cout << "Data needed, querying to server" << endl;

		std::string tname;
		map<std::string, CTexture*>::const_iterator tpos = raytracer_data->getFirstTexture();
		CTexture *texture = raytracer_data->getNextTexture(tpos, tname);
		while (NULL != texture)
		{
			//	Query texture to server
			size_t len = tname.length();
			unsigned char *buffer = new unsigned char[len + MSGSIZE];
			MSGSTRUCT msg;
			//msg.msg_header = MSG_START;
			msg.msg_id = IMG_REQUEST;
			//msg.msg_size = len;
			msg.msg_data[0] = OBJ_TEXTURE;
			msg.msg_data[1] = 0;
			msg.msg_data[2] = 0;
			msg.msg_data[3] = getPort();
			msg.msg_data[4] = getAddr();
			//msg.msg_tail = MSG_DATA;

			std::cout << "Querying texture: " << tname << endl;

			memcpy(buffer, &msg, MSGSIZE);
			memcpy(buffer + MSGSIZE, tname.c_str(), len);
			write(buffer, MSGSIZE + len);

			delete[] buffer;

			std::cout << "Query sent for texture " << tname << std::endl;
			texture = raytracer_data->getNextTexture(tpos, tname);
		}
	}

	//	Verifying plugins...
	std::string pname;
	map<std::string, CPlugin*>::const_iterator ppos = raytracer_data->getFirstPlugin();
	CPlugin *plugin = raytracer_data->getNextPlugin(ppos, pname);
	while (NULL != plugin)
	{
		if (plugin->moduleInstance == NULL)
		{
			if (!plugin->OpenPlugin(pname))
				std::cout << "Unable to load plugin " << pname << endl;
			else
			{
				nbPluginLoad--;
				std::cout << "Plugin " << pname << " loaded. " << endl;
			}
		}
	}

	return true;
}


bool CRaysWorkUnit::BuildTextures(MSGSTRUCT& msg, unsigned char raw_data[])
{
	if (msg.msg_id != ACK_NONE)
	{
		std::cout << "Building Texture..." << endl;

		int len = msg.msg_data[2];
		char *tname = new char[len + 1];

		memcpy(tname, &raw_data[0], len);
		tname[len] = 0;

		std::cout << "Downloading texture: " << tname << std::endl;
		std::cout << "Texture name: " << tname << std::endl;
		//std::cout << "Texture size: " << msg.msg_size - len << std::endl;

		CTexture *txt = raytracer_data->getTexture(tname);
		if (NULL != txt)
			txt->SetTexture(msg.msg_data[0], msg.msg_data[1], &raw_data[len]);
		else
			std::cout << "Internal error : texture object not created !" << std::endl;

		std::cout << "Texture is built !" << std::endl;
	}

	if (nbTextureLoad > 0)
		nbTextureLoad--;

	std::cout << "Still need " << nbTextureLoad << " textures" << std::endl;
	std::cout << "Still need " << nbPluginLoad << " plugins" << std::endl;

	return ((nbPluginLoad == 0) && (nbTextureLoad == 0));
}

//////////////////////////////////////////////////////////////////////
//
//	Job start
//
//////////////////////////////////////////////////////////////////////
bool CRaysWorkUnit::RunRaytrace(MSGSTRUCT& msg, unsigned char raw_data[])
{
	std::cout << "Running raytracer ..." << endl;

	//	Rendering
	raytracer_data->setStart(msg.msg_data[1]);
	raytracer_data->setEnd(msg.msg_data[2]);
	std::cout << "Raytrace initialisation... " << endl;
	HANDLE raytracer = CreateThread(NULL,
									0,
									Raytrace,
									(void*)&raytracer_data,
									CREATE_SUSPENDED,
									NULL );
	std::cout << "Rendering from scanline " << raytracer_data->getStart();
	std::cout << " to " << raytracer_data->getEnd();
	std::cout << " ( " << raytracer_data->getCamera().width;
	std::cout << " x " << raytracer_data->getCamera().height;
	std::cout << ")" << std::endl;

	ResumeThread(raytracer);

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//	Objects rebuilding
//
//////////////////////////////////////////////////////////////////////
bool CRaysWorkUnit::BuildCamera(MSGSTRUCT& msg, unsigned char raw_data[])
{
	rays_config_t config;
	//memcpy(&config,raw_data,msg.msg_size);

	raytracer_data->updateCamera(config);

	CTexture *txt = NULL;
	std::string tname = ""; // config.envtexname;
	std::cout << "Env texture: " << tname << std::endl;
	if (!tname.empty())
		txt = raytracer_data->getTexture(tname);
	CEnvironment::GetInstance()->SetTexture(txt);

	return true;
}


bool CRaysWorkUnit::BuildLights(MSGSTRUCT& msg, unsigned char raw_data[])
{
	rays_light_t tlight;
	unsigned int bufferpos=0;
	unsigned int nblights = msg.msg_data[2];
	CGenericLight *l;

	for (unsigned int i=0;i<nblights;i++)
	{
		memcpy(&tlight,raw_data+bufferpos,sizeof(rays_light_t));
		bufferpos+=sizeof(rays_light_t);
		l = new CGenericLight(tlight);
		raytracer_data->addLight(l);
		std::cout << "light: " << tlight.position.x;
		std::cout << "-" << tlight.position.y;
		std::cout << "-" << tlight.position.z;
		std::cout << "-" << tlight.intensity << std::endl;
		std::cout << "Light added" << std::endl;
	}

	return true;
}


bool CRaysWorkUnit::BuildSpheres(MSGSTRUCT& msg, unsigned char raw_data[])
{
	rays_sphere_t tsphere;
	unsigned int bufferpos=0;
	unsigned int nbspheres = msg.msg_data[2];
	CQuadric *q;

	for (unsigned int i=0;i<nbspheres;i++)
	{
		memcpy(&tsphere,raw_data+bufferpos,sizeof(rays_sphere_t));
		bufferpos+=sizeof(rays_sphere_t);

		q = new CQuadric(tsphere);
		SetTextures(q,tsphere.base);

		raytracer_data->addObject(q);
		std::cout << "sphere: " << tsphere.center.x << "-" << tsphere.center.y << "-";
		std::cout << tsphere.center.z << "-" << tsphere.radius << std::endl;
		std::cout << "Surface added" << std::endl;
	}

	return true;
}

bool CRaysWorkUnit::BuildFrames(MSGSTRUCT& msg, unsigned char raw_data[])
{
	unsigned int bufferpos=0;
	unsigned int nbframes = msg.msg_data[2];

	for (unsigned int i=0;i<nbframes;i++)
	{
		rays_frame_t *pFrame = new rays_frame_t;

		memcpy(pFrame,raw_data+bufferpos,sizeof(rays_frame_t));
		bufferpos+=sizeof(rays_frame_t);

		raytracer_data->addFrame(pFrame);

		pFrame->calls = new rays_plugin_t[pFrame->nbCalls];
		for (unsigned int j=0;j<pFrame->nbCalls;j++)
		{
			memcpy(&(pFrame->calls[j]),raw_data+bufferpos,sizeof(rays_plugin_t));
			bufferpos += sizeof(rays_plugin_t);
		}

		pFrame->transforms = new rays_transform_t[pFrame->nbTransforms];
		for (unsigned int j=0;j<pFrame->nbTransforms;j++)
		{
			memcpy(&(pFrame->transforms[j]),raw_data+bufferpos,sizeof(rays_transform_t));
			bufferpos += sizeof(rays_transform_t);
		}

		std::cout << "Frame: " << i << " added" << std::endl;
	}

	return true;
}


bool CRaysWorkUnit::BuildGeometries(MSGSTRUCT& msg, unsigned char raw_data[])
{
	rays_composition_header_t tgeometry;
	unsigned int bufferpos=0;
	unsigned int size = 0;
	unsigned int nbcomposition = msg.msg_data[2];

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

		std::cout << "Geometry size: " << tgeometry.geometrySize << endl;
		std::cout << "Mesh size: " << tgeometry.meshSize << endl;
		std::cout << "Creating mesh geometry..." << std::endl;

		const CPU_INFO& cpu = getCPUINFO();
		if (cpu.hasFeature(CPUINFO::SSE))
		{
			mm = new SSE_CMesh(tgeometry);
			SetTextures(mm,tgeometry.base);
			for (int j = 0; j<tgeometry.meshSize; j++)
				mm->AddTriangle(tgeometry.mesh[j], tgeometry.geometry);
			raytracer_data->addObject(mm);
		}
		else
		{
			m = new CMesh(tgeometry);
			SetTextures(m,tgeometry.base);
			for (int j = 0; j<tgeometry.meshSize; j++)
				m->AddTriangle(tgeometry.mesh[j], tgeometry.geometry);
			raytracer_data->addObject(m);
		}

		std::cout << "Mesh geometry added" << endl;

		delete [] tgeometry.geometry;
		delete [] tgeometry.mesh;
	}

	std::cout << "Build Geometries done !" << endl;

	return true;
}

void CRaysWorkUnit::SetTextures(CGenericRenderObject* obj,rays_objectbase_t& base)
{
	std::cout << "Diffuse Texture type: " << base.texture.type << std::endl;

	CTexture *txt = GetTexture(base.texture.texture.name);
	obj->SetTexture(txt);

	std::cout << "Normal Texture type: " << base.normal.type << std::endl;

	if (base.normal.type == MAP)
	{
		txt = GetTexture(base.normal.texture.name);
		obj->SetNormalMap(txt);
	}
	else
		obj->GetSurfacer(CGenericRenderObject::BUMP_USER)->InitNormalGenerator(base.normal.texture);
}

CTexture *CRaysWorkUnit::GetTexture(const std::string& tname)
{
	CTexture *txt = NULL;

	std::cout << "Querying texture: " << tname << std::endl;

	if (tname.empty())
	{
		std::cout << "Texture unavailable ( empty )" << std::endl;
		return txt;
	}

	if (tname == "none")
	{
		std::cout << "Texture unavailable ( none )" << std::endl;
		return txt;
	}

	txt = raytracer_data->getTexture(tname);
	if (NULL == txt)
	{
		std::cout << "Creating texture: " << tname << endl;
		txt = new CTexture();
		raytracer_data->addTexture(txt, tname);

		nbTextureLoad++;
	}

	return txt;
}


bool CRaysWorkUnit::BuildPlugins(MSGSTRUCT& msg, unsigned char raw_data[])
{
	unsigned int offset = 0;
	unsigned int nbPlugin = msg.msg_data[2];

	std::cout << "Nb plugins: " << nbPlugin << std::endl;

	for (unsigned int i=0;i<nbPlugin;i++)
	{
		rays_plugin_t plugin;
		memcpy(&plugin,raw_data+offset,sizeof(rays_plugin_t));
		std::cout << "Loading plugin : " << plugin.name << std::endl;
		std::cout << "Plugin nbParams: " << plugin.nbParams << std::endl;

		CPlugin *p = raytracer_data->getPlugin(plugin.name);
		if (0 == p)
		{
			p = new CPlugin;
			p->OpenPlugin(plugin.name);

			raytracer_data->addPlugin(p,plugin.name);

			if (NULL == p->moduleInstance)
			{
				std::cout << "Unable to load module: " << plugin.name << std::endl;
				nbPluginLoad++;

				//	Query module to server
				int len = strlen(plugin.name);
				unsigned char *buffer = new unsigned char[len + MSGSIZE];
				//msg.msg_header = MSG_START;
				msg.msg_id = IMG_REQUEST;
				//msg.msg_size = len;
				unsigned int jobID = msg.msg_data[0];
				msg.msg_data[0] = OBJ_PLUGIN;
				msg.msg_data[1] = 0;
				msg.msg_data[2] = 0;
				msg.msg_data[3] = getPort();
				msg.msg_data[4] = getAddr();
				//msg.msg_tail = MSG_DATA;

				std::cout << "Querying plugin: " << plugin.name << endl;

				memcpy(buffer, &msg, MSGSIZE);
				memcpy(buffer + MSGSIZE, plugin.name, len);
				write(buffer, MSGSIZE + len);
				delete[] buffer;

				msg.msg_data[0] = jobID;
			}
		}
		offset += sizeof(rays_plugin_t);
	}

	std::cout << "Build Plugins done !" << endl;

	return true;
}
