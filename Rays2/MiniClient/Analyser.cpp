#include "stdafx.h"
#include <iostream.h>	// cout
#include "MiniClient.h"
#include "..\Raptor\Include\Raptor.h"		// lecture .3DS, création géometries

///////////////////////////////////////////////////////////////////////////////
//
//	Analyser helpers
//
_inline void __fastcall copyCoord(coord_t &c1,rays_coord_t &c2)
{
	c2.x = c1.st_float1;
	c2.y = c1.st_float3;
	c2.z = c1.st_float5;
	c2.h = c1.st_float7;
}

_inline void __fastcall copyShading(shading_t &s1,rays_shading_t &s2)
{
	s2.ambient = s1.st_float1;
	s2.diffuse = s1.st_float3;
	s2.specular = s1.st_float5;
	s2.exponent = s1.st_integer7;
	s2.reflection = s1.st_float9;
	s2.refraction = s1.st_float11;
	s2.indice = s1.st_float13;
}

__inline void __fastcall copyTriangle(triangle_t &t1,rays_triangle_t &t2)
{
	t2.p1 = t1.st_integer1;
	t2.p2 = t1.st_integer3;
	t2.p3 = t1.st_integer5;
}

__inline void __fastcall CAnalyser::updateShading(rays_shading_t &r_shading)
{
	rays_shading_t *sh = NULL;
	shadings.Lookup(m_status.m_stsphere.st_objectbase0.st_string2,(void*&)sh);

	if (sh == NULL)
	{
		cout << "* Shading define not found, using default" << endl;
		r_shading.ambient = 0.5f;
		r_shading.diffuse = 0.5f;
		r_shading.exponent = 1.0f;
		r_shading.indice = 1.0f;
		r_shading.reflection = 0.0f;
		r_shading.refraction = 0.0f;
		r_shading.specular = 0.5f;
	}
	else
		r_shading = *sh;
}

void __fastcall CAnalyser::updateTexture(rays_texture_t &r_texture)
{
	rays_texture_t *txt = NULL;
	textures.Lookup(m_status.m_stsphere.st_objectbase0.st_string4,(void*&)txt);

	if (txt == NULL)
	{
		cout << "* Texture define not found, using \"none\"" << endl;
		strcpy(r_texture.tname,"\"none\"");
	}
	else
		r_texture = *txt;
}

///////////////////////////////////////////////////////////////////////////////
//
//	Analyser callbacks	( virtual defined by CYacc )
//
void CAnalyser::Oncoord(UINT)
{ 
	if (verbose)
	if (m_status.m_stcoord.m_step == 0)
	{
		cout << "Coord parsed: ";
		cout << m_status.m_stcoord.st_float1 << " ; ";
		cout << m_status.m_stcoord.st_float3 << " ; ";
		cout << m_status.m_stcoord.st_float5 << " ; ";
		cout << m_status.m_stcoord.st_float7 << endl;
	}
}


void CAnalyser::Onvertex(UINT) 
{ 
	if (verbose)
	if (m_status.m_stvertex.m_step == 0)
	{
		cout << "Vertex parsed" << endl;
	}
}

void CAnalyser::Ontriangle(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_sttriangle.m_step == 0)
	{
		cout << "Triangle parsed: ";
		cout << m_status.m_sttriangle.st_integer1 << " ; ";
		cout << m_status.m_sttriangle.st_integer3 << " ; ";
		cout << m_status.m_sttriangle.st_integer5 << endl;
	}
}

void CAnalyser::Onshading(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_stshading.m_step == 0)
	{
		cout << "Shading " << m_status.m_stdefine.st_string1 << " defined : ";

		shading_t sh;
		sh = m_status.m_stshading;

		cout << sh.st_float1 << " ; ";
		cout << sh.st_float3 << " ; ";
		cout << sh.st_float5 << " ; ";
		cout << sh.st_integer7 << " ; ";
		cout << sh.st_float9 << " ; ";
		cout << sh.st_float11 << " ; ";
		cout << sh.st_float13 << endl << endl;
	}
}

void CAnalyser::Onlight(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_stlight.m_step == 0)
	{
		cout << "Light " << m_status.m_stdefine.st_string1 << " defined : intensity ";

		cout << m_status.m_stlight.st_float5 << endl << endl;
	}
}


void CAnalyser::Ontexture(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_sttexture.m_step == 0)
	{
		cout << "Texture " << m_status.m_stdefine.st_string1 << " defined : file name ";

		cout << m_status.m_sttexture.st_string1 << endl << endl;
	}
}


void CAnalyser::Onelementname(UINT RULE_ID) 
{ 
	if(verbose)
	{
		cout << "Parsing element definition : " << endl;
		switch(RULE_ID)
		{
			case ATOM_shading_ID:
				cout << "Shading" << endl;
				break;
			case ATOM_light_ID:
				cout << "Light" << endl;
				break;
			case ATOM_texture_ID:
				cout << "Texture" << endl;
				break;
			default:
				cout << "Unknown element ..." << endl;
		}
	}
}

void CAnalyser::Onelement(UINT RULE_ID)
{
	switch(RULE_ID)
	{
		case RULE_light_ID:
		{
			rays_light_t	*l;	
			BOOL res = lights.Lookup( m_status.m_stdefine.st_string1, (void*&)l );

			if (res == 0)
			{
				l = new rays_light_t;
				copyCoord(m_status.m_stlight.st_coord1,l->position);
				copyCoord(m_status.m_stlight.st_coord3,l->color);
				l->intensity = m_status.m_stlight.st_float5;

				lights.SetAt(m_status.m_stdefine.st_string1, (void*&)l );
			}
			else
				cout << "* Object already defined !" << endl;

			break;
		}
		case RULE_shading_ID:
		{
			rays_shading_t	*s;	
			BOOL res = shadings.Lookup( m_status.m_stdefine.st_string1, (void*&)s );

			if (res == 0)
			{
				s = new rays_shading_t;
				copyShading(m_status.m_stshading,*s);
				
				shadings.SetAt(m_status.m_stdefine.st_string1, (void*&)s );
			}
			else
				cout << "* Object already defined !" << endl;
			break;
		}
		case RULE_texture_ID:
		{
			rays_texture_t	*t;	
			BOOL res = textures.Lookup( m_status.m_stdefine.st_string1, (void*&)t );

			if (res == 0)
			{
				t = new rays_texture_t;
				int len = m_status.m_sttexture.st_string1.GetLength();
				CString tname = m_status.m_sttexture.st_string1.Mid(1,len-2);
				sprintf(t->tname,"%s",LPCTSTR(tname));
				
				textures.SetAt(m_status.m_stdefine.st_string1, (void*&)t );
			}
			else
				cout << "* Object already defined !" << endl;
			break;
		}
		default:
			break;
	}
}


void CAnalyser::Onconfig(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_stconfig.m_step == 0)
	{
		cout << "Config parameter ";
		int val = m_status.m_stconfig.st_integer2;

		switch(m_status.m_stconfig.st_parameter0.st_parameterID)
		{
			case ATOM_width_ID:
				cout << "Width ";
				config.width = val;
				break;
			case ATOM_height_ID:
				cout << "Height ";
				config.height = val;
				break;
			case ATOM_variance_ID:
				cout << "Variance ";
				config.variance = val;
				break;
			case ATOM_deflection_ID:
				cout << "Reflection depth ";
				config.deflection = val;
				break;
			case ATOM_defraction_ID:
				cout << "Refraction depth ";
				config.defraction = val;
				break;
			case ATOM_crease_ID:
				cout << "Crease angle ";
				config.crease = val;
				break;
			default:
				cout << "Unknown ";
				break;
		}

		cout << "parsed : " << val << endl << endl;
	}
}




void CAnalyser::Onobjectbase(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_stobjectbase.m_step == 0)
	{
		cout << "Parsing object named ";
		cout << m_status.m_stobjectbase.st_string0;
		cout << " shaded with ";
		cout << m_status.m_stobjectbase.st_string2;
		cout << " textured with ";
		cout << m_status.m_stobjectbase.st_string4;
		cout << endl;
	}
}

void CAnalyser::Onobject(UINT RULE_ID) 
{ 
	switch(RULE_ID)
	{
		case RULE_sphere_ID:
		{
			rays_sphere_t	*s;	
			BOOL res = spheres.Lookup( m_status.m_stsphere.st_objectbase0.st_string0, (void*&)s );

			if (res == NULL)
			{
				s = new rays_sphere_t;
				copyCoord(m_status.m_stsphere.st_objectbase0.st_coord6,s->base.color);
				copyCoord(m_status.m_stsphere.st_coord1,s->center);

				updateShading(s->base.shading);

				updateTexture(s->base.texture);

				s->radius = m_status.m_stsphere.st_float3;
				
				spheres.SetAt(m_status.m_stsphere.st_objectbase0.st_string0, (void*&)s );
			}
			else
				cout << "* Object already defined !" << endl;
			break;
		}
		case RULE_composition_ID:
		{
			rays_composition_header_t *c;
			BOOL res = compositions.Lookup(m_status.m_stcomposition.st_objectbase0.st_string0,(void*&)c );

			if (res == NULL)
			{
				c = new rays_composition_header_t;
				copyCoord(m_status.m_stcomposition.st_objectbase0.st_coord6,c->base.color);

				updateShading(c->base.shading);

				updateTexture(c->base.texture);

				c->geometrySize = m_status.m_stcomposition.st_vertices3.GetSize();
				c->meshSize = m_status.m_stcomposition.st_triangles7.GetSize();

				c->geometry = new rays_vertex_t[c->geometrySize];
				c->mesh = new rays_triangle_t[c->meshSize];

				for (int i=0;i<c->geometrySize;i++)
				{
					copyCoord(m_status.m_stcomposition.st_vertices3[i].st_vertex0.st_coord1,c->geometry[i].position);
					copyCoord(m_status.m_stcomposition.st_vertices3[i].st_vertex0.st_coord3,c->geometry[i].texel);
				}

				for (i=0;i<c->meshSize;i++)
				{
					copyTriangle(m_status.m_stcomposition.st_triangles7[i].st_triangle0,c->mesh[i]);
				}

				compositions.SetAt(m_status.m_stcomposition.st_objectbase0.st_string0,(void*&)c );
			}
			else
				cout << "* Object already defined !" << endl;
			break;
		}
		case RULE_extern_ID:
		{
			CGeometry *geo = (CGeometry*)set->GetFirstChild();

			rays_composition_header_t *c;
			BOOL res = compositions.Lookup(m_status.m_stextern.st_objectbase0.st_string0,(void*&)c );

			if (res == NULL)
			{
				c = new rays_composition_header_t;
				copyCoord(m_status.m_stextern.st_objectbase0.st_coord6,c->base.color);

				updateShading(c->base.shading);

				updateTexture(c->base.texture);

				c->geometrySize = geo->NbVertex();
				c->meshSize = geo->NbFace();

				c->geometry = new rays_vertex_t[c->geometrySize];
				c->mesh = new rays_triangle_t[c->meshSize];

				C3DEngine::Vertex v;
				geo->GetCenter(v.coords);
				geo->Translate(-v.coords.x,-v.coords.y,-v.coords.z);
				geo->RotationX(-90.0f);
				geo->RotationY(45.0f);
				geo->RotationX(30.0f);
				geo->Translate(0.0f,0.0f,-300.0f);
				
				for (int i=0;i<c->geometrySize;i++)
				{
					geo->GetVertex(i,v);
					c->geometry[i].position.x = v.coords.x;
					c->geometry[i].position.y = v.coords.y;
					c->geometry[i].position.z = v.coords.z;
					c->geometry[i].position.h = 1.0f;
					c->geometry[i].texel.x = v.texcoords.u;
					c->geometry[i].texel.y = v.texcoords.v;
					c->geometry[i].texel.z = 1.0f;
					c->geometry[i].texel.h = 1.0f;
				}

				int p1,p2,p3;
				for (i=0;i<c->meshSize;i++)
				{
					geo->GetFace(i,p1,p2,p3);
					c->mesh[i].p1 = p1;
					c->mesh[i].p2 = p2;
					c->mesh[i].p3 = p3;
				}

				compositions.SetAt(m_status.m_stextern.st_objectbase0.st_string0,(void*&)c );
			}
			else
				cout << "* Object already defined !" << endl;
			break;
		}
		default:
			break;
	}
}

void CAnalyser::Onsphere(UINT RULE_ID) 
{ 
	if (verbose)
	if (m_status.m_stsphere.m_step == 0)
	{
		cout << "Sphere parsed : radius ";

		cout << m_status.m_stsphere.st_float3 << endl << endl;
	}
}


void CAnalyser::Oncomposition(UINT) 
{ 
	if (verbose)
	if (m_status.m_stcomposition.m_step == 0)
	{
		cout << "Geometric composition parsed :" << endl;

		cout << m_status.m_stcomposition.st_vertices3.GetSize() << " vertices" << endl;
		cout << m_status.m_stcomposition.st_triangles7.GetSize() << " triangles" << endl << endl;
	}
}

void CAnalyser::Onextern(UINT) 
{ 
	if (verbose)
	if (m_status.m_stextern.m_step == 0)
	{
		cout << "Importing extern composition ... :" << endl;

		int len = m_status.m_stextern.st_string2.GetLength();
		CString fname = m_status.m_stextern.st_string2.Mid(1,len-2);
		bool res = glLoadScene(fname,set);

		if (res)
			cout << "Extern composition imported successfully :" << m_status.m_stextern.st_string2 << endl;
		else
			cout << "Failed to import extern composition :" << m_status.m_stextern.st_string2 << endl;
	}
}


void CAnalyser::Onmain(UINT RULE_ID) 
{
	if (m_status.m_stmain.m_step == 0)
	{
		cout << endl << "Source file parsed successfully: " << endl;
		cout << "Program name: " << m_status.m_stmain.st_string0 << endl;
		cout << "Config : " << m_status.m_stmain.st_config3.GetSize() << endl;
		cout << "Defines : " << m_status.m_stmain.st_define6.GetSize() << endl;
		cout << "Objects : " << m_status.m_stmain.st_objects9.GetSize() << endl << endl;

		StartJob();		
		SendLights();
		SendSpheres();
		SendCompositions();
		RunJob();

	}		
}


///////////////////////////////////////////////////////////////////////////////
//
//	Analyser implementation
//
CAnalyser::CAnalyser(CParser *p,CClient<CClientSocket> *c):
CYacc(p),client(c),verbose(true)
{
}

void CAnalyser::InitAnalyse()
{
	config.deflection = 5;
	config.defraction = 5;
	config.height = 320;
	config.variance = 5;
	config.width = 240;
	config.crease = 45;

	m_status.Init();

	POSITION pos = lights.GetStartPosition();
	CString key;
	while (pos != NULL)
	{
		rays_light_t *l;
		lights.GetNextAssoc(pos,key,(void*&)l);
		lights.RemoveKey(LPCTSTR(key));
		delete l;
	}
	pos = shadings.GetStartPosition();
	while (pos != NULL)
	{
		rays_shading_t *s;
		shadings.GetNextAssoc(pos,key,(void*&)s);
		shadings.RemoveKey(LPCTSTR(key));
		delete s;
	}
	pos = textures.GetStartPosition();
	while (pos != NULL)
	{
		rays_texture_t *t;
		textures.GetNextAssoc(pos,key,(void*&)t);
		textures.RemoveKey(LPCTSTR(key));
		delete t;
	}
	pos = spheres.GetStartPosition();
	while (pos != NULL)
	{
		rays_sphere_t *s;
		spheres.GetNextAssoc(pos,key,(void*&)s);
		spheres.RemoveKey(LPCTSTR(key));
		delete s;
	}
	pos = compositions.GetStartPosition();
	while (pos != NULL)
	{
		rays_composition_header_t *s;
		compositions.GetNextAssoc(pos,key,(void*&)s);
		compositions.RemoveKey(LPCTSTR(key));
		delete [] s->geometry;
		delete [] s->mesh;
		delete s;
	}
}


void CAnalyser::StartJob()
{
	msg.msg_header = MSG_START;
	msg.msg_id = JOB_START;
	msg.msg_size = 0;
	msg.msg_data[0] = 12345;
	msg.msg_data[1] = config.width;
	msg.msg_data[2] = config.height;
	msg.msg_data[3] = client->GetPort();
	msg.msg_data[4] = client->GetAddr();
	msg.msg_tail = MSG_END;

	client->Write(&msg,MSGSIZE);

	MSGSTRUCT rmsg;
	client->Read(&rmsg,MSGSIZE);
	jobID = rmsg.msg_data[0];
	cout << endl;
	cout << "Sending configuration to server..." << endl;
	int totalsize = sizeof(config)+MSGSIZE;
	unsigned char *buffer = new unsigned char[totalsize];
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_CAMERA;
	msg.msg_tail = MSG_DATA;
	msg.msg_size = sizeof(config);
	memcpy(buffer,&msg,MSGSIZE);
	memcpy(buffer+MSGSIZE,&config,sizeof(config));

	client->Write(buffer,totalsize);

	client->Read(&rmsg,MSGSIZE);
	if (rmsg.msg_id == ACK_OBJ)
		cout << "Configuration object successfully sent to server" << endl;
	else
		cout << "Failed to send configuration object to server" << endl;
	delete [] buffer;
	
	cout << endl;
}

void CAnalyser::RunJob()
{
	cout << "Sending job start to server..." << endl;

	msg.msg_header = MSG_START;
	msg.msg_id = JOB_RUN;
	msg.msg_size = 0;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = 0;
	msg.msg_data[2] = 0;
	msg.msg_data[3] = 0;
	msg.msg_data[4] = 0;
	msg.msg_tail = MSG_END;

	client->Write(&msg,MSGSIZE);

	client->Read(&msg,MSGSIZE);
	if (msg.msg_id == ACK_JOB)
		cout << "Job " << jobID << " is beeing rendered..." << endl;
	else
		cout << "Failed to run job" << endl;
}

void CAnalyser::SendLights()
{
	unsigned int nblights = lights.GetCount();
	cout << "Sending " << nblights << " Lights to server..." << endl;

	int totalsize = nblights * sizeof(rays_light_t)+MSGSIZE;
	unsigned char *buffer = new unsigned char[totalsize];

	msg.msg_header = MSG_START;
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_LIGHT;
	msg.msg_data[2] = nblights;
	msg.msg_tail = MSG_DATA;
	msg.msg_size = nblights * sizeof(rays_light_t);
	memcpy(buffer,&msg,MSGSIZE);

	POSITION pos = lights.GetStartPosition();
	CString key;
	rays_light_t *l;
	unsigned int bufferpos = MSGSIZE;
	for (unsigned int i=0;i<nblights;i++)
	{
		lights.GetNextAssoc(pos,key,(void*&)l);
		memcpy(buffer+bufferpos,l,sizeof(rays_light_t));
		bufferpos+=sizeof(rays_light_t);
	}

	client->Write(buffer,bufferpos);

	client->Read(&msg,MSGSIZE);
	if (msg.msg_id == ACK_OBJ)
		cout << "Lights successfully sent to server" << endl;
	else
		cout << "Failed to send Lights to server" << endl;

	delete [] buffer;
	cout << endl;
}

void CAnalyser::SendSpheres()
{
	unsigned int nbspheres = spheres.GetCount();
	cout << "Sending " << nbspheres << " Spheres to server..." << endl;

	int totalsize = nbspheres*sizeof(rays_sphere_t)+MSGSIZE;
	unsigned char *buffer = new unsigned char[totalsize];

	msg.msg_header = MSG_START;
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_SPHERE;
	msg.msg_data[2] = nbspheres;
	msg.msg_tail = MSG_DATA;
	msg.msg_size = nbspheres * sizeof(rays_sphere_t);
	memcpy(buffer,&msg,MSGSIZE);

	POSITION pos = spheres.GetStartPosition();
	CString key;
	rays_sphere_t *s;
	unsigned int bufferpos = MSGSIZE;
	for (unsigned int i=0;i<nbspheres;i++)
	{
		spheres.GetNextAssoc(pos,key,(void*&)s);
		memcpy(buffer+bufferpos,s,sizeof(rays_sphere_t));
		bufferpos+=sizeof(rays_sphere_t);
	}

	client->Write(buffer,bufferpos);

	client->Read(&msg,MSGSIZE);
	if (msg.msg_id == ACK_OBJ)
		cout << "Spheres successfully sent to server" << endl;
	else
		cout << "Failed to send Spheres to server" << endl;

	delete [] buffer;
	cout << endl;
}

void CAnalyser::SendCompositions()
{
	unsigned int nbcompositions = compositions.GetCount();
	cout << "Sending " << nbcompositions << " Compositions to server..." << endl;

	CString key;
	rays_composition_header_t *s = NULL;
	POSITION pos = compositions.GetStartPosition();

	int compositionsize = nbcompositions * sizeof(rays_composition_header_t);
	for (int j=0;j<nbcompositions;j++)
	{
		compositions.GetNextAssoc(pos,key,(void*&)s);
		compositionsize += s->geometrySize * sizeof(rays_vertex_t);
		compositionsize += s->meshSize * sizeof(rays_triangle_t);
	}

	int totalsize = compositionsize + MSGSIZE;
	unsigned char *buffer = new unsigned char[totalsize];

	msg.msg_header = MSG_START;
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_GEOMETRY;
	msg.msg_data[2] = nbcompositions;
	msg.msg_tail = MSG_DATA;
	msg.msg_size = compositionsize;
	memcpy(buffer,&msg,MSGSIZE);

	pos = compositions.GetStartPosition();
	unsigned int bufferpos = MSGSIZE;
	unsigned int sz = 0;

	for (unsigned int i=0;i<nbcompositions;i++)
	{
		compositions.GetNextAssoc(pos,key,(void*&)s);
		memcpy(buffer+bufferpos,s,sizeof(rays_composition_header_t));
		bufferpos+=sizeof(rays_composition_header_t);

		sz = s->geometrySize * sizeof(rays_vertex_t);
		memcpy(buffer+bufferpos,s->geometry,sz);
		bufferpos += sz;

		sz = s->meshSize * sizeof(rays_triangle_t);
		memcpy(buffer+bufferpos,s->mesh,sz);
		bufferpos += sz;
	}

	client->Write(buffer,bufferpos);

	client->Read(&msg,MSGSIZE);
	if (msg.msg_id == ACK_OBJ)
		cout << "Compositions successfully sent to server" << endl;
	else
		cout << "Failed to send Compositions to server" << endl;

	delete [] buffer;
	cout << endl;
}