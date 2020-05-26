#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif

#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GLHierarchy/GeometryEditor.h"
#endif

#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif

#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
	#include "RaptorToolBox.h"
#endif


RAPTOR_NAMESPACE

CGeometry *CRaptorToolBox::mergeGeometries(C3DSet *set)
{
	if (set == NULL)
		return NULL;

    C3DSet::C3DSetIterator it = set->getIterator();
    if (set->getChild(it) == NULL)
        return NULL;

	unsigned int nbVertex = 0;
	unsigned int nbPolys = 0;

	// first step : count vertex and polys for allocation
	CObject3D *o = set->getChild(it++);
	CGeometry *g = NULL;

	while (o != NULL)
	{
		if (o->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
		{
			g = (CGeometry *)o;
			nbVertex += g->nbVertex();
			nbPolys += g->nbFace();
		}

		o = set->getChild(it++);
	}


	//	Allocation
	CGeometry *res = new CGeometry(set->getName());

	res->glSetVertices(nbVertex);
	res->glSetPolygons(nbPolys);

	unsigned int offset = 0;

	//	Second pass, transfer datas.
    it = set->getIterator();
	o = set->getChild(it++);
	while (o != NULL)
	{
		if (o->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
		{
			g = (CGeometry *)o;
			unsigned int nbV = g->nbVertex();
			unsigned int nbP = g->nbFace();

			GL_VERTEX_DATA	v;
			CColor::RGBA	c;
            
            unsigned int i=0;
			for (i=0;i<nbV;i++)
			{
				g->getVertex(i,v);
				res->addVertex(v.vertex.x,v.vertex.y,v.vertex.z,v.vertex.h);
				res->setTexCoord(i+offset,v.texCoord0.u,v.texCoord0.v);
#ifdef GL_EXT_fog_coord
				res->setFogCoord(i+offset,v.fog);
#endif
#ifdef GL_EXT_vertex_weighting
				res->setWeight(i+offset,v.weight);
#endif

				g->getColor(i,c);
				res->setColor(i+offset,c.r,c.g,c.b,c.a);
			}

			unsigned int p1,p2,p3;

			for (i=0;i<nbP;i++)
			{
				g->getFace(i,p1,p2,p3);
				res->addFace(p1+offset,p2+offset,p3+offset);
			}

			offset += nbV;
		}

		o = set->getChild(it++);
	}

	const CGeometryEditor &pEditor = res->getEditor();
	pEditor.genNormals();

	return res;
}



ITextureObject* CRaptorToolBox::mergeTextures(	CTextureSet *t,
												unsigned int width, unsigned int height,
												vector<GL_COORD_VERTEX> &placements)
{
	if (t == NULL)
		return NULL;

	size_t nb = placements.size();
	if (nb == 0)
		return NULL;

	unsigned int powx = 1;
	unsigned int powy = 1;

	while (powx<width)
		powx*=2;	
	while (powy<height)
		powy*=2;

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	CTextureObject* T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,ITextureObject::CGL_OPAQUE);
    T->setSize(powx,powy);
	T->glSetTransparency(255);
    
	CImage merge;
	merge.allocatePixels(powx, powy);
	unsigned int *result = (unsigned int*)merge.getPixels();

	if (t->getNbTexture() < nb)
		nb = t->getNbTexture();

	for (size_t i=0;i<nb;i++)
	{
		ITextureObject* current = t->getTexture(i);

		current->glvkRender();

		GLuint *buffer = new GLuint[current->getWidth()*current->getHeight()*4];

		glGetTexImage(	GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,buffer);

		int width_max = current->getWidth();
		int height_max = current->getHeight();

		if (placements[i].x + width_max > width)
			width_max = width - placements[i].x;
		if (placements[i].y + height_max > height)
			height_max = height - placements[i].y;

		int posx = placements[i].x;
		int posy = placements[i].y;

		for (int k=0;k<height_max;k++)
		{
			for (int j=0;j<width_max;j++)
			{
				result[(k+posy)*width+posx+j] = buffer[j+k*current->getWidth()];
			}
		}

		delete [] buffer;
	}

	T->glvkRender();
	factory.glLoadTexture(T,merge);
	return T;
}


void CRaptorToolBox::mergeGeometryTexCoords( C3DSet *set,
																				unsigned int width, unsigned int height,
																				vector<GL_COORD_VERTEX> &placements)
{
	if ((set == NULL) || (placements.size() == 0))
		return ;

    C3DSet::C3DSetIterator it = set->getIterator();
    if (set->getChild(it) == NULL)
        return;

	size_t pos = 0;

	CObject3D *o = set->getChild(it++);
	while (o != NULL)
	{
		if ((o->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId())) &&
			(pos < placements.size()))
		{
			CGeometry *g = (CGeometry *)o;
			unsigned int nbV = g->nbVertex();

			GL_VERTEX_DATA	v;
			float		tWidth = placements[pos].z;
			float		tHeight = placements[pos].h;

			float		w_factor = tWidth / width;
			float		h_factor = tHeight / height;
			float		x_offset = placements[pos].x / width;
			float		y_offset = placements[pos].y / height;

			float tx = 0;
			float ty = 0;

			for (unsigned int i=0;i<nbV;i++)
			{
				g->getVertex(i,v);

				tx = v.texCoord0.u - floor(v.texCoord0.u);
				ty = v.texCoord0.v - floor(v.texCoord0.v);

				g->setTexCoord(	i,
								x_offset + w_factor * tx,
								y_offset + h_factor * ty);
			}
		}

		pos++;
		o = set->getChild(it++);
	}
}
