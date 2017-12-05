#pragma once
#include "raptorservercompressor.h"

class CYUVCompressor : public CRaptorServerCompressor
{
public:
	CYUVCompressor(void);
	virtual ~CYUVCompressor(void);

private:
	virtual bool doCompress(unsigned char* data,size_t size);
};
