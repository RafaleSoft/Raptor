'''
/***************************************************************************/
/*                                                                         */
/*  buildpack.py                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
'''


import os
import sys
import shutil

RAPTOR_ROOT = os.environ.get("RAPTOR_ROOT")
PLATFORM = os.environ.get("PLATFORM")
RAPTOR_VERSION = os.environ.get("RAPTOR_VERSION")

print("Making Raptor Toolbox data package ...")
print("Root of Raptor is set to: [", RAPTOR_ROOT, "]")
print("Platform is defined as: [", PLATFORM, "]")
print("Raptor Version is defined as: [", RAPTOR_VERSION, "]")
print("\n")

if len(RAPTOR_ROOT) < 1:
	print("Raptor Root is not defined, job aborted !")
	exit(-1)
if not os.path.exists(RAPTOR_ROOT):
    print("Raptor Root is not accessible, job aborted !")
    exit(-1)


current_dir = os.path.curdir
os.chdir(RAPTOR_ROOT + os.path.sep + "RaptorToolBox"  + os.path.sep + "Filters")

datapackager = RAPTOR_ROOT + os.path.sep + "Redist"  + os.path.sep + "Bin" + os.path.sep + "RaptorDataPackager.exe"
if not os.path.exists(datapackager):
    print("  DataPackager not found !")
    exit(-1)
else:
    print("  Using DataPackager from:")
    print(datapackager)

if (os.path.exists("Toolbox.pck")):
    print("   Removing previous package...")
    os.remove("Toolbox.pck")

print("  Building package ...")
os.system(datapackager + " -C Toolbox.pck DOF.gs DOF.ps DOF.vs DOF_old.ps DOF.vp DOF.fp Color.ps Color_old.ps Color.fp Magnifier.gs Magnifier_X.ps Magnifier_Y.ps Magnifier.vs Magnifier_oldX.ps Magnifier_oldY.ps Magnifier.vp Magnifier_X.fp Magnifier_Y.fp HDR_threshold.ps HDR_composer.ps HDR_luminance.ps HDR_luminanceLast.ps HDR_threshold_old.ps HDR_composer_old.ps HDR_luminance_old.ps HDR_luminanceLast_old.ps HDR_threshold.fp HDR_composer.fp HDR_luminance.fp HDR_luminanceLast.fp MB_accum.ps MB_accum2.ps MB_accum_old.ps")
os.chmod('Toolbox.pck', 664)
print("  Done.")

print("  Removing temporary files ...")
dir = os.listdir()
for f in dir:
    if f.endswith('.zip'):
        os.remove(f)

print("  Delivering package ...")
os.chdir("..")

if os.path.exists("Toolbox.pck"):
    print("  Raptor Toolbox data package already exist, it will be replaced !")
    os.remove("Toolbox.pck")
        
shutil.move("Filters" + os.path.sep + "Toolbox.pck",".")

os.chdir(current_dir)

print("\nJob done !\n")


