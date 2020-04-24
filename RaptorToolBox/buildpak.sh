###########################################################################
#                                                                         #
#  buildpack.sh                                                           #
#                                                                         #
#    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       #
#                                                                         #
#  Copyright 1998-2019 by                                                 #
#  Fabrice FERRAND.                                                       #
#                                                                         #
#  This file is part of the Raptor project, and may only be used,         #
#  modified, and distributed under the terms of the Raptor project        #
#  license, LICENSE.  By continuing to use, modify, or distribute         #
#  this file you indicate that you have read the license and              #
#  understand and accept it fully.                                        #
#                                                                         #
###########################################################################
#!/bin/sh


echo 	Making ToolBox data package  ...
echo    Root of Raptor is set to: [$RAPTOR_ROOT] 

if  ( test -z "$RAPTOR_ROOT" )
then
	echo "Root is not defined, job aborted !"
	exit 1
fi

cd $RAPTOR_ROOT/RaptorToolBox/Filters

if !( test -f $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager)
then
	echo DataPackager not found !
	exit -1
else
	$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager

	echo Using DataPackager from:
	echo $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager
fi

$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager -C Toolbox.pck DOF.gs DOF.ps DOF.vs DOF_old.ps DOF.vp DOF.fp Color.ps Color_old.ps Color.fp Magnifier.gs Magnifier_X.ps Magnifier_Y.ps Magnifier.vs Magnifier_oldX.ps Magnifier_oldY.ps Magnifier.vp Magnifier_X.fp Magnifier_Y.fp HDR_threshold.ps HDR_composer.ps HDR_luminance.ps HDR_luminanceLast.ps HDR_threshold_old.ps HDR_composer_old.ps HDR_luminance_old.ps HDR_luminanceLast_old.ps HDR_threshold.fp HDR_composer.fp HDR_luminance.fp HDR_luminanceLast.fp MB_accum.ps MB_accum2.ps MB_accum_old.ps

pwd

ls *.zip
if (test $? -eq 0)
then
	echo Removing temporary files
	rm -rf *.zip
fi

echo Delivering package ...
cd ..

if ( test -f Toolbox.pck )
then
	echo Toolbox package already exist, it will be replaced !
	rm -rf Toolbox.pck
fi

mv Filters/Toolbox.pck .


