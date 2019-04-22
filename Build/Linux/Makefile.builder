export

OUTPUT = $(RAPTOR_ROOT)/Build/Linux/Release/Builder
SRCDIR = $(RAPTOR_ROOT)/Builder.Net
INCDIRS = -I$(SRCDIR) -I$(VULKAN_INCLUDE_PATH) -I$(REDIST)/include -I/usr/include
INCDIRS = -I$(SRCDIR) -I$(REDIST)/Include -I/usr/include
CCCFLAGS = -DSIMD_NO_ASSEMBLY -DLINUX

all:	redist $(OUTPUT)/Builder

clean:
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)
	
redist:
	mkdir -p $(OUTPUT)
	cp $(SRCDIR)/Redist.sh $(OUTPUT)/Redist.sh 
	chmod +x $(OUTPUT)/Redist.sh; $(OUTPUT)/Redist.sh
	

SRC = \
	$(SRCDIR)/GLBuilder.cpp \
	$(SRCDIR)/main.cpp

OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OUTPUT)/%.o)

$(OUTPUT)/Builder:	$(OBJ)
	g++ $(OBJ) -o $(OUTPUT)/Builder $(LIBGL)
	cp $(OUTPUT)/Builder $(REDIST)/Bin/

$(OBJ):	$(SRC)
	g++ $(CCCFLAGS) $(INCDIRS) -c $(<D)/$(*F).cpp -o $(OUTPUT)/$(@F)

