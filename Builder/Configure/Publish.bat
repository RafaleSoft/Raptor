REM		/***************************************************************************/
REM		/*                                                                         */
REM		/*  Publish.bat                                                            */
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


ECHO OFF

ECHO    Preparing publish structure...
ECHO    Root of Raptor is set to: [%RAPTOR_ROOT%]

IF NOT DEFINED RAPTOR_ROOT (
	ECHO    Root is not defined, job aborted !
	GOTO END
)


IF NOT DEFINED PUBLISH (
	ECHO    Publish base path is not defined, job aborted !
	GOTO END
)

ECHO	Check if the publish directory is here
ECHO	%PUBLISH%
CD %PUBLISH%

IF EXIST app^.publish (
	ECHO    Publish folder exist, it will be recreated
	RMDIR app^.publish
) ELSE (
	ECHO    Publish folder missing, it will be created
)

ECHO    Creating publish folders...
MKDIR app^.publish

CD %RAPTOR_ROOT%
ECHO Publish complete > Builder\Configure\Publish.log

:END

