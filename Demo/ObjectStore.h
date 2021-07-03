/***************************************************************************/
/*                                                                         */
/*  ObjectStore.h                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/



#if !defined(AFX_OBJECTSTORE_H__FF0B4668_29C4_4799_A78F_94B5A2F8A138__INCLUDED_)
#define AFX_OBJECTSTORE_H__FF0B4668_29C4_4799_A78F_94B5A2F8A138__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Raptordll.h"


class CObjectStore  
{
public:
	CObjectStore();
	virtual ~CObjectStore();

	void SetPerf(bool lowdef = false);

	void LoadPack(void);
	void LoadModels(void);
	void BuildObjects(void);
	void BuildScene(void);

	CLight	*const *getLights() const { return &m_pLights[0]; };

private:
	CTextureSet			*m_textures;
	CMaterial			*m_material;
	CLight				*m_pLights[9];
    CProjector          *m_projector;
    CShader             *m_pRoofShader;

	C3DSet				*m_ground;
    CObject3DInstance   *m_groundInstance;
	C3DSet				*m_groundTile;
	C3DSet				*m_groundTileLow;
	C3DSet				*m_column;
    C3DSet				*m_columnLow;
	C3DSet				*m_autel;
	C3DSet				*m_stair;
	C3DSet				*m_stairs;
	C3DSet				*m_torch;
	C3DSet				*m_roof;
	CGeometry			*m_topRoof;
	CShadedGeometry		*m_knot;

	CObject3DShadow		*m_knotShadow;
    CObject3DShadow		*m_torchShadow;

	CBumppedGeometry	*m_pBumpKnot;

	void MergeObjects(void);
	bool IsAStair(CGeometry *g);
	bool IsAColumn(CGeometry *&g);
	bool IsAGround(CGeometry *g);
	bool IsAGroundLow(CGeometry *g);
	bool IsARoof(CGeometry *g);
	void GenTexCoords(CGeometry *g);
    bool loadColumn();
};

#endif // !defined(AFX_OBJECTSTORE_H__FF0B4668_29C4_4799_A78F_94B5A2F8A138__INCLUDED_)
