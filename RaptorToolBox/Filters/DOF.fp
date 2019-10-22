!!ARBfp1.0
OPTION ARB_precision_hint_nicest;

#############################################################################
#                                                                           #
#   DOF.fp                                                                  #
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


ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex2 = fragment.texcoord[2];
ATTRIB iTex3 = fragment.texcoord[3];
ATTRIB iTex4 = fragment.texcoord[4];
ATTRIB iTex5 = fragment.texcoord[5];
ATTRIB iTex6 = fragment.texcoord[6];
ATTRIB iTex7 = fragment.texcoord[7];

PARAM one = { 1.0, 1.0, 1.0, 1.0 };
PARAM one2 = { 1.0, 1.0, 0.0, 0.0 };
PARAM factor = { 0.125, 0.95, 50.0, 0.1666 };
PARAM dofParams = program.local[0];

TEMP color;
TEMP depth_base;
TEMP filter_base;
TEMP filter_CoC;
TEMP depth;
TEMP depth_factor;
TEMP depth_factor2;

OUTPUT finalColor = result.color;

TEX depth_base, iTex0, texture[1], 2D;
TEX filter_base, iTex0, texture[0] , 2D ;
TEX color, iTex2, texture[0] , 2D ;
TEX depth, iTex2, texture[1] , 2D ;
SGE depth_factor.x, depth, depth_base;

MUL filter_CoC, color, depth_factor.xxxx;
TEX color, iTex3, texture[0] , 2D ;
TEX depth, iTex3, texture[1] , 2D ;
SGE depth_factor.y, depth, depth_base;

MAD filter_CoC, color, depth_factor.yyyy, filter_CoC;
TEX color, iTex4, texture[0] , 2D ;
TEX depth, iTex4, texture[1] , 2D ;
SGE depth_factor.z, depth, depth_base;

MAD filter_CoC, color, depth_factor.zzzz, filter_CoC;
TEX color, iTex5, texture[0] , 2D ;
TEX depth, iTex5, texture[1] , 2D ;
SGE depth_factor.w, depth, depth_base;

MAD filter_CoC, color, depth_factor.wwww, filter_CoC;
TEX color, iTex6, texture[0] , 2D ;
TEX depth, iTex6, texture[1] , 2D ;
SGE depth_factor2.x, depth, depth_base;

MAD filter_CoC, color, depth_factor2.xxxx, filter_CoC;
TEX color, iTex7, texture[0] , 2D ;
TEX depth, iTex7, texture[1] , 2D ;
SGE depth_factor2.y, depth, depth_base;

MAD filter_CoC, color, depth_factor2.yyyy, filter_CoC;
DP4 depth_factor, depth_factor, one;
DP4 depth_factor2, depth_factor2, one2;
ADD depth_factor, depth_factor, depth_factor2;

#
# final equation
#

SUB_SAT depth_base, depth_base, dofParams.yyyy;
MUL_SAT depth_base, depth_base, dofParams.zzzz;
MUL filter_CoC, filter_CoC, depth_base;

MUL depth_factor, depth_factor, factor.xxxx;
MUL depth_factor, depth_factor, -depth_base;
MAD filter_CoC, filter_CoC, factor.xxxx, filter_base;

MAD finalColor, filter_base, depth_factor, filter_CoC;
MOV finalColor.w, one.w;

END

