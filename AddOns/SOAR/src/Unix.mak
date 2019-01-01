include Unix.cfg

UNIX_MAKE = $(MAKE) -f Unix.mak

all: libio png2geo soar

clean:
	@$(CD) libio; $(UNIX_MAKE) clean; $(CD) ..
	@$(CD) png2geo; $(UNIX_MAKE) clean; $(CD) ..
	@$(CD) soar; $(UNIX_MAKE) clean; $(CD) ..

cleanall:
	@$(CD) libio; $(UNIX_MAKE) cleanall; $(CD) ..
	@$(CD) png2geo; $(UNIX_MAKE) cleanall; $(CD) ..
	@$(CD) soar; $(UNIX_MAKE) cleanall; $(CD) ..

libio: libio_
libio_:
	@$(ECHO) "Making in libio..."
	@$(CD) libio; $(UNIX_MAKE); $(CD) ..
	@$(ECHO) ""

png2geo: png2geo_
png2geo_:
	@$(ECHO) "Making in png2geo..."
	@$(CD) png2geo; $(UNIX_MAKE); $(CD) ..
	@$(ECHO) ""

soar: soar_
soar_:
	@$(ECHO) "Making in soar..."
	@$(CD) soar; $(UNIX_MAKE); $(CD) ..
	@$(ECHO) ""
