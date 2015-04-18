!!ARBfp1.0

ATTRIB iTex = fragment.texcoord[0];
ATTRIB iAtt = fragment.texcoord[1];
ATTRIB iDiffuse = fragment.texcoord[2];
ATTRIB iSpecular = fragment.texcoord[3];

PARAM shininess = state.material.shininess;
PARAM ambient_mat = state.material.ambient;
PARAM diffuse_mat = state.material.diffuse;
PARAM specular_mat = state.material.specular;
PARAM expand = { 2.0 , -1.0, 0.0, 1.0 };
PARAM globalAmbient = state.lightmodel.ambient;
PARAM specular_light = program.local[0];

TEMP normal;
TEMP ambient;
TEMP diffuse;
TEMP specular;
TEMP color;
TEMP lit;

OUTPUT finalColor = result.color;

TEX normal, iTex , texture[1] , 2D ;
TEX color, iTex , texture[0] , 2D ;
MAD normal, normal, expand.xxxx, expand.yyyy;
MUL ambient, ambient_mat,globalAmbient;
DP3_SAT lit.x, iDiffuse, normal;
DP3_SAT lit.y, iSpecular, normal;
MOV lit.w, shininess.x;
LIT lit,lit;
MUL lit, lit, iAtt;
MAD diffuse, diffuse_mat, lit.y, ambient;
MUL diffuse, color, diffuse;
MUL specular, lit.z, specular_light;
MAD_SAT finalColor, specular, specular_mat, diffuse;
MOV finalColor.w, color.w;

END
