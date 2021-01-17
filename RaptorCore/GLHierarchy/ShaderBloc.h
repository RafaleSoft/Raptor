/***************************************************************************/
/*                                                                         */
/*  ShaderBloc.h                                                           */
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


#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
#define AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CShaderBloc
{
public:
	//!	Shader usage for this bloc: a uniform bloc or a storage bloc.
	typedef enum BLOC_KIND_t
	{
		NONE,
		UNIFORM,
		STORAGE
	} BLOC_KIND;

    //! Constructor.
	//!	If uniform is NULL, a uniform buffer is allocated and managed privately to this bloc.
	//! Otherwise, the uniform buffer is considered external, for allocating, updating and releasing.
	//! @param kind: the kind of shader bloc: uniform or storage
	//! @param uniform: the uniform buffer device memory.
	//! @param size: the size of the bloc in uniform buffer
	//! @param offset: the offset of this bloc in the uniform buffer.
	CShaderBloc(BLOC_KIND kind, uint8_t *uniform, uint64_t size, uint64_t offset);

	//!	Destructor.
	virtual ~CShaderBloc();

	//!	Return the external state of this bloc.
	//! @return the external state, true if external.
	bool isExternal(void) const { return m_bExternal; };

	//!	Update bloc name: can be different from the shader source uniform name.
	//! @param name: the new bloc name.
	void setName(const std::string& name) { m_name = name; };

	//!	Retrieve this bloc name.
	//! @return the current bloc name.
	const std::string& getName(void) const { return m_name; };

	//!	Update bloc data in uniform buffer, only if not externally managed.
	void glvkUpdateBloc(uint8_t *src);

	//!	Update bloc uniform buffer to make it externally managed.
	void glvkSetUniformBuffer(uint8_t *uniform, uint64_t size, uint64_t offset);

	//!	Update bloc name: can be different from the shader source uniform name.
	//! @param name: the new bloc name.
	void bindBloc(uint32_t bindPoint) { m_index = bindPoint; };

	//! Rendering : bind bloc to current shader stage.
	void glRender();


private:
	//! Copy constructor: shader blocs cannot be copied.
	CShaderBloc(const CShaderBloc&);
	//! Copy operator: shader blocs cannot be copied.
	CShaderBloc& operator=(const CShaderBloc&);

	//!	Shader bloc name.
	std::string	m_name;

	//!	Uniform buffer device memory.
	uint8_t*	m_buffer;

	//!	Bloc size.
	uint64_t	m_size;

	//!	Offset in uniform buffer for this bloc.
	uint64_t	m_offset;

	//!	Bloc binding point.
	uint32_t	m_index;
	
	//!	If external, the uniform buffer is externally provided and managed.
	bool		m_bExternal;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
