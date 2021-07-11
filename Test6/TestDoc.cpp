// TestDoc.cpp : implementation of the CTestDoc class
//

#include "StdAfx.h"
#include <sstream>

#include "TestDoc.h"

#include "System/RaptorDisplay.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/OpenGLShaderStage.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/GeometryShader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/Shader.h"
#include "Engine/ViewModifier.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "Engine/IViewPoint.h"
#include "System/Raptor.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/3DSet.h"
#include "System/RaptorConsole.h"

#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/BasicObjects.h"


extern const std::string vp_src;
extern const std::string gp_src;
extern const std::string fp_src;

extern const std::string vp2_src;
extern const std::string gp2_src;
extern const std::string fp2_src;

extern const std::string vp3_src;
extern const std::string gp3_src;
extern const std::string fp3_src;

extern const std::string gp4_src_line =
"#version 460 compatibility\n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 v_color[]; \n\
const float extrusion = 5.0f; \n\
const vec4 lpos = vec4(5.0, 5.0, 5.0, 1.0); \n\
\n\
layout(lines) in; \n\
layout(line_strip, max_vertices=16) out; \n\
\n\
out vec4 g_color; \n\
\n\
vec4 extrude(vec4 p1)\n\
{\n\
	vec4 lvect = p1 - lpos; \n\
	vec4 pos = extrusion * normalize(lvect) + p1; \n\
	return pos; \n\
}\n\
\n\
void main() \n\
{\n\
	vec4 v_Min = gl_in[0].gl_Position; \n\
	vec4 v_Max = gl_in[1].gl_Position; \n\
\n\
	g_color = v_color[0]; \n\
\n\
	// top: Back right \n\
	vec4 pos = vec4(v_Max.x, v_Max.y, v_Min.z, 1.0); \n\
	vec4 tbr = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = tbr; \n\
	EmitVertex(); \n\
	// top: Back Left \n\
	pos = vec4(v_Min.x, v_Max.y, v_Min.z, 1.0); \n\
	vec4 tbl = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = tbl; \n\
	EmitVertex(); \n\
	// top: Front Left \n\
	pos = vec4(v_Min.x, v_Max.y, v_Max.z, 1.0); \n\
	vec4 tfl = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = tfl; \n\
	EmitVertex(); \n\
	// top: Front Right \n\
	pos = vec4(v_Max.x, v_Max.y, v_Max.z, 1.0); \n\
	vec4 tfr = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = tfr; \n\
	EmitVertex(); \n\
\n\
	// right: Top back \n\
	gl_Position = tbr; \n\
	EmitVertex(); \n\
	// right: Bottom back \n\
	pos = vec4(v_Max.x, v_Min.y, v_Min.z, 1.0); \n\
	vec4 bbr = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = bbr; \n\
	EmitVertex(); \n\
	// right: Bottom front \n\
	pos = vec4(v_Max.x, v_Min.y, v_Max.z, 1.0); \n\
	vec4 bfr = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = bfr; \n\
	EmitVertex(); \n\
	// front: Top Right \n\
	gl_Position = tfr; \n\
	EmitVertex(); \n\
\n\
	// front: Top Left \n\
	gl_Position = tfl; \n\
	EmitVertex(); \n\
	// front: Bottom Left \n\
	pos = vec4(v_Min.x, v_Min.y, v_Max.z, 1.0); \n\
	vec4 bfl = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = bfl; \n\
	EmitVertex(); \n\
	// back: Bottom Left \n\
	pos = vec4(v_Min.x, v_Min.y, v_Min.z, 1.0); \n\
	vec4 bbl = gl_ModelViewProjectionMatrix * extrude(pos); \n\
	gl_Position = bbl; \n\
	EmitVertex(); \n\
	// back: Top Left \n\
	gl_Position = tbl; \n\
	EmitVertex(); \n\
\n\
	// bottom: Back Left\n\
	gl_Position = bbl; \n\
	EmitVertex(); \n\
	// bottom: Back Right\n\
	gl_Position = bbr; \n\
	EmitVertex(); \n\
	// bottom: Front Right\n\
	gl_Position = bfr; \n\
	EmitVertex(); \n\
	// bottom: Front Left\n\
	gl_Position = bfl; \n\
	EmitVertex(); \n\
\n\
	EndPrimitive(); \n\
}\n\
";

extern const std::string gp4_src =
"#version 440 compatibility \n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 v_color[]; \n\
const float extrusion = 5.0f; \n\
const vec4 lpos = vec4(5.0, 5.0, 5.0, 1.0); \n\
\n\
layout(lines) in; \n\
layout(triangle_strip, max_vertices = 18) out; \n\
\n\
out vec4 g_color; \n\
\n\
vec4 extrude(vec4 p1)\n\
{\n\
	vec4 lvect = p1 - lpos; \n\
	vec4 pos = extrusion * normalize(lvect) + p1; \n\
	return pos; \n\
}\n\
\n\
void main()\n\
{\n\
	vec4 v_Min = gl_in[0].gl_Position;\n\
	vec4 v_Max = gl_in[1].gl_Position;\n\
\n\
	g_color = v_color[0];\n\
\n\
	// top: Back right\n\
	vec4 pos = vec4(v_Max.x, v_Max.y, v_Min.z, 1.0);\n\
	vec4 tbr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tbr;\n\
	EmitVertex();\n\
	// top: Back Left\n\
	pos = vec4(v_Min.x, v_Max.y, v_Min.z, 1.0);\n\
	vec4 tbl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tbl;\n\
	EmitVertex();\n\
	// top: Front Right\n\
	pos = vec4(v_Max.x, v_Max.y, v_Max.z, 1.0);\n\
	vec4 tfr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tfr;\n\
	EmitVertex();\n\
	// top: Front Left \n\
	pos = vec4(v_Min.x, v_Max.y, v_Max.z, 1.0);\n\
	vec4 tfl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
\n\
	// front: Top Left\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
	// front: Bottom Left\n\
	pos = vec4(v_Min.x, v_Min.y, v_Max.z, 1.0);\n\
	vec4 bfl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
	// front: Top Right\n\
	gl_Position = tfr;\n\
	EmitVertex();\n\
	// front: Bottom Right\n\
	pos = vec4(v_Max.x, v_Min.y, v_Max.z, 1.0);\n\
	vec4 bfr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bfr;\n\
	EmitVertex();\n\
\n\
	// right: Top back\n\
	gl_Position = tbr;\n\
	EmitVertex();\n\
	// right: Bottom back\n\
	pos = vec4(v_Max.x, v_Min.y, v_Min.z, 1.0);\n\
	vec4 bbr = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bbr;\n\
	EmitVertex();\n\
\n\
	// back: Top Left\n\
	gl_Position = tbl;\n\
	EmitVertex();\n\
	// back: Bottom Left\n\
	pos = vec4(v_Min.x, v_Min.y, v_Min.z, 1.0);\n\
	vec4 bbl = gl_ModelViewProjectionMatrix * extrude(pos);\n\
	gl_Position = bbl;\n\
	EmitVertex();\n\
\n\
	// left: Top front\n\
	gl_Position = tfl;\n\
	EmitVertex();\n\
	// left: Bottom front\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
\n\
	// bottom: Front Left\n\
	gl_Position = bfl;\n\
	EmitVertex();\n\
	// bottom: Back Left\n\
	gl_Position = bbl;\n\
	EmitVertex();\n\
	// bottom: Front Right\n\
	gl_Position = bfr;\n\
	EmitVertex();\n\
	// bottom: Back Right\n\
	gl_Position = bbr;\n\
	EmitVertex();\n\
\n\
	EndPrimitive();\n\
}";


class CPoints : public CShadedGeometry
{
public:
	CPoints() {};
	virtual ~CPoints()
	{
	};

	void Init(uint16_t s);

	virtual void glRender()
	{
		glPushAttrib(GL_ENABLE_BIT);

		GLint blendSrc;
		GLint blendDst;
		glGetIntegerv(GL_BLEND_SRC, &blendSrc);
		glGetIntegerv(GL_BLEND_DST, &blendDst);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		CShadedGeometry::glRender();

		glBlendFunc(blendSrc, blendDst);
		glPopAttrib();
	}
};

void CPoints::Init(uint16_t s)
{
	addModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_COLORS);
	addModel(CGeometry::CGL_TEXTURE);
	addModel(CGeometry::CGL_WEIGHT);
	addModel(CGeometry::CGL_FOG);

	glSetVertices(s);
	glSetWeights(s);
	glSetFogs(s);
	glSetColors(s);

	glLockData();

	for (uint16_t i = 0; i < s; i++)
	{
		float x = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float y = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float z = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		addVertex(x, y, z, 1.0f);
		setWeight(i, (0.5f * rand()) / RAND_MAX);
		setFogCoord(i, 2*PI*(rand()) / RAND_MAX);
		setColor(i, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX);
	}

	glUnLockData();

	CGeometryPrimitive *p = createPrimitive(CGeometryPrimitive::POINT);

	uint16_t *points = new uint16_t[s];
	for (uint16_t i = 0; i < s; i++)
		points[i] = i;

	p->setIndexes(s, points);

	delete[] points;
}

class CCube : public CShadedGeometry
{
public:
	CCube() {};
	virtual ~CCube()
	{
	};

	void Init(uint16_t s);

	virtual void glRender()
	{
		glPushAttrib(GL_ENABLE_BIT);

		GLint blendSrc;
		GLint blendDst;
		glGetIntegerv(GL_BLEND_SRC, &blendSrc);
		glGetIntegerv(GL_BLEND_DST, &blendDst);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		CShadedGeometry::glRender();

		glBlendFunc(blendSrc, blendDst);
		glPopAttrib();

		glRenderBBox();
	}
};

void CCube::Init(uint16_t s)
{
	addModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_COLORS);

	glSetVertices(s+s);
	glSetColors(s+s);

	glLockData();

	srand(GetTickCount());
	for (uint16_t i = 0; i < s+s; i+=2)
	{
		float x = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float y = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float z = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;

		float w = 0.1f * ((float)rand()) / RAND_MAX;
		float h = 0.1f * ((float)rand()) / RAND_MAX;
		float d = 0.1f * ((float)rand()) / RAND_MAX;

		addVertex(x - w, y - h, z - d, 1.0f);
		addVertex(x + w, y + h, z + d, 1.0f);

		float r = (float)(rand()) / RAND_MAX;
		float g = (float)(rand()) / RAND_MAX;
		float b = (float)(rand()) / RAND_MAX;
		float a = (float)(rand()) / RAND_MAX;
		setColor(i, r, g, b, a);
		setColor(i+1, r, g, b, a);
	}

	glUnLockData();

	CGeometryPrimitive *p = createPrimitive(CGeometryPrimitive::LINE);

	uint16_t *points = new uint16_t[s+s];
	for (uint16_t i = 0; i < s+s; i++)
		points[i] = i;

	p->setIndexes(s+s, points);

	delete[] points;
}

class CCube2 : public CShadedGeometry
{
public:
	CCube2() { i = 0; };
	virtual ~CCube2()
	{
	};

	int i;

	virtual void glRender()
	{
		CShadedGeometry::glRender();
		i++;
		if (i < 1500)
			return;
		if (i > 3000)
			i = 0;

		GL_COORD_VERTEX baseBBox[8] =
		{ 
			GL_COORD_VERTEX(xmin, ymin, zmin, 1.0f), /* 0 */
			GL_COORD_VERTEX(xmax, ymin, zmin, 1.0f), /* 1 */
			GL_COORD_VERTEX(xmin, ymax, zmin, 1.0f), /* 2 */
			GL_COORD_VERTEX(xmax, ymax, zmin, 1.0f), /* 3 */
			GL_COORD_VERTEX(xmin, ymin, zmax, 1.0f), /* 4 */
			GL_COORD_VERTEX(xmax, ymin, zmax, 1.0f), /* 5 */
			GL_COORD_VERTEX(xmin, ymax, zmax, 1.0f), /* 6 */
			GL_COORD_VERTEX(xmax, ymax, zmax, 1.0f)  /* 7 */
		};
		GL_COORD_VERTEX extBBox[8];
		CGenericVector<float> lightVect;

		CGenericVector<float> lp(5.0f,5.0f,5.0f,1.0f);
		float ext = 5.0f;

		//  Step 2 : render extruded origin bbox
		for (unsigned int i = 0; i < 8; i++)
		{
			const GL_COORD_VERTEX& box = baseBBox[i];

			lightVect.Set(box.x - lp.X(), box.y - lp.Y(), box.z - lp.Z(), 1.0f);
			float norm = ext / sqrt(lightVect.X()*lightVect.X() + lightVect.Y()*lightVect.Y() + lightVect.Z()*lightVect.Z());
			extBBox[i].x = lightVect.X()*norm + box.x;
			extBBox[i].y = lightVect.Y()*norm + box.y;
			extBBox[i].z = lightVect.Z()*norm + box.z;
			extBBox[i].h = 1.0f;
		}

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex3fv(extBBox[0]);	glVertex3fv(extBBox[1]); glVertex3fv(extBBox[3]); glVertex3fv(extBBox[2]);
			glVertex3fv(extBBox[4]);	glVertex3fv(extBBox[5]); glVertex3fv(extBBox[7]); glVertex3fv(extBBox[6]);
			glVertex3fv(extBBox[2]);	glVertex3fv(extBBox[3]); glVertex3fv(extBBox[7]); glVertex3fv(extBBox[6]);
			glVertex3fv(extBBox[0]);	glVertex3fv(extBBox[4]); glVertex3fv(extBBox[5]); glVertex3fv(extBBox[1]);
			glVertex3fv(extBBox[0]);	glVertex3fv(extBBox[2]); glVertex3fv(extBBox[6]); glVertex3fv(extBBox[4]);
			glVertex3fv(extBBox[3]);	glVertex3fv(extBBox[1]); glVertex3fv(extBBox[5]); glVertex3fv(extBBox[7]);
		glEnd();
		/*
		glBegin(GL_QUADS);
			glVertex3fv(baseBBox[0]);	glVertex3fv(baseBBox[1]); glVertex3fv(baseBBox[3]); glVertex3fv(baseBBox[2]);
			glVertex3fv(baseBBox[4]);	glVertex3fv(baseBBox[5]); glVertex3fv(baseBBox[7]); glVertex3fv(baseBBox[6]);
			glVertex3fv(baseBBox[2]);	glVertex3fv(baseBBox[3]); glVertex3fv(baseBBox[7]); glVertex3fv(baseBBox[6]);
			glVertex3fv(baseBBox[0]);	glVertex3fv(baseBBox[4]); glVertex3fv(baseBBox[5]); glVertex3fv(baseBBox[1]);
			glVertex3fv(baseBBox[0]);	glVertex3fv(baseBBox[2]); glVertex3fv(baseBBox[6]); glVertex3fv(baseBBox[4]);
			glVertex3fv(baseBBox[3]);	glVertex3fv(baseBBox[1]); glVertex3fv(baseBBox[5]); glVertex3fv(baseBBox[7]);
		glEnd();
		*/
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void Init(CCube &c);

	float xmin, xmax, ymin, ymax, zmin, zmax;
};

void CCube2::Init(CCube &c)
{
	addModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_COLORS);

	glSetVertices(2);
	glSetColors(2);

	glLockData();
	
	GL_COORD_VERTEX m, M;
	c.getBoundingBox(m,M);

	xmin = m.x;
	ymin = m.y;
	zmin = m.z;
	xmax = M.x;
	ymax = M.y;
	zmax = M.z;

	addVertex(m.x, m.y, m.z, 1.0f);
	addVertex(M.x, M.y, M.z, 1.0f);
	float r = (float)(rand()) / RAND_MAX;
	float g = (float)(rand()) / RAND_MAX;
	float b = (float)(rand()) / RAND_MAX;
	float a = (float)(rand()) / RAND_MAX;
	setColor(0, r, g, b, a);
	setColor(1, r, g, b, a);

	glUnLockData();

	CGeometryPrimitive *p = createPrimitive(CGeometryPrimitive::LINE);

	uint16_t *points = new uint16_t[2];
	for (uint16_t i = 0; i < 2; i++)
		points[i] = i;

	p->setIndexes(2, points);

	delete[] points;
}

CTestDoc::CTestDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay *pDisplay)
{
    m_pDisplay = pDisplay;
    m_wnd = wnd;
}

CTestDoc::~CTestDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTestDoc commands
//	Default virtual implementation
void CTestDoc::GLInitContext(void)
{
	CAnimator::SetAnimator(new CAnimator());
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = dsp->getRenderingProperties();
	props.setLighting(IRenderingProperties::ENABLE);
	props.setTexturing(IRenderingProperties::ENABLE);
	props.setMultisampling(IRenderingProperties::ENABLE);

    CLight *pLight = new CLight;
    pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
    pLight->setLightPosition(GL_COORD_VERTEX(0.0f,0.0f,10.0f,1.0f));
    pLight->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,-5.0f,1.0f));
    pLight->glActivate();
	
	IViewPoint *vp = dsp->getViewPoint();
    vp->setPosition(0,0,3.5,IViewPoint::EYE);
    vp->setPosition(0,0,0,IViewPoint::TARGET);

	CViewModifier *vm = new CViewModifier("test6");

	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.001f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.015f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.035f;
	ty.a1 = 0.02f;
	ty.a0 = 0;
	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);
	
	CTextureFactory &tf = CTextureFactory::glGetDefaultFactory();
	ITextureObject *T = tf.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_BILINEAR);
	tf.glLoadTexture(T, "Start.tga");

	CPoints *points = new CPoints();
	{
		points->Init(1024);
		CShader *shader = points->getShader();
		COpenGLShaderStage *stage = shader->glGetOpenGLShader();
		CVertexShader *vs = stage->glGetVertexShader();
		bool res = vs->glLoadProgram(vp_src);
		CGeometryShader *gs = stage->glGetGeometryShader();
#if !defined(GL_VERSION_3_2)
		gs->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
#endif
		res = res & gs->glLoadProgram(gp_src);
		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(fp_src);
		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

		res = res & stage->glCompileShader();

		CTextureUnitSetup *ts = shader->glGetTextureUnitsSetup();
		ts->setDiffuseMap(T);
	}
	
	CCube *cube = new CCube();
	{
		cube->Init(1024);
		CShader *shader = cube->getShader();
		COpenGLShaderStage *stage = shader->glGetOpenGLShader();
		CVertexShader *vs = stage->glGetVertexShader();
		bool res = vs->glLoadProgram(vp3_src);
		CGeometryShader *gs = stage->glGetGeometryShader();
#if !defined(GL_VERSION_3_2)
		gs->setGeometry(GL_LINES, GL_LINE_STRIP, 16);
#endif
		res = res & gs->glLoadProgram(gp3_src);
		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(fp3_src);

		res = res & stage->glCompileShader();

		//CTextureUnitSetup *ts = shader->glGetTextureUnitsSetup();
		//ts->setDiffuseMap(T);
	}

	CCube2 *cube2 = new CCube2();
	{
		cube2->Init(*cube);

		CShader *shader = cube2->getShader();
		COpenGLShaderStage *stage = shader->glGetOpenGLShader();
		CVertexShader *vs = stage->glGetVertexShader();
		bool res = vs->glLoadProgram(vp3_src);
		CGeometryShader *gs = stage->glGetGeometryShader();
#if !defined(GL_VERSION_3_2)
		gs->setGeometry(GL_LINES, GL_LINE_STRIP, 16);
#endif
		res = res & gs->glLoadProgram(gp4_src);
		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(fp3_src);

		res = res & stage->glCompileShader();
		if (!res)
			res = true;
	}

	//vm->setObject(points);
	vm->setObject(cube);

	C3DScene *pScene = dsp->getRootScene();
	pScene->addObject(vm->getObject());
	//pScene->addObject(cube2);
    pScene->addLight(pLight);
}


void CTestDoc::glRender()
{
	m_pDisplay->glvkBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}

