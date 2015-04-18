// Micro lex description file
//
// Atoms definitions

// Basic.lex must be included before this file
//	is parsed. Otherwise, forward declarations
//	must be done

atom:matrix=MATRIX
atom:vector=VECTOR
atom:object=OBJECT
atom:color=COLOR
atom:apply=->
atom:field=.

atom:sub=[-]
atom:add=[+]
atom:mul=[*]
atom:dot=[%]
atom:cross=[^]


//	Declarations
alternate:typedef=<matrix><vector><object><color>
alternate:binop=<add><mul><dot><cross><sub>

sequence:initid=<set><identifier>
sequence:initvec=<set><inbloc><float><sep><float><sep><float><sep><float><outbloc>
alternate:initialiser={initid}{initvec}<empty>
sequence:declaration={typedef}<identifier>{initialiser}<end>
sequence:declarations={declaration}{declarations}


//	Functions
//
//	Variable length parameter lists:
//	( )
//	( param )
//	( param1,param2,param3 ... paramn )
alternate:parameter=<identifier><float><string><integer>
forward:functionparameter_item
sequence:functionparameter_lastitem={parameter}<outparam>
sequence:functionparameter_listitem={parameter}<sep>{functionparameter_item}
alternate:functionparameter_item={functionparameter_listitem}{functionparameter_lastitem}
sequence:functionparameter_noitem=<outparam>
alternate:functionparameter={functionparameter_noitem}{functionparameter_listitem}{functionparameter_lastitem}

sequence:function=<identifier><inparam>{functionparameter}


//	Variable length typed parameter lists:
//	( )
//	( type param )
//	( type1 param1,type2 param2,type3  param3 ... typen paramn )
forward:programparameter_item
sequence:programparameter_lastitem={typedef}<identifier><outparam>
sequence:programparameter_listitem={typedef}<identifier><sep>{programparameter_item}
alternate:programparameter_item={programparameter_listitem}{programparameter_lastitem}
sequence:programparameter_noitem=<outparam>
alternate:programparameter={programparameter_noitem}{programparameter_listitem}{programparameter_lastitem}