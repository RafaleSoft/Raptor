!!ARBfp1.0
ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex1 = fragment.texcoord[1];
ATTRIB iLight = fragment.color.primary;

PARAM zero = { 0.0 , 0.0, 0.0, 1.0 };

TEMP color;
TEMP projection;
TEMP cull;
TEMP blend;

OUTPUT finalColor = result.color;

TXP color, iTex0 , texture[0] , 2D ;
MOV blend.w, color.w;
MUL color, iLight, color;
TXP projection, iTex1 , texture[1] , 2D ;
MUL projection, projection, projection.w;
SLT cull, zero.x, iTex1.w;
MAD_SAT color, cull, projection, color;
MOV finalColor, color;
MOV finalColor.w, blend.w;

END
