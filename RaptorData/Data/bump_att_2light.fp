!!ARBfp1.0 
ATTRIB iTex = fragment.texcoord[0];
ATTRIB iAtt = fragment.texcoord[1];
ATTRIB iD1 = fragment.texcoord[2];
ATTRIB iS1 = fragment.texcoord[3];
ATTRIB iD2 = fragment.texcoord[4];
ATTRIB iS2 = fragment.texcoord[5];

PARAM expand = { 2.0 , -1.0, 4.0, 1.0 };
PARAM shininess = state.material.shininess;
PARAM ambient_mat = state.material.ambient;
PARAM diffuse_mat = state.material.diffuse;
PARAM specular_mat = state.material.specular;
PARAM globalAmbient = state.lightmodel.ambient;
PARAM specular_light1 = program.local[0];
PARAM specular_light2 = program.local[1];

TEMP normal;
TEMP diffuse;
TEMP specular;
TEMP color;
TEMP lit;

OUTPUT finalColor = result.color;

TEX normal, iTex , texture[1] , 2D ;
TEX color, iTex , texture[0] , 2D ;
MAD normal, normal, expand.xxxx, expand.yyyy;
MUL diffuse, ambient_mat,globalAmbient;

DP3_SAT lit.x, iD1, normal;
DP3_SAT lit.y, iS1, normal;
MOV lit.w, shininess.x;
LIT lit,lit;
MUL lit, lit, iAtt.xxxx;
MAD diffuse, diffuse_mat, lit.y, diffuse;
MUL specular, lit.z, specular_light1;

DP3_SAT lit.x, iD2, normal;
DP3_SAT lit.y, iS2, normal;
MOV lit.w, shininess.x;
LIT lit,lit;
MUL lit, lit, iAtt.yyyy;
MAD diffuse, diffuse_mat, lit.y, diffuse;
MAD specular, lit.z, specular_light2, specular;

MUL diffuse, color, diffuse;
MAD_SAT finalColor, specular, specular_mat, diffuse;
MOV finalColor.w, color.w;

END

