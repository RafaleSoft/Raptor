!!ARBfp1.0
OPTION ARB_fragment_program_shadow;

ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex2 = fragment.texcoord[2];

PARAM one = { 0.0 , 0.001, 0.999, 1.0 } ;

TEMP shadow;
TEMP depth;
TEMP diffuse;
TEMP sFactor;

OUTPUT finalColor = result.color;

TEX diffuse, iTex0, texture[0], 2D;
TXP shadow, iTex2, texture[2], SHADOW2D;
SLT sFactor, iTex2.w, one.x;
ADD_SAT shadow, shadow, sFactor;
MUL diffuse, diffuse,shadow;
MUL finalColor, diffuse, fragment.color;
MOV finalColor.w, one.w;

END


