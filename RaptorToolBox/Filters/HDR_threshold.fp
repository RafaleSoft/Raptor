!!ARBfp1.0

#############################################################################
#                                                                           #
#   HDR_threshold.fp                                                        #
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


#  Compute luminance with bilinear components, averaging
#  the 4 surrouding of the texel's center.
	
ATTRIB iTex = fragment.texcoord[0];

PARAM offset = program.local[0];
PARAM treshhold = program.local[1];
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };

TEMP color1;
TEMP color2;
TEMP color3;
TEMP color4;
TEMP L;
TEMP tCoord;
TEMP color;

OUTPUT finalColor = result.color;

SUB tCoord, iTex, offset.xyww;
TEX color1, tCoord , texture[0] , 2D ;
DP3 L.x, color1, luminance;

ADD tCoord, iTex, offset.zyww;
TEX color2, tCoord , texture[0] , 2D ;
DP3 L.y, color2, luminance;

SUB tCoord, iTex, offset.zyww;
TEX color3, tCoord , texture[0] , 2D ;
DP3 L.z, color3, luminance;

ADD tCoord, iTex, offset.xyww;
TEX color4, tCoord , texture[0] , 2D ;
DP3 L.w, color4, luminance;

SUB L, L, treshhold.y;
MAX L, L, luminance.w;
MUL color, color1, L.x;
MAD color, color2, L.y, color;
MAD color, color3, L.z, color;
MAD color, color4, L.w, color;

MOV finalColor, color;
MOV finalColor.w, treshhold.w ;

END
