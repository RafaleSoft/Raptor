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

FREETYPE_VERSION = 2.9.1
FREETYPE_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/freetype-$(FREETYPE_VERSION)/include
FREETYPE_LIB_PATH = $(RAPTOR_ROOT)/AddOns/freetype-$(FREETYPE_VERSION)/lib
FREETYPE_BIN_PATH = $(RAPTOR_ROOT)/AddOns/freetype-$(FREETYPE_VERSION)/bin

ZLIB_VERSION = 1.2.11
ZLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/zlib-$(ZLIB_VERSION)/include
ZLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/zlib-$(ZLIB_VERSION)/lib
ZLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/zlib-$(ZLIB_VERSION)/bin

XERCES_VERSION = 3.2.2
XERCES_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/xercesc-$(XERCES_VERSION)/include
XERCES_LIB_PATH = $(RAPTOR_ROOT)/AddOns/xercesc-$(XERCES_VERSION)/lib
XERCES_BIN_PATH = $(RAPTOR_ROOT)/AddOns/xercesc-$(XERCES_VERSION)/bin

JPEGLIB_VERSION = v9
JPEGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib-$(JPEGLIB_VERSION)/include
JPEGLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib-$(JPEGLIB_VERSION)/lib
JPEGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/JpegLib-$(JPEGLIB_VERSION)/lib

TIFFLIB_VERSION = 4.0.10
TIFFLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib-$(TIFFLIB_VERSION)/include
TIFFLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib-$(TIFFLIB_VERSION)/lib
TIFFLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/TiffLib-$(TIFFLIB_VERSION)/lib

PNGLIB_VERSION = 1.6.36
PNGLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/libpng-$(PNGLIB_VERSION)/include
PNGLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/libpng-$(PNGLIB_VERSION)/lib
PNGLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/libpng-$(PNGLIB_VERSION)/bin

OPENEXRLIB_VERSION = 2.3.0
OPENEXRLIB_INCLUDE_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib-$(OPENEXRLIB_VERSION)/include
OPENEXRLIB_LIB_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib-$(OPENEXRLIB_VERSION)/lib
OPENEXRLIB_BIN_PATH = $(RAPTOR_ROOT)/AddOns/OpenEXRLib-$(OPENEXRLIB_VERSION)/bin

OPENCL_VERSION = 2.2
OPENCL_INCLUDE_PATH=/usr/include
OPENCL_LIB_PATH=/usr/lib64
OPENCL_BIN_PATH=/usr/lib64

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
	raptorcore \
	raptornetwork \
	raptortoolbox \
	raptorcompute \
	raptorserver \
	raptorviewer


raptorviewer: builder raptorcore raptordata raptortoolbox raptornetwork raptorserver $(REDIST)/Bin/RaptorViewer
	
raptorserver: builder raptorcore raptordata raptortoolbox raptornetwork $(REDIST)/Bin/RaptorServer

raptorcompute: builder raptorcore $(REDIST)/Lib/libRaptorCompute.a $(REDIST)/Bin/libRaptorCompute.so.$(RAPTOR_VERSION)

raptortoolbox: builder raptorcore raptordata $(REDIST)/Lib/libRaptorToolBox.a $(REDIST)/Bin/libRaptorToolBox.so.$(RAPTOR_VERSION)

raptornetwork: builder $(REDIST)/Lib/libRaptorNetwork.a $(REDIST)/Bin/libRaptorNetwork.so.$(RAPTOR_VERSION)

addons: zlib jpeglib tifflib pnglib freetype xercesc half IlmImf

jpeglib: $(REDIST)/Lib/libjpeg.a $(REDIST)/Bin/libjpeg.so.$(JPEGLIB_VERSION)

tifflib: $(REDIST)/Lib/libtiff.a $(REDIST)/Bin/libtiff.so.$(TIFFLIB_VERSION)

pnglib: $(REDIST)/Lib/libpng.a $(REDIST)/Bin/libpng.so.$(PNGLIB_VERSION)

zlib: $(REDIST)/Lib/libz.a $(REDIST)/Bin/libz.so.$(ZLIB_VERSION)

freetype: $(REDIST)/Lib/libfreetype.a $(REDIST)/Bin/libfreetype.so.$(FREETYPE_VERSION)

xercesc: $(REDIST)/Lib/libcerces-c.a $(REDIST)/Bin/libxerces-c.so.$(XERCES_VERSION)

raptorcore: builder simd microlex raptordata xercesc $(REDIST)/Lib/libRaptorCore.a $(REDIST)/Bin/libRaptorCore.so.$(RAPTOR_VERSION)

raptordata:	builder raptordatapackager $(REDIST)/Bin/RaptorData.pck $(REDIST)/Lib/libRaptorData.a $(REDIST)/Bin/libRaptorData.so.$(RAPTOR_VERSION)

IlmImf: generators Iex Imath IlmThread IexMath $(REDIST)/Lib/libIlmImf.a $(REDIST)/Bin/libIlmImf.so.$(OPENEXRLIB_VERSION)

IexMath: $(REDIST)/Lib/libIexMath.a $(REDIST)/Bin/libIexMath.so.$(OPENEXRLIB_VERSION)

Imath: $(REDIST)/Lib/libImath.a $(REDIST)/Bin/libImath.so.$(OPENEXRLIB_VERSION)

IlmThread: $(REDIST)/Lib/libIlmThread.a $(REDIST)/Bin/libIlmThread.so.$(OPENEXRLIB_VERSION)

Iex: $(REDIST)/Lib/libIex.a $(REDIST)/Bin/libIex.so.$(OPENEXRLIB_VERSION)

half: $(REDIST)/Bin/eLut $(REDIST)/Bin/toFloat $(REDIST)/Lib/libHalf.a $(REDIST)/Bin/libHalf.so.$(OPENEXRLIB_VERSION)

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
$(REDIST)/Bin/RaptorViewer:
	@echo "Building RaptorViewer project ..."
	make -C Build/Linux -f Makefile.raptorviewer all
	@echo "RaptorViewer project done."

$(REDIST)/Bin/RaptorServer:
	@echo "Building RaptorServer project ..."
	make -C Build/Linux -f Makefile.raptorserver all
	@echo "RaptorServer project done."

$(REDIST)/Lib/libRaptorCompute.a $(REDIST)/Bin/libRaptorCompute.so.$(RAPTOR_VERSION):
	@echo "Building RaptorCompute project ..."
	make -C Build/Linux -f Makefile.raptorcompute all
	@echo "RaptorCompute project done."
	
$(REDIST)/Lib/libRaptorToolBox.a $(REDIST)/Bin/libRaptorToolBox.so.$(RAPTOR_VERSION):
	@echo "Building RaptorToolBox project ..."
	make -C Build/Linux -f Makefile.raptortoolbox all
	@echo "RaptorToolBox project done."

$(REDIST)/Lib/libRaptorNetwork.a $(REDIST)/Bin/libRaptorNetwork.so.$(RAPTOR_VERSION):
	@echo "Building RaptorNetwork project ..."
	make -C Build/Linux -f Makefile.raptornetwork all
	@echo "RaptorNetwork project done."

$(REDIST)/Lib/libcerces-c.a $(REDIST)/Bin/libxerces-c.so.$(XERCES_VERSION):
	@echo "Building xerces-c project ..."
	make -C Build/Linux -f Makefile.xercesc all
	@echo "xerces-c project done."
	
$(REDIST)/Lib/libfreetype.a $(REDIST)/Bin/libfreetype.so.$(FREETYPE_VERSION):
	@echo "Building freetype project ..."
	make -C Build/Linux -f Makefile.freetype all
	@echo "freetype project done."
	
$(REDIST)/Lib/libz.a $(REDIST)/Bin/libz.so.$(ZLIB_VERSION):
	@echo "Building libz project ..."
	make -C Build/Linux -f Makefile.zlib all
	@echo "libz project done."
	
$(REDIST)/Lib/libpng.a $(REDIST)/Bin/libpng.so.$(PNGLIB_VERSION):
	@echo "Building libpng project ..."
	make -C Build/Linux -f Makefile.pnglib all
	@echo "libpng project done."
	
$(REDIST)/Lib/libtiff.a $(REDIST)/Bin/libtiff.so.$(TIFFLIB_VERSION):
	@echo "Building libtiff project ..."
	make -C Build/Linux -f Makefile.tifflib all
	@echo "libtiff project done."

$(REDIST)/Lib/libjpeg.a $(REDIST)/Bin/libjpeg.so.$(JPEGLIB_VERSION):
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

$(REDIST)/Lib/libIlmImf.a $(REDIST)/Bin/libIlmImf.so.$(OPENEXRLIB_VERSION):
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
	
$(REDIST)/Lib/libHalf.a $(REDIST)/Bin/libHalf.so.$(OPENEXRLIB_VERSION):
	@echo "Building OpenEXR Half project ..."
	make -C Build/Linux -f Makefile.Half all
	@echo "Half project done."

$(REDIST)/Lib/libIex.a $(REDIST)/Bin/libIex.so.$(OPENEXRLIB_VERSION):
	@echo "Building OpenEXR Iex project ..."
	make -C Build/Linux -f Makefile.Iex all
	@echo "Iex project done."

$(REDIST)/Lib/libIlmThread.a $(REDIST)/Bin/libIlmThread.so.$(OPENEXRLIB_VERSION):
	@echo "Building OpenEXR IlmThread project ..."
	make -C Build/Linux -f Makefile.IlmThread all
	@echo "IlmThread project done."

$(REDIST)/Lib/libImath.a $(REDIST)/Bin/libImath.so.$(OPENEXRLIB_VERSION):
	@echo "Building OpenEXR IMath project ..."
	make -C Build/Linux -f Makefile.IMath all
	@echo "IMath project done."

$(REDIST)/Lib/libIexMath.a $(REDIST)/Bin/libIexMath.so.$(OPENEXRLIB_VERSION):
	@echo "Building OpenEXR IexMath project ..."
	make -C Build/Linux -f Makefile.IexMath all
	@echo "IexMath project done."	

$(REDIST)/Bin/b44ExpLogTable:	$(REDIST)/Bin/libHalf.so.$(OPENEXRLIB_VERSION)
	@echo "Building b44ExpLogTable generator ..."
	make -C Build/Linux -f Makefile.b44ExpLogTable all
	@echo "b44ExpLogTable project done."

$(REDIST)/Bin/dwaLookups:	$(REDIST)/Bin/libHalf.so.$(OPENEXRLIB_VERSION) $(REDIST)/Bin/libIlmThread.so.$(OPENEXRLIB_VERSION) $(REDIST)/Bin/libIex.so.$(OPENEXRLIB_VERSION)
	@echo "Building dwaLookups generator ..."
	make -C Build/Linux -f Makefile.dwaLookups all
	@echo "dwaLookups project done."
		
	
#
# Common targets
#
clean:
	@echo "Cleaning intermediate build files..."
	make -C Build/Linux -f Makefile.raptorviewer clean
	make -C Build/Linux -f Makefile.raptorserver clean
	make -C Build/Linux -f Makefile.raptorcompute clean
	make -C Build/Linux -f Makefile.raptortoolbox clean
	make -C Build/Linux -f Makefile.raptornetwork clean
	make -C Build/Linux -f Makefile.tifflib clean
	make -C Build/Linux -f Makefile.jpeglib clean
	make -C Build/Linux -f Makefile.pnglib clean
	make -C Build/Linux -f Makefile.zlib clean
	make -C Build/Linux -f Makefile.freetype clean
	make -C Build/Linux -f Makefile.xercesc clean
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

