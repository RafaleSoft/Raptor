!!ARBfp1.0
ATTRIB iTex0 = fragment.texcoord[0];
ATTRIB iTex1 = fragment.texcoord[1];
ATTRIB iTex2 = fragment.texcoord[2];
ATTRIB iTex3 = fragment.texcoord[3];
ATTRIB ambient = fragment.color.primary;

PARAM expand = { 2.0 , -1.0, 4.0, 1.0 };

TEMP normal;
TEMP diffuse;
TEMP color;
TEMP U;
TEMP E;
TEMP R;

OUTPUT finalColor = result.color;

TEX normal, iTex0 , texture[1] , 2D ;
MAD normal, normal, expand.xxxx, expand.yyyy;
DP3 U.x, iTex1, normal;
DP3 U.y, iTex2, normal;
DP3 U.z, iTex3, normal;
MOV E.x, iTex1.w;
MOV E.y, iTex2.w;
MOV E.z, iTex3.w;
DP3 R, U, E;
MUL R, R, U;
MUL R, R, expand.xxxx;
SUB R, R, E;
MOV R.w, expand.w;
TEX color, R , texture[3] , CUBE ;
MUL diffuse, color, normal.zzzz;
MUL finalColor, ambient, diffuse;
MOV finalColor.w, color.w;

END
