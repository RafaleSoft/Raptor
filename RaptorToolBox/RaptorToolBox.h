// RaptorToolBox.h: interface for the CRaptorToolBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
#define AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raptordll.h"

RAPTOR_NAMESPACE

#include <vector>


class RAPTOR_API CRaptorToolBox  
{
public:

	// Renders only the normals of the parameter g.
	// Object's colors are used.
	// Normals are scaled using size parameter
    static void RenderNormals(CGeometry *g,const CColor::RGBA &c,float size = 1.0f);


	// Prepare geometries to use a merged texture.
	// Except the C3DSet parameters are the same as mergeTextures
	//	placement data:
	//	- x = x offset of texture
	//	- y = y offset of texture
	//	- z = texture width
	//	- h = texture height
    static void mergeGeometryTexCoords(	C3DSet *set,
										unsigned int width, unsigned int height,
										vector<GL_COORD_VERTEX> &placements);


	// Merge geometry datas.
	//	set must be C3DSet of CGeometry 'only' objects.
	//	They are all merged to a new CGeometry which is returned.
    static CGeometry *mergeGeometries(C3DSet *set);


	// Merge texture objects given in t generating
	// a texture_object sized width*height in RGBA of course,
	// placing as many images as requested in the array
	// placement ( provided they are available in t)
	//	placement data:
	//	- x = x offset of texture
	//	- y = y offset of texture
	//
	//	The GL context where texture are loaded must
	//	be the current active context
	//
	// The final <result> object is raw : it is ready to be bound in
	// a CTexture object using LoadTexture(<result>,".buffer")
    static ITextureObject *mergeTextures(	CTextureSet *t,
											unsigned int width, unsigned int height,
											vector<GL_COORD_VERTEX> &placements);


	//////////////////////////////////////////////////////////////////////
	//	
	//	Scene loaders:
	//	- VRML
	//	- 3DS
	//	- OBJ
	//
	//	Options structure
	typedef struct SCENE_LOADER_OPTIONS
	{
		std::string	texturePath;
		bool		useMaxTransform;
        bool		compressTextures;
		const char* const * discardGeometry;

		SCENE_LOADER_OPTIONS()
            :discardGeometry(NULL),
            useMaxTransform(false),compressTextures(false)
		{	}
	} SCENE_LOADER_OPTIONS;
	typedef SCENE_LOADER_OPTIONS *LP_SCENE_LOADER_OPTIONS;
	//
	//	VRML
	//	Exports a Raptor scene to a VRML2 file
    static bool exportVRML(const std::string& filename,C3DSet *scene);
	//
	//	Load a scene from a 3DStudio .3DS file.
	static bool load3DStudioScene(const std::string &fname,C3DSet *&set,LP_SCENE_LOADER_OPTIONS options = NULL);
	//
	//	Load a scene from a Wavefront .OBJ file.
	static bool loadWavefrontScene(const std::string &fname,C3DSet *&set,LP_SCENE_LOADER_OPTIONS options = NULL);
	//
	//	Exports an object to a Wavefront .OBJ file
	static bool saveWavefrontScene(const std::string &fname,C3DSet *scene);
	//
	//	Load an XML data set and import it in named display and scene.
	static bool loadRaptorData(const std::string &fname, LP_SCENE_LOADER_OPTIONS options = NULL);


private:
    CRaptorToolBox() {};
    virtual ~CRaptorToolBox() {};
};

#endif // !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
