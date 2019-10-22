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

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C Toolbox.pck DOF.gs DOF.ps DOF.vs DOF_old.ps DOF.vp DOF.fp Color.ps Color_old.ps Color.fp Magnifier.gs Magnifier_X.ps Magnifier_Y.ps Magnifier.vs Magnifier_oldX.ps Magnifier_oldY.ps Magnifier.vp Magnifier_X.fp Magnifier_Y.fp HDR_threshold.ps HDR_composer.ps HDR_luminance.ps HDR_luminanceLast.ps HDR_threshold_old.ps HDR_composer_old.ps HDR_luminance_old.ps HDR_luminanceLast_old.ps HDR_threshold.fp HDR_composer.fp HDR_luminance.fp HDR_luminanceLast.fp MB_accum.ps MB_accum2.ps MB_accum_old.ps

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

