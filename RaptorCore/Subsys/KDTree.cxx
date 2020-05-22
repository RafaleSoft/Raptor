// KDTree.cpp: implementation of the KDTree class.
//
//////////////////////////////////////////////////////////////////////
#if !defined __KDTREE_CXX__
#define __KDTREE_CXX__

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_KDTREE_H__2BD065C8_BF3F_4AA1_BD52_C072ADEC3A53__INCLUDED_)
	#include "KDTree.h"
#endif

RAPTOR_NAMESPACE

template <class USER_DATA_t>
CKDTree<USER_DATA_t>::CKDTree(void)
	:m_pRoot(NULL),m_blocSize(4),m_nbLevels(2)
{
}

template <class USER_DATA_t>
CKDTree<USER_DATA_t>::~CKDTree(void)
{
	if (m_pRoot != NULL)
		delete [] m_pRoot;
}

template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::addObject(CObject3D * const obj, USER_DATA_t userData)
{
	//! Manage the bbox
    if (CBaseTree<USER_DATA_t>::m_objects.size() == 0)
        bbox.operator=(*obj->boundingBox());
    else
        bbox.add(*obj->boundingBox());

	BASETREE_ITEM item;
	item.pObject = obj;
	item.userData = userData;
	CBaseTree<USER_DATA_t>::m_objects.push_back(item);
}

template <class USER_DATA_t>
vector<RAPTOR_TYPENAME CBaseTree<USER_DATA_t>::TREE_ITEM> CKDTree<USER_DATA_t>::getObjects(void) const
{
	vector<BASETREE_ITEM> res = CBaseTree<USER_DATA_t>::m_objects;
	return res;
}


template <class USER_DATA_t>
vector< RAPTOR_TYPENAME CBaseTree<USER_DATA_t>::TREE_ITEM > CKDTree<USER_DATA_t>::glClip(bool scanModelview) const
{
    C3DEngine *engine = C3DEngine::Get3DEngine();
    vector<BASETREE_ITEM> res;
    
    C3DEngine::CLIP_RESULT clipResult = engine->glClip(&m_pRoot[0].bbox,scanModelview); //,NULL);

    if ( clipResult == C3DEngine::CLIP_FULL)
        return res;
    
    if (clipResult == C3DEngine::CLIP_NONE)
    {
        res = CBaseTree<USER_DATA_t>::m_objects;
    }
    else    // CLIP_PARTS
    {
		/*
		int ii = (int)pow(2.0,(int)(m_nbLevels-1));

		int j = 0;
//#pragma omp parallel for default(none) private(j) shared(ii,engine)
		for (j=0;j<ii;j++)
		{
			int idKd = ii + j - 1;
			KdLine& line = m_pRoot[idKd];
			// Here, we miss all median objects
			if (C3DEngine::CLIP_FULL != engine->glClip(&line.bbox,false))
			{
				for (int i=line.Imin;i<line.Imax;i++)
					res.push_back(m_objects[i]);
			}
		}
		*/
		res.push_back(CBaseTree<USER_DATA_t>::m_objects[m_pRoot[0].idCoup]);
		for (int i=1;i<m_nbLevels;i++)
		{
			int ii = (int)pow(2.0,i);
			for (int j=0;j<ii;j++)
			{
				int idKd = ii + j - 1;
				KdLine& line = m_pRoot[idKd];
				
				if (C3DEngine::CLIP_FULL != engine->glClip(&line.bbox,false))
				{
					res.push_back(CBaseTree<USER_DATA_t>::m_objects[line.idCoup]);
					
				}
			}
		}
    }

	return res;
}

template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::calculParamKd(int *NVC,int *NBI)
{
	int Dimension = CBaseTree<USER_DATA_t>::m_objects.size();
	int nvc = 1;
	int n = 1;
	
	do
	{
		int Dim_temp = Dimension / 2;
		if (Dim_temp > (Dimension-Dim_temp)-1)
			Dimension = Dim_temp;
		else
			Dimension = (Dimension-Dim_temp)-1;

		if (Dimension >= m_blocSize)
		{
			nvc += (int) pow(2.0,n);
			n++;
		}
	} while (Dimension > m_blocSize);

	*NBI = (int)(pow(2.0,n));
	m_nbLevels = n;
	*NVC = nvc;
	m_blocSize = Dimension;
}

static float (RAPTOR_FASTCALL CBoundingBox::*fptr[6])(void) const = 
{
	&CBoundingBox::xMin,
	&CBoundingBox::xMax,
	&CBoundingBox::yMin,
	&CBoundingBox::yMax,
	&CBoundingBox::zMin,
	&CBoundingBox::zMax
};

template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::medianI(int i1,int i2,int coupure,int dim)
{
	int min = i1; 
	int max = i2 ;

	while (min < max) 
	{
		int i = min;
		int j = max;

		float x = 0.5f * (	(CBaseTree<USER_DATA_t>::m_objects[coupure].pObject->boundingBox()->*fptr[dim])() + 
							(CBaseTree<USER_DATA_t>::m_objects[coupure].pObject->boundingBox()->*fptr[dim+3])());

		do 
		{
			// x1 and x2 store temporary results to be comparable due to float rounding errors.
			float x1 = 0;
			while ( x > (x1 = (0.5f * ((CBaseTree<USER_DATA_t>::m_objects[i].pObject->boundingBox()->*fptr[dim])() + 
								(CBaseTree<USER_DATA_t>::m_objects[i].pObject->boundingBox()->*fptr[dim+3])()))))
				i++ ;

			float x2 = 0;
			while ( x < (x2 = (0.5f * ((CBaseTree<USER_DATA_t>::m_objects[j].pObject->boundingBox()->*fptr[dim])() + 
								(CBaseTree<USER_DATA_t>::m_objects[j].pObject->boundingBox()->*fptr[dim+3])()))))
				j-- ;

			if (i <= j) 
			{
				BASETREE_ITEM tmp = CBaseTree<USER_DATA_t>::m_objects[i];
				CBaseTree<USER_DATA_t>::m_objects[i] = CBaseTree<USER_DATA_t>::m_objects[j];
				CBaseTree<USER_DATA_t>::m_objects[j] = tmp;

				i++ ;
				j-- ;
			}
			else if ((j < i) && (i <= coupure))
			{
				BASETREE_ITEM tmp = CBaseTree<USER_DATA_t>::m_objects[i];
				CBaseTree<USER_DATA_t>::m_objects[i] = CBaseTree<USER_DATA_t>::m_objects[i+1];
				CBaseTree<USER_DATA_t>::m_objects[i+1] = tmp;
				i++ ;
			}
		} while (i <= j) ;

		if (j < coupure) min = i ;
		if (coupure < i) max = j ;
	}
}


template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::ConstructionKD(void)
{
	// 2- Construction de la structure de stockage de l'arbre
	// Le tableau Arbre contient Val*NVC valeurs
	int Dim = CBaseTree<USER_DATA_t>::m_objects.size();
	int Axe = 0;	// selection successive des axes X, Y & Z

	int temp = Dim / 2; 
	if ((Dim & 1) == 0) temp--;

	m_pRoot[0].idCoup = temp;
	m_pRoot[0].Imin = 0;
	m_pRoot[0].Imax = Dim - 1;

	medianI(m_pRoot[0].Imin,m_pRoot[0].Imax,m_pRoot[0].idCoup,Axe);
	m_pRoot[0].bbox = bbox;

	for (int i=1;i<m_nbLevels;i++)
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;
		int ii = (int)pow(2.0,i);

		int j=0;
#pragma omp parallel for default(none) private(j,i1,i2,i3,i4) shared(/*m_objects,/*m_pRoot,*/i,ii,Axe)
		// Boucle de plus bas niveau
		for (j=0;j<ii;j+=2)
		{
			int idKd = ii + j - 1;

			// Recuperation des indices du niveau precedent
			int Bip = m_pRoot[idKd/2].Imin;
			int Bfp = m_pRoot[idKd/2].Imax;
			int I_med = m_pRoot[idKd/2].idCoup;

			// Decoupage de [Bip,Bfp] en [i1,i2](I_med)[i3,i4]									
			i1 = Bip;
			i2 = I_med - 1;
			i3 = I_med + 1;
			i4 = Bfp;
			
			int I_med2 = (i2-i1+1)/2;				// Determination de l'index de la 
			if (((i2-i1+1) &1 )==0) I_med2--;		// "future" mediane de [i1,i2]
			I_med2 = I_med2 + i1;
			
			m_pRoot[idKd].idCoup = I_med2;			// Index de la Valeur de coupure
			m_pRoot[idKd].Imin = i1;				// màj des indices
			m_pRoot[idKd].Imax = i2;				// de debut et fin
			// C'est la partie qui conserve le point Xm,Ym et màj de XM ou YM

			int I_med3 = (i4-i3+1)/2;				// Determination de l'index de la
			if (((i4-i3+1) &1 )==0) I_med3--;		// "future" mediane de [i3,i4]
			I_med3 = I_med3 + i3;

			m_pRoot[idKd+1].idCoup = I_med3;		// Index de la valeur de coupure
			m_pRoot[idKd+1].Imin = i3;				// màj des indices
			m_pRoot[idKd+1].Imax = i4;				// de debut et fin
			// C'est la partie qui conserve le point XM,YM et màj de Xm ou Ym

			medianI(i1,i2,I_med2,Axe);				// Repartition des valeurs
			medianI(i3,i4,I_med3,Axe);				// Repartition des valeurs

			// Mise à jour de la BB
			m_pRoot[idKd].bbox = *CBaseTree<USER_DATA_t>::m_objects[Bip].pObject->boundingBox();
			for (int k=Bip+1;k<I_med;k++)
				m_pRoot[idKd].bbox.add(*CBaseTree<USER_DATA_t>::m_objects[k].pObject->boundingBox());

			m_pRoot[idKd+1].bbox = *CBaseTree<USER_DATA_t>::m_objects[I_med].pObject->boundingBox();
			for (int k=I_med+1;k<Bfp;k++)
				m_pRoot[idKd+1].bbox.add(*CBaseTree<USER_DATA_t>::m_objects[k].pObject->boundingBox());

		} // j

		Axe = (Axe+1)%3;
	} // Etapes	
}


template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::compress(void)
{
	int NVC = 0;
	int NBI = 0;

	// On calcule les paramètres de fabrication du Kd-tree
	calculParamKd(&NVC,&NBI);

	// Fabrication du kd-tree (vide) en mémoire
	m_pRoot = new KdLine[NVC];

	// Initialisation du kd-Tree
	for (int i=0;i<NVC;i++)
	{		
		m_pRoot[i].idCoup = 0;
		m_pRoot[i].Imin = 0;
		m_pRoot[i].Imax = 0;
	}

	ConstructionKD();
}

template <class USER_DATA_t>
void CKDTree<USER_DATA_t>::glRender(void)
{
    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glColor4f(1.0f,0.0f,0.0f,1.0f);

	float xm,xM,ym,yM,zm,zM;
	bbox.get(xm,ym,zm,xM,yM,zM);
    glBegin(GL_QUADS);
		glVertex3f(xm,ym,zm);	glVertex3f(xM,ym,zm);	glVertex3f(xM,yM,zm);	glVertex3f(xm,yM,zm);
	    glVertex3f(xm,ym,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,yM,zM);	glVertex3f(xm,yM,zM);
	    glVertex3f(xm,yM,zm);	glVertex3f(xm,yM,zM);	glVertex3f(xM,yM,zM);	glVertex3f(xM,yM,zm);
	    glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,ym,zm);
	    glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);	glVertex3f(xm,yM,zM);	glVertex3f(xm,yM,zm);
	    glVertex3f(xM,yM,zm);	glVertex3f(xM,yM,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,ym,zm);
	glEnd();

	glColor4f(1.0f,1.0f,0.0f,1.0f);

//	count++;
//	if (count > 20)
//	{
//		count = 0;
//		maxboxes++;
//		if (maxboxes > 15)
//			maxboxes = 0;
//	}

//	int boxes = 0;
	for (int i=1;i<m_nbLevels;i++)
	{
		if (i == 1)
			glColor4f(1.0f,0.0f,0.0f,1.0f);
		else if (i == 2)
			glColor4f(1.0f,1.0f,0.0f,1.0f);
		else if (i == 3)
			glColor4f(0.0f,1.0f,1.0f,1.0f);
		else if (i == 4)
			glColor4f(0.0f,1.0f,0.0f,1.0f);

		int ii = (int)pow(2.0,i);
		for (int j=0;j<ii;j++)
//			if ((boxes < maxboxes)&&(i==4))
		{
			int idKd = ii + j - 1;
			m_pRoot[idKd].bbox.get(xm,ym,zm,xM,yM,zM);
			glBegin(GL_QUADS);
				glVertex3f(xm,ym,zm);	glVertex3f(xM,ym,zm);	glVertex3f(xM,yM,zm);	glVertex3f(xm,yM,zm);
				glVertex3f(xm,ym,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,yM,zM);	glVertex3f(xm,yM,zM);
				glVertex3f(xm,yM,zm);	glVertex3f(xm,yM,zM);	glVertex3f(xM,yM,zM);	glVertex3f(xM,yM,zm);
				glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,ym,zm);
				glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);	glVertex3f(xm,yM,zM);	glVertex3f(xm,yM,zm);
				glVertex3f(xM,yM,zm);	glVertex3f(xM,yM,zM);	glVertex3f(xM,ym,zM);	glVertex3f(xM,ym,zm);
			glEnd();
//			boxes++;
		}
	}

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glPopAttrib();
}

#endif

