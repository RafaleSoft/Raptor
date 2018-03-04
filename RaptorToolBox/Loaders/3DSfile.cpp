#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif

#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GLHierarchy/GeometryEditor.h"
#endif

#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "GLHierarchy/ShadedGeometry.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif

#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
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

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
    #include "System/RaptorIO.h"
#endif

#if !defined(AFX_3DSFILE_H__4ACF0CDF_B92E_11D3_85A1_CA8882000000__INCLUDED_)
    #include "3DSfile.h"
#endif

RAPTOR_NAMESPACE

//#define _DEBUG
/////////////////////////////////////////////////////////////////////////////
//
//	Main datas for 3DSFile parsing
//
CRaptorToolBox::SCENE_LOADER_OPTIONS		maxOptions;

// the one and only one file object
CRaptorIO	*C3DSFile = NULL;


Chunk			CurrentChunk;		// last and current datas read
									//		to link between recursive calls
State			CurrentState = {0,"",0.0f,0,0,0.0f,0,0,0,0.0f,0.0f,0.0f,0,0.0f,0,
								{{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
								false};			

static C3DSet		*root = NULL;
static C3DSet		*currentSet = NULL;
CTextureSet			*maps = NULL;
MapStringToPtr		*matlib = NULL;
static Material		*mat = NULL;
static CGeometry	*geo = NULL;


bool				materialRenamed = false;
vector<MaterialAlias> materialRenames;
vector<std::string> discardGeometryHints;

typedef struct DATA_t
{
	float x;
	float y;
	float z;
	float u;
	float v;
	unsigned int idx;
	unsigned int sg;
} DATA;
vector<DATA>	currentData;
vector<DATA>	duplicateData;

typedef struct FACE_t
{
	unsigned short p1;
	unsigned short p2;
	unsigned short p3;
	unsigned short flags;
} FACE;
vector<FACE>	currentFace;


#pragma warning (disable : 4711)	//	hide warning 4711 :
									//	functinon selected for automatic inline expansion
int ReadSubChunk();
bool RAPTOR_FASTCALL ProcessChunk0xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk1xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk2xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk3xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk4xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk5xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk6xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk7xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk8xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunk9xxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkAxxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkBxxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkCxxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkDxxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkExxxH(long length);
bool RAPTOR_FASTCALL ProcessChunkFxxxH(long length);
typedef bool (RAPTOR_FASTCALL  *PFN)(long length);
PFN Procs[16] = 
{
	ProcessChunk0xxxH,ProcessChunk1xxxH,ProcessChunk2xxxH,
	ProcessChunk3xxxH,ProcessChunk4xxxH,ProcessChunk5xxxH,
	ProcessChunk6xxxH,ProcessChunk7xxxH,ProcessChunk8xxxH,
	ProcessChunk9xxxH,ProcessChunkAxxxH,ProcessChunkBxxxH,
	ProcessChunkCxxxH,ProcessChunkDxxxH,ProcessChunkExxxH,
	ProcessChunkFxxxH
};




void _PurgeObjects(void)
{
	if (maps != NULL)
	{
		delete maps;
		maps = NULL;
	}
	if (matlib != NULL)
	{
        MapStringToPtr::iterator pos = matlib->begin();
		Material *m;
		while (pos != matlib->end())
		{
            m = (Material*)((*pos++).second);
			delete m->mat;
			delete m;
		}
		matlib->clear();
		delete matlib;
		matlib = NULL;
	}
	if (root != NULL)
	{
		delete root;
		root = NULL;
	}
}


void ApplyMatrixToObject(CGeometry *g)
{
	float xx = CurrentState.matrix[0][0];
	float yx = CurrentState.matrix[0][1];
	float zx = CurrentState.matrix[0][2];
	
	float xy = CurrentState.matrix[1][0];
	float yy = CurrentState.matrix[1][1];
	float zy = CurrentState.matrix[1][2];

	float xz = CurrentState.matrix[2][0];
	float yz = CurrentState.matrix[2][1];
	float zz = CurrentState.matrix[2][2];

	float tx = CurrentState.matrix[3][0];
	float ty = CurrentState.matrix[3][1];
	float tz = CurrentState.matrix[3][2];

    g->glLockData();
	for (unsigned int i=0;i<g->nbVertex();i++)
	{
        GL_VERTEX_DATA vrtx;
        g->getVertex(i,vrtx);

        GL_COORD_VERTEX &v = vrtx.vertex;

		//	3DSMax really has a strange behaviour
		float x = v.x * xx + v.y * xy + v.z * xz - tx;
		float y = v.x * yx + v.y * yy + v.z * yz - ty;
		float z = v.x * zx + v.y * zy + v.z * zz - tz;

        g->setCoord(i,x,y,z,1.0f);
	}
    g->glUnLockData();
}

void FlushGeometry(CGeometry *g)
{
	bool discard = false;
    for (unsigned int i=0;i<discardGeometryHints.size() && !discard;i++)
		discard = (discardGeometryHints[i] == g->getName());

	if (!discard)
	{
		g->glSetVertices(currentData.size());
		g->glSetTexCoords(currentData.size());
		g->glSetPolygons(currentFace.size());
		g->glLockData();

		unsigned int numT = 0;
		vector<DATA>::const_iterator itv = currentData.begin();
		while (itv != currentData.end())
		{
			const DATA &v = *itv++;
			g->addVertex(v.x,v.y,v.z,1.0);
			g->setTexCoord(numT++,v.u,v.v);
		}


		vector<FACE>::const_iterator itf = currentFace.begin();
		while (itf != currentFace.end())
		{
			const FACE& f = *itf++;
			g->addFace(f.p1,f.p2,f.p3);
		}

		const CGeometryEditor &pEditor = g->getEditor();
		pEditor.genNormals();

		g->glUnLockData();

		//	Add geometry now, so that the currentSet set can update its bounding box
		C3DSet::C3DSetIterator it = currentSet->getIterator();
		currentSet->addChild(g,it);
	}

	currentData.clear();
	currentFace.clear();
	duplicateData.clear();
}

/////////////////////////////////////////////////////////////////////////////
//
//	3DSFile file functions
//
bool ReadChunkHeader()
{
#ifdef _DEBUG
	if (C3DSFile != NULL)
	{
#endif
		C3DSFile->read( &CurrentChunk.chunk_id,2);
		if (C3DSFile->getStatus() == CRaptorIO::IO_FAILED)
			return false;

		C3DSFile->read( &CurrentChunk.chunk_len,4);
		if (C3DSFile->getStatus() == CRaptorIO::IO_FAILED)
			return false;

		return true;
#ifdef _DEBUG
	}
	return false;
#endif
}

bool Open3DSFile(const std::string& filename)
{
	C3DSFile = CRaptorIO::Create(filename.data(),CRaptorIO::DISK_READ);

	if (C3DSFile->getStatus() != CRaptorIO::IO_OK)
	{
#ifdef _DEBUG
		cout << "File " << filename.data() << " could not be opened !" << endl;
#endif
		return false;
	}

	ReadChunkHeader();
	if (CurrentChunk.chunk_id != M3DMAGIC)
	{
#ifdef _DEBUG
		cout << "File not a 3DS file" << endl;
#endif
		return false;	// not a .3DS file
	}

	if (root==NULL)
		root = new C3DSet("GL ROOT SCENE");
	if (maps==NULL)
		maps = new CTextureSet("3DS_TEXTURE_SET");
	if (matlib==NULL)
		matlib = new MapStringToPtr;
	return true;
}

bool Close3DSFile()
{
	delete C3DSFile;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//	3DSFile parsing functions
//


//	Compile this file with debug option for testing



/////////////////////////////////////////////////////////////////////////////
// 
//	Basic 3DS types : colors, percentage, chunk type
//
bool RAPTOR_FASTCALL ProcessChunk0xxxH(long length)
{
	switch(CurrentChunk.chunk_id)
	{
	case M3D_VERSION:
		C3DSFile->read( &CurrentState.version,length);
#ifdef _DEBUG
			cout << "Magic number : version " << CurrentState.version << endl;
#endif
		return true;
		break;
	case COLOR_24:
	case LIN_COLOR_24:
		{
			// 24 bits R,G,B color
			long colors;
			C3DSFile->read( &colors,length);
			CurrentState.cred = (unsigned char)(colors&0xff);
			CurrentState.cgreen = (unsigned char)((colors>>8)&0xff);
			CurrentState.cblue = (unsigned char)((colors>>16)&0xff);
			CurrentState.fred = (float)CurrentState.cred/255.0f;
			CurrentState.fgreen = (float)CurrentState.cgreen/255.0f;
			CurrentState.fblue = (float)CurrentState.cblue/255.0f;
#ifdef _DEBUG
			cout << "Color r:" << (short)CurrentState.cred << " g:" << (short)CurrentState.cgreen << " b:" << (short)CurrentState.cblue << endl;
#endif
			return true;
		}
		break;
	case INT_PERCENTAGE:
		C3DSFile->read( &CurrentState.spercentage,length);
#ifdef _DEBUG
			cout << "Int Percentage :" << CurrentState.spercentage << endl;
#endif
		return true;
		break;
	case FLOAT_PERCENTAGE:
		C3DSFile->read( &CurrentState.fpercentage,length);
#ifdef _DEBUG
			cout << "Float Percentage : " << CurrentState.fpercentage << endl;
#endif
		return true;
		break;
	case MASTER_SCALE:
		C3DSFile->read( &CurrentState.scale,length);
#ifdef _DEBUG
			cout << "Scale :" << CurrentState.scale << endl;
#endif
		return true;
		break;
	default:
		C3DSFile->seek( length );
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////



bool RAPTOR_FASTCALL ProcessChunk1xxxH(long length)
{
#ifdef _DEBUG
	cout << "Chunk 1xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}

bool RAPTOR_FASTCALL ProcessChunk2xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 2xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// 
//	Cameras, viewing and magic numbers
//
bool RAPTOR_FASTCALL ProcessChunk3xxxH(long length)
{
	if (CurrentChunk.chunk_id == MESH_VERSION)
	{
		C3DSFile->read( &CurrentState.version,length);
#ifdef _DEBUG
			cout << "Mesh version : " << CurrentState.version << endl;
#endif
		return true;
	}
	else if (CurrentChunk.chunk_id == MDATA)
	{
		int l=0;
		while (l<length)
			// mesh magic number
			// mesh version
			// read datas in chunck
				l+=ReadSubChunk();
		return true;
	}
	else
	{
		C3DSFile->seek( length );
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// 
//	Objects datas : vertex, polygons, ...
//
bool RAPTOR_FASTCALL ProcessChunk4xxxH(long length)
{
	switch(CurrentChunk.chunk_id)
	{
		case NAMED_OBJECT:
		{
			int pos = 0;
			char c;
			// object name
			do
			{
				C3DSFile->read( &c,1);
				CurrentState.name[pos++]=c;
			}
			while (c!=0);
#ifdef _DEBUG
			cout << "Object Name : " << CurrentState.name << endl;
#endif
            string hintName = CurrentState.name;
			if (geo != NULL)
				FlushGeometry(geo);
            geo = new CShadedGeometry(CurrentState.name);
			geo->setRenderingModel(CGeometry::CRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY));
           
			// read object datas:
			int l=pos;
			while (l<length)
				l+=ReadSubChunk();
			return true;
			break;
		}
		case N_TRI_OBJECT:
		{
#ifdef _DEBUG
			cout << "Named triangular object" << endl;
#endif
			int l=0;
			while (l<length)
				l+=ReadSubChunk();
			return true;
			break;
		}
		case POINT_ARRAY:
		{
			unsigned short npoints;
			C3DSFile->read( &npoints,2);
#ifdef _DEBUG
			cout << "Points : " << npoints << endl;
#endif
			for (int i=0;i<npoints;i++)
			{
				DATA v;
				v.sg = 0;
				v.idx = i;
				v.u = 0.0f;
				v.v = 0.0f;

				C3DSFile->read( &v.x,4);
				C3DSFile->read( &v.y,4);
				C3DSFile->read( &v.z,4);
				currentData.push_back(v);
			}

			return true;
			break;
		}
		case TEX_VERTS:
		{
			unsigned short nverts;
			C3DSFile->read( &nverts,2);
#ifdef _DEBUG
			cout << "Texture vertex : " << nverts << endl;
#endif
			for (int i=0;i<nverts;i++)
			{
				DATA &v = currentData[i];
				C3DSFile->read( &v.u,4);
				C3DSFile->read( &v.v,4);
			}

			return true;
			break;
		}
/*	case POINT_FLAG_ARRAY:
		{
			ReadSubChunk();
			return true;
		}
		break;*/
		case MESH_MATRIX:
		{
#ifdef _DEBUG
			cout << "Object matrix :" << endl;
#endif
			//	global positionning of the object :
			//	rotation and translation given in the matrix
			//	with h coordinate assumed to be [0 0 0 1]
			C3DSFile->read( CurrentState.matrix,4*3*4);

			//	apply the current matrix to the vertices of the 
			//	current object geo
			if (maxOptions.useMaxTransform)
				ApplyMatrixToObject(geo);

#ifdef _DEBUG
			cout << CurrentState.matrix[0][0] << " - " << CurrentState.matrix[0][1] << " - " << CurrentState.matrix[0][2] << endl;
			cout << CurrentState.matrix[1][0] << " - " << CurrentState.matrix[1][1] << " - " << CurrentState.matrix[1][2] << endl;
			cout << CurrentState.matrix[2][0] << " - " << CurrentState.matrix[2][1] << " - " << CurrentState.matrix[2][2] << endl;
			cout << CurrentState.matrix[3][0] << " - " << CurrentState.matrix[3][1] << " - " << CurrentState.matrix[3][2] << endl;
#endif
			return true;
			break;
		}
		case FACE_ARRAY:
		{
			unsigned short nfaces;
			C3DSFile->read( &nfaces,2);

#ifdef _DEBUG
			cout << "Face array : " << nfaces << endl;
#endif
			for (int i=0;i<nfaces;i++)
			{
				FACE f;
				C3DSFile->read( &f.p1,2);
				C3DSFile->read( &f.p2,2);
				C3DSFile->read( &f.p3,2);
				C3DSFile->read( &f.flags,2);

				currentFace.push_back(f);
			}

			return true;
			break;
		}
		case MSH_MAT_GROUP:
		{
			int pos = 0;
			unsigned short nfaces,num;
			char c;
			// object name
			do
			{
				C3DSFile->read( &c,1);
				CurrentState.name[pos++]=c;
			}
			while (c!=0);
			Material *m = NULL;

			string matName = CurrentState.name;
			if (materialRenamed)
			{
				for (unsigned int i=0;i<materialRenames.size();i++)
				{
					if (0 == materialRenames[i].oldName.compare(matName))
						matName = materialRenames[i].newName;
				}
			}

            MapStringToPtr::const_iterator it = matlib->find(matName);
			if(it != matlib->end())
			{
                m = (Material*)((*it).second);

                CGeometry::CRenderingModel& model = geo->getRenderingModel();

				if (geo->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
                {
                    CShadedGeometry *sh = static_cast<CShadedGeometry*>(geo);
				    CShader *s = sh->getShader();
				    *s->getMaterial() = *m->mat;
                    model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
				    if (m->tp.nTexture >= 0)
				    {
                        CTextureUnitSetup* ts = s->glGetTextureUnitsSetup();
                        ts->setDiffuseMap(maps->getTexture(m->tp.nTexture));

						for (unsigned int i=0;i<currentData.size();i++)
						{
							DATA &v = currentData[i];

							v.u = v.u * m->tp.uscale + m->tp.uoffset;
							v.v = v.v * m->tp.vscale + m->tp.voffset;
						}

                        model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
                    }
				}
				else
				{
					//	remove texture from rendering model
					model.removeModel(CGeometry::CRenderingModel::CGL_TEXTURE);
				}
                geo->setRenderingModel(model);
			}
			C3DSFile->read( &nfaces,2);
			
			for (int i=0;i<nfaces;i++)
				C3DSFile->read( &num,2);

			return true;
			break;
		}
		case SMOOTH_GROUP:
		{
			vector<FACE>::iterator it = currentFace.begin();
			while (it != currentFace.end())
			{
				FACE &f = *it++;
				unsigned int smoothGroup = 0;
				C3DSFile->read(&smoothGroup,4);

				unsigned short *p = &f.p1;
				while (p != &f.flags)
				{
					DATA &d = currentData[*p];
					if (d.sg == 0)
						d.sg = smoothGroup;
					else if ((d.sg & smoothGroup) != 0)
						d.sg &= smoothGroup;
					else
					{
						bool found = false;
						vector<DATA>::iterator it = duplicateData.begin();
						while (!found && (it != duplicateData.end()))
						{
							DATA &dup = *it++;
							found = ((dup.x == d.x) &&
									 (dup.y == d.y) &&
									 (dup.z == d.z) &&
									 ((dup.sg & smoothGroup) != 0));
							if (found)
							{
								dup.sg &= smoothGroup;
								*p = dup.idx;
							}
						}
						if (!found)
						{
							DATA newd = d;
							newd.idx = currentData.size() + duplicateData.size();
							newd.sg = smoothGroup;
							*p = newd.idx;
							duplicateData.push_back(newd);
						}
					}

					p++;
				}
			}

			if (duplicateData.size() > 0)
			{
				vector<DATA>::iterator it2 = duplicateData.begin();
				while (it2 != duplicateData.end())
					currentData.push_back(*it2++);
			}

			return true;
			break;
		}
		default:
		{
			C3DSFile->seek( length );
			return false;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////


bool RAPTOR_FASTCALL ProcessChunk5xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 5xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunk6xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 6xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunk7xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 7xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunk8xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 9xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunk9xxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk 9xxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}



/////////////////////////////////////////////////////////////////////////////
// 
//	Parameters that define a material
//	Shading : ambient, diffuse, specular
//	Mapping
//
bool RAPTOR_FASTCALL ProcessChunkAxxxH(long length)
{
	if (length == 0)
		return true;

	switch(CurrentChunk.chunk_id)
	{
	case MAT_MAPNAME:
		{
			// object name
			C3DSFile->read( CurrentState.name,length);
#ifdef _DEBUG
			cout << "  Map name : " << CurrentState.name << endl;
#endif
			return true;
		}
		break;
	case MAT_NAME:
		{
			// object name
			C3DSFile->read( CurrentState.name,length);
#ifdef _DEBUG
			cout << "  Material name : " << CurrentState.name << endl;
#endif
			bool newAlias = false;
			MaterialAlias alias;
			alias.oldName = CurrentState.name;
            alias.newName = "";

			stringstream rename;
			rename << CurrentState.name;

			while (matlib->find(rename.str()) != matlib->end() )
			{
				string str = "Raptor renamed duplicate 3DS material ";
				str += CurrentState.name;
				str += " to ";
				
				rename << "2";
				
				str += rename.str();
				Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                                               CRaptorErrorManager::RAPTOR_WARNING,str);
				materialRenamed = true;
				newAlias = true;
				alias.newName = rename.str();
			}
			mat->mat = new CMaterial(	CMaterial::CGL_NO_MATERIAL,
										CMaterial::CGL_NO_MATERIAL,
										CMaterial::CGL_NO_MATERIAL,
										0,
										CMaterial::CGL_NO_MATERIAL,
										rename.str());
			matlib->insert(MapStringToPtr::value_type(rename.str(), mat));
			if (newAlias)
				materialRenames.push_back(alias);
			return true;
		}
		break;
	case MAT_AMBIENT:
		{
#ifdef _DEBUG
			cout << "  Material ambient" << endl;
#endif
			// followed by color chunk
			ReadSubChunk();
			mat->mat->setAmbient(CurrentState.fred,CurrentState.fgreen,CurrentState.fblue,1.0);
			return true;
		}
		break;
	case MAT_DIFFUSE:
				{
#ifdef _DEBUG
			cout << "  Material diffuse" << endl;
#endif
			// followed by color chunk
			ReadSubChunk();
			mat->mat->setDiffuse(CurrentState.fred,CurrentState.fgreen,CurrentState.fblue,1.0);
			return true;
		}
		break;
	case MAT_SPECULAR:
		{
#ifdef _DEBUG
			cout << "  Material specular" << endl;
#endif
			// followed by color chunk
			ReadSubChunk();
			mat->mat->setSpecular(CurrentState.fred,CurrentState.fgreen,CurrentState.fblue,1.0);
			return true;
		}
		break;
	case MAT_ENTRY:
		{
#ifdef _DEBUG
			cout << "Material entry" << endl;
#endif
			mat = new Material;
			mat->mat = NULL;
			mat->tp.nTexture = -1;
			mat->tp.uscale = 1.0f;
			mat->tp.vscale = 1.0f;
			mat->tp.uoffset = 0.0f;
			mat->tp.voffset = 0.0f;

			// length of data in subchunck
			int l=0;
			while (l<length)
				l+=ReadSubChunk();		// material name, ambient...
			return true;
		}
		break;
	case MAT_SHININESS:
	{
#ifdef _DEBUG
		cout << "  Material shininess" << endl;
#endif
		// followed by percentage chunk
		ReadSubChunk();
		mat->mat->setShininess(CurrentState.spercentage);
		return true;
	}
	case MAT_TRANSPARENCY:
	{
#ifdef _DEBUG
		cout << "  Material transparency" << endl;
#endif
		// followed by percentage chunk
		ReadSubChunk();

		CColor::RGBA color = mat->mat->getAmbient();
		if ((color.b != 0) && (color.g != 0) && (color.r != 0))
		{
			color.a = 0.01f * (100 - CurrentState.spercentage);
			mat->mat->setAmbient(color.r,color.g,color.b,color.a);
		}
		color = mat->mat->getDiffuse();
		if ((color.b != 0) && (color.g != 0) && (color.r != 0))
		{
			color.a = 0.01f * (100 - CurrentState.spercentage);
			mat->mat->setDiffuse(color.r,color.g,color.b,color.a);
		}
		color = mat->mat->getSpecular();
		if ((color.b != 0) && (color.g != 0) && (color.r != 0))
		{
			color.a = 0.01f * (100 - CurrentState.spercentage);
			mat->mat->setSpecular(color.r,color.g,color.b,color.a);
		}

		// should transparency be set for emission ?
		return true;
		break;
	}
	case MAT_SHIN2PCT:
	case MAT_SHIN3PCT:
	case MAT_XPFALL:
	case MAT_REFBLUR:
	{
#ifdef _DEBUG
		cout << "  Material effects" << endl;
#endif
		// followed by percentage chunk
		ReadSubChunk();

		return true;
		break;
	}
	case MAT_SELF_ILPCT:
	{
#ifdef _DEBUG
		cout << "  Material effects" << endl;
#endif
		// followed by percentage chunk
		ReadSubChunk();

		// 3DSMax has no color for self illumination
		// so we will apply it as gray levels
		if (CurrentState.spercentage > 0)
		{
			float transparency = 0.01f * CurrentState.spercentage;
			mat->mat->setEmission(transparency,transparency,transparency,1.0f);
		}
		return true;
		break;
	}
	case MAT_SHADING:
	{
		// 3dsMAX 5 shadings:
		// 1 = Anisotropic
		// 2 = Blinn
		// 3 = MultiLayer
		// 4 = Metallic
		// 5 = Translucent
		// 6 = Oren-Nayar-Blinn
		// 7 = Phong
		// 8 = Strauss
		short shading;
		C3DSFile->read( &shading, length);
#ifdef _DEBUG
		cout << "Shading : " << shading << endl;
#endif
		return true;
		break;
	}
	case MAT_MAP_TILING:
#ifdef _DEBUG
			cout << "Map Tiling" << endl;
#endif
		C3DSFile->read( &CurrentState.sparam, length);
		return true;
		break;
	case MAT_MAP_TEXBLUR:
#ifdef _DEBUG
			cout << "Map bluring" << endl;
#endif
		C3DSFile->read( &CurrentState.fparam, length);
		return true;
		break;
	case MAT_TEXMAP:
	{
#ifdef _DEBUG
		cout << "  Material Map" << endl;
#endif
		// length of data in subchunck
		int l=0;
		while (l<length)
			l+=ReadSubChunk();		// texture name, percentage...

		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		CTextureObject* T;
		T = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
		T->glSetTransparency((unsigned char)((float)CurrentState.spercentage*255.0/100.0));

		string fname = CurrentState.name;
		if (!maxOptions.texturePath.empty())
			fname = maxOptions.texturePath +"\\" + CurrentState.name;

		bool result = false;
		const CTextureFactoryConfig::ICompressor* compressor = NULL;
		if ((maxOptions.compressTextures) && (0 < f.getConfig().getNumCompressors()))
		{
			compressor = f.getConfig().getCurrentCompressor();
			f.getConfig().setCurrentCompressor(f.getConfig().getCompressor("OpenGL"));				
		}
        result = f.glLoadTexture(T,fname);
		if (compressor != NULL)
			f.getConfig().setCurrentCompressor(compressor);
		if (result)
		{
			maps->addTexture(T);
			mat->tp.nTexture = maps->getNbTexture() - 1;
		}
#ifdef _DEBUG
		if (result)
			cout << "		LoadTexture OK" << endl;
		else
			cout << "		LoadTexture failed" << endl;
#endif
		return true;
		break;
	}
	case MAT_MAP_USCALE:
	{
		C3DSFile->read( &mat->tp.uscale, length);
		return true;
		break;
	}
	case MAT_MAP_VSCALE:
	{
		C3DSFile->read( &mat->tp.vscale, length);
		return true;
		break;
	}
	case MAT_MAP_UOFFSET:
	{
		C3DSFile->read( &mat->tp.uoffset, length);
		return true;
		break;
	}
	case MAT_MAP_VOFFSET:
	{
		C3DSFile->read( &mat->tp.voffset, length);
		return true;
		break;
	}
	default:
		C3DSFile->seek( length );
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// 
//	Nodes management, animations...
//
//	TODO:
//		Objects are modified directly. 
//		An Object3DInstance should be used instead but
//		for easier manipulations and lower memory
//		consumption, a single Geometry is used here
//		All keyframe should be read, only the first one
//		is used for testing purposes
//		Watch for new Raptor versions.
bool RAPTOR_FASTCALL ProcessChunkBxxxH(long length)
{
	switch(CurrentChunk.chunk_id)
	{
		case KFDATA:
		{
			int l=0;
			while (l<length)
				l+=ReadSubChunk();		// kfhdr...
			return false;
			break;
		}
		case KFHDR:
		{
			//followed by viewport_layout, kfseg, kfcurtime, object_node_tag, light_node_tag, target_node_tag, camera_node_tag, l_target_node_tag,
			//spotlight_node_tag, ambient_node_tag...
			//short revision; 
			//cstr filename; 
			//short animlen;
			unsigned char buffer[32];
			C3DSFile->read( buffer, length);
			return false;
			break;
		}
		case KFSEG:
		{
			unsigned char buffer[32];
			C3DSFile->read( buffer, length);
			return false;
			break;
		}
		case KFCURTIME:
		{
			unsigned char buffer[32];
			C3DSFile->read( buffer, length);
			return false;
			break;
		}
		case OBJECT_NODE_TAG:
		{
			CurrentState.currentNode.doScale = false;
			CurrentState.currentNode.doTranslate = false;
			CurrentState.currentNode.doRotate = false;

			int l=0;
			while (l<length)
				l+=ReadSubChunk();		// kfhdr...

			//	Perfomr transforms:
			//	- scaling
			//	- rotation
			//	- translation
			if (CurrentState.currentNode.object != NULL)
			{

	//			GL_COORD_VERTEX v;
	//			CurrentState.currentNode.object->GetCenter(v);

		//		if (CurrentState.currentNode.doScale)
		//		CurrentState.currentNode.object->Scale(	CurrentState.currentNode.scale.scalex,
		//												CurrentState.currentNode.scale.scaley,
		//												CurrentState.currentNode.scale.scalez);
//
//		
//				if (CurrentState.currentNode.doTranslate)
//				{
//					GL_COORD_VERTEX	v;
//					CurrentState.currentNode.object->GetCenter(v);
//					CurrentState.currentNode.object->Translate(	v.x - CurrentState.currentNode.translation.vecx,
//																v.y - CurrentState.currentNode.translation.vecy,
//																v.z - CurrentState.currentNode.translation.vecz);
//				}


//				if (CurrentState.currentNode.doRotate)
//				CurrentState.currentNode.object->Rotation(	CurrentState.currentNode.rotation.angle * DEG,
//															CurrentState.currentNode.rotation.axisx,
//															CurrentState.currentNode.rotation.axisy,
//															CurrentState.currentNode.rotation.axisz);

			}

			return false;
			break;
		}
		case NODE_HDR:
		{
			int pos = 0;
			char c;
			do
			{
				C3DSFile->read( &c,1);
				CurrentState.name[pos++]=c;
			}
			while (c!=0);

			short flags[3];
			C3DSFile->read( flags, 6 );

			CPersistence *p = CPersistence::FindObject(CurrentState.name);
			if ((p != NULL) && (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId())))
			{
				CurrentState.currentNode.object = (CGeometry*)p;
			} 
			else 
			{ 
				CurrentState.currentNode.object = NULL; 
			}

			return false;
			break;
		}
		case PIVOT:
		{
			float buffer[32];
			C3DSFile->read( buffer, length);
			
			CurrentState.currentNode.pivot.posx = buffer[0];
			CurrentState.currentNode.pivot.posy = buffer[1];
			CurrentState.currentNode.pivot.posz = buffer[2];

			return false;
			break;
		}
		case POS_TRACK_TAG:
		{
			float buffer[32];
			C3DSFile->read( buffer, length);

			CurrentState.currentNode.translation.vecx = buffer[5];
			CurrentState.currentNode.translation.vecy = buffer[6];
			CurrentState.currentNode.translation.vecz = buffer[7];
			CurrentState.currentNode.doTranslate = true;

			return false;
			break;
		}
		case ROT_TRACK_TAG:
		{
			float buffer[32];
			C3DSFile->read( buffer, length);

			CurrentState.currentNode.rotation.angle = buffer[5];
			CurrentState.currentNode.rotation.axisx = buffer[6];
			CurrentState.currentNode.rotation.axisy = buffer[7];
			CurrentState.currentNode.rotation.axisz = buffer[8];
			CurrentState.currentNode.doRotate = (buffer[5] != 0);

			return false;
			break;
		}
		case SCL_TRACK_TAG:
		{
			float buffer[32];
			C3DSFile->read( buffer, length);

			CurrentState.currentNode.scale.scalex = buffer[5];
			CurrentState.currentNode.scale.scaley = buffer[6];
			CurrentState.currentNode.scale.scalez = buffer[7];
			CurrentState.currentNode.doScale = ((buffer[5] != 1.0f) && (buffer[6] != 1.0f) && (buffer[7] != 1.0f));

			return false;
			break;
		}
		case NODE_ID:
		{
			C3DSFile->read( &CurrentState.currentNode.id, length);
			return false;
			break;
		}
	default:
		C3DSFile->seek( length );
		return true;
	}
}
/////////////////////////////////////////////////////////////////////////////


bool RAPTOR_FASTCALL ProcessChunkCxxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk Cxxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunkDxxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk Dxxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunkExxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk Exxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}
bool RAPTOR_FASTCALL ProcessChunkFxxxH(long length)
{
#ifdef _DEBUG
		cout << "Chunk Fxxx" << endl;
#endif
	C3DSFile->seek( length );
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// 
//	Recursively parse hierarchy of chunks in 3DS file
//
int ReadSubChunk()
{
	if (0 == ReadChunkHeader())
		return 0;

	// chunk group
	unsigned char	group = (unsigned char)((CurrentChunk.chunk_id & 0xF000)>>12);
	long	size = CurrentChunk.chunk_len;
	CurrentState.unparsed_data = ((PFN)Procs[group])(size-6);

	return size;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading a 3DS file
//
bool _glLoadScene(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options)
{
	CurrentState.version = 0;
	CurrentState.name[0] = 0;
	CurrentState.scale = 0.0f;
	CurrentState.shading = 0;
	CurrentState.spercentage = 0;
	CurrentState.fpercentage = 0.0f;
	CurrentState.cred = 0;
	CurrentState.cgreen = 0;
	CurrentState.cblue = 0;
	CurrentState.fred = 0.0f;
	CurrentState.fgreen = 0.0f;
	CurrentState.fblue = 0.0f;
	CurrentState.sparam = 0;
	CurrentState.fparam = 0.0f;
	CurrentState.lparam = 0;
	memset(&CurrentState.matrix,0,sizeof(float)*4*3);
	CurrentState.unparsed_data = false;

	materialRenamed = false;
	materialRenames.clear();
    discardGeometryHints.clear();
    geo = NULL;

	maxOptions.texturePath = "";
	maxOptions.useMaxTransform = false;
    maxOptions.compressTextures = false;
    maxOptions.discardGeometry = NULL;

	if (Open3DSFile(fname.data()))
	{
		if (options != NULL)
			maxOptions = *options;

        int pos = 0;
        if (maxOptions.discardGeometry != NULL)
            while (maxOptions.discardGeometry[pos] != NULL)
                discardGeometryHints.push_back(maxOptions.discardGeometry[pos++]);

		currentSet = new C3DSet(fname);
        C3DSet::C3DSetIterator it = root->getIterator();
		root->addChild(currentSet,it);

		while (ReadSubChunk()>0);

        if (geo != NULL)
			FlushGeometry(geo);

		set = currentSet;
		return Close3DSFile();

        discardGeometryHints.clear();
	}
	else
	{
		set = NULL;
		return false;
	}
}

