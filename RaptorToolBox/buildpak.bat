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


ECHO Making RaptorToolbox data package  ...

cd "%RAPTOR_ROOT%\RaptorToolBox\Filters"

IF NOT EXIST "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" (
	ECHO DataPackager not found !
	GOTO END
) ELSE (
	"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"

	ECHO Using DataPackager from:
	ECHO "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"
)

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C Toolbox.pck 

ECHO Removing temporary files
DEL /F "*.zip"

ECHO Delivering package ...
cd ..

IF EXIST Toolbox.pck (
	ECHO RaptorToolbox package already exist, it will be replaced !
	DEL /F Toolbox.pck
)

move Filters\Toolbox.pck .

:END

