TARGET =	$(LIB_DIR)\libio.lib
O_FILES =	geoio.obj pngio.obj
IDB_FILE =	vc.idb

!include ..\Options.cfg
!include ..\Windows.cfg

INC_DIR =	..\..\inc
LIB_DIR =	..\..\lib

INCS =		/I $(INC_DIR)

CFLAGS =	$(CFLAGS) $(INCS) $(DEFS) /D "_LIB" /Ze
LDFLAGS =	-lib $(LDFLAGS)

all: $(TARGET)

clean:
	$(RM) $(O_FILES) $(IDB_FILE)

cleanall: clean
	$(RM) $(TARGET)

$(O_FILES): ..\Options.cfg ..\Windows.cfg

$(LIB_DIR):
	@if not exist $(LIB_DIR)\$(NULL) $(MKDIR) $(LIB_DIR)

$(TARGET): $(O_FILES) $(LIB_DIR)
	$(LD) $(LDFLAGS) $(O_FILES) /out:"$@"
