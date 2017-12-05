# Micro lex description file
#
# Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*

atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*["]

atom:in={
atom:out=}
atom:sep=,
atom:set==

atom:width=width
atom:height=height
atom:variance=variance
atom:deflection=deflection
atom:defraction=defraction

atom:vertex=vertex
atom:shading=shading
atom:light=light
atom:texture=texture
atom:triangle=triangle

atom:geometry=geometry
atom:mesh=mesh

# Rules definitions

sequence:coord=<in><float><sep><float><sep><float><sep><float><out>

sequence:vertex=<in>{coord}<sep>{coord}<out>
sequence:triangle=<in><integer><sep><integer><sep><integer><out>
sequence:shading=<in><float><sep><float><sep><float><sep><integer><sep><float><sep><float><sep><float><out>
sequence:light=<in>{coord}<sep>{coord}<sep><float><out>
sequence:texture=<in><string><out>

alternate:parameter=<width><height><variance><deflection><defraction>
# If elements contain rules, they must be compatible
alternate:element={shading}{light}{texture}
alternate:elementname=<shading><light><texture>

sequence:define={elementname}<string><set>{element}{define}
sequence:config={parameter}<set><integer>{config}
sequence:triangles={triangle}{triangles}
sequence:vertices={vertex}{vertices}

# Objects definitions
sequence:objectbase=<string><in><string><sep><string><sep>{coord}<sep>

sequence:composition={objectbase}<geometry><in>{vertices}<out><mesh><in>{triangles}<out><out>
sequence:sphere={objectbase}{coord}<sep><float><out>
alternate:object={composition}{sphere}
sequence:objects={object}{objects}

# Main rule
sequence:main=<string><in><in>{config}<out><in>{define}<out><in>{objects}<out><out>