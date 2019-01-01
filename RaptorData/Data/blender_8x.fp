!!ARBfp1.0

ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex1 = fragment.texcoord[1];
ATTRIB iTex2 = fragment.texcoord[2];
ATTRIB iTex3 = fragment.texcoord[3];
ATTRIB iTex4 = fragment.texcoord[4];
ATTRIB iTex5 = fragment.texcoord[5];
ATTRIB iTex6 = fragment.texcoord[6];
ATTRIB iTex7 = fragment.texcoord[7];

OUTPUT finalColor = result.color;

PARAM factor = { 0.125, 0.125, 0.125, 0.125 };

TEMP color;
TEMP accum;

TEX accum, iTex0 , texture[0] , 2D ;
TEX color, iTex1 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex2 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex3 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex4 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex5 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex6 , texture[0] , 2D ;
ADD accum, color, accum;
TEX color, iTex7 , texture[0] , 2D ;
ADD accum, color, accum;
MUL finalColor, accum, factor;

END
