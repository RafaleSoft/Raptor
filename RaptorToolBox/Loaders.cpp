#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
	#include "RaptorToolBox.h"
#endif

#include "Loaders/3DSfile.h"

extern bool _glLoadScene(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options);
extern bool _glLoadWavefront(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options);
extern bool _glSaveWavefront(const std::string &fname,C3DSet *set);

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading a 3DS file
//
bool CRaptorToolBox::load3DStudioScene(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options )
{
	return _glLoadScene(fname,set,options);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading an OBJ file
//
bool CRaptorToolBox::loadWavefrontScene(const std::string &fname,C3DSet *&set,LP_SCENE_LOADER_OPTIONS options)
{
	return _glLoadWavefront(fname,set,options);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for saving an OBJ file
//
bool CRaptorToolBox::saveWavefrontScene(const std::string &fname,C3DSet *scene)
{
	return _glSaveWavefront(fname,scene);
}