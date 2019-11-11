!!ARBfp1.0

#############################################################################
#                                                                           #
#   HDR_composer.fp                                                         #
#                                                                           #
#     Raptor OpenGL & Vulkan realtime 3D Engine SDK.                        #
#                                                                           #
#   Copyright 1998-2019 by                                                  #
#   Fabrice FERRAND.                                                        #
#                                                                           #
#   This file is part of the Raptor project, and may only be used,          #
#   modified, and distributed under the terms of the Raptor project         #
#   license, LICENSE.  By continuing to use, modify, or distribute          #
#   this file you indicate that you have read the license and               #
#   understand and accept it fully.                                         #
#                                                                           #
#############################################################################


ATTRIB iTex = fragment.texcoord[0];

PARAM one = { 1.0 , 1.0, 1.0, 1.0 };
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };

TEMP color;
TEMP blur;
TEMP lwhite;
TEMP l;
TEMP factor;

OUTPUT finalColor = result.color;

TEX color, iTex, texture[0], 2D ;
TEX blur, iTex, texture[1], 2D ;
TEX lwhite, iTex, texture[2], 2D;
DP3 l, color, luminance;
MUL l, l, lwhite;

MAD finalColor, color, lwhite, blur;
MOV finalColor.w, one.w;

END
