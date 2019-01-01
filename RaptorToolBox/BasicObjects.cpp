// BasicObjects.cpp: implementation of the CBasicObjects class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_BASICOBJECTS_H__2E0B717D_862A_4096_9D99_8EE0654F4678__INCLUDED_)
	#include "BasicObjects.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GLHierarchy/GeometryEditor.h"
#endif

#include "RaptorToolBox.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBasicObjects::CCube::CCube()
{
	CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(l_model);
	glSetVertices(24);
	glSetTexCoords(24);
	glSetPolygons(12);

    glLockData();

  // front face
	addVertex(-1.0f,-1.0f,1.0f,1.0f);
	addVertex(1.0f,-1.0f,1.0f,1.0f);
	addVertex(1.0f,1.0f,1.0f,1.0f);
	addVertex(-1.0f,1.0f,1.0f,1.0f);
    addFace(0,1,2);
    addFace(2,3,0);
	setTexCoord(0,0,0);
	setTexCoord(1,1,0);
	setTexCoord(2,1,1);
	setTexCoord(3,0,1);

  // up face
    addVertex(-1.0f,1.0f,1.0f,1.0f);
	addVertex(1.0f,1.0f,1.0f,1.0f);
	addVertex(1.0f,1.0f,-1.0f,1.0f);
	addVertex(-1.0f,1.0f,-1.0f,1.0f);
    addFace(4,5,6);
    addFace(6,7,4);
	setTexCoord(4,0,0);
	setTexCoord(5,1,0);
	setTexCoord(6,1,1);
	setTexCoord(7,0,1);

  // right face
    addVertex(1.0f,-1.0f,1.0f,1.0f);
	addVertex(1.0f,-1.0f,-1.0f,1.0f);
	addVertex(1.0f,1.0f,-1.0f,1.0f);
	addVertex(1.0f,1.0f,1.0f,1.0f);
    addFace(8,9,10);
    addFace(10,11,8);
	setTexCoord(8,0,0);
	setTexCoord(9,1,0);
	setTexCoord(10,1,1);
	setTexCoord(11,0,1);

  // left face
    addVertex(-1.0f,-1.0f,-1.0f,1.0f);
	addVertex(-1.0f,-1.0f,1.0f,1.0f);
	addVertex(-1.0f,1.0f,1.0f,1.0f);
	addVertex(-1.0f,1.0f,-1.0f,1.0f);
    addFace(12,13,14);
    addFace(14,15,12);
	setTexCoord(12,0,0);
	setTexCoord(13,1,0);
	setTexCoord(14,1,1);
	setTexCoord(15,0,1);

  // bottom face
    addVertex(-1.0f,-1.0f,-1.0f,1.0f);
	addVertex(1.0f,-1.0f,-1.0f,1.0f);
	addVertex(1.0f,-1.0f,1.0f,1.0f);
	addVertex(-1.0f,-1.0f,1.0f,1.0f);
    addFace(16,17,18);
    addFace(18,19,16);
	setTexCoord(16,0,0);
	setTexCoord(17,1,0);
	setTexCoord(18,1,1);
	setTexCoord(19,0,1);

  // back face
    addVertex(1.0f,-1.0f,-1.0f,1.0f);
	addVertex(-1.0f,-1.0f,-1.0f,1.0f);
	addVertex(-1.0f,1.0f,-1.0f,1.0f);
	addVertex(1.0f,1.0f,-1.0f,1.0f);
    addFace(20,21,22);
    addFace(22,23,20);
	setTexCoord(20,0,0);
	setTexCoord(21,1,0);
	setTexCoord(22,1,1);
	setTexCoord(23,0,1);

	const CGeometryEditor &pEditor = getEditor();
	pEditor.genNormals(true);

    glUnLockData();
}

CBasicObjects::CCube::~CCube()
{
}

void CBasicObjects::CCube::setDimensions(float width,float height,float depth)
{
	float w = width;
	float h = height;
	float d = depth;
	if (w <= 0)
		w = 1.0f;
	if (h <= 0)
		h = 1.0f;
	if (d <= 0)
		d = 1.0f;

    glLockData();


  // front face
	setCoord(0,-w,-h,d,1.0f);
	setCoord(1,w,-h,d,1.0f);
	setCoord(2,w,h,d,1.0f);
	setCoord(3,-w,h,d,1.0f);
    

  // up face
    setCoord(4,-w,h,d,1.0f);
	setCoord(5,w,h,d,1.0f);
	setCoord(6,w,h,-d,1.0f);
	setCoord(7,-w,h,-d,1.0f);
    

  // right face
    setCoord(8,w,-h,d,1.0f);
	setCoord(9,w,-h,-d,1.0f);
	setCoord(10,w,h,-d,1.0f);
	setCoord(11,w,h,d,1.0f);
    

  // left face
    setCoord(12,-w,-h,-d,1.0f);
	setCoord(13,-w,-h,d,1.0f);
	setCoord(14,-w,h,d,1.0f);
	setCoord(15,-w,h,-d,1.0f);
    

  // bottom face
    setCoord(16,-w,-h,-d,1.0f);
	setCoord(17,w,-h,-d,1.0f);
	setCoord(18,w,-h,d,1.0f);
	setCoord(19,-w,-h,d,1.0f);
    

  // back face
    setCoord(20,w,-h,-d,1.0f);
	setCoord(21,-w,-h,-d,1.0f);
	setCoord(22,-w,h,-d,1.0f);
	setCoord(23,w,h,-d,1.0f);
    
	const CGeometryEditor &pEditor = getEditor();
	pEditor.genNormals(true);

    glUnLockData();
}



CBasicObjects::CIsocahedron::CIsocahedron()
{
	CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(l_model);
	glSetVertices(12);
	glSetPolygons(20);

	glLockData();

	float X = 0.57f;
	float Z	= 0.85f;

	addVertex(-X, 0.0f, Z, 1.0f);
	addVertex( X, 0.0f, Z, 1.0f);
	addVertex(-X, 0.0f,-Z, 1.0f);
	addVertex( X, 0.0f,-Z, 1.0f);
	addVertex(0.0f, Z, X,1.0f);
	addVertex(0.0f, Z,-X,1.0f);
	addVertex(0.0f,-Z, X,1.0f);
	addVertex(0.0f,-Z,-X,1.0f);
	addVertex( Z, X, 0.0f, 1.0f);
	addVertex(-Z, X, 0.0f, 1.0f);
	addVertex( Z,-X, 0.0f, 1.0f);
	addVertex(-Z,-X, 0.0f, 1.0f);

	addFace(0, 1, 4);
	addFace(0, 6, 1);
	addFace(0, 11, 6);
	addFace(0, 9, 11);
	addFace(0, 4, 9);
	addFace(8, 4, 1);
	addFace(1, 10, 8);
	addFace(10, 1, 6);
	addFace(6, 7, 10);
	addFace(7, 6, 11);
	addFace(11, 2, 7);
	addFace(2, 11, 9);
	addFace(9, 5, 2);
	addFace(5, 9, 4);
	addFace(4, 8, 5);
	addFace(3, 8, 10);
	addFace(3, 10, 7);
	addFace(3, 7, 2);
	addFace(3,2,5);
	addFace(3,5,8);

	const CGeometryEditor &pEditor = getEditor();
	pEditor.genNormals(true);

    glUnLockData();
}

CBasicObjects::CIsocahedron::~CIsocahedron()
{
}

void CBasicObjects::CIsocahedron::setDimensions(float radius, unsigned int nbLevel)
{
	if (nbLevel > 0)
	{
		glLockData();

		unsigned int nbVrtx = 0;
		unsigned int nbFc = 0;
		unsigned int i = 0;
		for (i=0 ; i<nbLevel ; i++)
			nbVrtx += pow(4.0f,(float)i);
		nbVrtx = nbVrtx * 3 * 20 + 12;
		nbFc = pow(4.0f,(float)nbLevel) * 20;

		vector<CGenericVector<float> > newCoords;
		vector<unsigned int> newFaces;


		unsigned int j=0;
		for (j=0;j<nbVertex();j++)
		{
			CGenericVector<float> v(VERTEX(j));
			float n = v.Norm();
			v *= radius / n;
			v.H(1.0f);
			newCoords.push_back(v);
		}
		for (i=0;i<nbFace();i++)
		{
			unsigned int p1,p2,p3;
			getFace(i,p1,p2,p3);
			newFaces.push_back(p1);
			newFaces.push_back(p2);
			newFaces.push_back(p3);
		}
		unsigned int numFaces = newCoords.size(); 

		glUnLockData();
		
		glSetPolygons(nbFc);
		glSetVertices(nbVrtx);
		glSetTexCoords(nbVrtx);

		glLockData();

		for (unsigned int level=0 ; level<nbLevel ; level++)
		{
			vector<unsigned int> tmpFaces;
			for (unsigned int i=0;i<newFaces.size()/3;i++)
			{
				unsigned int p1 = newFaces[3*i];
				unsigned int p2 = newFaces[3*i+1];
				unsigned int p3 = newFaces[3*i+2];

				CGenericVector<float> &v1 = newCoords[p1];
				CGenericVector<float> &v2 = newCoords[p2];
				CGenericVector<float> &v3 = newCoords[p3];

				CGenericVector<float> v12 = v1+v2;
				float n = v12.Norm();
				v12 *= radius / n;
				v12.H(1.0f);

				CGenericVector<float> v23 = v2+v3;
				n = v23.Norm();
				v23 *= radius / n;
				v23.H(1.0f);

				CGenericVector<float> v31 = v1+v3;
				n = v31.Norm();
				v31 *= radius / n;
				v31.H(1.0f);

				newCoords.push_back(v12);	// numFaces
				newCoords.push_back(v23);	// numFaces+1
				newCoords.push_back(v31);	// numFaces+2

				tmpFaces.push_back(p1);
				tmpFaces.push_back(numFaces);
				tmpFaces.push_back(numFaces+2);
				tmpFaces.push_back(p2);
				tmpFaces.push_back(numFaces+1);
				tmpFaces.push_back(numFaces);
				tmpFaces.push_back(p3);
				tmpFaces.push_back(numFaces+2);
				tmpFaces.push_back(numFaces+1);
				tmpFaces.push_back(numFaces);
				tmpFaces.push_back(numFaces+1);
				tmpFaces.push_back(numFaces+2);

				numFaces += 3;
			}

			newFaces = tmpFaces;
		}

		for (j=0;j<newCoords.size();j++)
			addVertex(newCoords[j].X(),newCoords[j].Y(),newCoords[j].Z(),1.0f);
		for (j=0;j<newFaces.size() / 3;j++)
			addFace(newFaces[3*j],newFaces[3*j+1],newFaces[3*j+2]);

		glUnLockData();

		const CGeometryEditor &pEditor = getEditor();
		if (!pEditor.isMinimized())
			pEditor.minimize();
		pEditor.genNormals(true);

		glLockData();

		GL_VERTEX_DATA vrtx;
		for (i=0;i<nbVertex();i++)
		{
			getVertex(i,vrtx);
			float v = 0.5f + 2 * asin(vrtx.vertex.y/radius) / PI;
			float u = atan2(vrtx.vertex.x,vrtx.vertex.z) / PI;
			setTexCoord(i,u,v);
		}

		glUnLockData();
	}
}


CBasicObjects::CGeoSphere::CGeoSphere()
{
	CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(l_model);
	setDimensions(1.0f,16,16);
}

CBasicObjects::CGeoSphere::~CGeoSphere()
{
}

void CBasicObjects::CGeoSphere::setDimensions(float radius,unsigned int nbMeridiens,unsigned int nbParallels)
{
	removePrimitive(NULL);

	//	The number of meridiens must be incremented to handle
	//	texture coordinates properly
	unsigned int NM = nbMeridiens+1;
	glSetVertices(2 + NM*nbParallels);
	glSetTexCoords(2 + NM*nbParallels);

    glLockData();

	unsigned int numvtx = 0;
	addVertex(0.0f, radius, 0.0f, 1.0f);
	setNormal(0,0.0f,1.0f,0.0f,1.0f);
	setTexCoord(numvtx++,0.0f,1.0f);
	
	for (unsigned int i=1;i<nbParallels+1;i++)
	{
		float y = radius * cos(PI*i/(nbParallels + 1.0f));
		float r = radius * sin(PI*i/(nbParallels + 1.0f));

		for (unsigned int j=0;j<NM;j++)
		{
			float x = r * cos(2.0f*PI*j/nbMeridiens);
			float z = -r * sin(2.0f*PI*j/nbMeridiens);

			addVertex(x,y,z,1.0f);

			CGenericVector<float> n(x,y,z,1.0f);
			n.Normalize();
			setTexCoord(numvtx,
						((float)j)/nbMeridiens,
						1.0f - ((float)i)/nbParallels);
			setNormal(numvtx++,n.X(),n.Y(),n.Z(),1.0f);
		}
	}

	addVertex(0.0f, -radius, 0.0f, 1.0f);
	setTexCoord(numvtx,1.0f,0.0f);
	setNormal(numvtx++,0.0f,-1.0f,0.0f,1.0f);

	CGeometryPrimitive *prim = this->createPrimitive(CGeometryPrimitive::TRIANGLE_FAN);
	unsigned short *topfaces = new unsigned short[NM+1];
	for (unsigned int k=0;k<NM+1;k++)
		topfaces[k] = k;
	prim->setIndexes(NM+1,topfaces);
	delete [] topfaces;

	//prim = this->createPrimitive(CGeometryPrimitive::QUAD);
	prim = this->createPrimitive(CGeometryPrimitive::TRIANGLE_STRIP);
	unsigned short *faces = new unsigned short[(nbMeridiens-1)*nbParallels*4];
	unsigned int offset = 0;

	for (unsigned int i=0;i<(nbParallels-1);i++)
	{
		for (unsigned int j=0;j<nbMeridiens;j++)
		{
			faces[offset++] = 1+(i*NM)+j;
			faces[offset++] = 1+(i*NM)+j+NM;
			faces[offset++] = 1 + (i*NM) + j + 1;
			faces[offset++] = 1+(i*NM)+j+NM+1;
			//faces[offset++] = 1+(i*NM)+j+1;
		}
	}
	
	prim->setIndexes(offset,faces);
	delete [] faces;

	prim = this->createPrimitive(CGeometryPrimitive::TRIANGLE_FAN);
	unsigned short *bottomfaces = new unsigned short[NM+1];
	for (unsigned int k=0;k<NM+1;k++)
		bottomfaces[k] = nbParallels*NM+1 - k;
	prim->setIndexes(NM+1,bottomfaces);
	delete [] bottomfaces;

    glUnLockData();
}
/*
void CBasicObjects::CGeoSphere::glRender(void)
{
	CShadedGeometry::glRender();

	GL_RGBA_COLOR c(1.0f,0.0f,0.0f,1.0f);
	CRaptorToolBox::RenderNormals(this,c,0.2f);
}
*/

CBasicObjects::CRectMesh::CRectMesh()
{
	setDimensions(1.0f,1.0f,50,50);

	CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(l_model);
}

CBasicObjects::CRectMesh::~CRectMesh()
{
}

void CBasicObjects::CRectMesh::setDimensions(float width,float height,unsigned int hcels,unsigned int vcels)
{
	if ((hcels < 3) || (vcels < 3))
		return;

	glSetVertices(hcels*vcels);
	glSetTexCoords(hcels*vcels);

	removePrimitive(NULL);
	CGeometryPrimitive *prim = createPrimitive(CGeometryPrimitive::TRIANGLE_STRIP);

	GLushort *indexes = new unsigned short[2 * hcels * vcels];
	memset(indexes,0,2*hcels*vcels*sizeof(GLushort));

    //	The border indexes are not used to avoid
	//	texture border artifacts
	unsigned int pos = 0;
	for (int k=0;k<int(vcels-2);k+=2)		// do not use unsigned
	{
        int i=0;
		for (i=0;i<int(hcels);i++)			// do not use unsigned
			for (int j=k+1;j>=k;j--)	// do not use unsigned
				indexes[pos++] = hcels * j + i;

		for (i=hcels-1;i>=0;i--)
			for (int j=k+1;j<k+3;j++)
				indexes[pos++] = hcels * j + i;
	}
	// Add the missing odd line
	{
        int k=vcels-2;					// do not use unsigned
		for (int i=0;i<int(hcels);i++)		// do not use unsigned
			for (int j=k+1;j>=k;j--)	// do not use unsigned
				indexes[pos++] = hcels * j + i;

	}

	prim->setIndexes(pos,indexes);
	delete [] indexes;

	glLockData();

    float stepx = 1.0f / (hcels - 1);
    float stepy = 1.0f / (vcels - 1);

	for (unsigned int j=0;j<vcels;j++)
	{
		for (unsigned int i=0;i<hcels;i++)
		{
			addVertex(	width * (i * stepx - 0.5f),
						height * (j * stepy - 0.5f),
						0.0f,
						1.0f);

			setTexCoord(hcels * j + i,
						i * stepx,
						j * stepy);
			setNormal(i+j*hcels,0.0f,0.0f,1.0f,1.0f);
		}
	}

	glUnLockData();
}

CBasicObjects::CRectangle::CRectangle()
{
	glSetVertices(4);
	glSetPolygons(2);
	glSetTexCoords(4);
	glLockData();
		addVertex(-0.5f,-0.5f,0.0f,1.0f);
		addVertex(0.5f,-0.5f,0.0f,1.0f);
		addVertex(0.5f,0.5f,0.0f,1.0f);
		addVertex(-0.5f,0.5f,0.0f,1.0f);

		setTexCoord(0,0,0);
		setTexCoord(1,1,0);
		setTexCoord(2,1,1);
		setTexCoord(3,0,1);

		addFace(0,1,2);
		addFace(2,3,0);
	glUnLockData();

	CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(l_model);
}

CBasicObjects::CRectangle::~CRectangle()
{
}

void CBasicObjects::CRectangle::setDimensions(float width,float height)
{
	glLockData();
		setCoord(0,-width*0.5f,-height*0.5f,0.0f,1.0f);
		setCoord(1,width*0.5f,-height*0.5f,0.0f,1.0f);
		setCoord(2,width*0.5f,height*0.5f,0.0f,1.0f);
		setCoord(3,-width*0.5f,height*0.5f,0.0f,1.0f);
	glUnLockData();
}