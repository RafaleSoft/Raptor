// VertexShadersDisplay.cpp: implementation of the CVertexShadersDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#include "VertexShadersDisplay.h"
#include "..\VertexShader.h"
#include "..\GLDocument.h"
#include "..\3DEngine.h"
#include "..\TextureUnitSetup.h"
#include "..\Raptor.h"
#include "..\StrippedGeometry.h"
#include "..\Shader.h"
#include "..\TextureFactory.h"


CString waterShader = 
"!!ARBvp1.0 \
ATTRIB iPos = vertex.position; \
ATTRIB iColor = vertex.color.primary; \
ATTRIB iTexCoord0 = vertex.texcoord; \
PARAM mvp[4] = { state.matrix.mvp }; \
PARAM xDirs = program.local[1]; \
PARAM zDirs = program.local[2]; \
PARAM A = program.local[3]; \
PARAM freq = program.local[4]; \
PARAM phase = program.local[5]; \
PARAM Q = program.local[6]; \
PARAM kSinConsts = { 1.0 , -0.16667, 0.00833, -0.0001984 }; \
PARAM kCosConsts = { 1.0 , -0.5, 0.041667, -0.0013889 }; \
PARAM kPis = { 3.14159265 , 6.28318530 ,  0.15915494, 0.0 }; \
TEMP coord; \
TEMP coord2; \
TEMP coord3; \
TEMP coord4; \
TEMP coord5; \
TEMP coord6; \
TEMP coord7; \
TEMP sinus; \
TEMP cosinus; \
OUTPUT oPos = result.position; \
OUTPUT oPCol = result.color.primary; \
OUTPUT oTex0 = result.texcoord; \
#	Calcul de la position de l'onde:		\
	Di . (x,z) * wi + t * phi				\n\
MUL coord, xDirs, iPos.xxxx; \
MAD coord, zDirs, iPos.zzzz, coord; \
MAD coord, coord, freq, phase; \
#	Mod [-pi..pi], car le developpement limite		\
	des sinus et cosinus a l'ordre 7 est correct	\
	entre [-pi..pi]									\n\
ADD coord, kPis.xxxx, coord; \
MUL coord, kPis.zzzz, coord; \
FRC coord, coord; \
MUL coord, kPis.yyyy, coord; \
ADD coord, -kPis.xxxx, coord; \
#	Calcul des cosinus et sinus		\n\
MUL coord2, coord, coord; \
MUL coord3, coord2, coord; \
MUL coord4, coord2, coord2; \
MUL coord5, coord3, coord2; \
MUL coord6, coord3, coord3; \
MUL coord7, coord4, coord3; \
MAD sinus, coord3, kSinConsts.yyyy, coord; \
MAD sinus, coord5, kSinConsts.zzzz, sinus; \
MAD sinus, coord7, kSinConsts.wwww, sinus; \
MAD cosinus, coord2, kCosConsts.yyyy, kCosConsts.xxxx; \
MAD cosinus, coord4, kCosConsts.zzzz, cosinus; \
MAD cosinus, coord6, kCosConsts.wwww , cosinus; \
";

CString waterShader2 =
"#	Resultat final						\
	- position							\n\
PARAM factor = { 1.0, 0.0, 1.0, 1.0 }; \
PARAM kNx = program.local[7]; \
PARAM kNy = program.local[8]; \
PARAM kNz = program.local[9]; \
TEMP waves; \
MUL waves, cosinus, Q; \
DP4 coord.x, waves, xDirs; \
DP4 coord.y, A, sinus; \
DP4 coord.z, waves, zDirs; \
MOV coord.w, factor.y; \
MUL waves, iPos, factor; \
ADD coord, coord, waves; \
DP4 oPos.x , mvp[0] , coord; \
DP4 oPos.y , mvp[1] , coord; \
DP4 oPos.z , mvp[2] , coord; \
DP4 oPos.w, mvp[3], coord; \
MOV oTex0, iTexCoord0; \
#	- couleur							\n\
PARAM ambient = { 0.0, 0.0, 0.0, 0.0 }; \
TEMP normal;\
MOV waves,factor.yxyw; \
DP4 normal.x, kNx, cosinus; \
DP4 normal.y, kNy, sinus; \
DP4 normal.z, kNz, cosinus; \
SUB normal, waves, normal; \
DP3 normal.w, normal, normal; \
RSQ normal.w, normal.w; \
MUL normal.xyz, normal.w, normal; \
MOV coord, factor; \
DP3 coord.x, normal, waves; \
MAD oPCol, iColor, coord.xxxw, ambient; \
END\
";


#define TREE_SIZE	8192
#define MAX_DEPTH	6

#define SPLIT_LIMIT	0.01


GLfloat leaf[4*16] = 
{ 
	0.0f , 0.0f , 0.0f , 1.0f,
	-1.5f, 1.0f , 0.0f , 1.0f,
	-3.0f, 1.0f , 0.0f , 1.0f,
	-2.0f, 0.0f , 0.0f , 1.0f,
	-0.75f,2.0f , 0.0f , 1.0f,
	-2.0f, 3.0f , 0.0f , 1.0f,
	-1.5f, 1.5f , 0.0f , 1.0f,
	0.5f , 2.0f , 0.0f , 1.0f,
	0.0f , 5.0f , 0.0f , 1.0f,
	-0.5f, 2.0f , 0.0f , 1.0f,
	1.5f , 1.5f , 0.0f , 1.0f,
	2.0f , 3.0f , 0.0f , 1.0f,
	0.75f, 2.0f , 0.0f , 1.0f,
	2.0f , 0.0f , 0.0f , 1.0f,
	3.0f , 1.0f , 0.0f , 1.0f,
	1.5f , 1.0f , 0.0f , 1.0f
};

GLuint leafIndexes[21] = 
{	
	0 , 
	1 , 2 ,
	2 , 3 ,
	4 , 5 ,
	5 , 6 ,
	7 , 8 ,
	8 , 9 ,
	10, 11,
	11, 12,
	13, 14,
	14, 15,
};

GLfloat depths[MAX_DEPTH+1] =
{
	0.01f,
	0.01f,
	0.05f,
	0.1f,
	0.2f,
	0.3f,
	0.4f,
};

static GL_COORD_VERTEX tree[TREE_SIZE];
static GL_COORD_VERTEX solidtree[TREE_SIZE][8];
static GL_COORD_VERTEX leaves[TREE_SIZE];
static GL_COORD_VERTEX leavesTransforms[TREE_SIZE];
static unsigned int tree_size[32];
static unsigned int current_pos = 0;
static unsigned int current_leaf = 0;
static unsigned int current_solidtree = 0;
static unsigned int solidtreeIndexes[TREE_SIZE*32];


void DrawCylinder(int index0,int index1,float diameter0,float diameter1)
{
	GL_COORD_VERTEX v;
	GL_COORD_VERTEX v1;
	GL_COORD_VERTEX v2;

	v.x = tree[index1].x - tree[index0].x;
	v.y = tree[index1].y - tree[index0].y;
	v.z = tree[index1].z - tree[index0].z;

	Raptor::gl3DEngine()->Normalize(v,1.0f);
	Raptor::gl3DEngine()->Normals(v,v1,v2);

	for (int i=0;i<8;i++)
		solidtree[index0][i] = tree[index0];
	for (i=0;i<8;i++)
		solidtree[index1][i] = tree[index1];

	//	Base
	solidtree[index0][0].x += v1.x * diameter0;
	solidtree[index0][0].y += v1.y * diameter0;
	solidtree[index0][0].z += v1.z * diameter0;

	solidtree[index0][1].x += (v1.x + v2.x) * 0.707f * diameter0;
	solidtree[index0][1].y += (v1.y + v2.y) * 0.707f * diameter0;
	solidtree[index0][1].z += (v1.z + v2.z) * 0.707f * diameter0;

	solidtree[index0][2].x += v2.x * diameter0;
	solidtree[index0][2].y += v2.y * diameter0;
	solidtree[index0][2].z += v2.z * diameter0;

	solidtree[index0][3].x += (-v1.x + v2.x) * 0.707f * diameter0;
	solidtree[index0][3].y += (-v1.y + v2.y) * 0.707f * diameter0;
	solidtree[index0][3].z += (-v1.z + v2.z) * 0.707f * diameter0;

	solidtree[index0][4].x -= v1.x * diameter0;
	solidtree[index0][4].y -= v1.y * diameter0;
	solidtree[index0][4].z -= v1.z * diameter0;

	solidtree[index0][5].x += (-v1.x - v2.x) * 0.707f * diameter0;
	solidtree[index0][5].y += (-v1.y - v2.y) * 0.707f * diameter0;
	solidtree[index0][5].z += (-v1.z - v2.z) * 0.707f * diameter0;

	solidtree[index0][6].x -= v2.x * diameter0;
	solidtree[index0][6].y -= v2.y * diameter0;
	solidtree[index0][6].z -= v2.z * diameter0;

	solidtree[index0][7].x += (v1.x - v2.x) * 0.707f * diameter0;
	solidtree[index0][7].y += (v1.y - v2.y) * 0.707f * diameter0;
	solidtree[index0][7].z += (v1.z - v2.z) * 0.707f * diameter0;


	//	Top
	solidtree[index1][0].x += v1.x * diameter1;
	solidtree[index1][0].y += v1.y * diameter1;
	solidtree[index1][0].z += v1.z * diameter1;

	solidtree[index1][1].x += (v1.x + v2.x) * 0.707f * diameter1;
	solidtree[index1][1].y += (v1.y + v2.y) * 0.707f * diameter1;
	solidtree[index1][1].z += (v1.z + v2.z) * 0.707f * diameter1;

	solidtree[index1][2].x += v2.x * diameter1;
	solidtree[index1][2].y += v2.y * diameter1;
	solidtree[index1][2].z += v2.z * diameter1;

	solidtree[index1][3].x += (-v1.x + v2.x) * 0.707f * diameter1;
	solidtree[index1][3].y += (-v1.y + v2.y) * 0.707f * diameter1;
	solidtree[index1][3].z += (-v1.z + v2.z) * 0.707f * diameter1;

	solidtree[index1][4].x -= v1.x * diameter1;
	solidtree[index1][4].y -= v1.y * diameter1;
	solidtree[index1][4].z -= v1.z * diameter1;

	solidtree[index1][5].x += (-v1.x - v2.x) * 0.707f * diameter1;
	solidtree[index1][5].y += (-v1.y - v2.y) * 0.707f * diameter1;
	solidtree[index1][5].z += (-v1.z - v2.z) * 0.707f * diameter1;

	solidtree[index1][6].x -= v2.x * diameter1;
	solidtree[index1][6].y -= v2.y * diameter1;
	solidtree[index1][6].z -= v2.z * diameter1;

	solidtree[index1][7].x += (v1.x - v2.x) * 0.707f * diameter1;
	solidtree[index1][7].y += (v1.y - v2.y) * 0.707f * diameter1;
	solidtree[index1][7].z += (v1.z - v2.z) * 0.707f * diameter1;

}

void BuildTree(GL_COORD_VERTEX &dir,GL_COORD_VERTEX &pos,unsigned int depth)
{
	GL_COORD_VERTEX p = pos;
	GL_COORD_VERTEX d = dir;

	if (depth == 0)
	{
		leaves[current_leaf++] = p;
		return;
	}

	float factor = 1.0f / (MAX_DEPTH - depth + 1);

	tree[current_pos++] = p;

	if (depth == 1)
	{
		p.x += 0.8f * d.x;
		p.y += 0.8f * d.y;
		p.z += 0.8f * d.z;
		leaves[current_leaf++] = p;
	}
	else
	{
		p.x += d.x;
		p.y += d.y;
		p.z += d.z;
	}

	tree[current_pos++] = p;

	DrawCylinder(current_pos-2,current_pos-1,depths[depth],depths[depth-1]);

	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;

	for (unsigned int i=0;i<depth;i++)
	{
		dx = (rand() % 100) * 0.001f - 0.05f;
		dy = (rand() % 100) * 0.001f - 0.05f;
		dz = (rand() % 100) * 0.001f - 0.05f;

		d.x += dx;
		d.y += dy;
		d.z += dz;

		tree[current_pos++] = p;
		p.x += factor * d.x;
		p.y += factor * d.y;
		p.z += factor * d.z;
		tree[current_pos++] = p;

		DrawCylinder(current_pos-2,current_pos-1,depths[depth-1],depths[depth-1]);

		if ((fabs(dx) > SPLIT_LIMIT) || (fabs(dy) > SPLIT_LIMIT) || (fabs(dz) > SPLIT_LIMIT))
		{
			GL_COORD_VERTEX b_dir;
			b_dir.x = d.x + (rand() % 100) * 0.01f - 0.5f;
			b_dir.y = d.y + (rand() % 100) * 0.01f - 0.5f;
			b_dir.z = d.z + (rand() % 100) * 0.01f - 0.5f;
			b_dir.h = 1.0f;

			C3DEngine::Normalize(b_dir,1.0f);

			BuildTree(b_dir,p,depth-1);
		}
	
	}
}

__inline void __fastcall drawLeaf(void)
{
	glDrawElements(GL_TRIANGLE_FAN,21,GL_UNSIGNED_INT,leafIndexes);
}



class See : public CStrippedGeometry
{
public:
	See(float width,float height,int hcels,int vcels)
	{
		float orgx = -width * 0.5f;
		float orgy = -height * 0.5f;
		float stepx = width / hcels;
		float stepy = height / vcels;

		m_indexes = new GLuint[2 * hcels * vcels];

		int pos = 0;
		memset(m_indexes,0,2*hcels*vcels*sizeof(GLuint));
		for (int k=0;k<vcels-2;k+=2)
		{
			for (int i=0;i<hcels;i++)
				for (int j=k+1;j>=k;j--)
				{
					m_indexes[pos++] = hcels * j + i;
				}
			for (i=hcels-1;i>=0;i--)
				for (int j=k+1;j<k+3;j++)
				{
					m_indexes[pos++] = hcels * j + i;
				}
		}

		AllocVertex(hcels*vcels);
		AllocTexCoords(hcels*vcels);
		for (int j=0;j<vcels;j++)
			for (int i=0;i<hcels;i++)
			{
				AddVertex(orgx+i*stepx,0,-orgy-j*stepy,1.0f);
				SetTexCoord(i+j*hcels, 20 * (float)i / hcels , 20 * (float)j / vcels);
			}
		AddStrip(pos /*2*hcels*vcels*/,m_indexes);

	}

	virtual ~See() {};

private:
	GLuint		*m_indexes;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVertexShader *shaderProgram = NULL;

CVertexShadersDisplay::CVertexShadersDisplay():
	dt(0),see(NULL)
{
}

CVertexShadersDisplay::~CVertexShadersDisplay()
{
}


void CVertexShadersDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

void CVertexShadersDisplay::Init()
{
	CGenericDisplay::Init();

	GL_COORD_VERTEX v(0.0f , 1.0f , 0.0f , 1.0f );

	GL_COORD_VERTEX pos(0.0f , 0.0f , 0.0f , 1.0f );
	BuildTree(v,pos,MAX_DEPTH);

	for (unsigned int i=0;i<4*16;i+=4)
	{
		leaf[i] *= 0.1f;
		leaf[i+1] *= 0.1f;
		leaf[i+2] *= 0.1f;
	}

	for (i=0;i<current_leaf;i++)
	{
		v.x = 0.01f * (rand()%100);
		v.y = 0.01f * (rand()%100);
		v.z = 0.01f * (rand()%100);
		v.h = 1.0f;

		C3DEngine::Normalize(v,1.0f);
		leavesTransforms[i].x = (float)(rand() % 360);
		leavesTransforms[i].y = v.x;
		leavesTransforms[i].z = v.y;
		leavesTransforms[i].h = v.z;
	}

	See *water = new See(2000.0f,2000.0f,100,100);
	water->SetRenderingModel(CGL_GEOMETRY/*|CGL_WIREFRAME*/|CGL_TEXTURE);
	CShader *pShader = new CShader("WATER_SHADER");
	water->SetShader(pShader);
	see = water;

	CTextureUnitSetup *ts = pShader->GetTextureUnitsSetup();
	CTextureFactory *t = (CTextureFactory*)(CPersistence::FindObject("main_textures"));
	LP_GL_TEXTURE_OBJECT T = t->CreateTexture(	CTextureFactory::CGL_ALPHA_TRANSPARENT,
												CTextureFactory::CGL_BILINEAR);
	T->alpha = 128;
	t->LoadTexture(T,"Datas\\water006.jpg");
	ts->tmu0 = T;

	CVertexShader *vp = pShader->GetVertexShader();
	vp->LoadProgram(waterShader+waterShader2);
	shaderProgram = vp;
}



void CVertexShadersDisplay::Display()
{
	if (reinit)
		ReInit();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glColor4f(1.0f,1.0f,1.0f,0.5f);

	glTranslatef(0,0.0f,-1000.0f);
	glRotatef(10.0f,1.0f,0.0f,0.0f);
//	glRotatef(360*dt,0.0f,1.0f,0.0f);

	shaderProgram->glRender();
	
	GL_COORD_VERTEX xDir(0.0, 0.866, -0.866, 0.0);
	GL_COORD_VERTEX zDir(-1.0, -0.5, -0.5, -1.0);
	GL_COORD_VERTEX A(12.0, 4.0, 3.0, 2.0);
	GL_COORD_VERTEX freq(	2 * PI / 200.0f, 
							2 * PI / 150.0f, 
							2 * PI / 100.0f, 
							2 * PI / 80.0f);
	GL_COORD_VERTEX phase(	5000 * dt * 2 * freq.x, 
							5000 * dt * 1.5 * freq.y,
							5000 * dt * 1 * freq.z,
							5000 * dt * 0.5 * freq.h);

	// Qi = 1 / ( wi . Ai )
	// Sum(Qi.wi.Ai) are slightly > 1 to create a small loop on crest, more funny.
	GL_COORD_VERTEX Q(	1.0 / (2.5f * freq.x),
						1.0 / (2.5f * freq.y),
						1.0 / (2.5f * freq.z),
						1.0 / (2.5f * freq.h));

	GL_COORD_VERTEX kNx(	xDir.x * A.x * freq.x,
							xDir.y * A.y * freq.y,
							xDir.z * A.z * freq.z,
							xDir.h * A.h * freq.h);
	GL_COORD_VERTEX kNy(	0.25,
							0.25,
							0.25,
							0.25);
	GL_COORD_VERTEX kNz(	zDir.x * A.x * freq.x,
							zDir.y * A.y * freq.y,
							zDir.z * A.z * freq.z,
							zDir.h * A.h * freq.h);

	shaderProgram->glProgramParameter(1,xDir);
	shaderProgram->glProgramParameter(2,zDir);
	shaderProgram->glProgramParameter(3,A);
	shaderProgram->glProgramParameter(4,freq);
	shaderProgram->glProgramParameter(5,phase);
	shaderProgram->glProgramParameter(6,Q);
	shaderProgram->glProgramParameter(7,kNx);
	shaderProgram->glProgramParameter(8,kNy);
	shaderProgram->glProgramParameter(9,kNz);

	glEnable(GL_TEXTURE_2D);
	see->glRender();
	shaderProgram->Stop();

/*
	glTranslatef(0,-8.0f,-10.0f);
	glRotatef(360*dt,0.0f,1.0f,0.0f);

	unsigned int pos = 0;

	glDisable(GL_CULL_FACE);
	glColor4f(1.0f,0.0f,0.0f,0.5f);


	while (pos < current_pos)
	{
		glBegin(GL_QUAD_STRIP);
		for (int i=0;i<8;i++)
		{	
			glVertex3fv(solidtree[pos+1][i]);
			glVertex3fv(solidtree[pos][i]);
		}
		glVertex3fv(solidtree[pos+1][0]);
		glVertex3fv(solidtree[pos][0]);
		pos+=2;
		glEnd();
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(4,GL_FLOAT,0,leaf);
	glDisable(GL_CULL_FACE);	
	glColor4f(1.0f,1.0f,0.0f,1.0f);

	for (unsigned int i=0;i<current_leaf;i++)
	{
		GL_COORD_VERTEX &v = leaves[i];
		glPushMatrix();
		glTranslatef(v.x,v.y,v.z);
		glRotatef(leavesTransforms[i].x,leavesTransforms[i].y,leavesTransforms[i].z,leavesTransforms[i].h);
		drawLeaf();
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_CULL_FACE);
*/
	glPopMatrix();

	dt += 0.0001f;
	if (dt > 1) dt = 0.0f;
}