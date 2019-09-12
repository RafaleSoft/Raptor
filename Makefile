###########################################################################
#                                                                         #
#  Makefile                                                               #
#                                                                         #
#    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       #
#                                                                         #
#  Copyright 1998-2019 by                                                 #
#  Fabrice FERRAND.                                                       #
#                                                                         #
#  This file is part of the Raptor project, and may only be used,         #
#  modified, and distributed under the terms of the Raptor project        #
#  license, LICENSE.  By continuing to use, modify, or distribute         #
#  this file you indicate that you have read the license and              #
#  understand and accept it fully.                                        #
#                                                                         #
###########################################################################

export

#
# Projects ependencies sources
#
RAPTOR_VERSION = 2.17.0
RAPTOR_ROOT = /media/sf_OPENGL/Raptor
REDIST = $(RAPTOR_ROOT)/Redist

LIBGL = -L/usr/lib64 -lGL
LIBGLU = -L/usr/lib64 -lGLU
LIBX11 = -L/usr/lib64 -lX11

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

VULKAN_INCLUDE_PATH=$(RAPTOR_ROOT)/AddOns/vulkan/include
VULKAN_BIN_PATH=c:/windows/system32

#
# Projects target building dependencies and order
#
all: \
	configure \
	simd \
	microlex \
	buildernative \
	builder \
	addons \
	raptordatapackager \
	raptordata \
	openexr \
	raptorcore \
	raptortoolbox

raptortoolbox: builder raptorcore raptordata $(REDIST)/Lib/libRaptorToolBox.a $(REDIST)/Bin/libRaptorToolBox.so.$(RAPTOR_VERSION)

addons: jpeglib tifflib

jpeglib: $(REDIST)/Lib/libjpeg.a $(REDIST)/Bin/libjpeg.so.$(RAPTOR_VERSION)

tifflib: $(REDIST)/Lib/libtiff.a $(REDIST)/Bin/libtiff.so.$(RAPTOR_VERSION)

raptorcore: builder simd microlex raptordata $(REDIST)/Lib/libRaptorCore.a $(REDIST)/Bin/libRaptorCore.so.$(RAPTOR_VERSION)

raptordata:	builder raptordatapackager $(REDIST)/Bin/RaptorData.pck $(REDIST)/Lib/libRaptorData.a $(REDIST)/Bin/libRaptorData.so.$(RAPTOR_VERSION)

openexr: half IlmImf

IlmImf: generators Iex Imath IlmThread IexMath $(REDIST)/Lib/libIlmImf.a $(REDIST)/Bin/libIlmImf.so.$(RAPTOR_VERSION)

IexMath: $(REDIST)/Lib/libIexMath.a $(REDIST)/Bin/libIexMath.so.$(RAPTOR_VERSION)

Imath: $(REDIST)/Lib/libImath.a $(REDIST)/Bin/libImath.so.$(RAPTOR_VERSION)

IlmThread: $(REDIST)/Lib/libIlmThread.a $(REDIST)/Bin/libIlmThread.so.$(RAPTOR_VERSION)

Iex: $(REDIST)/Lib/libIex.a $(REDIST)/Bin/libIex.so.$(RAPTOR_VERSION)

half: $(REDIST)/Bin/eLut $(REDIST)/Bin/toFloat $(REDIST)/Lib/libHalf.a $(REDIST)/Bin/libHalf.so.$(RAPTOR_VERSION)

generators:	$(REDIST)/Bin/eLut $(REDIST)/Bin/toFloat $(REDIST)/Bin/b44ExpLogTable $(REDIST)/Bin/dwaLookups

raptordatapackager:	builder $(REDIST)/Bin/RaptorDataPackager

builder:	configure simd microlex buildernative $(REDIST)/Bin/Builder

buildernative:	$(REDIST)/Lib/libBuilderNative.a $(REDIST)/Bin/libBuilderNative.so.$(RAPTOR_VERSION)

microlex:	$(REDIST)/Bin/Microlex

simd:	$(REDIST)/Lib/libsimd.a $(REDIST)/Bin/libsimd.so.$(RAPTOR_VERSION)

configure:	Builder/Configure/Redist.sh
	@echo "Configuring Raptor projects ..."
	cd Builder/Configure; ls ; chmod +x ./Redist.sh ; ./Redist.sh
	@echo "Configuration done."



#
# Projects building rules
#
$(REDIST)/Lib/libRaptorToolBox.a $(REDIST)/Bin/libRaptorToolBox.so.$(RAPTOR_VERSION):
	@echo "Building RaptorToolBox project ..."
	make -C Build/Linux -f Makefile.raptortoolbox all
	@echo "RaptorToolBox project done."

$(REDIST)/Lib/libtiff.a $(REDIST)/Bin/libtiff.so.$(RAPTOR_VERSION):
	@echo "Building libtiff project ..."
	make -C Build/Linux -f Makefile.tifflib all
	@echo "libjpeg libtiff done."

$(REDIST)/Lib/libjpeg.a $(REDIST)/Bin/libjpeg.so.$(RAPTOR_VERSION):
	@echo "Building libjpeg project ..."
	make -C Build/Linux -f Makefile.jpeglib all
	@echo "libjpeg project done."
		
$(REDIST)/Lib/libRaptorCore.a $(REDIST)/Bin/libRaptorCore.so.$(RAPTOR_VERSION):
	@echo "Building RaptorCore project ..."
	make -C Build/Linux -f Makefile.raptorcore all
	@echo "RaptorCore project done."

$(REDIST)/Lib/libRaptorData.a $(REDIST)/Bin/libRaptorData.so.$(RAPTOR_VERSION) $(REDIST)/Bin/RaptorData.pck:
	@echo "Building RaptorData project ..."
	make -C Build/Linux -f Makefile.raptordata all
	@echo "RaptorData project done."

$(REDIST)/Bin/RaptorDataPackager:
	@echo "Building RaptorDataPackager project ..."
	make -C Build/Linux -f Makefile.raptordatapackager all
	@echo "RaptorDataPackager project done."

$(REDIST)/Bin/Builder:	$(REDIST)/Bin/libBuilderNative.so.$(RAPTOR_VERSION)
	@echo "Building Builder project ..."
	make -C Build/Linux -f Makefile.builder all
	@echo "Builder project done."

$(REDIST)/Lib/libBuilderNative.a $(REDIST)/Bin/libBuilderNative.so.$(RAPTOR_VERSION):
	@echo "Building BuilderNative project ..."
	make -C Build/Linux -f Makefile.BuilderNative all
	@echo "BuilderNative project done."

$(REDIST)/Bin/Microlex:
	@echo "Building Microlex project ..."
	make -C Build/Linux -f Makefile.microlex all
	@echo "Microlex project done."

$(REDIST)/Lib/libsimd.a $(REDIST)/Bin/libsimd.so.$(RAPTOR_VERSION):
	@echo "Building simd project ..."
	make -C Build/Linux -f Makefile.simd all
	@echo "simd project done."

$(REDIST)/Lib/libIlmImf.a $(REDIST)/Bin/libIlmImf.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR IlmImf project ..."
	make -C Build/Linux -f Makefile.IlmImf all
	@echo "IlmImf project done."
	
$(REDIST)/Bin/eLut:
	@echo "Building eLut generator ..."
	make -C Build/Linux -f Makefile.eLut all
	@echo "eLut project done."

$(REDIST)/Bin/toFloat:
	@echo "Building toFloat generator ..."
	make -C Build/Linux -f Makefile.toFloat all
	@echo "toFloat project done."
	
$(REDIST)/Lib/libHalf.a $(REDIST)/Bin/libHalf.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR Half project ..."
	make -C Build/Linux -f Makefile.Half all
	@echo "Half project done."

$(REDIST)/Lib/libIex.a $(REDIST)/Bin/libIex.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR Iex project ..."
	make -C Build/Linux -f Makefile.Iex all
	@echo "Iex project done."

$(REDIST)/Lib/libIlmThread.a $(REDIST)/Bin/libIlmThread.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR IlmThread project ..."
	make -C Build/Linux -f Makefile.IlmThread all
	@echo "IlmThread project done."

$(REDIST)/Lib/libImath.a $(REDIST)/Bin/libImath.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR IMath project ..."
	make -C Build/Linux -f Makefile.IMath all
	@echo "IMath project done."

$(REDIST)/Lib/libIexMath.a $(REDIST)/Bin/libIexMath.so.$(RAPTOR_VERSION):
	@echo "Building OpenEXR IexMath project ..."
	make -C Build/Linux -f Makefile.IexMath all
	@echo "IexMath project done."	

$(REDIST)/Bin/b44ExpLogTable:	$(REDIST)/Bin/libHalf.so.$(RAPTOR_VERSION)
	@echo "Building b44ExpLogTable generator ..."
	make -C Build/Linux -f Makefile.b44ExpLogTable all
	@echo "b44ExpLogTable project done."

$(REDIST)/Bin/dwaLookups:	$(REDIST)/Bin/libHalf.so.$(RAPTOR_VERSION) $(REDIST)/Bin/libIlmThread.so.$(RAPTOR_VERSION) $(REDIST)/Bin/libIex.so.$(RAPTOR_VERSION)
	@echo "Building dwaLookups generator ..."
	make -C Build/Linux -f Makefile.dwaLookups all
	@echo "dwaLookups project done."
		
	
#
# Common targets
#
clean:
	@echo "Cleaning intermediate build files..."
	make -C Build/Linux -f Makefile.raptortoolbox clean
	make -C Build/Linux -f Makefile.tifflib clean
	make -C Build/Linux -f Makefile.jpeglib clean
	make -C Build/Linux -f Makefile.raptorcore clean
	make -C Build/Linux -f Makefile.raptordata clean
	make -C Build/Linux -f Makefile.raptordatapackager clean
	make -C Build/Linux -f Makefile.builder clean
	make -C Build/Linux -f Makefile.microlex clean
	make -C Build/Linux -f Makefile.simd clean
	make -C Build/Linux -f Makefile.eLut clean
	make -C Build/Linux -f Makefile.toFloat clean
	make -C Build/Linux -f Makefile.Half clean
	make -C Build/Linux -f Makefile.Iex clean
	make -C Build/Linux -f Makefile.IlmThread clean
	make -C Build/Linux -f Makefile.IMath clean
	make -C Build/Linux -f Makefile.IexMath clean
	make -C Build/Linux -f Makefile.b44ExpLogTable clean
	make -C Build/Linux -f Makefile.dwaLookups clean
	make -C Build/Linux -f Makefile.IlmImf clean
	make -C Build/Linux -f Makefile.BuilderNative clean
	@echo "Done."

distclean:
	@echo "Cleaning redistribuable files in $(REDIST)..."
	rm -f $(REDIST)/Bin/*
	rm -f $(REDIST)/Lib/*
	@echo "Done."

