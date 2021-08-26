/***************************************************************************/
/*                                                                         */
/*  NVStreaming.cpp                                                        */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_NVSTREAMING_H__AC9C0954_0871_48E0_B487_07FC76C2CF47__INCLUDED_)
	#include "Streaming/NVStreaming.h"
#endif

#include "DataManager/RaptorDataManager.h"


// CUDA Header includes
#include "NVCuVid/dynlink_nvcuvid.h" // <nvcuvid.h>
#include "NVCuVid/dynlink_cuda.h"    // <cuda.h>
#include "NVCuVid/dynlink_cudaGL.h"  // <cudaGL.h>
#include "NVCuVid/dynlink_builtin_types.h"
#include "NVCuVid/helper_cuda_drvapi.h"

#include "NVCuVid/ImageGL.h"
#include "NVCuVid/FrameQueue.h"
#include "NVCuVid/VideoSource.h"
#include "NVCuVid/VideoDecoder.h"
#include "NVCuVid/VideoParser.h"
#include "NVCuVid/cudaModuleMgr.h"
#include "NVCuVid/cudaProcessFrame.h"

CUcontext          g_oContext = 0;
CUdevice           g_oDevice = 0;
cudaVideoCreateFlags g_eVideoCreateFlags = cudaVideoCreate_PreferCUVID;
CUmoduleManager   *g_pCudaModule = 0;
CUstream           g_KernelSID = 0;
//CUmodule           cuModNV12toARGB = 0;
CUfunction         g_kernelNV12toARGB = 0;
CUfunction         g_kernelPassThru = 0;

FrameQueue    *g_pFrameQueue = 0;
VideoSource   *g_pVideoSource = 0;
VideoParser   *g_pVideoParser = 0;
VideoDecoder  *g_pVideoDecoder = 0;

bool			g_bUpdateCSC = true;
eColorSpace        g_eColorSpace = ITU601;
float              g_nHue = 0.0f;

ImageGL		*g_pImageGL = 0; // if we're using OpenGL
int			g_DeviceID = 0;

bool         g_bIsProgressive = true; // assume it is progressive, unless otherwise noted
unsigned int g_DecodeFrameCount = 0;

unsigned int g_nWindowWidth = 0;
unsigned int g_nWindowHeight = 0;

unsigned int g_nClientAreaWidth = 0;
unsigned int g_nClientAreaHeight = 0;

unsigned int g_nVideoWidth = 0;
unsigned int g_nVideoHeight = 0;

CUvideoctxlock       g_CtxLock = NULL;

CUdeviceptr	g_pInteropFrame[2] = { 0, 0 }; // if we're using CUDA malloc
/*
#define checkCudaErrors(err)  __checkCudaErrors (err, __FILE__, __LINE__)

// These are the inline versions for all of the SDK helper functions
inline void __checkCudaErrors(CUresult err, const char *file, const int line)
{
	if (CUDA_SUCCESS != err)
	{
		stringstream msg;
		msg << "checkCudaErrors() Driver API error: ";
		msg << err;
		msg << "from file: ";
		msg << file;
		msg << ", line: ";
		msg << line;

		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), msg.str());
	}
}
*/
bool CNVStreaming::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool CNVStreaming::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return (("AVC" == ext) ||
			("MKV" == ext) ||
			("M2V" == ext) ||
			("264" == ext) ||
			("H264" == ext) ||
			("MP2" == ext) ||
			("MPEG2" == ext) ||
			("MPG" == ext));
}

std::vector<std::string> CNVStreaming::getKind(void) const
{
	std::vector<std::string> result;

	result.push_back("AVC");
	result.push_back("MKV");
	result.push_back("M2V");
	result.push_back("264");
	result.push_back("H264");
	result.push_back("MP2");
	result.push_back("MPEG2");
	result.push_back("MPG");

	return result;
}

class CRenderer
{
public:
	CRenderer() {};

	unsigned int m_lVidWidth;
	unsigned int m_lVidHeight;
};

CNVStreaming::CNVStreaming()
	:frameLength(0.0f), streamPos(0), locked(false), m_Duration(0), m_pRenderer(NULL)
{
	// Initialize the CUDA and NVCUVID
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	typedef HMODULE CUDADRIVER;
#else
	typedef void *CUDADRIVER;
#endif
	CUDADRIVER hHandleDriver = 0;
	cuInit(0, __CUDA_API_VERSION, hHandleDriver);
	cuvidInit(0);
}


// Initialize OpenGL Resources
bool initGL(void)
{
	int dev, device_count = 0;
	bool bSpecifyDevice = false;
	char device_name[256];

	int bTCC = 0;

	// Check for a min spec of Compute 1.1 capability before running
	checkCudaErrors(cuDeviceGetCount(&device_count));

	// If deviceID == 0, and there is more than 1 device, let's find the first available graphics GPU
	if (!bSpecifyDevice && device_count > 0)
	{
		for (int i = 0; i < device_count; i++)
		{
			checkCudaErrors(cuDeviceGet(&dev, i));
			checkCudaErrors(cuDeviceGetName(device_name, 256, dev));

			int bSupported = checkCudaCapabilitiesDRV(1, 1, i);

			if (!bSupported)
			{
				printf("  -> GPU: \"%s\" does not meet the minimum spec of SM 1.1\n", device_name);
				printf("  -> A GPU with a minimum compute capability of SM 1.1 or higher is required.\n");
				return false;
			}

			checkCudaErrors(cuDeviceGetAttribute(&bTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev));
			printf("  -> GPU %d: < %s > driver mode is: %s\n", dev, device_name, bTCC ? "TCC" : "WDDM");

			if (bTCC)
			{
				continue;
			}
			else
			{
				g_DeviceID = i; // we choose an available WDDM display device
			}
		}
	}
	else
	{
		if ((g_DeviceID > (device_count - 1)) || (g_DeviceID < 0))
		{
			printf(" >>> Invalid GPU Device ID=%d specified, only %d GPU device(s) are available.<<<\n", g_DeviceID, device_count);
			printf(" >>> Valid GPU ID (n) range is between [%d,%d]...  Exiting... <<<\n", 0, device_count - 1);
			return false;
		}

		// We are specifying a GPU device, check to see if it is TCC or not
		checkCudaErrors(cuDeviceGet(&dev, g_DeviceID));
		checkCudaErrors(cuDeviceGetName(device_name, 256, dev));

		checkCudaErrors(cuDeviceGetAttribute(&bTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev));
		printf("  -> GPU %d: < %s > driver mode is: %s\n", dev, device_name, bTCC ? "TCC" : "WDDM");
	}

	// initialize GLUT
	/*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(g_nWindowWidth, g_nWindowHeight);
	glutCreateWindow(sAppName);
	reshape(g_nWindowWidth, g_nWindowHeight);

	printf(">> initGL() creating window [%d x %d]\n", g_nWindowWidth, g_nWindowHeight);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glewInit();

	if (!glewIsSupported("GL_VERSION_1_5 GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object"))
	{
		fprintf(stderr, "Error: failed to get minimal extensions for demo\n");
		fprintf(stderr, "This sample requires:\n");
		fprintf(stderr, "  OpenGL version 1.5\n");
		fprintf(stderr, "  GL_ARB_vertex_buffer_object\n");
		fprintf(stderr, "  GL_ARB_pixel_buffer_object\n");
		return true;
	}

	setVSync(g_bUseVsync ? 1 : 0);
	*/

	return true;
}


// Initializes OpenGL Textures (allocation and initialization)
bool
initGLTexture(unsigned int nWidth, unsigned int nHeight)
{
	g_pImageGL = new ImageGL(	nWidth, nHeight,
								nWidth, nHeight,
								g_bIsProgressive,
								ImageGL::BGRA_PIXEL_FORMAT);
	g_pImageGL->clear(0x80);

	g_pImageGL->setCUDAcontext(g_oContext);
	g_pImageGL->setCUDAdevice(g_oDevice);

	return true;
}


void
initCudaVideo()
{
	// bind the context lock to the CUDA context
	CUresult result = cuvidCtxLockCreate(&g_CtxLock, g_oContext);

	if (result != CUDA_SUCCESS)
	{
		printf("cuvidCtxLockCreate failed: %d\n", result);
		assert(0);
	}

	g_pVideoDecoder = new VideoDecoder(g_pVideoSource->format(), g_oContext, g_eVideoCreateFlags, g_CtxLock);
	g_pVideoParser = new VideoParser(g_pVideoDecoder, g_pFrameQueue, &g_oContext);
	g_pVideoSource->setParser(*g_pVideoParser);
}

bool initCudaResources()
{
	CUdevice cuda_device;

	// If we want to use Graphics Interop, then choose the GPU that is capable
	initGL();

	const char *argv[1] = { "" };
	cuda_device = findCudaGLDeviceDRV(0, (const char**)argv);
	checkCudaErrors(cuDeviceGet(&g_oDevice, cuda_device));

	// get compute capabilities and the devicename
	int major, minor;
	size_t totalGlobalMem;
	char deviceName[256];
	checkCudaErrors(cuDeviceComputeCapability(&major, &minor, g_oDevice));
	checkCudaErrors(cuDeviceGetName(deviceName, 256, g_oDevice));
	checkCudaErrors(cuDeviceTotalMem(&totalGlobalMem, g_oDevice));

	// Create CUDA Device w/ GL interop (if WDDM), otherwise CUDA w/o interop (if TCC)
	// (use CU_CTX_BLOCKING_SYNC for better CPU synchronization)
	checkCudaErrors(cuGLCtxCreate(&g_oContext, CU_CTX_BLOCKING_SYNC, g_oDevice));

	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager == NULL)
	{
		return false;
	}

	try
	{
		std::string module_path;
		// Initialize CUDA releated Driver API (32-bit or 64-bit), depending the platform running
		if (sizeof(void *) == 4)
			module_path = dataManager->exportFile("NV12ToARGB_drvapi_Win32.ptx");
		else
			module_path = dataManager->exportFile("NV12ToARGB_drvapi_x64.ptx");
		g_pCudaModule = new CUmoduleManager(module_path.c_str(), 2, 2, 2);
	}
	catch (char const *p_file)
	{
		// If the CUmoduleManager constructor fails to load the PTX file, it will throw an exception
		//printf("\n>> CUmoduleManager::Exception!  %s not found!\n", p_file);
		//printf(">> Please rebuild NV12ToARGB_drvapi.cu or re-install this sample.\n");
		return false;
	}


	g_pCudaModule->GetCudaFunction("NV12ToARGB_drvapi", &g_kernelNV12toARGB);
	g_pCudaModule->GetCudaFunction("Passthru_drvapi", &g_kernelPassThru);

	/////////////////Change///////////////////////////
	// Now we create the CUDA resources and the CUDA decoder context
	initCudaVideo();
	initGLTexture(g_pVideoDecoder->targetWidth(), g_pVideoDecoder->targetHeight());

	CUcontext cuCurrent = NULL;
	CUresult result = cuCtxPopCurrent(&cuCurrent);

	if (result != CUDA_SUCCESS)
		return false;

	/////////////////////////////////////////
	return ((g_pCudaModule && g_pVideoDecoder) ? true : false);
}

void freeCudaResources(bool bDestroyContext)
{
	if (g_pVideoParser)
	{
		delete g_pVideoParser;
	}

	if (g_pVideoDecoder)
	{
		delete g_pVideoDecoder;
	}

	if (g_pVideoSource)
	{
		delete g_pVideoSource;
	}

	if (g_pFrameQueue)
	{
		delete g_pFrameQueue;
	}

	if (g_KernelSID)
	{
		checkCudaErrors(cuStreamDestroy(g_KernelSID));
	}

	if (g_CtxLock)
	{
		checkCudaErrors(cuvidCtxLockDestroy(g_CtxLock));
	}

	if (g_oContext && bDestroyContext)
	{
		checkCudaErrors(cuCtxDestroy(g_oContext));
		g_oContext = NULL;
	}
}

CNVStreaming::~CNVStreaming()
{
	// clean up CUDA and OpenGL resources
	
	// Attach the CUDA Context (so we may properly free memroy)
	checkCudaErrors(cuCtxPushCurrent(g_oContext));

	if (g_pInteropFrame[0])
	{
		checkCudaErrors(cuMemFree(g_pInteropFrame[0]));
	}

	if (g_pInteropFrame[1])
	{
		checkCudaErrors(cuMemFree(g_pInteropFrame[1]));
	}

	// Detach from the Current thread
	checkCudaErrors(cuCtxPopCurrent(NULL));

	if (g_pImageGL)
	{
		delete g_pImageGL;
		g_pImageGL = NULL;
	}

	freeCudaResources(true);
}


void CNVStreaming::seekFrame(unsigned int framePos)
{
}

unsigned int CNVStreaming::getWidth() const
{
	return g_pVideoDecoder->targetWidth();
};

unsigned int CNVStreaming::getHeight() const
{
	return g_pVideoDecoder->targetHeight();
};



// This is the CUDA stage for Video Post Processing.  Last stage takes care of the NV12 to ARGB
void cudaPostProcessFrame(	CUdeviceptr *ppDecodedFrame, size_t nDecodedPitch,
							CUdeviceptr *ppTextureData, size_t nTexturePitch,
							CUmodule cuModNV12toARGB,
							CUfunction fpCudaKernel, CUstream streamID)
{
	uint32_t nWidth = g_pVideoDecoder->targetWidth();
	uint32_t nHeight = g_pVideoDecoder->targetHeight();

	// Upload the Color Space Conversion Matrices
	if (g_bUpdateCSC)
	{
		// CCIR 601/709
		float hueColorSpaceMat[9];
		setColorSpaceMatrix(g_eColorSpace, hueColorSpaceMat, g_nHue);
		updateConstantMemory_drvapi(cuModNV12toARGB, hueColorSpaceMat);

		g_bUpdateCSC = false;
	}

	// TODO: Stage for handling video post processing

	// Final Stage: NV12toARGB color space conversion
	cudaLaunchNV12toARGBDrv(*ppDecodedFrame, nDecodedPitch,
							*ppTextureData, nTexturePitch,
							nWidth, nHeight, fpCudaKernel, streamID);
}

// Run the Cuda part of the computation (if g_pFrameQueue is empty, then return false)
bool copyDecodedFrameToTexture(unsigned int &nRepeats, int bUseInterop, int *pbIsProgressive)
{
	CUVIDPARSERDISPINFO oDisplayInfo;

	if (g_pFrameQueue->dequeue(&oDisplayInfo))
	{
		CCtxAutoLock lck(g_CtxLock);
		// Push the current CUDA context (only if we are using CUDA decoding path)
		cuCtxPushCurrent(g_oContext);

		CUdeviceptr  pDecodedFrame[2] = { 0, 0 };
		CUdeviceptr  pInteropFrame[2] = { 0, 0 };

		*pbIsProgressive = oDisplayInfo.progressive_frame;
		g_bIsProgressive = oDisplayInfo.progressive_frame ? true : false;

		int distinct_fields = 1;
		if (!oDisplayInfo.progressive_frame && oDisplayInfo.repeat_first_field <= 1) {
			distinct_fields = 2;
		}
		nRepeats = oDisplayInfo.repeat_first_field;

		for (int active_field = 0; active_field < distinct_fields; active_field++)
		{
			CUVIDPROCPARAMS oVideoProcessingParameters;
			memset(&oVideoProcessingParameters, 0, sizeof(CUVIDPROCPARAMS));

			oVideoProcessingParameters.progressive_frame = oDisplayInfo.progressive_frame;
			oVideoProcessingParameters.second_field = active_field;
			oVideoProcessingParameters.top_field_first = oDisplayInfo.top_field_first;
			oVideoProcessingParameters.unpaired_field = (distinct_fields == 1);

			unsigned int nDecodedPitch = 0;
			unsigned int nWidth = 0;
			unsigned int nHeight = 0;

			// map decoded video frame to CUDA surfae
			g_pVideoDecoder->mapFrame(oDisplayInfo.picture_index, &pDecodedFrame[active_field], &nDecodedPitch, &oVideoProcessingParameters);
			nWidth = g_pVideoDecoder->targetWidth(); // PAD_ALIGN(g_pVideoDecoder->targetWidth(), 0x3F);
			nHeight = g_pVideoDecoder->targetHeight(); // PAD_ALIGN(g_pVideoDecoder->targetHeight(), 0x0F);
			// map OpenGL PBO or CUDA memory
			size_t nTexturePitch = 0;

			if (g_pImageGL)
			{
				// map the texture surface
				g_pImageGL->map(&pInteropFrame[active_field], &nTexturePitch, active_field);
				nTexturePitch = g_nWindowWidth * 4;
			}
			else
			{
				pInteropFrame[active_field] = g_pInteropFrame[active_field];
				nTexturePitch = g_pVideoDecoder->targetWidth() * 2;
			}

			// perform post processing on the CUDA surface (performs colors space conversion and post processing)
			// comment this out if we inclue the line of code seen above

			cudaPostProcessFrame(	&pDecodedFrame[active_field], nDecodedPitch, &pInteropFrame[active_field],
									nTexturePitch, g_pCudaModule->getModule(), g_kernelNV12toARGB, g_KernelSID);

			if (g_pImageGL)
			{
				// unmap the texture surface
				g_pImageGL->unmap(active_field);
			}

			// unmap video frame
			// unmapFrame() synchronizes with the VideoDecode API (ensures the frame has finished decoding)
			g_pVideoDecoder->unmapFrame(pDecodedFrame[active_field]);
			// release the frame, so it can be re-used in decoder
			g_pFrameQueue->releaseFrame(&oDisplayInfo);
			g_DecodeFrameCount++;
		}

		// Detach from the Current thread
		checkCudaErrors(cuCtxPopCurrent(NULL));
	}
	else
	{
		// Frame Queue has no frames, we don't compute FPS until we start
		return false;
	}

	// check if decoding has come to an end.
	// if yes, signal the app to shut down.
	if (!g_pVideoSource->isStarted() && g_pFrameQueue->isEndOfDecode() && g_pFrameQueue->isEmpty())
	{
		// Let's just stop, and allow the user to quit, so they can at least see the results
		g_pVideoSource->stop();
	}

	return true;
}

bool CNVStreaming::readFrame(unsigned char *& readBuffer, float timestamp)
{
	if (!locked)
		return false;

	static unsigned int nRepeatFrame = 0;
	bool bFramesDecoded = true;
	int bIsProgressive = 1;

	if (0 != g_pFrameQueue)
	{
		// if not running, we simply don't copy new frames from the decoder
		bFramesDecoded = copyDecodedFrameToTexture(nRepeatFrame, true, &bIsProgressive);
	}

	readBuffer = NULL;
	streamPos++;

	/*if (m_pRenderer->sampleEnd >= m_Duration)
	{
		seekFrame(0);
		streamPos = 0;
	}*/

	return true;
}

bool loadVideoSource(	const char *video_file,
						unsigned int &width, unsigned int &height,
						unsigned int &dispWidth, unsigned int &dispHeight)
{
	g_pFrameQueue = new FrameQueue;
	g_pVideoSource = new VideoSource(video_file, g_pFrameQueue);

	// retrieve the video source (width,height)
	g_pVideoSource->getSourceDimensions(width, height);
	g_pVideoSource->getSourceDimensions(dispWidth, dispHeight);

	if (g_pVideoSource->format().codec == cudaVideoCodec_JPEG)
	{
		g_eVideoCreateFlags = cudaVideoCreate_PreferCUDA;
	}

	bool IsProgressive = 0;
	g_pVideoSource->getProgressive(IsProgressive);

	return IsProgressive;
}

bool CNVStreaming::openReader(const std::string &fname)
{
	if (locked)
        return false;
	
	// Find out the video size (uses NVCUVID calls)
	g_bIsProgressive = loadVideoSource(	fname.c_str(),
										g_nVideoWidth, g_nVideoHeight,
										g_nWindowWidth, g_nWindowHeight);

	g_nVideoWidth = PAD_ALIGN(g_nVideoWidth, 0x3F);
	g_nVideoHeight = PAD_ALIGN(g_nVideoHeight, 0x0F);

	// Initialize CUDA and try to connect with an OpenGL context
	// Other video memory resources will be available
	if (initCudaResources())
	{
		g_pVideoSource->start();
		locked = true;
		return true;
	}
	else
		return false;
}

bool CNVStreaming::closeReader(void)
{
	g_pFrameQueue->endDecode();
	g_pVideoSource->stop();

	locked = false;
    return true;
}

bool CNVStreaming::openRecorder(const std::string& fname,unsigned int width,unsigned int height)
{
	return false;
}

bool CNVStreaming::writeFrame(const unsigned char *writeBuffer)
{
	return false;
}

bool CNVStreaming::closeRecorder(void)
{
	return false;
}


