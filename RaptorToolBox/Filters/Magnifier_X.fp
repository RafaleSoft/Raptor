!!ARBfp1.0

#############################################################################
#                                                                           #
#   Magnifier_X.fp                                                          #
#                                                                           #
#     Raptor OpenGL & Vulkan realtime 3D Engine SDK.                        #
#                                                                           #
#   Copyright 1998-2021 by                                                  #
#   Fabrice FERRAND.                                                        #
#                                                                           #
#   This file is part of the Raptor project, and may only be used,          #
#   modified, and distributed under the terms of the Raptor project         #
#   license, LICENSE.  By continuing to use, modify, or distribute          #
#   this file you indicate that you have read the license and               #
#   understand and accept it fully.                                         #
#                                                                           #
#############################################################################


ATTRIB iTex0 = fragment.texcoord[0];

PARAM one_plusx = program.local[0];
PARAM size = program.local[1];

TEMP color;
TEMP factors;
TEMP subpixels;
TEMP colorsum;
TEMP offset;

OUTPUT finalColor = result.color;

MUL subpixels, iTex0, size;
FRC subpixels, subpixels;

TEX color, iTex0, texture[0] , 2D ;
TEX factors, subpixels.xyzw, texture[1], 2D;

MUL colorsum, color, factors.zzzz;
ADD offset, iTex0, one_plusx.xzzz;
TEX color, offset, texture[0] , 2D ;
MAD colorsum, color, factors.wwww, colorsum;
SUB offset, iTex0, one_plusx.xzzz;
TEX color, offset, texture[0] , 2D ;
MAD colorsum, color, factors.yyyy, colorsum;
SUB offset, offset, one_plusx.xzzz;
TEX color, offset, texture[0] , 2D ;
MAD finalColor, color, factors.xxxx, colorsum;
MOV finalColor.w, size.w;

END
