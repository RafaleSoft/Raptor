###########################################################################
#                                                                         #
#  Makefile.builder                                                       #
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

OUTPUT = $(RAPTOR_ROOT)/Build/Linux/Release/Builder
SRCDIR = $(RAPTOR_ROOT)/Builder
INCDIRS = -I$(SRCDIR) -I$(RAPTOR_ROOT)/Builder/BuilderNative -I$(VULKAN_INCLUDE_PATH) -I$(REDIST)/include -I/usr/include
CCCFLAGS = -DSIMD_NO_ASSEMBLY -DLINUX -DGLX_EXTENSIONS
LDFLAGS = $(LIBGL) $(LIBX11) $(REDIST)/Bin/libBuilderNative.so.$(RAPTOR_VERSION) $(REDIST)/Bin/libsimd.so.$(RAPTOR_VERSION)

all:	$(OUTPUT)/Builder

clean:
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)

SRC = \
	$(SRCDIR)/main.cpp

OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OUTPUT)/%.o)

$(OUTPUT)/Builder:	$(OBJ)
	g++ $(OBJ) -o $(OUTPUT)/Builder $(LDFLAGS)
	cp $(OUTPUT)/Builder $(REDIST)/Bin/

$(OBJ):	$(SRC)
	g++ $(CCCFLAGS) $(INCDIRS) -c $(<D)/$(*F).cpp -o $(OUTPUT)/$(@F)

