// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "Image.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "CGLTypes.h"
#endif

#ifndef __GLOBAL_H__
	#include "Global.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorExtensions.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE

std::map<std::string, CImage::IImageIO*>				CImage::IMAGE_KIND_IO;
std::map<CImage::IImageOP::OP_KIND, CImage::IImageOP*>	CImage::IMAGE_KIND_OP;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImage::CImage()
{
}

CImage::~CImage()
{
}

void CImage::setImageKindIO(IImageIO *imager)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((imager == NULL) || (imager->getImageKind().empty()))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	vector<std::string> extensionKind = imager->getImageKind();
	for (size_t j=0;j<extensionKind.size();j++)
	{
		string ext;
		for (unsigned int i=0;i<extensionKind[j].size();i++)
			ext += toupper(extensionKind[j][i]);

		IMAGE_KIND_IO.insert(std::map<std::string,IImageIO*>::value_type(ext,imager));
	}
}

CImage::IImageIO* const CImage::getImageKindIO(const std::string &extension)
{
	//	extract the right image loader
    string ext = extension;
	std::string::size_type pos = ext.rfind('.');
    if (pos < ext.size())
        ext = ext.substr(pos+1);

    pos = 0;
	for (pos=0;pos<ext.size();pos++)
        ext[pos] = toupper(ext[pos]);

	std::map<std::string,IImageIO*>::const_iterator itr = IMAGE_KIND_IO.find(ext);
	if (IMAGE_KIND_IO.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}


void CImage::setImageKindOP(IImageOP *op)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (op == NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	IMAGE_KIND_OP.insert(std::map<IImageOP::OP_KIND,IImageOP*>::value_type(op->getKind(),op));
}


CImage::IImageOP* const CImage::getImageKindOP(IImageOP::OP_KIND kind)
{
    std::map<IImageOP::OP_KIND,IImageOP*>::const_iterator itr = IMAGE_KIND_OP.find(kind);
	if (IMAGE_KIND_OP.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}


