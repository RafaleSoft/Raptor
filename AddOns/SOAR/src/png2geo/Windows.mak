TARGET =	$(BIN_DIR)\png2geo.exe
O_FILES =	main.obj
IDB_FILE =	vc.idb

!include ..\Options.cfg
!include ..\Windows.cfg

BIN_DIR =	..\..\bin
INC_DIR =	..\..\inc
LIB_DIR =	..\..\lib

INCS =		/I $(INC_DIR)
LIBS =		kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libc.lib libpng.lib $(LIB_DIR)\libio.lib

CFLAGS =	$(CFLAGS) $(INCS) $(DEFS) /Za
LDFLAGS =	$(LDFLAGS) /subsystem:console /incremental:no /machine:I386 /nodefaultlib $(LIBS)

all: $(TARGET)

clean:
	$(RM) $(O_FILES) $(IDB_FILE)

cleanall: clean
	$(RM) $(TARGET)

$(O_FILES): ..\Options.cfg ..\Windows.cfg

$(BIN_DIR):
	@if not exist $(BIN_DIR)\$(NULL) $(MKDIR) $(BIN_DIR)

$(TARGET): $(O_FILES) $(LIB_DIR)\libio.lib $(BIN_DIR)
	$(LD) $(LDFLAGS) $(O_FILES) /out:"$@"
