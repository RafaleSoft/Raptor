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


ECHO Making Demo data package  ...

cd "%RAPTOR_ROOT%\Demo\Datas"

IF NOT EXIST "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" (
	ECHO DataPackager not found !
	GOTO END
) ELSE (
	"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"

	ECHO Using DataPackager from:
	ECHO "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"
)

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C Demo.pck BRICKS.jpg BRICKS2.jpg BRICKS3.jpg BRICKS4.jpg BRICKS5.jpg BRICKS6.jpg BRICKS7.jpg BRICKS8.jpg BRICKS9.jpg BRICKS10.jpg colonneLow.3DS Columns.3DS ColumnsLow.3DS Marble.jpg Marble2.jpg Marble3.jpg Marble4.jpg Marble5.jpg Marble6.jpg Sculpt.tga OLDWOOD2.JPG LensFlare2.tga lrock049.jpg Fire.tga flare0.jpg flare1.jpg flare3.jpg flare4.jpg flare5.jpg flare6.jpg flare7.jpg flare8.jpg flare10.jpg

ECHO Removing temporary files
DEL /F "*.zip"

ECHO Delivering package ...
cd ..

IF EXIST Demo.pck (
	ECHO Demo package already exist, it will be replaced !
	DEL /F Demo.pck
)

move Datas\Demo.pck .

:END

