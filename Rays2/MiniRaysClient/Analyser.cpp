#include "stdafx.h"
#include <iostream.h>	// cout
#include "MiniRaysClient.h"
#include "MiniRaysClientDlg.h"
#include "..\Raptor\Include\Raptor.h"		// lecture .3DS
#include "..\Raptor\Include\3DSet.h"		// création géometries
#include "..\Raptor\Include\Geometry.h"		// création géometries

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
	shadings.Lookup(status->m_stobjectbase.st_string2,(void*&)sh);

	if (sh == NULL)
	{
		output->Output("* Shading define not found, using default");
		r_shading.ambient = 0.5f;
		r_shading.diffuse = 0.5f;
		r_shading.exponent = 1;
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
	textures.Lookup(status->m_stobjectbase.st_string4,(void*&)txt);

	if (txt == NULL)
	{
		output->Output("* Texture define not found, using \"none\"");
		strcpy(r_texture.texture.name,"none");
		r_texture.type = MAP;
	}
	else
		r_texture = *txt;
}

void __fastcall CAnalyser::updateNormal(rays_texture_t &r_texture)
{
	normal_specifier_t ns = status->m_stobjectbase.st_normal_specifier6;

	if (ns.st_normal_specifierID == RULE_name_ID)
	{
		rays_texture_t *txt = NULL;
		textures.Lookup(((lpname_t)(ns.p_stnormal_specifier))->st_string0,(void*&)txt);
		r_texture = *txt;
	}
	else if (ns.st_normal_specifierID == RULE_plugin_call_ID)
	{
		rays_plugin_t *plg = NULL;
		CString name = ((lpplugin_call_t)(ns.p_stnormal_specifier))->st_string1;
		name = name.Mid(1,name.GetLength() - 2);
		plugins.Lookup(name,(void*&)plg);
		r_texture.type = PROCEDURE;
		r_texture.texture = *plg;
	}
	else
	{
		output->Output("* Normal Map Texture define not found, using \"none\"");
		strcpy(r_texture.texture.name,"none");
		r_texture.type = MAP;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	Analyser callbacks	( virtual defined by CYacc )
//
void CAnalyser::Oncoord(UINT)
{ 
	if (verbose)
	if (status->m_stcoord.m_step == 0)
	{
		CString coord;

		coord.Format("Coord parsed: %f ; %f ; %f ; %f",
						status->m_stcoord.st_float1,
						status->m_stcoord.st_float3,
						status->m_stcoord.st_float5,
						status->m_stcoord.st_float7);
		output->Output(coord);
	}
}


void CAnalyser::Onvertex(UINT) 
{ 
	if (verbose)
	if (status->m_stvertex.m_step == 0)
	{
		output->Output("Vertex parsed");
	}
}

void CAnalyser::Ontriangle(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_sttriangle.m_step == 0)
	{
		CString triangle;
		triangle.Format("Triangle parsed: %d ; %d ; %d",
						status->m_sttriangle.st_integer1,
						status->m_sttriangle.st_integer3,
						status->m_sttriangle.st_integer5);
		output->Output(triangle);
	}
}

void CAnalyser::Onshading(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_stshading.m_step == 0)
	{
		CString shading;

		shading = "Shading " + status->m_stdefine.st_string1 + " defined : ";
		output->Output(shading);

		shading_t sh;
		sh = status->m_stshading;

		shading.Format("amb:%f diff:%f spec:%f exp:%d refl:%f refr:%f ind:%f",
						sh.st_float1,sh.st_float3,sh.st_float5,
						sh.st_integer7,sh.st_float9,sh.st_float11,
						sh.st_float13);
		output->Output(shading);
	}
}

void CAnalyser::Onlight(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_stlight.m_step == 0)
	{
		CString light;

		light = "Light " + status->m_stdefine.st_string1 + " defined :";
		output->Output(light);

		light.Format("intensity: %f",status->m_stlight.st_float5);
		output->Output(light);
	}
}


void CAnalyser::Ontexture(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_sttexture.m_step == 0)
	{
		CString texture;
		texture = "Texture " + status->m_stdefine.st_string1 + 
					" defined : file name " + status->m_sttexture.st_string1;

		output->Output(texture);
	}
}


void CAnalyser::Onelementname(UINT RULE_ID) 
{ 
	if(verbose)
	{
		CString ename = "Parsing element definition : ";

		switch(RULE_ID)
		{
			case ATOM_shading_ID:
				ename += "Shading";
				break;
			case ATOM_light_ID:
				ename += "Light";
				break;
			case ATOM_texture_ID:
				ename += "Texture";
				break;
			case ATOM_environment_ID:
				ename += "Environment";
				break;
			default:
				ename += "ERROR : Unknown element ...";
		}

		output->Output(ename);
	}
}

void CAnalyser::Onelement(UINT RULE_ID)
{
	switch(RULE_ID)
	{
		case RULE_light_ID:
		{
			rays_light_t	*l;	
			BOOL res = lights.Lookup( status->m_stdefine.st_string1, (void*&)l );

			if (res == 0)
			{
				l = new rays_light_t;
				copyCoord(status->m_stlight.st_coord1,l->position);
				copyCoord(status->m_stlight.st_coord3,l->color);
				l->intensity = status->m_stlight.st_float5;
				copyCoord(status->m_stlight.st_coord7,l->direction);
				l->cutoff = status->m_stlight.st_float9;
				l->model = OMNI;

				for (int i=0;i<status->m_stlight.st_light_models11.GetSize();i++)
				{
					switch(status->m_stlight.st_light_models11[i].st_light_model0.st_light_modelID)
					{
						case ATOM_none_ID:
							l->model |= OMNI;
							break;
						case ATOM_spot_ID:
							l->model |= SPOT;
							break;
						case ATOM_spot_cos_ID:
							l->model |= SPOT_COS;
							break;
						case ATOM_spot_exp_ID:
							l->model |= SPOT_EXP;
							break;
						case ATOM_photon_map_ID:
							l->model |= PHOTON_MAP;
							break;
						default:
							l->model |= OMNI;
							break;
					}
				}

				lights.SetAt(status->m_stdefine.st_string1, (void*&)l );
			}
			else
				output->Output("* Object already defined !");

			break;
		}
		case RULE_shading_ID:
		{
			rays_shading_t	*s;	
			BOOL res = shadings.Lookup( status->m_stdefine.st_string1, (void*&)s );

			if (res == 0)
			{
				s = new rays_shading_t;
				copyShading(status->m_stshading,*s);
				
				shadings.SetAt(status->m_stdefine.st_string1, (void*&)s );
			}
			else
				output->Output("* Object already defined !");
			break;
		}
		case RULE_texture_ID:
		{
			rays_texture_t	*t;	
			BOOL res = textures.Lookup( status->m_stdefine.st_string1, (void*&)t );

			if (res == 0)
			{
				t = new rays_texture_t;
				int len = status->m_sttexture.st_string1.GetLength();
				CString tname = status->m_sttexture.st_string1.Mid(1,len-2);
				sprintf(t->texture.name,"%s",LPCTSTR(tname));
				t->type = MAP;
				
				textures.SetAt(status->m_stdefine.st_string1, (void*&)t );

				if (status->m_stdefine.st_elementname0.st_elementnameID == ATOM_environment_ID)
				{
					strcpy(config.envtexname,t->texture.name);
				}
			}
			else
				output->Output("* Object already defined !");
			break;
		}
		default:
			break;
	}
}


void CAnalyser::Onconfig(UINT RULE_ID) 
{ 
	if (status->m_stconfig.m_step == 0)
	{
		CString parameter = "Config parameter ";

		int ival = status->m_stconfig.st_configparam2.configparam_value.integer_0;
		float fval = status->m_stconfig.st_configparam2.configparam_value.float_1;
		
		switch(status->m_stconfig.st_parameter0.st_parameterID)
		{
			case ATOM_width_ID:
				parameter += "Width ";
				config.width = ival;
				break;
			case ATOM_height_ID:
				parameter += "Height ";
				config.height = ival;
				break;
			case ATOM_variance_ID:
				parameter += "Variance ";
				config.variance = ival;
				break;
			case ATOM_deflection_ID:
				parameter += "Reflection depth ";
				config.deflection = ival;
				break;
			case ATOM_defraction_ID:
				parameter += "Refraction depth ";
				config.defraction = ival;
				break;
			case ATOM_crease_ID:
				parameter += "Crease angle ";
				config.crease = ival;
				break;
			case ATOM_focale_ID:
				parameter += "Camera focale ";
				config.focale = fval;
				break;
			case ATOM_object_plane_ID:
				parameter += "Camera object plane ";
				config.focale = fval;
				break;
			case ATOM_photon_map_ID:
				parameter += "Lights photon map size ";
				config.photon_map = ival;
				break;
			default:
				parameter += "Unknown ";
				break;
		}

		if (verbose)
		{
			CString value;
			value.Format("parsed : %d/%f",ival,fval);

			parameter += value;
			output->Output(parameter);
		}
	}
}




void CAnalyser::Onobjectbase(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_stobjectbase.m_step == 0)
	{
		CString objectbase;

		objectbase = "Parsing object named ";
		objectbase += status->m_stobjectbase.st_string0;
		objectbase += " shaded with ";
		objectbase += status->m_stobjectbase.st_string2;
		objectbase += " textured with ";
		objectbase += status->m_stobjectbase.st_string4;
		if (status->m_stobjectbase.st_normal_specifier6.st_normal_specifierID == RULE_name_ID)
			objectbase += " with bump mapping";
		else
			objectbase += " with user normals";
		
		output->Output(objectbase);
	}
}

void CAnalyser::Onobject(UINT RULE_ID) 
{ 
	switch(RULE_ID)
	{
		case RULE_sphere_ID:
		{
			rays_sphere_t	*s;	
			BOOL res = spheres.Lookup( status->m_stsphere.st_objectbase0.st_string0, (void*&)s );

			if (res == NULL)
			{
				s = new rays_sphere_t;
				strcpy(s->base.name,LPCTSTR(status->m_stsphere.st_objectbase0.st_string0));
				copyCoord(status->m_stsphere.st_objectbase0.st_coord8,s->base.color);
				copyCoord(status->m_stsphere.st_coord1,s->center);

				updateShading(s->base.shading);
				updateTexture(s->base.texture);
				updateNormal(s->base.normal);

				s->radius = status->m_stsphere.st_float3;
				
				spheres.SetAt(status->m_stsphere.st_objectbase0.st_string0, (void*&)s );
			}
			else
				output->Output("* Object already defined !");
			break;
		}
		case RULE_composition_ID:
		{
			rays_composition_header_t *c;
			BOOL res = compositions.Lookup(status->m_stcomposition.st_objectbase0.st_string0,(void*&)c );

			if (res == NULL)
			{
				c = new rays_composition_header_t;
				strcpy(c->base.name,LPCTSTR(status->m_stsphere.st_objectbase0.st_string0));
				copyCoord(status->m_stcomposition.st_objectbase0.st_coord8,c->base.color);

				updateShading(c->base.shading);
				updateTexture(c->base.texture);
				updateNormal(c->base.normal);

				c->geometrySize = status->m_stcomposition.st_vertices3.GetSize();
				c->meshSize = status->m_stcomposition.st_triangles7.GetSize();

				c->geometry = new rays_vertex_t[c->geometrySize];
				c->mesh = new rays_triangle_t[c->meshSize];

				for (int i=0;i<c->geometrySize;i++)
				{
					copyCoord(status->m_stcomposition.st_vertices3[i].st_vertex0.st_coord1,c->geometry[i].position);
					copyCoord(status->m_stcomposition.st_vertices3[i].st_vertex0.st_coord3,c->geometry[i].texel);
				}

				for (i=0;i<c->meshSize;i++)
				{
					copyTriangle(status->m_stcomposition.st_triangles7[i].st_triangle0,c->mesh[i]);
				}

				compositions.SetAt(status->m_stcomposition.st_objectbase0.st_string0,(void*&)c );
			}
			else
				output->Output("* Object already defined !");
			break;
		}
		case RULE_extern_ID:
		{
			rays_composition_header_t *c;
			CGeometry *geo = (CGeometry*)set->GetFirstChild();
			
			while (geo != NULL)
			{
				CString name = status->m_stextern.st_objectbase0.st_string0+"_"+geo->Name();
				BOOL res = compositions.Lookup(name,(void*&)c );

				if (res == NULL)
				{
					c = new rays_composition_header_t;
					strcpy(c->base.name,LPCTSTR(status->m_stsphere.st_objectbase0.st_string0));
					copyCoord(status->m_stextern.st_objectbase0.st_coord8,c->base.color);

					updateShading(c->base.shading);
					updateTexture(c->base.texture);
					updateNormal(c->base.normal);

					c->geometrySize = geo->NbVertex();
					c->meshSize = geo->NbFace();

					c->geometry = new rays_vertex_t[c->geometrySize];
					c->mesh = new rays_triangle_t[c->meshSize];

					GL_VERTEX v;
					geo->GetCenter(v.coords);
					geo->Translate(-v.coords.x,-v.coords.y,-v.coords.z);
					geo->RotationX(-90.0f);
					//geo->RotationY(45.0f);
					//geo->RotationX(30.0f);
					
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

					compositions.SetAt(name,(void*&)c );
				}
				else
					output->Output("* Object already defined !");

				geo = (CGeometry*)(geo->GetAfter());
			}
			break;
		}
		default:
			break;
	}
}

void CAnalyser::Onsphere(UINT RULE_ID) 
{ 
	if (verbose)
	if (status->m_stsphere.m_step == 0)
	{
		CString sphere;

		sphere.Format("Sphere parsed : radius %f",status->m_stsphere.st_float3);
		output->Output(sphere);
	}
}


void CAnalyser::Oncomposition(UINT) 
{ 
	if (verbose)
	if (status->m_stcomposition.m_step == 0)
	{
		output->Output("Geometric composition parsed :");

		CString composition;
		composition.Format("%d vertices and %d triangles",
							status->m_stcomposition.st_vertices3.GetSize(),
							status->m_stcomposition.st_triangles7.GetSize());

		output->Output(composition);
	}
}

void CAnalyser::Onextern(UINT) 
{ 
	if (status->m_stextern.m_step == 0)
	{
		output->Output("Importing extern composition ... :");

		int len = status->m_stextern.st_string2.GetLength();
		CString fname = status->m_stextern.st_string2.Mid(1,len-2);
		fname = miniRaysClient.scene_location + fname;
		bool res = Raptor::glLoadScene(fname,set);

		CString composition;

		if (res)
			composition = "Extern composition imported :" + fname;
		else
		{
			composition = "Failed to import extern composition :" + fname;
			Abort();
		}

		output->Output(composition);
	}
}

void CAnalyser::Onplugin_call(UINT)
{
	if (status->m_stplugin_call.m_step == 0)
	{
		CString pname = status->m_stplugin_call.st_string1;
		pname = pname.Mid(1,pname.GetLength() - 2);
		output->Output("Calling plugin " + pname);

		rays_plugin_t *plugin = new rays_plugin_t;
		plugin->nbParams = 0;
		memset(plugin->name,0,MAX_STR_LEN);
		strcpy(plugin->name,LPCTSTR(pname));


		//	TODO
		//	handle the case of maxparams : 16
		//
		const functionparameter_t &params = status->m_stplugin_call.st_functionparameter3;
		switch (params.st_functionparameterID)
		{
			case RULE_functionparameter_noitem_ID:
				output->Output("  plugin has no parameters");
				break;
			case RULE_functionparameter_lastitem_ID:
			{
				vl_parameter_t value = ((lpfunctionparameter_lastitem_t)(params.p_stfunctionparameter))->st_vl_parameter0;
				if (value.st_vl_parameterID == ATOM_float_ID)
					plugin->params[plugin->nbParams++] = value.vl_parameter_value.float_0;
				else if (value.st_vl_parameterID == ATOM_integer_ID)
					plugin->params[plugin->nbParams++] = value.vl_parameter_value.integer_1;
				break;
			}
			case RULE_functionparameter_listitem_ID:
			{
				lpfunctionparameter_listitem_t listitem = ((lpfunctionparameter_listitem_t)(params.p_stfunctionparameter));
				lpfunctionparameter_lastitem_t lastitem = NULL;
				
				while (listitem != NULL)
				{
					vl_parameter_t value = listitem->st_vl_parameter0;
					if (value.st_vl_parameterID == ATOM_float_ID)
						plugin->params[plugin->nbParams++] = value.vl_parameter_value.float_0;
					else if (value.st_vl_parameterID == ATOM_integer_ID)
						plugin->params[plugin->nbParams++] = value.vl_parameter_value.integer_1;

					if (listitem->st_functionparameter_item2.st_functionparameter_itemID == RULE_functionparameter_listitem_ID)
						listitem = (lpfunctionparameter_listitem_t)(listitem->st_functionparameter_item2.p_stfunctionparameter_item);
					else
					{
						lastitem = (lpfunctionparameter_lastitem_t)(listitem->st_functionparameter_item2.p_stfunctionparameter_item);
						listitem = NULL;
					}
				}

				if (lastitem != NULL)
				{
					vl_parameter_t value = lastitem->st_vl_parameter0;
					if (value.st_vl_parameterID == ATOM_float_ID)
						plugin->params[plugin->nbParams++] = value.vl_parameter_value.float_0;
					else if (value.st_vl_parameterID == ATOM_integer_ID)
						plugin->params[plugin->nbParams++] = value.vl_parameter_value.integer_1;
				}

				break;
			}
			default:
				break;
		}
		plugins.SetAt(LPCTSTR(pname),plugin);
	}
}

void CAnalyser::Onframe_element(UINT)
{
	if (status->m_stframe_element.m_step == 0)
	{
		if (status->m_stframe_element.st_frame_elementID == RULE_transform_ID)
		{
			lptransform_t pt = (lptransform_t)(status->m_stframe_element.p_stframe_element);

			rays_transform_t t;
			memset(t.target,0,MAX_STR_LEN);
			strcpy(t.target,LPCTSTR(pt->st_string1));
			t.coeffs.x = pt->st_float3;
			t.coeffs.y = pt->st_float5;
			t.coeffs.z = pt->st_float7;
			t.coeffs.h = pt->st_float9;
			switch(pt->st_transform_type0.st_transform_typeID)
			{
				case ATOM_scale_ID:
					t.transform = SCALE;
					break;
				case ATOM_rotate_ID:
					t.transform = ROTATE;
					break;
				case ATOM_translate_ID:
					t.transform = TRANSLATE;
					break;
			}

			currentTransforms[currentFrame.nbTransforms] = t;
			currentFrame.nbTransforms++;
			
			//currentFrame.transforms
		}
		else if (status->m_stframe_element.st_frame_elementID == RULE_plugin_call_ID)
		{
			lpplugin_call_t pp = (lpplugin_call_t)(status->m_stframe_element.p_stframe_element);
			rays_plugin_t *p = NULL;

			CString name = pp->st_string1.Mid(1,pp->st_string1.GetLength()-2);
			plugins.Lookup(LPCTSTR(name),(void*&)p);

			currentPlugins[currentFrame.nbCalls] = *p;
			currentFrame.nbCalls++;
		}
	}
}

void CAnalyser::Onframes(UINT)
{
	if (status->m_stframes.m_step == 0)
	{
		rays_frame_t *pFrame = new rays_frame_t;

		pFrame->nbCalls = currentFrame.nbCalls;
		pFrame->nbTransforms = currentFrame.nbTransforms;
		pFrame->calls = new rays_plugin_t[currentFrame.nbCalls];
		pFrame->transforms = new rays_transform_t[currentFrame.nbTransforms];

		for (int i=0;i<currentFrame.nbCalls;i++)
			pFrame->calls[i] = currentPlugins[i];
		for (i=0;i<currentFrame.nbTransforms;i++)
			pFrame->transforms[i] = currentTransforms[i];

		int pos = frames.GetCount();
		frames.SetAt(pos,pFrame);

		currentFrame.nbCalls = 0;
		currentFrame.nbTransforms = 0;
	}
}

void CAnalyser::Onmain(UINT RULE_ID) 
{
	if (status->m_stmain.m_step == 0)
	{
		output->Output("Source file parsed : ");

		CString main;

		main = "Program name: " + status->m_stmain.st_string0;
		output->Output(main);

		main.Format("Config : %d",status->m_stmain.st_config4.GetSize());
		output->Output(main);

		main.Format("Defines : %d",status->m_stmain.st_define8.GetSize());
		output->Output(main);

		main.Format("Objects : %d",status->m_stmain.st_objects12.GetSize());
		output->Output(main);
	}		
}


///////////////////////////////////////////////////////////////////////////////
//
//	Analyser implementation
//
CAnalyser::CAnalyser(CParser *p,CClient<CCltSocket> *c):
CYacc(p),client(c),verbose(true),output(NULL)
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
	config.object_plane = 10.0f;
	config.focale = 50.0f;
	config.envtexname[0] = 0;
	config.photon_map = 200000;

	currentFrame.nbCalls = 0;
	currentFrame.nbTransforms = 0;

	status->Init();

	POSITION pos = lights.GetStartPosition();
	CString key;
	while (pos != NULL)
	{
		rays_light_t *l;
		lights.GetNextAssoc(pos,key,(void*&)l);
		delete l;
	}
	lights.RemoveAll();
	pos = shadings.GetStartPosition();
	while (pos != NULL)
	{
		rays_shading_t *s;
		shadings.GetNextAssoc(pos,key,(void*&)s);
		delete s;
	}
	shadings.RemoveAll();
	pos = textures.GetStartPosition();
	while (pos != NULL)
	{
		rays_texture_t *t;
		textures.GetNextAssoc(pos,key,(void*&)t);
		delete t;
	}
	textures.RemoveAll();
	pos = spheres.GetStartPosition();
	while (pos != NULL)
	{
		rays_sphere_t *s;
		spheres.GetNextAssoc(pos,key,(void*&)s);
		delete s;
	}
	spheres.RemoveAll();
	pos = compositions.GetStartPosition();
	while (pos != NULL)
	{
		rays_composition_header_t *s;
		compositions.GetNextAssoc(pos,key,(void*&)s);
		delete [] s->geometry;
		delete [] s->mesh;
		delete s;
	}
	compositions.RemoveAll();
	pos = plugins.GetStartPosition();
	while (pos != NULL)
	{
		rays_plugin_t *p;
		plugins.GetNextAssoc(pos,key,(void*&)p);
		delete p;
	}
	plugins.RemoveAll();
	pos = frames.GetStartPosition();
	while (pos != NULL)
	{
		WORD idx;
		rays_frame_t *f;
		frames.GetNextAssoc(pos,idx,(void*&)f);
		delete [] f->calls;
		delete [] f->transforms;
		delete f;
	}
	frames.RemoveAll();

	Raptor::glPurgeRaptor(false);
}


void CAnalyser::StartJob(unsigned int jobID)
{
	msg.msg_header = MSG_START;
	msg.msg_id = JOB_START;
	msg.msg_size = 0;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = config.width;
	msg.msg_data[2] = config.height;
	msg.msg_data[3] = client->GetPort();
	msg.msg_data[4] = client->GetAddr();
	msg.msg_tail = MSG_END;

	client->Write(&msg,MSGSIZE);
}

void CAnalyser::SendConfiguration(unsigned int jobID)
{
	output->Output("Sending configuration to server...");

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
	delete [] buffer;
}

void CAnalyser::RunJob(unsigned int jobID)
{
	output->Output("Sending job start to server...");

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
}

void CAnalyser::SendLights(unsigned int jobID)
{
	unsigned int nblights = lights.GetCount();
	CString lightsStr;
	lightsStr.Format("Sending %d lights to server...",nblights);
	output->Output(lightsStr);

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
	delete [] buffer;
}

void CAnalyser::SendSpheres(unsigned int jobID)
{
	unsigned int nbspheres = spheres.GetCount();
	CString sphereStr;
	sphereStr.Format("Sending %d spheres to server...",nbspheres);
	output->Output(sphereStr);

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
	delete [] buffer;
}

void CAnalyser::SendCompositions(unsigned int jobID)
{
	unsigned int nbcompositions = compositions.GetCount();
	CString compoStr;
	compoStr.Format("Sending %d compositions to server...",nbcompositions);
	output->Output(compoStr);

	CString key;
	rays_composition_header_t *s = NULL;
	POSITION pos = compositions.GetStartPosition();

	int compositionsize = nbcompositions * sizeof(rays_composition_header_t);
	for (unsigned int j=0;j<nbcompositions;j++)
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
	delete [] buffer;
}

void CAnalyser::SendPlugins(unsigned int jobID)
{
	CString compoPlg;
	compoPlg.Format("Sending %d plugins to server...",plugins.GetCount());
	output->Output(compoPlg);

	msg.msg_header = MSG_START;
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_PLUGIN;
	msg.msg_data[2] = plugins.GetCount();
	msg.msg_tail = MSG_DATA;

	int size = sizeof(rays_plugin_t);
	msg.msg_size = plugins.GetCount() * size;

	unsigned int offset = MSGSIZE;
	unsigned char *buffer = new unsigned char[msg.msg_size+MSGSIZE];

	POSITION pos = plugins.GetStartPosition();
	while(pos != NULL)
	{
		CString key;
		rays_plugin_t *plugin = NULL;

		plugins.GetNextAssoc(pos,key,(void*&)plugin);
		
		memcpy(buffer+offset,plugin,size);
		offset = offset + size;
	}

	memcpy(buffer,&msg,MSGSIZE);

	client->Write(buffer,msg.msg_size+MSGSIZE);
	delete [] buffer;
}


void CAnalyser::SendFrames(unsigned int jobID)
{
	unsigned int nbframes = frames.GetCount();
	CString frameStr;
	frameStr.Format("Sending %d frames to server...",nbframes);
	output->Output(frameStr);

	int totalsize = MSGSIZE;
	POSITION pos = frames.GetStartPosition();
	while (pos != NULL)
	{
		rays_frame_t *pFrame = NULL;
		WORD key = 0;
		frames.GetNextAssoc(pos,key,(void*&)pFrame);
		totalsize += sizeof(rays_frame_t);
		totalsize += pFrame->nbCalls * sizeof(rays_plugin_t);
		totalsize += pFrame->nbTransforms * sizeof(rays_transform_t);
	}
	unsigned char *buffer = new unsigned char[totalsize];

	msg.msg_header = MSG_START;
	msg.msg_id = OBJ_DATA;
	msg.msg_data[0] = jobID;
	msg.msg_data[1] = OBJ_FRAME;
	msg.msg_data[2] = nbframes;
	msg.msg_tail = MSG_DATA;
	msg.msg_size = totalsize - MSGSIZE;
	memcpy(buffer,&msg,MSGSIZE);

	unsigned char *bufferpos = buffer + MSGSIZE;
	for (int i=0;i<frames.GetCount();i++)
	{
		rays_frame_t *pFrame;
		frames.Lookup(i,(void*&)pFrame);

		memcpy(bufferpos,pFrame,sizeof(rays_frame_t));
		bufferpos+=sizeof(rays_frame_t);

		for (int i=0;i<pFrame->nbCalls;i++)
		{
			memcpy(bufferpos,&(pFrame->calls[i]),sizeof(rays_plugin_t));
			bufferpos += sizeof(rays_plugin_t);
		}

		for (i=0;i<pFrame->nbTransforms;i++)
		{
			memcpy(bufferpos,&(pFrame->transforms[i]),sizeof(rays_transform_t));
			bufferpos += sizeof(rays_transform_t);
		}
	}

	client->Write(buffer,totalsize);
	delete [] buffer;
}
