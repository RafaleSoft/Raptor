#!/bin/sh

echo    Preparing redist structure...
echo    Root of Raptor is set to: [$RAPTOR_ROOT] 

if  ( test -z "$RAPTOR_ROOT" )
then
        echo "Root is not defined, job aborted !"
        exit 1
fi

cd $RAPTOR_ROOT

if  ( test -d Redist )
then
	echo "Redist folder exist, it will be updated"
else
	echo "Creating redist folders..."

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
        mkdir GLXSpecific
        mkdir ToolBox
        cd ToolBox
        mkdir Filters
        cd ..
	mkdir DataManager
        cd Subsys
        mkdir SimdLib
        cd ..
        cd ..
        cd ..
fi

echo "	Copying Raptor include files ..."
echo "		Engine:"
cp ./RaptorCore/Engine/3DEngine.h Redist/Include/Engine
cp ./RaptorCore/Engine/3DEngineMatrix.h Redist/Include/Engine
cp ./RaptorCore/Engine/3DPath.h Redist/Include/Engine
cp ./RaptorCore/Engine/3DScene.h Redist/Include/Engine
cp ./RaptorCore/Engine/3DSqueleton.h Redist/Include/Engine
cp ./RaptorCore/Engine/Animator.h Redist/Include/Engine
cp ./RaptorCore/Engine/BoundingBox.h Redist/Include/Engine
cp ./RaptorCore/Engine/EngineJob.h Redist/Include/Engine
cp ./RaptorCore/Engine/Environment.h Redist/Include/Engine
cp ./RaptorCore/Engine/GeometricModifier.h Redist/Include/Engine
cp ./RaptorCore/Engine/ImageModifier.h Redist/Include/Engine
cp ./RaptorCore/Engine/LightModifier.h Redist/Include/Engine
cp ./RaptorCore/Engine/Modifier.h Redist/Include/Engine
cp ./RaptorCore/Engine/ParticleManager.h Redist/Include/Engine
cp ./RaptorCore/Engine/Physics.h Redist/Include/Engine
cp ./RaptorCore/Engine/TimeObject.h Redist/Include/Engine
cp ./RaptorCore/Engine/ViewModifier.h Redist/Include/Engine
cp ./RaptorCore/Engine/ViewPoint.h Redist/Include/Engine

echo "		GLHierarchy:"
cp ./RaptorCore/GLHierarchy/3DSet.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/BumppedGeometry.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/FragmentProgram.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/FragmentShader.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/GLFont.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/GLLayer.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/GLLod.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Geometry.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/GeometryEditor.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/GeometryPrimitive.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Light.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/LightObserver.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Material.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Object3D.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Object3DContour.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Object3DInstance.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Object3DInstanceSet.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Object3DShadow.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/ObjectFactory.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/ObjectProperties.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/ObjectReference.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Particle.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/PerlinNoise.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Persistence.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Projector.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/RenderingProperties.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/ShadedGeometry.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/Shader.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/ShaderProgram.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/SimpleObject.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureFactory.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureFactoryConfig.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureGenerator.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureObject.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureSet.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/TextureUnitSetup.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/UnifiedProgram.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/VertexProgram.h Redist/Include/GLHierarchy
cp ./RaptorCore/GLHierarchy/VertexShader.h Redist/Include/GLHierarchy

echo "		SSE_Engine:"
cp ./RaptorCore/SSE_Engine/SSE_3DEngine.h Redist/Include/SSE_Engine
cp ./RaptorCore/SSE_Engine/SSE_BoundingBox.h Redist/Include/SSE_Engine

echo "		System:"
cp ./RaptorCore/System/Color.h Redist/Include/System
cp ./RaptorCore/System/Glext.h Redist/Include/System
cp ./RaptorCore/System/GLEXTMacros.h Redist/Include/System
cp ./RaptorCore/System/CGLTypes.h Redist/Include/System
cp ./RaptorCore/System/Memory.h Redist/Include/System
cp ./RaptorCore/System/Raptor.h Redist/Include/System
cp ./RaptorCore/System/RaptorApplication.h Redist/Include/System
cp ./RaptorCore/System/RaptorConfig.h Redist/Include/System
cp ./RaptorCore/System/RaptorConsole.h Redist/Include/System
cp ./RaptorCore/System/RaptorDisplay.h Redist/Include/System
cp ./RaptorCore/System/RaptorDisplayConfig.h Redist/Include/System
cp ./RaptorCore/System/RaptorDisplayFilter.h Redist/Include/System
cp ./RaptorCore/System/RaptorErrorManager.h Redist/Include/System
cp ./RaptorCore/System/RaptorEventManager.h Redist/Include/System
cp ./RaptorCore/System/RaptorExtensions.h Redist/Include/System
cp ./RaptorCore/System/RaptorIO.h Redist/Include/System
cp ./RaptorCore/System/RaptorMessages.h Redist/Include/System
cp ./RaptorCore/System/RenderEntryPoint.h Redist/Include/System

echo "		GLX:"
cp ./RaptorCore/GLXSpecific/GLXGlext.h Redist/Include/GLXSpecific

echo "		Common:"
cp ./RaptorCore/Raptordll.h Redist/Include


echo "	Copying ToolBox files:"
cp ./RaptorToolBox/BasicObjects.h Redist/Include/ToolBox
cp ./RaptorToolBox/Controllers.h Redist/Include/ToolBox
cp ./RaptorToolBox/Imaging.h Redist/Include/ToolBox
cp ./RaptorToolBox/RaptorToolBox.h Redist/Include/ToolBox
cp ./RaptorToolBox/Streaming.h Redist/Include/ToolBox

cp ./RaptorToolBox/Filters/BlurFilter.h Redist/Include/ToolBox/Filters
cp ./RaptorToolBox/Filters/ColorControlFilter.h Redist/Include/ToolBox/Filters
cp ./RaptorToolBox/Filters/DOFFilter.h Redist/Include/ToolBox/Filters
cp ./RaptorToolBox/Filters/HDRFilter.h Redist/Include/ToolBox/Filters
cp ./RaptorToolBox/Filters/MagnifierFilter.h Redist/Include/ToolBox/Filters
cp ./RaptorToolBox/Filters/MBFilter.h Redist/Include/ToolBox/Filters

echo "	Copying RaptorData files:"
cp ./RaptorData/RaptorDataManager.h Redist/Include/DataManager


echo "	Copying Subsys files:"
echo "		Simd files ..."

cp ./RaptorCore/Subsys/SimdLib/BVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/FVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericAlignedMatrix.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericAlignedVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericMatrix.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericVectorArray.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/MMXBVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/MMXWVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/MMXWMatrix.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/SIMD.H Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/SSEFMatrix.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/SSEFVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/SSEFVectorArray.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/WVector.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/SIMDRegister.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/Registers.h Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/simdMacros.h Redist/Include/Subsys/SimdLib

cp ./RaptorCore/Subsys/SimdLib/SIMDRegister.cxx Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericVector.cxx Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericVectorArray.cxx Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericAlignedVector.cxx Redist/Include/Subsys/SimdLib
cp ./RaptorCore/Subsys/SimdLib/GenericMatrix.cxx Redist/Include/Subsys/SimdLib

cp ./RaptorCore/Subsys/SimdLib/simd.lib Redist/Lib/

echo "		Xml files ..."
cp ./AddOns/xercesc_2_7/libxerces-c.so.27.0 Redist/Lib/libxerces-c.so

echo "		Copying help files ..."

cp ./Installer/Raptor.html Redist/Doc
cp ./Installer/Raptor_splash.JPG Redist/Doc/html


echo "		Copying license files ..."

cp ./License/FREETYPE.TXT Redist/Licenses
cp ./License/DOXYGEN.txt Redist/Licenses
cp ./License/XERCESC.txt Redist/Licenses
cp ./License/APACHE.TXT Redist/Licenses
cp ./License/OPENEXR.txt Redist/Licenses
cp ./License/LIBPNG.TXT Redist/Licenses

echo "		Copying builder tools ..."
cp ./Builder/Microlex Redist/Bin

echo "Redist complete" > Builder/Redist.log

