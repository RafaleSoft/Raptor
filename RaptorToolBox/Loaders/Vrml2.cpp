#include "stdafx.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif

#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif

#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif

#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE


static CString indent;
void IncrIndent(void)
{
	indent += "	";
}

void DecrIndent(void)
{
	indent = indent.Left(indent.GetLength() - 1);
}

bool Write(CStdioFile &f,CString &text)
{
	try
	{
		f.WriteString(text);
		return true;
	}
	catch(CFileException&)
	{
		AfxMessageBox("VRML2 output failed to write data");
		return false;
	}
}


void exportVRML(CStdioFile &f,CObject3DInstance *geo)
{
	CString text = "Transform\n{\n	";
	Write(f,text);

	text = "	}\n";
	Write(f,text);
}

void exportVRML(CStdioFile &f,CMaterial *mat)
{
	if (NULL == mat)
	{
		CString text = indent + "material NULL\n";
		Write(f,text);
	}
	else
	{
		CString text = indent + "material Material\n" + indent + "{\n";
		IncrIndent();
		Write(f,text);

		//	Ambient
		CString data;
		GL_RGBA_COLOR color = mat->getAmbient();
		data.Format("ambientIntensity	%f\n",(color.r+color.g+color.b)/3.0f);
		text = indent + data;
		Write(f,text);

		//	Diffuse
		color = mat->getDiffuse();
		data.Format("diffuseColor		%f %f %f\n",color.r,color.g,color.b);
		text = indent + data;
		Write(f,text);

		//	Transparency
		data.Format("transparency		%f\n",1.0f - color.a);
		text = indent + data;
		Write(f,text);

		//	Specular
		color = mat->getSpecular();
		data.Format("specularColor		%f %f %f\n",color.r,color.g,color.b);
		text = indent + data;
		Write(f,text);

		//	Emission
		color = mat->getEmission();
		data.Format("emissiveColor		%f %f %f\n",color.r,color.g,color.b);
		text = indent + data;
		Write(f,text);

		//	Shininess
		color = mat->getShininess();
		data.Format("shininess		%f\n",0.01f * color.r);
		text = indent + data;
		Write(f,text);

		DecrIndent();
		text = indent + "}\n";
		Write(f,text);
	}
}

void exportVRML(CStdioFile &f,CTextureFactory *txt)
{
	if (NULL == txt)
	{
		CString text = indent + "texture NULL\n";
		Write(f,text);
	}
	else
	{
		CString text = indent + "texture Texture\n{\n";
		IncrIndent();
		Write(f,text);

		DecrIndent();
		text = indent + "}\n";
		Write(f,text);
	}
}

void exportVRML(CStdioFile &f,CGeometry *geo)
{
	CString text = indent + "DEF " + CString(geo->getName().data()) + " Shape\n" + indent + "{\n";
	CString data;
	IncrIndent();
	Write(f,text);

	//	Material and texture properties
	text = indent + "appearance Appearance\n" + indent + "{\n";
	IncrIndent();
	Write(f,text);
/*	if (geo->GetShader() != NULL)
	{
		if (geo->GetShader()->HasMaterial())
			exportVRML(f,geo->GetShader()->GetMaterial());
		else
			exportVRML(f,(CMaterial*)NULL);
	}*/
	exportVRML(f,(CTextureFactory*)NULL);// for the moment not terminated because need to explore TMUSteup geo->GetTexture());
	DecrIndent();
	text = indent + "}\n";
	Write(f,text);

	//	Geometry properties
	text = indent + "geometry IndexedFaceSet\n" + indent + "{\n";
	IncrIndent();
	Write(f,text);
	data.Format("ccw		TRUE\n");
	text = indent + data;
	Write(f,text);
	data.Format("solid		TRUE\n");
	text = indent + data;
	Write(f,text);
	data.Format("normalPerVertex	TRUE\n");
	text = indent + data;
	Write(f,text);

	//	Vertices
	text = indent + "coord Coordinate\n" + indent + "{ point [\n";
	IncrIndent();
	Write(f,text);
	text = "";
	GL_VERTEX_DATA v;
    unsigned int i=0;
	for(i=0;i<geo->nbVertex();i++)
	{
		geo->getVertex(i,v);
		data.Format("%f %f %f, ",v.vertex.x,v.vertex.y,v.vertex.z);
		text += data;

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "] }\n";
	Write(f,text);
	
	//	Normals
	text = indent + "normal Normal\n" + indent + "{ vector [\n";
	IncrIndent();
	Write(f,text);
	text = "";
	GL_COORD_VERTEX n;
	for(i=0;i<geo->nbVertex();i++)
	{
		geo->getNormal(i,n);
		data.Format("%f %f %f, ",n.x,n.y,n.z);
		text += data;

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "] }\n";
	Write(f,text);

	//	Texture Coordinate
	text = indent + "texCoord TextureCoordinate\n" + indent + "{ point [\n";
	IncrIndent();
	Write(f,text);
	text = "";
	for(i=0;i<geo->nbVertex();i++)
	{
		geo->getVertex(i,v);
		data.Format("%f %f, ",v.texCoord0.u,v.texCoord0.v);
		text += data;

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "] }\n";
	Write(f,text);

	// Pour l'instant, pas de couleur par vertex exportée
	//	Color
	/*
	GL_RGBA_COLOR color = geo->GetColor();
	data.Format("color Color { color [ %f %f %f ] }\n",color.r,color.g,color.b);
	text = indent + data;
	Write(f,text);
	*/
	/*
	text = indent + "colorPerVertex FALSE\n";
	Write(f,text);
	text = indent + "colorIndex\n" + indent + "[\n";
	IncrIndent();
	Write(f,text);
	text = "";
	for(i=0;i<geo->NbFace();i++)
	{
		text += "0, ";

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "]\n";
	Write(f,text);
	*/

	// Les polygones
	text = indent + "coordIndex\n" + indent + "[\n";
	IncrIndent();
	Write(f,text);
	text = "";
	for(i=0;i<geo->nbFace();i++)
	{
		unsigned int p1,p2,p3;
		geo->getFace(i,p1,p2,p3);
		data.Format("%d, %d, %d, -1, ",p1,p2,p3);
		text += data;

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "]\n";
	Write(f,text);

	text = indent + "normalIndex\n" + indent + "[\n";
	IncrIndent();
	Write(f,text);
	text = "";
	for(i=0;i<geo->nbFace();i++)
	{
		unsigned int p1,p2,p3;
		geo->getFace(i,p1,p2,p3);
		data.Format("%d, %d, %d, -1, ",p1,p2,p3);
		text += data;

		if ((i%4) == 3)
		{
			text = indent + text + "\n";
			Write(f,text);
			text = "";
		}
	}
	if (!text.IsEmpty())
	{
		text = indent + text + "\n";
		Write(f,text);
	}
	DecrIndent();
	text = indent + "]\n";
	Write(f,text);


	DecrIndent();
	text = indent + "}\n";
	Write(f,text);

	DecrIndent();
	text = indent + "}\n";
	Write(f,text);
}

void exportVRML(CStdioFile &f,C3DSet *set)
{
	CString text = indent + "Group\n" + indent + "{\n";
	IncrIndent();	
	text += indent + "children\n" + indent + "[\n";
	IncrIndent();
	Write(f,text);

    C3DSet::C3DSetIterator it = set->getIterator();
	CObject3D *obj = set->getChild(it++);
	while (obj != NULL)
	{
		switch(obj->getId())
		{
			case CPersistence::ID_CLASS_3DSET:
			{
				exportVRML(f,(C3DSet*)obj);
				break;
			}
			case CPersistence::ID_CLASS_GEOMETRY:
			{
				exportVRML(f,(CGeometry*)obj);
				break;
			}
			case CPersistence::ID_CLASS_OBJECT3DINSTANCE:
			{
				exportVRML(f,(CObject3DInstance*)obj);
				break;
			}
		}

		obj = set->getChild(it++);
	}

	DecrIndent();
	text = indent + "]\n";
	DecrIndent();
	text += "}\n";
	Write(f,text);
}

bool exportVRML(string filename,C3DSet *scene)
{
	bool res = true;

	if (scene == NULL)
	{
		AfxMessageBox("VRML2 output failed to read NULL scene");
	}

	try
	{
		CStdioFile vrml(filename.data(),CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);

		CString header;
	
		header = "#VRML V2.0 utf8\n\nWorldInfo\n{\n	title \"";
		header += scene->getName().data();
		header += "\"\n	info [ \"Raptor VRML2 generated scene\" ]\n}\n";
		Write(vrml,header);
	
		indent = "";
		exportVRML(vrml,scene);

		vrml.Close();
	}
	catch(CFileException&)
	{
		AfxMessageBox("VRML2 output failed to generate file");
	}

	return res;
}
