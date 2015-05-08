OUTPUT = Release
INCDIRS = -I. 
LIBGL = -L/usr/X11R6/lib -lGL -lGLU

all:	redist $(OUTPUT)/Builder

clean:
	rm -f $(OUTPUT)/*.o $(OUTPUT)/Builder
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)
	
redist:
	chmod +x ./Redist.sh; ./Redist.sh
	

SRC = \
	GLBuilder.cpp \
    main.cpp

OBJ = \
	$(OUTPUT)/GLBuilder.o \
    $(OUTPUT)/main.o

$(OUTPUT)/Builder:	$(OBJ)
	g++ $(OBJ) -o $(OUTPUT)/Builder $(LIBGL)
	cp $(OUTPUT)/Builder $(RAPTOR_ROOT)/Redist/Bin

$(OBJ):	$(SRC)
	g++ $(CCCFLAGS) $(INCDIRS) -c $(<D)/$(*F).cpp  -o $(OUTPUT)/$(@F)

