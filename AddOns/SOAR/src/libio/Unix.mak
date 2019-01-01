TARGET =	$(LIB_DIR)/libio.a
O_FILES =	geoio.o pngio.o

include ../Options.cfg
include ../Unix.cfg

INC_DIR =	../../inc
LIB_DIR =	../../lib

INCS =		-I$(INC_DIR) -I$(PNG_INC_DIR)

CFLAGS =	$(INCS) $(DEFS)

all: $(TARGET)

clean:
	$(RM) $(O_FILES)

cleanall: clean
	$(RM) $(TARGET)

$(O_FILES): ../Options.cfg ../Unix.cfg

$(TARGET): $(O_FILES)
	@$(RM) $@
	$(AR) $@ $(O_FILES)
	$(RANLIB) $@
