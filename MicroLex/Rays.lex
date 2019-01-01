# Micro lex description file
#
# Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*

atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.]*["]

atom:in={
atom:out=}
atom:sep=,
atom:set==

atom:width=width
atom:height=height
atom:variance=variance
atom:deflection=deflection
atom:defraction=defraction
atom:crease=crease

atom:separate_specular=separate_specular
atom:none=none

atom:vertex=vertex
atom:shading=shading
atom:rendering=rendering
atom:light=light
atom:texture=texture
atom:triangle=triangle

atom:geometry=geometry
atom:mesh=mesh
atom:import=import

map:integer=int
map:float=float
map:string=CString

#
# Rules definitions
#

sequence:coord=<in><float><sep><float><sep><float><sep><float><out>

# A vertex is defined by
# - a position coordinate
# - a texel coordinate
sequence:vertex=<in>{coord}<sep>{coord}<out>

# A triangle is defined by 3 vertex indices
sequence:triangle=<in><integer><sep><integer><sep><integer><out>

# A shading is :
# - ambient		[0.0 .. 1.0]
# - diffuse		[0.0 .. 1.0]
# - specular	[0.0 .. 1.0]
# - exponent	> 0
# - reflection	[0.0 .. 1.0]
# - refraction	[0.0 .. 1.0]
# - light speed	> 0
# - shadingoptions
alternate:shadingoption=<separate_specular><none>
sequence:shadingoptions={shadingoption}{shadingoptions}
sequence:shading=<in><float><sep><float><sep><float><sep><integer><sep><float><sep><float><sep><float><sep>{shadingoptions}<out>

sequence:light=<in>{coord}<sep>{coord}<sep><float><out>
sequence:texture=<in><string><out>

sequence:renderoption=<in><out>
sequence:renderoptions={renderoption}{renderoptions}
sequence:render=<in>{renderoptions}<out>

alternate:parameter=<width><height><variance><deflection><defraction><crease>
# If elements contain rules, they must be compatible
alternate:element={shading}{light}{texture}{render}
alternate:elementname=<shading><light><texture><rendering>

sequence:define={elementname}<string><set>{element}{define}
sequence:config={parameter}<set><integer>{config}
sequence:triangles={triangle}{triangles}
sequence:vertices={vertex}{vertices}

# Objects definitions
# objectname
# {
#	shading,
#	texture,
#	base default color,
sequence:objectbase=<string><in><string><sep><string><sep>{coord}<sep>

sequence:composition={objectbase}<geometry><in>{vertices}<out><mesh><in>{triangles}<out><out>
sequence:sphere={objectbase}{coord}<sep><float><out>
sequence:extern={objectbase}<import><string><out>
alternate:object={composition}{sphere}{extern}
sequence:objects={object}{objects}

# Main rule
sequence:main=<string><in><in>{config}<out><in>{define}<out><in>{objects}<out><out>