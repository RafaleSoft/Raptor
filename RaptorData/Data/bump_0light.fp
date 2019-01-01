!!ARBfp1.0 

ATTRIB iTex = fragment.texcoord[0];
ATTRIB ambient = fragment.color.primary;

PARAM expand = { 2.0 , -1.0, 4.0, 1.0 };
PARAM white = { 1.0, 1.0, 1.0, 1.0 };

TEMP normal;
TEMP diffuse;
TEMP color;

OUTPUT finalColor = result.color;

TEX color, iTex , texture[0] , 2D ;
MAD normal, normal, expand.xxxx, expand.yyyy;
MUL finalColor, ambient, color;
MOV finalColor.w, color.w;

END
