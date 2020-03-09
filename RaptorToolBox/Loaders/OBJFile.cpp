#include "Subsys/CodeGeneration.h"


#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "GLHierarchy/ShadedGeometry.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GLHierarchy/GeometryEditor.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
	#include "RaptorToolBox.h"
#endif

#include "OBJScript_lex.h"
#include "OBJScript_yacc.h"
#include "OBJScript_tree.h"
#include <strstream>
#include <fstream>

using namespace std;

RAPTOR_NAMESPACE


class OBJLex : public CLex
{
public:
	OBJLex() {};
	virtual ~OBJLex() {};

	virtual void OnError(CLex::LexError err)
	{	};
};

class OBJYacc : public CYacc
{
public:
	OBJYacc(OBJLex *parser):CYacc(parser) 
	{
		m_pCurrentGeometry = NULL;
		m_pDefaultShader = NULL;
	};

	virtual ~OBJYacc() {};

	virtual void OnError(CYacc::YaccError err) { };

	void buildGeometry();
	const vector<CGeometry*>& getGeometries(void) const { return geometries; };


private:
	virtual int	MapTypeinteger(string toMap) const			{ return atoi(toMap.data()); };
	virtual float	MapTypefloat(string toMap) const		{ return (float)(atof(toMap.data())); };
	virtual float	MapTypeexponent(string toMap) const		{ return (float)(atof(toMap.data())); };
	virtual string	MapTypestring(string toMap) const		{ return (toMap.substr(1,toMap.size()-2)); };
	virtual string	MapTypeidentifier(string toMap) const	{ return toMap; }

	virtual void Onobjectnamevalue(unsigned int RULE_ID);
	virtual void Onmaterial(unsigned int);
	virtual void Onobjdata(unsigned int);
	virtual void Onmain(unsigned int);

	vector<CGeometry*>	geometries;
	vector<GL_COORD_VERTEX>	vertices;
	vector<unsigned short>	quadFaces;
	vector<unsigned short>	triFaces;
	vector<vector<unsigned short> >	polyFaces;

	string	objectName;

	CShadedGeometry		*m_pCurrentGeometry;
	CShader				*m_pDefaultShader;
};

void OBJYacc::Onmain(unsigned int )
{
	main_t& st = status->m_stmain;
	if (st.st_objdatas0.size() > 0)
	{
		objdatas_t& datas = st.st_objdatas0.back();
		st.st_objdatas0.pop_back();
		delete (datas.st_objdata0.p_stobjdata);
	}
}

void OBJYacc::Onobjectnamevalue(unsigned int RULE_ID) 
{
	if (RULE_ID == ATOM_integer_ID)
	{
		strstream str;
		str << status->m_stobjectnamevalue.objectnamevalue_value.integer_1 << ends;
		objectName = str.str();
	}
	else
		objectName = status->m_stobjectnamevalue.objectnamevalue_value.identifier_0;
}

void OBJYacc::Onobjdata(unsigned int RULE_ID)
{
	switch(RULE_ID)
	{
		case RULE_face_ID:
		{
			face_t& face = status->m_stface;
			switch(face.st_faceelts1.size())
			{
				case 0:
				case 1:
				case 2:
					// TODO raise error
					break;
				case 3:
				{
					for (unsigned int i=0;i<3;i++)
					{
						const faceelt_t& faceelt = face.st_faceelts1[i].st_faceelt0;
						if (faceelt.st_faceeltID == RULE_facev_ID)
						{
							facev_t *facev = (facev_t*)faceelt.p_stfaceelt;
							triFaces.push_back(facev->st_integer0-1);
						}
						else if (faceelt.st_faceeltID == RULE_facevvtvn_ID)
						{
							facevvtvn_t *facevvtvn = (facevvtvn_t*)faceelt.p_stfaceelt;
							triFaces.push_back(facevvtvn->st_integer0-1);
							// vertex normals & tex coords require automatic smooth group generation !!!
						}
					}
					break;
				}
				case 4:
				{
					for (unsigned int i=0;i<4;i++)
					{
						const faceelt_t& faceelt = face.st_faceelts1[i].st_faceelt0;
						if (faceelt.st_faceeltID == RULE_facev_ID)
						{
							facev_t *facev = (facev_t*)faceelt.p_stfaceelt;
							quadFaces.push_back(facev->st_integer0-1);
						}
						else if (faceelt.st_faceeltID == RULE_facevvtvn_ID)
						{
							facevvtvn_t *facevvtvn = (facevvtvn_t*)faceelt.p_stfaceelt;
							quadFaces.push_back(facevvtvn->st_integer0-1);
							// vertex normals & tex coords require automatic smooth group generation !!!
						}
					}
					break;
				}
				default:
				{
					vector<unsigned short> polys;
					for (unsigned int i=0;i<face.st_faceelts1.size();i++)
					{
						const faceelt_t& faceelt = face.st_faceelts1[i].st_faceelt0;
						if (faceelt.st_faceeltID == RULE_facev_ID)
						{
							facev_t *facev = (facev_t*)faceelt.p_stfaceelt;
							polys.push_back(facev->st_integer0-1);
						}
						else if (faceelt.st_faceeltID == RULE_facevvtvn_ID)
						{
							facevvtvn_t *facevvtvn = (facevvtvn_t*)faceelt.p_stfaceelt;
							polys.push_back(facevvtvn->st_integer0-1);
							// vertex normals & tex coords require automatic smooth group generation !!!
						}
					}
					polyFaces.push_back(polys);
					break;
				}
			}
			for (unsigned int i=0;i<face.st_faceelts1.size();i++)
			{
				const faceelt_t& faceelt = face.st_faceelts1[i].st_faceelt0;
				delete(faceelt.p_stfaceelt);
			}
			face.st_faceelts1.clear();
			break;
		}
		case RULE_vertex_ID:
		{
			vertex_t &v = status->m_stvertex;
			float x = 0;
			float y = 0;
			float z = 0;
			if (v.st_real1.st_realID == RULE_singleflt_ID)
				x = ((singleflt_t*)(v.st_real1.p_streal))->st_float0;
			else
			{
				expflt_t *e = ((expflt_t*)(v.st_real1.p_streal));
				x = e->st_float0 * pow(10.0f,e->st_integer2);
			}

			if (v.st_real2.st_realID == RULE_singleflt_ID)
				y = ((singleflt_t*)(v.st_real2.p_streal))->st_float0;
			else
			{
				expflt_t *e = ((expflt_t*)(v.st_real2.p_streal));
				y = e->st_float0 * pow(10.0f,e->st_integer2);
			}

			if (v.st_real3.st_realID == RULE_singleflt_ID)
				z = ((singleflt_t*)(v.st_real3.p_streal))->st_float0;
			else
			{
				expflt_t *e = ((expflt_t*)(v.st_real3.p_streal));
				z = e->st_float0 * pow(10.0f,e->st_integer2);
			}

			vertices.push_back(GL_COORD_VERTEX(	x,y,z ));
			break;
		}
	}
}


void OBJYacc::Onmaterial(unsigned int) 
{
	if (status->m_stmaterial.m_step == 0)
	{
		// Flush current geometry to start a new one.
		if (m_pCurrentGeometry != NULL)
			buildGeometry();
		
		m_pCurrentGeometry = new CShadedGeometry("default");

		string matname = "default_mat";
		materialvalue_t matvalue = status->m_stmaterial.st_materialvalue1;
		if (matvalue.st_materialvalueID == ATOM_integer_ID)
		{
			strstream str;
			str << matvalue.materialvalue_value.integer_1 << ends;
			matname = str.str();
		}
		else
			matname = matvalue.materialvalue_value.identifier_0;

		CPersistence *mat = CPersistence::FindObject(matname);
		if ((mat != NULL) && (mat->getId().isSubClassOf(CShader::CShaderClassID::GetClassId())))
			m_pCurrentGeometry->setShader((CShader*)mat);
		else
		{
			CShader *pShader = m_pCurrentGeometry->getShader();
			pShader->setName(matname);

			CMaterial *pMaterial = pShader->getMaterial();
			pMaterial->setAmbient(0.1f,0.1f,0.1f,1.0f);
			pMaterial->setDiffuse(0.6f,0.6f,0.6f,1.0f);
			pMaterial->setSpecular(1.0f,1.0f,1.0f,1.0f);
			pMaterial->setShininess(20.0f);

			if (matname == "Fuselage")
			{
				CTextureFactory &factory = CTextureFactory::getDefaultFactory();
				CTextureObject *diffuse = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
				factory.glLoadTexture(diffuse,"RafaleM2.jpg");

				CTextureUnitSetup *const pSetup = pShader->glGetTextureUnitsSetup();
				pSetup->setDiffuseMap(diffuse);
				diffuse->addReference();
			}
		}
	}
}

void OBJYacc::buildGeometry()
{
	if (m_pCurrentGeometry == NULL)
		m_pCurrentGeometry = new CShadedGeometry("default");

	geometries.push_back(m_pCurrentGeometry);

	unsigned int numVertex = 0;
	map<unsigned int,unsigned int> vertexRemap;
	vector<unsigned int> vertexOrder;

	if (quadFaces.size() > 0)
	{
		for (unsigned int i=0;i<quadFaces.size();i++)
		{
			unsigned int idx = quadFaces[i];
			map<unsigned int,unsigned int>::const_iterator it = vertexRemap.find(idx);

			if (it == vertexRemap.end())
			{
				vertexOrder.push_back(idx);
				vertexRemap[idx] = numVertex;
				quadFaces[i] = numVertex++;
			}
			else
				quadFaces[i] = (*it).second;
		}

		CGeometryPrimitive	*pQuadPrimitives = 
			m_pCurrentGeometry->createPrimitive(CGeometryPrimitive::QUAD);
		pQuadPrimitives->setIndexes(quadFaces);

		quadFaces.clear();
	}

	if (triFaces.size() > 0)
	{
		for (unsigned int i=0;i<triFaces.size();i++)
		{
			unsigned int idx = triFaces[i];
			map<unsigned int,unsigned int>::const_iterator it = vertexRemap.find(idx);

			if (it == vertexRemap.end())
			{
				vertexOrder.push_back(idx);
				vertexRemap[idx] = numVertex;
				triFaces[i] = numVertex++;
			}
			else
				triFaces[i] = (*it).second;
		}

		CGeometryPrimitive	*pTrianglePrimitives = 
			m_pCurrentGeometry->createPrimitive(CGeometryPrimitive::TRIANGLE);
		pTrianglePrimitives->setIndexes(triFaces);

		triFaces.clear();
	}

	if (polyFaces.size() > 0)
	{
		CGeometryPrimitive *pPolyPrimitives = 
			m_pCurrentGeometry->createPrimitive(CGeometryPrimitive::POLYGON);
		vector<unsigned short> polys;
		vector<unsigned short> faces;
		for (unsigned int i=0;i<polyFaces.size();i++)
		{
			polys.push_back(polyFaces[i].size());
			for (unsigned int j=0;j<polyFaces[i].size();j++)
			{
				unsigned int idx = polyFaces[i][j];
				map<unsigned int,unsigned int>::const_iterator it = vertexRemap.find(idx);

				if (it == vertexRemap.end())
				{
					vertexOrder.push_back(idx);
					vertexRemap[idx] = numVertex;
					faces.push_back(numVertex++);
				}
				else
					faces.push_back((*it).second);
			}
		}
		pPolyPrimitives->setIndexes(polys,faces);

		polyFaces.clear();
	}


	m_pCurrentGeometry->glSetVertices(numVertex);
	m_pCurrentGeometry->glLockData();
	for (unsigned int i=0;i<vertexOrder.size();i++)
	{
		const GL_COORD_VERTEX &vtx = vertices[vertexOrder[i]];
		m_pCurrentGeometry->addVertex(vtx.x,vtx.y,vtx.z,1.0f);	
	}
	const CGeometryEditor &pEditor = m_pCurrentGeometry->getEditor();
	pEditor.genNormals(true);
	m_pCurrentGeometry->glUnLockData();

	m_pCurrentGeometry->glSetTexCoords(numVertex);
	const CGeometryEditor &rEditor = m_pCurrentGeometry->getEditor();
	//rEditor.genTexCoords(CGeometryEditor::XY_PLANE,0.55f,-0.5f,1.0f,0.0f);
	rEditor.genTexCoords(CGeometryEditor::XY_PLANE,0.014f,0.4915f,0.016f,0.395f);

	m_pCurrentGeometry->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
	m_pCurrentGeometry->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	m_pCurrentGeometry->addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading a OBJ file
//
bool _glLoadWavefront(const string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options)
{
	OBJLex objlex;
	if (objlex.InitLex(fname))
	{
		OBJYacc objyacc(&objlex);

		if (objyacc.Analyse() == RULE_main_ID)
		{
			set = new C3DSet(fname);

			// build the current (last) geometry
			objyacc.buildGeometry();

			const vector<CGeometry*>& geometries = objyacc.getGeometries();
			vector<CGeometry*>::const_iterator it = geometries.begin();
			
			C3DSet::C3DSetIterator  pos = set->getIterator();
			while (it != geometries.end())
			{
				set->addChild((*it++),pos);
			}

			return objlex.FinishLex();
		}
		else
		{
			objlex.FinishLex();
			return false;
		}
	}
	else
		return false;
}


/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for saving a OBJ file
//
bool _glSaveWavefront(const string &fname,C3DSet *scene)
{
	if (scene == NULL)
		return false;

	string fname2 = fname;
	string fname1 = fname;
	size_t pos = fname.rfind(".obj");
	if ( pos != string::npos )
		fname2 = fname.substr(0,pos) + ".mtl";
	else
	{
		size_t pos2 = fname.rfind(".OBJ");
		if ( pos2 != string::npos )
			fname2 = fname.substr(0,pos2) + ".mtl";
		else
		{
			fname1 += ".obj";
			fname2 += ".mtl";
		}
	}

	ofstream objfile;
	objfile.open(fname1.data(),ios_base::out);
	if (!objfile.is_open() || !objfile.good())
		return false;

	ofstream mtlfile;
	mtlfile.open(fname2.data(),ios_base::out);
	if (!mtlfile.is_open() || !mtlfile.good())
		return false;

	objfile.precision(9);

	objfile << "#####" << endl;
	objfile << "#	OBJ file generated with Raptor OpenGL SDK" << endl;
	objfile << "#####" << endl;
	objfile << endl;

	objfile << endl << "mtllib " << fname2 << endl;

	unsigned int vertexCounter = 1;
	MapStringToPtr exportedMaterials;

	CObject3D *pObject = NULL;
	C3DSet::C3DSetIterator  scenePos = scene->getIterator();
	while (NULL != (pObject = scene->getChild(scenePos++)))
	{
		CShadedGeometry *g = (CShadedGeometry*)pObject;
		objfile << endl;
		objfile << "#	object" << endl;
		objfile << "o " << g->getName() << endl << endl;

		CShader *pShader = g->getShader();
		if (pShader != NULL)
		{
			objfile << "usemtl " << pShader->getName() << endl << endl;

			if (exportedMaterials.end() == exportedMaterials.find(pShader->getName()))
			{
				mtlfile << "newmtl " << pShader->getName() << endl;
				if (pShader->hasMaterial())
				{
					CMaterial *m = pShader->getMaterial();
					CColor::RGBA c = m->getAmbient();
					mtlfile << "Ka " << c.r << " " << c.g << " " << c.b << endl;
					c = m->getDiffuse();
					mtlfile << "Kd " << c.r << " " << c.g << " " << c.b << endl;
					c = m->getSpecular();
					mtlfile << "Ks " << c.r << " " << c.g << " " << c.b << endl;
					c = m->getShininess();
					mtlfile << "Ns " << c.r << endl;
				}

				if (pShader->hasTextureUnitSetup())
				{
					CTextureUnitSetup *t = pShader->glGetTextureUnitsSetup();
					mtlfile << "map_Kd " << t->getDiffuseMap()->getName() << endl;
				}

				exportedMaterials[pShader->getName()] = pShader;
			}
		}

		g->glLockData();

		unsigned int nbVertex = g->nbVertex();
		objfile << "#	" << nbVertex << " vertices" << endl;
		
		for (uint32_t i=0;i < nbVertex;i++)
		{
			GL_VERTEX_DATA v;
			g->getVertex(i,v);

			objfile << "v " << v.vertex.x << " " << v.vertex.y << " " << v.vertex.z << endl;
			objfile << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << endl;
			objfile << "vt " << v.texCoord0.u << " " << v.texCoord0.v << endl;
		}

		objfile << endl;
		objfile << "#	elements " << endl;
		objfile << endl;

		unsigned int nbFace = g->nbFace();
		for (uint32_t i=0; i < nbFace; i++)
		{
		}

		g->glUnLockData();

		vector<CGeometryPrimitive*> primitives = g->getPrimitives();
		for (size_t i=0;i<primitives.size();i++)
		{
			CGeometryPrimitive *p = primitives[i];

			switch(p->getKind())
			{
				case CGeometryPrimitive::TRIANGLE:
				{
					unsigned short *faces = new unsigned short[p->getSize()];
					p->getIndexes(faces);

					for (unsigned int j=0;j<p->getSize();j+=3)
					{
						objfile << "f ";
						objfile << vertexCounter+faces[j] << "/" << vertexCounter+faces[j] << "/" << vertexCounter+faces[j] << " ";
						objfile << vertexCounter+faces[j+1] << "/" << vertexCounter+faces[j+1] << "/" << vertexCounter+faces[j+1] << " ";
						objfile << vertexCounter+faces[j+2] << "/" << vertexCounter+faces[j+2] << "/" << vertexCounter+faces[j+2] << endl;
					}
					break;
				}
				case CGeometryPrimitive::QUAD:
				{
					unsigned short *faces = new unsigned short[p->getSize()];
					p->getIndexes(faces);

					for (unsigned int j=0;j<p->getSize();j+=4)
					{
						objfile << "f ";
						objfile << vertexCounter+faces[j] << "/" << vertexCounter+faces[j] << "/" << vertexCounter+faces[j] << " ";
						objfile << vertexCounter+faces[j+1] << "/" << vertexCounter+faces[j+1] << "/" << vertexCounter+faces[j+1] << " ";
						objfile << vertexCounter+faces[j+2] << "/" << vertexCounter+faces[j+2] << "/" << vertexCounter+faces[j+2] << " ";
						objfile << vertexCounter+faces[j+3] << "/" << vertexCounter+faces[j+3] << "/" << vertexCounter+faces[j+3] << endl;
					}
					break;
				}
				case CGeometryPrimitive::POLYGON:
				{
					vector<unsigned short> polygonSizes;
					vector<unsigned short> polygonFaces;
					p->getIndexes(polygonSizes,polygonFaces);

					unsigned short offset = 0;
					for (unsigned int j=0;j<polygonSizes.size();j++)
					{
						objfile << "f ";

						unsigned short size = polygonSizes[j];
						for (unsigned short k=0;k<size;k++)
						{
							unsigned short idx = vertexCounter+polygonFaces[offset++];
							objfile << idx << "/" << idx << "/" << idx << " ";
						}

						objfile << endl;
					}

					break;
				}
			}
		}

		vertexCounter += nbVertex;
	}

	objfile.close();
	mtlfile.close();

	return true;
}

