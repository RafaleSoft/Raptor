
#if !defined(AFX_3DSFILE_H__4ACF0CDF_B92E_11D3_85A1_CA8882000000__INCLUDED_)
#define AFX_3DSFILE_H__4ACF0CDF_B92E_11D3_85A1_CA8882000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Raptordll.h"

RAPTOR_NAMESPACE




#define MAX_NAME_LENGTH 255

typedef struct Chunktag
{
	unsigned short	chunk_id; 
    long			chunk_len;
} Chunk;

typedef struct MaterialAliastag
{
	string oldName;
	string newName;
} MaterialAlias;


typedef struct TexParamtag
{
	int		nTexture;
	float	uscale;
	float	vscale;
	float	uoffset;
	float	voffset;
} TexParam;


//class CMaterial;
typedef struct Materialtag
{
    CMaterial	*mat;
	TexParam	tp;
} Material;


//class CObject3D;
typedef struct NodeTrackTag
{
	short		id;
    CObject3D	*object;
	bool		doScale;
	bool		doTranslate;
	bool		doRotate;
	struct
	{
		float	posx;
		float	posy;
		float	posz;
	} pivot;
	struct
	{
		float	angle;
		float	axisx;
		float	axisy;
		float	axisz;
	} rotation;
	struct
	{
		float	vecx;
		float	vecy;
		float	vecz;
	} translation;
	struct
	{
		float	scalex;
		float	scaley;
		float	scalez;
	} scale;
} NodeTag;

typedef struct Statetag
{
	long				version;					// for versions and magic numbers
	char				name[MAX_NAME_LENGTH];		// for every cstr name
	float				scale;
	short				shading;					// type of shading : ex: phong = 3
	unsigned short		spercentage;
	float				fpercentage;
	unsigned char		cred,cgreen,cblue;
	float				fred,fgreen,fblue;
	unsigned short		sparam;
	float				fparam;
	long				lparam;
	float				matrix[4][3];
	bool				unparsed_data;
	NodeTag				currentNode;
} State;


//	0xxxH group
#define NULL_CHUNK				0x0000 
#define UNKNOWN_CHUNK			0x0001
#define M3D_VERSION				0x0002	//short version; 
#define M3D_KFVERSION			0x0005 
#define COLOR_F					0x0010	//float red, grn, blu; 
#define COLOR_24				0x0011	//char red, grn, blu; 
#define LIN_COLOR_24			0x0012	//char red, grn, blu; 
#define LIN_COLOR_F				0x0013	//float red, grn, blu; 
#define INT_PERCENTAGE			0x0030	//short percentage; 
#define FLOAT_PERCENTAGE		0x0031	//float percentage; 
#define MASTER_SCALE			0x0100	//float scale; 
#define CHUNK_TYPE				0x0995 
#define CHUNK_UNIQUE			0x0996 
#define NOT_CHUNK				0x0997 
#define CONTAINER				0x0998 
#define IS_CHUNK				0x0999 
#define C_SXP_SELFI_MASKDATA	0x0c3c 
        

//	1xxxH Group 
#define BIT_MAP					0x1100	//cstr filename; 
#define USE_BIT_MAP				0x1101	//
#define SOLID_BGND				0x1200	//followed by color_f 
#define USE_SOLID_BGND			0x1201	//
#define V_GRADIENT				0x1300	//followed by three color_f: start, mid, end
										//float midpoint; 
#define USE_V_GRADIENT			0x1301	//
#define LO_SHADOW_BIAS			0x1400	//float bias; 
#define HI_SHADOW_BIAS			0x1410	//
#define SHADOW_MAP_SIZE			0x1420	//short size; 
#define SHADOW_SAMPLES			0x1430
#define SHADOW_RANGE			0x1440
#define SHADOW_FILTER			0x1450	//float filter; 
#define RAY_BIAS				0x1460	//float bias; 
#define O_CONSTS				0x1500	//float plane_x, plane_y, plane_z; 



//	2xxxH Group 
#define AMBIENT_LIGHT			0x2100
#define FOG						0x2200	//followed by color_f, fog_bgnd
										//float near_plane, near_density;
										//float far_plane, far_density; 
#define USE_FOG					0x2201
#define FOG_BGND				0x2210
#define DISTANCE_CUE			0x2300	//followed by dcue_bgnd
										//float near_plane, near_density;
										//float far_plane, far_density; 
#define USE_DISTANCE_CUE		0x2301
#define LAYER_FOG				0x2302	//float fog_z_from, fog_z_to;
										//float fog_density;
										//short fog_type; 
#define USE_LAYER_FOG			0x2303
#define DCUE_BGND				0x2310
#define SMAGIC					0x2d2d
#define LMAGIC					0x2d3d
         


//	3xxxH Group 
#define DEFAULT_VIEW			0x3000
#define VIEW_TOP				0x3010	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_BOTTOM				0x3020	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_LEFT				0x3030	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_RIGHT				0x3040	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_FRONT				0x3050	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_BACK				0x3060	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_USER				0x3070	//float targe_x, target_y, target_z;
										//float view_width; 
#define VIEW_CAMERA				0x3080	//cstr camera_name; 
#define VIEW_WINDOW				0x3090
#define MDATA					0x3d3d	//Mesh Data Magic Number (.3DS files sub of 4d4d) 
#define MESH_VERSION			0x3d3e
#define MLIBMAGIC				0x3daa	//Material Library Magic Number (.MLI files) 
#define PRJMAGIC				0x3dc2	//3dS Project Magic Number (.PRJ files) 
#define MATMAGIC				0x3dff	//Material File Magic Number (.MAT files) 



//	4xxxH Group 
#define NAMED_OBJECT			0x4000	//starts an object : cstr name ( no length ); 
										//followed by n_tri_object
#define OBJ_HIDDEN				0x4010
#define OBJ_VIS_LOFTER			0x4011
#define OBJ_DOESNT_CAST			0x4012
#define OBJ_MATTE				0x4013
#define OBJ_FAST				0x4014
#define OBJ_PROCEDURAL			0x4015
#define OBJ_FROZEN				0x4016
#define OBJ_DONT_RCVSHADOW		0x4017
#define N_TRI_OBJECT			0x4100	//named triangle object
										//followed by point_array, point_flag_array, mesh_matrix,
										//face_array 
#define POINT_ARRAY				0x4110	//short npoints;
										//struct {
										//float x, y, z;
										//} points[npoints]; 
#define POINT_FLAG_ARRAY		0x4111	//short nflags;
										//short flags[nflags]; 
#define FACE_ARRAY				0x4120	//may be followed by smooth_group
										//short nfaces;
										//struct {
										//short vertex1, vertex2, vertex3;
										//short flags;
										//} facearray[nfaces]; 
#define MSH_MAT_GROUP			0x4130	//mesh_material_group
										//cstr material_name;
										//short nfaces;
										//short facenum[nfaces]; 
#define OLD_MAT_GROUP			0x4131
#define TEX_VERTS				0x4140	//short nverts;
										//struct {
										//float x, y;
										//} vertices[nverts]; 
#define SMOOTH_GROUP			0x4150	//short grouplist[n]; determined by length, seems to be 4 per face 
#define MESH_MATRIX				0x4160	//float matrix[4][3]; 
#define MESH_COLOR				0x4165	//short color_index; 
#define MESH_TEXTURE_INFO		0x4170	//short map_type;
										//float x_tiling, y_tiling;
										//float icon_x, icon_y, icon_z;
										//float matrix[4][3];
										//float scaling, plan_icon_w, plan_icon_h, cyl_icon_h; 
#define	PROC_NAME				0x4181
#define PROC_DATA				0x4182
#define MSH_BOXMAP				0x4190
#define N_D_L_OLD				0x4400
#define N_CAM_OLD				0x4500
#define	N_DIRECT_LIGHT			0x4600	//followed by color_f
										//float x, y, z; 
#define DL_SPOTLIGHT			0x4610	//float target_x, target_y, target_z;
										//float hotspot_ang;
										//float falloff_ang; 
#define DL_OFF					0x4620	
#define DL_ATTENUATE			0x4625
#define DL_RAYSHAD				0x4627
#define DL_SHADOWED				0x4630
#define DL_LOCAL_SHADOW			0x4640
#define DL_LOCAL_SHADOW2		0x4641
#define DL_SEE_CONE				0x4650
#define DL_SPOT_RECTANGULAR		0x4651
#define DL_SPOT_OVERSHOOT		0x4652
#define DL_SPOT_PROJECTOR		0x4653
#define DL_EXCLUDE				0x4654
#define DL_RANGE				0x4655
#define DL_SPOT_ROLL			0x4656	//float roll_ang; 
#define DL_SPOT_ASPECT			0x4657
#define DL_RAY_BIAS				0x4658	//float bias; 
#define DL_INNER_RANGE			0x4659	//float range; 
#define DL_OUTER_RANGE			0x465a	//float range; 
#define DL_MULTIPLIER			0x465b	//float multiple; 
#define N_AMBIENT_LIGHT			0x4680
#define N_CAMERA				0x4700	//float camera_x, camera_y, camera_z;
										//float target_x, target_y, target_z;
										//float bank_angle;
										//float focus; 
#define CAM_SEE_CONE			0x4710
#define CAM_RANGES				0x4720	//float near_range, far_range; 
#define M3DMAGIC				0x4d4d	//3DS Magic Number (.3DS file) 
#define HIERARCHY				0x4f00
#define PARENT_OBJECT			0x4f10
#define PIVOT_OBJECT			0x4f20
#define PIVOT_LIMITS			0x4f30
#define PIVOT_ORDER				0x4f40
#define XLATE_RANGE				0x4f50
        


//	5xxxH Group 
#define POLY_2D					0x5000
#define SHAPE_OK				0x5010
#define SHAPE_NOT_OK			0x5011
#define SHAPE_HOOK				0x5020
        


//	6xxxH Group 
#define	PATH_3D					0x6000
#define PATH_MATRIX				0x6005
#define SHAPE_2D				0x6010
#define M_SCALE					0x6020
#define M_TWIST					0x6030
#define M_TEETER				0x6040
#define M_FIT					0x6050
#define M_BEVEL					0x6060
#define XZ_CURVE				0x6070
#define YZ_CURVE				0x6080
#define INTERPCT				0x6090
#define DEFORM_LIMIT			0x60a0
#define USE_CONTOUR				0x6100
#define USE_TWEEN				0x6110
#define USE_SCALE				0x6120
#define USE_TWIST				0x6130
#define USE_TEETER				0x6140
#define USE_FIT					0x6150
#define USE_BEVEL				0x6160
         


//	7xxxH Group 
#define VIEWPORT_LAYOUT_OLD		0x7000
#define VIEWPORT_LAYOUT			0x7001	//followed by viewport_size, viewport_data
										//short form, top, ready, wstate, swapws, swapport, swapcur; 
#define VIEWPORT_DATA_OLD		0x7010
#define VIEWPORT_DATA			0x7011	//short flags, axis_lockout;
										//short win_x, win_y, win_w, winh_, win_view;
										//float zoom; 
										//float worldcenter_x, worldcenter_y, worldcenter_z;
										//float horiz_ang, vert_ang;
										//cstr camera_name; 
#define VIEWPORT_DATA_3			0x7012	//short flags, axis_lockout;
										//short win_x, win_y, win_w, winh_, win_view;
										//float zoom; 
										//float worldcenter_x, worldcenter_y, worldcenter_z;
										//float horiz_ang, vert_ang;
										//cstr camera_name; 
#define VIEWPORT_SIZE			0x7020	//short x, y, w, h; 
#define NETWORK_VIEW			0x7030
         


//	8xxxH Group 
#define XDATA_SECTION			0x8000
#define XDATA_ENTRY				0x8001
#define XDATA_APPNAME			0x8002
#define XDATA_STRING			0x8003
#define XDATA_FLOAT				0x8004
#define XDATA_DOUBLE			0x8005
#define XDATA_SHORT				0x8006
#define XDATA_LONG				0x8007
#define XDATA_VOID				0x8008
#define XDATA_GROUP				0x8009
#define XDATA_RFU6				0x800a
#define XDATA_RFU5				0x800b
#define XDATA_RFU4				0x800c
#define XDATA_RFU3				0x800d
#define XDATA_RFU2				0x800e
#define XDATA_RFU1				0x800f
#define PARENT_NAME				0x80f0
         


//	AxxxH Group 
#define MAT_NAME				0xa000	//cstr material_name; 
#define MAT_AMBIENT				0xa010	//followed by color chunk 
#define MAT_DIFFUSE				0xa020	//followed by color chunk 
#define MAT_SPECULAR			0xa030	//followed by color chunk 
#define MAT_SHININESS			0xa040	//followed by percentage chunk 
#define MAT_SHIN2PCT			0xa041	//followed by percentage chunk 
#define MAT_SHIN3PCT			0xa042	//followed by percentage chunk 
#define MAT_TRANSPARENCY		0xa050	//followed by percentage chunk 
#define MAT_XPFALL				0xa052	//followed by percentage chunk 
#define MAT_REFBLUR				0xa053	//followed by percentage chunk 
#define MAT_SELF_ILLUM			0xa080	//
#define MAT_TWO_SIDE			0xa081
#define MAT_DECAL				0xa082
#define MAT_ADDITIVE			0xa083
#define MAT_SELF_ILPCT			0xa084	//followed by percentage chunk 
#define MAT_WIRE				0xa085
#define MAT_SUPERSMP			0xa086
#define MAT_WIRESIZE			0xa087	//float wire_size; 
#define MAT_FACEMAP				0xa088
#define MAT_XPFALLIN			0xa08a
#define MAT_PHONGSOFT			0xa08c
#define MAT_WIREABS				0xa08e
#define MAT_SHADING				0xa100	//short shading_value; 
#define MAT_TEXMAP				0xa200	//followed by percentage chunk, mat_mapname,
										//mat_map_tiling, mat_map_texblur... 
#define MAT_SPECMAP				0xa204	//followed by percentage_chunk, mat_mapname 
#define MAT_OPACMAP				0xa210	//followed by percentage_chunk, mat_mapname 
#define MAT_REFLMAP				0xa220	//followed by percentage_chunk, mat_mapname 
#define MAT_BUMPMAP				0xa230	//followed by percentage_chunk, mat_mapname 
#define MAT_USE_XPFALL			0xa240	
#define MAT_USE_REFBLUR			0xa250
#define MAT_BUMP_PERCENT		0xa252
#define MAT_MAPNAME				0xa300	//cstr filename;
#define MAT_ACUBIC				0xa310
#define MAT_SXP_TEXT_DATA		0xa320
#define MAT_SXP_TEXT2_DATA		0xa321
#define MAT_SXP_OPAC_DATA		0xa322
#define MAT_SXP_BUMP_DATA		0xa324
#define MAT_SXP_SPEC_DATA		0xa325
#define MAT_SXP_SHIN_DATA		0xa326
#define MAT_SXP_SELFI_DATA		0xa328
#define MAT_SXP_TEXT_MASKDATA	0xa32a
#define MAT_SXP_TEXT2_MASKDATA	0xa32c
#define MAT_SXP_OPAC_MASKDATA	0xa32e
#define MAT_SXP_BUMP_MASKDATA	0xa330
#define MAT_SXP_SPEC_MASKDATA	0xa332
#define MAT_SXP_SHIN_MASKDATA	0xa334
#define MAT_SXP_SELFI_MASKDATA	0xa336
#define MAT_SXP_REFL_MASKDATA	0xa338
#define MAT_TEX2MAP				0xa33a
#define MAT_SHINMAP				0xa33c
#define MAT_SELFIMAP			0xa33d
#define MAT_TEXMASK				0xa33e
#define MAT_TEX2MASK			0xa340
#define MAT_OPACMASK			0xa342
#define MAT_BUMPMASK			0xa344
#define MAT_SHINMASK			0xa346
#define MAT_SPECMASK			0xa348
#define MAT_SELFIMASK			0xa34a
#define MAT_REFLMASK			0xa34c
#define MAT_MAP_TILINGOLD		0xa350
#define MAT_MAP_TILING			0xa351	//short flags;
#define MAT_MAP_TEXBLUR_OLD		0xa352
#define MAT_MAP_TEXBLUR			0xa353	//float blurring;
#define MAT_MAP_USCALE			0xa354
#define MAT_MAP_VSCALE			0xa356
#define MAT_MAP_UOFFSET			0xa358
#define MAT_MAP_VOFFSET			0xa35a
#define MAT_MAP_ANG				0xa35c
#define MAT_MAP_COL1			0xa360
#define MAT_MAP_COL2			0xa362
#define MAT_MAP_RCOL			0xa364
#define MAT_MAP_GCOL			0xa366
#define MAT_MAP_BCOL			0xa368
#define MAT_ENTRY				0xafff
         


//	BxxxH Group 
#define KFDATA					0xb000	//; followed by kfhdr 
#define AMBIENT_NODE_TAG		0xb001
#define OBJECT_NODE_TAG			0xb002	//followed by node_hdr, pivot, pos_track_tag, rot_track_tag, scl_track_tag, morph_smooth... 
#define CAMERA_NODE_TAG			0xb003	//followed by node_hdr, pos_track_tag, fov_track_tag, roll_track_tag... 
#define TARGET_NODE_TAG			0xb004	//followed by node_hdr, pos_track_tag... 
#define LIGHT_NODE_TAG			0xb005	//followed by node_hdr, pos_track_tag, col_track_tag... 
#define L_TARGET_NODE_TAG		0xb006	//followed by node_id, node_hdr, pos_track_tag 
#define SPOTLIGHT_NODE_TAG		0xb007	//followed by node_id, node_hdr, pos_track_tag, hot_track_tag, fall_track_tag, roll_track_tag, col_track_tag... 
#define KFSEG					0xb008	//short start, end; 
#define KFCURTIME				0xb009	//short curframe; 
#define KFHDR					0xb00a	//followed by viewport_layout, kfseg, kfcurtime, object_node_tag, light_node_tag, target_node_tag, camera_node_tag, l_target_node_tag,
										//spotlight_node_tag, ambient_node_tag...
										//short revision; 
										//cstr filename; 
										//short animlen; 
#define NODE_HDR				0xb010	//cstr objname;
										//short flags1;
										//short flags2; 
										//short hierarchy; ? 
#define INSTANCE_NAME			0xb011	
#define PRESCALE				0xb012
#define PIVOT					0xb013	//float pivot_x, pivot_y, pivot_z; 
#define BOUNDBOX				0xb014
#define MORPH_SMOOTH			0xb015	//float morph_smoothing_angle_rad; 
#define POS_TRACK_TAG			0xb020	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float pos_x, pos_y, pos_z; 
										//} pos[keys]; 
#define ROT_TRACK_TAG			0xb021	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float rotation_rad;
										//float axis_x, axis_y, axis_z; 
										//} rot[keys]; 
#define SCL_TRACK_TAG			0xb022	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float scale_x, scale_y, scale_z; 
										//} scale[keys]; 
#define FOV_TRACK_TAG			0xb023	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float camera_field_of_view;
										//} fov[keys] 
#define ROLL_TRACK_TAG			0xb024	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float camera_roll;
										//} roll[keys]; 
#define COL_TRACK_TAG			0xb025	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float red, rgn, blu;
										//} color[keys]; 
#define MORPH_TRACK_TAG			0xb026	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//cstr obj_name;
										//} morph[keys]; 
#define HOT_TRACK_TAG			0xb027	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float hotspot_ang;
										//} hotspot[keys]; 
#define FALL_TRACK_TAG			0xb028	//short flags;
										//short unknown[4];
										//short keys;
										//short unknown;
										//struct {
										//short framenum;
										//long unknown;
										//float falloff_ang;
										//} falloff[keys]; 
#define HIDE_TRACK_TAG			0xb029	
#define NODE_ID					0xb030	//short id; 



//	CxxxH Group 
#define C_MDRAWER				0xc010
#define C_TDRAWER				0xc020
#define C_SHPDRAWER				0xc030
#define C_MODDRAWER				0xc040
#define C_RIPDRAWER				0xc050
#define C_TXDRAWER				0xc060
#define C_PDRAWER				0xc062
#define C_MTLDRAWER				0xc064
#define C_FLIDRAWER				0xc066
#define C_CUBDRAWER				0xc067
#define C_MFILE					0xc070
#define C_SHPFILE				0xc080
#define C_MODFILE				0xc090
#define C_RIPFILE				0xc0a0
#define C_TXFILE				0xc0b0
#define C_PFILE					0xc0b2
#define C_MTLFILE				0xc0b4
#define C_FLIFILE				0xc0b6
#define C_PALFILE				0xc0b8
#define C_TX_STRING				0xc0c0
#define C_CONSTS				0xc0d0
#define C_SNAPS					0xc0e0
#define C_GRIDS					0xc0f0
#define C_ASNAPS				0xc100
#define C_GRID_RANGE			0xc110
#define C_RENDTYPE				0xc120
#define C_PROGMODE				0xc130
#define C_PREVMODE				0xc140
#define C_MODWMODE				0xc150
#define C_MODMODEL				0xc160
#define C_ALL_LINES				0xc170
#define C_BACK_TYPE				0xc180
#define C_MD_CS					0xc190
#define C_MD_CE					0xc1a0
#define C_MD_SML				0xc1b0
#define C_MD_SMW				0xc1c0
#define C_LOFT_WITH_TEXTURE		0xc1c3
#define C_LOFT_L_REPEAT			0xc1c4
#define C_LOFT_W_REPEAT			0xc1c5
#define C_LOFT_UV_NORMALIZE		0xc1c6
#define C_WELD_LOFT				0xc1c7
#define C_MD_PDET				0xc1d0
#define C_MD_SDET				0xc1e0
#define C_RGB_RMODE				0xc1f0
#define C_RGB_HIDE				0xc200
#define C_RGB_MAPSW				0xc202
#define C_RGB_TWOSIDE			0xc204
#define C_RGB_SHADOW			0xc208
#define C_RGB_AA				0xc210
#define C_RGB_OVW				0xc220
#define C_RGB_OVH				0xc230
#define CMAGIC					0xc23d
#define C_RGB_PICTYPE			0xc240
#define C_RGB_OUTPUT			0xc250
#define C_RGB_TODISK			0xc253
#define C_RGB_COMPRESS			0xc254
#define C_JPEG_COMPRESSION		0xc255
#define C_RGB_DISPDEV			0xc256
#define C_RGB_HARDDEV			0xc259
#define C_RGB_PATH				0xc25a
#define C_BITMAP_DRAWER			0xc25b
#define C_RGB_FILE				0xc260
#define C_RGB_OVASPECT			0xc270
#define C_RGB_ANIMTYPE			0xc271
#define C_RENDER_ALL			0xc272
#define C_REND_FROM				0xc273
#define C_REND_TO				0xc274
#define C_REND_NTH				0xc275
#define C_PAL_TYPE				0xc276
#define C_RND_TURBO				0xc277
#define C_RND_MIP				0xc278
#define C_BGND_METHOD			0xc279
#define C_AUTO_REFLECT			0xc27a
#define C_VP_FROM				0xc27b
#define C_VP_TO					0xc27c
#define C_VP_NTH				0xc27d
#define C_REND_TSTEP			0xc27e
#define C_VP_TSTEP				0xc27f
#define C_SRDIAM				0xc280
#define C_SRDEG					0xc290
#define C_SRSEG					0xc2a0
#define C_SRDIR					0xc2b0
#define C_HETOP					0xc2c0
#define C_HEBOT					0xc2d0
#define C_HEHT					0xc2e0
#define C_HETURNS				0xc2f0
#define C_HEDEG					0xc300
#define C_HESEG					0xc310
#define C_HEDIR					0xc320
#define C_QUIKSTUFF				0xc330
#define C_SEE_LIGHTS			0xc340
#define C_SEE_CAMERAS			0xc350
#define C_SEE_3D				0xc360
#define C_MESHSEL				0xc370
#define C_MESHUNSEL				0xc380
#define C_POLYSEL				0xc390
#define C_POLYUNSEL				0xc3a0
#define C_SHPLOCAL				0xc3a2
#define C_MSHLOCAL				0xc3a4
#define C_NUM_FORMAT			0xc3b0
#define C_ARCH_DENOM			0xc3c0
#define C_IN_DEVICE				0xc3d0
#define C_MSCALE				0xc3e0
#define C_COMM_PORT				0xc3f0
#define C_TAB_BASES				0xc400
#define C_TAB_DIVS				0xc410
#define C_MASTER_SCALES			0xc420
#define C_SHOW_1STVERT			0xc430
#define C_SHAPER_OK				0xc440
#define C_LOFTER_OK				0xc450
#define C_EDITOR_OK				0xc460
#define C_KEYFRAMER_OK			0xc470
#define C_PICKSIZE				0xc480
#define C_MAPTYPE				0xc490
#define C_MAP_DISPLAY			0xc4a0
#define C_TILE_XY				0xc4b0
#define C_MAP_XYZ				0xc4c0
#define C_MAP_SCALE				0xc4d0
#define C_MAP_MATRIX_OLD		0xc4e0
#define C_MAP_MATRIX			0xc4e1
#define C_MAP_WID_HT			0xc4f0
#define C_OBNAME				0xc500
#define C_CAMNAME				0xc510
#define C_LTNAME				0xc520
#define C_CUR_MNAME				0xc525
#define C_CURMTL_FROM_MESH		0xc526
#define C_GET_SHAPE_MAKE_FACES	0xc527
#define C_DETAIL				0xc530
#define C_VERTMARK				0xc540
#define C_MSHAX					0xc550
#define C_MSHCP					0xc560
#define C_USERAX				0xc570
#define C_SHOOK					0xc580
#define C_RAX					0xc590
#define C_STAPE					0xc5a0
#define C_LTAPE					0xc5b0
#define C_ETAPE					0xc5c0
#define C_KTAPE					0xc5c8
#define C_SPHSEGS				0xc5d0
#define C_GEOSMOOTH				0xc5e0
#define C_HEMISEGS				0xc5f0
#define C_PRISMSEGS				0xc600
#define C_PRISMSIDES			0xc610
#define C_TUBESEGS				0xc620
#define C_TUBESIDES				0xc630
#define C_TORSEGS				0xc640
#define C_TORSIDES				0xc650
#define C_CONESIDES				0xc660
#define C_CONESEGS				0xc661
#define C_NGPARMS				0xc670
#define C_PTHLEVEL				0xc680
#define C_MSCSYM				0xc690
#define C_MFTSYM				0xc6a0
#define C_MTTSYM				0xc6b0
#define C_SMOOTHING				0xc6c0
#define C_MODICOUNT				0xc6d0
#define C_FONTSEL				0xc6e0
#define C_TESS_TYPE				0xc6f0
#define C_TESS_TENSION			0xc6f1
#define C_SEG_START				0xc700
#define C_SEG_END				0xc705
#define C_CURTIME				0xc710
#define C_ANIMLENGTH			0xc715
#define C_PV_FROM				0xc720
#define C_PV_TO					0xc725
#define C_PV_DOFNUM				0xc730
#define C_PV_RNG				0xc735
#define C_PV_NTH				0xc740
#define C_PV_TYPE				0xc745
#define C_PV_METHOD				0xc750
#define C_PV_FPS				0xc755
#define C_VTR_FRAMES			0xc765
#define C_VTR_HDTL				0xc770
#define C_VTR_HD				0xc771
#define C_VTR_TL				0xc772
#define C_VTR_IN				0xc775
#define C_VTR_PK				0xc780
#define C_VTR_SH				0xc785
#define C_WORK_MTLS				0xc790
#define C_WORK_MTLS_2			0xc792
#define C_WORK_MTLS_3			0xc793
#define C_WORK_MTLS_4			0xc794
#define C_BGTYPE				0xc7a1
#define C_MEDTILE				0xc7b0
#define C_LO_CONTRAST			0xc7d0
#define C_HI_CONTRAST			0xc7d1
#define C_FROZ_DISPLAY			0xc7e0
#define C_BOOLWELD				0xc7f0
#define C_BOOLTYPE				0xc7f1
#define C_ANG_THRESH			0xc900
#define C_SS_THRESH				0xc901
#define C_TEXTURE_BLUR_DEFAULT	0xc903
#define C_MAPDRAWER				0xca00
#define C_MAPDRAWER1			0xca01
#define C_MAPDRAWER2			0xca02
#define C_MAPDRAWER3			0xca03
#define C_MAPDRAWER4			0xca04
#define C_MAPDRAWER5			0xca05
#define C_MAPDRAWER6			0xca06
#define C_MAPDRAWER7			0xca07
#define C_MAPDRAWER8			0xca08
#define C_MAPDRAWER9			0xca09
#define C_MAPDRAWER_ENTRY		0xca10
#define C_BACKUP_FILE			0xca20
#define C_DITHER_256			0xca21
#define C_SAVE_LAST				0xca22
#define C_USE_ALPHA				0xca23
#define C_TGA_DEPTH				0xca24
#define C_REND_FIELDS			0xca25
#define C_REFLIP				0xca26
#define C_SEL_ITEMTOG			0xca27
#define C_SEL_RESET				0xca28
#define C_STICKY_KEYINF			0xca29
#define C_WELD_THRESHOLD		0xca2a
#define C_ZCLIP_POINT			0xca2b
#define C_ALPHA_SPLIT			0xca2c
#define C_KF_SHOW_BACKFACE		0xca30
#define C_OPTIMIZE_LOFT			0xca40
#define C_TENS_DEFAULT			0xca42
#define C_CONT_DEFAULT			0xca44
#define C_BIAS_DEFAULT			0xca46
#define C_DXFNAME_SRC			0xca50
#define C_AUTO_WELD				0xca60
#define C_AUTO_UNIFY			0xca70
#define C_AUTO_SMOOTH			0xca80
#define C_DXF_SMOOTH_ANG		0xca90
#define C_SMOOTH_ANG			0xcaa0
#define C_WORK_MTLS_5			0xcb00
#define C_WORK_MTLS_6			0xcb01
#define C_WORK_MTLS_7			0xcb02
#define C_WORK_MTLS_8			0xcb03
#define C_WORKMTL				0xcb04
#define C_SXP_TEXT_DATA			0xcb10
#define C_SXP_OPAC_DATA			0xcb11
#define C_SXP_BUMP_DATA			0xcb12
#define C_SXP_SHIN_DATA			0xcb13
#define C_SXP_TEXT2_DATA		0xcb20
#define C_SXP_SPEC_DATA			0xcb24
#define C_SXP_SELFI_DATA		0xcb28
#define C_SXP_TEXT_MASKDATA		0xcb30
#define C_SXP_TEXT2_MASKDATA	0xcb32
#define C_SXP_OPAC_MASKDATA		0xcb34
#define C_SXP_BUMP_MASKDATA		0xcb36
#define C_SXP_SPEC_MASKDATA		0xcb38
#define C_SXP_SHIN_MASKDATA		0xcb3a
#define C_SXP_REFL_MASKDATA		0xcb3e
#define C_NET_USE_VPOST			0xcc00
#define C_NET_USE_GAMMA			0xcc10
#define C_NET_FIELD_ORDER		0xcc20
#define C_BLUR_FRAMES			0xcd00
#define C_BLUR_SAMPLES			0xcd10
#define C_BLUR_DUR				0xcd20
#define C_HOT_METHOD			0xcd30
#define C_HOT_CHECK				0xcd40
#define C_PIXEL_SIZE			0xcd50
#define C_DISP_GAMMA			0xcd60
#define C_FBUF_GAMMA			0xcd70
#define C_FILE_OUT_GAMMA		0xcd80
#define C_FILE_IN_GAMMA			0xcd82
#define C_GAMMA_CORRECT			0xcd84
#define C_APPLY_DISP_GAMMA		0xcd90
#define C_APPLY_FBUF_GAMMA		0xcda0
#define C_APPLY_FILE_GAMMA		0xcdb0
#define C_FORCE_WIRE			0xcdc0
#define C_RAY_SHADOWS			0xcdd0
#define C_MASTER_AMBIENT		0xcde0
#define C_SUPER_SAMPLE			0xcdf0
#define C_OBJECT_MBLUR			0xce00
#define C_MBLUR_DITHER			0xce10
#define C_DITHER_24				0xce20
#define C_SUPER_BLACK			0xce30
#define C_SAFE_FRAME			0xce40
#define C_VIEW_PRES_RATIO		0xce50
#define C_BGND_PRES_RATIO		0xce60
#define C_NTH_SERIAL_NUM		0xce70
         


//	DxxxH Group 
#define VPDATA					0xd000
#define P_QUEUE_ENTRY			0xd100
#define P_QUEUE_IMAGE			0xd110
#define P_QUEUE_USEIGAMMA		0xd114
#define P_QUEUE_PROC			0xd120
#define P_QUEUE_SOLID			0xd130
#define P_QUEUE_GRADIENT		0xd140
#define P_QUEUE_KF				0xd150
#define P_QUEUE_MOTBLUR			0xd152
#define P_QUEUE_MB_REPEAT		0xd153
#define P_QUEUE_NONE			0xd160
#define P_QUEUE_RESIZE			0xd180
#define P_QUEUE_OFFSET			0xd185
#define P_QUEUE_ALIGN			0xd190
#define P_CUSTOM_SIZE			0xd1a0
#define P_ALPH_NONE				0xd210
#define P_ALPH_PSEUDO			0xd220
#define P_ALPH_OP_PSEUDO		0xd221
#define P_ALPH_BLUR				0xd222
#define P_ALPH_PCOL				0xd225
#define P_ALPH_C0				0xd230
#define P_ALPH_OP_KEY			0xd231
#define P_ALPH_KCOL				0xd235
#define P_ALPH_OP_NOCONV		0xd238
#define P_ALPH_IMAGE			0xd240
#define P_ALPH_ALPHA			0xd250
#define P_ALPH_QUES				0xd260
#define P_ALPH_QUEIMG			0xd265
#define P_ALPH_CUTOFF			0xd270
#define P_ALPHANEG				0xd280
#define P_TRAN_NONE				0xd300
#define P_TRAN_IMAGE			0xd310
#define P_TRAN_FRAMES			0xd312
#define P_TRAN_FADEIN			0xd320
#define P_TRAN_FADEOUT			0xd330
#define P_TRANNEG				0xd340
#define P_RANGES				0xd400
#define P_PROC_DATA				0xd500
         


//	FxxxH Group
#define POS_TRACK_TAG_KEY		0xf020
#define ROT_TRACK_TAG_KEY		0xf021
#define SCL_TRACK_TAG_KEY		0xf022
#define FOV_TRACK_TAG_KEY		0xf023
#define ROLL_TRACK_TAG_KEY		0xf024
#define COL_TRACK_TAG_KEY		0xf025
#define MORPH_TRACK_TAG_KEY		0xf026
#define HOT_TRACK_TAG_KEY		0xf027
#define FALL_TRACK_TAG_KEY		0xf028
#define POINT_ARRAY_ENTRY		0xf110
#define POINT_FLAG_ARRAY_ENTRY	0xf111
#define FACE_ARRAY_ENTRY		0xf120
#define MSH_MAT_GROUP_ENTRY		0xf130
#define TEX_VERTS_ENTRY			0xf140
#define SMOOTH_GROUP_ENTRY		0xf150
#define DUMMY					0xffff
        
#endif // !defined(AFX_3DSFILE_H__4ACF0CDF_B92E_11D3_85A1_CA8882000000__INCLUDED_)
