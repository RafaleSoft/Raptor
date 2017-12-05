//
//	ATOMS
//

IdFirstChar ::= 
Any ISO-10646 character encoded using UTF-8 except: 0x30-0x39, 0x0-0x20, 0x22, 0x23, 0x27, 0x2b, 0x2c, 0x2d, 0x2e, 0x5b, 0x5c, 0x5d, 0x7b, 0x7d, 0x7f ; 
IdRestChars ::= 
Any number of ISO-10646 characters except: 0x0-0x20, 0x22, 0x23, 0x27, 0x2c, 0x2e, 0x5b, 0x5c, 0x5d, 0x7b, 0x7d, 0x7f ; 

atom:DEF=DEF
atom:USE=USE
atom:PROTO=PROTO
atom:eventIn=
atom:eventOut=
atom:field=
atom:exposedField=
atom:EXTERNPROTO=EXTERNPROTO
atom:ROUTE=ROUTE
atom:dot=.
atom:TO=TO
atom:Script=Script
atom:IS=IS

//
//	RULES
//

//
//	Fields rules
//
fieldType ::= 
MFColor | 
MFFloat | 
MFInt32 | 
MFNode | 
MFRotation | 
MFString | 
MFTime | 
MFVec2f | 
MFVec3f | 
SFBool | 
SFColor | 
SFFloat | 
SFImage | 
SFInt32 | 
SFNode | 
SFRotation | 
SFString | 
SFTime | 
SFVec2f | 
SFVec3f ; 
fieldValue ::= 
sfboolValue | 
sfcolorValue | 
sffloatValue | 
sfimageValue | 
sfint32Value | 
sfnodeValue | 
sfrotationValue | 
sfstringValue | 
sftimeValue | 
sfvec2fValue | 
sfvec3fValue | 
mfcolorValue | 
mffloatValue | 
mfint32Value | 
mfnodeValue | 
mfrotationValue | 
mfstringValue | 
mftimeValue | 
mfvec2fValue | 
mfvec3fValue ; 
sfboolValue ::= 
TRUE | 
FALSE ; 
sfcolorValue ::= 
float float float ; 
sffloatValue ::= 
... floating point number in ANSI C floating point format... 
sfimageValue ::= 
int32 int32 int32 int32 ... 
sfint32Value ::= 
[[+]|-]{[0-9]+|0x[0-9a-fA-F]+}; 
sfnodeValue ::= 
nodeStatement | 
NULL ; 
sfrotationValue ::= 
float float float float ; 
sfstringValue ::= 
".*" ... double-quotes must be \", backslashes must be \\... 
sftimeValue ::= 
... double-precision number in ANSI C floating point format... 
mftimeValue ::= 
sftimeValue | 
[ ] | 
[ sftimeValues ] ; 
sftimeValues ::= 
sftimeValue | 
sftimeValue sftimeValues ; 
sfvec2fValue ::= 
float float ; 
sfvec3fValue ::= 
float float float ; 
mfcolorValue ::= 
sfcolorValue | 
[ ] | 
[ sfcolorValues ] ; 
sfcolorValues ::= 
sfcolorValue | 
sfcolorValue sfcolorValues ; 
mffloatValue ::= 
sffloatValue | 
[ ] | 
[ sffloatValues ] ; 
sffloatValues ::= 
sffloatValue | 
sffloatValue sffloatValues ; 
mfint32Value ::= 
sfint32Value | 
[ ] | 
[ sfint32Values ] ; 
sfint32Values ::= 
sfint32Value | 
sfint32Value sfint32Values ; 
mfnodeValue ::= 
nodeStatement | 
[ ] | 
[ nodeStatements ] ; 
nodeStatements ::= 
nodeStatement | 
nodeStatement nodeStatements ; 
mfrotationValue ::= 
sfrotationValue | 
[ ] | 
[ sfrotationValues ] ; 
sfrotationValues ::= 
sfrotationValue | 
sfrotationValue sfrotationValues ; 
mfstringValue ::= 
sfstringValue | 
[ ] | 
[ sfstringValues ] ; 
sfstringValues ::= 
sfstringValue | 
sfstringValue sfstringValues ; 
mfvec2fValue ::= 
sfvec2fValue | 
[ ] | 
[ sfvec2fValues] ; 
sfvec2fValues ::= 
sfvec2fValue | 
sfvec2fValue sfvec2fValues ; 
mfvec3fValue ::= 
sfvec3fValue | 
[ ] | 
[ sfvec3fValues ] ; 
sfvec3fValues ::= 
sfvec3fValue | 
sfvec3fValue sfvec3fValues ;

//
//	Node rules
//

sequence:Id=<IdFirstChar><IdRestChars>

sequence:nodeNameId={Id}
sequence:nodeTypeId={Id}
sequence:fieldId={Id}
sequence:eventInId={Id}
sequence:eventOutId={Id}

sequence:nBEFV={fieldId}{fieldValue}
sequence:nBEFId={fieldId}<IS>{fieldId}
sequence:nBEEIn={eventInId}<IS>{eventInId}
sequence:nBEEOut={eventOutId}<IS>{eventOutId}
alternate:nodeBodyElement={nBEFV}{nBEFId}{nBEEIn}{nBEEOut}{routeStatement}{protoStatement}

sequence:sBEIn=<eventIn>{fieldType}{eventInId}<IS>{eventInId}
sequence:sBEOut=<eventOut>{fieldType}{eventOutId}<IS>{eventOutId}
sequence:sBEField=<field>{fieldType}{fieldId}<IS>{fieldId}
alternate:scriptBodyElement={nodeBodyElement}{restrictedInterfaceDeclaration}{sBEIn}{sBEOut}{sBEField}
 
sequence:scriptBody={scriptBodyElement}{scriptBody}
sequence:nodeBody={nodeBodyElement}{nodeBody}

sequence:nodeKind={nodeTypeId}{nodeBody}
sequence:scriptKind=<Script>{scriptBody}
alternate:node={nodeKind}{scriptKind}


//
//	General rules
//

//	define Interface Declarations
sequence:rIDIn=<eventIn>{fieldType}{eventInId}
sequence:rIDOut=<eventOut>{fieldType}{eventOutId}
sequence:rIDFields=<field>{fieldType}{fieldId}{fieldValue}
sequence:rIDEFields=<exposedField>{fieldType}{fieldId}{fieldValue}
sequence:eIDFields=<field>{fieldType}{fieldId}
sequence:eIDEFields=<exposedField>{fieldType}{fieldId}

alternate:restrictedInterfaceDeclaration={rIDIn}{rIDOut}{rIDField}
alternate:interfaceDeclaration={restrictedInterfaceDeclaration}{rIDEFields}
aternate:externInterfaceDeclaration={rIDIn}{rIDOut}{eIDFields}{eIDEFields}

sequence:interfaceDeclarations={interfaceDeclaration}{interfaceDeclarations}
sequence:externInterfaceDeclarations={externInterfaceDeclaration}{externInterfaceDeclarations}

//	define Proto Statement rule
sequence:externproto=<EXTERNPROTO>{nodeTypeId}{externInterfaceDeclarations}{URLList}
sequence:protoBody={protoStatements}{node}{statements}
sequence:proto=<PROTO>{nodeTypeId}{interfaceDeclarations}{protoBody}
alternate:protoStatement={proto}{externproto}
sequence:protoStatements={protoStatement}{protoStatements}
 
//	define node Statement rule
sequence:nodeDEF=<DEF>{nodeNameId}{node}
sequence:nodeUSE=<USE>{nodeNameId}
alternate:nodeStatement={node}{nodeUSE}{nodeDEF}

//	define Route Statement rule
sequence:routeStatement=<ROUTE>{nodeNameId}<dot>{eventOutId}<TO>{nodeNameId}<dot>{eventInId}


//	define statement rule
alternate:statement={nodeStatement}{protoStatement}{routeStatement}
sequence:statements={statement}{statements}

sequence:main={statements}