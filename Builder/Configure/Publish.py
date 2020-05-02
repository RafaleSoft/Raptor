
'''
/***************************************************************************/
/*                                                                         */
/*  Publish.py                                                             */
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

RAPTOR_ROOT = os.environ.get("RAPTOR_ROOT")
PLATFORM = os.environ.get("PLATFORM")
RAPTOR_VERSION = os.environ.get("RAPTOR_VERSION")
PUBLISH = os.environ.get("PUBLISH")

print("Checking publish directory ...")
print("Root of Raptor is set to: [", RAPTOR_ROOT, "]")
print("Platform is defined as: [", PLATFORM, "]")
print("Raptor Version is defined as: [", RAPTOR_VERSION, "]")
print("\n")

if len(RAPTOR_ROOT) < 1:
	print("Raptor Root is not defined, job aborted !");
	exit(-1);
if not os.path.exists(RAPTOR_ROOT):
    print("Raptor Root is not accessible, job aborted !");
    exit(-1);
if len(RAPTOR_VERSION) < 1:
	print("Raptor Version is not defined, job aborted !");
	exit(-1);

if len(PUBLISH) < 1:
	print("Publish base path is not defined, job aborted !");
	exit(-1);
else:
    print("Publish base path defined as: [", PUBLISH, "]");


current_dir = os.path.curdir
os.chdir(PUBLISH)

if os.path.exists("app.publish"):
    print("  Publish folder exist, it will be recreated ...")
    os.rmdir("app.publish")
else:
    print("  Publish folder missing, it will be created ...")

os.mkdir("app.publish")
os.chdir(current_dir)

print("\nJob done !\n")

