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

// CUDA Header includes
#include "NVCuVid/dynlink_nvcuvid.h" // <nvcuvid.h>
#include "NVCuVid/dynlink_cuda.h"    // <cuda.h>
#include "NVCuVid/dynlink_cudaGL.h"  // <cudaGL.h>
#include "NVCuVid/dynlink_builtin_types.h"

#include "NVCuVid/ImageGL.h"

CUcontext	g_oContext = 0;
ImageGL		*g_pImageGL = 0; // if we're using OpenGL
CUdeviceptr	g_pInteropFrame[2] = { 0, 0 }; // if we're using CUDA malloc

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

CNVStreaming::CNVStreaming()
	:frameLength(0.0f), streamPos(0), locked(false), m_Duration(0) 
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

#if CUDA_VERSION >= 3020
			checkCudaErrors(cuDeviceGetAttribute(pbTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev));
			printf("  -> GPU %d: < %s > driver mode is: %s\n", dev, device_name, *pbTCC ? "TCC" : "WDDM");

			if (*pbTCC)
			{
				continue;
			}
			else
			{
				g_DeviceID = i; // we choose an available WDDM display device
			}

#else

			// We don't know for sure if this is a TCC device or not, if it is Tesla we will not run
			if (!STRNCASECMP(device_name, "Tesla", 5))
			{
				printf("  \"%s\" does not support %s\n", device_name, sSDKname);
				*pbTCC = 1;
				return false;
			}
			else
			{
				*pbTCC = 0;
			}

#endif
			printf("\n");
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

#if CUDA_VERSION >= 3020
		checkCudaErrors(cuDeviceGetAttribute(pbTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev));
		printf("  -> GPU %d: < %s > driver mode is: %s\n", dev, device_name, *pbTCC ? "TCC" : "WDDM");
#else

		// We don't know for sure if this is a TCC device or not, if it is Tesla we will not run
		if (!STRNCASECMP(device_name, "Tesla", 5))
		{
			printf("  \"%s\" does not support %s\n", device_name, sSDKname);
			*pbTCC = 1;
			return false;
		}
		else
		{
			*pbTCC = 0;
		}

#endif
	}

	if (!(*pbTCC))
	{
		// initialize GLUT
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
	}
	else
	{
		fprintf(stderr, "> %s is decoding w/o visualization\n", sSDKname);
	}

	return true;
}

bool initCudaResources()
{
	CUdevice cuda_device;

	// If we want to use Graphics Interop, then choose the GPU that is capable
	initGL();
	g_bUseInterop = true;

	if (g_bUseInterop && !(*bTCC))
	{
		cuda_device = findCudaGLDeviceDRV(argc, (const char **)argv);
		checkCudaErrors(cuDeviceGet(&g_oDevice, cuda_device));
	}
	else
	{
		cuda_device = findCudaDeviceDRV(argc, (const char **)argv);
		checkCudaErrors(cuDeviceGet(&g_oDevice, cuda_device));
	}

	// get compute capabilities and the devicename
	int major, minor;
	size_t totalGlobalMem;
	char deviceName[256];
	checkCudaErrors(cuDeviceComputeCapability(&major, &minor, g_oDevice));
	checkCudaErrors(cuDeviceGetName(deviceName, 256, g_oDevice));
	printf("> Using GPU Device %d: %s has SM %d.%d compute capability\n", cuda_device, deviceName, major, minor);

	checkCudaErrors(cuDeviceTotalMem(&totalGlobalMem, g_oDevice));
	printf("  Total amount of global memory:     %4.4f MB\n", (float)totalGlobalMem / (1024 * 1024));

	// Create CUDA Device w/ GL interop (if WDDM), otherwise CUDA w/o interop (if TCC)
	// (use CU_CTX_BLOCKING_SYNC for better CPU synchronization)
	if (g_bUseInterop && !(*bTCC))
	{
		checkCudaErrors(cuGLCtxCreate(&g_oContext, CU_CTX_BLOCKING_SYNC, g_oDevice));
	}
	else
	{
		checkCudaErrors(cuCtxCreate(&g_oContext, CU_CTX_BLOCKING_SYNC, g_oDevice));
	}

	try
	{
		// Initialize CUDA releated Driver API (32-bit or 64-bit), depending the platform running
		if (sizeof(void *) == 4)
		{
			g_pCudaModule = new CUmoduleManager("NV12ToARGB_drvapi_Win32.ptx", exec_path, 2, 2, 2);
		}
		else
		{
			g_pCudaModule = new CUmoduleManager("NV12ToARGB_drvapi_x64.ptx", exec_path, 2, 2, 2);
		}
	}
	catch (char const *p_file)
	{
		// If the CUmoduleManager constructor fails to load the PTX file, it will throw an exception
		printf("\n>> CUmoduleManager::Exception!  %s not found!\n", p_file);
		printf(">> Please rebuild NV12ToARGB_drvapi.cu or re-install this sample.\n");
		return false;
	}


	g_pCudaModule->GetCudaFunction("NV12ToARGB_drvapi", &g_kernelNV12toARGB);
	g_pCudaModule->GetCudaFunction("Passthru_drvapi", &g_kernelPassThru);

	/////////////////Change///////////////////////////
	// Now we create the CUDA resources and the CUDA decoder context
	initCudaVideo();

	if (g_bUseInterop)
	{
		initGLTexture(g_pVideoDecoder->targetWidth(),
			g_pVideoDecoder->targetHeight());
	}
	else
	{
		checkCudaErrors(cuMemAlloc(&g_pInteropFrame[0], g_pVideoDecoder->targetWidth() * g_pVideoDecoder->targetHeight() * 2));
		checkCudaErrors(cuMemAlloc(&g_pInteropFrame[1], g_pVideoDecoder->targetWidth() * g_pVideoDecoder->targetHeight() * 2));
	}

	CUcontext cuCurrent = NULL;
	CUresult result = cuCtxPopCurrent(&cuCurrent);

	if (result != CUDA_SUCCESS)
	{
		printf("cuCtxPopCurrent: %d\n", result);
		assert(0);
	}

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

	if (g_ReadbackSID)
	{
		checkCudaErrors(cuStreamDestroy(g_ReadbackSID));
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

//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;
class CTextureRenderer : public CBaseVideoRenderer
{
public:
	CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr);
	~CTextureRenderer();

public:
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	LONG	m_lVidWidth;   // Video width
	LONG	m_lVidHeight;  // Video Height
	LONG	m_lVidPitch;   // Video Pitch
	
	float	frameLength;

	LONGLONG sampleStart;
	LONGLONG sampleEnd;

	uint8_t	*m_pFrameBuffer;
};

unsigned int CNVStreaming::getWidth() const
{
	CTextureRenderer *txt = (CTextureRenderer *)m_pFilter;
	
	return txt->m_lVidWidth;
};

unsigned int CNVStreaming::getHeight() const
{
	CTextureRenderer *txt = (CTextureRenderer *)m_pFilter;

	return txt->m_lVidHeight;
};


bool CNVStreaming::readFrame(unsigned char *& readBuffer, float timestamp)
{
	if ((!locked) || (m_pControl == NULL))
		return false;

	renderVideoFrame(g_bUseInterop);

	CTextureRenderer *txt = (CTextureRenderer*)m_pFilter;
	readBuffer = txt->m_pFrameBuffer;
	streamPos++;

	if ((m_pRenderer->sampleEnd >= m_Duration) && (NULL != m_pSeeking))
	{
		seekFrame(0);
		streamPos = 0;
		m_pRenderer->sampleStart = 0;
		m_pRenderer->sampleEnd = 0;
	}

	return true;
}

//-----------------------------------------------------------------------------
// CTextureRenderer constructor
//-----------------------------------------------------------------------------
CTextureRenderer::CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr)
	: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr),
	m_lVidWidth(0), m_lVidHeight(0), m_lVidPitch(0), frameLength(0.0f), m_pFrameBuffer(NULL),
	sampleStart(0), sampleEnd(0)
{
	if (phr)
		*phr = S_OK;
}


//-----------------------------------------------------------------------------
// CTextureRenderer destructor
//-----------------------------------------------------------------------------
CTextureRenderer::~CTextureRenderer()
{
	if (NULL != m_pFrameBuffer)
		delete[] m_pFrameBuffer;
}

//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::DoRenderSample(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	long size = pSample->GetActualDataLength();

	// Get the video bitmap buffer
	BYTE  *pBmpBuffer;	// Bitmap buffer
	HRESULT hr = pSample->GetPointer(&pBmpBuffer);

	if (SUCCEEDED(hr) && (NULL != m_pFrameBuffer))
		memcpy(m_pFrameBuffer, pBmpBuffer, min(size, m_lVidPitch*m_lVidHeight));

	hr = pSample->GetTime(&sampleStart, &sampleEnd);

	return hr;
}

bool
loadVideoSource(const char *video_file,
	unsigned int &width, unsigned int &height,
	unsigned int &dispWidth, unsigned int &dispHeight)
{
	std::auto_ptr<FrameQueue> apFrameQueue(new FrameQueue);
	std::auto_ptr<VideoSource> apVideoSource(new VideoSource(video_file, apFrameQueue.get()));

	// retrieve the video source (width,height)
	apVideoSource->getSourceDimensions(width, height);
	apVideoSource->getSourceDimensions(dispWidth, dispHeight);

	std::cout << apVideoSource->format() << std::endl;

	if (g_bFrameRepeat)
	{
		if (apVideoSource->format().frame_rate.denominator > 0)
		{
			g_iRepeatFactor = (int)(60.0f / ceil((float)apVideoSource->format().frame_rate.numerator / (float)apVideoSource->format().frame_rate.denominator));
		}
	}

	printf("Frame Rate Playback Speed = %d fps\n", 60 / g_iRepeatFactor);

	g_pFrameQueue = apFrameQueue.release();
	g_pVideoSource = apVideoSource.release();

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

	// Determine the proper window size needed to create the correct *client* area
	// that is of the size requested by m_dimensions.
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	RECT adjustedWindowSize;
	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	SetRect(&adjustedWindowSize, 0, 0, g_nVideoWidth, g_nVideoHeight);
	AdjustWindowRect(&adjustedWindowSize, dwWindowStyle, false);
#endif

	g_nVideoWidth = PAD_ALIGN(g_nVideoWidth, 0x3F);
	g_nVideoHeight = PAD_ALIGN(g_nVideoHeight, 0x0F);

	// Initialize CUDA and try to connect with an OpenGL context
	// Other video memory resources will be available
	if (initCudaResources() == false)
	{
		g_bAutoQuit = true;
		g_bException = true;
		g_bWaived = true;
		goto ExitApp;
	}

	g_pVideoSource->start();
	g_bRunning = true;

	locked = true;
	return true;
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

