!include Windows.cfg

WIN_MAKE = $(MAKE) /f Windows.mak

all: libio png2geo soar

clean:
	@$(CD) libio
	@$(WIN_MAKE) clean
	@$(CD) ..
	@$(CD) png2geo
	@$(WIN_MAKE) clean
	@$(CD) ..
	@$(CD) soar
	@$(WIN_MAKE) clean
	@$(CD) ..

cleanall:
	@$(CD) libio
	@$(WIN_MAKE) cleanall
	@$(CD) ..
	@$(CD) png2geo
	@$(WIN_MAKE) cleanall
	@$(CD) ..
	@$(CD) soar
	@$(WIN_MAKE) cleanall
	@$(CD) ..

libio: libio_
libio_:
	@$(ECHO) Making in libio...
	@$(CD) libio
	@$(WIN_MAKE)
	@$(CD) ..

png2geo: png2geo_
png2geo_:
	@$(ECHO) Making in png2geo...
	@$(CD) png2geo
	@$(WIN_MAKE)
	@$(CD) ..

soar: soar_
soar_:
	@$(ECHO) Making in soar...
	@$(CD) soar
	@$(WIN_MAKE)
	@$(CD) ..
