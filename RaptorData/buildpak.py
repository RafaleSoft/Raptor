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
os.chdir(RAPTOR_ROOT + os.path.sep + "RaptorData"  + os.path.sep + "Data")

datapackager = RAPTOR_ROOT + os.path.sep + "Redist"  + os.path.sep + "Bin" + os.path.sep + "RaptorDataPackager.exe"
if not os.path.exists(datapackager):
    print("  DataPackager not found !")
    exit(-1)
else:
    print("  Using DataPackager from:")
    print(datapackager)

if (os.path.exists("RaptorData.pck")):
    print("   Removing previous package...")
    os.remove("RaptorData.pck")

print("  Building package ...")
os.system(datapackager + " -C RaptorData.pck Raptor_logo_sml.txt rapsplsh.AVI lucon.ttf RaptorMessages.xml Raptor.glsl bump_0light.vp bump_0light.fp bump.vp bump.fp bump_att.vp bump_att.fp bump_att_2light.vp bump_att_2light.fp bump_att_3light.vp bump_att_3light.fp embm_0light.vp embm_0light.fp embm.vp embm.fp blinn.vs blinn.ps phong.vs phong.ps bump.vs bump.ps projection.fp shadowmap.fp shadowmap_pcf.fp shadowmap_pcf_4x.fp shadowmap_pcf_16x.fp blenderX_8x.vp blenderY_8x.vp blender_8x.fp AO.vs AO.ps blender_8x.gs blenderX_8x.ps blenderY_8x.ps tquad.vs tquad.ps tquad.gs empty.vs diffuse.ps particle.vs particle2D.gs particle3D.gs particle3D.ps font2D.vs font2D.gs font2D.ps box.vs box.gs box_line.gs box.ps vector_font.vs vector_font.gs vector_font.ps")
os.chmod('RaptorData.pck', 664)
print("  Done.")

print("  Removing temporary files ...")
dir = os.listdir()
for f in dir:
    if f.endswith('.zip'):
        os.remove(f)

print("  Delivering package ...")
os.chdir("..")

if os.path.exists("RaptorData.pck"):
    print("  RaptorData package already exist, it will be replaced !")
    os.remove("RaptorData.pck")
        
shutil.move("Data" + os.path.sep + "RaptorData.pck",".")

os.chdir(current_dir)

print("\nJob done !\n")


