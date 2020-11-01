'''
/***************************************************************************/
/*                                                                         */
/*  buildpack.py                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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

print("Making Raptor data package ...")
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
os.chdir(RAPTOR_ROOT + os.path.sep + "RaptorViewer")

datapackager = RAPTOR_ROOT + os.path.sep + "Redist"  + os.path.sep + "Bin" + os.path.sep + "RaptorDataPackager.exe"
if not os.path.exists(datapackager):
    print("  DataPackager not found !")
    exit(-1)
else:
    print("  Using DataPackager from:")
    print(datapackager)

if (os.path.exists("viewer.pck")):
    print("   Removing previous package...")
    os.remove("viewer.pck")

print("  Building package ...")
os.system(datapackager + " -C viewer.pck Start.tga Demo.xml")
os.chmod('viewer.pck', 664)
print("  Done.")

print("  Removing temporary files ...")
dir = os.listdir()
for f in dir:
    if f.endswith('.zip'):
        os.remove(f)

os.chdir(current_dir)

print("\nJob done !\n")


