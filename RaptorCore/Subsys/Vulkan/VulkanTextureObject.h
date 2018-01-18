// VulkanTextureObject.h: interface for the CVulkanTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
#define AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CVulkanTextureObject : public ITextureObject
{
public:
	CVulkanTextureObject();
	virtual ~CVulkanTextureObject();
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)


