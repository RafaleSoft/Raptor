export

RAPTOR_VERSION = "2.17.0"
RAPTOR_ROOT = /media/sf_OPENGL/Raptor
REDIST = $(RAPTOR_ROOT)/Redist

LIBGL = -L/usr/X11R6/lib -lGL
LIBGLU = -L/usr/X11R6/lib -lGLU

FREETYPE_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.5.5/include
FREETYPE_LIB_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.5.5/lib
FREETYPE_BIN_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.5.5/bin

ZLIB_INCLUDE_PATH = /usr/include
ZLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/zlib128/lib/VC10
ZLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/zlib128/bin/VC10

XERCES_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/xercesc_3_1
XERCES_LIB_PATH = $(RAPTOR_ROOT)/AddOns/xercesc_3_1/VC10
XERCES_BIN_PATH = $(RAPTOR_ROOT)/AddOns/xercesc_3_1/VC10

JPEGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib/include
JPEGLIB_LIB_PATH = /usr/lib
JPEGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib/lib

TIFFLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib/include
TIFFLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib/lib/VC10
TIFFLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib/lib/VC10

PNGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.16/include
PNGLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.16/lib/VC10
PNGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.16/bin/VC10

OPENEXRLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib/include
OPENEXRLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib/lib/vc10
OPENEXRLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib/bin/vc10

OPENCL_INCLUDE_PATH=C:/Intel/OpenCL/sdk/include
OPENCL_LIB_PATH=C:/Intel/OpenCL/sdk/lib/x86
OPENCL_BIN_PATH=C:/Intel/OpenCL/sdk/bin/x86

VULKAN_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/vulkan/include
VULKAN_BIN_PATH=c:/windows/system32

all:	simd microlex builder

	
builder:	simd microlex $(REDIST)/Bin/Builder
	echo "Building builder..."

microlex:	$(REDIST)/Bin/Microlex
	echo "Building microlex..."

simd:	$(REDIST)/Lib/simd.lib
	echo "Building simd..."


$(REDIST)/Bin/Builder:
	make -C Build/Linux -f Makefile.builder all

$(REDIST)/Bin/Microlex:
	make -C Build/Linux -f Makefile.microlex all

$(REDIST)/Lib/simd.lib:
	echo "make simd.lib ..."
	make -C Build/Linux -f Makefile.simd all


clean:
	make -C Build/Linux -f Makefile.builder clean
	make -C Build/Linux -f Makefile.microlex clean
	make -C Build/Linux -f Makefile.simd clean

distclean:
	rm -f $(REDIST)/Bin/*
	rm -f $(REDIST)/Lib/*

