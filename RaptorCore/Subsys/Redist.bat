ECHO Making redistribuable package documentation ...

FOR /F "usebackq delims==" %%i IN (`find /C "BUILD_REDIST" Subsys\CodeGeneration.h`) DO (
IF "---------- SUBSYS\CODEGENERATION.H: 1" == "%%i" (
ECHO Build of documentation by code generation specification ...
) ELSE (
ECHO Keep previous package
GOTO END
)
)

cd ..

IF EXIST Redist (
	ECHO Generating HTML documentation ...

	cd RaptorCore\Subsys
	doxygen RaptorDoxyfile
	cd ..
	cd ..	
)  ELSE (
	ECHO Redist directory not created, skipping ...
)

:END

