// Test3.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorConsole.h"
#include "System/RaptorDisplay.h"
#include "System/RaptorApplication.h"
#include "System/RenderEntryPoint.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/ITextureObject.h"
#include "Engine/ParticleManager.h"
#include "Engine/3DScene.h"
#include "Engine/IViewPoint.h"
#include "Engine/Animator.h"

#include "ToolBox/RaptorToolBox.h"
#include "Toolbox/BasicObjects.h"
#include "ToolBox/ParticleCompute.h"

RAPTOR_NAMESPACE

#include "RaptorCompute/RaptorCompute.h"
#include "RaptorCompute/RaptorComputeJob.h"
#include "RaptorCompute/RaptorComputeMemory.h"
#include "RaptorCompute/RaptorComputeTask.h"
#include "RaptorCompute/ReadBufferTask.h"


#include "BDDGPU.h"

static const char *TriTest = {
	"inline void OrdonneGPU(__global float4 *data,int val1,int val2,int dir){\n"
	"if( (data[val1].x > data[val2].x) == (dir==0) ){\n"
    "    float4 t;\n"
	"    t = data[val1]; data[val1] = data[val2]; data[val2] = t;\n"    
    "}\n"
	"}\n"
	"\n"
	"__kernel void calculT(__global float4 *data, const uint tailleB,\n"
	" const uint decal,const uint pk) {\n"
	"  int id = get_global_id(0);\n"
	"  int pos = (id&(decal-1))+ 2*decal*(id/decal);\n"
	"  int dir = (id/pk)&1;\n"
	"  OrdonneGPU(data,pos,pos+decal,dir);	\n"
	"  \n"
	"}\n"
};


float Alea(int n)
{
	float ran;

	ran = ( rand()/(float)RAND_MAX ) * (float)n;

	return ran;
}

cl_float4* Initialise(int N)
{
	int i;
	cl_float4 P;
	cl_float4 *T = (cl_float4*)malloc(sizeof(cl_float4)*N);
	if (NULL == T)
		return NULL;

	for ( i=0;i<N;i++)
	{
		P.s[0] = Alea(100);
		P.s[1] = Alea(100);
		P.s[2] = Alea(100);
		P.s[3] = (float) (100+i);

		T[i] = P;
	}

	return T;
}

int KD_GPU(int NElem,CBDDGPU &bddGPU)
{
	std::vector<std::string> programs;
	programs.push_back("Common.h");
	programs.push_back("kernel_Split.cl");

	CRaptorComputeJob *pJob = CRaptorComputeJob::clCreateJob(0,0);
	pJob->clLoadPrograms(programs);
																							
	bddGPU.clBitoniX(*pJob);
	bddGPU.clRangTriX(*pJob);
	bddGPU.clBitoniY(*pJob);
	bddGPU.clRangTriY(*pJob);
	bddGPU.clBuild(*pJob);
	bddGPU.clPrintData(*pJob);

	pJob->clDestroyJob();

	return 0;
}


void Test(cl_float4 *data,size_t NPoints)
{
	CRaptorComputeJob *pJob = CRaptorComputeJob::clCreateJob(0,0);

	static std::string testinit =
	"__kernel void testinit(__global float4 *data,__local float4 *temp) \n\
	{ \n\
		int id = get_global_id(0); \n\
		temp[id] = data[id]; \n\
		temp[id] = 2 * temp[id];\n\
		data[id] = temp[id]; \n\
	}\n";

	pJob->clLoadProgramString(testinit);

	CRaptorComputeMemory &mem = CRaptorComputeMemory::GetInstance(0,0);
	CRaptorComputeMemory::IBufferObject *buffer = mem.clCreateBuffer(sizeof(cl_float4)*NPoints,data);
	CRaptorComputeMemory::IBufferObject *tempbuffer = mem.clCreateBuffer(sizeof(cl_float4)*NPoints,NULL,CRaptorComputeMemory::IBufferObject::LOCAL_BUFFER);

	CRaptorComputeTask task("testinit",NPoints,NPoints);
	task.addParameter(*buffer);
	task.addParameter(*tempbuffer);

	pJob->clAddTask(task);
	pJob->run();

	mem.clDestroyBuffer(buffer);
	mem.clDestroyBuffer(tempbuffer);
	pJob->clDestroyJob();
}

void BitonicGPU(cl_float4 *data,size_t NPoints)
{
	CRaptorComputeJob *pJob = CRaptorComputeJob::clCreateJob(0,0);

	cl_uint tailleB = NPoints/2;
	pJob->clLoadProgramString(TriTest);

	CRaptorComputeMemory &mem = CRaptorComputeMemory::GetInstance(0,0);
	CRaptorComputeMemory::IBufferObject *buffer = mem.clCreateBuffer(sizeof(cl_float4)*NPoints,data);

	CRaptorComputeTask task("calculT",tailleB,0);
	task.addParameter(*buffer);
	task.addParameter(tailleB);

	// Détermination de la puissance de 2
	int puiss = (int)(log((float)NPoints) / log(2.0f));

	// Pour chaque etape on spécifie les arguments du kernel qui changent
	for (int k=0,pk=1;k<puiss;k++,pk<<=1)
	{
		for (int j=k,decal=1<<j;j>=0;j--,decal>>=1)
		{
			CRaptorComputeTask pTask(task);
			pTask.addParameter(decal);
			pTask.addParameter(pk);
			pJob->clAddTask(pTask);
		}//j
	}//k

	CReadBufferTask<cl_float4> readT(*buffer,data);
	pJob->clAddTask(readT);

	pJob->run();

	for (size_t i=0;i<NPoints;i++)
		std::cout << data[i].s[0] << " " <<  data[i].s[1] << " " << data[i].s[2] << " " << data[i].s[3] << std::endl;
	std::cout << std::endl;

	mem.clDestroyBuffer(buffer);
	pJob->clDestroyJob();

	cout << "End of bitonic." << endl << endl;
}

class GalaxyModel : public CParticleManager::CParticleCompute
{
public:
	//! Constructor
	GalaxyModel():CParticleManager::CParticleCompute() {};
	virtual ~GalaxyModel() {};

	//! Implements a random particle system compute
	virtual std::string getModelName(void) const
	{ return "galaxy"; };
	virtual void compute(CParticle& particle,float dt);
	virtual void initialize(CParticle& particle);

private:
	void computeLife(CParticle::PARTICLE_ATTRIBUTE& attrs,float dt);
	void computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs);
};

void GalaxyModel::initialize(CParticle& particle)
{
	size_t nbParticle = particle.getSize();
	for(size_t i = 0; i < nbParticle; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		computeBirth(attrs);
	}
	particle.setQuantity(particle.getSize());
}

void GalaxyModel::computeBirth(CParticle::PARTICLE_ATTRIBUTE& attrs)
{
	// Set initial position
	attrs.position.x = (float)(rand() & 0x1FF) - 255.5f;
	attrs.position.y = (float)(rand() & 0x1FF) - 255.5f;
	attrs.position.z = (float)(rand() & 0x1FF) - 255.5f;
	attrs.position.h = 1.0f;

	// Set speed
	attrs.speed.x = 0.0f;
	attrs.speed.y = 0.0f;
	attrs.speed.z = 0.0f;
	attrs.speed.h = 0.0f;

	// Set pull
	attrs.pull.x = 0.0f;
	attrs.pull.y = 0.0f;
	attrs.pull.z = 0.0f;
	attrs.pull.h = 0.0f;

	// Set color
	attrs.color.x = (rand() & 0x0FF) * ONE_OVER_256_F;	// red
	attrs.color.y = (rand() & 0x0FF) * ONE_OVER_256_F;	// green
	attrs.color.z = (rand() & 0x0FF) * ONE_OVER_256_F;	// blue
	attrs.color.h = 1.0f;								// alpha
	
	// Set size (here mass)
	attrs.size = (float)(rand() & 0x0F) + 15;
	
	// Fading rate unused
	attrs.fade = 0.0f;

	// Rotation angle unused
	attrs.angle = 0.0f;
}

ULONGLONG minT = MAXDWORD;
ULONGLONG maxT = 0;

void GalaxyModel::compute(CParticle& particle,float dt)
{
	ULONGLONG count = GetTickCount64();

	int nbParticle = particle.getSize();
	float G = 0.1f;
	int i=0;

#pragma omp parallel for default(none) private (i,G) shared(dt,particle,nbParticle)
	for (i = 0; i < nbParticle; i++)
	{
		G = 0.1f;
		CParticle::PARTICLE_ATTRIBUTE& attrs = particle.getAttribute(i);
		CGenericVector<float> pos(attrs.position);
		CGenericVector<float> adir;

		for (size_t j=0; j < nbParticle; j++)
		{
			if (i == j) continue;
			CParticle::PARTICLE_ATTRIBUTE& current = particle.getAttribute(j);
			CGenericVector<float> cpos(current.position);
			cpos -= pos;
			double d2 = cpos.Normalize();
			if (d2 < 0.001)
				d2 = 0.001;

			float f = G*current.size / (d2 * d2);
			cpos *= f;
			adir += cpos;
		}

		attrs.speed.x += adir.X() * dt;
		attrs.speed.y += adir.Y() * dt;
		attrs.speed.z += adir.Z() * dt;

		attrs.position.x += attrs.speed.x * dt;
		attrs.position.y += attrs.speed.y * dt;
		attrs.position.z += attrs.speed.z * dt;
	}

	ULONGLONG T = GetTickCount64() - count;
	if (T > maxT)
		maxT = T;
	if (T < minT)
		minT = T;
}

void GLinterop(CRaptorDisplay *pDisplay,RAPTOR_HANDLE wnd)
{
	pDisplay->glvkBindDisplay(wnd);

	CRaptorComputeJob *pJob = CRaptorComputeJob::clCreateJob(0,0,pDisplay);
	
	CParticleManager *pManager = new CParticleManager();
	GalaxyModel *pModel = new GalaxyModel();
	CParticle *pGalaxy = pManager->glCreateParticle(5000,true,CParticle::CGL_PARTICLE_POINT,CParticle::CGL_COLOR_BLEND,pModel,"GALAXY");
	size_t sz = pGalaxy->getSize() * sizeof(CParticle::PARTICLE_ATTRIBUTE);
	pManager->animate(true);

	CObject3DInstance *pSpace = new CObject3DInstance(pGalaxy);
	pSpace->translate(0.0f, 0.0f, -500.0f);

	C3DScene *pScene = pDisplay->getRootScene();
	pScene->addObject(pSpace);

	pDisplay->glvkUnBindDisplay();
	CRaptorApplication *pApp = CRaptorApplication::GetInstance();
	pApp->run();

	/*
	CHostMemoryManager::CBufferObject *glBuffer = CHostMemoryManager::GetInstance()->glAllocateBufferObject(	CHostMemoryManager::CBufferObject::VERTEX_BUFFER,
																						CHostMemoryManager::CBufferObject::DYNAMIC,sz);
	CParticle::PARTICLE_ATTRIBUTE *buf = new CParticle::PARTICLE_ATTRIBUTE[sz];
	{
		for (size_t i=0;i<sz;i++)
		{
			CParticle::PARTICLE_ATTRIBUTE p;
			memcpy(&buf[i],&p,sizeof(CParticle::PARTICLE_ATTRIBUTE));
		}
	}
	CHostMemoryManager::GetInstance()->glSetBufferObjectData(*glBuffer,0,buf,sz);						

	CRaptorComputeMemory &mem = CRaptorComputeMemory::GetInstance(0,1);
	CRaptorComputeMemory::CBufferObject* cBuffer = mem.clCreateBuffer(glBuffer);

	if (NULL == cBuffer)
		cout << "CL/GL interop initialize failed." << endl;
	else
		cout << "CL/GL interop initialized." << endl;

	for (int i=0;i<20;i++)
		cout << "buf[" << i << "]: " << buf[i].position.x << " " 
			 << buf[i].position.y << " " << buf[i].position.z
			 << " " << buf[i].position.h << endl;

	// Job
	static std::string testinit =
	"__kernel void testinit(__global float4 *vertex,__local float4 *temp) \n\
	{ \n\
		int id = get_global_id(0); \n\
		vertex[id].x = id*2.2; \n\
	}\n";

	pJob->clLoadProgramString(testinit);
	CRaptorComputeMemory::CBufferObject *tempbuffer = new CRaptorComputeMemory::CBufferObject(sizeof(GL_COORD_VERTEX)*64);

	CRaptorComputeTask task("testinit",64,64);
	task.addParameter(*cBuffer);
	task.addParameter(*tempbuffer);
	pJob->clAddTask(task);

	pJob->run();

	//	Todo : add semaphore or callback to continue when results are available
	//Sleep(1000);

	CHostMemoryManager::GetInstance()->glGetBufferObjectData(*glBuffer,0,buf,20*sizeof(GL_COORD_VERTEX));
	for (int i=0;i<20;i++)
		cout << "buf[" << i << "]: " << buf[i].position.x << " " 
			 << buf[i].position.y << " " << buf[i].position.z
			 << " " << buf[i].position.h << endl;
	delete [] buf;
	
	pJob->clDestroyJob();
	mem.clDestroyBuffer(cBuffer);
	CHostMemoryManager::GetInstance()->glReleaseBufferObject(glBuffer);
	*/
}

int _tmain(int argc, _TCHAR* argv[])
{
	CRaptorConfig config;
	config.m_bRelocation = true;
    config.m_uiPolygons = 30000;
    config.m_uiVertices = 1000000;
	config.m_bCompute = true;
	Raptor::glInitRaptor(config);

	CRaptorDisplayConfig glcs;
	glcs.width = 640;
	glcs.height = 480;
	glcs.x = 700;
	glcs.y = 20;
	glcs.caption = "Raptor Compute";
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.stencil_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;
	//glcs.refresh_rate.fps = 12;

	CRaptorApplication *pApp = CRaptorApplication::CreateApplication();
	pApp->initApplication(glcs);
	pApp->grabCursor(false);

	RAPTOR_HANDLE wnd = pApp->getRootWindow();
	CRaptorDisplay *pDisplay = pApp->getRootDisplay();
	pDisplay->glvkBindDisplay(wnd);

	IViewPoint *vp = pDisplay->getViewPoint();
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);
	pDisplay->setViewPoint(vp);

	CRaptorCompute::clInitRaptorCompute();
	std::vector<CRaptorCompute::PLATFORM> platforms = CRaptorCompute::getPlatforms();
	for (size_t i=0;i<platforms.size();i++)
	{
		const CRaptorCompute::PLATFORM &p = platforms[i];
		cout << "Version: " << p.version << endl;
		cout << "Vendor: " << p.vendor << endl;
		cout << "Name: " << p.name << endl;
		cout << "Profile: " << p.profile << endl;
		cout << "Extensions: " << p.extensions << endl;
		cout << "Devices: " << endl;
		for (size_t j=0;j<p.devices.size();j++)
		{
			const CRaptorCompute::DEVICE &d = p.devices[j];
			cout << "  Name: " << d.name << endl;
			cout << "  Global memory: " << d.global_memory_size << " bytes" << endl;
			cout << "  Local memory: " << d.local_memory_size << " bytes" << endl;
			cout << endl;
		}
	}

	CTimeObject::setTimeFactor(1.0f);
	CAnimator *animator = new CAnimator();
	CAnimator::SetAnimator(animator);

	CRaptorConsole *pConsole = Raptor::GetConsole();
    pConsole->glInit();
    pConsole->showStatus(true);
	pConsole->showFPS(true);
	pConsole->showFrameTime(true);
    pConsole->activateConsole(true);

	class EntryPoint : CRenderEntryPoint
	{
	public:
		EntryPoint(CRaptorDisplay *dsp,RAPTOR_HANDLE wnd)
			:CRenderEntryPoint(),m_dsp(dsp),m_wnd(wnd) {};
		virtual ~EntryPoint() {};
		virtual void glRender()
		{
			m_dsp->glvkBindDisplay(m_wnd);
			m_dsp->glRender();
			m_dsp->glvkUnBindDisplay();
		}
		CRaptorDisplay *m_dsp;
		RAPTOR_HANDLE m_wnd;
	};
	new EntryPoint(pDisplay,wnd);
	pDisplay->glvkUnBindDisplay();

	size_t NPoints = 16;
	cl_float4 *semiT = Initialise(NPoints);
	for (size_t i=0;i<NPoints;i++)
		cout << semiT[i].s[0] << " " <<  semiT[i].s[1] << " " << semiT[i].s[2] << " " << semiT[i].s[3] << std::endl;
	cout << endl;

	// Test init
	Test(semiT,NPoints);
	
	// sample 1 : bitonic GPU
//	BitonicGPU(semiT,NPoints);

	// sample 2 : GL/CL interop
	GLinterop(pDisplay,wnd);

	return (pApp->quitApplication() ? 1 : 0);

	// sample 3 : build a Kd-Tree
	CBDDGPU bddGPU;
	bddGPU.clInitBdd(NPoints,semiT);
	KD_GPU(NPoints,bddGPU);

	CRaptorCompute::clReleaseRaptorCompute();

	return (pApp->quitApplication() ? 1 : 0);
}

