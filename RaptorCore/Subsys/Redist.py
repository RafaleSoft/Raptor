'''
/***************************************************************************/
/*                                                                         */
/*  Redist.py                                                              */
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

print("Making redistribuable package documentation ...")
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
os.chdir(RAPTOR_ROOT + os.path.sep + "RaptorCore"  + os.path.sep + "Subsys")

gendoc = False
with open('CodeGeneration.h') as f:
    for count, line in enumerate(f):
        if line.find("BUILD_REDIST") >0:
            gendoc = True

if not gendoc:
    print("  Keep previous documentation package, exiting !")
    exit(0)
else:
    print("  Generating HTML documentation ...")


doxygen = RAPTOR_ROOT + os.path.sep + "Build"  + os.path.sep + "doxygen.exe"
doxyfile = RAPTOR_ROOT + os.path.sep + "Build"  + os.path.sep + "Doxyfile"
if not os.path.exists(doxygen):
    print("  Doxygen not found !")
    exit(-1)
else:
    print("  Using Doxygen from:")
    print(doxygen)

if not os.path.exists(doxyfile):
    print("  Doxygen configuration file not found !")
    exit(-1)
else:
    print("  Using Doxyfile from:")
    print(doxyfile)

redist = RAPTOR_ROOT + os.path.sep + "Redist"  + os.path.sep + "Doc"
if not os.path.exists(redist):
    print("  Redistribuable directory for doucmentation not found !")
    exit(-1)
else:
    print("  Buliding documentation at location:")
    print(redist)

os.chdir(RAPTOR_ROOT)
print("  Building documentation package ...")
os.system(doxygen + " " + doxyfile)
print("  Done.")

os.chdir(current_dir)

print("\nJob done !\n")

