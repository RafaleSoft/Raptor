#pragma once


//#include "Raptordll.h"

namespace RaysClient {
	
	class RaysClientData
	{
	public:
		RaysClientData();
		virtual ~RaysClientData();

		//!	Initialise Raptor API for data loading
		bool initDataloader(void);

		//!	Load XML Raptor scene model
		bool LoadData(const char* fname);
		

	private:
		bool	m_bInitialised;
		/*
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
		CBumppedGeometry	*m_columnBump;
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
		*/
	};
}
