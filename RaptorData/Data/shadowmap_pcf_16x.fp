!!ARBfp1.0
OPTION ARB_fragment_program_shadow;

ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex2 = fragment.texcoord[2];

OUTPUT finalColor = result.color;

PARAM texScale = { 0.0009765625 , 0.0009765625, 0.0, 0.0 } ;
PARAM offset1 = { -1.5 , -1.5, 0.0, 0.0 } ;
PARAM offset2 = { -0.5 , -1.5, 0.0, 0.0 } ;
PARAM offset3 = { 0.5 , -1.5, 0.0, 0.0 } ;
PARAM offset4 = { 1.5 , -1.5, 0.0, 0.0 } ;
PARAM offset5 = { -1.5 , -0.5, 0.0, 0.0 } ;
PARAM offset6 = { -0.5 , -0.5, 0.0, 0.0 } ;
PARAM offset7 = { 0.5 , -0.5, 0.0, 0.0 } ;
PARAM offset8 = { 1.5 , -0.5, 0.0, 0.0 } ;
PARAM offset9 = { -1.5 , 0.5, 0.0, 0.0 } ;
PARAM offset10 = { -0.5 , 0.5, 0.0, 0.0 } ;
PARAM offset11 = { 0.5 , 0.5, 0.0, 0.0 } ;
PARAM offset12 = { 1.5 , 0.5, 0.0, 0.0 } ;
PARAM offset13 = { -1.5 , 1.5, 0.0, 0.0 } ;
PARAM offset14 = { -0.5 , 1.5, 0.0, 0.0 } ;
PARAM offset15 = { 0.5 , 1.5, 0.0, 0.0 } ;
PARAM offset16 = { 1.5 , 1.5, 0.0, 0.0 } ;
PARAM coeffs = { 0.25, 0.25, 0.25, 0.25 };
PARAM one = { 0.0, 1.0, 1.0, 1.0 };

TEMP shadow;
TEMP shadowW;
TEMP finalShadow;
TEMP depth;
TEMP diffuse;
TEMP texCoord;
TEMP sFactor;

#   First row:                         
TEX diffuse, iTex0, texture[0], 2D;
MUL texCoord, offset1, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.x, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset2, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.y, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset3, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.z, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset4, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadowW.x, texCoord, texture[2], SHADOW2D;
MOV shadow.w, shadowW.x;
DP4 finalShadow.x, shadow, coeffs;

#   Second row:                  
MUL texCoord, offset5, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.x, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset6, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.y, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset7, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.z, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset8, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadowW.x, texCoord, texture[2], SHADOW2D;
MOV shadow.w, shadowW.x;
DP4 finalShadow.y, shadow, coeffs;

#   Third row:
MUL texCoord, offset9, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.x, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset10, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.y, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset11, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.z, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset12, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadowW.x, texCoord, texture[2], SHADOW2D;
MOV shadow.w, shadowW.x;
DP4 finalShadow.z, shadow, coeffs;

#   Fourth row:
MUL texCoord, offset13, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.x, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset14, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.y, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset15, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadow.z, texCoord, texture[2], SHADOW2D;
MUL texCoord, offset16, texScale;
MAD texCoord, texCoord, iTex2.w, iTex2;
TXP shadowW.x, texCoord, texture[2], SHADOW2D;
MOV shadow.w, shadowW.x;
DP4 finalShadow.w, shadow, coeffs;

#	All rows:
DP4 shadow, finalShadow, coeffs;

#   Final processing:
#	- Clip shadow
SLT sFactor, iTex2.w, one.x;
ADD_SAT shadow, shadow, sFactor;
#   - Apply shadow to incoming texture multiplied by current fragment
MUL diffuse, diffuse, shadow;
MUL finalColor, diffuse, fragment.color;
MOV finalColor.w, one.w;

END
