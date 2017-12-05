// Micro lex description file
//
// Atoms definitions
#include Basic.lex
#include Expressions.lex
// #include Shader.lex
#include Commands.lex

// Keywords
atom:deltat=deltat
atom:run=run
atom:once=once
atom:loop=loop
//atom:opengl=opengl
atom:load=load
atom:import=import

atom:RotateX=RotateX
atom:RotateY=RotateY
atom:RotateZ=RotateZ
atom:Translate=Translate
atom:Push=Push
atom:Pop=Pop
atom:Render=Render
atom:Enable=Enable
atom:Disable=Disable


atom:constant=constant
atom:linear=linear
atom:quadric=quadric
atom:cubic=cubic
atom:cosine=cos
atom:sine=sin


// Time functions
sequence:constant=<constant><inparam><float><outparam>
sequence:linear=<linear><inparam><float><sep><float><outparam>
sequence:quadric=<quadric><inparam><float><sep><float><sep><float><outparam>
sequence:cubic=<cubic><inparam><float><sep><float><sep><float><sep><float><outparam>
sequence:cosine=<cosine><inparam><float><outparam>
sequence:sine=<sine><inparam><float><outparam>

alternate:proto={linear}{quadric}{cubic}{cosine}{sine}
sequence:timefunction=<string><set>{proto}<end>


// Actions

sequence:RotateX=<RotateX><inparam>{parameter}<outparam>
sequence:RotateY=<RotateY><inparam>{parameter}<outparam>
sequence:RotateZ=<RotateZ><inparam>{parameter}<outparam>
sequence:Translate=<Translate><inparam>{parameter}<sep>{parameter}<sep>{parameter}<outparam>
sequence:Push=<Push><inparam><outparam>
sequence:Pop=<Pop><inparam><outparam>
sequence:Render=<Render><inparam><outparam>
sequence:Enable=<Enable><inparam><string><outparam>
sequence:Disable=<Disable><inparam><string><outparam>


// Frames structures

alternate:action={RotateX}{RotateY}{RotateZ}{Translate}{Push}{Pop}{Render}{Enable}{Disable}
sequence:targetaction=<identifier><apply>{action}<end>

sequence:targetactions={targetaction}{targetactions}
sequence:timefunctions={timefunction}{timefunctions}



// Frame rule
//	Describe the content of a group of frames:	
//	- integer: number of frames for this group
//	- timefunctions: define the function that will be applyed to transforms
//	- target actions: define the actions that will apply to an object, using
//		transforms and time functions
sequence:framesequence=<inbloc><integer><inbloc>{timefunctions}<outbloc><inbloc>{targetactions}<outbloc><outbloc>

//	Framesequences are an unbounded set of frame descriptions
sequence:framesequences={framesequence}{framesequences}
//	Looping control
alternate:loop_mode=<once><loop>
//	Data loading
sequence:load3DS=<identifier><set><load><string><end>
//alternate:data={load3DS}{vertexprogram}{declaration}
alternate:data={load3DS}{declaration}{command}
sequence:datas={data}{datas}

// Main rule
//	Describe a sequence of frames:
//	- float			: delta t ( time increment )
//	- mode			: loop control ( once or loop )
//	- data			: desciption of datas used for rendering
//	- framesequences: the description of a set of frames
sequence:script=<inbloc><deltat><set><float><run><set>{loop_mode}<inbloc>{datas}<outbloc>{framesequences}<outbloc>
alternate:main={help}{clear}{selection}{scaleobject}{translateobject}{visibleobject}{rotateobject}{translateobjectabsolute}{script}