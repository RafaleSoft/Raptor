!!ARBfp1.0

ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex2 = fragment.texcoord[2];

OUTPUT finalColor = result.color;

PARAM one = { 0.002 , 1.0, 1.0, 1.0 } ;

TEMP shadow;
TEMP depth;
TEMP diffuse;

TEX diffuse, iTex0, texture[0], 2D;
TXP shadow, iTex2, texture[2], 2D;
RCP depth, iTex2.w;
MUL depth, depth , iTex2;
SGE shadow, shadow.x, depth.z;
SLT depth, iTex2.w, one.x;
ADD_SAT shadow, shadow, depth;
MUL diffuse, diffuse,shadow;
MUL finalColor, diffuse, fragment.color;
MOV finalColor.w, one.w;

END
