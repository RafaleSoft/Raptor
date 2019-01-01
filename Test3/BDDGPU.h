
#ifndef __BDDGPU_H__
#define __BDDGPU_H__

#include "Common.h"

#include "RaptorCompute/RaptorCompute.h"
#include "RaptorCompute/RaptorComputeMemory.h"

#include <iostream>
using namespace std;

#include "CL/cl.h"
#include "CL/cl_platform.h"
#include "CL/opencl.h"

class CBDDGPU
{
public:
	typedef struct LigneKdGPU_t 
	{
		cl_float4 Pmin; // Deux point de dimensions N pour definir une boîte
		cl_float4 Pmax;
		cl_uint BI;
		cl_uint Visite;
		cl_uint dummy;
		cl_uint dummy2;
	} LigneKdGPU;

	CBDDGPU(void);
	~CBDDGPU(void);

	bool clInitBdd(size_t size,cl_float4 *data);

	bool clBitoniX(CRaptorComputeJob &job);
	bool clRangTriX(CRaptorComputeJob &job);
	bool clBitoniY(CRaptorComputeJob &job);
	bool clRangTriY(CRaptorComputeJob &job);
	bool clBuild(CRaptorComputeJob &job);
	bool clPrintData(CRaptorComputeJob &job);

private:
	void calculParamKd(int *BSE,int *NVC,int *NBI);
	
	size_t			m_size;
	unsigned int	m_nbKd;
	unsigned int	m_nSteps;

	CRaptorComputeMemory::IBufferObject *buffer_Data;
	CRaptorComputeMemory::IBufferObject *buffer_Kd;
	CRaptorComputeMemory::IBufferObject *buffer_RangId;
	CRaptorComputeMemory::IBufferObject *buffer_IdOk;

	CRaptorComputeMemory::IBufferObject* buffer_RangX;
	CRaptorComputeMemory::IBufferObject* buffer_RangY;
};

#endif


