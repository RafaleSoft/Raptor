'''
    /***************************************************************************/
    /*                                                                         */
    /*  Redist.bat                                                             */
    /*                                                                         */
    /*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
    /*                                                                         */
    /*  Copyright 1998-2019 by                                                 */
    /*  Fabrice FERRAND.                                                       */
    /*                                                                         */
    /*  This file is part of the Raptor project, and may only be used,         */
    /*  modified, and distributed under the terms of the Raptor project        */
    /*  license, LICENSE.  By continuing to use, modify, or distribute         */
    /*  this file you indicate that you have read the license and              */
    /*  understand and accept it fully.                                        */
    /*                                                                         */
    /***************************************************************************/
'''


import os
import sys
import shutil

RAPTOR_ROOT = os.environ.get("RAPTOR_ROOT")
PLATFORM = os.environ.get("PLATFORM")
RAPTOR_VERSION = os.environ.get("RAPTOR_VERSION")
PUBLISH = os.environ.get("PUBLISH")

PLATFORM="x86"
if len(sys.argv) > 0:
    PLATFORM = sys.argv[1] 

print("Preparing redist structure...")
print("Root of Raptor is set to: [", RAPTOR_ROOT, "]")
print("Platform is defined as: [", PLATFORM, "]")
print("Raptor Version is defined as: [", RAPTOR_VERSION, "]")
print("\n")


if len(RAPTOR_ROOT) < 1:
	print("Raptor Root is not defined, job aborted !");
	exit(-1);
if not os.path.exists(RAPTOR_ROOT):
    print("Raptor Root is not accessible, job aborted !");
    exit(-1);
if len(RAPTOR_VERSION) < 1:
	print("Raptor Version is not defined, job aborted !");
	exit(-1);


current_dir = os.path.curdir
os.chdir(RAPTOR_ROOT)


if os.path.exists('Redist'):
	print("Redist folder exist, it will be updated")
else:
    print("Creating redist folders ...")
    os.mkdir('Redist')
    os.chdir('Redist')
    os.mkdir('Include')
    os.mkdir('Bin')
    os.mkdir('Doc')
    os.chdir('Doc')
    os.mkdir('html')
    os.chdir('..')
    os.mkdir('Lib')
    os.mkdir('Licenses')
    os.chdir('Include')
    os.mkdir('Engine')
    os.mkdir('GLHierarchy')
    os.mkdir('SSE_Engine')
    os.mkdir('System')
    os.mkdir('Subsys')
    os.mkdir('Win32Specific')
    os.mkdir('RaptorCompute')
    os.mkdir('RaptorNetwork')
    os.mkdir('ToolBox')
    os.mkdir('SimdLib')
    os.chdir('ToolBox')
    os.mkdir('Filters')
    os.mkdir('Imaging')
    os.chdir('..')
    os.mkdir('DataManager')
    os.mkdir('MFCExtension')
    os.chdir('MFCExtension')
    os.mkdir('CWnd')
    os.mkdir('Dialog')
    os.chdir('..')
    os.chdir('Subsys')
    os.chdir('..')
    os.chdir('..')
    os.chdir('..')

s=os.path.sep

print("Copying Raptor include files ...")

print(" Configure:")
src = 'Builder'+s+'Configure'+s
dst = 'Redist'+s+'Include'+s
shutil.copyfile(src+'Portability.h', dst+'Portability.h')
shutil.copyfile(src+'Version.h', dst+'Version.h')


print(" Engine:")

src = 'RaptorCore'+s+'Engine'+s
dst = 'Redist'+s+'Include'+s+'Engine'+s
shutil.copyfile(src+'3DEngine.h', dst+'3DEngine.h')
shutil.copyfile(src+'3DEngineMatrix.h', dst+'3DEngineMatrix.h')
shutil.copyfile(src+'3DPath.h', dst+'3DPath.h')
shutil.copyfile(src+'3DScene.h', dst+'3DScene.h')
shutil.copyfile(src+'3DSqueleton.h', dst+'3DSqueleton.h')
shutil.copyfile(src+'Animator.h', dst+'Animator.h')
shutil.copyfile(src+'BoundingBox.h', dst+'BoundingBox.h')
shutil.copyfile(src+'EngineJob.h', dst+'EngineJob.h')
shutil.copyfile(src+'Environment.h', dst+'Environment.h')
shutil.copyfile(src+'GeometricModifier.h', dst+'GeometricModifier.h')
shutil.copyfile(src+'ImageModifier.h', dst+'ImageModifier.h')
shutil.copyfile(src+'LightModifier.h', dst+'LightModifier.h')
shutil.copyfile(src+'Mirror.h', dst+'Mirror.h')
shutil.copyfile(src+'Modifier.h', dst+'Modifier.h')
shutil.copyfile(src+'ParticleManager.h', dst+'ParticleManager.h')
shutil.copyfile(src+'Physics.h', dst+'Physics.h')
shutil.copyfile(src+'TimeObject.h', dst+'TimeObject.h')
shutil.copyfile(src+'ViewModifier.h', dst+'ViewModifier.h')
shutil.copyfile(src+'IViewPoint.h', dst+'IViewPoint.h')


print(" GLHierarchy:")

src = 'RaptorCore'+s+'GLHierarchy'+s
dst = 'Redist'+s+'Include'+s+'GLHierarchy'+s
shutil.copyfile(src+'3DSet.h', dst+'3DSet.h')
shutil.copyfile(src+'BumppedGeometry.h', dst+'BumppedGeometry.h')
shutil.copyfile(src+'FragmentShader.h', dst+'FragmentShader.h')
shutil.copyfile(src+'FragmentProgram.h', dst+'FragmentProgram.h')
shutil.copyfile(src+'GLFont.h', dst+'GLFont.h')
shutil.copyfile(src+'GL2DFont.h', dst+'GL2DFont.h')
shutil.copyfile(src+'GL2DTextureFont.h', dst+'GL2DTextureFont.h')
shutil.copyfile(src+'GL3DFont.h', dst+'GL3DFont.h')
shutil.copyfile(src+'GLVectorFont.h', dst+'GLVectorFont.h')
shutil.copyfile(src+'GLFontFactory.h', dst+'GLFontFactory.h')
shutil.copyfile(src+'GLLayer.h', dst+'GLLayer.h')
shutil.copyfile(src+'GLLod.h', dst+'GLLod.h')
shutil.copyfile(src+'Geometry.h', dst+'Geometry.h')
shutil.copyfile(src+'GeometryEditor.h', dst+'GeometryEditor.h')
shutil.copyfile(src+'GeometryPrimitive.h', dst+'GeometryPrimitive.h')
shutil.copyfile(src+'GeometryShader.h', dst+'GeometryShader.h')
shutil.copyfile(src+'ITextureObject.h', dst+'ITextureObject.h')
shutil.copyfile(src+'Light.h', dst+'Light.h')
shutil.copyfile(src+'LightGlow.h', dst+'LightGlow.h')
shutil.copyfile(src+'LightFlare.h', dst+'LightFlare.h')
shutil.copyfile(src+'LightObserver.h', dst+'LightObserver.h')
shutil.copyfile(src+'Material.h', dst+'Material.h')
shutil.copyfile(src+'Object3D.h', dst+'Object3D.h')
shutil.copyfile(src+'Object3DContour.h', dst+'Object3DContour.h')
shutil.copyfile(src+'Object3DInstance.h', dst+'Object3DInstance.h')
shutil.copyfile(src+'Object3DInstanceSet.h', dst+'Object3DInstanceSet.h')
shutil.copyfile(src+'Object3DShadow.h', dst+'Object3DShadow.h')
shutil.copyfile(src+'ObjectFactory.h', dst+'ObjectFactory.h')
shutil.copyfile(src+'ObjectProperties.h', dst+'ObjectProperties.h')
shutil.copyfile(src+'ObjectReference.h', dst+'ObjectReference.h')
shutil.copyfile(src+'OpenGLProgramStage.h', dst+'OpenGLProgramStage.h')
shutil.copyfile(src+'OpenGLShaderStage.h', dst+'OpenGLShaderStage.h')
shutil.copyfile(src+'Reference.cxx', dst+'Reference.cxx')
shutil.copyfile(src+'Particle.h', dst+'Particle.h')
shutil.copyfile(src+'PerlinNoise.h', dst+'PerlinNoise.h')
shutil.copyfile(src+'Persistence.h', dst+'Persistence.h')
shutil.copyfile(src+'ProgramParameters.h', dst+'ProgramParameters.h')
shutil.copyfile(src+'Projector.h', dst+'Projector.h')
shutil.copyfile(src+'IRenderingProperties.h', dst+'IRenderingProperties.h')
shutil.copyfile(src+'ShadedGeometry.h', dst+'ShadedGeometry.h')
shutil.copyfile(src+'Shader.h', dst+'Shader.h')
shutil.copyfile(src+'ShaderProgram.h', dst+'ShaderProgram.h')
shutil.copyfile(src+'SimpleObject.h', dst+'SimpleObject.h')
shutil.copyfile(src+'TextureFactory.h', dst+'TextureFactory.h')
shutil.copyfile(src+'TextureGenerator.h', dst+'TextureGenerator.h')
shutil.copyfile(src+'TextureFactoryConfig.h', dst+'TextureFactoryConfig.h')
shutil.copyfile(src+'TextureObject.h', dst+'TextureObject.h')
shutil.copyfile(src+'TextureSet.h', dst+'TextureSet.h')
shutil.copyfile(src+'TextureUnitSetup.h', dst+'TextureUnitSetup.h')
shutil.copyfile(src+'UnifiedShader.h', dst+'UnifiedShader.h')
shutil.copyfile(src+'VertexShader.h', dst+'VertexShader.h')
shutil.copyfile(src+'VertexProgram.h', dst+'VertexProgram.h')
shutil.copyfile(src+'VulkanShaderStage.h', dst+'VulkanShaderStage.h')

print(" SSE_Engine:")

src = 'RaptorCore'+s+'SSE_Engine'+s
dst = 'Redist'+s+'Include'+s+'SSE_Engine'+s
shutil.copyfile(src+'SSE_3DEngine.h', dst+'SSE_3DEngine.h')
shutil.copyfile(src+'SSE_BoundingBox.h', dst+'SSE_BoundingBox.h')
shutil.copyfile(src+'SSE_GLLayer.h', dst+'SSE_GLLayer.h')

print(" System:")

src = 'RaptorCore'+s+'System'+s
dst = 'Redist'+s+'Include'+s+'System'+s
shutil.copyfile(src+'CGLTypes.h', dst+'CGLTypes.h')
shutil.copyfile(src+'Color.h', dst+'Color.h')
shutil.copyfile(src+'Glext.h', dst+'Glext.h')
shutil.copyfile(src+'Image.h', dst+'Image.h')
shutil.copyfile(src+'Memory.h', dst+'Memory.h')
shutil.copyfile(src+'Raptor.h', dst+'Raptor.h')
shutil.copyfile(src+'RaptorApplication.h', dst+'RaptorApplication.h')
shutil.copyfile(src+'RaptorConfig.h', dst+'RaptorConfig.h')
shutil.copyfile(src+'RaptorConsole.h', dst+'RaptorConsole.h')
shutil.copyfile(src+'RaptorDisplay.h', dst+'RaptorDisplay.h')
shutil.copyfile(src+'RaptorDisplayConfig.h', dst+'RaptorDisplayConfig.h')
shutil.copyfile(src+'RaptorDisplayFilter.h', dst+'RaptorDisplayFilter.h')
shutil.copyfile(src+'RaptorErrorManager.h', dst+'RaptorErrorManager.h')
shutil.copyfile(src+'RaptorEventManager.h', dst+'RaptorEventManager.h')
shutil.copyfile(src+'RaptorGLExtensions.h', dst+'RaptorGLExtensions.h')
shutil.copyfile(src+'RaptorVKExtensions.h', dst+'RaptorVKExtensions.h')
shutil.copyfile(src+'RaptorIO.h', dst+'RaptorIO.h')
shutil.copyfile(src+'RaptorMessages.h', dst+'RaptorMessages.h')
shutil.copyfile(src+'RenderEntryPoint.h', dst+'RenderEntryPoint.h')
shutil.copyfile(src+'vkext.h', dst+'vkext.h')

print(" Win32:")

shutil.copyfile('RaptorCore'+s+'Win32Specific'+s+'Win32Glext.h', 'Redist'+s+'Include'+s+'Win32Specific'+s+'Win32Glext.h')

print(" MFCExtension:")

src = 'RaptorMFCExtension'+s+'CWnd'+s
dst = 'Redist'+s+'Include'+s+'MFCExtension'+s+'CWnd'+s
shutil.copyfile(src+'GLDocument.h', dst+'GLDocument.h')
shutil.copyfile(src+'GLFrameWnd.h', dst+'GLFrameWnd.h')
shutil.copyfile(src+'GLMDIChildWnd.h', dst+'GLMDIChildWnd.h')
shutil.copyfile(src+'GLView.h', dst+'GLView.h')
shutil.copyfile(src+'GLWnd.h', dst+'GLWnd.h')
shutil.copyfile(src+'MultiGLWnd.h', dst+'MultiGLWnd.h')
shutil.copyfile('RaptorMFCExtension'+s+'Dialog'+s+'RaptorStatus.h', 'Redist'+s+'Include'+s+'MFCExtension'+s+'Dialog'+s+'RaptorStatus.h')
shutil.copyfile('RaptorMFCExtension'+s+'RaptorMFCApplication.h', 'Redist'+s+'Include'+s+'MFCExtension'+s+'RaptorMFCApplication.h')

print(" Common:")

shutil.copyfile('RaptorCore'+s+'Raptordll.h', 'Redist'+s+'Include'+s+'Raptordll.h')

print("Copying ToolBox files:")

src = 'RaptorToolBox'+s
dst = 'Redist'+s+'Include'+s+'ToolBox'+s
shutil.copyfile(src+'BasicObjects.h', dst+'BasicObjects.h')
shutil.copyfile(src+'CmdLineParser.h', dst+'CmdLineParser.h')
shutil.copyfile(src+'Controllers.h', dst+'Controllers.h')
shutil.copyfile(src+'Imaging.h', dst+'Imaging.h')
shutil.copyfile(src+'ParticleCompute.h', dst+'ParticleCompute.h')
shutil.copyfile(src+'RaptorToolBox.h', dst+'RaptorToolBox.h')
shutil.copyfile(src+'Filters.h', dst+'Filters.h')
shutil.copyfile(src+'Streaming.h', dst+'Streaming.h')
shutil.copyfile(src+'Imaging'+s+'AlphaTransparency.h', dst+'Imaging'+s+'AlphaTransparency.h')
shutil.copyfile(src+'Imaging'+s+'BumpmapLoader.h', dst+'Imaging'+s+'BumpmapLoader.h')
shutil.copyfile(src+'Imaging'+s+'ImageScaler.h', dst+'Imaging'+s+'ImageScaler.h')
shutil.copyfile(src+'Imaging'+s+'MipmapBuilder.h', dst+'Imaging'+s+'MipmapBuilder.h')
shutil.copyfile(src+'Filters'+s+'BlurFilter.h', dst+'Filters'+s+'BlurFilter.h')
shutil.copyfile(src+'Filters'+s+'ColorControlFilter.h', dst+'Filters'+s+'ColorControlFilter.h')
shutil.copyfile(src+'Filters'+s+'DOFFilter.h', dst+'Filters'+s+'DOFFilter.h')
shutil.copyfile(src+'Filters'+s+'HDRFilter.h', dst+'Filters'+s+'HDRFilter.h')
shutil.copyfile(src+'Filters'+s+'MagnifierFilter.h', dst+'Filters'+s+'MagnifierFilter.h')
shutil.copyfile(src+'Filters'+s+'MBFilter.h', dst+'Filters'+s+'MBFilter.h')

print("Copying RaptorData files:")

shutil.copyfile('RaptorData'+s+'RaptorDataManager.h', 'Redist'+s+'Include'+s+'DataManager'+s+'RaptorDataManager.h')

print("Copying RaptorCompute files:")

src = 'RaptorCompute'+s
dst = 'Redist'+s+'Include'+s+'RaptorCompute'+s
shutil.copyfile(src+'RaptorCompute.h', dst+'RaptorCompute.h')
shutil.copyfile(src+'RaptorComputeJob.h', dst+'RaptorComputeJob.h')
shutil.copyfile(src+'RaptorComputeMemory.h', dst+'RaptorComputeMemory.h')
shutil.copyfile(src+'RaptorComputeTask.h', dst+'RaptorComputeTask.h')
shutil.copyfile(src+'ReadBufferTask.h', dst+'ReadBufferTask.h')

print("Copying RaptorNetwork files:")

src = 'RaptorNetwork'+s
dst = 'Redist'+s+'Include'+s+'RaptorNetwork'+s
shutil.copyfile(src+'RaptorNetwork.h', dst+'RaptorNetwork.h')
shutil.copyfile(src+'Network.h', dst+'Network.h')
shutil.copyfile(src+'NetworkLogger.h', dst+'NetworkLogger.h')
shutil.copyfile(src+'Server.h', dst+'Server.h')
shutil.copyfile(src+'ServerSocket.h', dst+'ServerSocket.h')
shutil.copyfile(src+'Client.h', dst+'Client.h')
shutil.copyfile(src+'ClientSocket.h', dst+'ClientSocket.h')

print(" Copying Simd files:")

src = 'SIMD'+s
dst = 'Redist'+s+'Include'+s+'SimdLib'+s
shutil.copyfile(src+'BVector.h', dst+'BVector.h')
shutil.copyfile(src+'GenericAlignedMatrix.h', dst+'GenericAlignedMatrix.h')
shutil.copyfile(src+'GenericAlignedVector.h', dst+'GenericAlignedVector.h')
shutil.copyfile(src+'GenericMatrix.h', dst+'GenericMatrix.h')
shutil.copyfile(src+'GenericVector.h', dst+'GenericVector.h')
shutil.copyfile(src+'MMXBVector.h', dst+'MMXBVector.h')
shutil.copyfile(src+'MMXWVector.h', dst+'MMXWVector.h')
shutil.copyfile(src+'MMXWMatrix.h', dst+'MMXWMatrix.h')
shutil.copyfile(src+'SIMD.h', dst+'SIMD.h')
shutil.copyfile(src+'SSEFMatrix.h', dst+'SSEFMatrix.h')
shutil.copyfile(src+'SSEFVector.h', dst+'SSEFVector.h')
shutil.copyfile(src+'WVector.h', dst+'WVector.h')
shutil.copyfile(src+'SIMDRegister.h', dst+'SIMDRegister.h')
shutil.copyfile(src+'Registers.h', dst+'Registers.h')
shutil.copyfile(src+'simdMacros.h', dst+'simdMacros.h')
shutil.copyfile(src+'Vector4f.h', dst+'Vector4f.h')
shutil.copyfile(src+'Matrix4f.h', dst+'Matrix4f.h')

shutil.copyfile(src+'SIMDRegister.cxx', dst+'SIMDRegister.cxx')
shutil.copyfile(src+'GenericVector.cxx', dst+'GenericVector.cxx')
shutil.copyfile(src+'GenericAlignedVector.cxx', dst+'GenericAlignedVector.cxx')
shutil.copyfile(src+'GenericMatrix.cxx', dst+'GenericMatrix.cxx')
shutil.copyfile(src+'Vector4f.cxx', dst+'Vector4f.cxx')
shutil.copyfile(src+'Matrix4f.cxx', dst+'Matrix4f.cxx')

print(" Copying help files:")

shutil.copyfile('Installer'+s+'Raptor.html', 'Redist'+s+'Doc'+s+'Raptor.html')
shutil.copyfile('Installer'+s+'Raptor_splash.JPG', 'Redist'+s+'Doc'+s+'html'+s+'Raptor_splash.JPG')

print(" Copying license files:")

src = 'Licenses'+s
dst = 'Redist'+s+'Licenses'+s
shutil.copyfile(src+'APACHE.TXT', dst+'APACHE.TXT')
shutil.copyfile(src+'COPYRIGHT', dst+'COPYRIGHT')
shutil.copyfile(src+'DOXYGEN.txt', dst+'DOXYGEN.txt')
shutil.copyfile(src+'FREETYPE.TXT', dst+'FREETYPE.TXT')
shutil.copyfile(src+'FTL.TXT', dst+'FTL.TXT')
shutil.copyfile(src+'GPLv2.TXT', dst+'GPLv2.TXT')
shutil.copyfile(src+'lgpl-3.0.txt', dst+'lgpl-3.0.txt')
shutil.copyfile(src+'LIBPNG.TXT', dst+'LIBPNG.TXT')
shutil.copyfile(src+'LICENSE.TXT', dst+'LICENSE.TXT')
shutil.copyfile(src+'OPENEXR.TXT', dst+'OPENEXR.TXT')
shutil.copyfile(src+'XERCESC.TXT', dst+'XERCESC.TXT')
shutil.copyfile(src+'LICENSE.md', dst+'LICENSE.md')
shutil.copyfile(src+'PNG_LICENSE', dst+'PNG_LICENSE')
shutil.copyfile(src+'README', dst+'README')
shutil.copyfile(src+'XERCESC.txt', dst+'XERCESC.txt')


print(" Copying library files:")

lib = 'Redist'+s+'lib'+s
bin = 'Redist'+s+'bin'+s
JPEGLIB_LIB_PATH = os.environ.get("JPEGLIB_LIB_PATH")
shutil.copyfile(JPEGLIB_LIB_PATH+s+PLATFORM+s+'libjpeg.lib', lib+'libjpeg.lib')

ZLIB_BIN_PATH = os.environ.get("ZLIB_BIN_PATH")
shutil.copyfile(ZLIB_BIN_PATH+s+PLATFORM+s+'zlib.dll', bin+'zlib.dll')
ZLIB_LIB_PATH = os.environ.get("ZLIB_LIB_PATH")
shutil.copyfile(ZLIB_LIB_PATH+s+PLATFORM+s+'zlib.lib', lib+'zlib.lib')

TIFFLIB_LIB_PATH = os.environ.get("TIFFLIB_LIB_PATH")
shutil.copyfile(TIFFLIB_LIB_PATH+s+PLATFORM+s+'libtiff.lib', lib+'libtiff.lib')

PNGLIB_BIN_PATH = os.environ.get("PNGLIB_BIN_PATH")
shutil.copyfile(PNGLIB_BIN_PATH+s+PLATFORM+s+'libpng.dll', bin+'libpng.dll')
PNGLIB_LIB_PATH = os.environ.get("PNGLIB_LIB_PATH")
shutil.copyfile(PNGLIB_LIB_PATH+s+PLATFORM+s+'libpng.lib', lib+'libpng.lib')

FREETYPE_BIN_PATH = os.environ.get("FREETYPE_BIN_PATH")
shutil.copyfile(FREETYPE_BIN_PATH+s+PLATFORM+s+'freetype.dll', bin+'freetype.dll')
FREETYPE_LIB_PATH = os.environ.get("FREETYPE_LIB_PATH")
shutil.copyfile(FREETYPE_LIB_PATH+s+PLATFORM+s+'freetype.lib', lib+'freetype.lib')

XERCES_BIN_PATH = os.environ.get("XERCES_BIN_PATH")
shutil.copyfile(XERCES_BIN_PATH+s+PLATFORM+s+'xerces-c_3_2.dll', bin+'xerces-c_3_2.dll')
XERCES_LIB_PATH = os.environ.get("XERCES_LIB_PATH")
shutil.copyfile(XERCES_LIB_PATH+s+PLATFORM+s+'xerces-c_3.lib', lib+'xerces-c_3.lib')

OPENEXRLIB_BIN_PATH = os.environ.get("OPENEXRLIB_BIN_PATH")
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'Half.dll', bin+'Half.dll')
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'Iex.dll', bin+'Iex.dll')
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'IexMath.dll', bin+'IexMath.dll')
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'IMath.dll', bin+'IMath.dll')
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'IlmThread.dll', bin+'IlmThread.dll')
shutil.copyfile(OPENEXRLIB_BIN_PATH+s+PLATFORM+s+'IlmImf.dll', bin+'IlmImf.dll')
OPENEXRLIB_LIB_PATH = os.environ.get("OPENEXRLIB_LIB_PATH")
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'Half.lib', lib+'Half.lib')
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'Iex.lib', lib+'Iex.lib')
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'IexMath.lib', lib+'IexMath.lib')
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'IMath.lib', lib+'IMath.lib')
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'IlmThread.lib', lib+'IlmThread.lib')
shutil.copyfile(OPENEXRLIB_LIB_PATH+s+PLATFORM+s+'IlmImf.lib', lib+'IlmImf.lib')

CPPUNIT_BIN_PATH = os.environ.get("CPPUNIT_BIN_PATH")
shutil.copyfile(CPPUNIT_BIN_PATH+s+PLATFORM+s+'cppunit_dll.dll', bin+'cppunit_dll.dll')
CPPUNIT_LIB_PATH = os.environ.get("CPPUNIT_LIB_PATH")
shutil.copyfile(CPPUNIT_LIB_PATH+s+PLATFORM+s+'cppunit_dll.lib', lib+'cppunit_dll.lib')


os.chdir(current_dir)

print("\nJob done !\n")



