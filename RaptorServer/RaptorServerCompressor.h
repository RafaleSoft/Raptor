/***************************************************************************/
/*                                                                         */
/*  RaptorServerCompressor.h                                               */
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


#if !defined(AFX_RAPTORSERVERCOMPRESSOR_H__4E72D8E5_4C9B_44CF_900D_3599D537F6B6__INCLUDED_)
#define AFX_RAPTORSERVERCOMPRESSOR_H__4E72D8E5_4C9B_44CF_900D_3599D537F6B6__INCLUDED_

#include <vector>

#if !defined(AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork/RaptorNetwork.h"
#endif

//!
//!	Define a basic image compressor based on
//!	collecting only changes from the previous frame.
//!	Real instances will implement an image compression
//!	(final stage befor date replied to clients).
//!
class CRaptorServerCompressor
{
public:
	CRaptorServerCompressor()
		:m_pData(NULL),m_size(0)
	{};

	virtual ~CRaptorServerCompressor();

	//!	
	//!	Compute the difference with the previous frame
	//!
	bool differentialCompress(unsigned char* data,size_t size);

	//!
	//!	Remove alpha component to redule network limitation
	//!
	void removeAlpha(unsigned char* data,size_t size);

	void compress(unsigned char* data,size_t size);


protected:
	//!
	//! Compute the list of blocs of image containing 
	//! at least a change from previous frame
	//!
	void makeBlocs(unsigned char* data,size_t &size);

	unsigned char *getData(void) const { return m_pData; };

	std::vector<CRaptorNetwork::IMAGE_COMMAND> m_blocs;


private:
	virtual bool doCompress(unsigned char* data,size_t size) = 0;

	unsigned char	*m_pData;
	size_t			m_size;
};

#endif	// !#define AFX_RAPTORSERVERCOMPRESSOR_H__4E72D8E5_4C9B_44CF_900D_3599D537F6B6__INCLUDED_