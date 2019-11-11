; -- Raptor.iss --

[Setup]
AppName=Raptor SDK
AppVerName=Raptor SDK 2.17.1
DefaultDirName={pf}\Raptor
DefaultGroupName=Raptor
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma
OutputBaseFilename=RaptorSDK
SolidCompression=yes
VersionInfoCompany=Rafale Soft Inc.(c)
VersionInfoProductName=Raptor SDK
VersionInfoProductVersion=2.17.1.0
VersionInfoVersion=2.17.1.0

[Files]
Source: "..\*.sln";               DestDir: "{app}"
Source: "..\*.Cmd";               DestDir: "{app}"
Source: "..\*.bat";               DestDir: "{app}"
Source: "..\Makefile";            DestDir: "{app}"
Source: "..\TODO.txt";            DestDir: "{app}"
Source: "..\README.md";           DestDir: "{app}"
Source: "..\LICENSE";             DestDir: "{app}"
Source: "..\.gitignore";          DestDir: "{app}"
Source: "..\.gitattributes";      DestDir: "{app}"

Source: "..\AddOns\*.lib";            DestDir: "{app}\AddOns"
Source: "..\AddOns\*.pdb";            DestDir: "{app}\AddOns"
Source: "..\AddOns\usertype.dat";     DestDir: "{app}\AddOns"
Source: "..\AddOns\freetype-2.9.1\*";  DestDir: "{app}\AddOns\freetype-2.9.1\"; Flags: recursesubdirs
Source: "..\AddOns\JpegLib-v9\*";      DestDir: "{app}\AddOns\JpegLib-v9\";     Flags: recursesubdirs
Source: "..\AddOns\libpng-1.6.36\*";   DestDir: "{app}\AddOns\libpng-1.6.36\";  Flags: recursesubdirs
Source: "..\AddOns\Opencl\Intel_v11\*.*";           DestDir: "{app}\AddOns\Opencl\Intel_v11"
Source: "..\AddOns\OpenEXR-2.3.0\*";    DestDir: "{app}\AddOns\OpenEXRLib-2.3.0\";    Flags: recursesubdirs
Source: "..\AddOns\Openmp\*";           DestDir: "{app}\AddOns\Openmp\";        Flags: recursesubdirs
Source: "..\AddOns\TiffLib-4.0.10\*";   DestDir: "{app}\AddOns\TiffLib-4.0.10\";        Flags: recursesubdirs
Source: "..\AddOns\zlib-1.2.11\*";         DestDir: "{app}\AddOns\zlib-1.2.11\";        Flags: recursesubdirs
Source: "..\AddOns\xercesc-3.2.2\*";       DestDir: "{app}\AddOns\xercesc-3.2.2\";        Flags: recursesubdirs
Source: "..\AddOns\vulkan\*";         DestDir: "{app}\AddOns\vulkan\";        Flags: recursesubdirs


Source: "../Build/Linux/Makefile.*";  DestDir: "{app}/Build"
Source: "../Build/VS2008/*.vcproj";   DestDir: "{app}/Build/VS2008"
Source: "../Build/VS2010/*.vcxproj";  DestDir: "{app}/Build/VS2010"
Source: "../Build/VS2010/*.filters";  DestDir: "{app}/Build/VS2010"
Source: "../Build/VS2013/*.vcxproj";  DestDir: "{app}/Build/VS2013"
Source: "../Build/VS2013/*.filters";  DestDir: "{app}/Build/VS2013"
Source: "../Build/VS2017/*.vcxproj";  DestDir: "{app}/Build/VS2017"
Source: "../Build/VS2017/*.filters";  DestDir: "{app}/Build/VS2017"
Source: "../Build/VS2019/*.vcxproj";  DestDir: "{app}/Build/VS2019"
Source: "../Build/VS2019/*.filters";  DestDir: "{app}/Build/VS2019"
Source: "../Build/Doxyfile";          DestDir: "{app}/Build"

Source: "../Builder/BuilderNative/*.cpp";           DestDir: "{app}/Builder/BuilderNative"
Source: "../Builder/BuilderNative/*.h";             DestDir: "{app}/Builder/BuilderNative"
Source: "../Builder/Configure/*.bat";               DestDir: "{app}/Builder/Configure"
Source: "../Builder/Configure/*.sh";                DestDir: "{app}/Builder/Configure"
Source: "../Builder/Properties/*.*";                DestDir: "{app}/Builder/Properties"
Source: "../Builder/Resource Files/*.*";            DestDir: "{app}/Builder/Resource Files"
Source: "../Builder/*.config";                      DestDir: "{app}/Builder"
Source: "../Builder/*.csproj";                      DestDir: "{app}/Builder"
Source: "../Builder/*.cs";                          DestDir: "{app}/Builder"
Source: "../Builder/*.resx";                        DestDir: "{app}/Builder"
Source: "../Builder/*.cpp";                         DestDir: "{app}/Builder"
Source: "../Builder/*.rc";                          DestDir: "{app}/Builder"
Source: "../Builder/*.h";                           DestDir: "{app}/Builder"

Source: "../Demo/Datas/*.*";          DestDir: "{app}/Demo/Datas"
Source: "../Demo/Ressources/*.*";     DestDir: "{app}/Demo/Ressources"
Source: "../Demo/Demo.cmd";           DestDir: "{app}/Demo"
Source: "../Demo/*.cpp";              DestDir: "{app}/Demo"
Source: "../Demo/*.h";                DestDir: "{app}/Demo"
Source: "../Demo/Demo.xml";           DestDir: "{app}/Demo"
Source: "../Demo/buildpak.*";         DestDir: "{app}/Demo"

Source: "../GLBench/Datas/*.*";       DestDir: "{app}/GLBench/Datas"
Source: "../GLBench/res/*.*";         DestDir: "{app}/GLBench/res"
Source: "../GLBench/*.rc";            DestDir: "{app}/GLBench"
Source: "../GLBench/*.cpp";           DestDir: "{app}/GLBench"
Source: "../GLBench/*.h";             DestDir: "{app}/GLBench"
Source: "../GLBench/buildpak.*";      DestDir: "{app}/GLBench"
Source: "../GLBench/GLBenchFillRate/*.cpp";           DestDir: "{app}/GLBench/GLBenchFillRate"
Source: "../GLBench/GLBenchFillRate/*.h";             DestDir: "{app}/GLBench/GLBenchFillRate"
Source: "../GLBench/GLBenchModuleTemplate/*.cpp";           DestDir: "{app}/GLBench/GLBenchModuleTemplate"
Source: "../GLBench/GLBenchModuleTemplate/*.h";             DestDir: "{app}/GLBench/GLBenchModuleTemplate"
Source: "../GLBench/GLBenchPoints/*.cpp";           DestDir: "{app}/GLBench/GLBenchPoints"
Source: "../GLBench/GLBenchPoints/*.h";             DestDir: "{app}/GLBench/GLBenchPoints"
Source: "../GLBench/GLBenchPolygons/*.cpp";           DestDir: "{app}/GLBench/GLBenchPolygons"
Source: "../GLBench/GLBenchPolygons/*.h";             DestDir: "{app}/GLBench/GLBenchPolygons"

Source: "../GLObjectViewer/res/*.*";         DestDir: "{app}/GLObjectViewer/res"
Source: "../GLObjectViewer/*.rc";            DestDir: "{app}/GLObjectViewer"
Source: "../GLObjectViewer/*.cpp";           DestDir: "{app}/GLObjectViewer"
Source: "../GLObjectViewer/*.h";             DestDir: "{app}/GLObjectViewer"

Source: "../Installer/Raptor_fichiers/*.*";     DestDir: "{app}/Installer/Raptor_fichiers"
Source: "../Installer/Raptor SDK.sln";          DestDir: "{app}/Installer"
Source: "../Installer/*.bmp";                   DestDir: "{app}/Installer"
Source: "../Installer/*.tif";                   DestDir: "{app}/Installer"
Source: "../Installer/*.jpg";                   DestDir: "{app}/Installer"
Source: "../Installer/Raptor.iss";              DestDir: "{app}/Installer"
Source: "../Installer/Raptor.html";             DestDir: "{app}/Installer"

Source: "../Licenses/*.*";                       DestDir: "{app}/Licenses"
Source: "../Licenses/README";                    DestDir: "{app}/Licenses"

Source: "../MicroLex/*.*";                      DestDir: "{app}/MicroLex"
    
Source: "../RaptorCompute/*.cpp";               DestDir: "{app}/RaptorCompute"
Source: "../RaptorCompute/*.h";                 DestDir: "{app}/RaptorCompute"

Source: "../RaptorCore/Engine/*.*";             DestDir: "{app}/RaptorCore/Engine"
Source: "../RaptorCore/GLHierarchy/*.*";        DestDir: "{app}/RaptorCore/GLHierarchy"
Source: "../RaptorCore/GLXSpecific/*.*";        DestDir: "{app}/RaptorCore/GLXSpecific"
Source: "../RaptorCore/SSE_Engine/*.*";         DestDir: "{app}/RaptorCore/SSE_Engine"
Source: "../RaptorCore/Win32Specific/*.*";      DestDir: "{app}/RaptorCore/Win32Specific"
Source: "../RaptorCore/System/*.*";             DestDir: "{app}/RaptorCore/System"
Source: "../RaptorCore/Subsys/*.*";             DestDir: "{app}/RaptorCore/Subsys"
Source: "../RaptorCore/Subsys/Hershey/*.*";     DestDir: "{app}/RaptorCore/Subsys/Hershey"
Source: "../RaptorCore/Subsys/ObjectModels/*.*";     DestDir: "{app}/RaptorCore/Subsys/ObjectModels"
Source: "../RaptorCore/Subsys/FreeType/*.*";    DestDir: "{app}/RaptorCore/Subsys/SimdLib"
Source: "../RaptorCore/Subsys/OpenGL/*.*";      DestDir: "{app}/RaptorCore/Subsys/SimdLib"
Source: "../RaptorCore/Subsys/Vulkan/*.*";      DestDir: "{app}/RaptorCore/Subsys/SimdLib"
Source: "../RaptorCore/*.cpp";                  DestDir: "{app}/RaptorCore"
Source: "../RaptorCore/*.h";                    DestDir: "{app}/RaptorCore"
Source: "../RaptorCore/*.rc";                   DestDir: "{app}/RaptorCore"

Source: "../RaptorData/RaptorDataPackager/*.cpp";           DestDir: "{app}/RaptorData/RaptorDataPackager"
Source: "../RaptorData/RaptorDataPackager/*.h";             DestDir: "{app}/RaptorData/RaptorDataPackager"
Source: "../RaptorData/RaptorDataPackager/*.rc";            DestDir: "{app}/RaptorData/RaptorDataPackager"
Source: "../RaptorData/Data/*.*";                           DestDir: "{app}/RaptorData/Data"
Source: "../RaptorData/buildpak.*";                         DestDir: "{app}/RaptorData"
Source: "../RaptorData/*.cpp";                              DestDir: "{app}/RaptorData"
Source: "../RaptorData/*.h";                                DestDir: "{app}/RaptorData"
Source: "../RaptorData/*.rc";                               DestDir: "{app}/RaptorData"

Source: "../RaptorMFCExtension/res/*.*";            DestDir: "{app}/RaptorMFCExtension/res"
Source: "../RaptorMFCExtension/*.cpp";              DestDir: "{app}/RaptorMFCExtension"
Source: "../RaptorMFCExtension/*.h";                DestDir: "{app}/RaptorMFCExtension"
Source: "../RaptorMFCExtension/*.rc";               DestDir: "{app}/RaptorMFCExtension"
Source: "../RaptorMFCExtension/Dialog/*.*";         DestDir: "{app}/RaptorMFCExtension/Dialog"
Source: "../RaptorMFCExtension/CWnd/*.*";           DestDir: "{app}/RaptorMFCExtension/CWnd"

Source: "../RaptorNetwork/*.cpp";                    DestDir: "{app}/RaptorNetwork"
Source: "../RaptorNetwork/*.h";                      DestDir: "{app}/RaptorNetwork"
Source: "../RaptorNetwork/*.rc";                     DestDir: "{app}/RaptorNetwork"

Source: "../RaptorServer/*.cpp";                    DestDir: "{app}/RaptorServer"
Source: "../RaptorServer/*.h";                      DestDir: "{app}/RaptorServer"
Source: "../RaptorServer/*.rc";                     DestDir: "{app}/RaptorServer"

Source: "../RaptorToolBox/*.cpp";                   DestDir: "{app}/RaptorToolBox"
Source: "../RaptorToolBox/*.h";                     DestDir: "{app}/RaptorToolBox"
Source: "../RaptorToolBox/*.rc";                    DestDir: "{app}/RaptorToolBox"
Source: "../RaptorToolBox/Controllers/*.*";         DestDir: "{app}/RaptorToolBox/Controllers"
Source: "../RaptorToolBox/Filters/*.*";             DestDir: "{app}/RaptorToolBox/Filters"
Source: "../RaptorToolBox/Imaging/*.*";             DestDir: "{app}/RaptorToolBox/Imaging"
Source: "../RaptorToolBox/Loaders/*.*";             DestDir: "{app}/RaptorToolBox/Loaders"
Source: "../RaptorToolBox/Streaming/*.*";           DestDir: "{app}/RaptorToolBox/Streaming"

Source: "../RaptorViewer/*.cpp";                    DestDir: "{app}/RaptorViewer"
Source: "../RaptorViewer/*.h";                      DestDir: "{app}/RaptorViewer"
Source: "../RaptorViewer/*.rc";                     DestDir: "{app}/RaptorViewer"
Source: "../RaptorViewer/Demo.xml";                 DestDir: "{app}/RaptorViewer"

Source: "../Rays2/*.*";                             DestDir: "{app}/Rays2"
Source: "../Rays2/MiniClient/*.*";                  DestDir: "{app}/Rays2/MiniClient"
Source: "../Rays2/MiniRaysClient/*.*";              DestDir: "{app}/Rays2/MiniRaysClient"
Source: "../Rays2/MiniRaysClient/res/*.*";          DestDir: "{app}/Rays2/MiniRaysClient/res"
Source: "../Rays2/ModuleDOF/*.*";                   DestDir: "{app}/Rays2/ModuleDOF"
Source: "../Rays2/ModuleDOF/res/*.*";               DestDir: "{app}/Rays2/ModuleDOF/res"
Source: "../Rays2/ModuleNoise/*.*";                 DestDir: "{app}/Rays2/ModuleNoise"
Source: "../Rays2/ModuleNoise/res/*.*";             DestDir: "{app}/Rays2/ModuleNoise/res"
Source: "../Rays2/ModuleTemplate/*.*";              DestDir: "{app}/Rays2/ModuleTemplate"
Source: "../Rays2/ModuleTemplate/res/*.*";          DestDir: "{app}/Rays2/ModuleTemplate/res"
Source: "../Rays2/ModuleTester/*.*";                DestDir: "{app}/Rays2/ModuleTester"
Source: "../Rays2/RaysClient/*.*";                  DestDir: "{app}/Rays2/RaysClient"
Source: "../Rays2/RaysDeamon/*.*";                  DestDir: "{app}/Rays2/RaysDeamon"
Source: "../Rays2/RaysServer/*.*";                  DestDir: "{app}/Rays2/RaysServer"
Source: "../Rays2/RaysWorkUnit/*.*";                DestDir: "{app}/Rays2/WorkUnit"
Source: "../Rays2/RaysWorkUnit/res/*.*";            DestDir: "{app}/Rays2/WorkUnit/res"
                      
Source: "../ShaderCompiler/*.cpp";                  DestDir: "{app}/ShaderCompiler"
Source: "../ShaderCompiler/*.h";                    DestDir: "{app}/ShaderCompiler"
Source: "../ShaderCompiler/Raptor_splash.*";        DestDir: "{app}/ShaderCompiler"
Source: "../ShaderCompiler/Dune.tif";               DestDir: "{app}/ShaderCompiler"

Source: "../SIMD/*.*";                              DestDir: "{app}/SIMD"

Source: "../SOAR2/*.*";                             DestDir: "{app}/SOAR2"

Source: "../Test/*.cpp";                            DestDir: "{app}/Test"
Source: "../Test/*.h";                              DestDir: "{app}/Test"
Source: "../Test/Sponge.3DS";                       DestDir: "{app}/Test"
Source: "../Test/*.JPG";                            DestDir: "{app}/Test"

Source: "../Test2/res/*.*";                         DestDir: "{app}/Test2/res"
Source: "../Test2/Datas/*.*";                       DestDir: "{app}/Test2/Datas"
Source: "../Test2/*.cpp";                           DestDir: "{app}/Test2"
Source: "../Test2/*.h";                             DestDir: "{app}/Test2"
Source: "../Test2/*.rc";                            DestDir: "{app}/Test2"

Source: "../Test3/*.cpp";                           DestDir: "{app}/Test3"
Source: "../Test3/*.h";                             DestDir: "{app}/Test3"

Source: "../Test4/Datas/*.*";                       DestDir: "{app}/Test4/Datas"
Source: "../Test4/*.cpp";                           DestDir: "{app}/Test4"
Source: "../Test4/*.h";                             DestDir: "{app}/Test4"

Source: "../Test5/*.cpp";                           DestDir: "{app}/Test5"
Source: "../Test5/*.h";                             DestDir: "{app}/Test5"
Source: "../Test5/*.rc";                            DestDir: "{app}/Test5"
Source: "../Test5/*.ico";                           DestDir: "{app}/Test5"
Source: "../Test5/*.tga";                           DestDir: "{app}/Test5"
Source: "../Test5/*.vert";                          DestDir: "{app}/Test5"
Source: "../Test5/*.frag";                          DestDir: "{app}/Test5"

Source: "../Test6/*.*";                             DestDir: "{app}/Test6"

[Icons]
Name: "{group}\Raptor SDK"; Filename: "{app}\RaptorSDK.exe"
