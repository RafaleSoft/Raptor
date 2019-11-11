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


ECHO Making Raptor data package  ...


cd "%RAPTOR_ROOT%\RaptorData\Data"

IF NOT EXIST "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" (
	ECHO DataPackager not found !
	GOTO END
) ELSE (
	"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"

	ECHO Using DataPackager from:
	ECHO "%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe"
)

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C RaptorData.pck Raptor_logo_sml.txt rapsplsh.AVI lucon.ttf RaptorMessages.xml bump_0light.vp bump_0light.fp bump.vp bump.fp bump_att.vp bump_att.fp bump_att_2light.vp bump_att_2light.fp bump_att_3light.vp bump_att_3light.fp embm_0light.vp embm_0light.fp embm.vp embm.fp blinn.vs blinn.ps phong.vs phong.ps bump.vs bump.ps projection.fp shadowmap.fp shadowmap_pcf.fp shadowmap_pcf_4x.fp shadowmap_pcf_16x.fp blenderX_8x.vp blenderY_8x.vp blender_8x.fp AO.vs AO.ps blender_8x.gs blenderX_8x.ps blenderY_8x.ps tquad.vs tquad.ps tquad.gs empty.vs diffuse.ps particle.vs particle2D.gs particle3D.gs particle3D.ps font2D.vs font2D.gs font2D.ps
ECHO Removing temporary files
DEL /F "*.zip"

ECHO Delivering package ...
cd ..

IF EXIST RaptorData.pck (
	ECHO RaptorData package already exist, it will be replaced !
	DEL /F RaptorData.pck
)

move Data\RaptorData.pck .

:END

