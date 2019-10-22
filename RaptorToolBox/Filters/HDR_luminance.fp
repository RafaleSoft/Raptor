!!ARBfp1.0

#############################################################################
#                                                                           #
#   HDR_luminance.fp                                                        #
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

PARAM offset = program.local[0];
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };

TEMP L;
TEMP LMax;
TEMP tCoord;
TEMP color;

OUTPUT finalColor = result.color;

SUB tCoord, iTex, offset.xyww;
TEX color, tCoord , texture[0] , 2D ;
DP3 LMax, color, luminance;

ADD tCoord, iTex, offset.zyww;
TEX color, tCoord , texture[0] , 2D ;
DP3 L, color, luminance;
MAX LMax, L, LMax;

SUB tCoord, iTex, offset.zyww;
TEX color, tCoord , texture[0] , 2D ;
DP3 L, color, luminance;
MAX LMax, L, LMax;

ADD tCoord, iTex, offset.xyww;
TEX color, tCoord , texture[0] , 2D ;
DP3 L, color, luminance;
MAX finalColor, L, LMax;

MOV finalColor.w, luminance.w ;

END
