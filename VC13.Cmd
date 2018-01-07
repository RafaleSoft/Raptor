set RAPTOR_VERSION="2.17.0"
set RAPTOR_ROOT=E:\Share\Raptor
set PATH=%PATH%;%RAPTOR_ROOT%\Redist\bin


set FREETYPE_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\freetype-2.5.5\include
set FREETYPE_LIB_PATH=%RAPTOR_ROOT%\AddOns\freetype-2.5.5\lib
set FREETYPE_BIN_PATH=%RAPTOR_ROOT%\AddOns\freetype-2.5.5\bin

set ZLIB_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\zlib128\include
set ZLIB_LIB_PATH=%RAPTOR_ROOT%\AddOns\zlib128\lib\VC10
set ZLIB_BIN_PATH=%RAPTOR_ROOT%\AddOns\zlib128\bin\VC10

set XERCES_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\xercesc_3_1
set XERCES_LIB_PATH=%RAPTOR_ROOT%\AddOns\xercesc_3_1\VC10
set XERCES_BIN_PATH=%RAPTOR_ROOT%\AddOns\xercesc_3_1\VC10

set JPEGLIB_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\JpegLib\include
set JPEGLIB_LIB_PATH=%RAPTOR_ROOT%\AddOns\JpegLib\lib
set JPEGLIB_BIN_PATH=%RAPTOR_ROOT%\AddOns\JpegLib\lib

set TIFFLIB_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\TiffLib\include
set TIFFLIB_LIB_PATH=%RAPTOR_ROOT%\AddOns\TiffLib\lib\VC10
set TIFFLIB_BIN_PATH=%RAPTOR_ROOT%\AddOns\TiffLib\lib\VC10

set PNGLIB_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\libpng-1.6.16\include
set PNGLIB_LIB_PATH=%RAPTOR_ROOT%\AddOns\libpng-1.6.16\lib\VC10
set PNGLIB_BIN_PATH=%RAPTOR_ROOT%\AddOns\libpng-1.6.16\bin\VC10

set OPENEXRLIB_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\OpenEXRLib\include
set OPENEXRLIB_LIB_PATH=%RAPTOR_ROOT%\AddOns\OpenEXRLib\lib\vc10
set OPENEXRLIB_BIN_PATH=%RAPTOR_ROOT%\AddOns\OpenEXRLib\bin\vc10

set OPENCL_INCLUDE_PATH=E:\Intel\OpenCL\sdk\include
set OPENCL_LIB_PATH=E:\Intel\OpenCL\sdk\lib\x86
set OPENCL_BIN_PATH=E:\Intel\OpenCL\sdk\bin\x86

set VULKAN_INCLUDE_PATH=%RAPTOR_ROOT%\AddOns\vulkan\include
set VULKAN_BIN_PATH=c:\windows\system32

cd %RAPTOR_ROOT%

"E:\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe"

exit


