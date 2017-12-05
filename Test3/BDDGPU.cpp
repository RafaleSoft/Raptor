#include "StdAfx.h"
#include "BDDGPU.h"

#include "RaptorCompute/RaptorComputeTask.h"
#include "RaptorCompute/RaptorComputeJob.h"
#include "RaptorCompute/RaptorComputeMemory.h"
#include "RaptorCompute/ReadBufferTask.h"

#include <math.h>


CBDDGPU::CBDDGPU(void)
	:buffer_Data(NULL),buffer_Kd(NULL),buffer_RangId(NULL),buffer_IdOk(NULL),
	buffer_RangX(NULL),buffer_RangY(NULL),
	m_nbKd(0),m_size(0),m_nSteps(0)
{
}


CBDDGPU::~CBDDGPU(void)
{
}

void CBDDGPU::calculParamKd(int *BSE,int *NVC,int *NBI)
{
	int Dimension = m_size;
	int  B_S = *BSE;
	int Dim_temp = 0;
	int TBI;
	int nvc = 1;
	int n=1;
	bool ok = true;
	
	while (ok)
	{
		Dim_temp = Dimension /2;
		if (Dim_temp > (Dimension-Dim_temp)-1) 
		{
			Dimension = Dim_temp;
		}else
		{
			Dimension = (Dimension-Dim_temp)-1;
		}

		if (Dimension <= B_S) 
		{
			ok = false;
			TBI = Dimension;
		}else
		{
			nvc += (int) pow(2.0,n);
			n++;
		}
	}

	*NBI = (int)(pow(2.0,n));
	*BSE = TBI;
	*NVC = nvc;
	
	m_nbKd = (unsigned int) (pow(2.0,n+1)-1);
	m_nSteps = n;
}

bool CBDDGPU::clInitBdd(size_t size,cl_float4 *data)
{
	int NVC = 0;
	int NBVal= 6;
	int NBI = 0;
	int BS = Max;

	m_size = size;

	calculParamKd(&BS,&NVC,&NBI);

	CRaptorComputeMemory &mem = CRaptorComputeMemory::GetInstance(0,0);
	LigneKdGPU *KdTGPU = (LigneKdGPU*) calloc(m_nbKd,sizeof(LigneKdGPU));
	buffer_Kd = mem.clCreateBuffer(sizeof(LigneKdGPU)*m_nbKd, KdTGPU);

	cl_uint *IdOk  = (cl_uint*) calloc(size,sizeof(cl_uint));
	buffer_IdOk = mem.clCreateBuffer(sizeof(cl_uint)*size, IdOk);

	cl_uint *Rangs = (cl_uint*) calloc(size,sizeof(cl_uint));
	buffer_RangX = mem.clCreateBuffer(sizeof(cl_uint)*size, Rangs);	
	buffer_RangY = mem.clCreateBuffer(sizeof(cl_uint)*size, Rangs);
	buffer_RangId = mem.clCreateBuffer(sizeof(cl_uint)*size, Rangs);

	buffer_Data = mem.clCreateBuffer(sizeof(cl_float4)*size, data);

	free(KdTGPU);
	free(IdOk);
	free(Rangs);
	
	return ((buffer_Kd != NULL) && 
			(buffer_RangId != NULL) &&
			(buffer_IdOk != NULL) &&
			(buffer_Data != NULL) &&
			(buffer_RangX != NULL) && 
			(buffer_RangY != NULL));
}

bool CBDDGPU::clPrintData(CRaptorComputeJob &job)
{
	if (buffer_Data == NULL)
		return false;

	cl_float4 *data = new cl_float4[m_size];
	cl_uint *RangX = new cl_uint[m_size];
	cl_uint *RangY = new cl_uint[m_size];

	CReadBufferTask<cl_float4> readData(*buffer_Data,data);
	CReadBufferTask<cl_uint> readRangX(*buffer_RangX,RangX);
	CReadBufferTask<cl_uint> readRangY(*buffer_RangY,RangY);

	job.clAddTask(readData);
	job.clAddTask(readRangX);
	job.clAddTask(readRangY);
	bool res = job.run();

	if (res)
	{
		for (size_t i=0;i<m_size;i++)
			std::cout << data[i].s[0] << " " <<  data[i].s[1] << " " << data[i].s[2] << " " << data[i].s[3] << std::endl;
		std::cout << std::endl;

		for (size_t i=0;i<m_size;i++)
			std::cout << RangX[i] << " ";
		std::cout << std::endl;
		for (size_t i=0;i<m_size;i++)
			std::cout << RangY[i] << " ";
		std::cout << std::endl;
	}

	delete [] data;
	delete [] RangX;
	delete [] RangY;

	return (res && job.clDestroyTasks());
}

bool CBDDGPU::clBitoniX(CRaptorComputeJob &job)
{
	cl_uint tailleB = m_size / 2;
	CRaptorComputeTask bitonicX("BiToniX",tailleB,0);
	bitonicX.addParameter(*buffer_Data);
	bitonicX.addParameter(tailleB);
	bitonicX.addParameter(0);
	bitonicX.addParameter(0);

	// Détermination de la puissance de 2
	int puiss = (int)(log((float)m_size) / log(2.0f));

	// Pour chaque etape on spécifie les arguments du kernel qui changent
	for (int k=0,pk=1;k<puiss;k++,pk<<=1)
	{
		for (int j=k,decal=1<<j;j>=0;j--,decal>>=1)
		{
			bitonicX.setParameter(2,decal);
			bitonicX.setParameter(3,pk);
			job.clAddTask(bitonicX);
		}//j
	}//k
	
	bool res = job.run();

	return (res && job.clDestroyTasks());
}

bool CBDDGPU::clRangTriX(CRaptorComputeJob &job)
{
	cl_uint tailleB = m_size;
	CRaptorComputeTask rangTriX("Rang_TriX",tailleB,0);
	rangTriX.addParameter(*buffer_Data);
	rangTriX.addParameter(*buffer_RangX);
	rangTriX.addParameter(*buffer_Kd);
	rangTriX.addParameter(tailleB);

	job.clAddTask(rangTriX);
	bool res = job.run();

	return (res && job.clDestroyTasks());
}

bool CBDDGPU::clRangTriY(CRaptorComputeJob &job)
{
	cl_uint tailleB = m_size;
	CRaptorComputeTask rangTriY("Rang_TriY",tailleB,0);
	rangTriY.addParameter(*buffer_Data);
	rangTriY.addParameter(*buffer_RangY);
	rangTriY.addParameter(*buffer_RangId);
	rangTriY.addParameter(*buffer_Kd);
	rangTriY.addParameter(tailleB);

	job.clAddTask(rangTriY);
	bool res = job.run();

	return (res && job.clDestroyTasks());
}

bool CBDDGPU::clBitoniY(CRaptorComputeJob &job)
{
	cl_uint tailleB = m_size / 2;
	CRaptorComputeTask bitonicY("BiToniY",tailleB,0);
	bitonicY.addParameter(*buffer_Data);
	bitonicY.addParameter(tailleB);
	bitonicY.addParameter(0);
	bitonicY.addParameter(0);
	bitonicY.addParameter(*buffer_RangX);
	
	// Détermination de la puissance de 2
	int puiss = (int)(log((float)m_size) / log(2.0f));

	// Pour chaque etape on spécifie les arguments du kernel qui changent
	for (int k=0,pk=1;k<puiss;k++,pk<<=1)
	{
		for (int j=k,decal=1<<j;j>=0;j--,decal>>=1)
		{
			bitonicY.setParameter(2,decal);
			bitonicY.setParameter(3,pk);
			job.clAddTask(bitonicY);
		}//j
	}//k
	
	bool res = job.run();

	return (res && job.clDestroyTasks());
}

bool CBDDGPU::clBuild(CRaptorComputeJob &job)
{
	cl_uint debut = 0;
	cl_uint NBE = Max;

	CRaptorComputeMemory &mem = CRaptorComputeMemory::GetInstance(0,0);

	cl_uint *buffers = (cl_uint*) calloc(m_size,sizeof(cl_uint));
	CRaptorComputeMemory::IBufferObject* bufferFlags = mem.clCreateBuffer(sizeof(cl_uint)*m_size, buffers);
	CRaptorComputeMemory::IBufferObject* bufferFlagsN = mem.clCreateBuffer(sizeof(cl_uint)*m_size, buffers);
	CRaptorComputeMemory::IBufferObject* bufferSumPrefix = mem.clCreateBuffer(sizeof(cl_uint)*m_size,buffers);
	CRaptorComputeMemory::IBufferObject* bufferSumPrefixN = mem.clCreateBuffer(sizeof(cl_uint)*m_size,buffers);
	CRaptorComputeMemory::IBufferObject* buffer_RangO = mem.clCreateBuffer(sizeof(cl_uint)*m_size, buffers);
	CRaptorComputeMemory::IBufferObject* buffer_Inter = mem.clCreateBuffer(sizeof(cl_uint)*m_size, buffers);
	free(buffers);

	cl_uint NbBloc = m_size / NBE;
	cl_uint *blocs = (cl_uint*) calloc(NbBloc,sizeof(cl_uint));
	CRaptorComputeMemory::IBufferObject* bufferSumBloc = mem.clCreateBuffer(sizeof(cl_uint)*NbBloc, blocs);
	CRaptorComputeMemory::IBufferObject* bufferSumBlocN = mem.clCreateBuffer(sizeof(cl_uint)*NbBloc, blocs);
	free(blocs);
	
	CRaptorComputeTask flagRaZ("Flag_RaZ",m_size,0);
	flagRaZ.addParameter(*bufferFlags);
	flagRaZ.addParameter(*bufferFlagsN);
	flagRaZ.addParameter(*bufferSumPrefix);
	flagRaZ.addParameter(*bufferSumPrefixN);

	CRaptorComputeTask flagInitD("Flag_InitD",m_size,0);
	flagInitD.addParameter(*buffer_RangX);
	flagInitD.addParameter(*bufferFlags);
	flagInitD.addParameter(debut);
	flagInitD.addParameter(debut);

	CRaptorComputeMemory::IBufferObject *localBuffer = mem.clCreateBuffer(NBE,NULL,CRaptorComputeMemory::IBufferObject::LOCAL_BUFFER);
	CRaptorComputeTask scanGPU("ScanGPU",m_size,NBE/2);
	scanGPU.addParameter(*bufferFlags);
	scanGPU.addParameter(NBE);
	scanGPU.addParameter(*bufferSumPrefix);
	scanGPU.addParameter(*bufferSumPrefixN);
	scanGPU.addParameter(*localBuffer);
	scanGPU.addParameter(*localBuffer);
	
	CRaptorComputeTask affBloc("Aff_Bloc",NbBloc,0);
	affBloc.addParameter(*bufferSumPrefix);
	affBloc.addParameter(*bufferSumPrefixN);
	affBloc.addParameter(*bufferSumBloc);
	affBloc.addParameter(*bufferSumBlocN);
	affBloc.addParameter(*bufferFlags);
	
	CRaptorComputeTask sumBloc("SumBloc",NbBloc/2,NbBloc/2);
	sumBloc.addParameter(NbBloc);
	sumBloc.addParameter(*bufferSumBloc);
	sumBloc.addParameter(*bufferSumBlocN);
	sumBloc.addParameter(*localBuffer);
	sumBloc.addParameter(*localBuffer);

	CRaptorComputeTask prefixBloc("Prefix_Bloc",m_size,0);
	prefixBloc.addParameter(*bufferSumPrefix);
	prefixBloc.addParameter(*bufferSumPrefixN);
	prefixBloc.addParameter(*bufferSumBloc);
	prefixBloc.addParameter(*bufferSumBlocN);

	CRaptorComputeTask reparti("Reparti",m_size,0);
	reparti.addParameter(*bufferSumPrefix);
	reparti.addParameter(*bufferSumPrefixN);
	reparti.addParameter(*buffer_RangX);
	reparti.addParameter(*buffer_RangO);
	reparti.addParameter(*buffer_RangId);
	reparti.addParameter(*buffer_Data);
	reparti.addParameter(*buffer_Kd);
	reparti.addParameter(debut);
	reparti.addParameter(debut);
	reparti.addParameter(debut);
	reparti.addParameter(debut);
	reparti.addParameter(*buffer_Inter);

	CRaptorComputeTask echange("Echange",m_size,0);
	echange.addParameter(*buffer_RangX);
	echange.addParameter(*buffer_RangO);
	echange.addParameter(debut);

	CRaptorComputeTask suivre("Suivre",m_size,0);
	suivre.addParameter(*buffer_RangY);
	suivre.addParameter(*buffer_RangO);
	suivre.addParameter(*buffer_Inter);
	suivre.addParameter(debut);

	CRaptorComputeTask soustrait("Soustrait",m_size/2,0);
	soustrait.addParameter(*buffer_RangX);
	soustrait.addParameter(debut);
	soustrait.addParameter(debut);
	
	CRaptorComputeTask repercute_Init("Repercute_Init",m_size/2,0);
	repercute_Init.addParameter(*buffer_RangY);
	repercute_Init.addParameter(*bufferFlags);
	repercute_Init.addParameter(debut);
	
	CRaptorComputeTask renum("Renum",m_size/2,0);
	renum.addParameter(*buffer_RangY);
	renum.addParameter(*bufferSumPrefix);
	renum.addParameter(*bufferSumPrefixN);
	renum.addParameter(*buffer_RangO);
	renum.addParameter(debut);
	renum.addParameter(debut);
	
	unsigned int i=1;
	int ii = 2;
	//for (i=1,ii=2 ; i <= 1 /*m_nSteps*/ ; i++,ii<<=1)
	{
		cl_uint dimCoup = (i-1) & 1;
		int TailleBloc = (m_size / ii);

		for (int j=0;j<ii;j=j+2)
		{
			job.clAddTask(flagRaZ);

			// Reglage des parametres
			debut = j*TailleBloc;
			int Taille_temp = 2 * TailleBloc;
			cl_uint mediane = TailleBloc;
			
			flagInitD.setParameter(2,mediane);
			flagInitD.setParameter(3,debut);
			//flagInitD.setGlobalSize(Taille_temp);

			//	Specificités selon les dimensions de coupure
			if (dimCoup == 0)
			{
				flagInitD.setParameter(0,*buffer_RangX);
			}
			else
			{
				flagInitD.setParameter(0,*buffer_RangY);
			}
			job.clAddTask(flagInitD);

			scanGPU.setGlobalSize(TailleBloc);
			job.clAddTask(scanGPU);

			affBloc.setGlobalSize(Taille_temp / NBE);
			job.clAddTask(affBloc);
			job.clAddTask(sumBloc);
			job.clAddTask(prefixBloc);

			cl_uint idKd = ii+j-1;
			reparti.setParameter(7,mediane);
			reparti.setParameter(8,debut);
			reparti.setParameter(9,dimCoup);
			reparti.setParameter(10,idKd);
			job.clAddTask(reparti);

			// Swap des deux vecteurs RangX et Rang0
			// Repercussion sur les autres dimensions des positions Interchangées: RangY
			if (dimCoup == 0)
			{
				echange.setParameter(0,*buffer_RangX);
				suivre.setParameter(0,*buffer_RangY);
			}
			else
			{
				echange.setParameter(0,*buffer_RangY);
				suivre.setParameter(0,*buffer_RangX);
			}
			echange.setParameter(2,debut);
			job.clAddTask(echange);

			suivre.setParameter(3,debut);
			job.clAddTask(suivre);

			// Puis Swap de RangY Rang0
			if (dimCoup == 0)
			{
				echange.setParameter(0,*buffer_RangY);
			}
			else
			{
				echange.setParameter(0,*buffer_RangX);
			}
			job.clAddTask(echange);

			// Kernel 16 : Ensuite repercussion sur RangId
			suivre.setParameter(0,*buffer_RangId);
			job.clAddTask(suivre);
			echange.setParameter(0,*buffer_RangId);
			job.clAddTask(echange);

			// On soustrait la mediane dans la deuxieme partie de tableau
			// de la dimension de coupure
			soustrait.setParameter(1,m_size/2);
			soustrait.setParameter(2,debut + TailleBloc);
			job.clAddTask(soustrait);

			// Seconde partie : Repercussion sur Y, on ne fait que renuméroter, c'est plus "simple"
			job.clAddTask(flagRaZ);
			repercute_Init.setParameter(0,*buffer_RangY);
			repercute_Init.setParameter(2,debut);
			job.clAddTask(repercute_Init);

			job.clAddTask(scanGPU);
			job.clAddTask(affBloc);
			job.clAddTask(sumBloc);
			job.clAddTask(prefixBloc);

			renum.setParameter(4,debut);
			renum.setParameter(5,debut + TailleBloc);
			job.clAddTask(renum);

			// Puis Swap de RangY Rang0
			if (dimCoup == 0)
			{
				echange.setParameter(0,*buffer_RangY);
			}
			else
			{
				echange.setParameter(0,*buffer_RangX);
			}
			job.clAddTask(echange);
		}
	}

	CReadBufferTask<cl_uint> readBuffer(*bufferFlags);
	CReadBufferTask<cl_uint> readBuffer2(*bufferSumPrefix);
	CReadBufferTask<cl_uint> readBuffer3(*bufferSumBloc);
	CReadBufferTask<cl_uint> readBuffer4(*buffer_RangO);
	job.clAddTask(readBuffer);
	job.clAddTask(readBuffer2);
	job.clAddTask(readBuffer3);
	job.clAddTask(readBuffer4);

	bool res = job.run();

	if (res)
	{
		const cl_uint *data = readBuffer.getResult();
		for (size_t j=0;j<m_size;j++)
			cout << data[j] << " ";
		cout << endl;

		const cl_uint *data2 = readBuffer2.getResult();
		for (size_t j=0;j<m_size;j++)
			cout << data2[j] << " ";
		cout << endl;
		
		const cl_uint *data3 = readBuffer3.getResult();
		for (size_t j=0;j<NbBloc;j++)
			cout << data3[j] << " ";
		cout << endl;

		const cl_uint *data4 = readBuffer4.getResult();
		for (size_t j=0;j<m_size;j++)
			cout << data4[j] << " ";
		cout << endl;
	}

	res &= mem.clDestroyBuffer(bufferFlags);
	res &= mem.clDestroyBuffer(bufferFlagsN);
	res &= mem.clDestroyBuffer(bufferSumPrefix);
	res &= mem.clDestroyBuffer(bufferSumPrefixN);
	res &= mem.clDestroyBuffer(bufferSumBloc);
	res &= mem.clDestroyBuffer(bufferSumBlocN);
	res &= mem.clDestroyBuffer(buffer_RangO);
	res &= mem.clDestroyBuffer(buffer_Inter);
	res &= mem.clDestroyBuffer(localBuffer);

	return (res && job.clDestroyTasks());
}
