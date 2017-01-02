//! This header must be included instead of any generic glext.h extensions file.
//! It provides the necessary extensions to the SDK, they have been extensively
//! tested, and obselete extentions are removed despite backward compatibility needs.
//! Besides, all standard extensions ( ARB ) are prefered to vendor's specifics, which
//! should disappear un future versions of Raptor.
//! If this header cannot be included to prefer the OS version ( or because this header is
//! incomplete ), all function prototypes defined here under must be defined to compile 
//! and use Raptor, it should work fine.
//!
//! The directive RAPTOR_GLEXT is given here instead of __glext_h_ to be able to easily
//! use one of the possible extensions header file. The only place where gl is included is 
//! in this file.
#ifndef __RAPTOR_GLEXT_H__
#define __RAPTOR_GLEXT_H__

//! This macro prevents inclusion of other "glext" definitions
#define __glext_h_
#include <GL/gl.h>

/*
	OpenGL extensions features for Raptor framework
*/
#include "Subsys/CodeGeneration.h"

//	OGL versions before 2 has C entry points.
#ifdef __cplusplus
extern "C" {
#endif


/*	GL VERSION 1.2	*/
#if defined(GL_VERSION_1_2)
	#define GL_UNSIGNED_BYTE_3_3_2            0x8032
	#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
	#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
	#define GL_UNSIGNED_INT_8_8_8_8           0x8035
	#define GL_UNSIGNED_INT_10_10_10_2        0x8036
	#define GL_RESCALE_NORMAL                 0x803A
	#define GL_TEXTURE_BINDING_3D             0x806A
	#define GL_PACK_SKIP_IMAGES               0x806B
	#define GL_PACK_IMAGE_HEIGHT              0x806C
	#define GL_UNPACK_SKIP_IMAGES             0x806D
	#define GL_UNPACK_IMAGE_HEIGHT            0x806E
	#define GL_TEXTURE_3D                     0x806F
	#define GL_PROXY_TEXTURE_3D               0x8070
	#define GL_TEXTURE_DEPTH                  0x8071
	#define GL_TEXTURE_WRAP_R                 0x8072
	#define GL_MAX_3D_TEXTURE_SIZE            0x8073
	#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
	#define GL_UNSIGNED_SHORT_5_6_5           0x8363
	#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
	#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
	#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
	#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
	#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
	#define GL_BGR                            0x80E0
	#define GL_BGRA                           0x80E1
	#define GL_MAX_ELEMENTS_VERTICES          0x80E8
	#define GL_MAX_ELEMENTS_INDICES           0x80E9
	#define GL_CLAMP_TO_EDGE                  0x812F
	#define GL_TEXTURE_MIN_LOD                0x813A
	#define GL_TEXTURE_MAX_LOD                0x813B
	#define GL_TEXTURE_BASE_LEVEL             0x813C
	#define GL_TEXTURE_MAX_LEVEL              0x813D
	#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
	#define GL_SINGLE_COLOR                   0x81F9
	#define GL_SEPARATE_SPECULAR_COLOR        0x81FA
	#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
	#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
	#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
	#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
	#define GL_ALIASED_POINT_SIZE_RANGE       0x846D
	#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#endif

/*	GL VERSION 1.3	*/
#if defined(GL_VERSION_1_3)
	#define GL_TEXTURE0                       0x84C0
	#define GL_TEXTURE1                       0x84C1
	#define GL_TEXTURE2                       0x84C2
	#define GL_TEXTURE3                       0x84C3
	#define GL_TEXTURE4                       0x84C4
	#define GL_TEXTURE5                       0x84C5
	#define GL_TEXTURE6                       0x84C6
	#define GL_TEXTURE7                       0x84C7
	#define GL_TEXTURE8                       0x84C8
	#define GL_TEXTURE9                       0x84C9
	#define GL_TEXTURE10                      0x84CA
	#define GL_TEXTURE11                      0x84CB
	#define GL_TEXTURE12                      0x84CC
	#define GL_TEXTURE13                      0x84CD
	#define GL_TEXTURE14                      0x84CE
	#define GL_TEXTURE15                      0x84CF
	#define GL_TEXTURE16                      0x84D0
	#define GL_TEXTURE17                      0x84D1
	#define GL_TEXTURE18                      0x84D2
	#define GL_TEXTURE19                      0x84D3
	#define GL_TEXTURE20                      0x84D4
	#define GL_TEXTURE21                      0x84D5
	#define GL_TEXTURE22                      0x84D6
	#define GL_TEXTURE23                      0x84D7
	#define GL_TEXTURE24                      0x84D8
	#define GL_TEXTURE25                      0x84D9
	#define GL_TEXTURE26                      0x84DA
	#define GL_TEXTURE27                      0x84DB
	#define GL_TEXTURE28                      0x84DC
	#define GL_TEXTURE29                      0x84DD
	#define GL_TEXTURE30                      0x84DE
	#define GL_TEXTURE31                      0x84DF
	#define GL_ACTIVE_TEXTURE                 0x84E0
	#define GL_CLIENT_ACTIVE_TEXTURE          0x84E1
	#define GL_MAX_TEXTURE_UNITS              0x84E2
	#define GL_TRANSPOSE_MODELVIEW_MATRIX     0x84E3
	#define GL_TRANSPOSE_PROJECTION_MATRIX    0x84E4
	#define GL_TRANSPOSE_TEXTURE_MATRIX       0x84E5
	#define GL_TRANSPOSE_COLOR_MATRIX         0x84E6
	#define GL_MULTISAMPLE                    0x809D
	#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
	#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
	#define GL_SAMPLE_COVERAGE                0x80A0
	#define GL_SAMPLE_BUFFERS                 0x80A8
	#define GL_SAMPLES                        0x80A9
	#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
	#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
	#define GL_MULTISAMPLE_BIT                0x20000000
	#define GL_NORMAL_MAP                     0x8511
	#define GL_REFLECTION_MAP                 0x8512
	#define GL_TEXTURE_CUBE_MAP               0x8513
	#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
	#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
	#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
	#define GL_COMPRESSED_ALPHA               0x84E9
	#define GL_COMPRESSED_LUMINANCE           0x84EA
	#define GL_COMPRESSED_LUMINANCE_ALPHA     0x84EB
	#define GL_COMPRESSED_INTENSITY           0x84EC
	#define GL_COMPRESSED_RGB                 0x84ED
	#define GL_COMPRESSED_RGBA                0x84EE
	#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
	#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
	#define GL_TEXTURE_COMPRESSED             0x86A1
	#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
	#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
	#define GL_CLAMP_TO_BORDER                0x812D
	#define GL_COMBINE                        0x8570
	#define GL_COMBINE_RGB                    0x8571
	#define GL_COMBINE_ALPHA                  0x8572
	#define GL_SOURCE0_RGB                    0x8580
	#define GL_SOURCE1_RGB                    0x8581
	#define GL_SOURCE2_RGB                    0x8582
	#define GL_SOURCE0_ALPHA                  0x8588
	#define GL_SOURCE1_ALPHA                  0x8589
	#define GL_SOURCE2_ALPHA                  0x858A
	#define GL_OPERAND0_RGB                   0x8590
	#define GL_OPERAND1_RGB                   0x8591
	#define GL_OPERAND2_RGB                   0x8592
	#define GL_OPERAND0_ALPHA                 0x8598
	#define GL_OPERAND1_ALPHA                 0x8599
	#define GL_OPERAND2_ALPHA                 0x859A
	#define GL_RGB_SCALE                      0x8573
	#define GL_ADD_SIGNED                     0x8574
	#define GL_INTERPOLATE                    0x8575
	#define GL_SUBTRACT                       0x84E7
	#define GL_CONSTANT                       0x8576
	#define GL_PRIMARY_COLOR                  0x8577
	#define GL_PREVIOUS                       0x8578
	#define GL_DOT3_RGB                       0x86AE
	#define GL_DOT3_RGBA                      0x86AF
#endif

/*	GL VERSION 1.4	*/
#if defined(GL_VERSION_1_4)
	#define GL_BLEND_DST_RGB                  0x80C8
	#define GL_BLEND_SRC_RGB                  0x80C9
	#define GL_BLEND_DST_ALPHA                0x80CA
	#define GL_BLEND_SRC_ALPHA                0x80CB
	#define GL_POINT_SIZE_MIN                 0x8126
	#define GL_POINT_SIZE_MAX                 0x8127
	#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
	#define GL_POINT_DISTANCE_ATTENUATION     0x8129
	#define GL_GENERATE_MIPMAP                0x8191
	#define GL_GENERATE_MIPMAP_HINT           0x8192
	#define GL_DEPTH_COMPONENT16              0x81A5
	#define GL_DEPTH_COMPONENT24              0x81A6
	#define GL_DEPTH_COMPONENT32              0x81A7
	#define GL_MIRRORED_REPEAT                0x8370
	#define GL_FOG_COORDINATE_SOURCE          0x8450
	#define GL_FOG_COORDINATE                 0x8451
	#define GL_FRAGMENT_DEPTH                 0x8452
	#define GL_CURRENT_FOG_COORDINATE         0x8453
	#define GL_FOG_COORDINATE_ARRAY_TYPE      0x8454
	#define GL_FOG_COORDINATE_ARRAY_STRIDE    0x8455
	#define GL_FOG_COORDINATE_ARRAY_POINTER   0x8456
	#define GL_FOG_COORDINATE_ARRAY           0x8457
	#define GL_COLOR_SUM                      0x8458
	#define GL_CURRENT_SECONDARY_COLOR        0x8459
	#define GL_SECONDARY_COLOR_ARRAY_SIZE     0x845A
	#define GL_SECONDARY_COLOR_ARRAY_TYPE     0x845B
	#define GL_SECONDARY_COLOR_ARRAY_STRIDE   0x845C
	#define GL_SECONDARY_COLOR_ARRAY_POINTER  0x845D
	#define GL_SECONDARY_COLOR_ARRAY          0x845E
	#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
	#define GL_TEXTURE_FILTER_CONTROL         0x8500
	#define GL_TEXTURE_LOD_BIAS               0x8501
	#define GL_INCR_WRAP                      0x8507
	#define GL_DECR_WRAP                      0x8508
	#define GL_TEXTURE_DEPTH_SIZE             0x884A
	#define GL_DEPTH_TEXTURE_MODE             0x884B
	#define GL_TEXTURE_COMPARE_MODE           0x884C
	#define GL_TEXTURE_COMPARE_FUNC           0x884D
	#define GL_COMPARE_R_TO_TEXTURE           0x884E
#endif

/*	GL VERSION 1.5	*/
#if defined(GL_VERSION_1_5)
	#define GL_BUFFER_SIZE                    0x8764
	#define GL_BUFFER_USAGE                   0x8765
	#define GL_QUERY_COUNTER_BITS             0x8864
	#define GL_CURRENT_QUERY                  0x8865
	#define GL_QUERY_RESULT                   0x8866
	#define GL_QUERY_RESULT_AVAILABLE         0x8867
	#define GL_ARRAY_BUFFER                   0x8892
	#define GL_ELEMENT_ARRAY_BUFFER           0x8893
	#define GL_ARRAY_BUFFER_BINDING           0x8894
	#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
	#define GL_VERTEX_ARRAY_BUFFER_BINDING    0x8896
	#define GL_NORMAL_ARRAY_BUFFER_BINDING    0x8897
	#define GL_COLOR_ARRAY_BUFFER_BINDING     0x8898
	#define GL_INDEX_ARRAY_BUFFER_BINDING     0x8899
	#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
	#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
	#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
	#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
	#define GL_WEIGHT_ARRAY_BUFFER_BINDING    0x889E
	#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
	#define GL_READ_ONLY                      0x88B8
	#define GL_WRITE_ONLY                     0x88B9
	#define GL_READ_WRITE                     0x88BA
	#define GL_BUFFER_ACCESS                  0x88BB
	#define GL_BUFFER_MAPPED                  0x88BC
	#define GL_BUFFER_MAP_POINTER             0x88BD
	#define GL_STREAM_DRAW                    0x88E0
	#define GL_STREAM_READ                    0x88E1
	#define GL_STREAM_COPY                    0x88E2
	#define GL_STATIC_DRAW                    0x88E4
	#define GL_STATIC_READ                    0x88E5
	#define GL_STATIC_COPY                    0x88E6
	#define GL_DYNAMIC_DRAW                   0x88E8
	#define GL_DYNAMIC_READ                   0x88E9
	#define GL_DYNAMIC_COPY                   0x88EA
	#define GL_SAMPLES_PASSED                 0x8914
	#define GL_FOG_COORD_SRC                  GL_FOG_COORDINATE_SOURCE
	#define GL_FOG_COORD                      GL_FOG_COORDINATE
	#define GL_CURRENT_FOG_COORD              GL_CURRENT_FOG_COORDINATE
	#define GL_FOG_COORD_ARRAY_TYPE           GL_FOG_COORDINATE_ARRAY_TYPE
	#define GL_FOG_COORD_ARRAY_STRIDE         GL_FOG_COORDINATE_ARRAY_STRIDE
	#define GL_FOG_COORD_ARRAY_POINTER        GL_FOG_COORDINATE_ARRAY_POINTER
	#define GL_FOG_COORD_ARRAY                GL_FOG_COORDINATE_ARRAY
	#define GL_FOG_COORD_ARRAY_BUFFER_BINDING GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING
	#define GL_SRC0_RGB                       GL_SOURCE0_RGB
	#define GL_SRC1_RGB                       GL_SOURCE1_RGB
	#define GL_SRC2_RGB                       GL_SOURCE2_RGB
	#define GL_SRC0_ALPHA                     GL_SOURCE0_ALPHA
	#define GL_SRC1_ALPHA                     GL_SOURCE1_ALPHA
	#define GL_SRC2_ALPHA                     GL_SOURCE2_ALPHA
#endif

/*	GL VERSION 2.0	*/
#if defined(GL_VERSION_2_0)
	#define GL_BLEND_EQUATION_RGB             GL_BLEND_EQUATION
	#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
	#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
	#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
	#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
	#define GL_CURRENT_VERTEX_ATTRIB          0x8626
	#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
	#define GL_VERTEX_PROGRAM_TWO_SIDE        0x8643
	#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645
	#define GL_STENCIL_BACK_FUNC              0x8800
	#define GL_STENCIL_BACK_FAIL              0x8801
	#define GL_STENCIL_BACK_PASS_DEPTH_FAIL   0x8802
	#define GL_STENCIL_BACK_PASS_DEPTH_PASS   0x8803
	#define GL_MAX_DRAW_BUFFERS               0x8824
	#define GL_DRAW_BUFFER0                   0x8825
	#define GL_DRAW_BUFFER1                   0x8826
	#define GL_DRAW_BUFFER2                   0x8827
	#define GL_DRAW_BUFFER3                   0x8828
	#define GL_DRAW_BUFFER4                   0x8829
	#define GL_DRAW_BUFFER5                   0x882A
	#define GL_DRAW_BUFFER6                   0x882B
	#define GL_DRAW_BUFFER7                   0x882C
	#define GL_DRAW_BUFFER8                   0x882D
	#define GL_DRAW_BUFFER9                   0x882E
	#define GL_DRAW_BUFFER10                  0x882F
	#define GL_DRAW_BUFFER11                  0x8830
	#define GL_DRAW_BUFFER12                  0x8831
	#define GL_DRAW_BUFFER13                  0x8832
	#define GL_DRAW_BUFFER14                  0x8833
	#define GL_DRAW_BUFFER15                  0x8834
	#define GL_BLEND_EQUATION_ALPHA           0x883D
	#define GL_POINT_SPRITE                   0x8861
	#define GL_COORD_REPLACE                  0x8862
	#define GL_MAX_VERTEX_ATTRIBS             0x8869
	#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
	#define GL_MAX_TEXTURE_COORDS             0x8871
	#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872
	#define GL_FRAGMENT_SHADER                0x8B30
	#define GL_VERTEX_SHADER                  0x8B31
	#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
	#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
	#define GL_MAX_VARYING_FLOATS             0x8B4B
	#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
	#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
	#define GL_SHADER_TYPE                    0x8B4F
	#define GL_FLOAT_VEC2                     0x8B50
	#define GL_FLOAT_VEC3                     0x8B51
	#define GL_FLOAT_VEC4                     0x8B52
	#define GL_INT_VEC2                       0x8B53
	#define GL_INT_VEC3                       0x8B54
	#define GL_INT_VEC4                       0x8B55
	#define GL_BOOL                           0x8B56
	#define GL_BOOL_VEC2                      0x8B57
	#define GL_BOOL_VEC3                      0x8B58
	#define GL_BOOL_VEC4                      0x8B59
	#define GL_FLOAT_MAT2                     0x8B5A
	#define GL_FLOAT_MAT3                     0x8B5B
	#define GL_FLOAT_MAT4                     0x8B5C
	#define GL_SAMPLER_1D                     0x8B5D
	#define GL_SAMPLER_2D                     0x8B5E
	#define GL_SAMPLER_3D                     0x8B5F
	#define GL_SAMPLER_CUBE                   0x8B60
	#define GL_SAMPLER_1D_SHADOW              0x8B61
	#define GL_SAMPLER_2D_SHADOW              0x8B62
	#define GL_DELETE_STATUS                  0x8B80
	#define GL_COMPILE_STATUS                 0x8B81
	#define GL_LINK_STATUS                    0x8B82
	#define GL_VALIDATE_STATUS                0x8B83
	#define GL_INFO_LOG_LENGTH                0x8B84
	#define GL_ATTACHED_SHADERS               0x8B85
	#define GL_ACTIVE_UNIFORMS                0x8B86
	#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
	#define GL_SHADER_SOURCE_LENGTH           0x8B88
	#define GL_ACTIVE_ATTRIBUTES              0x8B89
	#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
	#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
	#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
	#define GL_CURRENT_PROGRAM                0x8B8D
	#define GL_POINT_SPRITE_COORD_ORIGIN      0x8CA0
	#define GL_LOWER_LEFT                     0x8CA1
	#define GL_UPPER_LEFT                     0x8CA2
	#define GL_STENCIL_BACK_REF               0x8CA3
	#define GL_STENCIL_BACK_VALUE_MASK        0x8CA4
	#define GL_STENCIL_BACK_WRITEMASK         0x8CA5
#endif

/*	GL VERSION 2.1	*/
#if defined(GL_VERSION_2_1)
	#define GL_PIXEL_PACK_BUFFER              0x88EB
	#define GL_PIXEL_UNPACK_BUFFER            0x88EC
	#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
	#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF
	#define GL_SRGB                           0x8C40
	#define GL_SRGB8                          0x8C41
	#define GL_SRGB_ALPHA                     0x8C42
	#define GL_SRGB8_ALPHA8                   0x8C43
	#define GL_SLUMINANCE_ALPHA               0x8C44
	#define GL_SLUMINANCE8_ALPHA8             0x8C45
	#define GL_SLUMINANCE                     0x8C46
	#define GL_SLUMINANCE8                    0x8C47
	#define GL_COMPRESSED_SRGB                0x8C48
	#define GL_COMPRESSED_SRGB_ALPHA          0x8C49
	#define GL_COMPRESSED_SLUMINANCE          0x8C4A
	#define GL_COMPRESSED_SLUMINANCE_ALPHA    0x8C4B
	#define GL_FLOAT_MAT2x3                   0x8B65
	#define GL_FLOAT_MAT2x4                   0x8B66
	#define GL_FLOAT_MAT3x2                   0x8B67
	#define GL_FLOAT_MAT3x4                   0x8B68
	#define GL_FLOAT_MAT4x2                   0x8B69
	#define GL_FLOAT_MAT4x3                   0x8B6A
	#define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
#endif


/* EXT_abgr */
#if defined(GL_EXT_abgr)
	#define GL_ABGR_EXT                         0x8000
#endif


/* EXT_packed_pixels */
#if defined(GL_EXT_packed_pixels)
	#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
	#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
	#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
	#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
	#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036
#endif


/* EXT_vertex_array */
#if defined(GL_EXT_vertex_array)
	#define GL_VERTEX_ARRAY_EXT                 0x8074
	#define GL_NORMAL_ARRAY_EXT                 0x8075
	#define GL_COLOR_ARRAY_EXT                  0x8076
	#define GL_INDEX_ARRAY_EXT                  0x8077
	#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
	#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
	#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
	#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
	#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
	#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
	#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
	#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
	#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
	#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
	#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
	#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
	#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
	#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
	#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
	#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
	#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
	#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
	#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
	#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
	#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
	#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
	#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
	#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
	#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
	#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
	#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
	#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_ARRAY_ELEMENT_EXT_PROC) (GLint i);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COLOR_POINTER_EXT_PROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_DRAW_ARRAYS_EXT_PROC) (GLenum mode, GLint first, GLsizei count);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_EDGE_FLAG_POINTER_EXT_PROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_POINTERV_EXT_PROC) (GLenum pname, GLvoid* *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_INDEX_POINTER_EXT_PROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_NORMAL_POINTER_EXT_PROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_TEX_COORD_POINTER_EXT_PROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_POINTER_EXT_PROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
#endif


/* EXT_bgra */
#if defined(GL_EXT_bgra)
	#define GL_BGR_EXT                          0x80E0
	#define GL_BGRA_EXT                         0x80E1
#endif


/* EXT_clip_volume_hint */
#if defined(GL_EXT_clip_volume_hint)
	#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0
#endif


/* EXT_point_parameters */
#if defined(GL_EXT_point_parameters)
	#define GL_POINT_SIZE_MIN_EXT               0x8126
	#define GL_POINT_SIZE_MAX_EXT               0x8127
	#define GL_POINT_FADE_THRESHOLD_SIZE_EXT    0x8128
	#define GL_DISTANCE_ATTENUATION_EXT         0x8129

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_F_EXT_PROC) (GLenum pname, GLfloat param);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_FV_EXT_PROC) (GLenum pname, const GLfloat *params);

	#ifndef DECLARE_GL_EXT_point_parameters
	#define DECLARE_GL_EXT_point_parameters(LINKAGE) \
		LINKAGE PFN_GL_POINT_PARAMETER_F_EXT_PROC		glPointParameterfEXT; \
		LINKAGE PFN_GL_POINT_PARAMETER_FV_EXT_PROC		glPointParameterfvEXT;
	#endif
#else 
	#define DECLARE_GL_EXT_point_parameters(LINKAGE)
#endif


/* ARB_point_parameters */
#if defined(GL_ARB_point_parameters)
	#define GL_POINT_SIZE_MIN_ARB               0x8126
	#define GL_POINT_SIZE_MAX_ARB               0x8127
	#define GL_POINT_FADE_THRESHOLD_SIZE_ARB    0x8128
	#define GL_DISTANCE_ATTENUATION_ARB         0x8129

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_F_ARB_PROC) (GLenum pname, GLfloat param);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_FV_ARB_PROC) (GLenum pname, const GLfloat *params);

	#ifndef DECLARE_GL_ABR_point_parameters
	#define DECLARE_GL_ARB_point_parameters(LINKAGE) \
		LINKAGE PFN_GL_POINT_PARAMETER_F_ARB_PROC		glPointParameterfARB; \
		LINKAGE PFN_GL_POINT_PARAMETER_FV_ARB_PROC		glPointParameterfvARB;
	#endif
#else 
	#define DECLARE_GL_ARB_point_parameters(LINKAGE)
#endif

/* GL_ARB_matrix_palette */
#if defined(GL_ARB_matrix_palette)
	#define GL_MATRIX_PALETTE_ARB             0x8840
	#define GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB 0x8841
	#define GL_MAX_PALETTE_MATRICES_ARB       0x8842
	#define GL_CURRENT_PALETTE_MATRIX_ARB     0x8843
	#define GL_MATRIX_INDEX_ARRAY_ARB         0x8844
	#define GL_CURRENT_MATRIX_INDEX_ARB       0x8845
	#define GL_MATRIX_INDEX_ARRAY_SIZE_ARB    0x8846
	#define GL_MATRIX_INDEX_ARRAY_TYPE_ARB    0x8847
	#define GL_MATRIX_INDEX_ARRAY_STRIDE_ARB  0x8848
	#define GL_MATRIX_INDEX_ARRAY_POINTER_ARB 0x8849

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CURRENT_PALETTE_MATRIX_ARB_PROC) (GLint index);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MATRIX_INDEX_UBV_ARB_PROC) (GLint size, const GLubyte *indices);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MATRIX_INDEX_USV_ARB_PROC) (GLint size, const GLushort *indices);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MATRIX_INDEX_UIV_ARBPROC) (GLint size, const GLuint *indices);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MATRIX_INDEX_POINTER_ARB_PROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

	#ifndef DECLARE_GL_ARB_matrix_palette
	#define DECLARE_GL_ARB_matrix_palette(LINKAGE) \
		LINKAGE PFN_GL_CURRENT_PALETTE_MATRIX_ARB_PROC glCurrentPaletteMatrixARB; \
		LINKAGE PFN_GL_MATRIX_INDEX_UBV_ARB_PROC glMatrixIndexubvARB; \
		LINKAGE PFN_GL_MATRIX_INDEX_USV_ARB_PROC glMatrixIndexusvARB; \
		LINKAGE PFN_GL_MATRIX_INDEX_UIV_ARBPROC glMatrixIndexuivARB; \
		LINKAGE PFN_GL_MATRIX_INDEX_POINTER_ARB_PROC glMatrixIndexPointerARB;
	#endif
#else
	#define DECLARE_GL_ARB_matrix_palette(LINKAGE)
#endif


/* NV_point_sprite */
#if defined(GL_NV_point_sprite)
	#define GL_POINT_SPRITE_NV					0x8861
	#define GL_COORD_REPLACE_NV					0x8862
	#define GL_POINT_SPRITE_R_MODE_NV			0x8863

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_I_NV_PROC)(GLenum pname, GLint param);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_POINT_PARAMETER_IV_NV_PROC)(GLenum pname, const GLint *params);

	#ifndef DECLARE_GL_NV_point_sprite
	#define DECLARE_GL_NV_point_sprite(LINKAGE) \
		LINKAGE PFN_GL_POINT_PARAMETER_I_NV_PROC		glPointParameteriNV; \
		LINKAGE PFN_GL_POINT_PARAMETER_IV_NV_PROC	glPointParameterivNV;
	#endif
#else
	#define DECLARE_GL_NV_point_sprite(LINKAGE)
#endif

/* ARB_point_sprite */
#if defined(GL_ARB_point_sprite)
	#define GL_POINT_SPRITE_ARB					0x8861
	#define GL_COORD_REPLACE_ARB				0x8862
#endif


/* EXT_compiled_vertex_array */
#if defined(GL_EXT_compiled_vertex_array)
	#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
	#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_LOCK_ARRAYS_EXT_PROC) (GLint first, GLsizei count);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_UNLOCK_ARRAYS_EXT_PROC) (void);

	#ifndef DECLARE_GL_EXT_compiled_vertex_array
	#define DECLARE_GL_EXT_compiled_vertex_array(LINKAGE) \
		LINKAGE PFN_GL_LOCK_ARRAYS_EXT_PROC				glLockArraysEXT; \
		LINKAGE PFN_GL_UNLOCK_ARRAYS_EXT_PROC			glUnlockArraysEXT;
	#endif
#else
	#define DECLARE_GL_EXT_compiled_vertex_array(LINKAGE)
#endif


/* EXT_cull_vertex */
#if defined(GL_EXT_cull_vertex)
	#define GL_CULL_VERTEX_EXT                  0x81AA
	#define GL_CULL_VERTEX_EYE_POSITION_EXT     0x81AB
	#define GL_CULL_VERTEX_OBJECT_POSITION_EXT  0x81AC

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CULL_PARAMETER_DV_EXT_PROC) (GLenum pname, GLdouble* params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CULL_PARAMETER_FV_EXT_PROC) (GLenum pname, GLfloat* params);
#endif


/* ARB_multitexture */
#if defined(GL_ARB_multitexture)
	#define GL_ACTIVE_TEXTURE_ARB               0x84E0
	#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
	#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
	#define GL_TEXTURE0_ARB                     0x84C0
	#define GL_TEXTURE1_ARB                     0x84C1
	#define GL_TEXTURE2_ARB                     0x84C2
	#define GL_TEXTURE3_ARB                     0x84C3
	#define GL_TEXTURE4_ARB                     0x84C4
	#define GL_TEXTURE5_ARB                     0x84C5
	#define GL_TEXTURE6_ARB                     0x84C6
	#define GL_TEXTURE7_ARB                     0x84C7
	#define GL_TEXTURE8_ARB                     0x84C8
	#define GL_TEXTURE9_ARB                     0x84C9
	#define GL_TEXTURE10_ARB                    0x84CA
	#define GL_TEXTURE11_ARB                    0x84CB
	#define GL_TEXTURE12_ARB                    0x84CC
	#define GL_TEXTURE13_ARB                    0x84CD
	#define GL_TEXTURE14_ARB                    0x84CE
	#define GL_TEXTURE15_ARB                    0x84CF
	#define GL_TEXTURE16_ARB                    0x84D0
	#define GL_TEXTURE17_ARB                    0x84D1
	#define GL_TEXTURE18_ARB                    0x84D2
	#define GL_TEXTURE19_ARB                    0x84D3
	#define GL_TEXTURE20_ARB                    0x84D4
	#define GL_TEXTURE21_ARB                    0x84D5
	#define GL_TEXTURE22_ARB                    0x84D6
	#define GL_TEXTURE23_ARB                    0x84D7
	#define GL_TEXTURE24_ARB                    0x84D8
	#define GL_TEXTURE25_ARB                    0x84D9
	#define GL_TEXTURE26_ARB                    0x84DA
	#define GL_TEXTURE27_ARB                    0x84DB
	#define GL_TEXTURE28_ARB                    0x84DC
	#define GL_TEXTURE29_ARB                    0x84DD
	#define GL_TEXTURE30_ARB                    0x84DE
	#define GL_TEXTURE31_ARB                    0x84DF

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1D_ARB_PROC) (GLenum target, GLdouble s);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1DV_ARB_PROC) (GLenum target, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1F_ARB_PROC) (GLenum target, GLfloat s);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1FV_ARB_PROC) (GLenum target, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1I_ARB_PROC) (GLenum target, GLint s);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1IV_ARB_PROC) (GLenum target, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1S_ARB_PROC) (GLenum target, GLshort s);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_1SV_ARB_PROC) (GLenum target, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2D_ARB_PROC) (GLenum target, GLdouble s, GLdouble t);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2DV_ARB_PROC) (GLenum target, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2F_ARB_PROC) (GLenum target, GLfloat s, GLfloat t);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2FV_ARB_PROC) (GLenum target, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2I_ARB_PROC) (GLenum target, GLint s, GLint t);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2IV_ARB_PROC) (GLenum target, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2S_ARB_PROC) (GLenum target, GLshort s, GLshort t);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_2SV_ARB_PROC) (GLenum target, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3D_ARB_PROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3DV_ARB_PROC) (GLenum target, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3F_ARB_PROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3FV_ARB_PROC) (GLenum target, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3I_ARB_PROC) (GLenum target, GLint s, GLint t, GLint r);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3IV_ARB_PROC) (GLenum target, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3S_ARB_PROC) (GLenum target, GLshort s, GLshort t, GLshort r);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_3SV_ARB_PROC) (GLenum target, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4D_ARB_PROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4DV_ARB_PROC) (GLenum target, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4F_ARB_PROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4FV_ARB_PROC) (GLenum target, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4I_ARB_PROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4IV_ARB_PROC) (GLenum target, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4S_ARB_PROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULTI_TEX_COORD_4SV_ARB_PROC) (GLenum target, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_ACTIVE_TEXTURE_ARB_PROC) (GLenum target);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CLIENT_ACTIVE_TEXTURE_ARB_PROC) (GLenum target);

	#ifndef DECLARE_GL_ARB_multitexture
	#define DECLARE_GL_ARB_multitexture(LINKAGE) \
		/*	these extensions are equivalent to a call to */\
		/*	glActiveTextureARB + TexCoord{1234}{dfis}[v] */\
		LINKAGE PFN_GL_MULTI_TEX_COORD_1D_ARB_PROC		glMultiTexCoord1dARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_1DV_ARB_PROC		glMultiTexCoord1dvARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_1F_ARB_PROC		glMultiTexCoord1fARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_1FV_ARB_PROC		glMultiTexCoord1fvARB; \
		/*	PFN_GL_MULTI_TEX_COORD_1I_ARB_PROC	glMultiTexCoord1iARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_1IV_ARB_PROC		glMultiTexCoord1ivARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_1S_ARB_PROC		glMultiTexCoord1svARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_1SV_ARB_PROC		glMultiTexCoord1sARB;*/ \
		LINKAGE PFN_GL_MULTI_TEX_COORD_2D_ARB_PROC		glMultiTexCoord2dARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_2DV_ARB_PROC		glMultiTexCoord2dvARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_2F_ARB_PROC		glMultiTexCoord2fARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_2FV_ARB_PROC		glMultiTexCoord2fvARB; \
		/*	PFN_GL_MULTI_TEX_COORD_2I_ARB_PROC	glMultiTexCoord2iARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_2IV_ARB_PROC		glMultiTexCoord2ivARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_2S_ARB_PROC		glMultiTexCoord2sARB; */ \
		/*	PFN_GL_MULTI_TEX_COORD_2SV_ARB_PROC		glMultiTexCoord2svARB; */ \
		LINKAGE PFN_GL_MULTI_TEX_COORD_3D_ARB_PROC		glMultiTexCoord3dARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_3DV_ARB_PROC		glMultiTexCoord3dvARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_3F_ARB_PROC		glMultiTexCoord3fARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_3FV_ARB_PROC		glMultiTexCoord3fvARB; \
		/*	PFN_GL_MULTI_TEX_COORD_3I_ARB_PROC	glMultiTexCoord3iARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_3IV_ARB_PROC		glMultiTexCoord3ivARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_3S_ARB_PROC		glMultiTexCoord3sARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_3SV_ARB_PROC		glMultiTexCoord3svARB;	*/ \
		LINKAGE PFN_GL_MULTI_TEX_COORD_4D_ARB_PROC		glMultiTexCoord4dARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_4DV_ARB_PROC		glMultiTexCoord4dvARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_4F_ARB_PROC		glMultiTexCoord4fARB; \
		LINKAGE PFN_GL_MULTI_TEX_COORD_4FV_ARB_PROC		glMultiTexCoord4fvARB; \
		/*	PFN_GL_MULTI_TEX_COORD_4I_ARB_PROC	glMultiTexCoord4iARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_4IV_ARB_PROC		glMultiTexCoord4ivARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_4S_ARB_PROC		glMultiTexCoord4sARB;	*/ \
		/*	PFN_GL_MULTI_TEX_COORD_4SV_ARB_PROC		glMultiTexCoord4svARB;	*/ \
		LINKAGE PFN_GL_ACTIVE_TEXTURE_ARB_PROC			glActiveTextureARB; \
		LINKAGE PFN_GL_CLIENT_ACTIVE_TEXTURE_ARB_PROC	glClientActiveTextureARB;
	#endif
#else
	#define DECLARE_GL_ARB_multitexture(LINKAGE)
#endif


/* EXT_stencil_wrap */
#if defined(GL_EXT_stencil_wrap)
	#define GL_INCR_WRAP_EXT                    0x8507
	#define GL_DECR_WRAP_EXT                    0x8508
#endif


/* EXT_stencil_two_side */
#if defined(GL_EXT_stencil_two_side)
	#define GL_STENCIL_TEST_TWO_SIDE_EXT        0x8910
	#define GL_ACTIVE_STENCIL_FACE_EXT          0x8911

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_ACTIVE_STENCIL_FACE_EXT_PROC) (GLenum face);

	#ifndef DECLARE_GL_EXT_stencil_two_side
	#define DECLARE_GL_EXT_stencil_two_side(LINKAGE) \
		LINKAGE PFN_GL_ACTIVE_STENCIL_FACE_EXT_PROC		glActiveStencilFaceEXT;
	#endif
#else
	#define DECLARE_GL_EXT_stencil_two_side(LINKAGE)
#endif


/* NV_texgen_reflection */
#if defined(GL_NV_texgen_reflection)
	#define GL_NORMAL_MAP_NV                    0x8511
	#define GL_REFLECTION_MAP_NV                0x8512
#endif

/* EXT_texture_env_add */
#if defined(GL_EXT_texture_env_add)
//	#define	GL_ADD								0x0104		//	already defined
#endif

/* GL_ARB_texture_mirrored_repeat */
#if defined(GL_ARB_texture_mirrored_repeat)
	#define GL_MIRRORED_REPEAT_ARB            0x8370
#endif


/* ARB_texture_env_combine */
//	texture combiners:
//	operators:
//	GL_REPLACE			Arg0
//	GL_MODULATE			Arg0 * Arg1
//	GL_ADD				Arg0 + Arg1
//	GL_ADD_SIGNED_ARB	Arg0 + Arg1 - 0.5
//	GL_SUBTRACT_ARB		Arg0 - Arg1
//	GL_INTERPOLATE_ARB	Arg0 * (Arg2) + Arg1 * (1-Arg2)
#if defined(GL_ARB_texture_env_combine)
//	TEXTURE_ENV_MODE
	#define GL_COMBINE_ARB                      0x8570

//	TEXTURE_ENV
	#define GL_COMBINE_RGB_ARB                  0x8571
	#define GL_COMBINE_ALPHA_ARB                0x8572
	#define GL_SOURCE0_RGB_ARB                  0x8580
	#define GL_SOURCE1_RGB_ARB                  0x8581
	#define GL_SOURCE2_RGB_ARB                  0x8582
	#define GL_SOURCE0_ALPHA_ARB                0x8588
	#define GL_SOURCE1_ALPHA_ARB                0x8589
	#define GL_SOURCE2_ALPHA_ARB                0x858A
	#define GL_OPERAND0_RGB_ARB                 0x8590
	#define GL_OPERAND1_RGB_ARB                 0x8591
	#define GL_OPERAND2_RGB_ARB                 0x8592
	#define GL_OPERAND0_ALPHA_ARB               0x8598
	#define GL_OPERAND1_ALPHA_ARB               0x8599
	#define GL_OPERAND2_ALPHA_ARB               0x859A
	#define GL_RGB_SCALE_ARB					0x8573
//	#define GL_ALPHA_SCALE						0x0D1C		//	already defined

//	GL_COMBINE_RGB_ARB & GL_COMBINE_ALPHA_ARB
//	#define	GL_REPLACE							0x1E01		//	already defined
//	#define	GL_MODULATE							0x2100		//	already defined
//	#define	GL_ADD								0x0104		//	already defined
	#define	GL_ADD_SIGNED_ARB					0x8574
	#define	GL_INTERPOLATE_ARB					0x8575
	#define	GL_SUBTRACT_ARB						0x84E7

//	SOURCE0_RGB_ARB & SOURCE1_RGB_ARB & SOURCE2_RGB_ARB & SOURCE0_ALPHA_ARB
//	& SOURCE1_ALPHA_ARB & SOURCE2_ALPHA_ARB
//	#define GL_TEXTURE							0x1702		//	already defined
	#define GL_CONSTANT_ARB                     0x8576
	#define GL_PRIMARY_COLOR_ARB                0x8577
	#define GL_PREVIOUS_ARB                     0x8578

//	OPERAND0_RGB_ARB & OPERAND1_RGB_ARB & OPERAND2_RGB_ARB	
//	#define GL_SRC_COLOR						0x0300		//	already defined
//	#define GL_ONE_MINUS_SRC_COLOR				0x0301		//	already defined
//	#define GL_SRC_ALPHA						0x0302		//	already defined
//	#define GL_ONE_MINUS_SRC_ALPHA				0x0303		//	already defined

//	OPERAND0_ALPHA_ARB & OPERAND1_ALPHA_ARB & OPERAND2_ALPHA_ARB
//	#define GL_SRC_ALPHA						0x0302		//	already defined
//	#define GL_ONE_MINUS_SRC_ALPHA				0x0303		//	already defined
#endif



/* ARB_texture_env_dot3 */
#if defined(GL_ARB_texture_env_dot3)
	#define GL_DOT3_RGB_ARB						0x86AE
	#define GL_DOT3_RGBA_ARB					0x86AF
#endif

/* NV_texture_env_combine4 */
#if defined(GL_NV_texture_env_combine4)
	#define GL_COMBINE4_NV                      0x8503
	#define GL_SOURCE3_RGB_NV                   0x8583
	#define GL_SOURCE3_ALPHA_NV                 0x858B
	#define GL_OPERAND3_RGB_NV                  0x8593
	#define GL_OPERAND3_ALPHA_NV                0x859B
#endif

/* ARB_texture_cube_map */
#if defined(GL_ARB_texture_cube_map)
	//	Texture generation
	#define GL_NORMAL_MAP_ARB					0x8511
	#define GL_REFLECTION_MAP_ARB				0x8512
	//	Enable/Disable/Bind
	#define GL_TEXTURE_CUBE_MAP_ARB				0x8513
	#define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
	//	TexImage
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB	0x8518
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x8519
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB	0x851A
	//	Proxies
	#define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
	#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C
#endif

/* GL_EXT_texture3D */
#if defined(GL_EXT_texture3D)
	#define GL_PACK_SKIP_IMAGES_EXT				0x806B
	#define GL_PACK_IMAGE_HEIGHT_EXT			0x806C
	#define GL_UNPACK_SKIP_IMAGES_EXT			0x806D
	#define GL_UNPACK_IMAGE_HEIGHT_EXT			0x806E
	#define GL_TEXTURE_3D_EXT					0x806F
	#define GL_PROXY_TEXTURE_3D_EXT				0x8070
	#define GL_TEXTURE_DEPTH_EXT				0x8071
	#define GL_TEXTURE_WRAP_R_EXT				0x8072
	#define GL_MAX_3D_TEXTURE_SIZE_EXT			0x8073

    typedef GLvoid (RAPTOR_APICALL * PFN_GL_TEX_IMAGE3D_EXT_PROC)(GLenum target,int level,GLenum internalformat,GLsizei width,GLsizei height,GLsizei depth,int border,GLenum format,GLenum type,const GLvoid* pixels);

    #ifndef DECLARE_GL_EXT_texture3D
	#define DECLARE_GL_EXT_texture3D(LINKAGE) \
		LINKAGE PFN_GL_TEX_IMAGE3D_EXT_PROC		glTexImage3DEXT;
	#endif
#else
	#define DECLARE_GL_EXT_texture3D(LINKAGE)
#endif



/* NV_register_combiners */
#if defined(GL_NV_register_combiners)

	#define GL_REGISTER_COMBINERS_NV			0x8522

	#define GL_COMBINER0_NV						0x8550
	#define GL_COMBINER1_NV						0x8551
	#define GL_COMBINER2_NV						0x8552
	#define GL_COMBINER3_NV						0x8553
	#define GL_COMBINER4_NV						0x8554
	#define GL_COMBINER5_NV						0x8555
	#define GL_COMBINER6_NV						0x8556
	#define GL_COMBINER7_NV						0x8557

	#define GL_VARIABLE_A_NV					0x8523
	#define GL_VARIABLE_B_NV					0x8524
	#define GL_VARIABLE_C_NV					0x8525
	#define GL_VARIABLE_D_NV					0x8526

	#define GL_VARIABLE_E_NV					0x8527
	#define GL_VARIABLE_F_NV					0x8528
	#define GL_VARIABLE_G_NV					0x8529

	//	register input RGBA
	//#define GL_ZERO (not new)					//	already defined
	#define GL_CONSTANT_COLOR0_NV				0x852A
	#define GL_CONSTANT_COLOR1_NV				0x852B
	//#define GL_FOG (not new)					//	already defined
	#define GL_PRIMARY_COLOR_NV					0x852C
	#define GL_SECONDARY_COLOR_NV				0x852D
	#define GL_SPARE0_NV						0x852E
	#define GL_SPARE1_NV						0x852F
	//#define GL_TEXTURE0_ARB					//	already defined
	//#define GL_TEXTURE1_ARB					//	already defined

	//	register input mapping
	#define GL_UNSIGNED_IDENTITY_NV				0x8536
	#define GL_UNSIGNED_INVERT_NV				0x8537
	#define GL_EXPAND_NORMAL_NV					0x8538
	#define GL_EXPAND_NEGATE_NV					0x8539
	#define GL_HALF_BIAS_NORMAL_NV				0x853A
	#define GL_HALF_BIAS_NEGATE_NV				0x853B
	#define GL_SIGNED_IDENTITY_NV				0x853C
	#define GL_SIGNED_NEGATE_NV					0x853D

	#define GL_E_TIMES_F_NV						0x8531
	#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV	0x8532

	//#define GL_NONE (not new)					// already defined
	#define GL_SCALE_BY_TWO_NV					0x853E
	#define GL_SCALE_BY_FOUR_NV					0x853F
	#define GL_SCALE_BY_ONE_HALF_NV				0x8540

	//#define GL_NONE (not new)					// already defined
	#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV		0x8541

	#define GL_DISCARD_NV						0x8530

	#define GL_COMBINER_INPUT_NV				0x8542
	#define GL_COMBINER_MAPPING_NV				0x8543
	#define GL_COMBINER_COMPONENT_USAGE_NV		0x8544

	#define GL_COMBINER_AB_DOT_PRODUCT_NV		0x8545
	#define GL_COMBINER_CD_DOT_PRODUCT_NV		0x8546
	#define GL_COMBINER_MUX_SUM_NV				0x8547
	#define GL_COMBINER_SCALE_NV				0x8548
	#define GL_COMBINER_BIAS_NV					0x8549
	#define GL_COMBINER_AB_OUTPUT_NV			0x854A
	#define GL_COMBINER_CD_OUTPUT_NV			0x854B
	#define GL_COMBINER_SUM_OUTPUT_NV			0x854C


	#define GL_NUM_GENERAL_COMBINERS_NV			0x854E
	#define GL_COLOR_SUM_CLAMP_NV				0x854F
	#define GL_MAX_GENERAL_COMBINERS_NV			0x854D

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_PARAMETER_FV_NV_PROC)(GLenum pname,const GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_PARAMETER_IV_NV_PROC)(GLenum pname,const GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_PARAMETER_F_NV_PROC)(GLenum pname,GLfloat param);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_PARAMETER_I_NV_PROC)(GLenum pname,GLint param);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_INPUT_NV_PROC)(GLenum stage,GLenum portion,GLenum variable,GLenum input,GLenum mapping,GLenum componentUsage);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMBINER_OUTPUT_NV_PROC)(GLenum stage,GLenum portion,GLenum abOutput,GLenum cdOutput,GLenum sumOutput,GLenum scale,GLenum bias,GLboolean abDotProduct,GLboolean cdDotProduct,GLboolean muxSum);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FINAL_COMBINER_INPUT_NV)(GLenum variable,GLenum input,GLenum mapping,GLenum componentUsage);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_COMBINER_INPUT_PARAMETER_FV_NV_PROC)(GLenum stage,GLenum portion,GLenum variable,GLenum pname,GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_COMBINER_INPUT_PARAMETER_IV_NV_PROC)(GLenum stage,GLenum portion,GLenum variable,GLenum pname,GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_FV_NV_PROC)(GLenum stage,GLenum portion,GLenum pname,GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_IV_NV_PROC)(GLenum stage,GLenum portion,GLenum pname,GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_FV_NV_PROC)(GLenum variable,GLenum pname,GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_IV_NV_PROC)(GLenum variable,GLenum pname,GLfloat *params);

	#ifndef DECLARE_GL_NV_register_combiners
	#define DECLARE_GL_NV_register_combiners(LINKAGE) \
		LINKAGE PFN_GL_COMBINER_PARAMETER_FV_NV_PROC					glCombinerParameterfvNV; \
		LINKAGE PFN_GL_COMBINER_PARAMETER_IV_NV_PROC					glCombinerParameterivNV; \
		LINKAGE PFN_GL_COMBINER_PARAMETER_F_NV_PROC					glCombinerParameterfNV; \
		LINKAGE PFN_GL_COMBINER_PARAMETER_I_NV_PROC					glCombinerParameteriNV; \
		LINKAGE PFN_GL_COMBINER_INPUT_NV_PROC						glCombinerInputNV; \
		LINKAGE PFN_GL_COMBINER_OUTPUT_NV_PROC						glCombinerOutputNV; \
		LINKAGE PFN_GL_FINAL_COMBINER_INPUT_NV						glFinalCombinerInputNV; \
		LINKAGE PFN_GL_GET_COMBINER_INPUT_PARAMETER_FV_NV_PROC		glGetCombinerInputParameterfvNV; \
		LINKAGE PFN_GL_GET_COMBINER_INPUT_PARAMETER_IV_NV_PROC		glGetCombinerInputParameterivNV; \
		LINKAGE PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_FV_NV_PROC		glGetCombinerOutputParameterfvNV; \
		LINKAGE PFN_GL_GET_COMBINER_OUTPUT_PARAMETER_IV_NV_PROC		glGetCombinerOutputParameterivNV; \
		LINKAGE PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_FV_NV_PROC	glGetFinalCombinerInputParameterfvNV; \
		LINKAGE PFN_GL_GET_FINAL_COMBINER_INPUT_PARAMETER_IV_NV_PROC	glGetFinalCombinerInputParameterivNV;
	#endif
#else
	#define DECLARE_GL_NV_register_combiners(LINKAGE)
#endif


/* EXT_secondary_color */
#if defined(GL_EXT_secondary_color)
	#define GL_COLOR_SUM_EXT					0x8458
	#define GL_CURRENT_SECONDARY_COLOR_EXT		0x8459
	#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT	0x845A
	#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT	0x845B
	#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT	0x845C
	#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT	0x845D
	#define GL_SECONDARY_COLOR_ARRAY_EXT		0x845E

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3S_EXT_PROC) (GLshort r, GLshort g, GLshort b);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3I_EXT_PROC) (GLint r, GLint g, GLint b);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3F_EXT_PROC) (GLfloat r, GLfloat g, GLfloat b);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3D_EXT_PROC) (GLdouble r, GLdouble g, GLdouble b);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3SV_EXT_PROC) (const GLshort *c);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3IV_EXT_PROC) (const GLint *c);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3FV_EXT_PROC) (const GLfloat *c);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_3DV_EXT_PROC) (const GLdouble *c);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SECONDARY_COLOR_POINTER_EXT_PROC) (GLint size, GLenum type, GLsizei stride,GLvoid *pointer);

	#ifndef DECLARE_GL_EXT_secondary_color
	#define DECLARE_GL_EXT_secondary_color(LINKAGE) \
		LINKAGE PFN_GL_SECONDARY_COLOR_3F_EXT_PROC		glSecondaryColor3fEXT; \
		LINKAGE PFN_GL_SECONDARY_COLOR_3D_EXT_PROC		glSecondaryColor3dEXT; \
		LINKAGE PFN_GL_SECONDARY_COLOR_3FV_EXT_PROC		glSecondaryColor3fvEXT; \
		LINKAGE PFN_GL_SECONDARY_COLOR_3DV_EXT_PROC		glSecondaryColor3dvEXT; \
		LINKAGE PFN_GL_SECONDARY_COLOR_POINTER_EXT_PROC	glSecondaryColorPointerEXT;
	#endif
#else
	#define DECLARE_GL_EXT_secondary_color(LINKAGE)
#endif


/* EXT_separate_specular_color */
#if defined(GL_EXT_separate_specular_color)
	#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT	0x81F8
	#define GL_SINGLE_COLOR_EXT					0x81F9
	#define GL_SEPARATE_SPECULAR_COLOR_EXT		0x81FA
#endif


/* ARB_transpose_matrix */
#if defined(GL_ARB_transpose_matrix)
	#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
	#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB	0x84E4
	#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5
	#define GL_TRANSPOSE_COLOR_MATRIX_ARB		0x84E6

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_LOAD_TRANSPOSE_MATRIX_F_ARB_PROC)(GLfloat m[16]);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_LOAD_TRANSPOSE_MATRIX_D_ARB_PROC)(GLdouble m[16]);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULT_TRANSPOSE_MATRIX_F_ARB_PROC)(GLfloat m[16]);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MULT_TRANSPOSE_MATRIX_D_ARB_PROC)(GLdouble m[16]);

	#ifndef DECLARE_GL_ARB_transpose_matrix
	#define DECLARE_GL_ARB_transpose_matrix(LINKAGE) \
		LINKAGE PFN_GL_LOAD_TRANSPOSE_MATRIX_F_ARB_PROC	glLoadTransposeMatrixfARB; \
		LINKAGE PFN_GL_LOAD_TRANSPOSE_MATRIX_D_ARB_PROC	glLoadTransposeMatrixdARB; \
		LINKAGE PFN_GL_MULT_TRANSPOSE_MATRIX_F_ARB_PROC	glMultTransposeMatrixfARB; \
		LINKAGE PFN_GL_MULT_TRANSPOSE_MATRIX_D_ARB_PROC	glMultTransposeMatrixdARB;
	#endif
#else
	#define DECLARE_GL_ARB_transpose_matrix(LINKAGE)
#endif

/* GL_ARB_vertex_blend */
#if defined(GL_ARB_vertex_blend)
	#define GL_MAX_VERTEX_UNITS_ARB           0x86A4
	#define GL_ACTIVE_VERTEX_UNITS_ARB        0x86A5
	#define GL_WEIGHT_SUM_UNITY_ARB           0x86A6
	#define GL_VERTEX_BLEND_ARB               0x86A7
	#define GL_CURRENT_WEIGHT_ARB             0x86A8
	#define GL_WEIGHT_ARRAY_TYPE_ARB          0x86A9
	#define GL_WEIGHT_ARRAY_STRIDE_ARB        0x86AA
	#define GL_WEIGHT_ARRAY_SIZE_ARB          0x86AB
	#define GL_WEIGHT_ARRAY_POINTER_ARB       0x86AC
	#define GL_WEIGHT_ARRAY_ARB               0x86AD
	#define GL_MODELVIEW0_ARB                 0x1700
	#define GL_MODELVIEW1_ARB                 0x850A
	#define GL_MODELVIEW2_ARB                 0x8722
	#define GL_MODELVIEW3_ARB                 0x8723
	#define GL_MODELVIEW4_ARB                 0x8724
	#define GL_MODELVIEW5_ARB                 0x8725
	#define GL_MODELVIEW6_ARB                 0x8726
	#define GL_MODELVIEW7_ARB                 0x8727
	#define GL_MODELVIEW8_ARB                 0x8728
	#define GL_MODELVIEW9_ARB                 0x8729
	#define GL_MODELVIEW10_ARB                0x872A
	#define GL_MODELVIEW11_ARB                0x872B
	#define GL_MODELVIEW12_ARB                0x872C
	#define GL_MODELVIEW13_ARB                0x872D
	#define GL_MODELVIEW14_ARB                0x872E
	#define GL_MODELVIEW15_ARB                0x872F
	#define GL_MODELVIEW16_ARB                0x8730
	#define GL_MODELVIEW17_ARB                0x8731
	#define GL_MODELVIEW18_ARB                0x8732
	#define GL_MODELVIEW19_ARB                0x8733
	#define GL_MODELVIEW20_ARB                0x8734
	#define GL_MODELVIEW21_ARB                0x8735
	#define GL_MODELVIEW22_ARB                0x8736
	#define GL_MODELVIEW23_ARB                0x8737
	#define GL_MODELVIEW24_ARB                0x8738
	#define GL_MODELVIEW25_ARB                0x8739
	#define GL_MODELVIEW26_ARB                0x873A
	#define GL_MODELVIEW27_ARB                0x873B
	#define GL_MODELVIEW28_ARB                0x873C
	#define GL_MODELVIEW29_ARB                0x873D
	#define GL_MODELVIEW30_ARB                0x873E
	#define GL_MODELVIEW31_ARB                0x873F

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_BV_ARB_PROC) (GLint size, const GLbyte *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_SV_ARB_PROC) (GLint size, const GLshort *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_IV_ARB_PROC) (GLint size, const GLint *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_FV_ARB_PROC) (GLint size, const GLfloat *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_DV_ARB_PROC) (GLint size, const GLdouble *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_UBV_ARB_PROC) (GLint size, const GLubyte *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_USV_ARB_PROC) (GLint size, const GLushort *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_UIV_ARB_PROC) (GLint size, const GLuint *weights);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WEIGHT_POINTER_ARB_PROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_BLEND_ARB_PROC) (GLint count);

	#ifndef DECLARE_GL_ARB_vertex_blend
	#define DECLARE_GL_ARB_vertex_blend(LINKAGE) \
		LINKAGE PFN_GL_WEIGHT_BV_ARB_PROC glWeightbvARB; \
		LINKAGE PFN_GL_WEIGHT_SV_ARB_PROC glWeightsvARB; \
		LINKAGE PFN_GL_WEIGHT_IV_ARB_PROC glWeightivARB; \
		LINKAGE PFN_GL_WEIGHT_FV_ARB_PROC glWeightfvARB; \
		LINKAGE PFN_GL_WEIGHT_DV_ARB_PROC glWeightdvARB; \
		LINKAGE PFN_GL_WEIGHT_UBV_ARB_PROC glWeightubvARB; \
		LINKAGE PFN_GL_WEIGHT_USV_ARB_PROC glWeightusvARB; \
		LINKAGE PFN_GL_WEIGHT_UIV_ARB_PROC glWeightuivARB; \
		LINKAGE PFN_GL_WEIGHT_POINTER_ARB_PROC glWeightPointerARB; \
		LINKAGE PFN_GL_VERTEX_BLEND_ARB_PROC glVertexBlendARB;
	#endif
#else
	#define DECLARE_GL_ARB_vertex_blend(LINKAGE)
#endif



/* EXT_vertex_weighting */
#if defined(GL_EXT_vertex_weighting)
	#define GL_VERTEX_WEIGHTING_EXT				0x8509
	#define GL_MODELVIEW0_EXT					0x1700 // alias to MODELVIEW enumerant
	#define GL_MODELVIEW1_EXT					0x850A
	#define GL_MODELVIEW0_MATRIX_EXT			0x0BA6 // alias to MODELVIEW_MATRIX
	#define GL_MODELVIEW1_MATRIX_EXT			0x8506
	#define GL_MODELVIEW0_STACK_DEPTH_EXT		0x0BA3 // alias to MODELVIEW_STACK_DEPTH
	#define GL_MODELVIEW1_STACK_DEPTH_EXT		0x8502
	#define GL_CURRENT_VERTEX_WEIGHT_EXT		0x850B
	#define GL_VERTEX_WEIGHT_ARRAY_EXT			0x850C
	#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT		0x850D
	#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT		0x850E
	#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT	0x850F
	#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT	0x8510

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_WEIGHT_F_EXT_PROC)(GLfloat weight);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_WEIGHT_FV_EXT_PROC)(GLfloat *weight);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_WEIGHT_POINTER_EXT_PROC)(GLint size, GLenum type,GLsizei stride, GLvoid *pointer);

	#ifndef DECLARE_GL_EXT_vertex_weighting
	#define DECLARE_GL_EXT_vertex_weighting(LINKAGE) \
		LINKAGE PFN_GL_VERTEX_WEIGHT_F_EXT_PROC			glVertexWeightfEXT; \
		LINKAGE PFN_GL_VERTEX_WEIGHT_FV_EXT_PROC			glVertexWeightfvEXT; \
		LINKAGE PFN_GL_VERTEX_WEIGHT_POINTER_EXT_PROC	glVertexWeightPointerEXT;
	#endif
#else
	#define DECLARE_GL_EXT_vertex_weighting(LINKAGE)
#endif

/* EXT_fog_coord */
#if defined(GL_EXT_fog_coord)
	#define GL_FOG_COORDINATE_SOURCE_EXT		0x8450
	#define GL_FOG_COORDINATE_EXT				0x8451
	#define GL_FRAGMENT_DEPTH_EXT				0x8452
	#define GL_CURRENT_FOG_COORDINATE_EXT		0x8453
	#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT	0x8454
	#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT	0x8455
	#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT	0x8456
	#define GL_FOG_COORDINATE_ARRAY_EXT			0x8457

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FOG_COORD_F_EXT_PROC)(GLfloat coord);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FOG_COORD_D_EXT_PROC)(GLdouble coord);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FOG_COORD_FV_EXT_PROC)(GLfloat coord);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FOG_COORD_DV_EXT_PROC)(GLfloat coord);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FOG_COORD_POINTER_EXT_PROC)(GLenum type, GLsizei stride, GLvoid *pointer);

	#ifndef DECLARE_GL_EXT_fog_coord
	#define DECLARE_GL_EXT_fog_coord(LINKAGE) \
		LINKAGE PFN_GL_FOG_COORD_F_EXT_PROC			glFogCoordfEXT; \
		LINKAGE PFN_GL_FOG_COORD_D_EXT_PROC			glFogCoorddEXT; \
		LINKAGE PFN_GL_FOG_COORD_FV_EXT_PROC			glFogCoordfvEXT; \
		LINKAGE PFN_GL_FOG_COORD_DV_EXT_PROC			glFogCoorddvEXT; \
		LINKAGE PFN_GL_FOG_COORD_POINTER_EXT_PROC	glFogCoordPointerEXT;
	#endif
#else
	#define DECLARE_GL_EXT_fog_coord(LINKAGE)
#endif

/* ARB_texture_compression */
#if defined(GL_ARB_texture_compression)
	#define GL_COMPRESSED_ALPHA_ARB				0x84E9
	#define GL_COMPRESSED_LUMINANCE_ARB			0x84EA
	#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB	0x84EB
	#define GL_COMPRESSED_INTENSITY_ARB			0x84EC
	#define GL_COMPRESSED_RGB_ARB				0x84ED
	#define GL_COMPRESSED_RGBA_ARB				0x84EE
	#define GL_TEXTURE_COMPRESSION_HINT_ARB		0x84EF
	#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
	#define GL_TEXTURE_COMPRESSED_ARB			0x86A1
	#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
	#define GL_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A3

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_IMAGE_3D_ARB_PROC)(GLenum target, GLint level,GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize,const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_IMAGE_2D_ARB_PROC)(GLenum target, GLint level,GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_IMAGE_1D_ARB_PROC)(GLenum target, GLint level,GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_SUB_IMAGE_3D_ARB_PROC)(GLenum target, GLint level,GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize,const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_SUB_IMAGE_2D_ARB_PROC)(GLenum target, GLint level,GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize,const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COMPRESSED_TEX_SUB_IMAGE_1D_ARB_PROC)(GLenum target, GLint level,GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_COMPRESSED_TEX_IMAGE_ARB_PROC)(GLenum target, GLint lod,GLvoid *img);

	#ifndef DECLARE_GL_ARB_texture_compression
	#define DECLARE_GL_ARB_texture_compression(LINKAGE) \
		LINKAGE PFN_GL_COMPRESSED_TEX_IMAGE_3D_ARB_PROC	glCompressedTexImage3DARB; \
		LINKAGE PFN_GL_COMPRESSED_TEX_IMAGE_2D_ARB_PROC	glCompressedTexImage2DARB; \
		LINKAGE PFN_GL_COMPRESSED_TEX_IMAGE_1D_ARB_PROC	glCompressedTexImage1DARB; \
		LINKAGE PFN_GL_COMPRESSED_TEX_SUB_IMAGE_3D_ARB_PROC		glCompressedTexSubImage3DARB; \
		LINKAGE PFN_GL_COMPRESSED_TEX_SUB_IMAGE_2D_ARB_PROC		glCompressedTexSubImage2DARB; \
		LINKAGE PFN_GL_COMPRESSED_TEX_SUB_IMAGE_1D_ARB_PROC		glCompressedTexSubImage1DARB; \
		LINKAGE PFN_GL_GET_COMPRESSED_TEX_IMAGE_ARB_PROC	glGetCompressedTexImageARB;
	#endif
#else
	#define DECLARE_GL_ARB_texture_compression(LINKAGE)
#endif


/* EXT_texture_compression_s3tc */
/* GL_NV_texture_compression_vtc */
/* GL_EXT_texture_compression_dxt1 */
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc) || defined(GL_EXT_texture_compression_dxt1))
	#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT		0x83F0
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT	0x83F1
#endif
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc))
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	0x83F2
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	0x83F3
#endif

/* ARB_texture_compression_rgtc */
#if defined(GL_ARB_texture_compression_rgtc)
	#define GL_COMPRESSED_RED_RGTC1_ARB                   0x8DBB
	#define GL_COMPRESSED_SIGNED_RED_RGTC1_ARB            0x8DBC
	#define GL_COMPRESSED_RG_RGTC2_ARB                    0x8DBD
	#define GL_COMPRESSED_SIGNED_RG_RGTC2_ARB             0x8DBE
#endif

/* ARB_texture_compression_bptc */
#if defined(GL_ARB_texture_compression_bptc)
	#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB                 0x8E8C
	#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB           0x8E8D
	#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB           0x8E8E
	#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB         0x8E8F
#endif

/* 3DFX_texture_compression_FXT1 */
#if defined(GL_3DFX_texture_compression_FXT1)
	#define GL_COMPRESSED_RGB_FXT1_3DFX			0x86B0
	#define GL_COMPRESSED_RGBA_FXT1_3DFX		0x86B1
#endif

/* EXT_texture_compression_latc */
#if defined(GL_EXT_texture_compression_latc)
	#define GL_COMPRESSED_LUMINANCE_LATC1_EXT                 0x8C70
	#define GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT          0x8C71
	#define GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT           0x8C72
	#define GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT    0x8C73
#endif

/* GL_ARB_compressed_texture_pixel_storage */
#if defined(GL_ARB_compressed_texture_pixel_storage)
	#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH                    0x9127
	#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT                   0x9128
	#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH                    0x9129
	#define GL_UNPACK_COMPRESSED_BLOCK_SIZE                     0x912A
	#define GL_PACK_COMPRESSED_BLOCK_WIDTH                      0x912B
	#define GL_PACK_COMPRESSED_BLOCK_HEIGHT                     0x912C
	#define GL_PACK_COMPRESSED_BLOCK_DEPTH                      0x912D
	#define GL_PACK_COMPRESSED_BLOCK_SIZE                       0x912E
#endif

/*GL_OES_compressed_paletted_texture*/
#if defined(GL_OES_compressed_paletted_texture)
	#define GL_PALETTE4_RGB8_OES         0x8B90
	#define GL_PALETTE4_RGBA8_OES        0x8B91
	#define GL_PALETTE4_R5_G6_B5_OES     0x8B92
	#define GL_PALETTE4_RGBA4_OES        0x8B93
	#define GL_PALETTE4_RGB5_A1_OES      0x8B94
	#define GL_PALETTE8_RGB8_OES         0x8B95
	#define GL_PALETTE8_RGBA8_OES        0x8B96
	#define GL_PALETTE8_R5_G6_B5_OES     0x8B97
	#define GL_PALETTE8_RGBA4_OES        0x8B98
	#define GL_PALETTE8_RGB5_A1_OES      0x8B99
#endif


/* GL_ARB_texture_rg */
#if defined(GL_ARB_texture_rg)
	#define GL_R8                      0x8229
	#define GL_R16                     0x822A
	#define GL_RG8                     0x822B
	#define GL_RG16                    0x822C
	#define GL_R16F                    0x822D
	#define GL_R32F                    0x822E
	#define GL_RG16F                   0x822F
	#define GL_RG32F                   0x8230
	#define GL_R8I                     0x8231
	#define GL_R8UI                    0x8232
	#define GL_R16I                    0x8233
	#define GL_R16UI                   0x8234
	#define GL_R32I                    0x8235
	#define GL_R32UI                   0x8236
	#define GL_RG8I                    0x8237
	#define GL_RG8UI                   0x8238
	#define GL_RG16I                   0x8239
	#define GL_RG16UI                  0x823A
	#define GL_RG32I                   0x823B
	#define GL_RG32UI                  0x823C
//	#define GL_RED                     0x1903	Already defined in core GL
	#define GL_RG                      0x8227
	#define GL_RG_INTEGER              0x8228
	#define GL_COMPRESSED_RED_ARB      0x8225
	#define GL_COMPRESSED_RG_ARB       0x8226
#endif


/* WIN_swap_hint */
#if defined(GL_WIN_swap_hint)
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_ADD_SWAP_HINT_RECT_WIN_PROC) (GLint x, GLint y, GLsizei width, GLsizei height);
#endif


/* ARB_imaging */
#if defined(GL_ARB_imaging)
	#define GL_CONSTANT_COLOR_ARB				0x8001
	#define GL_ONE_MINUS_CONSTANT_COLOR_ARB		0x8002
	#define GL_CONSTANT_ALPHA_ARB				0x8003
	#define GL_ONE_MINUS_CONSTANT_ALPHA_ARB		0x8004
	#define GL_BLEND_COLOR_ARB					0x8005

	#define GL_FUNC_ADD_ARB						0x8006
	#define GL_MIN_ARB							0x8007
	#define GL_MAX_ARB							0x8008
	#define GL_BLEND_EQUATION_ARB				0x8009

	#define GL_FUNC_SUBTRACT_EXT				0x800A
	#define GL_FUNC_REVERSE_SUBTRACT_EXT		0x800B

	#define GL_CONSTANT_COLOR					0x8001
	#define GL_ONE_MINUS_CONSTANT_COLOR			0x8002
	#define GL_CONSTANT_ALPHA					0x8003
	#define GL_ONE_MINUS_CONSTANT_ALPHA			0x8004
	#define GL_BLEND_COLOR						0x8005
	#define GL_FUNC_ADD							0x8006
	#define GL_MIN								0x8007
	#define GL_MAX								0x8008
	#define GL_BLEND_EQUATION					0x8009
	#define GL_FUNC_SUBTRACT					0x800A
	#define GL_FUNC_REVERSE_SUBTRACT			0x800B
	#define GL_CONVOLUTION_1D					0x8010
	#define GL_CONVOLUTION_2D					0x8011
	#define GL_SEPARABLE_2D						0x8012
	#define GL_CONVOLUTION_BORDER_MODE			0x8013
	#define GL_CONVOLUTION_FILTER_SCALE			0x8014
	#define GL_CONVOLUTION_FILTER_BIAS			0x8015
	#define GL_REDUCE							0x8016
	#define GL_CONVOLUTION_FORMAT				0x8017
	#define GL_CONVOLUTION_WIDTH				0x8018
	#define GL_CONVOLUTION_HEIGHT				0x8019
	#define GL_MAX_CONVOLUTION_WIDTH			0x801A
	#define GL_MAX_CONVOLUTION_HEIGHT			0x801B
	#define GL_POST_CONVOLUTION_RED_SCALE		0x801C
	#define GL_POST_CONVOLUTION_GREEN_SCALE		0x801D
	#define GL_POST_CONVOLUTION_BLUE_SCALE		0x801E
	#define GL_POST_CONVOLUTION_ALPHA_SCALE		0x801F
	#define GL_POST_CONVOLUTION_RED_BIAS		0x8020
	#define GL_POST_CONVOLUTION_GREEN_BIAS		0x8021
	#define GL_POST_CONVOLUTION_BLUE_BIAS		0x8022
	#define GL_POST_CONVOLUTION_ALPHA_BIAS		0x8023
	#define GL_HISTOGRAM						0x8024
	#define GL_PROXY_HISTOGRAM					0x8025
	#define GL_HISTOGRAM_WIDTH					0x8026
	#define GL_HISTOGRAM_FORMAT					0x8027
	#define GL_HISTOGRAM_RED_SIZE				0x8028
	#define GL_HISTOGRAM_GREEN_SIZE				0x8029
	#define GL_HISTOGRAM_BLUE_SIZE				0x802A
	#define GL_HISTOGRAM_ALPHA_SIZE				0x802B
	#define GL_HISTOGRAM_LUMINANCE_SIZE			0x802C
	#define GL_HISTOGRAM_SINK					0x802D
	#define GL_MINMAX							0x802E
	#define GL_MINMAX_FORMAT					0x802F
	#define GL_MINMAX_SINK						0x8030
	#define GL_TABLE_TOO_LARGE					0x8031
	#define GL_COLOR_MATRIX						0x80B1
	#define GL_COLOR_MATRIX_STACK_DEPTH			0x80B2
	#define GL_MAX_COLOR_MATRIX_STACK_DEPTH		0x80B3
	#define GL_POST_COLOR_MATRIX_RED_SCALE		0x80B4
	#define GL_POST_COLOR_MATRIX_GREEN_SCALE	0x80B5
	#define GL_POST_COLOR_MATRIX_BLUE_SCALE		0x80B6
	#define GL_POST_COLOR_MATRIX_ALPHA_SCALE	0x80B7
	#define GL_POST_COLOR_MATRIX_RED_BIAS		0x80B8
	#define GL_POST_COLOR_MATRIX_GREEN_BIAS		0x80B9
	#define GL_POST_COLOR_MATRIX_BLUE_BIAS		0x80BA
	#define GL_POST_COLOR_MATRIX_ALPHA_BIAS		0x80BB
	#define GL_COLOR_TABLE						0x80D0
	#define GL_POST_CONVOLUTION_COLOR_TABLE		0x80D1
	#define GL_POST_COLOR_MATRIX_COLOR_TABLE	0x80D2
	#define GL_PROXY_COLOR_TABLE				0x80D3
	#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
	#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
	#define GL_COLOR_TABLE_SCALE				0x80D6
	#define GL_COLOR_TABLE_BIAS					0x80D7
	#define GL_COLOR_TABLE_FORMAT				0x80D8
	#define GL_COLOR_TABLE_WIDTH				0x80D9
	#define GL_COLOR_TABLE_RED_SIZE				0x80DA
	#define GL_COLOR_TABLE_GREEN_SIZE			0x80DB
	#define GL_COLOR_TABLE_BLUE_SIZE			0x80DC
	#define GL_COLOR_TABLE_ALPHA_SIZE			0x80DD
	#define GL_COLOR_TABLE_LUMINANCE_SIZE		0x80DE
	#define GL_COLOR_TABLE_INTENSITY_SIZE		0x80DF
	#define GL_CONSTANT_BORDER					0x8151
	#define GL_REPLICATE_BORDER					0x8153
	#define GL_CONVOLUTION_BORDER_COLOR			0x8154

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLEND_COLOR_ARB_PROC)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLEND_EQUATION_ARB_PROC)( GLenum mode );

	#ifndef DECLARE_GL_ARB_imaging
	#define DECLARE_GL_ARB_imaging(LINKAGE) \
		LINKAGE PFN_GL_BLEND_COLOR_ARB_PROC		glBlendColorARB;\
		LINKAGE PFN_GL_BLEND_EQUATION_ARB_PROC	glBlendEquationARB;
	#endif
#else
	#define DECLARE_GL_ARB_imaging(LINKAGE)
#endif



/* ARB_multisample */
#if defined(GL_ARB_multisample)
	#define WGL_SAMPLE_BUFFERS_ARB				0x2041
	#define WGL_SAMPLES_ARB						0x2042

	#define GL_MULTISAMPLE_ARB					0x809D
	#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB		0x809E
	#define GL_SAMPLE_ALPHA_TO_ONE_ARB			0x809F
	#define GL_SAMPLE_COVERAGE_ARB				0x80A0

	#define GL_MULTISAMPLE_BIT_ARB				0x20000000

	#define GL_SAMPLE_BUFFERS_ARB				0x80A8
	#define GL_SAMPLES_ARB						0x80A9
	#define GL_SAMPLE_COVERAGE_VALUE_ARB		0x80AA
	#define GL_SAMPLE_COVERAGE_INVERT_ARB		0x80AB

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SAMPLE_COVERAGE_ARB_PROC)(GLclampf value,GLboolean invert);

	#ifndef DECLARE_GL_ARB_multisample
	#define DECLARE_GL_ARB_multisample(LINKAGE) \
		LINKAGE PFN_GL_SAMPLE_COVERAGE_ARB_PROC	glSampleCoverageARB;
	#endif
#else
	#define DECLARE_GL_ARB_multisample(LINKAGE)
#endif

/* GL_EXT_texture_filter_anisotropic */
#if defined(GL_EXT_texture_filter_anisotropic)
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
#endif


/* GL_NV_texture_shader */
#if defined(GL_NV_texture_shader)
	#define GL_TEXTURE_SHADER_NV				0x86DE
	#define GL_CONST_EYE_NV						0x86E5
	#define GL_SHADER_CONSISTENT_NV				0x86DD

	#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV \
												0x86D9
	//
	//	Dot product Mapping :
	//
	//#define GL_UNSIGNED_IDENTITY_NV (not new)	// already defined
	//#define GL_EXPAND_NORMAL_NV (not new)		// already defined											0x86D9

	#define GL_CULL_MODES_NV					0x86E0
	//
	//	cull modes :
	//
	//#define GL_LESS (not new)					// already defined
	//#define GL_GEQUAL (not new)				// already defined

	#define GL_SHADER_OPERATION_NV				0x86DF
	//
	//	shader operations :
	//
	//#define GL_NONE (not new)					// already defined
	//#define GL_TEXTURE_1D (not new)			// already defined
	//#define GL_TEXTURE_2D (not new)			// already defined
	//#define GL_TEXTURE_3D (not new)			// already defined
	#define GL_TEXTURE_RECTANGLE_NV				0x84F5
	//#define GL_TEXTURE_CUBE_MAP_ARB (not new)	// already defined
	#define GL_PASS_THROUGH_NV					0x86E6
	#define GL_CULL_FRAGMENT_NV					0x86E7
	#define GL_OFFSET_TEXTURE_2D_NV				0x86E8
	//#define GL_OFFSET_TEXTURE_2D_SCALE_NV		// see below
	#define GL_OFFSET_TEXTURE_RECTANGLE_NV		0x864C
	#define GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV \
												0x864D
	#define GL_DEPENDENT_AR_TEXTURE_2D_NV		0x86E9
	#define GL_DEPENDENT_GB_TEXTURE_2D_NV		0x86EA
	#define GL_DOT_PRODUCT_NV					0x86EC
	#define GL_DOT_PRODUCT_DEPTH_REPLACE_NV		0x86ED
	#define GL_DOT_PRODUCT_TEXTURE_2D_NV		0x86EE
	#define GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV 0x864E
	#define GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV	0x86F0
	#define GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV	0x86F1
	#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV	0x86F2
	#define GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV \
												0x86F3

	
	#define GL_OFFSET_TEXTURE_MATRIX_NV			0x86E1
	#define GL_OFFSET_TEXTURE_SCALE_NV			0x86E2
	#define GL_OFFSET_TEXTURE_BIAS_NV			0x86E3
	#define GL_OFFSET_TEXTURE_2D_MATRIX_NV		GL_OFFSET_TEXTURE_MATRIX_NV
	#define GL_OFFSET_TEXTURE_2D_SCALE_NV		GL_OFFSET_TEXTURE_SCALE_NV
	#define GL_OFFSET_TEXTURE_2D_BIAS_NV		GL_OFFSET_TEXTURE_BIAS_NV

	#define GL_PREVIOUS_TEXTURE_INPUT_NV		0x86E4
	//
	//	Previous input :
	//
	//#define GL_TEXTURE0_ARB (not new)			// already defined
	//#define GL_TEXTURE1_ARB (not new)			// already defined
	//#define GL_TEXTURE2_ARB (not new)			// already defined
	//#define GL_TEXTURE3_ARB (not new)			// already defined
	//#define GL_TEXTURE4_ARB (not new)			// already defined
	//#define GL_TEXTURE5_ARB (not new)			// already defined
	//#define GL_TEXTURE6_ARB (not new)			// already defined
	//#define GL_TEXTURE7_ARB (not new)			// already defined
	
	//
	//	New Tex image formats :
	//
	#define GL_HILO_NV							0x86F4
	#define GL_DSDT_NV							0x86F5
	#define GL_DSDT_MAG_NV						0x86F6
	#define GL_DSDT_MAG_VIB_NV					0x86F7

	//	
	//	New Tex image data type :
	//
	#define GL_UNSIGNED_INT_S8_S8_8_8_NV		0x86DA
	#define GL_UNSIGNED_INT_8_8_S8_S8_REV_NV	0x86DB

	//
	//	New Tex image internal format :
	//
	#define GL_SIGNED_RGBA_NV					0x86FB
	#define GL_SIGNED_RGBA8_NV					0x86FC
	#define GL_SIGNED_RGB_NV					0x86FE
	#define GL_SIGNED_RGB8_NV					0x86FF
	#define GL_SIGNED_LUMINANCE_NV				0x8701
	#define GL_SIGNED_LUMINANCE8_NV				0x8702
	#define GL_SIGNED_LUMINANCE_ALPHA_NV		0x8703
	#define GL_SIGNED_LUMINANCE8_ALPHA8_NV		0x8704
	#define GL_SIGNED_ALPHA_NV					0x8705
	#define GL_SIGNED_ALPHA8_NV					0x8706
	#define GL_SIGNED_INTENSITY_NV				0x8707
	#define GL_SIGNED_INTENSITY8_NV				0x8708
	#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV		0x870C
	#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV	0x870D
	//#define GL_HILO_NV
	#define GL_HILO16_NV						0x86F8
	#define GL_SIGNED_HILO_NV					0x86F9
	#define GL_SIGNED_HILO16_NV					0x86FA
	//#define GL_DSDT_NV
	#define GL_DSDT8_NV							0x8709
	//#define GL_DSDT_MAG_NV
	#define GL_DSDT8_MAG8_NV					0x870A
	#define GL_DSDT_MAG_INTENSITY_NV			0x86DC
	#define GL_DSDT8_MAG8_INTENSITY8_NV			0x870B

	//
	//	Get parameters :
	//
	#define GL_HI_SCALE_NV						0x870E
	#define GL_LO_SCALE_NV						0x870F
	#define GL_DS_SCALE_NV						0x8710
	#define GL_DT_SCALE_NV						0x8711
	#define GL_MAGNITUDE_SCALE_NV				0x8712
	#define GL_VIBRANCE_SCALE_NV				0x8713
	#define GL_HI_BIAS_NV						0x8714
	#define GL_LO_BIAS_NV						0x8715
	#define GL_DS_BIAS_NV						0x8716
	#define GL_DT_BIAS_NV						0x8717
	#define GL_MAGNITUDE_BIAS_NV				0x8718
	#define GL_VIBRANCE_BIAS_NV					0x8719

	//
	// Get Tex parameters :
	//
	#define GL_TEXTURE_BORDER_VALUES_NV			0x871A

	//
	//	Get Tex Level parameters :
	//
	#define GL_TEXTURE_HI_SIZE_NV				0x871B
	#define GL_TEXTURE_LO_SIZE_NV				0x871C
	#define GL_TEXTURE_DS_SIZE_NV				0x871D
	#define GL_TEXTURE_DT_SIZE_NV				0x871E
	#define GL_TEXTURE_MAG_SIZE_NV				0x871F
#endif



/* GL_NV_vertex_array_range	*/
#if defined(GL_NV_vertex_array_range)
	#define GL_VERTEX_ARRAY_RANGE_NV			0x851D
	#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV		0x851E
	#define GL_VERTEX_ARRAY_RANGE_VALID_NV		0x851F
	#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV \
												0x8520
	#define GL_VERTEX_ARRAY_RANGE_POINTER_NV	0x8521
	#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV \
												0x8533

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_VERTEX_ARRAY_RANGE_NV_PROC)(GLsizei length, GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FLUSH_VERTEX_ARRAY_RANGE_NV_PROC)(void);

	typedef GLvoid * (RAPTOR_APICALL * PFN_WGL_ALLOCATE_MEMORY_NV_PROC)(GLsizei size,GLfloat readFrequency,GLfloat writeFrequency,GLfloat priority);
	typedef GLvoid (RAPTOR_APICALL * PFN_WGL_FREE_MEMORY_NV_PROC)(GLvoid *pointer);

	#ifndef DECLARE_GL_NV_vertex_array_range
	#define DECLARE_GL_NV_vertex_array_range(LINKAGE) \
		LINKAGE PFN_GL_VERTEX_ARRAY_RANGE_NV_PROC		glVertexArrayRangeNV;\
		LINKAGE PFN_GL_FLUSH_VERTEX_ARRAY_RANGE_NV_PROC	glFlushVertexArrayRangeNV;\
		LINKAGE PFN_WGL_ALLOCATE_MEMORY_NV_PROC			wglAllocateMemoryNV;\
		LINKAGE PFN_WGL_FREE_MEMORY_NV_PROC				wglFreeMemoryNV;
	#endif
#else
	#define DECLARE_GL_NV_vertex_array_range(LINKAGE)
#endif


/* GL_ARB_vertex_buffer_object */
#if defined(GL_ARB_vertex_buffer_object)
	#define GL_ARRAY_BUFFER_ARB					0x8892
	#define GL_ELEMENT_ARRAY_BUFFER_ARB			0x8893

	#define GL_ARRAY_BUFFER_BINDING_ARB			0x8894
	#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB	0x8895
	#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB	0x8896
	#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB	0x8897
	#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB	0x8898
	#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB	0x8899
	#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB \
												0x889A
	#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB \
												0x889B
	#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB \
												0x889C
	#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB \
												0x889D
	#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB	0x889E
	#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB \
												0x889F

	#define GL_STREAM_DRAW_ARB					0x88E0
	#define GL_STREAM_READ_ARB					0x88E1
	#define GL_STREAM_COPY_ARB					0x88E2
	#define GL_STATIC_DRAW_ARB					0x88E4
	#define GL_STATIC_READ_ARB					0x88E5
	#define GL_STATIC_COPY_ARB					0x88E6
	#define GL_DYNAMIC_DRAW_ARB					0x88E8
	#define GL_DYNAMIC_READ_ARB					0x88E9
	#define GL_DYNAMIC_COPY_ARB					0x88EA

	#define GL_READ_ONLY_ARB					0x88B8
	#define GL_WRITE_ONLY_ARB					0x88B9
	#define GL_READ_WRITE_ARB					0x88BA

	#define GL_BUFFER_SIZE_ARB					0x8764
	#define GL_BUFFER_USAGE_ARB					0x8765
	#define GL_BUFFER_ACCESS_ARB				0x88BB
	#define GL_BUFFER_MAPPED_ARB				0x88BC

	#define GL_BUFFER_MAP_POINTER_ARB			0x88BD

	typedef long			GLintptrARB;		//	might be defined to __int64 if necessary
	typedef unsigned long	GLsizeiptrARB;

	typedef GLvoid (RAPTOR_APICALL * PFN_BIND_BUFFER_ARB_PROC)(GLenum target, GLuint buffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_DELETE_BUFFERS_ARB_PROC)(GLsizei n, const GLuint *buffers);
	typedef GLvoid (RAPTOR_APICALL * PFN_GEN_BUFFERS_ARB_PROC)(GLsizei n, GLuint *buffers);
	typedef GLboolean (RAPTOR_APICALL * PFN_IS_BUFFERS_ARB_PROC)(GLuint buffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_BUFFER_DATA_ARB_PROC)(GLenum target, GLsizeiptrARB size, const GLvoid *data,GLenum usage);
	typedef GLvoid (RAPTOR_APICALL * PFN_BUFFER_SUB_DATA_ARB_PROC)(GLenum target, GLintptrARB offset, GLsizeiptrARB size,const GLvoid *data);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_BUFFER_SUB_DATA_ARB_PROC)(GLenum target, GLintptrARB offset,GLsizeiptrARB size, GLvoid *data);
	typedef GLvoid * (RAPTOR_APICALL * PFN_MAP_BUFFER_ARB_PROC)(GLenum target, GLenum access);
	typedef GLboolean (RAPTOR_APICALL * PFN_UNMAP_BUFFER_ARB_PROC)(GLenum target);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_BUFFER_PARAMETER_IV_ARB_PROC)(GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_BUFFER_POINTER_V_ARB_PROC)(GLenum target, GLenum pname, GLvoid **params);

	#ifndef DECLARE_GL_ARB_vertex_buffer_object
	#define DECLARE_GL_ARB_vertex_buffer_object(LINKAGE) \
		LINKAGE PFN_BIND_BUFFER_ARB_PROC		glBindBufferARB;\
		LINKAGE PFN_DELETE_BUFFERS_ARB_PROC	glDeleteBuffersARB;\
		LINKAGE PFN_GEN_BUFFERS_ARB_PROC		glGenBuffersARB;\
		LINKAGE PFN_IS_BUFFERS_ARB_PROC		glIsBufferARB; \
		LINKAGE PFN_BUFFER_DATA_ARB_PROC		glBufferDataARB; \
		LINKAGE PFN_BUFFER_SUB_DATA_ARB_PROC glBufferSubDataARB; \
		LINKAGE PFN_GET_BUFFER_SUB_DATA_ARB_PROC glGetBufferSubDataARB; \
		LINKAGE PFN_MAP_BUFFER_ARB_PROC		glMapBufferARB; \
		LINKAGE PFN_UNMAP_BUFFER_ARB_PROC	glUnmapBufferARB; \
		LINKAGE PFN_GET_BUFFER_PARAMETER_IV_ARB_PROC glGetBufferParameterivARB; \
		LINKAGE PFN_GET_BUFFER_POINTER_V_ARB_PROC glGetBufferPointervARB;
	#endif
#else
	#define DECLARE_GL_ARB_vertex_buffer_object(LINKAGE)
#endif


/* GL_ARB_vertex_program			*/
/*	ARB model of "vertex shaders"	*/
#if defined(GL_ARB_vertex_program)
	#define GL_VERTEX_PROGRAM_ARB				0x8620

	#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB	0x8642
	#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB		0x8643
	#define GL_COLOR_SUM_ARB					0x8458

	#define GL_PROGRAM_FORMAT_ASCII_ARB			0x8875

	#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB	0x8622
	#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB		0x8623
	#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB	0x8624
	#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB		0x8625
	#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB 0x886A
	#define GL_CURRENT_VERTEX_ATTRIB_ARB		0x8626

	#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB	0x8645

	#define GL_PROGRAM_LENGTH_ARB				0x8627
	#define GL_PROGRAM_FORMAT_ARB				0x8876
	#define GL_PROGRAM_BINDING_ARB				0x8677
	#define GL_PROGRAM_INSTRUCTIONS_ARB			0x88A0
	#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB		0x88A1
	#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB	0x88A2
	#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
	#define GL_PROGRAM_TEMPORARIES_ARB			0x88A4
	#define GL_MAX_PROGRAM_TEMPORARIES_ARB		0x88A5
	#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB	0x88A6
	#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
	#define GL_PROGRAM_PARAMETERS_ARB			0x88A8
	#define GL_MAX_PROGRAM_PARAMETERS_ARB		0x88A9
	#define GL_PROGRAM_NATIVE_PARAMETERS_ARB	0x88AA
	#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
	#define GL_PROGRAM_ATTRIBS_ARB				0x88AC
	#define GL_MAX_PROGRAM_ATTRIBS_ARB			0x88AD
	#define GL_PROGRAM_NATIVE_ATTRIBS_ARB		0x88AE
	#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB	0x88AF
	#define GL_PROGRAM_ADDRESS_REGISTERS_ARB	0x88B0
	#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B1
	#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B2
	#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B3
	#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB	0x88B4
	#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB	0x88B5
	#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB	0x88B6
	
	#define GL_PROGRAM_STRING_ARB				0x8628

	#define GL_PROGRAM_ERROR_POSITION_ARB		0x864B
	#define GL_CURRENT_MATRIX_ARB				0x8641
	#define GL_TRANSPOSE_CURRENT_MATRIX_ARB		0x88B7
	#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB	0x8640
	#define GL_MAX_VERTEX_ATTRIBS_ARB			0x8869
	#define GL_MAX_PROGRAM_MATRICES_ARB			0x862F
	#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E

	#define GL_PROGRAM_ERROR_STRING_ARB			0x8874

	#define GL_MATRIX0_ARB						0x88C0
	#define GL_MATRIX1_ARB						0x88C1
	#define GL_MATRIX2_ARB						0x88C2
	#define GL_MATRIX3_ARB						0x88C3
	#define GL_MATRIX4_ARB						0x88C4
	#define GL_MATRIX5_ARB						0x88C5
	#define GL_MATRIX6_ARB						0x88C6
	#define GL_MATRIX7_ARB						0x88C7
	#define GL_MATRIX8_ARB						0x88C8
	#define GL_MATRIX9_ARB						0x88C9
	#define GL_MATRIX10_ARB						0x88CA
	#define GL_MATRIX11_ARB						0x88CB
	#define GL_MATRIX12_ARB						0x88CC
	#define GL_MATRIX13_ARB						0x88CD
	#define GL_MATRIX14_ARB						0x88CE
	#define GL_MATRIX15_ARB						0x88CF
	#define GL_MATRIX16_ARB						0x88D0
	#define GL_MATRIX17_ARB						0x88D1
	#define GL_MATRIX18_ARB						0x88D2
	#define GL_MATRIX19_ARB						0x88D3
	#define GL_MATRIX20_ARB						0x88D4
	#define GL_MATRIX21_ARB						0x88D5
	#define GL_MATRIX22_ARB						0x88D6
	#define GL_MATRIX23_ARB						0x88D7
	#define GL_MATRIX24_ARB						0x88D8
	#define GL_MATRIX25_ARB						0x88D9
	#define GL_MATRIX26_ARB						0x88DA
	#define GL_MATRIX27_ARB						0x88DB
	#define GL_MATRIX28_ARB						0x88DC
	#define GL_MATRIX29_ARB						0x88DD
	#define GL_MATRIX30_ARB						0x88DE
	#define GL_MATRIX31_ARB						0x88DF

	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1S_ARB_PROC)(GLuint index, GLshort x);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1F_ARB_PROC)(GLuint index, GLfloat x);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1D_ARB_PROC)(GLuint index, GLdouble x);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2S_ARB_PROC)(GLuint index, GLshort x, GLshort y);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2F_ARB_PROC)(GLuint index, GLfloat x, GLfloat y);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2D_ARB_PROC)(GLuint index, GLdouble x, GLdouble y);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3S_ARB_PROC)(GLuint index, GLshort x, GLshort y, GLshort z);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3F_ARB_PROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3D_ARB_PROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4S_ARB_PROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4F_ARB_PROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4D_ARB_PROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NUB_ARB_PROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1SV_ARB_PROC)(GLuint index, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1FV_ARB_PROC)(GLuint index, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_1DV_ARB_PROC)(GLuint index, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2SV_ARB_PROC)(GLuint index, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2FV_ARB_PROC)(GLuint index, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_2DV_ARB_PROC)(GLuint index, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3SV_ARB_PROC)(GLuint index, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3FV_ARB_PROC)(GLuint index, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_3DV_ARB_PROC)(GLuint index, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4BV_ARB_PROC)(GLuint index, const GLbyte *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4SV_ARB_PROC)(GLuint index, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4IV_ARB_PROC)(GLuint index, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4UBV_ARB_PROC)(GLuint index, const GLubyte *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4USV_ARB_PROC)(GLuint index, const GLushort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4UIV_ARB_PROC)(GLuint index, const GLuint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4FV_ARB_PROC)(GLuint index, const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4DV_ARB_PROC)(GLuint index, const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NBV_ARB_PROC)(GLuint index, const GLbyte *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NSV_ARB_PROC)(GLuint index, const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NIV_ARB_PROC)(GLuint index, const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NUBV_ARB_PROC)(GLuint index, const GLubyte *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NUSV_ARB_PROC)(GLuint index, const GLushort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_4NUIV_ARB_PROC)(GLuint index, const GLuint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_VERTEX_ATTRIB_POINTER_ARB_PROC)(GLuint index, GLint size, GLenum type,GLboolean normalized, GLsizei stride,const GLvoid *pointer);
	typedef GLvoid (RAPTOR_APICALL * PFN_ENABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC)(GLuint index);
	typedef GLvoid (RAPTOR_APICALL * PFN_DISABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC)(GLuint index);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_STRING_ARB_PROC)(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
	typedef GLvoid (RAPTOR_APICALL * PFN_BIND_PROGRAM_ARB_PROC)(GLenum target, GLuint id);
	typedef GLvoid (RAPTOR_APICALL * PFN_DELETE_PROGRAMS_ARB_PROC)(GLsizei n, const GLuint *ids);
	typedef GLvoid (RAPTOR_APICALL * PFN_GEN_PROGRAMS_ARB_PROC)(GLsizei n, GLuint *ids);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_ENV_PARAMETER_4F_ARB_PROC)(GLenum target, GLuint index,GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_ENV_PARAMETER_4D_ARB_PROC)(GLenum target, GLuint index,GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_ENV_PARAMETER_4DV_ARB_PROC)(GLenum target, GLuint index,const GLdouble *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_ENV_PARAMETER_4FV_ARB_PROC)(GLenum target, GLuint index,const GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_LOCAL_PARAMETER_4F_ARB_PROC)(GLenum target, GLuint index,GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_LOCAL_PARAMETER_4D_ARB_PROC)(GLenum target, GLuint index,GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_LOCAL_PARAMETER_4DV_ARB_PROC)(GLenum target, GLuint index,const GLdouble *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_LOCAL_PARAMETER_4FV_ARB_PROC)(GLenum target, GLuint index,const GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_ENV_PARAMETER_FV_ARB_PROC)(GLenum target, GLuint index,GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_ENV_PARAMETER_DV_ARB_PROC)(GLenum target, GLuint index,GLenum pname, GLdouble *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_LOCAL_PARAMETER_FV_ARB_PROC)(GLenum target, GLuint index,GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_LOCAL_PARAMETER_DV_ARB_PROC)(GLenum target, GLuint index,GLenum pname, GLdouble *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_IV_ARB_PROC)(GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_PROGRAM_STRING_ARB_PROC)(GLenum target, GLenum pname, GLvoid *program);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_VERTEX_ATTRIB_DV_ARB_PROC)(GLuint index, GLenum pname, GLdouble *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_VERTEX_ATTRIB_FV_ARB_PROC)(GLuint index, GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_VERTEX_ATTRIB_IV_ARB_PROC)(GLuint index, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_VERTEX_ATTRIB_POINTERV_ARB_PROC)(GLuint index, GLenum pname, GLvoid **pointer);
	typedef GLboolean (RAPTOR_APICALL * PFN_IS_PROGRAM_ARB_PROC)(GLuint id);
	
	#ifndef DECLARE_GL_ARB_vertex_program
	#define DECLARE_GL_ARB_vertex_program(LINKAGE) \
		LINKAGE PFN_VERTEX_ATTRIB_1S_ARB_PROC			glVertexAttrib1sARB; \
		LINKAGE PFN_VERTEX_ATTRIB_1F_ARB_PROC			glVertexAttrib1fARB; \
		LINKAGE PFN_VERTEX_ATTRIB_1D_ARB_PROC			glVertexAttrib1dARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2S_ARB_PROC			glVertexAttrib2sARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2F_ARB_PROC			glVertexAttrib2fARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2D_ARB_PROC			glVertexAttrib2dARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3S_ARB_PROC			glVertexAttrib3sARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3F_ARB_PROC			glVertexAttrib3fARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3D_ARB_PROC			glVertexAttrib3dARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4S_ARB_PROC			glVertexAttrib4sARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4F_ARB_PROC			glVertexAttrib4fARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4D_ARB_PROC			glVertexAttrib4dARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NUB_ARB_PROC			glVertexAttrib4NubARB; \
		LINKAGE PFN_VERTEX_ATTRIB_1SV_ARB_PROC			glVertexAttrib1svARB; \
		LINKAGE PFN_VERTEX_ATTRIB_1FV_ARB_PROC			glVertexAttrib1fvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_1DV_ARB_PROC			glVertexAttrib1dvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2SV_ARB_PROC			glVertexAttrib2svARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2FV_ARB_PROC			glVertexAttrib2fvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_2DV_ARB_PROC			glVertexAttrib2dvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3SV_ARB_PROC			glVertexAttrib3svARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3FV_ARB_PROC			glVertexAttrib3fvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_3DV_ARB_PROC			glVertexAttrib3dvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4BV_ARB_PROC			glVertexAttrib4bvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4SV_ARB_PROC			glVertexAttrib4svARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4IV_ARB_PROC			glVertexAttrib4ivARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4UBV_ARB_PROC			glVertexAttrib4ubvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4USV_ARB_PROC			glVertexAttrib4usvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4UIV_ARB_PROC			glVertexAttrib4uivARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4FV_ARB_PROC			glVertexAttrib4fvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4DV_ARB_PROC			glVertexAttrib4dvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NBV_ARB_PROC			glVertexAttrib4NbvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NSV_ARB_PROC			glVertexAttrib4NsvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NIV_ARB_PROC			glVertexAttrib4NivARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NUBV_ARB_PROC			glVertexAttrib4NubvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NUSV_ARB_PROC			glVertexAttrib4NusvARB; \
		LINKAGE PFN_VERTEX_ATTRIB_4NUIV_ARB_PROC			glVertexAttrib4NuivARB; \
		LINKAGE PFN_VERTEX_ATTRIB_POINTER_ARB_PROC		glVertexAttribPointerARB; \
		LINKAGE PFN_ENABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC	glEnableVertexAttribArrayARB; \
		LINKAGE PFN_DISABLE_VERTEX_ATTRIB_ARRAY_ARB_PROC	glDisableVertexAttribArrayARB; \
		LINKAGE PFN_PROGRAM_STRING_ARB_PROC				glProgramStringARB; \
		LINKAGE PFN_BIND_PROGRAM_ARB_PROC				glBindProgramARB; \
		LINKAGE PFN_DELETE_PROGRAMS_ARB_PROC				glDeleteProgramsARB; \
		LINKAGE PFN_GEN_PROGRAMS_ARB_PROC				glGenProgramsARB; \
		LINKAGE PFN_PROGRAM_ENV_PARAMETER_4F_ARB_PROC	glProgramEnvParameter4fARB; \
		LINKAGE PFN_PROGRAM_ENV_PARAMETER_4D_ARB_PROC	glProgramEnvParameter4dARB; \
		LINKAGE PFN_PROGRAM_ENV_PARAMETER_4DV_ARB_PROC	glProgramEnvParameter4dvARB; \
		LINKAGE PFN_PROGRAM_ENV_PARAMETER_4FV_ARB_PROC	glProgramEnvParameter4fvARB; \
		LINKAGE PFN_PROGRAM_LOCAL_PARAMETER_4F_ARB_PROC	glProgramLocalParameter4fARB; \
		LINKAGE PFN_PROGRAM_LOCAL_PARAMETER_4D_ARB_PROC	glProgramLocalParameter4dARB; \
		LINKAGE PFN_PROGRAM_LOCAL_PARAMETER_4DV_ARB_PROC	glProgramLocalParameter4dvARB; \
		LINKAGE PFN_PROGRAM_LOCAL_PARAMETER_4FV_ARB_PROC	glProgramLocalParameter4fvARB; \
		LINKAGE PFN_GET_PROGRAM_ENV_PARAMETER_FV_ARB_PROC	glGetProgramEnvParameterfvARB; \
		LINKAGE PFN_GET_PROGRAM_ENV_PARAMETER_DV_ARB_PROC	glGetProgramEnvParameterdvARB; \
		LINKAGE PFN_GET_PROGRAM_LOCAL_PARAMETER_FV_ARB_PROC	glGetProgramLocalParameterfvARB; \
		LINKAGE PFN_GET_PROGRAM_LOCAL_PARAMETER_DV_ARB_PROC	glGetProgramLocalParameterdvARB; \
		LINKAGE PFN_GET_PROGRAM_IV_ARB_PROC				glGetProgramivARB; \
		LINKAGE PFN_GET_PROGRAM_STRING_ARB_PROC			glGetProgramStringARB; \
		LINKAGE PFN_GET_VERTEX_ATTRIB_DV_ARB_PROC		glGetVertexAttribdvARB; \
		LINKAGE PFN_GET_VERTEX_ATTRIB_FV_ARB_PROC		glGetVertexAttribfvARB; \
		LINKAGE PFN_GET_VERTEX_ATTRIB_IV_ARB_PROC		glGetVertexAttribivARB; \
		LINKAGE PFN_GET_VERTEX_ATTRIB_POINTERV_ARB_PROC	glGetVertexAttribPointervARB; \
		LINKAGE PFN_IS_PROGRAM_ARB_PROC					glIsProgramARB;
	#endif
#else
	#define DECLARE_GL_ARB_vertex_program(LINKAGE)
#endif


/* GL_ARB_fragment_program			*/
/*	ARB model of "pixel shaders"	*/
#if defined(GL_ARB_fragment_program)
	#define GL_FRAGMENT_PROGRAM_ARB				0x8804

	// #define GL_PROGRAM_FORMAT_ASCII_ARB		0x8875
	// #define GL_PROGRAM_LENGTH_ARB			0x8627
	// #defien GL_PROGRAM_FORMAT_ARB			0x8876
	// #define GL_PROGRAM_BINDING_ARB			0x8677
	// #define GL_PROGRAM_INSTRUCTIONS_ARB		0x88A0
	// #define GL_MAX_PROGRAM_INSTRUCTIONS_ARB	0x88A1
	// #define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A2
	// #define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
	// #define GL_PROGRAM_TEMPORARIES_ARB 0x88A4
	// #define GL_MAX_PROGRAM_TEMPORARIES_ARB 0x88A5
	// #define GL_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A6
	// #define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
	// #define GL_PROGRAM_PARAMETERS_ARB 0x88A8
	// #define GL_MAX_PROGRAM_PARAMETERS_ARB 0x88A9
	// #define GL_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AA
	// #define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
	// #define GL_PROGRAM_ATTRIBS_ARB 0x88AC
	// #define GL_MAX_PROGRAM_ATTRIBS_ARB 0x88AD
	// #define GL_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AE
	// #define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AF
	// #define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB 0x88B4
	// #define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB 0x88B5
	// #define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB 0x88B6
	#define GL_PROGRAM_ALU_INSTRUCTIONS_ARB		0x8805
	#define GL_PROGRAM_TEX_INSTRUCTIONS_ARB		0x8806
	#define GL_PROGRAM_TEX_INDIRECTIONS_ARB		0x8807
	#define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x8808
	#define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x8809
	#define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x880A
	#define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB 0x880B
	#define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB 0x880C
	#define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB 0x880D
	#define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x880E
	#define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x880F
	#define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x8810

	// #define GL_PROGRAM_STRING_ARB			0x8628
	// #define GL_PROGRAM_ERROR_POSITION_ARB	0x864B
	// #define GL_CURRENT_MATRIX_ARB			0x8641
	// #define GL_TRANSPOSE_CURRENT_MATRIX_ARB 0x88B7
	// #define GL_CURRENT_MATRIX_STACK_DEPTH_ARB 0x8640
	// #define GL_MAX_PROGRAM_MATRICES_ARB 0x862F
	// #define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
	#define GL_MAX_TEXTURE_COORDS_ARB			0x8871
	#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB		0x8872

	// #define GL_PROGRAM_ERROR_STRING_ARB 0x8874
	// all enumerants from MATRIX0_ARB to MATRIX31_ARB 
	// are also previously defined by vertex_program
#endif

/* WGL_NV_render_depth_texture */
#if defined(WGL_NV_render_depth_texture)
	#define WGL_BIND_TO_TEXTURE_DEPTH_NV		0x20A3
	#define WGL_BIND_TO_TEXTURE_RECTANGLE_DEPTH_NV \
												0x20A4
	#define WGL_DEPTH_TEXTURE_FORMAT_NV			0x20A5
	#define WGL_TEXTURE_DEPTH_COMPONENT_NV		0x20A6
	#define WGL_NO_TEXTURE_ARB					0x2077
	#define WGL_DEPTH_COMPONENT_NV				0x20A7
#endif

/* GL_ARB_shadow */
#if defined(GL_ARB_shadow)
	#define GL_TEXTURE_COMPARE_MODE_ARB			0x884C
	#define GL_TEXTURE_COMPARE_FUNC_ARB			0x884D
	#define GL_COMPARE_R_TO_TEXTURE_ARB			0x884E
#endif

/* GL_ARB_shadow_ambient */
#if defined(GL_ARB_shadow_ambient)
	#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF
#endif

/* GL_ARB_window_pos*/
#if defined(GL_ARB_window_pos)
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2D_ARB_PROC) (GLdouble x, GLdouble y);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2DV_ARB_PROC) (const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2F_ARB_PROC) (GLfloat x, GLfloat y);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2FV_ARB_PROC) (const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2I_ARB_PROC) (GLint x, GLint y);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2IV_ARB_PROC) (const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2S_ARB_PROC) (GLshort x, GLshort y);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_2SV_ARB_PROC) (const GLshort *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3D_ARB_PROC) (GLdouble x, GLdouble y, GLdouble z);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3DV_ARB_PROC) (const GLdouble *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3F_ARB_PROC) (GLfloat x, GLfloat y, GLfloat z);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3FV_ARB_PROC) (const GLfloat *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3I_ARB_PROC) (GLint x, GLint y, GLint z);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3IV_ARB_PROC) (const GLint *v);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3S_ARB_PROC) (GLshort x, GLshort y, GLshort z);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_WINDOW_POS_3SV_ARB_PROC) (const GLshort *v);

	#ifndef DECLARE_GL_ARB_window_pos
	#define DECLARE_GL_ARB_window_pos(LINKAGE)\
		LINKAGE PFN_GL_WINDOW_POS_2D_ARB_PROC glWindowPos2dARB; \
		LINKAGE PFN_GL_WINDOW_POS_2DV_ARB_PROC glWindowPos2dvARB; \
		LINKAGE PFN_GL_WINDOW_POS_2F_ARB_PROC glWindowPos2fARB; \
		LINKAGE PFN_GL_WINDOW_POS_2FV_ARB_PROC glWindowPos2fvARB; \
		LINKAGE PFN_GL_WINDOW_POS_2I_ARB_PROC glWindowPos2iARB; \
		LINKAGE PFN_GL_WINDOW_POS_2IV_ARB_PROC glWindowPos2ivARB; \
		LINKAGE PFN_GL_WINDOW_POS_2S_ARB_PROC glWindowPos2sARB; \
		LINKAGE PFN_GL_WINDOW_POS_2SV_ARB_PROC glWindowPos2svARB; \
		LINKAGE PFN_GL_WINDOW_POS_3D_ARB_PROC glWindowPos3dARB; \
		LINKAGE PFN_GL_WINDOW_POS_3DV_ARB_PROC glWindowPos3dvARB; \
		LINKAGE PFN_GL_WINDOW_POS_3F_ARB_PROC glWindowPos3fARB; \
		LINKAGE PFN_GL_WINDOW_POS_3FV_ARB_PROC glWindowPos3fvARB; \
		LINKAGE PFN_GL_WINDOW_POS_3I_ARB_PROC glWindowPos3iARB; \
		LINKAGE PFN_GL_WINDOW_POS_3IV_ARB_PROC glWindowPos3ivARB; \
		LINKAGE PFN_GL_WINDOW_POS_3S_ARB_PROC glWindowPos3sARB; \
		LINKAGE PFN_GL_WINDOW_POS_3SV_ARB_PROC glWindowPos3svARB;
	#endif
#else
	#define DECLARE_GL_ARB_window_pos(LINKAGE)
#endif

/* GL_ARB_depth_texture */
#if defined(GL_ARB_depth_texture)
	#define GL_DEPTH_COMPONENT_ARB				0x20A7
	#define GL_DEPTH_COMPONENT16_ARB			0x81A5
	#define GL_DEPTH_COMPONENT24_ARB			0x81A6
	#define GL_DEPTH_COMPONENT32_ARB			0x81A7
	#define GL_TEXTURE_DEPTH_SIZE_ARB			0x884A
	#define GL_DEPTH_TEXTURE_MODE_ARB			0x884B
#endif

#if defined(GL_ARB_occlusion_query)
	#define GL_SAMPLES_PASSED_ARB               0x8914
	#define GL_QUERY_COUNTER_BITS_ARB           0x8864
	#define GL_CURRENT_QUERY_ARB                0x8865
	#define GL_QUERY_RESULT_ARB                 0x8866
	#define GL_QUERY_RESULT_AVAILABLE_ARB       0x8867

	typedef GLvoid (RAPTOR_APICALL * PFN_GEN_QUERIES_ARB_PROC)(GLsizei n, GLuint *ids);
	typedef GLvoid (RAPTOR_APICALL * PFN_DELETE_QUERIES_ARB_PROC)(GLsizei n, const GLuint *ids);
	typedef GLboolean (RAPTOR_APICALL * PFN_IS_QUERY_ARB_PROC)(GLuint id);
	typedef GLvoid (RAPTOR_APICALL * PFN_BEGIN_QUERY_ARB_PROC)(GLenum target, GLuint id);
	typedef GLvoid (RAPTOR_APICALL * PFN_END_QUERY_ARB_PROC)(GLenum target);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_QUERY_IV_ARB_PROC)(GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_QUERY_OBJECT_IV_ARB_PROC)(GLuint id, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GET_QUERY_OBJECT_UIV_ARB_PROC)(GLuint id, GLenum pname, GLuint *params);

	#ifndef DECLARE_GL_ARB_occlusion_query
	#define DECLARE_GL_ARB_occlusion_query(LINKAGE)\
		LINKAGE PFN_GEN_QUERIES_ARB_PROC glGenQueriesARB;\
		LINKAGE PFN_DELETE_QUERIES_ARB_PROC glDeleteQueriesARB;\
		LINKAGE PFN_IS_QUERY_ARB_PROC glIsQueryARB;\
		LINKAGE PFN_BEGIN_QUERY_ARB_PROC glBeginQueryARB;\
		LINKAGE PFN_END_QUERY_ARB_PROC glEndQueryARB;\
		LINKAGE PFN_GET_QUERY_IV_ARB_PROC glGetQueryivARB;\
		LINKAGE PFN_GET_QUERY_OBJECT_IV_ARB_PROC glGetQueryObjectivARB;\
		LINKAGE PFN_GET_QUERY_OBJECT_UIV_ARB_PROC glGetQueryObjectuivARB;
	#endif
#else
	#define DECLARE_GL_ARB_occlusion_query(LINKAGE)
#endif

#if defined(GL_ARB_color_buffer_float)
    #define GL_RGBA_FLOAT_MODE_ARB		0x8820
    #define GL_CLAMP_VERTEX_COLOR_ARB	0x891A
    #define GL_CLAMP_FRAGMENT_COLOR_ARB	0x891B
    #define GL_CLAMP_READ_COLOR_ARB		0x891C
    #define GL_FIXED_ONLY_ARB			0x891D
    #define WGL_TYPE_RGBA_FLOAT_ARB		0x21A0

    typedef GLvoid (RAPTOR_APICALL * PFN_CLAMP_COLOR_ARB_PROC)(GLenum target, GLenum clamp);
    #ifndef DECLARE_GL_ARB_color_buffer_float
    #define DECLARE_GL_ARB_color_buffer_float(LINKAGE) \
        LINKAGE PFN_CLAMP_COLOR_ARB_PROC glClampColorARB;
    #endif
#else
    #define DECLARE_GL_ARB_color_buffer_float(LINKAGE)
#endif

#if defined(GL_ARB_texture_float)
    #define GL_TEXTURE_RED_TYPE_ARB			0x8C10
    #define GL_TEXTURE_GREEN_TYPE_ARB		0x8C11
    #define GL_TEXTURE_BLUE_TYPE_ARB		0x8C12
    #define GL_TEXTURE_ALPHA_TYPE_ARB		0x8C13
    #define GL_TEXTURE_LUMINANCE_TYPE_ARB	0x8C14
    #define GL_TEXTURE_INTENSITY_TYPE_ARB	0x8C15
    #define GL_TEXTURE_DEPTH_TYPE_ARB		0x8C16
    #define GL_UNSIGNED_NORMALIZED_ARB		0x8C17
    #define GL_RGBA32F_ARB					0x8814
    #define GL_RGB32F_ARB					0x8815
    #define GL_ALPHA32F_ARB					0x8816
    #define GL_INTENSITY32F_ARB				0x8817
    #define GL_LUMINANCE32F_ARB				0x8818
    #define GL_LUMINANCE_ALPHA32F_ARB		0x8819
    #define GL_RGBA16F_ARB					0x881A
    #define GL_RGB16F_ARB					0x881B
    #define GL_ALPHA16F_ARB					0x881C
    #define GL_INTENSITY16F_ARB				0x881D
    #define GL_LUMINANCE16F_ARB				0x881E
    #define GL_LUMINANCE_ALPHA16F_ARB		0x881F
#endif

#if defined (GL_ARB_texture_border_clamp)
    #define GL_CLAMP_TO_BORDER_ARB	0x812D
#endif

#if defined(GL_EXT_texture_edge_clamp)
    #define GL_CLAMP_TO_EDGE_EXT	0x812F
#endif

#if defined(GL_ARB_pixel_buffer_object)
    #define GL_PIXEL_PACK_BUFFER_ARB			0x88EB
    #define GL_PIXEL_UNPACK_BUFFER_ARB			0x88EC
    #define GL_PIXEL_PACK_BUFFER_BINDING_ARB	0x88ED
    #define GL_PIXEL_UNPACK_BUFFER_BINDING_ARB	0x88EF
#endif


#if defined(GL_ARB_texture_rectangle)
    #define GL_TEXTURE_RECTANGLE_ARB			0x84F5
    #define GL_TEXTURE_BINDING_RECTANGLE_ARB    0x84F6
    #define GL_PROXY_TEXTURE_RECTANGLE_ARB      0x84F7
    #define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB   0x84F8
#endif


#if defined(GL_ARB_shading_language_100)
    #define GL_SHADING_LANGUAGE_VERSION_ARB	0x8B8C
#endif

#if defined(GL_ARB_shader_objects)
    #define GL_PROGRAM_OBJECT_ARB					0x8B40
	#define GL_OBJECT_TYPE_ARB						0x8B4E
	#define GL_OBJECT_SUBTYPE_ARB					0x8B4F
	#define GL_OBJECT_DELETE_STATUS_ARB				0x8B80
	#define GL_OBJECT_COMPILE_STATUS_ARB			0x8B81
	#define GL_OBJECT_LINK_STATUS_ARB				0x8B82
	#define GL_OBJECT_VALIDATE_STATUS_ARB			0x8B83
	#define GL_OBJECT_INFO_LOG_LENGTH_ARB			0x8B84
	#define GL_OBJECT_ATTACHED_OBJECTS_ARB			0x8B85
	#define GL_OBJECT_ACTIVE_UNIFORMS_ARB			0x8B86
	#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB	0x8B87
	#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB		0x8B88
	#define GL_SHADER_OBJECT_ARB					0x8B48
	#define GL_FLOAT								0x1406
	#define GL_FLOAT_VEC2_ARB						0x8B50
	#define GL_FLOAT_VEC3_ARB						0x8B51
	#define GL_FLOAT_VEC4_ARB						0x8B52
	#define GL_INT									0x1404
	#define GL_INT_VEC2_ARB							0x8B53
	#define GL_INT_VEC3_ARB							0x8B54
	#define GL_INT_VEC4_ARB							0x8B55
	#define GL_BOOL_ARB								0x8B56
	#define GL_BOOL_VEC2_ARB						0x8B57
	#define GL_BOOL_VEC3_ARB						0x8B58
	#define GL_BOOL_VEC4_ARB						0x8B59
	#define GL_FLOAT_MAT2_ARB						0x8B5A
	#define GL_FLOAT_MAT3_ARB						0x8B5B
	#define GL_FLOAT_MAT4_ARB						0x8B5C
	#define GL_SAMPLER_1D_ARB						0x8B5D
	#define GL_SAMPLER_2D_ARB						0x8B5E
	#define GL_SAMPLER_3D_ARB						0x8B5F
	#define GL_SAMPLER_CUBE_ARB						0x8B60
	#define GL_SAMPLER_1D_SHADOW_ARB				0x8B61
	#define GL_SAMPLER_2D_SHADOW_ARB				0x8B62
	#define GL_SAMPLER_2D_RECT_ARB					0x8B63
	#define GL_SAMPLER_2D_RECT_SHADOW_ARB			0x8B64

    typedef char GLcharARB;
	typedef long GLhandleARB;

    typedef GLvoid (RAPTOR_APICALL * PFN_DELETE_OBJECT_ARB_PROC)(GLhandleARB obj);
    typedef GLhandleARB (RAPTOR_APICALL * PFN_GET_HANDLE_ARB_PROC)(GLenum pname);
    typedef GLvoid (RAPTOR_APICALL * PFN_DETACH_OBJECT_ARB_PROC)(GLhandleARB containerObj, GLhandleARB attachedObj);
    typedef GLhandleARB (RAPTOR_APICALL * PFN_CREATE_SHADER_OBJECT_ARB_PROC)(GLenum shaderType);
    typedef GLvoid (RAPTOR_APICALL * PFN_SHADER_SOURCE_ARB_PROC)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string,const GLint *length);
    typedef GLvoid (RAPTOR_APICALL * PFN_COMPILE_SHADER_ARB_PROC)(GLhandleARB shaderObj);
    typedef GLhandleARB (RAPTOR_APICALL * PFN_CREATE_PROGRAM_OBJECT_ARB_PROC)(void);
    typedef GLvoid (RAPTOR_APICALL * PFN_ATTACH_OBJECT_ARB_PROC)(GLhandleARB containerObj, GLhandleARB obj);
    typedef GLvoid (RAPTOR_APICALL * PFN_LINK_PROGRAM_ARB_PROC)(GLhandleARB programObj);
    typedef GLvoid (RAPTOR_APICALL * PFN_USE_PROGRAM_OBJECT_ARB_PROC)(GLhandleARB programObj);
    typedef GLvoid (RAPTOR_APICALL * PFN_VALIDATE_PROGRAM_ARB_PROC)(GLhandleARB programObj);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_1F_ARB_PROC)(GLint location, GLfloat v0);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_2F_ARB_PROC)(GLint location, GLfloat v0, GLfloat v1);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_3F_ARB_PROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_4F_ARB_PROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_1I_ARB_PROC)(GLint location, GLint v0);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_2I_ARB_PROC)(GLint location, GLint v0, GLint v1);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_3I_ARB_PROC)(GLint location, GLint v0, GLint v1, GLint v2);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_4I_ARB_PROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_1FV_ARB_PROC)(GLint location, GLsizei count, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_2FV_ARB_PROC)(GLint location, GLsizei count, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_3FV_ARB_PROC)(GLint location, GLsizei count, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_4FV_ARB_PROC)(GLint location, GLsizei count, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_1IV_ARB_PROC)(GLint location, GLsizei count, const GLint *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_2IV_ARB_PROC)(GLint location, GLsizei count, const GLint *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_3IV_ARB_PROC)(GLint location, GLsizei count, const GLint *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_4IV_ARB_PROC)(GLint location, GLsizei count, const GLint *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_MATRIX_2FV_ARB_PROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_MATRIX_3FV_ARB_PROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_UNIFORM_MATRIX_4FV_ARB_PROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_OBJECT_PARAMETER_FV_ARB_PROC)(GLhandleARB obj, GLenum pname, GLfloat *params);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_OBJECT_PARAMETER_IV_ARB_PROC)(GLhandleARB obj, GLenum pname, GLint *params);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_INFO_LOG_ARB_PROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_ATTACHED_OBJECTS_ARB_PROC)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count,GLhandleARB *obj);
    typedef GLint (RAPTOR_APICALL * PFN_GET_UNIFORM_LOCATION_ARB_PROC)(GLhandleARB programObj, const GLcharARB *name);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_ACTIVE_UNIFORM_ARB_PROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength,GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_UNIFORM_FV_ARB_PROC)(GLhandleARB programObj, GLint location, GLfloat *params);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_UNIFORM_IV_ARB_PROC)(GLhandleARB programObj, GLint location, GLint *params);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_SHADER_SOURCE_ARB_PROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length,GLcharARB *source);
    #ifndef DECLARE_GL_ARB_shader_objects
    #define DECLARE_GL_ARB_shader_objects(LINKAGE) \
        LINKAGE PFN_DELETE_OBJECT_ARB_PROC glDeleteObjectARB; \
        LINKAGE PFN_GET_HANDLE_ARB_PROC glGetHandleARB; \
        LINKAGE PFN_DETACH_OBJECT_ARB_PROC glDetachObjectARB; \
        LINKAGE PFN_CREATE_SHADER_OBJECT_ARB_PROC glCreateShaderObjectARB; \
        LINKAGE PFN_SHADER_SOURCE_ARB_PROC glShaderSourceARB; \
        LINKAGE PFN_COMPILE_SHADER_ARB_PROC glCompileShaderARB; \
        LINKAGE PFN_CREATE_PROGRAM_OBJECT_ARB_PROC glCreateProgramObjectARB; \
        LINKAGE PFN_ATTACH_OBJECT_ARB_PROC glAttachObjectARB; \
        LINKAGE PFN_LINK_PROGRAM_ARB_PROC glLinkProgramARB; \
        LINKAGE PFN_USE_PROGRAM_OBJECT_ARB_PROC glUseProgramObjectARB; \
        LINKAGE PFN_VALIDATE_PROGRAM_ARB_PROC glValidateProgramARB; \
        LINKAGE PFN_UNIFORM_1F_ARB_PROC glUniform1fARB; \
        LINKAGE PFN_UNIFORM_2F_ARB_PROC glUniform2fARB; \
        LINKAGE PFN_UNIFORM_3F_ARB_PROC glUniform3fARB; \
        LINKAGE PFN_UNIFORM_4F_ARB_PROC glUniform4fARB; \
        LINKAGE PFN_UNIFORM_1I_ARB_PROC glUniform1iARB; \
        LINKAGE PFN_UNIFORM_2I_ARB_PROC glUniform2iARB; \
        LINKAGE PFN_UNIFORM_3I_ARB_PROC glUniform3iARB; \
        LINKAGE PFN_UNIFORM_4I_ARB_PROC glUniform4iARB; \
        LINKAGE PFN_UNIFORM_1FV_ARB_PROC glUniform1fvARB; \
        LINKAGE PFN_UNIFORM_2FV_ARB_PROC glUniform2fvARB; \
        LINKAGE PFN_UNIFORM_3FV_ARB_PROC glUniform3fvARB; \
        LINKAGE PFN_UNIFORM_4FV_ARB_PROC glUniform4fvARB; \
        LINKAGE PFN_UNIFORM_1IV_ARB_PROC glUniform1ivARB; \
        LINKAGE PFN_UNIFORM_2IV_ARB_PROC glUniform2ivARB; \
        LINKAGE PFN_UNIFORM_3IV_ARB_PROC glUniform3ivARB; \
        LINKAGE PFN_UNIFORM_4IV_ARB_PROC glUniform4ivARB; \
        LINKAGE PFN_UNIFORM_MATRIX_2FV_ARB_PROC glUniformMatrix2fvARB; \
        LINKAGE PFN_UNIFORM_MATRIX_3FV_ARB_PROC glUniformMatrix3fvARB; \
        LINKAGE PFN_UNIFORM_MATRIX_4FV_ARB_PROC glUniformMatrix4fvARB; \
        LINKAGE PFN_GET_OBJECT_PARAMETER_FV_ARB_PROC glGetObjectParameterfvARB; \
        LINKAGE PFN_GET_OBJECT_PARAMETER_IV_ARB_PROC glGetObjectParameterivARB; \
        LINKAGE PFN_GET_INFO_LOG_ARB_PROC glGetInfoLogARB; \
        LINKAGE PFN_GET_ATTACHED_OBJECTS_ARB_PROC glGetAttachedObjectsARB; \
        LINKAGE PFN_GET_UNIFORM_LOCATION_ARB_PROC glGetUniformLocationARB; \
        LINKAGE PFN_GET_ACTIVE_UNIFORM_ARB_PROC glGetActiveUniformARB; \
        LINKAGE PFN_GET_UNIFORM_FV_ARB_PROC glGetUniformfvARB; \
        LINKAGE PFN_GET_UNIFORM_IV_ARB_PROC glGetUniformivARB; \
        LINKAGE PFN_GET_SHADER_SOURCE_ARB_PROC glGetShaderSourceARB;
    #endif
#else
    #define DECLARE_GL_ARB_shader_objects(LINKAGE)
#endif

#if defined(GL_ARB_vertex_shader)
    #define GL_VERTEX_SHADER_ARB					0x8B31
    #define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB	0x8B4A
	#define GL_MAX_VARYING_FLOATS_ARB				0x8B4B
	#define GL_MAX_VERTEX_ATTRIBS_ARB				0x8869
	#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872
	#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB	0x8B4C
	#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB	0x8B4D
	#define GL_MAX_TEXTURE_COORDS_ARB				0x8871
//  #define GL_VERTEX_PROGRAM_POINT_SIZE_ARB		0x8642      : already defined
//	#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB			0x8643          : idem
    #define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB			0x8B89
    #define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB	0x8B8A
//  #define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB		0x8622    : already defined
//	#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB			0x8623                  : idem
//	#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB		0x8624          : idem ...
//	#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB			0x8625
//	#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB	0x886A
//	#define GL_CURRENT_VERTEX_ATTRIB_ARB			0x8626
//  #define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB		0x8645
//  #define GL_FLOAT								0x1406
//	#define GL_FLOAT_VEC2_ARB						0x8B50
//	#define GL_FLOAT_VEC3_ARB						0x8B51
//	#define GL_FLOAT_VEC4_ARB						0x8B52
//	#define GL_FLOAT_MAT2_ARB						0x8B5A
//	#define GL_FLOAT_MAT3_ARB						0x8B5B
//	#define GL_FLOAT_MAT4_ARB						0x8B5C

    typedef GLvoid (RAPTOR_APICALL * PFN_BIND_ATTRIB_LOCATION_ARB_PROC)(GLhandleARB programObj, GLuint index, const GLcharARB *name);
    typedef GLvoid (RAPTOR_APICALL * PFN_GET_ACTIVE_ATTRIB_ARB_PROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
    typedef GLint (RAPTOR_APICALL * PFN_GET_ATTRIB_LOCATION_ARB_PROC)(GLhandleARB programObj, const GLcharARB *name);

    #ifndef DECLARE_GL_ARB_vertex_shader
    #define DECLARE_GL_ARB_vertex_shader(LINKAGE) \
        LINKAGE PFN_BIND_ATTRIB_LOCATION_ARB_PROC glBindAttribLocationARB; \
        LINKAGE PFN_GET_ACTIVE_ATTRIB_ARB_PROC glGetActiveAttribARB; \
        LINKAGE PFN_GET_ATTRIB_LOCATION_ARB_PROC glGetAttribLocationARB; 
    #endif
#else
    #define DECLARE_GL_ARB_vertex_shader(LINKAGE)
#endif

#if defined(GL_ARB_fragment_shader)
    #define GL_FRAGMENT_SHADER_ARB					0x8B30
    #define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB	0x8B49
	#define GL_MAX_TEXTURE_COORDS_ARB				0x8871
	#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872
    #define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB	0x8B8B
#endif

#if defined(GL_ARB_geometry_shader4)
	#define GL_GEOMETRY_SHADER_ARB						0x8DD9
    #define GL_GEOMETRY_VERTICES_OUT_ARB				0x8DDA
    #define GL_GEOMETRY_INPUT_TYPE_ARB					0x8DDB
    #define GL_GEOMETRY_OUTPUT_TYPE_ARB					0x8DDC
    #define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB		0x8C29
    #define GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB		0x8DDD
    #define GL_MAX_VERTEX_VARYING_COMPONENTS_ARB		0x8DDE
    #define GL_MAX_VARYING_COMPONENTS					0x8B4B	// alias GL_MAX_VARYING_FLOATS
    #define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB		0x8DDF
    #define GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB			0x8DE0
    #define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB	0x8DE1
    #define GL_LINES_ADJACENCY_ARB						0xA
    #define GL_LINE_STRIP_ADJACENCY_ARB					0xB
    #define GL_TRIANGLES_ADJACENCY_ARB					0xC
    #define GL_TRIANGLE_STRIP_ADJACENCY_ARB				0xD
    #define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB	0x8DA8
    #define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB	0x8DA9
    #define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB		0x8DA7
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER		0x8CD4
    #define GL_PROGRAM_POINT_SIZE_ARB					0x8642

	typedef GLvoid (RAPTOR_APICALL * PFN_PROGRAM_PARAMETER_I_ARB_PROC)(GLuint program, GLuint pname, GLint value);
    typedef GLvoid (RAPTOR_APICALL * PFN_FRAMEBUFFER_TEXTURE_ARB_PROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
    typedef GLvoid (RAPTOR_APICALL * PFN_FRAMEBUFFER_TEXTURE_LAYER_ARB_PROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	typedef GLvoid (RAPTOR_APICALL * PFN_FRAMEBUFFER_TEXTURE_FACE_ARB_PROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);

    #ifndef DECLARE_GL_ARB_geometry_shader4
    #define DECLARE_GL_ARB_geometry_shader4(LINKAGE) \
        LINKAGE PFN_PROGRAM_PARAMETER_I_ARB_PROC glProgramParameteriARB; \
        LINKAGE PFN_FRAMEBUFFER_TEXTURE_ARB_PROC glFramebufferTextureARB; \
        LINKAGE PFN_FRAMEBUFFER_TEXTURE_LAYER_ARB_PROC glFramebufferTextureLayerARB; \
		LINKAGE PFN_FRAMEBUFFER_TEXTURE_FACE_ARB_PROC glFramebufferTextureFaceARB; 
    #endif
#else
    #define DECLARE_GL_ARB_geometry_shader4(LINKAGE)
#endif


#if defined(GL_ARB_draw_buffers)
    #define GL_MAX_DRAW_BUFFERS_ARB	0x8824
    #define GL_DRAW_BUFFER0_ARB		0x8825
    #define GL_DRAW_BUFFER1_ARB		0x8826
    #define GL_DRAW_BUFFER2_ARB		0x8827
    #define GL_DRAW_BUFFER3_ARB		0x8828
    #define GL_DRAW_BUFFER4_ARB		0x8829
    #define GL_DRAW_BUFFER5_ARB		0x882A
    #define GL_DRAW_BUFFER6_ARB		0x882B
    #define GL_DRAW_BUFFER7_ARB		0x882C
    #define GL_DRAW_BUFFER8_ARB		0x882D
    #define GL_DRAW_BUFFER9_ARB		0x882E
    #define GL_DRAW_BUFFER10_ARB	0x882F
    #define GL_DRAW_BUFFER11_ARB	0x8830
    #define GL_DRAW_BUFFER12_ARB	0x8831
    #define GL_DRAW_BUFFER13_ARB	0x8832
    #define GL_DRAW_BUFFER14_ARB	0x8833
    #define GL_DRAW_BUFFER15_ARB	0x8834

    typedef GLvoid (RAPTOR_APICALL * PFN_DRAW_BUFFERS_ARB)(GLsizei n, const GLenum *bufs);
    #ifndef DECLARE_GL_ARB_draw_buffers
    #define DECLARE_GL_ARB_draw_buffers(LINKAGE) \
        LINKAGE PFN_DRAW_BUFFERS_ARB glDrawBuffersARB;
    #endif
#else
    #define DECLARE_GL_ARB_draw_buffers(LINKAGE)
#endif


#if defined(GL_ARB_half_float_pixel)
	#define GL_HALF_FLOAT_ARB		0x140B
#endif


#if defined(GL_EXT_blend_color)
	#define GL_CONSTANT_COLOR_EXT             0x8001
	#define GL_ONE_MINUS_CONSTANT_COLOR_EXT   0x8002
	#define GL_CONSTANT_ALPHA_EXT             0x8003
	#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT   0x8004
	#define GL_BLEND_COLOR_EXT                0x8005

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLEND_COLOR_EXT_PROC)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	#ifndef DECLARE_GL_EXT_blend_color
	#define DECLARE_GL_EXT_blend_color(LINKAGE) \
		LINKAGE PFN_GL_BLEND_COLOR_EXT_PROC glBlendColorEXT;
	#endif
#else
	#define DECLARE_GL_EXT_blend_color(LINKAGE)
#endif


#if defined(GL_EXT_framebuffer_object)
	#define GL_FRAMEBUFFER_EXT									0x8D40
	#define GL_RENDERBUFFER_EXT									0x8D41
	#define GL_STENCIL_INDEX1_EXT								0x8D46
	#define GL_STENCIL_INDEX4_EXT								0x8D47
	#define GL_STENCIL_INDEX8_EXT								0x8D48
	#define GL_STENCIL_INDEX16_EXT								0x8D49
	#define GL_RENDERBUFFER_WIDTH_EXT							0x8D42
	#define GL_RENDERBUFFER_HEIGHT_EXT							0x8D43
	#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT					0x8D44
	#define GL_RENDERBUFFER_RED_SIZE_EXT						0x8D50
	#define GL_RENDERBUFFER_GREEN_SIZE_EXT						0x8D51
	#define GL_RENDERBUFFER_BLUE_SIZE_EXT						0x8D52
	#define GL_RENDERBUFFER_ALPHA_SIZE_EXT						0x8D53
	#define GL_RENDERBUFFER_DEPTH_SIZE_EXT						0x8D54
	#define GL_RENDERBUFFER_STENCIL_SIZE_EXT					0x8D55
	#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT			0x8CD0
	#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT			0x8CD1
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT			0x8CD2
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT	0x8CD3
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT	0x8CD4
	#define GL_COLOR_ATTACHMENT0_EXT							0x8CE0
	#define GL_COLOR_ATTACHMENT1_EXT							0x8CE1
	#define GL_COLOR_ATTACHMENT2_EXT							0x8CE2
	#define GL_COLOR_ATTACHMENT3_EXT							0x8CE3
	#define GL_COLOR_ATTACHMENT4_EXT							0x8CE4
	#define GL_COLOR_ATTACHMENT5_EXT							0x8CE5
	#define GL_COLOR_ATTACHMENT6_EXT							0x8CE6
	#define GL_COLOR_ATTACHMENT7_EXT							0x8CE7
	#define GL_COLOR_ATTACHMENT8_EXT							0x8CE8
	#define GL_COLOR_ATTACHMENT9_EXT							0x8CE9
	#define GL_COLOR_ATTACHMENT10_EXT							0x8CEA
	#define GL_COLOR_ATTACHMENT11_EXT							0x8CEB
	#define GL_COLOR_ATTACHMENT12_EXT							0x8CEC
	#define GL_COLOR_ATTACHMENT13_EXT							0x8CED
	#define GL_COLOR_ATTACHMENT14_EXT							0x8CEE
	#define GL_COLOR_ATTACHMENT15_EXT							0x8CEF
	#define GL_DEPTH_ATTACHMENT_EXT								0x8D00
	#define GL_STENCIL_ATTACHMENT_EXT							0x8D20
	#define GL_FRAMEBUFFER_COMPLETE_EXT							0x8CD5
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT			0x8CD6
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT	0x8CD7
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT			0x8CD9
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT				0x8CDA
	#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT			0x8CDB
	#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT			0x8CDC
	#define GL_FRAMEBUFFER_UNSUPPORTED_EXT						0x8CDD
	#define GL_FRAMEBUFFER_BINDING_EXT							0x8CA6
	#define GL_RENDERBUFFER_BINDING_EXT							0x8CA7
	#define GL_MAX_COLOR_ATTACHMENTS_EXT						0x8CDF
	#define GL_MAX_RENDERBUFFER_SIZE_EXT						0x84E8
	#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT				0x0506

	typedef GLboolean (RAPTOR_APICALL * PFN_GL_IS_RENDER_BUFFER_EXT_PROC)(GLuint renderbuffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BIND_RENDER_BUFFER_EXT_PROC)(GLenum target, GLuint renderbuffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_DELETE_RENDER_BUFFERS_EXT_PROC)(GLsizei n, const GLuint *renderbuffers);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GEN_RENDER_BUFFERS_EXT_PROC)(GLsizei n, GLuint *renderbuffers);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_RENDER_BUFFER_STORAGE_EXT_PROC)(GLenum target, GLenum internalformat,GLsizei width, GLsizei height);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_RENDER_BUFFER_PARAMETER_IV_EXT_PROC)(GLenum target, GLenum pname, GLint *params);
	typedef GLboolean (RAPTOR_APICALL * PFN_GL_IS_FRAME_BUFFER_EXT_PROC)(GLuint framebuffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BIND_FRAME_BUFFER_EXT_PROC)(GLenum target, GLuint framebuffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_DELETE_FRAME_BUFFERS_EXT_PROC)(GLsizei n, const GLuint *framebuffers);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GEN_FRAME_BUFFERS_EXT_PROC)(GLsizei n, GLuint *framebuffers);
	typedef GLenum (RAPTOR_APICALL * PFN_GL_CHECK_FRAME_BUFFER_STATUS_EXT_PROC)(GLenum target);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FRAME_BUFFER_TEXTURE_1D_EXT_PROC)(GLenum target, GLenum attachment,GLenum textarget, GLuint texture,GLint level);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FRAME_BUFFER_TEXTURE_2D_EXT_PROC)(GLenum target, GLenum attachment,GLenum textarget, GLuint texture,GLint level);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FRAME_BUFFER_TEXTURE_3D_EXT_PROC)(GLenum target, GLenum attachment,GLenum textarget, GLuint texture,GLint level, GLint zoffset);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_FRAME_BUFFER_RENDER_BUFFER_EXT_PROC)(GLenum target, GLenum attachment,GLenum renderbuffertarget, GLuint renderbuffer);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_FRAME_BUFFER_ATTACHMENT_PARAMETER_IV_EXT_PROC)(GLenum target, GLenum attachment,GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GENERATE_MIPMAP_EXT_PROC)(GLenum target);

	#ifndef DECLARE_GL_EXT_framebuffer_object
	#define DECLARE_GL_EXT_framebuffer_object(LINKAGE) \
		LINKAGE PFN_GL_IS_RENDER_BUFFER_EXT_PROC glIsRenderbufferEXT; \
		LINKAGE PFN_GL_BIND_RENDER_BUFFER_EXT_PROC glBindRenderbufferEXT; \
		LINKAGE PFN_GL_DELETE_RENDER_BUFFERS_EXT_PROC glDeleteRenderbuffersEXT; \
		LINKAGE PFN_GL_GEN_RENDER_BUFFERS_EXT_PROC glGenRenderbuffersEXT; \
		LINKAGE PFN_GL_RENDER_BUFFER_STORAGE_EXT_PROC glRenderbufferStorageEXT; \
		LINKAGE PFN_GL_GET_RENDER_BUFFER_PARAMETER_IV_EXT_PROC glGetRenderbufferParameterivEXT; \
		LINKAGE PFN_GL_IS_FRAME_BUFFER_EXT_PROC glIsFramebufferEXT; \
		LINKAGE PFN_GL_BIND_FRAME_BUFFER_EXT_PROC glBindFramebufferEXT; \
		LINKAGE PFN_GL_DELETE_FRAME_BUFFERS_EXT_PROC glDeleteFramebuffersEXT; \
		LINKAGE PFN_GL_GEN_FRAME_BUFFERS_EXT_PROC glGenFramebuffersEXT; \
		LINKAGE PFN_GL_CHECK_FRAME_BUFFER_STATUS_EXT_PROC glCheckFramebufferStatusEXT; \
		LINKAGE PFN_GL_FRAME_BUFFER_TEXTURE_1D_EXT_PROC glFramebufferTexture1DEXT; \
		LINKAGE PFN_GL_FRAME_BUFFER_TEXTURE_2D_EXT_PROC glFramebufferTexture2DEXT; \
		LINKAGE PFN_GL_FRAME_BUFFER_TEXTURE_3D_EXT_PROC glFramebufferTexture3DEXT; \
		LINKAGE PFN_GL_FRAME_BUFFER_RENDER_BUFFER_EXT_PROC glFramebufferRenderbufferEXT; \
		LINKAGE PFN_GL_GET_FRAME_BUFFER_ATTACHMENT_PARAMETER_IV_EXT_PROC glGetFramebufferAttachmentParameterivEXT; \
		LINKAGE PFN_GL_GENERATE_MIPMAP_EXT_PROC glGenerateMipmapEXT;
	#endif
#else
	#define DECLARE_GL_EXT_framebuffer_object(LINKAGE)
#endif

#if defined(GL_EXT_framebuffer_blit)
	#define GL_READ_FRAMEBUFFER_EXT			0x8CA8
	#define GL_DRAW_FRAMEBUFFER_EXT			0x8CA9
	#define GL_DRAW_FRAMEBUFFER_BINDING_EXT 0x8CA6 // alias FRAMEBUFFER_BINDING_EXT
	#define GL_READ_FRAMEBUFFER_BINDING_EXT 0x8CAA

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLIT_FRAME_BUFFER_EXT_PROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
																		GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
																		GLbitfield mask, GLenum filter);
	#ifndef DECLARE_GL_EXT_framebuffer_blit
	#define DECLARE_GL_EXT_framebuffer_blit(LINKAGE) \
		LINKAGE PFN_GL_BLIT_FRAME_BUFFER_EXT_PROC glBlitFrameBufferEXT;
	#endif
#else
	#define DECLARE_GL_EXT_framebuffer_blit(LINKAGE)
#endif

#if defined(GL_EXT_framebuffer_multisample)
	#define GL_RENDERBUFFER_SAMPLES_EXT					0x8CAB
	#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT	0x8D56
	#define GL_MAX_SAMPLES_EXT							0x8D57

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_RENDER_BUFFER_STORAGE_MULTISAMPLE_EXT_PROC)(GLenum target, GLsizei samples,
																						GLenum internalformat,
																						GLsizei width, GLsizei height);
	#ifndef DECLARE_GL_EXT_framebuffer_multisample
	#define DECLARE_GL_EXT_framebuffer_multisample(LINKAGE) \
		LINKAGE PFN_GL_RENDER_BUFFER_STORAGE_MULTISAMPLE_EXT_PROC glRenderbufferStorageMultisampleEXT;
	#endif
#else
	#define DECLARE_GL_EXT_framebuffer_multisample(LINKAGE)
#endif


#if defined(GL_NV_float_buffer)
	#define GL_FLOAT_R_NV					0x8880
	#define GL_FLOAT_RG_NV					0x8881
	#define GL_FLOAT_RGB_NV					0x8882
	#define GL_FLOAT_RGBA_NV				0x8883
	#define GL_FLOAT_R16_NV					0x8884
	#define GL_FLOAT_R32_NV					0x8885
	#define GL_FLOAT_RG16_NV				0x8886
	#define GL_FLOAT_RG32_NV				0x8887
	#define GL_FLOAT_RGB16_NV				0x8888
	#define GL_FLOAT_RGB32_NV				0x8889
	#define GL_FLOAT_RGBA16_NV				0x888A
	#define GL_FLOAT_RGBA32_NV				0x888B
	#define GL_TEXTURE_FLOAT_COMPONENTS_NV	0x888C
	#define GL_FLOAT_CLEAR_COLOR_VALUE_NV	0x888D
	#define GL_FLOAT_RGBA_MODE_NV			0x888E
	#define GL_WGL_FLOAT_COMPONENTS_NV		0x20B0
	#define GL_WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV		0x20B1
	#define GL_WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RG_NV	0x20B2
	#define GL_WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV	0x20B3
	#define GL_WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV	0x20B4
	#define GL_WGL_TEXTURE_FLOAT_R_NV		0x20B5
	#define GL_WGL_TEXTURE_FLOAT_RG_NV		0x20B6
	#define GL_WGL_TEXTURE_FLOAT_RGB_NV		0x20B7
	#define GL_WGL_TEXTURE_FLOAT_RGBA_NV	0x20B8
#endif


#if defined(GL_EXT_histogram)
	#define GL_HISTOGRAM_EXT                  0x8024
	#define GL_PROXY_HISTOGRAM_EXT            0x8025
	#define GL_HISTOGRAM_WIDTH_EXT            0x8026
	#define GL_HISTOGRAM_FORMAT_EXT           0x8027
	#define GL_HISTOGRAM_RED_SIZE_EXT         0x8028
	#define GL_HISTOGRAM_GREEN_SIZE_EXT       0x8029
	#define GL_HISTOGRAM_BLUE_SIZE_EXT        0x802A
	#define GL_HISTOGRAM_ALPHA_SIZE_EXT       0x802B
	#define GL_HISTOGRAM_LUMINANCE_SIZE_EXT   0x802C
	#define GL_HISTOGRAM_SINK_EXT             0x802D
	#define GL_MINMAX_EXT                     0x802E
	#define GL_MINMAX_FORMAT_EXT              0x802F
	#define GL_MINMAX_SINK_EXT                0x8030
	#define GL_TABLE_TOO_LARGE_EXT            0x8031

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_HISTOGRAM_EXT_PROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_HISTOGRAM_PARAMETER_FV_EXT_PROC) (GLenum target, GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_HISTOGRAM_PARAMETER_IV_EXT_PROC) (GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_MINMAX_EXT_PROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_MINMAX_PARAMETER_FV_EXT_PROC) (GLenum target, GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_MINMAX_PARAMETER_IV_EXT_PROC) (GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_HISTOGRAM_EXT_PROC) (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_MINMAX_EXT_PROC) (GLenum target, GLenum internalformat, GLboolean sink);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_RESET_HISTOGRAM_EXT_PROC) (GLenum target);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_RESET_MINMAX_EXT_PROC) (GLenum target);

	#ifndef DECLARE_GL_EXT_histogram
	#define DECLARE_GL_EXT_histogram(LINKAGE) \
		LINKAGE PFN_GL_GET_HISTOGRAM_EXT_PROC glGetHistogramEXT;\
		LINKAGE PFN_GL_GET_HISTOGRAM_PARAMETER_FV_EXT_PROC glGetHistogramParameterfvEXT;\
		LINKAGE PFN_GL_GET_HISTOGRAM_PARAMETER_IV_EXT_PROC glGetHistogramParameterivEXT;\
		LINKAGE PFN_GL_GET_MINMAX_EXT_PROC glGetMinmaxEXT;\
		LINKAGE PFN_GL_GET_MINMAX_PARAMETER_FV_EXT_PROC glGetMinmaxParameterfvEXT;\
		LINKAGE PFN_GL_GET_MINMAX_PARAMETER_IV_EXT_PROC glGetMinmaxParameterivEXT;\
		LINKAGE PFN_GL_HISTOGRAM_EXT_PROC glHistogramEXT;\
		LINKAGE PFN_GL_MINMAX_EXT_PROC glMinmaxEXT;\
		LINKAGE PFN_GL_RESET_HISTOGRAM_EXT_PROC glResetHistogramEXT;\
		LINKAGE PFN_GL_RESET_MINMAX_EXT_PROC glResetMinmaxEXT;
	#endif
#else
	#define DECLARE_GL_EXT_histogram(LINKAGE)
#endif

#if defined(GL_EXT_convolution)
	#define GL_CONVOLUTION_1D_EXT             0x8010
	#define GL_CONVOLUTION_2D_EXT             0x8011
	#define GL_SEPARABLE_2D_EXT               0x8012
	#define GL_CONVOLUTION_BORDER_MODE_EXT    0x8013
	#define GL_CONVOLUTION_FILTER_SCALE_EXT   0x8014
	#define GL_CONVOLUTION_FILTER_BIAS_EXT    0x8015
	#define GL_REDUCE_EXT                     0x8016
	#define GL_CONVOLUTION_FORMAT_EXT         0x8017
	#define GL_CONVOLUTION_WIDTH_EXT          0x8018
	#define GL_CONVOLUTION_HEIGHT_EXT         0x8019
	#define GL_MAX_CONVOLUTION_WIDTH_EXT      0x801A
	#define GL_MAX_CONVOLUTION_HEIGHT_EXT     0x801B
	#define GL_POST_CONVOLUTION_RED_SCALE_EXT 0x801C
	#define GL_POST_CONVOLUTION_GREEN_SCALE_EXT 0x801D
	#define GL_POST_CONVOLUTION_BLUE_SCALE_EXT 0x801E
	#define GL_POST_CONVOLUTION_ALPHA_SCALE_EXT 0x801F
	#define GL_POST_CONVOLUTION_RED_BIAS_EXT  0x8020
	#define GL_POST_CONVOLUTION_GREEN_BIAS_EXT 0x8021
	#define GL_POST_CONVOLUTION_BLUE_BIAS_EXT 0x8022
	#define GL_POST_CONVOLUTION_ALPHA_BIAS_EXT 0x8023

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_FILTER_1D_EXT_PROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_FILTER_2D_EXT_PROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_PARAMETER_F_EXT_PROC) (GLenum target, GLenum pname, GLfloat params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_PARAMETER_FV_EXT_PROC) (GLenum target, GLenum pname, const GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_PARAMETER_I_EXT_PROC) (GLenum target, GLenum pname, GLint params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_CONVOLUTION_PARAMETER_IV_EXT_PROC) (GLenum target, GLenum pname, const GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COPY_CONVOLUTION_FILTER_1D_EXT_PROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_COPY_CONVOLUTION_FILTER_2D_EXT_PROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_CONVOLUTION_FILTER_EXT_PROC) (GLenum target, GLenum format, GLenum type, GLvoid *image);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_CONVOLUTION_PARAMETER_FV_EXT_PROC) (GLenum target, GLenum pname, GLfloat *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_CONVOLUTION_PARAMETER_IV_EXT_PROC) (GLenum target, GLenum pname, GLint *params);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_GET_SEPARABLE_FILTER_EXT_PROC) (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
	typedef GLvoid (RAPTOR_APICALL * PFN_GL_SEPARABLE_FILTER_2D_EXT_PROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);

	#ifndef DECLARE_GL_EXT_convolution
	#define DECLARE_GL_EXT_convolution(LINKAGE) \
		LINKAGE PFN_GL_CONVOLUTION_FILTER_1D_EXT_PROC glConvolutionFilter1DEXT;\
		LINKAGE PFN_GL_CONVOLUTION_FILTER_2D_EXT_PROC glConvolutionFilter2DEXT;\
		LINKAGE PFN_GL_CONVOLUTION_PARAMETER_F_EXT_PROC glConvolutionParameterfEXT;\
		LINKAGE PFN_GL_CONVOLUTION_PARAMETER_FV_EXT_PROC glConvolutionParameterfvEXT;\
		LINKAGE PFN_GL_CONVOLUTION_PARAMETER_I_EXT_PROC glConvolutionParameteriEXT;\
		LINKAGE PFN_GL_CONVOLUTION_PARAMETER_IV_EXT_PROC glConvolutionParameterivEXT;\
		LINKAGE PFN_GL_COPY_CONVOLUTION_FILTER_1D_EXT_PROC glCopyConvolutionFilter1DEXT;\
		LINKAGE PFN_GL_COPY_CONVOLUTION_FILTER_2D_EXT_PROC glCopyConvolutionFilter2DEXT;\
		LINKAGE PFN_GL_GET_CONVOLUTION_FILTER_EXT_PROC glGetConvolutionFilterEXT;\
		LINKAGE PFN_GL_GET_CONVOLUTION_PARAMETER_FV_EXT_PROC glGetConvolutionParameterfvEXT;\
		LINKAGE PFN_GL_GET_CONVOLUTION_PARAMETER_IV_EXT_PROC glGetConvolutionParameterivEXT;\
		LINKAGE PFN_GL_GET_SEPARABLE_FILTER_EXT_PROC glGetSeparableFilterEXT;\
		LINKAGE PFN_GL_SEPARABLE_FILTER_2D_EXT_PROC glSeparableFilter2DEXT;
	#endif
#else
	#define DECLARE_GL_EXT_convolution(LINKAGE)
#endif

#if defined(GL_EXT_cmyka)
	#define GL_CMYK_EXT                       0x800C
	#define GL_CMYKA_EXT                      0x800D
	#define GL_PACK_CMYK_HINT_EXT             0x800E
	#define GL_UNPACK_CMYK_HINT_EXT           0x800F
#endif


#if defined(GL_EXT_rescale_normal)
	#define GL_RESCALE_NORMAL_EXT             0x803A
#endif

#if defined(GL_WIN_phong_shading)
	#define GL_PHONG_WIN                      0x80EA
	#define GL_PHONG_HINT_WIN                 0x80EB
#endif

#if defined(GL_WIN_specular_fog)
	#define GL_FOG_SPECULAR_TEXTURE_WIN       0x80EC
#endif

#if defined(GL_EXT_blend_func_separate)
	#define GL_BLEND_DST_RGB_EXT              0x80C8
	#define GL_BLEND_SRC_RGB_EXT              0x80C9
	#define GL_BLEND_DST_ALPHA_EXT            0x80CA
	#define GL_BLEND_SRC_ALPHA_EXT            0x80CB

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLEND_FUNC_SEPARATE_EXT_PROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

	#ifndef DECLARE_GL_EXT_blend_func_separate
	#define DECLARE_GL_EXT_blend_func_separate(LINKAGE) \
		LINKAGE PFN_GL_BLEND_FUNC_SEPARATE_EXT_PROC glBlendFuncSeparateEXT;
	#endif
#else
	#define DECLARE_GL_EXT_blend_func_separate(LINKAGE)
#endif


#if defined(GL_EXT_blend_equation_separate)
	#define GL_BLEND_EQUATION_RGB_EXT         GL_BLEND_EQUATION
	#define GL_BLEND_EQUATION_ALPHA_EXT       0x883D

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_BLEND_EQUATION_SEPARATE_EXT_PROC) (GLenum modeRGB, GLenum modeAlpha);

	#ifndef DECLARE_GL_EXT_blend_equation_separate
	#define DECLARE_GL_EXT_blend_equation_separate(LINKAGE) \
		LINKAGE PFN_GL_BLEND_EQUATION_SEPARATE_EXT_PROC glBlendEquationSeparateEXT;
	#endif
#else
	#define DECLARE_GL_EXT_blend_equation_separate(LINKAGE)
#endif


#if defined(GL_EXT_texture_buffer_object)
	#define GL_TEXTURE_BUFFER_EXT						0x8C2A
	#define GL_MAX_TEXTURE_BUFFER_SIZE_EXT				0x8C2B
	#define GL_TEXTURE_BINDING_BUFFER_EXT				0x8C2C
	#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT	0x8C2D
	#define GL_TEXTURE_BUFFER_FORMAT_EXT				0x8C2E

	typedef GLvoid (RAPTOR_APICALL * PFN_GL_TEX_BUFFER_EXT_PROC) (GLenum target, GLenum internalformat, GLuint buffer);

	#ifndef DECLARE_GL_EXT_texture_buffer_object
	#define DECLARE_GL_EXT_texture_buffer_object(LINKAGE) \
		LINKAGE PFN_GL_TEX_BUFFER_EXT_PROC glTexBufferEXT;
	#endif
#else
	#define DECLARE_GL_EXT_texture_buffer_object(LINKAGE)
#endif

#if defined(GL_EXT_texture_compression_rgtc)
	#define GL_COMPRESSED_RED_RGTC1_EXT       0x8DBB
	#define GL_COMPRESSED_SIGNED_RED_RGTC1_EXT 0x8DBC
	#define GL_COMPRESSED_RED_GREEN_RGTC2_EXT 0x8DBD
	#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT 0x8DBE
#endif

#if defined(GL_EXT_packed_depth_stencil)
	#define GL_DEPTH_STENCIL_EXT		0x84F9
	#define GL_UNSIGNED_INT_24_8_EXT	0x84FA
	#define GL_DEPTH24_STENCIL8_EXT		0x88F0
	#define GL_TEXTURE_STENCIL_SIZE_EXT	0x88F1
#endif

#if defined(GL_EXT_cmyka)
	#define GL_CMYK_EXT                0x800C
	#define GL_CMYKA_EXT               0x800D
	#define GL_PACK_CMYK_HINT_EXT      0x800E
	#define GL_UNPACK_CMYK_HINT_EXT    0x800F
#endif

#if (defined(GL_KHR_texture_compression_astc_hdr) || defined(GL_KHR_texture_compression_astc_ldr))
	#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR            0x93B0
	#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR            0x93B1
	#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR            0x93B2
	#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR            0x93B3
	#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR            0x93B4
	#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR            0x93B5
	#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR            0x93B6
	#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR            0x93B7
	#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR           0x93B8
	#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR           0x93B9
	#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR           0x93BA
	#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR          0x93BB
	#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR          0x93BC
	#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR          0x93BD

	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR    0x93D0
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR    0x93D1
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR    0x93D2
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR    0x93D3
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR    0x93D4
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR    0x93D5
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR    0x93D6
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR    0x93D7
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR   0x93D8
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR   0x93D9
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR   0x93DA
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR  0x93DB
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR  0x93DC
	#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR  0x93DD
#endif



#if defined(GL_ARB_uniform_buffer_object)
	#define GL_UNIFORM_BUFFER_ARB								0x8A11
	#define GL_UNIFORM_BUFFER_BINDING_ARB						0x8A28
	#define GL_UNIFORM_BUFFER_START_ARB							0x8A29
	#define GL_UNIFORM_BUFFER_SIZE_ARB							0x8A2A
	#define GL_MAX_VERTEX_UNIFORM_BLOCKS_ARB					0x8A2B
	#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_ARB					0x8A2C
	#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS_ARB					0x8A2D
	#define GL_MAX_COMBINED_UNIFORM_BLOCKS_ARB					0x8A2E
	#define GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB					0x8A2F
	#define GL_MAX_UNIFORM_BLOCK_SIZE_ARB						0x8A30
	#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS_ARB		0x8A31
	#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_ARB		0x8A32
	#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS_ARB		0x8A33
	#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT_ARB				0x8A34
	#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB			0x8A35
	#define GL_ACTIVE_UNIFORM_BLOCKS_ARB						0x8A36
	#define GL_UNIFORM_TYPE_ARB									0x8A37
	#define GL_UNIFORM_SIZE_ARB									0x8A38
	#define GL_UNIFORM_NAME_LENGTH_ARB							0x8A39
	#define GL_UNIFORM_BLOCK_INDEX_ARB							0x8A3A
	#define GL_UNIFORM_OFFSET_ARB								0x8A3B
	#define GL_UNIFORM_ARRAY_STRIDE_ARB							0x8A3C
	#define GL_UNIFORM_MATRIX_STRIDE_ARB						0x8A3D
	#define GL_UNIFORM_IS_ROW_MAJOR_ARB							0x8A3E
	#define GL_UNIFORM_BLOCK_BINDING_ARB						0x8A3F
	#define GL_UNIFORM_BLOCK_DATA_SIZE_ARB						0x8A40
	#define GL_UNIFORM_BLOCK_NAME_LENGTH_ARB					0x8A41
	#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB				0x8A42
	#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES_ARB			0x8A43
	#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER_ARB	0x8A44
	#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER_ARB	0x8A45
	#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER_ARB	0x8A46
	#define GL_INVALID_INDEX_ARB								0xFFFFFFFFu

	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_UNIFORM_INDICES_ARB_PROC)(GLuint program, GLsizei uniformCount, const char* const * uniformNames, GLuint* uniformIndices);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_ACTIVE_UNIFORMS_IV_ARB_PROC)(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_ACTIVE_UNIFORM_NAME_ARB_PROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, char* uniformName);
	typedef GLuint(RAPTOR_APICALL * PFN_GL_GET_UNIFORM_BLOCK_INDEX_ARB_PROC)(GLuint program,const char* uniformBlockName);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_IV_ARB_PROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_NAME_ARB_PROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, char* uniformBlockName);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_BIND_BUFFER_RANGE_ARB_PROC)(GLenum target, GLuint index, GLuint buffer, GLintptrARB offset, GLsizeiptrARB size);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_BIND_BUFFER_BASE_ARB_PROC)(GLenum target, GLuint index, GLuint buffer);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_GET_INTEGERI_V_ARB_PROC)(GLenum target, GLuint index, GLint* data);
	typedef GLvoid(RAPTOR_APICALL * PFN_GL_UNIFORM_BLOCK_BINDING_ARB_PROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

	#ifndef DECLARE_GL_ARB_uniform_buffer_object
	#define DECLARE_GL_ARB_uniform_buffer_object(LINKAGE) \
		LINKAGE PFN_GL_GET_UNIFORM_INDICES_ARB_PROC glGetUniformIndicesARB; \
		LINKAGE PFN_GL_GET_ACTIVE_UNIFORMS_IV_ARB_PROC glGetActiveUniformsivARB; \
		LINKAGE PFN_GL_GET_ACTIVE_UNIFORM_NAME_ARB_PROC glGetActiveUniformNameARB; \
		LINKAGE PFN_GL_GET_UNIFORM_BLOCK_INDEX_ARB_PROC glGetUniformBlockIndexARB; \
		LINKAGE PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_IV_ARB_PROC glGetActiveUniformBlockivARB; \
		LINKAGE PFN_GL_GET_ACTIVE_UNIFORM_BLOCK_NAME_ARB_PROC glGetActiveUniformBlockNameARB; \
		LINKAGE PFN_GL_BIND_BUFFER_RANGE_ARB_PROC glBindBufferRangeARB; \
		LINKAGE PFN_GL_BIND_BUFFER_BASE_ARB_PROC glBindBufferBaseARB; \
		LINKAGE PFN_GL_GET_INTEGERI_V_ARB_PROC glGetIntegeri_vARB; \
		LINKAGE PFN_GL_UNIFORM_BLOCK_BINDING_ARB_PROC glUniformBlockBindingARB;
	#endif
#else
	#define DECLARE_GL_ARB_uniform_buffer_object(LINKAGE)
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! WIN32 SPECIFIC
//!
#ifdef WIN32
	#ifndef __WIN32_RAPTOR_GLEXT_H__
		#include "Win32Specific/Win32Glext.h"
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! GLX SPECIFIC
//!
#ifdef GLX_EXTENSIONS
	#ifndef __GLX_RAPTOR_GLEXT_H__
		#include "GLXSpecific/GLXGlext.h"
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __glext_h_ */

