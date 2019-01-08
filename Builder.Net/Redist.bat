ECHO OFF

ECHO    Preparing redist structure...
ECHO    Root of Raptor is set to: [%RAPTOR_ROOT%]

IF NOT DEFINED RAPTOR_ROOT (
	ECHO    Root is not defined, job aborted !
	GOTO END
)

cd %RAPTOR_ROOT%

REM	Check if the redist directory is here

IF EXIST Redist (
	ECHO    Redist folder exist, it will be updated
) ELSE (
	ECHO    Creating redist folders...

	mkdir Redist
	cd Redist
	mkdir Include
	mkdir Bin
	mkdir Doc
	cd Doc
	mkdir html
	cd ..
	mkdir Lib
	mkdir Licenses
	cd Include
	mkdir Engine
	mkdir GLHierarchy
	mkdir SSE_Engine
	mkdir System
	mkdir Subsys
	mkdir Win32Specific
	mkdir RaptorCompute
	mkdir RaptorNetwork
	mkdir ToolBox
	mkdir SimdLib
	cd ToolBox
	mkdir Filters
	cd ..
	mkdir DataManager
	mkdir MFCExtension
	cd MFCExtension
	mkdir CWnd
	mkdir Dialog
	cd ..
	cd Subsys
	cd ..
	cd ..
	cd ..
)


ECHO    Copying Raptor include files ...
ECHO       Engine:
copy .\RaptorCore\Engine\3DEngine.h Redist\Include\Engine
copy .\RaptorCore\Engine\3DEngineMatrix.h Redist\Include\Engine
copy .\RaptorCore\Engine\3DPath.h Redist\Include\Engine
copy .\RaptorCore\Engine\3DScene.h Redist\Include\Engine
copy .\RaptorCore\Engine\3DSqueleton.h Redist\Include\Engine
copy .\RaptorCore\Engine\Animator.h Redist\Include\Engine
copy .\RaptorCore\Engine\BoundingBox.h Redist\Include\Engine
copy .\RaptorCore\Engine\EngineJob.h Redist\Include\Engine
copy .\RaptorCore\Engine\Environment.h Redist\Include\Engine
copy .\RaptorCore\Engine\GeometricModifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\ImageModifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\LightModifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\ShaderModifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\Mirror.h Redist\Include\Engine
copy .\RaptorCore\Engine\Modifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\ParticleManager.h Redist\Include\Engine
copy .\RaptorCore\Engine\Physics.h Redist\Include\Engine
copy .\RaptorCore\Engine\TimeObject.h Redist\Include\Engine
copy .\RaptorCore\Engine\ViewModifier.h Redist\Include\Engine
copy .\RaptorCore\Engine\IViewPoint.h Redist\Include\Engine

ECHO       GLHierarchy:
copy .\RaptorCore\GLHierarchy\3DSet.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\BumppedGeometry.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\FragmentProgram.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\FragmentShader.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GLFont.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GL2DFont.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GL2DTextureFont.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GL3DFont.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GLVectorFont.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GLFontFactory.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GLLayer.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GLLod.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Geometry.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GeometryEditor.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GeometryPrimitive.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\GeometryProgram.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ITextureObject.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Light.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\LightGlow.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\LightObserver.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Material.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Object3D.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Object3DContour.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Object3DInstance.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Object3DInstanceSet.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Object3DShadow.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ObjectFactory.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ObjectProperties.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ObjectReference.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Reference.cxx Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Particle.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\PerlinNoise.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Persistence.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ProgramParameters.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Projector.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\IRenderingProperties.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ShadedGeometry.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\Shader.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\ShaderProgram.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\SimpleObject.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureFactory.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureFactoryConfig.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureGenerator.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureObject.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureSet.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\TextureUnitSetup.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\UnifiedProgram.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\VertexProgram.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\VertexShader.h Redist\Include\GLHierarchy
copy .\RaptorCore\GLHierarchy\VulkanShaderStage.h Redist\Include\GLHierarchy

ECHO       SSE_Engine:
copy .\RaptorCore\SSE_Engine\SSE_3DEngine.h Redist\Include\SSE_Engine
copy .\RaptorCore\SSE_Engine\SSE_BoundingBox.h Redist\Include\SSE_Engine
copy .\RaptorCore\SSE_Engine\SSE_GLLayer.h Redist\Include\SSE_Engine

ECHO       System:
copy .\RaptorCore\System\CGLTypes.h Redist\Include\System
copy .\RaptorCore\System\Color.h Redist\Include\System
copy .\RaptorCore\System\Glext.h Redist\Include\System
copy .\RaptorCore\System\Image.h Redist\Include\System
copy .\RaptorCore\System\Memory.h Redist\Include\System
copy .\RaptorCore\System\Raptor.h Redist\Include\System
copy .\RaptorCore\System\RaptorApplication.h Redist\Include\System
copy .\RaptorCore\System\RaptorConfig.h Redist\Include\System
copy .\RaptorCore\System\RaptorConsole.h Redist\Include\System
copy .\RaptorCore\System\RaptorDisplay.h Redist\Include\System
copy .\RaptorCore\System\RaptorDisplayConfig.h Redist\Include\System
copy .\RaptorCore\System\RaptorDisplayFilter.h Redist\Include\System
copy .\RaptorCore\System\RaptorErrorManager.h Redist\Include\System
copy .\RaptorCore\System\RaptorEventManager.h Redist\Include\System
copy .\RaptorCore\System\RaptorGLExtensions.h Redist\Include\System
copy .\RaptorCore\System\RaptorVKExtensions.h Redist\Include\System
copy .\RaptorCore\System\RaptorIO.h Redist\Include\System
copy .\RaptorCore\System\RaptorMessages.h Redist\Include\System
copy .\RaptorCore\System\RenderEntryPoint.h Redist\Include\System
copy .\RaptorCore\System\Version.h Redist\Include\System
copy .\RaptorCore\System\vkext.h Redist\Include\System

ECHO       Win32:
copy .\RaptorCore\Win32Specific\Win32Glext.h Redist\Include\Win32Specific

ECHO       MFCExtension:
copy .\RaptorMFCExtension\CWnd\GLDocument.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\CWnd\GLFrameWnd.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\CWnd\GLMDIChildWnd.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\CWnd\GLView.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\CWnd\GLWnd.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\CWnd\MultiGLWnd.h Redist\Include\MFCExtension\CWnd
copy .\RaptorMFCExtension\Dialog\RaptorStatus.h Redist\Include\MFCExtension\Dialog
copy .\RaptorMFCExtension\RaptorMFCApplication.h Redist\Include\MFCExtension


ECHO       Common:
copy .\RaptorCore\Raptordll.h Redist\Include


ECHO    Copying ToolBox files:
copy .\RaptorToolBox\BasicObjects.h Redist\Include\ToolBox
copy .\RaptorToolBox\CmdLineParser.h Redist\Include\ToolBox
copy .\RaptorToolBox\Controllers.h Redist\Include\ToolBox
copy .\RaptorToolBox\Imaging.h Redist\Include\ToolBox
copy .\RaptorToolBox\ParticleCompute.h Redist\Include\ToolBox
copy .\RaptorToolBox\RaptorToolBox.h Redist\Include\ToolBox
copy .\RaptorToolBox\Streaming.h Redist\Include\ToolBox
copy .\RaptorToolBox\Filters\BlurFilter.h Redist\Include\ToolBox\Filters
copy .\RaptorToolBox\Filters\ColorControlFilter.h Redist\Include\ToolBox\Filters
copy .\RaptorToolBox\Filters\DOFFilter.h Redist\Include\ToolBox\Filters
copy .\RaptorToolBox\Filters\HDRFilter.h Redist\Include\ToolBox\Filters
copy .\RaptorToolBox\Filters\MagnifierFilter.h Redist\Include\ToolBox\Filters
copy .\RaptorToolBox\Filters\MBFilter.h Redist\Include\ToolBox\Filters

ECHO    Copying RaptorData files:
copy .\RaptorData\RaptorDataManager.h Redist\Include\DataManager

ECHO    Copying RaptorCompute files:
copy .\Raptorcompute\RaptorCompute.h Redist\Include\RaptorCompute
copy .\Raptorcompute\RaptorComputeJob.h Redist\Include\RaptorCompute
copy .\Raptorcompute\RaptorComputeMemory.h Redist\Include\RaptorCompute
copy .\Raptorcompute\RaptorComputeTask.h Redist\Include\RaptorCompute
copy .\Raptorcompute\ReadBufferTask.h Redist\Include\RaptorCompute

ECHO    Copying RaptorNetwork files:
copy .\RaptorNetwork\RaptorNetwork.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\Network.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\NetworkLogger.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\Server.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\ServerSocket.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\Client.h Redist\Include\RaptorNetwork
copy .\RaptorNetwork\ClientSocket.h Redist\Include\RaptorNetwork


ECHO    Copying Subsys files:

ECHO	   Microlex files ...

copy .\MicroLex\MicroLex.exe Redist\Bin

ECHO       Simd files ...

copy .\SIMD\BVector.h Redist\Include\SimdLib
copy .\SIMD\GenericAlignedMatrix.h Redist\Include\SimdLib
copy .\SIMD\GenericAlignedVector.h Redist\Include\SimdLib
copy .\SIMD\GenericMatrix.h Redist\Include\SimdLib
copy .\SIMD\GenericVector.h Redist\Include\SimdLib
copy .\SIMD\MMXBVector.h Redist\Include\SimdLib
copy .\SIMD\MMXWVector.h Redist\Include\SimdLib
copy .\SIMD\MMXWMatrix.h Redist\Include\SimdLib
copy .\SIMD\SIMD.H Redist\Include\SimdLib
copy .\SIMD\SSEFMatrix.h Redist\Include\SimdLib
copy .\SIMD\SSEFVector.h Redist\Include\SimdLib
copy .\SIMD\WVector.h Redist\Include\SimdLib
copy .\SIMD\SIMDRegister.h Redist\Include\SimdLib
copy .\SIMD\Registers.h Redist\Include\SimdLib
copy .\SIMD\simdMacros.h Redist\Include\SimdLib
copy .\SIMD\Vector4f.h Redist\Include\SimdLib
copy .\SIMD\Matrix4f.h Redist\Include\SimdLib

copy .\SIMD\SIMDRegister.cxx Redist\Include\SimdLib
copy .\SIMD\GenericVector.cxx Redist\Include\SimdLib
copy .\SIMD\GenericAlignedVector.cxx Redist\Include\SimdLib
copy .\SIMD\GenericMatrix.cxx Redist\Include\SimdLib
copy .\SIMD\Vector4f.cxx Redist\Include\SimdLib
copy .\SIMD\Matrix4f.cxx Redist\Include\SimdLib

copy .\SIMD\simd.lib Redist\Lib

ECHO    Copying help files ...

copy .\Installer\Raptor.html Redist\Doc
copy .\Installer\Raptor_splash.JPG Redist\Doc\html


ECHO    Copying license files ...

copy .\License\FREETYPE.TXT Redist\Licenses
copy .\License\DOXYGEN.txt Redist\Licenses
copy .\License\XERCESC.txt Redist\Licenses
copy .\License\APACHE.TXT Redist\Licenses
copy .\License\OPENEXR.txt Redist\Licenses
copy .\License\LIBPNG.TXT Redist\Licenses


ECHO    Copying sub-system files ...

copy "%JPEGLIB_LIB_PATH%\libjpeg.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%ZLIB_BIN_PATH%\zlib.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%ZLIB_LIB_PATH%\zlibs.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%ZLIB_LIB_PATH%\zlib.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%TIFFLIB_LIB_PATH%\libtiff.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%PNGLIB_BIN_PATH%\libpng.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%PNGLIB_LIB_PATH%\libpng.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%FREETYPE_BIN_PATH%\freetype.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%FREETYPE_BIN_PATH%\freetype.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%XERCES_BIN_PATH%\xerces-c_3_2.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%XERCES_LIB_PATH%\xerces-c_3.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_LIB_PATH%\Half.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\Half.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%OPENEXRLIB_LIB_PATH%\Iex.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\Iex.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%OPENEXRLIB_LIB_PATH%\IexMath.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\IexMath.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%OPENEXRLIB_LIB_PATH%\IMath.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\IMath.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%OPENEXRLIB_LIB_PATH%\IlmThread.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\IlmThread.dll" "%RAPTOR_ROOT%\Redist\bin"
copy "%OPENEXRLIB_LIB_PATH%\IlmImf.lib" "%RAPTOR_ROOT%\Redist\lib"
copy "%OPENEXRLIB_BIN_PATH%\IlmImf.dll" "%RAPTOR_ROOT%\Redist\bin"

ECHO Redist complete > Builder.Net\Redist.log


:END

