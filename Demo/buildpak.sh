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


echo 	Making Demo data package  ...
echo    Root of Raptor is set to: [$RAPTOR_ROOT] 

if  ( test -z "$RAPTOR_ROOT" )
then
	echo "Root is not defined, job aborted !"
	exit 1
fi


cd $RAPTOR_ROOT/Demo/Datas


if !( test -f $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager)
then
	echo DataPackager not found !
	exit -1
else
	$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager

	echo Using DataPackager from:
	echo $RAPTOR_ROOT/Redist/Bin/RaptorDataPackager
fi

$RAPTOR_ROOT/Redist/Bin/RaptorDataPackager -C Demo.pck BRICKS.jpg BRICKS2.jpg BRICKS3.jpg BRICKS4.jpg BRICKS5.jpg BRICKS6.jpg BRICKS7.jpg BRICKS8.jpg BRICKS9.jpg BRICKS10.jpg colonneLow.3DS Columns.3DS ColumnsLow.3DS Marble.jpg Marble2.jpg Marble3.jpg Marble4.jpg Marble5.jpg Marble6.jpg Sculpt.tga OLDWOOD2.JPG LensFlare2.tga lrock049.jpg Fire.tga flare0.jpg flare1.jpg flare3.jpg flare4.jpg flare5.jpg flare6.jpg flare7.jpg flare8.jpg flare10.jpg

pwd

ls *.zip
if (test $? -eq 0)
then
	echo Removing temporary files
	rm -rf *.zip
fi

echo Delivering package ...
cd ..

if ( test -f Demo.pck )
then
	echo Demo package already exist, it will be replaced !
	rm -rf Demo.pck
fi

mv Datas/Demo.pck .



