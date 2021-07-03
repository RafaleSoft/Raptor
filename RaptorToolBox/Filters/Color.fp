!!ARBfp1.0

#############################################################################
#                                                                           #
#   Color.fp                                                                #
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


#
# Simple B&W fragment program.
#

# ATTRIB iTex0 = fragment.texcoord[0];
# PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };
# PARAM percentage = program.local[0];
# PARAM baseColor = program.local[1];
# TEMP color;
# TEMP bw;
# OUTPUT finalColor = result.color;
# TEX color, iTex0, texture[0] , 2D ;
# DP3 bw, color, luminance;
# MUL bw, bw, baseColor;
# LRP finalColor, percentage.xxxx, bw, color;
# MOV finalColor.w, percentage.w;
# END


#
# Simple YUV converter fragment program.
#

# ATTRIB iTex0 = fragment.texcoord[0];
# PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };
# PARAM u_chrominance = { -0.14713, -0.28886, 0.436, 0.0 };
# PARAM v_chrominance = { 0.615, -0.51498, -0.10001, 0.0 };
# TEMP color;
# OUTPUT finalColor = result.color;
# TEX color, iTex0, texture[0] , 2D ;
# DP3 finalColor.x, color, luminance;
# DP3 finalColor.y, color, u_chrominance;
# DP3 finalColor.z, color, v_chrominance;
# MOV finalColor.w, color.w;
# END


#
# Simple RGB converter fragment program.
#

# ATTRIB iTex0 = fragment.texcoord[0];
# PARAM red = { 1.0, 0.0, 1.13983, 1.0 };
# PARAM green = { 1.0, -0.39465, 0.-58060, 0.0 };
# PARAM blue = { 1.0, 2.03211, 0.0, 0.0 };
# TEMP color;
# OUTPUT finalColor = result.color;
# TEX color, iTex0, texture[0] , 2D ;
# DP3 finalColor.x, color, red;
# DP3 finalColor.y, color, green;
# DP3 finalColor.z, color, blue;
# MOV finalColor.w, color.w;
# END


#
# Color control fragment program.
#

ATTRIB iTex0 = fragment.texcoord[0];

PARAM percentage = program.local[0];
PARAM baseColor = program.local[1];
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };
PARAM u_chrominance = { -0.14713, -0.28886, 0.436, 0.0 };
PARAM v_chrominance = { 0.615, -0.51498, -0.10001, 0.0 };
PARAM red = { 1.0, 0.0, 1.13983, 1.0 };
PARAM green = { 1.0, -0.39465, -0.58060, 0.0 };
PARAM blue = { 1.0, 2.03211, 0.0, 0.0 };

TEMP color;
TEMP yuvcolor;

OUTPUT finalColor = result.color;

TEX color, iTex0, texture[0] , 2D ;

DP3 yuvcolor.x, color, luminance;
DP3 yuvcolor.y, color, u_chrominance;
DP3 yuvcolor.z, color, v_chrominance;
MOV yuvcolor.w, color.w;
MUL yuvcolor, yuvcolor, percentage.wzzw;

DP3 color.x, yuvcolor, red;
DP3 color.y, yuvcolor, green;
DP3 color.z, yuvcolor, blue;
MUL color, color, percentage.yyyy;

MUL yuvcolor, baseColor, yuvcolor.xxxx;
LRP finalColor, percentage.xxxx, yuvcolor, color;
MOV finalColor.w, red.w;

END
