// Micro lex description file
//
// Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*
atom:exp=[eE]
atom:identifier=[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]+
atom:sep=/
atom:vertex=v
atom:vt=vt
atom:vn=vn
atom:o=o
atom:f=f
atom:usemtl=usemtl
atom:mtllib=mtllib
atom:mtlfile=.mtl
atom:string=["]<identifier>["]

atom:linecomment=[#][^\$]*

map:integer=int
map:float=float
map:string=string
map:identifier=string

comment:linecomment

sequence:singleintg=<integer>
sequence:singleflt=<float>
sequence:expflt=<float><exp><integer>
alternate:real={singleflt}{expflt}{singleintg}
sequence:vertex=<vertex>{real}{real}{real}
sequence:texcoord=<vt>{real}{real}
sequence:normal=<vn>{real}{real}{real}

sequence:facev=<integer>
sequence:facevvtvn=<integer><sep><integer><sep><integer>
alternate:faceelt={facev}{facevvtvn}
sequence:faceelts={faceelt}{faceelts}
sequence:face=<f>{faceelts}

alternate:objectnamevalue=<identifier><integer>
sequence:objectname=<o>{objectnamevalue}
alternate:materialvalue=<identifier><integer>
sequence:material=<usemtl>{materialvalue}
sequence:library=<mtllib>{materialvalue}<mtlfile>

alternate:objdata={vertex}{texcoord}{normal}{objectname}{material}{face}{library}
sequence:objdatas={objdata}{objdatas}

sequence:main={objdatas}<eof>


