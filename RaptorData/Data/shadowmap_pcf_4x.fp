!!ARBfp1.0
OPTION ARB_fragment_program_shadow;

ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex2 = fragment.texcoord[2];

PARAM texScale = { 0.0009765625 , 0.0009765625, 0.0, 0.0 };
PARAM offset1 = { -0.5 , -0.5, 0.0, 0.0 };
PARAM offset2 = { -0.5 , 0.5, 0.0, 0.0 };
PARAM offset3 = { 0.5 , 0.5, 0.0, 0.0 };
PARAM offset4 = { 0.5 , -0.5, 0.0, 0.0 };
PARAM coeffs = { 0.25, 0.25, 0.25, 0.25 };
PARAM one = { 0.0, 1.0, 1.0, 1.0 };

TEMP shadow;
TEMP shadow2;
TEMP sFactor;
TEMP diffuse;
TEMP texCoord;

OUTPUT finalColor = result.color;

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
TXP shadow2.x, texCoord, texture[2], SHADOW2D;
MOV shadow.w, shadow2.x;
DP4 shadow, shadow, coeffs;

#   Final processing:
#	- Clip shadow
SLT sFactor, iTex2.w, one.x;
ADD_SAT shadow, shadow, sFactor;
#   - Apply shadow to incoming texture multiplied by current fragment
MUL diffuse, diffuse, shadow;
MUL finalColor, diffuse, fragment.color;
MOV finalColor.w, one.w;

END
