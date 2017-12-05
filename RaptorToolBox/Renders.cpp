#include "StdAfx.h"


#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
	#include "RaptorToolBox.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif


void CRaptorToolBox::RenderNormals(CGeometry *g,const CColor::RGBA &c,float size)
{
	GLint mode;
	glGetIntegerv(GL_POLYGON_MODE,&mode);
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    float current[4];
    glGetFloatv(GL_CURRENT_COLOR,current);
    glColor4fv(c);
	glBegin(GL_LINES);

	for (unsigned int i=0;i<g->nbVertex();i++)
	{
		GL_VERTEX_DATA v;

		g->getVertex(i,v);

		glVertex3fv((float*)&v.vertex);
		glVertex3f(	v.vertex.x + size * v.normal.x,
					v.vertex.y + size * v.normal.y,
					v.vertex.z + size * v.normal.z);
	}

	glEnd();

	glPopAttrib();
	glPolygonMode(GL_FRONT_AND_BACK,mode);
    glColor4fv(current);
}

