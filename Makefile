export

RAPTOR_VERSION = 2.17.0
RAPTOR_ROOT = /media/sf_OPENGL/Raptor
REDIST = $(RAPTOR_ROOT)/Redist

LIBGL = -L/usr/X11R6/lib -lGL
LIBGLU = -L/usr/X11R6/lib -lGLU

FREETYPE_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.9.1/include
FREETYPE_LIB_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.9.1/lib
FREETYPE_BIN_PATH = $(RAPTOR_ROOT)/AddOns/freetype-2.9.1/bin

ZLIB_INCLUDE_PATH = /usr/include
ZLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/zlib128/lib/VC10
ZLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/zlib128/bin/VC10

XERCES_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/xercesc-3.2.2
XERCES_LIB_PATH = $(RAPTOR_ROOT)/AddOns/xercesc_3_1/VC10
XERCES_BIN_PATH = $(RAPTOR_ROOT)/AddOns/xercesc_3_1/VC10

JPEGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib-v9/include
JPEGLIB_LIB_PATH = /usr/lib
JPEGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib/lib

TIFFLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib-4.0.10/include
TIFFLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib/lib/VC10
TIFFLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib/lib/VC10

PNGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.36/include
PNGLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.36/lib/VC10
PNGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/libpng-1.6.36/bin/VC10

OPENEXRLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib-2.3.0/include
OPENEXRLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib/lib/vc10
OPENEXRLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib/bin/vc10

OPENCL_INCLUDE_PATH=C:/Intel/OpenCL/sdk/include
OPENCL_LIB_PATH=C:/Intel/OpenCL/sdk/lib/x86
OPENCL_BIN_PATH=C:/Intel/OpenCL/sdk/bin/x86

VULKAN_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/vulkan/include
VULKAN_BIN_PATH=c:/windows/system32

#
# Projects target building dependencies and order
#
all:	simd microlex builder raptordatapackager raptordata raptorcore

raptorcore: builder simd microlex raptordata $(REDIST)/Lib/RaptorCore.lib $(REDIST)/Bin/RaptorCore.so.$(RAPTOR_VERSION)

raptordata:	builder raptordatapackager $(REDIST)/Lib/RaptorData.lib $(REDIST)/Bin/RaptorData.so.$(RAPTOR_VERSION)

raptordatapackager:	builder $(REDIST)/Bin/RaptorDataPackager

builder:	simd microlex $(REDIST)/Bin/Builder

microlex:	$(REDIST)/Bin/Microlex

simd:	$(REDIST)/Lib/simd.lib $(REDIST)/Bin/simd.so.$(RAPTOR_VERSION)


#
# Projects building rules
#
$(REDIST)/Lib/RaptorCore.lib $(REDIST)/Bin/RaptorCore.so.$(RAPTOR_VERSION):
	@echo "Building RaptorCore project ..."
	make -C Build/Linux -f Makefile.raptorcore all
	@echo "RaptorCore project done."

$(REDIST)/Lib/RaptorData.lib $(REDIST)/Bin/RaptorData.so.$(RAPTOR_VERSION):
	@echo "Building RaptorData project ..."
	make -C Build/Linux -f Makefile.raptordata all
	@echo "RaptorData project done."

$(REDIST)/Bin/RaptorDataPackager:
	@echo "Building RaptorDataPackager project ..."
	make -C Build/Linux -f Makefile.raptordatapackager all
	@echo "RaptorDataPackager project done."

$(REDIST)/Bin/Builder:
	@echo "Building Builder project ..."
	make -C Build/Linux -f Makefile.builder all
	@echo "Builder project done."

$(REDIST)/Bin/Microlex:
	@echo "Building Microlex project ..."
	make -C Build/Linux -f Makefile.microlex all
	@echo "Microlex project done."

$(REDIST)/Lib/simd.lib $(REDIST)/Bin/simd.so.$(RAPTOR_VERSION):
	@echo "Building simd project ..."
	make -C Build/Linux -f Makefile.simd all
	@echo "simd project done."


#
# Common targets
#
clean:
	@echo "Cleaning intermediate build files..."
	make -C Build/Linux -f Makefile.raptorcore clean
	make -C Build/Linux -f Makefile.raptordata clean
	make -C Build/Linux -f Makefile.raptordatapackager clean
	make -C Build/Linux -f Makefile.builder clean
	make -C Build/Linux -f Makefile.microlex clean
	make -C Build/Linux -f Makefile.simd clean
	@echo "Done."

distclean:
	@echo "Cleaning redistribuable files in $(REDIST)..."
	rm -f $(REDIST)/Bin/*
	rm -f $(REDIST)/Lib/*
	@echo "Done."

