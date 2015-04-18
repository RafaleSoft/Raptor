!!ARBfp1.0

ATTRIB iTex = fragment.texcoord[0];
ATTRIB iDiffuse = fragment.color.primary;
ATTRIB iSpecular = fragment.color.secondary;

PARAM expand = { 2.0 , -1.0, 4.0, 1.0 };

TEMP normal;
TEMP diffuse;
TEMP specular;
TEMP color;

OUTPUT finalColor = result.color;

TEX normal, iTex , texture[1] , 2D ;
TEX color, iTex , texture[0] , 2D ;
MAD normal, normal, expand.xxxx, expand.yyyy;
MAD diffuse, iDiffuse, expand.xxxx, expand.yyyy;
MAD specular, iSpecular, expand.xxxx, expand.yyyy;
DP3_SAT diffuse, diffuse, normal;
DP3_SAT specular, specular, normal;
MUL diffuse, color, diffuse;
MUL specular, specular, specular;
MUL specular, specular, specular;
MAD finalColor, specular, normal.w, diffuse;
MOV finalColor.w, color.w;

END
