REM		/***************************************************************************/
REM		/*                                                                         */
REM		/*  buildpack.bat                                                          */
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


ECHO Making GLBench data package  ...

cd "%RAPTOR_ROOT%\GLBench\Datas"

IF NOT EXIST "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" (
	ECHO DataPackager not found !
	GOTO END
) ELSE (
	"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"

	ECHO Using DataPackager from:
	ECHO "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"
)

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C GLBench.pck M1_1.jpg M1_1024.jpg M1_128.jpg M1_1280.jpg M1_16.jpg M1_2.jpg M1_256.jpg M1_32.jpg M1_4.jpg M1_512.jpg M1_64.jpg M1_8.jpg M74_1024.jpg M74_256.jpg SmallTeapot.3DS

ECHO Removing temporary files
DEL /F "*.zip"

ECHO Delivering package ...
cd ..

IF EXIST GLBench.pck (
	ECHO GLBench package already exist, it will be replaced !
	DEL /F GLBench.pck
)

move Datas\GLBench.pck .

:END

