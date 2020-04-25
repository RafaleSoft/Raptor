ECHO OFF
REM		/***************************************************************************/
REM		/*                                                                         */
REM		/*  CheckVersion.bat                                                       */
REM		/*                                                                         */
REM		/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
REM		/*                                                                         */
REM		/*  Copyright 1998-2019 by                                                 */
REM		/*  Fabrice FERRAND.                                                       */
REM		/*                                                                         */
REM		/*  This file is part of the Raptor project, and may only be used,         */
REM		/*  modified, and distributed under the terms of the Raptor project        */
REM		/*  license, LICENSE.  By continuing to use, modify, or distribute         */
REM		/*  this file you indicate that you have read the license and              */
REM		/*  understand and accept it fully.                                        */
REM		/*                                                                         */
REM		/***************************************************************************/

SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

ECHO    Checking file versions...
ECHO    Root of Raptor is set to: [%RAPTOR_ROOT%]
ECHO    Platform is defined as: [%PLATFORM%]
ECHO    Raptor Version is defined as: [%RAPTOR_VERSION%]

IF NOT DEFINED RAPTOR_ROOT (
	ECHO    Root is not defined, job aborted !
	GOTO END
)

PUSHD
CD %RAPTOR_ROOT%


IF NOT DEFINED RAPTOR_VERSION (
	ECHO    Raptor Version is not defined, job aborted !
	GOTO END
)

SET VERSIONS=GLBench\GLBench.rc UnitTest\UnitTest.rc Rays2\RaysServer\RaysServer.rc RaptorCore\RaptorCore.rc Demo\Demo.rc Test2\Test2.rc Test5\Test5.rc SIMD\simd.rc RaptorViewer\RaptorViewer.rc RaptorToolBox\RaptorToolBox.rc RaptorServer\RaptorServer.rc RaptorNetwork\RaptorNetwork.rc RaptorMFCExtension\RaptorMFCExtension.rc RaptorData\RaptorDataPackager\RaptorDataPackager.rc RaptorData\RaptorData.rc RaptorCompute\RaptorCompute.rc MicroLex\MicroLex.rc GLObjectViewer\GLObjectViewer.rc Builder\BuilderNative.rc 
echo Versions: %VERSIONS%


	
FOR %%i IN (%VERSIONS%) DO (
	ECHO Check file: [%%i]
	
	REM	Search for FILE VERSION text in file
	SET found="FALSE"
	FOR /F "usebackq tokens=* delims==" %%f IN (`FINDSTR /C:"FILEVERSION" %%i`) DO (
		SET found="TRUE"
	)
	
	IF NOT !found! == "TRUE" (
		ECHO Skip file %%i
	) ELSE (
		REM	FILE VERSION found, rewrite file
		FOR /F "usebackq tokens=* delims==" %%j IN (%%i) DO (
			SET str=%%j
			IF "!str:~0,12!" == " FILEVERSION" (
				ECHO  FILEVERSION %RAPTOR_VERSION:.=,%,0 >> %%i.tmp
			) ELSE (
				IF "!str:~0,15!" == " PRODUCTVERSION" (
					ECHO  PRODUCTVERSION %RAPTOR_VERSION:.=,%,0 >> %%i.tmp
				) ELSE (
					IF "!str:~19,11!" == "FileVersion" (
						ECHO             VALUE "FileVersion", "%RAPTOR_VERSION%.0" >> %%i.tmp
					) ELSE (
						IF "!str:~19,14!" == "ProductVersion" (
							ECHO             VALUE "ProductVersion", "%RAPTOR_VERSION%.0" >> %%i.tmp
						) ELSE (
							ECHO !str! >> %%i.tmp
						)
					)
				)
			)
		)
		
		COPY %%i.tmp %%i
		DEL %%i.tmp
	)
)

	REM powershell -Command "(gc %%i)" -replace 'FILEVERSION .*', 'FILEVERSION %RAPTOR_VERSION%.0' > N:\TEMP_Raptor\test.txt
	REM powershell -Command "(gc N:\TEMP_Raptor\test.txt)" -replace 'PRODUCTVERSION .*', 'PRODUCTVERSION %RAPTOR_VERSION%.0' > N:\TEMP_Raptor\test2.txt

POPD

IF %ERRORLEVEL% NEQ 0 (
  ECHO do something here to address the error
)

:END

