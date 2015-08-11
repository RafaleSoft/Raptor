//////////////////////////////////////////////////////////////////////////
//	This file is generated MicroLex application from Rafale Soft Inc. (c)
//
//	Do not modify this file, your changes would be lost on next generation
//	This file defines and implements a 'lex like' parser as well as a
//	'yacc like' interpreter to use with your application that requires 
//	a small language to format your data files in an expandable way
///////////////////////////////////////////////////////////////////////////


#pragma warning (disable : 4711)	// automatic inline expansion warning

#include "stdafx.h"
#include "lexyacc.h"




CLex::CLex():bufferpos(BUFFER_SIZE),maxpos(BUFFER_SIZE),sourceReady(false)
{}

CLex::~CLex()
{}

CYacc::CYacc(CLex *parser):lex(parser),abort(false)
{}

CYacc::~CYacc()
{}

// continous char provider from file
char CLex::GetChar(void)
{
	char c;

	if (bufferpos == maxpos)
	{
		if (source.GetPosition()<source.GetLength())
		{
			maxpos = source.Read(buffer,BUFFER_SIZE);
			bufferpos = 0;

			if (maxpos == 0)
			{
				bufferpos = -1;
				c = EOF;
			}
			else
				c = buffer[bufferpos++];
		}
		else
		{
			bufferpos = -1;
			c = EOF;
		}
	}
	else
		c = buffer[bufferpos++];

	if (c == NEW_LINE)
	{
		numLine++;
		linePos = 0;
	}
	else
		linePos++;

	chunck+=c;

	return c;
}

bool CLex::InitLex(CString filename)
{
	numLine = 1;
	linePos = 1;
	CFileException	exc;

	if (FALSE == source.Open(LPCTSTR(filename),CFile::modeRead|CFile::shareDenyWrite,&exc))
	{
		OnError(CLex::LEX_ERR_SRC);
		sourceReady = false;
	}
	else
	{
		lastChar = GetChar();
		sourceReady = true;
	}
	return sourceReady;
}

bool CLex::FinishLex(void)
{
	try
	{
		sourceReady = false;
		bufferpos = BUFFER_SIZE;
		maxpos = BUFFER_SIZE;
		lastChar = 0;
		source.Close();
		return true;
	}
	catch(CFileException)
	{
		OnError(CLex::LEX_ERR_SRC);
		return false;
	}
}

void CYaccStatus::Init(void)
{
	m_in = false;
	m_out = false;
	m_sep = false;
	m_set = false;
	m_width = false;
	m_height = false;
	m_variance = false;
	m_deflection = false;
	m_defraction = false;
	m_crease = false;
	m_separate_specular = false;
	m_none = false;
	m_vertex = false;
	m_shading = false;
	m_rendering = false;
	m_light = false;
	m_texture = false;
	m_triangle = false;
	m_geometry = false;
	m_mesh = false;
	m_import = false;
	m_stshading.st_shadingoptions15.RemoveAll();
	m_strender.st_renderoptions1.RemoveAll();
	m_stcomposition.st_vertices3.RemoveAll();
	m_stcomposition.st_triangles7.RemoveAll();
	m_stmain.st_config3.RemoveAll();
	m_stmain.st_define6.RemoveAll();
	m_stmain.st_objects9.RemoveAll();
}



//Syntaxic structures operators
coord_tag& coord_tag::operator=(const coord_tag& rsh)
{
	st_float1 = rsh.st_float1;
	st_float3 = rsh.st_float3;
	st_float5 = rsh.st_float5;
	st_float7 = rsh.st_float7;
	m_step = rsh.m_step;
	return *this;
}

vertex_tag& vertex_tag::operator=(const vertex_tag& rsh)
{
	st_coord1 = rsh.st_coord1;
	st_coord3 = rsh.st_coord3;
	m_step = rsh.m_step;
	return *this;
}

triangle_tag& triangle_tag::operator=(const triangle_tag& rsh)
{
	st_integer1 = rsh.st_integer1;
	st_integer3 = rsh.st_integer3;
	st_integer5 = rsh.st_integer5;
	m_step = rsh.m_step;
	return *this;
}

shadingoption_tag& shadingoption_tag::operator=(const shadingoption_tag& rsh)
{
	st_shadingoptionID = rsh.st_shadingoptionID;
	m_step = rsh.m_step;
	return *this;
}

shadingoptions_tag& shadingoptions_tag::operator=(const shadingoptions_tag& rsh)
{
	st_shadingoption0 = rsh.st_shadingoption0;
	m_step = rsh.m_step;
	return *this;
}

shading_tag& shading_tag::operator=(const shading_tag& rsh)
{
	st_float1 = rsh.st_float1;
	st_float3 = rsh.st_float3;
	st_float5 = rsh.st_float5;
	st_integer7 = rsh.st_integer7;
	st_float9 = rsh.st_float9;
	st_float11 = rsh.st_float11;
	st_float13 = rsh.st_float13;
	st_shadingoptions15.Copy(rsh.st_shadingoptions15);
	m_step = rsh.m_step;
	return *this;
}

light_tag& light_tag::operator=(const light_tag& rsh)
{
	st_coord1 = rsh.st_coord1;
	st_coord3 = rsh.st_coord3;
	st_float5 = rsh.st_float5;
	m_step = rsh.m_step;
	return *this;
}

texture_tag& texture_tag::operator=(const texture_tag& rsh)
{
	st_string1 = rsh.st_string1;
	m_step = rsh.m_step;
	return *this;
}

renderoption_tag& renderoption_tag::operator=(const renderoption_tag& rsh)
{
	m_step = rsh.m_step;
	return *this;
}

renderoptions_tag& renderoptions_tag::operator=(const renderoptions_tag& rsh)
{
	st_renderoption0 = rsh.st_renderoption0;
	m_step = rsh.m_step;
	return *this;
}

render_tag& render_tag::operator=(const render_tag& rsh)
{
	st_renderoptions1.Copy(rsh.st_renderoptions1);
	m_step = rsh.m_step;
	return *this;
}

parameter_tag& parameter_tag::operator=(const parameter_tag& rsh)
{
	st_parameterID = rsh.st_parameterID;
	m_step = rsh.m_step;
	return *this;
}

element_tag& element_tag::operator=(const element_tag& rsh)
{
	st_elementID = rsh.st_elementID;
	p_stelement = rsh.p_stelement;
	m_step = rsh.m_step;
	return *this;
}

elementname_tag& elementname_tag::operator=(const elementname_tag& rsh)
{
	st_elementnameID = rsh.st_elementnameID;
	m_step = rsh.m_step;
	return *this;
}

define_tag& define_tag::operator=(const define_tag& rsh)
{
	st_elementname0 = rsh.st_elementname0;
	st_string1 = rsh.st_string1;
	st_element3 = rsh.st_element3;
	m_step = rsh.m_step;
	return *this;
}

config_tag& config_tag::operator=(const config_tag& rsh)
{
	st_parameter0 = rsh.st_parameter0;
	st_integer2 = rsh.st_integer2;
	m_step = rsh.m_step;
	return *this;
}

triangles_tag& triangles_tag::operator=(const triangles_tag& rsh)
{
	st_triangle0 = rsh.st_triangle0;
	m_step = rsh.m_step;
	return *this;
}

vertices_tag& vertices_tag::operator=(const vertices_tag& rsh)
{
	st_vertex0 = rsh.st_vertex0;
	m_step = rsh.m_step;
	return *this;
}

objectbase_tag& objectbase_tag::operator=(const objectbase_tag& rsh)
{
	st_string0 = rsh.st_string0;
	st_string2 = rsh.st_string2;
	st_string4 = rsh.st_string4;
	st_coord6 = rsh.st_coord6;
	m_step = rsh.m_step;
	return *this;
}

composition_tag& composition_tag::operator=(const composition_tag& rsh)
{
	st_objectbase0 = rsh.st_objectbase0;
	st_vertices3.Copy(rsh.st_vertices3);
	st_triangles7.Copy(rsh.st_triangles7);
	m_step = rsh.m_step;
	return *this;
}

sphere_tag& sphere_tag::operator=(const sphere_tag& rsh)
{
	st_objectbase0 = rsh.st_objectbase0;
	st_coord1 = rsh.st_coord1;
	st_float3 = rsh.st_float3;
	m_step = rsh.m_step;
	return *this;
}

extern_tag& extern_tag::operator=(const extern_tag& rsh)
{
	st_objectbase0 = rsh.st_objectbase0;
	st_string2 = rsh.st_string2;
	m_step = rsh.m_step;
	return *this;
}

object_tag& object_tag::operator=(const object_tag& rsh)
{
	st_objectID = rsh.st_objectID;
	p_stobject = rsh.p_stobject;
	m_step = rsh.m_step;
	return *this;
}

objects_tag& objects_tag::operator=(const objects_tag& rsh)
{
	st_object0 = rsh.st_object0;
	m_step = rsh.m_step;
	return *this;
}

main_tag& main_tag::operator=(const main_tag& rsh)
{
	st_string0 = rsh.st_string0;
	st_config3.Copy(rsh.st_config3);
	st_define6.Copy(rsh.st_define6);
	st_objects9.Copy(rsh.st_objects9);
	m_step = rsh.m_step;
	return *this;
}

void CYacc::m_oncoord(UINT RULE_ID)
{
	switch(m_status.m_stcoord.m_step)
	{
		case 1:
			m_status.m_stcoord.st_float1=MapTypefloat(lex->GetChunck());
			break;
		case 3:
			m_status.m_stcoord.st_float3=MapTypefloat(lex->GetChunck());
			break;
		case 5:
			m_status.m_stcoord.st_float5=MapTypefloat(lex->GetChunck());
			break;
		case 7:
			m_status.m_stcoord.st_float7=MapTypefloat(lex->GetChunck());
			break;
		case 8:
			m_status.m_stcoord.m_step=-1;
			break;
		default:
			if (m_status.m_stcoord.m_step > 8)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stcoord.m_step++;
	Oncoord(RULE_ID);
}

void CYacc::m_onvertex(UINT RULE_ID)
{
	switch(m_status.m_stvertex.m_step)
	{
		case 1:
			m_status.m_stvertex.st_coord1=m_status.m_stcoord;
			break;
		case 3:
			m_status.m_stvertex.st_coord3=m_status.m_stcoord;
			break;
		case 4:
			m_status.m_stvertex.m_step=-1;
			break;
		default:
			if (m_status.m_stvertex.m_step > 4)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stvertex.m_step++;
	Onvertex(RULE_ID);
}

void CYacc::m_ontriangle(UINT RULE_ID)
{
	switch(m_status.m_sttriangle.m_step)
	{
		case 1:
			m_status.m_sttriangle.st_integer1=MapTypeinteger(lex->GetChunck());
			break;
		case 3:
			m_status.m_sttriangle.st_integer3=MapTypeinteger(lex->GetChunck());
			break;
		case 5:
			m_status.m_sttriangle.st_integer5=MapTypeinteger(lex->GetChunck());
			break;
		case 6:
			m_status.m_sttriangle.m_step=-1;
			break;
		default:
			if (m_status.m_sttriangle.m_step > 6)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_sttriangle.m_step++;
	Ontriangle(RULE_ID);
}

void CYacc::m_onshadingoption(UINT RULE_ID)
{
	m_status.m_stshadingoption.st_shadingoptionID=RULE_ID;
	m_status.m_stshadingoption.m_step=0;
	Onshadingoption(RULE_ID);
}

void CYacc::m_onshadingoptions(UINT RULE_ID)
{
	switch(m_status.m_stshadingoptions.m_step)
	{
		case 0:
			m_status.m_stshadingoptions.st_shadingoption0=m_status.m_stshadingoption;
			m_status.m_stshadingoptions.m_step=-1;
			break;
		default:
			if (m_status.m_stshadingoptions.m_step > 0)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stshadingoptions.m_step++;
	Onshadingoptions(RULE_ID);
}

void CYacc::m_onshading(UINT RULE_ID)
{
	switch(m_status.m_stshading.m_step)
	{
		case 1:
			m_status.m_stshading.st_float1=MapTypefloat(lex->GetChunck());
			break;
		case 3:
			m_status.m_stshading.st_float3=MapTypefloat(lex->GetChunck());
			break;
		case 5:
			m_status.m_stshading.st_float5=MapTypefloat(lex->GetChunck());
			break;
		case 7:
			m_status.m_stshading.st_integer7=MapTypeinteger(lex->GetChunck());
			break;
		case 9:
			m_status.m_stshading.st_float9=MapTypefloat(lex->GetChunck());
			break;
		case 11:
			m_status.m_stshading.st_float11=MapTypefloat(lex->GetChunck());
			break;
		case 13:
			m_status.m_stshading.st_float13=MapTypefloat(lex->GetChunck());
			break;
		case 15:
			m_status.m_stshading.st_shadingoptions15.Add(m_status.m_stshadingoptions);
			break;
		case 16:
			m_status.m_stshading.m_step=-1;
			break;
		default:
			if (m_status.m_stshading.m_step > 16)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stshading.m_step++;
	Onshading(RULE_ID);
}

void CYacc::m_onlight(UINT RULE_ID)
{
	switch(m_status.m_stlight.m_step)
	{
		case 1:
			m_status.m_stlight.st_coord1=m_status.m_stcoord;
			break;
		case 3:
			m_status.m_stlight.st_coord3=m_status.m_stcoord;
			break;
		case 5:
			m_status.m_stlight.st_float5=MapTypefloat(lex->GetChunck());
			break;
		case 6:
			m_status.m_stlight.m_step=-1;
			break;
		default:
			if (m_status.m_stlight.m_step > 6)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stlight.m_step++;
	Onlight(RULE_ID);
}

void CYacc::m_ontexture(UINT RULE_ID)
{
	switch(m_status.m_sttexture.m_step)
	{
		case 1:
			m_status.m_sttexture.st_string1=MapTypestring(lex->GetChunck());
			break;
		case 2:
			m_status.m_sttexture.m_step=-1;
			break;
		default:
			if (m_status.m_sttexture.m_step > 2)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_sttexture.m_step++;
	Ontexture(RULE_ID);
}

void CYacc::m_onrenderoption(UINT RULE_ID)
{
	switch(m_status.m_strenderoption.m_step)
	{
		case 1:
			m_status.m_strenderoption.m_step=-1;
			break;
		default:
			if (m_status.m_strenderoption.m_step > 1)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_strenderoption.m_step++;
	Onrenderoption(RULE_ID);
}

void CYacc::m_onrenderoptions(UINT RULE_ID)
{
	switch(m_status.m_strenderoptions.m_step)
	{
		case 0:
			m_status.m_strenderoptions.st_renderoption0=m_status.m_strenderoption;
			m_status.m_strenderoptions.m_step=-1;
			break;
		default:
			if (m_status.m_strenderoptions.m_step > 0)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_strenderoptions.m_step++;
	Onrenderoptions(RULE_ID);
}

void CYacc::m_onrender(UINT RULE_ID)
{
	switch(m_status.m_strender.m_step)
	{
		case 1:
			m_status.m_strender.st_renderoptions1.Add(m_status.m_strenderoptions);
			break;
		case 2:
			m_status.m_strender.m_step=-1;
			break;
		default:
			if (m_status.m_strender.m_step > 2)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_strender.m_step++;
	Onrender(RULE_ID);
}

void CYacc::m_onparameter(UINT RULE_ID)
{
	m_status.m_stparameter.st_parameterID=RULE_ID;
	m_status.m_stparameter.m_step=0;
	Onparameter(RULE_ID);
}

void CYacc::m_onelement(UINT RULE_ID)
{
	m_status.m_stelement.st_elementID=RULE_ID;
	m_status.m_stelement.m_step=0;

	switch(RULE_ID)
	{
		case 30:
		{
			shading_t *p = new shading_t;
			*p = m_status.m_stshading;
			m_status.m_stelement.p_stelement = p;
			break;
		}
		case 31:
		{
			light_t *p = new light_t;
			*p = m_status.m_stlight;
			m_status.m_stelement.p_stelement = p;
			break;
		}
		case 32:
		{
			texture_t *p = new texture_t;
			*p = m_status.m_sttexture;
			m_status.m_stelement.p_stelement = p;
			break;
		}
		case 35:
		{
			render_t *p = new render_t;
			*p = m_status.m_strender;
			m_status.m_stelement.p_stelement = p;
			break;
		}
		default:
			m_status.m_stelement.p_stelement = NULL;
			break;
	}

	Onelement(RULE_ID);
}

void CYacc::m_onelementname(UINT RULE_ID)
{
	m_status.m_stelementname.st_elementnameID=RULE_ID;
	m_status.m_stelementname.m_step=0;
	Onelementname(RULE_ID);
}

void CYacc::m_ondefine(UINT RULE_ID)
{
	switch(m_status.m_stdefine.m_step)
	{
		case 0:
			m_status.m_stdefine.st_elementname0=m_status.m_stelementname;
			break;
		case 1:
			m_status.m_stdefine.st_string1=MapTypestring(lex->GetChunck());
			break;
		case 3:
			m_status.m_stdefine.st_element3=m_status.m_stelement;
			m_status.m_stdefine.m_step=-1;
			break;
		default:
			if (m_status.m_stdefine.m_step > 3)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stdefine.m_step++;
	Ondefine(RULE_ID);
}

void CYacc::m_onconfig(UINT RULE_ID)
{
	switch(m_status.m_stconfig.m_step)
	{
		case 0:
			m_status.m_stconfig.st_parameter0=m_status.m_stparameter;
			break;
		case 2:
			m_status.m_stconfig.st_integer2=MapTypeinteger(lex->GetChunck());
			m_status.m_stconfig.m_step=-1;
			break;
		default:
			if (m_status.m_stconfig.m_step > 2)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stconfig.m_step++;
	Onconfig(RULE_ID);
}

void CYacc::m_ontriangles(UINT RULE_ID)
{
	switch(m_status.m_sttriangles.m_step)
	{
		case 0:
			m_status.m_sttriangles.st_triangle0=m_status.m_sttriangle;
			m_status.m_sttriangles.m_step=-1;
			break;
		default:
			if (m_status.m_sttriangles.m_step > 0)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_sttriangles.m_step++;
	Ontriangles(RULE_ID);
}

void CYacc::m_onvertices(UINT RULE_ID)
{
	switch(m_status.m_stvertices.m_step)
	{
		case 0:
			m_status.m_stvertices.st_vertex0=m_status.m_stvertex;
			m_status.m_stvertices.m_step=-1;
			break;
		default:
			if (m_status.m_stvertices.m_step > 0)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stvertices.m_step++;
	Onvertices(RULE_ID);
}

void CYacc::m_onobjectbase(UINT RULE_ID)
{
	switch(m_status.m_stobjectbase.m_step)
	{
		case 0:
			m_status.m_stobjectbase.st_string0=MapTypestring(lex->GetChunck());
			break;
		case 2:
			m_status.m_stobjectbase.st_string2=MapTypestring(lex->GetChunck());
			break;
		case 4:
			m_status.m_stobjectbase.st_string4=MapTypestring(lex->GetChunck());
			break;
		case 6:
			m_status.m_stobjectbase.st_coord6=m_status.m_stcoord;
			break;
		case 7:
			m_status.m_stobjectbase.m_step=-1;
			break;
		default:
			if (m_status.m_stobjectbase.m_step > 7)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stobjectbase.m_step++;
	Onobjectbase(RULE_ID);
}

void CYacc::m_oncomposition(UINT RULE_ID)
{
	switch(m_status.m_stcomposition.m_step)
	{
		case 0:
			m_status.m_stcomposition.st_objectbase0=m_status.m_stobjectbase;
			break;
		case 3:
			m_status.m_stcomposition.st_vertices3.Add(m_status.m_stvertices);
			break;
		case 7:
			m_status.m_stcomposition.st_triangles7.Add(m_status.m_sttriangles);
			break;
		case 9:
			m_status.m_stcomposition.m_step=-1;
			break;
		default:
			if (m_status.m_stcomposition.m_step > 9)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stcomposition.m_step++;
	Oncomposition(RULE_ID);
}

void CYacc::m_onsphere(UINT RULE_ID)
{
	switch(m_status.m_stsphere.m_step)
	{
		case 0:
			m_status.m_stsphere.st_objectbase0=m_status.m_stobjectbase;
			break;
		case 1:
			m_status.m_stsphere.st_coord1=m_status.m_stcoord;
			break;
		case 3:
			m_status.m_stsphere.st_float3=MapTypefloat(lex->GetChunck());
			break;
		case 4:
			m_status.m_stsphere.m_step=-1;
			break;
		default:
			if (m_status.m_stsphere.m_step > 4)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stsphere.m_step++;
	Onsphere(RULE_ID);
}

void CYacc::m_onextern(UINT RULE_ID)
{
	switch(m_status.m_stextern.m_step)
	{
		case 0:
			m_status.m_stextern.st_objectbase0=m_status.m_stobjectbase;
			break;
		case 2:
			m_status.m_stextern.st_string2=MapTypestring(lex->GetChunck());
			break;
		case 3:
			m_status.m_stextern.m_step=-1;
			break;
		default:
			if (m_status.m_stextern.m_step > 3)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stextern.m_step++;
	Onextern(RULE_ID);
}

void CYacc::m_onobject(UINT RULE_ID)
{
	m_status.m_stobject.st_objectID=RULE_ID;
	m_status.m_stobject.m_step=0;

	switch(RULE_ID)
	{
		case 44:
		{
			composition_t *p = new composition_t;
			*p = m_status.m_stcomposition;
			m_status.m_stobject.p_stobject = p;
			break;
		}
		case 45:
		{
			sphere_t *p = new sphere_t;
			*p = m_status.m_stsphere;
			m_status.m_stobject.p_stobject = p;
			break;
		}
		case 46:
		{
			extern_t *p = new extern_t;
			*p = m_status.m_stextern;
			m_status.m_stobject.p_stobject = p;
			break;
		}
		default:
			m_status.m_stobject.p_stobject = NULL;
			break;
	}

	Onobject(RULE_ID);
}

void CYacc::m_onobjects(UINT RULE_ID)
{
	switch(m_status.m_stobjects.m_step)
	{
		case 0:
			m_status.m_stobjects.st_object0=m_status.m_stobject;
			m_status.m_stobjects.m_step=-1;
			break;
		default:
			if (m_status.m_stobjects.m_step > 0)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stobjects.m_step++;
	Onobjects(RULE_ID);
}

void CYacc::m_onmain(UINT RULE_ID)
{
	switch(m_status.m_stmain.m_step)
	{
		case 0:
			m_status.m_stmain.st_string0=MapTypestring(lex->GetChunck());
			break;
		case 3:
			m_status.m_stmain.st_config3.Add(m_status.m_stconfig);
			break;
		case 6:
			m_status.m_stmain.st_define6.Add(m_status.m_stdefine);
			break;
		case 9:
			m_status.m_stmain.st_objects9.Add(m_status.m_stobjects);
			break;
		case 11:
			m_status.m_stmain.m_step=-1;
			break;
		default:
			if (m_status.m_stmain.m_step > 11)
				OnError(CYacc::YACC_ERR_SYN);
			break;
	}

	m_status.m_stmain.m_step++;
	Onmain(RULE_ID);
}

int CLex::Parse(char &chr)
{
	CString branch;
	char c;

	c = chr;

	if (c=='-')
		goto NODE_32c6d0;
	branch="\"";
	if (branch.Find(c)>=0)
		goto NODE_32c758;
	if (c=='{')
		goto NODE_32c7d0;
	if (c=='}')
		goto NODE_32c838;
	if (c==',')
		goto NODE_32c8b0;
	if (c=='=')
		goto NODE_32c928;
	if (c=='w')
		goto NODE_32c9a0;
	if (c=='h')
		goto NODE_32ca08;
	if (c=='v')
		goto NODE_32ca70;
	if (c=='d')
		goto NODE_32c7b0;
	if (c=='c')
		goto NODE_32cb88;
	if (c=='s')
		goto NODE_32cbe0;
	if (c=='n')
		goto NODE_32cca0;
	if (c=='r')
		goto NODE_32ccf8;
	if (c=='l')
		goto NODE_32cd60;
	if (c=='t')
		goto NODE_32cdc8;
	if (c=='g')
		goto NODE_32ce88;
	if (c=='m')
		goto NODE_32cee0;
	if (c=='i')
		goto NODE_32cf48;
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32d010;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32c6d0:
	c=GetChar();
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32d098;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d098:
	c=GetChar();
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32d098;
	if (c=='.')
		goto NODE_32e0a0;
	chr = c;
	return 1;

NODE_32e0a0:
	c=GetChar();
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32e0a0;
	chr = c;
	return 2;

NODE_32c758:
	c=GetChar();
	branch="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.";
	if (branch.Find(c)>=0)
		goto NODE_32c758;
	branch="\"";
	if (branch.Find(c)>=0)
		goto NODE_32d908;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d908:
	c=GetChar();
	chr = c;
	return 3;

NODE_32c7d0:
	c=GetChar();
	chr = c;
	return 4;

NODE_32c838:
	c=GetChar();
	chr = c;
	return 5;

NODE_32c8b0:
	c=GetChar();
	chr = c;
	return 6;

NODE_32c928:
	c=GetChar();
	chr = c;
	return 7;

NODE_32c9a0:
	c=GetChar();
	if (c=='i')
		goto NODE_32d188;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d188:
	c=GetChar();
	if (c=='d')
		goto NODE_32d980;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d980:
	c=GetChar();
	if (c=='t')
		goto NODE_32e118;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e118:
	c=GetChar();
	if (c=='h')
		goto NODE_32e928;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e928:
	c=GetChar();
	chr = c;
	return 8;

NODE_32ca08:
	c=GetChar();
	if (c=='e')
		goto NODE_32d1d0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d1d0:
	c=GetChar();
	if (c=='i')
		goto NODE_32da08;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32da08:
	c=GetChar();
	if (c=='g')
		goto NODE_32e1a0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e1a0:
	c=GetChar();
	if (c=='h')
		goto NODE_32e990;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e990:
	c=GetChar();
	if (c=='t')
		goto NODE_32ef80;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ef80:
	c=GetChar();
	chr = c;
	return 9;

NODE_32ca70:
	c=GetChar();
	if (c=='a')
		goto NODE_32d238;
	if (c=='e')
		goto NODE_32d2a0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d238:
	c=GetChar();
	if (c=='r')
		goto NODE_32da60;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32da60:
	c=GetChar();
	if (c=='i')
		goto NODE_32e1f8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e1f8:
	c=GetChar();
	if (c=='a')
		goto NODE_32e9f8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e9f8:
	c=GetChar();
	if (c=='n')
		goto NODE_32eff8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32eff8:
	c=GetChar();
	if (c=='c')
		goto NODE_32f538;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f538:
	c=GetChar();
	if (c=='e')
		goto NODE_32f928;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f928:
	c=GetChar();
	chr = c;
	return 10;

NODE_32d2a0:
	c=GetChar();
	if (c=='r')
		goto NODE_32dac8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dac8:
	c=GetChar();
	if (c=='t')
		goto NODE_32e260;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e260:
	c=GetChar();
	if (c=='e')
		goto NODE_32ea80;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ea80:
	c=GetChar();
	if (c=='x')
		goto NODE_32f070;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f070:
	c=GetChar();
	chr = c;
	return 16;

NODE_32c7b0:
	c=GetChar();
	if (c=='e')
		goto NODE_32d168;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d168:
	c=GetChar();
	if (c=='f')
		goto NODE_32db30;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32db30:
	c=GetChar();
	if (c=='l')
		goto NODE_32e2c8;
	if (c=='r')
		goto NODE_32e180;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e2c8:
	c=GetChar();
	if (c=='e')
		goto NODE_32ead8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ead8:
	c=GetChar();
	if (c=='c')
		goto NODE_32f0e8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f0e8:
	c=GetChar();
	if (c=='t')
		goto NODE_32f5b0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f5b0:
	c=GetChar();
	if (c=='i')
		goto NODE_32f9a0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f9a0:
	c=GetChar();
	if (c=='o')
		goto NODE_32fa80;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fa80:
	c=GetChar();
	if (c=='n')
		goto NODE_32fd00;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fd00:
	c=GetChar();
	chr = c;
	return 11;

NODE_32e180:
	c=GetChar();
	if (c=='a')
		goto NODE_32eb40;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32eb40:
	c=GetChar();
	if (c=='c')
		goto NODE_32f170;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f170:
	c=GetChar();
	if (c=='t')
		goto NODE_32f638;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f638:
	c=GetChar();
	if (c=='i')
		goto NODE_32fa18;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fa18:
	c=GetChar();
	if (c=='o')
		goto NODE_32fc98;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fc98:
	c=GetChar();
	if (c=='n')
		goto NODE_32fe48;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fe48:
	c=GetChar();
	chr = c;
	return 12;

NODE_32cb88:
	c=GetChar();
	if (c=='r')
		goto NODE_32d370;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d370:
	c=GetChar();
	if (c=='e')
		goto NODE_32dbd0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dbd0:
	c=GetChar();
	if (c=='a')
		goto NODE_32e378;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e378:
	c=GetChar();
	if (c=='s')
		goto NODE_32eba8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32eba8:
	c=GetChar();
	if (c=='e')
		goto NODE_32f1c8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f1c8:
	c=GetChar();
	chr = c;
	return 13;

NODE_32cbe0:
	c=GetChar();
	if (c=='e')
		goto NODE_32d3c8;
	if (c=='h')
		goto NODE_32d430;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d3c8:
	c=GetChar();
	if (c=='p')
		goto NODE_32dc28;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dc28:
	c=GetChar();
	if (c=='a')
		goto NODE_32e3e0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e3e0:
	c=GetChar();
	if (c=='r')
		goto NODE_32ea60;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ea60:
	c=GetChar();
	if (c=='a')
		goto NODE_32f240;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f240:
	c=GetChar();
	if (c=='t')
		goto NODE_32f690;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f690:
	c=GetChar();
	if (c=='e')
		goto NODE_32faa0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32faa0:
	c=GetChar();
	if (c=='_')
		goto NODE_32fd20;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fd20:
	c=GetChar();
	if (c=='s')
		goto NODE_32fec0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fec0:
	c=GetChar();
	if (c=='p')
		goto NODE_32ff48;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ff48:
	c=GetChar();
	if (c=='e')
		goto NODE_32ffb0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ffb0:
	c=GetChar();
	if (c=='c')
		goto NODE_8fa018;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_8fa018:
	c=GetChar();
	if (c=='u')
		goto NODE_8fa080;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_8fa080:
	c=GetChar();
	if (c=='l')
		goto NODE_8fa0e8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_8fa0e8:
	c=GetChar();
	if (c=='a')
		goto NODE_8fa150;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_8fa150:
	c=GetChar();
	if (c=='r')
		goto NODE_8fa1b8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_8fa1b8:
	c=GetChar();
	chr = c;
	return 14;

NODE_32d430:
	c=GetChar();
	if (c=='a')
		goto NODE_32dc90;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dc90:
	c=GetChar();
	if (c=='d')
		goto NODE_32e448;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e448:
	c=GetChar();
	if (c=='i')
		goto NODE_32ec58;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ec58:
	c=GetChar();
	if (c=='n')
		goto NODE_32f2a8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f2a8:
	c=GetChar();
	if (c=='g')
		goto NODE_32f6f8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f6f8:
	c=GetChar();
	chr = c;
	return 17;

NODE_32cca0:
	c=GetChar();
	if (c=='o')
		goto NODE_32d498;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d498:
	c=GetChar();
	if (c=='n')
		goto NODE_32dcf8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dcf8:
	c=GetChar();
	if (c=='e')
		goto NODE_32e4b0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e4b0:
	c=GetChar();
	chr = c;
	return 15;

NODE_32ccf8:
	c=GetChar();
	if (c=='e')
		goto NODE_32d500;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d500:
	c=GetChar();
	if (c=='n')
		goto NODE_32dd60;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dd60:
	c=GetChar();
	if (c=='d')
		goto NODE_32e528;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e528:
	c=GetChar();
	if (c=='e')
		goto NODE_32ecc0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ecc0:
	c=GetChar();
	if (c=='r')
		goto NODE_32f310;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f310:
	c=GetChar();
	if (c=='i')
		goto NODE_32f770;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f770:
	c=GetChar();
	if (c=='n')
		goto NODE_32faf8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32faf8:
	c=GetChar();
	if (c=='g')
		goto NODE_32fd78;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fd78:
	c=GetChar();
	chr = c;
	return 18;

NODE_32cd60:
	c=GetChar();
	if (c=='i')
		goto NODE_32d568;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d568:
	c=GetChar();
	if (c=='g')
		goto NODE_32ddc8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ddc8:
	c=GetChar();
	if (c=='h')
		goto NODE_32e590;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e590:
	c=GetChar();
	if (c=='t')
		goto NODE_32ed28;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ed28:
	c=GetChar();
	chr = c;
	return 19;

NODE_32cdc8:
	c=GetChar();
	if (c=='e')
		goto NODE_32d5d0;
	if (c=='r')
		goto NODE_32d638;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d5d0:
	c=GetChar();
	if (c=='x')
		goto NODE_32de30;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32de30:
	c=GetChar();
	if (c=='t')
		goto NODE_32e5f8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e5f8:
	c=GetChar();
	if (c=='u')
		goto NODE_32eda0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32eda0:
	c=GetChar();
	if (c=='r')
		goto NODE_32f150;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f150:
	c=GetChar();
	if (c=='e')
		goto NODE_32f7d8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f7d8:
	c=GetChar();
	chr = c;
	return 20;

NODE_32d638:
	c=GetChar();
	if (c=='i')
		goto NODE_32de98;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32de98:
	c=GetChar();
	if (c=='a')
		goto NODE_32e660;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e660:
	c=GetChar();
	if (c=='n')
		goto NODE_32ee08;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ee08:
	c=GetChar();
	if (c=='g')
		goto NODE_32f3c0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f3c0:
	c=GetChar();
	if (c=='l')
		goto NODE_32f850;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f850:
	c=GetChar();
	if (c=='e')
		goto NODE_32fb60;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fb60:
	c=GetChar();
	chr = c;
	return 21;

NODE_32ce88:
	c=GetChar();
	if (c=='e')
		goto NODE_32d6a0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d6a0:
	c=GetChar();
	if (c=='o')
		goto NODE_32df00;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32df00:
	c=GetChar();
	if (c=='m')
		goto NODE_32e6c8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e6c8:
	c=GetChar();
	if (c=='e')
		goto NODE_32ee70;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32ee70:
	c=GetChar();
	if (c=='t')
		goto NODE_32f428;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f428:
	c=GetChar();
	if (c=='r')
		goto NODE_32f618;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f618:
	c=GetChar();
	if (c=='y')
		goto NODE_32fbd8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32fbd8:
	c=GetChar();
	chr = c;
	return 22;

NODE_32cee0:
	c=GetChar();
	if (c=='e')
		goto NODE_32d708;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d708:
	c=GetChar();
	if (c=='s')
		goto NODE_32df68;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32df68:
	c=GetChar();
	if (c=='h')
		goto NODE_32e730;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e730:
	c=GetChar();
	chr = c;
	return 23;

NODE_32cf48:
	c=GetChar();
	if (c=='m')
		goto NODE_32d770;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32d770:
	c=GetChar();
	if (c=='p')
		goto NODE_32dfd0;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32dfd0:
	c=GetChar();
	if (c=='o')
		goto NODE_32e7a8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32e7a8:
	c=GetChar();
	if (c=='r')
		goto NODE_32eed8;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32eed8:
	c=GetChar();
	if (c=='t')
		goto NODE_32f490;
	OnError(LEX_ERR_SYN);
	return -1;

NODE_32f490:
	c=GetChar();
	chr = c;
	return 24;

NODE_32d010:
	c=GetChar();
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32d010;
	if (c=='.')
		goto NODE_32e038;
	chr = c;
	return 1;

NODE_32e038:
	c=GetChar();
	branch="0123456789";
	if (branch.Find(c)>=0)
		goto NODE_32e038;
	chr = c;
	return 2;

	chr = c;
}

int CLex::ReadAtom()
{
	if (!sourceReady)
		return -1;

	while IS_SEPARATOR(lastChar)
		lastChar = GetChar();

	chunck.Empty();
	chunck+=lastChar;

	return Parse(lastChar);
}

int CYacc::parsecoord(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_7:
	if (step>7) goto STEP_8;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_8:
	if (step>8) goto STEP_9;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_oncoord(result);
	if (abort) return -1;
STEP_9:

	result=RULE_coord_ID;
	return result;
}

int CYacc::parsevertex(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onvertex(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onvertex(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onvertex(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onvertex(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onvertex(result);
	if (abort) return -1;
STEP_5:

	result=RULE_vertex_ID;
	return result;
}

int CYacc::parsetriangle(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_integer_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_integer_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_integer_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_ontriangle(result);
	if (abort) return -1;
STEP_7:

	result=RULE_triangle_ID;
	return result;
}

int CYacc::parseshadingoption(int step)
{
	int result=0;
	int currentStep=0;
	UNREFERENCED_PARAMETER(step);

	result = lex->ReadAtom();
	if ((result!=ATOM_none_ID)&&(result!=ATOM_separate_specular_ID))
		return result;
	m_onshadingoption(result);
	if (abort) return -1;

	result=RULE_shadingoption_ID;
	return result;
}

int CYacc::parseshadingoptions(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseshadingoption(0);
	if (result!=RULE_shadingoption_ID)
		return result;
	m_onshadingoptions(result);
	if (abort) return -1;
STEP_1:

	result=RULE_shadingoptions_ID;
	return result;
}

int CYacc::parseshading(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_7:
	if (step>7) goto STEP_8;
	result = lex->ReadAtom();
	if (result!=ATOM_integer_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_8:
	if (step>8) goto STEP_9;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_9:
	if (step>9) goto STEP_10;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_10:
	if (step>10) goto STEP_11;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_11:
	if (step>11) goto STEP_12;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_12:
	if (step>12) goto STEP_13;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_13:
	if (step>13) goto STEP_14;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_14:
	if (step>14) goto STEP_15;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_15:
	if (step>15) goto STEP_16;
	currentStep = m_status.m_stshading.m_step;
	while ((result=parseshadingoptions(0))==RULE_shadingoptions_ID)
	{
		m_status.m_stshading.m_step = currentStep;
		m_onshading(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_onshading(result);
	if (abort) return -1;
STEP_16:

	result=RULE_shading_ID;
	return result;
}

int CYacc::parselight(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onlight(result);
	if (abort) return -1;
STEP_7:

	result=RULE_light_ID;
	return result;
}

int CYacc::parsetexture(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_ontexture(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_ontexture(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_ontexture(result);
	if (abort) return -1;
STEP_3:

	result=RULE_texture_ID;
	return result;
}

int CYacc::parserenderoption(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onrenderoption(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onrenderoption(result);
	if (abort) return -1;
STEP_2:

	result=RULE_renderoption_ID;
	return result;
}

int CYacc::parserenderoptions(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parserenderoption(0);
	if (result!=RULE_renderoption_ID)
		return result;
	m_onrenderoptions(result);
	if (abort) return -1;
STEP_1:

	result=RULE_renderoptions_ID;
	return result;
}

int CYacc::parserender(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onrender(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	currentStep = m_status.m_strender.m_step;
	while ((result=parserenderoptions(0))==RULE_renderoptions_ID)
	{
		m_status.m_strender.m_step = currentStep;
		m_onrender(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_onrender(result);
	if (abort) return -1;
STEP_2:

	result=RULE_render_ID;
	return result;
}

int CYacc::parseparameter(int step)
{
	int result=0;
	int currentStep=0;
	UNREFERENCED_PARAMETER(step);

	result = lex->ReadAtom();
	if ((result!=ATOM_crease_ID)&&(result!=ATOM_defraction_ID)&&(result!=ATOM_deflection_ID)&&(result!=ATOM_variance_ID)&&(result!=ATOM_height_ID)&&(result!=ATOM_width_ID))
		return result;
	m_onparameter(result);
	if (abort) return -1;

	result=RULE_parameter_ID;
	return result;
}

int CYacc::parseelement(int step)
{
	int result=0;
	int currentStep=0;
	UNREFERENCED_PARAMETER(step);

	m_status.m_stshading.m_step = 0;
	m_status.m_stlight.m_step = 0;
	m_status.m_sttexture.m_step = 0;
	m_status.m_strender.m_step = 0;

	result = lex->ReadAtom();
	if (result == 4)
	{
		m_onshading(result);
		if (abort) return -1;
		m_onlight(result);
		if (abort) return -1;
		m_ontexture(result);
		if (abort) return -1;
		m_onrender(result);
		if (abort) return -1;
		goto STEP_1;
	}
	else
		return result;
STEP_1:
	result = lex->ReadAtom();
	if (result == 2)
	{
		m_onshading(result);
		if (abort) return -1;
		result=parseshading(2);
		m_onelement(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	if (result == 4)
	{
		m_oncoord(result);
		if (abort) return -1;
		result=parsecoord(1);
		m_onlight(result);
		if (abort) return -1;
		m_onrender(result);
		if (abort) return -1;
		goto STEP_2;
	}
	if (result == 3)
	{
		m_ontexture(result);
		if (abort) return -1;
		result=parsetexture(2);
		m_onelement(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	else
		return result;
STEP_2:
	result = lex->ReadAtom();
	if (result == 6)
	{
		m_onlight(result);
		if (abort) return -1;
		result=parselight(3);
		m_onelement(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	if (result == 5)
	{
		m_onrender(result);
		if (abort) return -1;
		result=parserender(3);
		m_onelement(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	else
		return result;
FINAL_STEP:

	result=RULE_element_ID;
	return result;
}

int CYacc::parseelementname(int step)
{
	int result=0;
	int currentStep=0;
	UNREFERENCED_PARAMETER(step);

	result = lex->ReadAtom();
	if ((result!=ATOM_rendering_ID)&&(result!=ATOM_texture_ID)&&(result!=ATOM_light_ID)&&(result!=ATOM_shading_ID))
		return result;
	m_onelementname(result);
	if (abort) return -1;

	result=RULE_elementname_ID;
	return result;
}

int CYacc::parsedefine(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseelementname(0);
	if (result!=RULE_elementname_ID)
		return result;
	m_ondefine(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_ondefine(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_set_ID)
		return result;
	m_ondefine(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = parseelement(0);
	if (result!=RULE_element_ID)
		return result;
	m_ondefine(result);
	if (abort) return -1;
STEP_4:

	result=RULE_define_ID;
	return result;
}

int CYacc::parseconfig(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseparameter(0);
	if (result!=RULE_parameter_ID)
		return result;
	m_onconfig(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_set_ID)
		return result;
	m_onconfig(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_integer_ID)
		return result;
	m_onconfig(result);
	if (abort) return -1;
STEP_3:

	result=RULE_config_ID;
	return result;
}

int CYacc::parsetriangles(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parsetriangle(0);
	if (result!=RULE_triangle_ID)
		return result;
	m_ontriangles(result);
	if (abort) return -1;
STEP_1:

	result=RULE_triangles_ID;
	return result;
}

int CYacc::parsevertices(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parsevertex(0);
	if (result!=RULE_vertex_ID)
		return result;
	m_onvertices(result);
	if (abort) return -1;
STEP_1:

	result=RULE_vertices_ID;
	return result;
}

int CYacc::parseobjectbase(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_7:
	if (step>7) goto STEP_8;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onobjectbase(result);
	if (abort) return -1;
STEP_8:

	result=RULE_objectbase_ID;
	return result;
}

int CYacc::parsecomposition(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseobjectbase(0);
	if (result!=RULE_objectbase_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_geometry_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	currentStep = m_status.m_stcomposition.m_step;
	while ((result=parsevertices(0))==RULE_vertices_ID)
	{
		m_status.m_stcomposition.m_step = currentStep;
		m_oncomposition(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_mesh_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	currentStep = m_status.m_stcomposition.m_step;
	while ((result=parsetriangles(0))==RULE_triangles_ID)
	{
		m_status.m_stcomposition.m_step = currentStep;
		m_oncomposition(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_7:
	if (step>7) goto STEP_8;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_oncomposition(result);
	if (abort) return -1;
STEP_8:

	result=RULE_composition_ID;
	return result;
}

int CYacc::parsesphere(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseobjectbase(0);
	if (result!=RULE_objectbase_ID)
		return result;
	m_onsphere(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = parsecoord(0);
	if (result!=RULE_coord_ID)
		return result;
	m_onsphere(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_sep_ID)
		return result;
	m_onsphere(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_float_ID)
		return result;
	m_onsphere(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onsphere(result);
	if (abort) return -1;
STEP_5:

	result=RULE_sphere_ID;
	return result;
}

int CYacc::parseextern(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseobjectbase(0);
	if (result!=RULE_objectbase_ID)
		return result;
	m_onextern(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_import_ID)
		return result;
	m_onextern(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_onextern(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onextern(result);
	if (abort) return -1;
STEP_4:

	result=RULE_extern_ID;
	return result;
}

int CYacc::parseobject(int step)
{
	int result=0;
	int currentStep=0;
	UNREFERENCED_PARAMETER(step);

	m_status.m_stcomposition.m_step = 0;
	m_status.m_stsphere.m_step = 0;
	m_status.m_stextern.m_step = 0;

	result = lex->ReadAtom();
	if (result == 3)
	{
		m_onobjectbase(result);
		if (abort) return -1;
		result=parseobjectbase(1);
		m_oncomposition(result);
		if (abort) return -1;
		m_onsphere(result);
		if (abort) return -1;
		m_onextern(result);
		if (abort) return -1;
		goto STEP_1;
	}
	else
		return result;
STEP_1:
	result = lex->ReadAtom();
	if (result == 22)
	{
		m_oncomposition(result);
		if (abort) return -1;
		result=parsecomposition(2);
		m_onobject(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	if (result == 4)
	{
		m_oncoord(result);
		result=parsecoord(1);
		m_onsphere(result);
		if (abort) return -1;
		result=parsesphere(2);
		m_onobject(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	if (result == 24)
	{
		m_onextern(result);
		if (abort) return -1;
		result=parseextern(2);
		m_onobject(result);
		if (abort) return -1;
		goto FINAL_STEP;
	}
	else
		return result;
FINAL_STEP:

	result=RULE_object_ID;
	return result;
}

int CYacc::parseobjects(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = parseobject(0);
	if (result!=RULE_object_ID)
		return result;
	m_onobjects(result);
	if (abort) return -1;
STEP_1:

	result=RULE_objects_ID;
	return result;
}

int CYacc::parsemain(int step)
{
	int result=0;
	int currentStep=0;
	if (step>0) goto STEP_1;
	result = lex->ReadAtom();
	if (result!=ATOM_string_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_1:
	if (step>1) goto STEP_2;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_2:
	if (step>2) goto STEP_3;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_3:
	if (step>3) goto STEP_4;
	currentStep = m_status.m_stmain.m_step;
	while ((result=parseconfig(0))==RULE_config_ID)
	{
		m_status.m_stmain.m_step = currentStep;
		m_onmain(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_4:
	if (step>4) goto STEP_5;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_5:
	if (step>5) goto STEP_6;
	currentStep = m_status.m_stmain.m_step;
	while ((result=parsedefine(0))==RULE_define_ID)
	{
		m_status.m_stmain.m_step = currentStep;
		m_onmain(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_6:
	if (step>6) goto STEP_7;
	result = lex->ReadAtom();
	if (result!=ATOM_in_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_7:
	if (step>7) goto STEP_8;
	currentStep = m_status.m_stmain.m_step;
	while ((result=parseobjects(0))==RULE_objects_ID)
	{
		m_status.m_stmain.m_step = currentStep;
		m_onmain(result);
		if (abort) return -1;
	}
	if (result!=ATOM_out_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_8:
	if (step>8) goto STEP_9;
	result = lex->ReadAtom();
	if (result!=ATOM_out_ID)
		return result;
	m_onmain(result);
	if (abort) return -1;
STEP_9:

	result=RULE_main_ID;
	return result;
}

int CYacc::Analyse(void)
{
	int result=0;
	m_status.Init();

	result = parsemain(0);

	if(result!=RULE_main_ID)
		result = -1;

	return result;
}
