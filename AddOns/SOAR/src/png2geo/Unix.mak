TARGET =	$(BIN_DIR)/png2geo
O_FILES =	main.o

include ../Options.cfg
include ../Unix.cfg

BIN_DIR =	../../bin
INC_DIR =	../../inc
LIB_DIR =	../../lib

INCS =		-I$(INC_DIR)
LIBS =		-L$(LIB_DIR) -L$(PNG_LIB_DIR) -lio -lpng -lz -lm

CFLAGS =	$(INCS) $(DEFS)

all: $(TARGET)

clean:
	$(RM) $(O_FILES)

cleanall: clean
	$(RM) $(TARGET)

$(O_FILES): ../Options.cfg ../Unix.cfg

$(TARGET): $(O_FILES) $(LIB_DIR)/libio.a
	$(CC) $(O_FILES) $(LIBS) -o $@
