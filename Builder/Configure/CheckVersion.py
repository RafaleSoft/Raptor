''' 
/***************************************************************************/
/*                                                                         */
/*  CheckVersion.py                                                        */
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

print("Checking file versions...")
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

current_dir = os.path.curdir
os.chdir(RAPTOR_ROOT)

versions = ("GLBench" + os.path.sep + "GLBench.rc", 
            "UnitTest" + os.path.sep + "UnitTest.rc",
            "Rays2" + os.path.sep + "RaysServer" + os.path.sep + "RaysServer.rc",
            "RaptorCore" + os.path.sep + "RaptorCore.rc",
            "Demo" + os.path.sep + "Demo.rc",
            "Test2" + os.path.sep + "Test2.rc",
            "Test5" + os.path.sep + "Test5.rc",
            "SIMD" + os.path.sep + "simd.rc",
            "RaptorViewer" + os.path.sep + "RaptorViewer.rc",
            "RaptorToolBox" + os.path.sep + "RaptorToolBox.rc",
            "RaptorServer" + os.path.sep + "RaptorServer.rc",
            "RaptorNetwork" + os.path.sep + "RaptorNetwork.rc",
            "RaptorData" + os.path.sep + "RaptorDataPackager" + os.path.sep + "RaptorDataPackager.rc",
            "RaptorData" + os.path.sep + "RaptorData.rc",
            "RaptorCompute" + os.path.sep + "RaptorCompute.rc",
            "MicroLex" + os.path.sep + "MicroLex.rc",
            "GLObjectViewer" + os.path.sep + "GLObjectViewer.rc",
            "Builder" + os.path.sep + "BuilderNative.rc")

for file in versions:
    print("Checking file: [",file,"]")

    found = False
    uptodate = False
    with open(file, 'r') as f:
        for count, line in enumerate(f):
            if line.find("FILEVERSION") >= 0:
                found = True
                v = line.split()
                vv = ""
                if len(v) > 2:
                    vv = v[1].rstrip(',')+"."+v[2].rstrip(',')+"."+v[3].rstrip(',')
                else:
                    vv = v[1].replace(',','.')
                uptodate = (vv == RAPTOR_VERSION)
        
    if not found:
        print("  No product version found, skipping file ...")
    elif not uptodate:
        out = open(file+'.new','w')

        with open(file, 'r') as f:
            for count, line in enumerate(f):
                if line.find("FILEVERSION") >= 0:
                    out.write(" FILEVERSION "+RAPTOR_VERSION.replace('.',', ')+", 0\n")
                elif line.find("PRODUCTVERSION") >= 0:
                    out.write(" PRODUCTVERSION "+RAPTOR_VERSION.replace('.',', ')+", 0\n")
                else:
                    pos = line.find("FileVersion")
                    if pos > 0:
                        out.write(line[0:pos+14]+'"'+RAPTOR_VERSION+'.0"\n')
                    else:
                        pos = line.find("ProductVersion")
                        if pos > 0:
                            out.write(line[0:pos+17]+'"'+RAPTOR_VERSION+'.0"\n')
                        else:
                            out.write(line)

        out.close()
        # os.rename(file,file+".old")
        os.remove(file)
        os.rename(file+".new",file)
    else:
        print("  Product version up-to-date, skipping file ...")


os.chdir(current_dir)

print("\nJob done !\n")
