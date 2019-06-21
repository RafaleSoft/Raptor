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

echo 	Making Raptor data package  ...
echo    Root of Raptor is set to: [$RAPTOR_ROOT] 

if  ( test -z "$RAPTOR_ROOT" )
then
	echo "Root is not defined, job aborted !"
	exit 1
fi

cd $RAPTOR_ROOT/RaptorData/Data

if !( test -f $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager)
then
	echo DataPackager not found !
	exit -1
else
	$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager

	echo Using DataPackager from:
	echo $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager
fi

$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager -C RaptorData.pck Raptor_logo_sml.txt rapsplsh.AVI lucon.ttf RaptorMessages.xml bump_0light.vp bump_0light.fp bump.vp bump.fp bump_att.vp bump_att.fp bump_att_2light.vp bump_att_2light.fp bump_att_3light.vp bump_att_3light.fp embm_0light.vp embm_0light.fp embm.vp embm.fp blinn.vs blinn.ps phong.vs phong.ps bump.vs bump.ps projection.fp shadowmap.fp shadowmap_pcf.fp shadowmap_pcf_4x.fp shadowmap_pcf_16x.fp blenderX_8x.vp blenderY_8x.vp blender_8x.fp AO.vs AO.ps

echo Removing temporary files
rm -rf "*.zip"

echo Delivering package ...
cd ..

if ( test -f RaptorData.pck )
then
	echo RaptorData package already exist, it will be replaced !
	rm -rf RaptorData.pck
fi

mv Data/RaptorData.pck .


