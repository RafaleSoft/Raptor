<?xml version="1.0"?>
<Data>
<Shader name="simple_shader">
    <Material>
	<Ambient r="0.1" g="0.1" b="0.1" a="1.0"/>
	<Diffuse r="0.2" g="0.6" b="0.8" a="1.0"/>
	<Specular r="0.5" g="0.9" b="0.9" a="1.0"/>
	<Emission r="0.0" g="0.0" b="0.0" a="1.0"/>
	<Shininess r="10.0" g="0.0" b="0.0" a="1.0"/>
    </Material>
</Shader>
<ShadedGeometry>
  <Geometry name="cube">
    <Vertex>
      <Coord x="-1.0" y="-1.0" z="1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="-1.0" z="1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="1.0" z="1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="0" vertex2="1" vertex3="2"/>
    <Face vertex1="2" vertex2="3" vertex3="0"/>
	
    <Vertex>
      <Coord x="1.0" y="-1.0" z="1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="-1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="4" vertex2="5" vertex3="6"/>
    <Face vertex1="6" vertex2="7" vertex3="4"/>

    <Vertex>
      <Coord x="1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="1.0" z="-1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="-1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="8" vertex2="9" vertex3="10"/>
    <Face vertex1="10" vertex2="11" vertex3="8"/>

    <Vertex>
      <Coord x="-1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="-1.0" z="1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="1.0" z="1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="1.0" z="-1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="12" vertex2="13" vertex3="14"/>
    <Face vertex1="14" vertex2="15" vertex3="12"/>

    <Vertex>
      <Coord x="-1.0" y="1.0" z="-1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="1.0" z="1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="1.0" z="-1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="16" vertex2="17" vertex3="18"/>
    <Face vertex1="18" vertex2="19" vertex3="16"/>

    <Vertex>
      <Coord x="-1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="1.0" b="1.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="-1.0" z="-1.0"/>
      <Color r="1.0" g="0.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="1.0" y="-1.0" z="1.0"/>
      <Color r="0.0" g="1.0" b="0.0" a="1.0"/>
    </Vertex>
    <Vertex>
      <Coord x="-1.0" y="-1.0" z="1.0"/>
      <Color r="0.0" g="0.0" b="1.0" a="1.0"/>
    </Vertex>
    <Face vertex1="20" vertex2="21" vertex3="22"/>
    <Face vertex1="22" vertex2="23" vertex3="20"/>
  </Geometry>
  <RenderingProperties texturing="0" lighting="2" wireframe="0"/>
  <Shader name="simple_shader"/>
</ShadedGeometry>
<Object3DInstance name="cube-instance">
  <Object name="cube"/>
  <Transform 	m11="1.0" m12="0.0" m13="0.0" m14="2.0"
		m21="0.0" m22="1.0" m23="0.0" m24="0.0"
		m31="0.0" m32="0.0" m33="1.0" m34="0.0"
		m41="0.0" m42="0.0" m43="0.0" m44="1.0" />
</Object3DInstance>
<ViewModifier name="cube_modifier">
    <Action action="Rotation">
	<TimeFunction coord="x" type="time_linear" a0="0" a1="1"/>
	<TimeFunction coord="y" type="time_linear" a0="0" a1="2"/>
	<TimeFunction coord="z" type="time_linear" a0="0" a1="3"/>
    </Action>
    <Object name="cube"/>
</ViewModifier>
<Light name="main_light">
     <Material>
	<Ambient r="1.0" g="1.0" b="1.0" a="1.0"/>
	<Diffuse r="1.0" g="1.0" b="1.0" a="1.0"/>
	<Specular r="1.0" g="1.0" b="1.0" a="1.0"/>
	<Emission r="0.0" g="0.0" b="0.0" a="1.0"/>
	<Shininess r="10.0" g="0.0" b="0.0" a="1.0"/>
    </Material>
    <Position x="0.0" y="2.0" z="5.0"/>
    <Direction x="0.0" y="0.0" z="-1.0"/>
</Light>
<RaptorDisplay width="320" height="240" x="450" y="200">
    <RenderingProperties texturing="0" lighting="1" wireframe="0"/>
    <ViewPoint left="-1.33" right="1.33" bottom="-1.0" top="1.0" near="1.0" far="100.0" model="perspective">
        <EyePosition x="0.0" y="0.0" z="3.0"/>
        <TargetPosition x="0.0" y="0.0" z="-1.0"/>
    </ViewPoint>
    <Scene name="root_scene">
        <Object name="cube_modifier"/>
        <Light name="main_light"/>
    </Scene>
</RaptorDisplay>
</Data>