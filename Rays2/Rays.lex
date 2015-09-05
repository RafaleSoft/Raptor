// Micro lex description file
//
// Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*

atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.]*["]
atom:linecomment=[/][/][^\$]*
comment:linecomment

atom:in={
atom:out=}
atom:pin=(
atom:pout=)
atom:sep=,
atom:set==

atom:width=width
atom:height=height
atom:variance=variance
atom:deflection=deflection
atom:defraction=defraction
atom:crease=crease
atom:focale=focale
atom:object_plane=object_plane
atom:plugin=plugin

atom:separate_specular=separate_specular
atom:spot=spot
atom:spot_cos=spot_cos
atom:spot_exp=spot_exp
atom:photon_map=photon_map
atom:none=none

atom:vertex=vertex
atom:config=config
atom:scene=scene
atom:frame=frame
atom:shading=shading
atom:light=light
atom:texture=texture
atom:triangle=triangle
atom:environment=environment

atom:geometry=geometry
atom:scale=scale
atom:translate=translate
atom:rotate=rotate
atom:mesh=mesh
atom:import=import
atom:call=call

map:integer=int
map:float=float
map:string=CString

//
// Rules definitions
//

sequence:coord=<in><float><sep><float><sep><float><sep><float><out>

// A vertex is defined by
// - a position coordinate
// - a texel coordinate
sequence:vertex=<in>{coord}<sep>{coord}<out>

// A triangle is defined by 3 vertex indices
sequence:triangle=<in><integer><sep><integer><sep><integer><out>

// A shading is :
// - ambient	[0.0 .. 1.0]
// - diffuse	[0.0 .. 1.0]
// - specular	[0.0 .. 1.0]
// - exponent	> 0
// - reflection	[0.0 .. 1.0]
// - refraction	[0.0 .. 1.0]
// - light speed	> 0
// - shadingoptions
alternate:shadingoption=<separate_specular><none>
sequence:shadingoptions={shadingoption}{shadingoptions}
sequence:shading=<in><float><sep><float><sep><float><sep><integer><sep><float><sep><float><sep><float><sep>{shadingoptions}<out>

// A light is :
// - position coordinate
// - a color
// - an intensity [0.0 .. 1.0]
// - a direction ( target coordinate )
// - a cutoff value
alternate:light_model=<spot><spot_cos><spot_exp><photon_map><none>
sequence:light_models={light_model}{light_models}
sequence:light=<in>{coord}<sep>{coord}<sep><float><sep>{coord}<sep><float><sep>{light_models}<out>
sequence:texture=<in><string><out>

alternate:parameter=<width><height><variance><deflection><defraction><crease><focale><object_plane><photon_map>
// If elements contain rules, they must be compatible
alternate:element={shading}{light}{texture}
alternate:elementname=<shading><light><texture><environment>

sequence:define={elementname}<string><set>{element}{define}
alternate:configparam=<integer><float>
sequence:config={parameter}<set>{configparam}{config}
sequence:triangles={triangle}{triangles}
sequence:vertices={vertex}{vertices}

//	Functions
//
//	Variable length parameter lists:
//	( )
//	( param )
//	( param1,param2,param3 ... paramn )
//alternate:vl_parameter=<float><string><integer>
alternate:vl_parameter=<float><integer>
forward:functionparameter_item
sequence:functionparameter_lastitem={vl_parameter}<pout>
sequence:functionparameter_listitem={vl_parameter}<sep>{functionparameter_item}
alternate:functionparameter_item={functionparameter_listitem}{functionparameter_lastitem}
sequence:functionparameter_noitem=<pout>
alternate:functionparameter={functionparameter_noitem}{functionparameter_listitem}{functionparameter_lastitem}

sequence:plugin_call=<call><string><pin>{functionparameter}

//	objects operations
alternate:transform_type=<scale><rotate><translate>
sequence:transform={transform_type}<string><pin><float><sep><float><sep><float><sep><float><pout>
//sequence:transforms={transform}{transforms}

// Objects definitions : 
// objectname
// {
//	shading,
//	diffuse texture,
//	normal map
//	base default color,
sequence:name=<string>
alternate:normal_specifier={name}{plugin_call}
sequence:objectbase=<string><in><string><sep><string><sep>{normal_specifier}<sep>{coord}<sep>

sequence:composition={objectbase}<geometry><in>{vertices}<out><mesh><in>{triangles}<out><out>
sequence:sphere={objectbase}{coord}<sep><float><out>
sequence:extern={objectbase}<import><string><out>
alternate:object={composition}{sphere}{extern}
sequence:objects={object}{objects}


// Rendering :
// the rendering is composed of a sequence of frames,
// each frame defining the scene behaviour
alternate:frame_element={transform}{plugin_call}
sequence:frame_elements={frame_element}{frame_elements}
//sequence:frame=<frame><in>{frame_elements}<out>
//sequence:frames={frame}{frames}
sequence:frames=<frame><in>{frame_elements}<out>{frames}

// Main rule
sequence:main=<string><in><config><in>{config}<out><shading><in>{define}<out><scene><in>{objects}<out>{frames}<out>