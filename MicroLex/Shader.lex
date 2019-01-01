// Micro lex description file
//
// Atoms definitions

// Basic.lex must be included before this file
//	is parsed. Otherwise, forward declarations
//	must be done

atom:shader=SHADER

atom:HPOS=VERTEX
atom:COL0=COLOR0
atom:COL1=COLOR1
atom:BFC0=BACKFACECOLOR0
atom:BFC1=BACKFACECOLOR1
atom:FOGC=FOGCOORD
atom:PSIZ=POINTSIZE
atom:TEX0=TEXCOORD0
atom:TEX1=TEXCOORD1
atom:TEX2=TEXCOORD2
atom:TEX3=TEXCOORD3
atom:TEX4=TEXCOORD4
atom:TEX5=TEXCOORD5
atom:TEX6=TEXCOORD6
atom:TEX7=TEXCOORD7
atom:OPOS=POSITION
atom:WGHT=WEIGHT
atom:NRML=NORMAL
atom:TMP0=PARAM0
atom:TMP1=PARAM1
atom:DATA=DATA
atom:TMU0=TMU0
atom:TMU1=TMU1
atom:TMU2=TMU2
atom:TMU3=TMU3
atom:PIXEL=PIXEL

//	Vertex shaders can handle x,y,z & w coordinates separately.
//	For colors and texture shaders, rgba is always used as a vector,
//	or used one by one. ( e.g. texture shader AR dependant )
atom:x=x
atom:y=y
atom:z=z
atom:w=w
atom:xy=xy
atom:xz=xz
atom:yz=yz
atom:xyz=xyz
atom:xw=xw
atom:yw=yw
atom:xyw=xyw
atom:zw=zw
atom:xzw=xzw
atom:yzw=yzw
atom:xyzw=xyzw
atom:r=r
atom:g=g
atom:b=b
atom:a=a
atom:rgba=rgba
atom:env=env

alternate:swizzle=<x><y><xy><z><xz><yz><xyz><w><xw><yw><xyw><zw><xzw><yzw><xyzw>
sequence:swizzlefield=<field>{swizzle}
alternate:optionnalswizzle={swizzlefield}<empty>
alternate:tmuswizzle=<r><g><b><a><rgba><x><y><z><w><xyzw><env>
sequence:tmuswizzlefield=<field>{tmuswizzle}
alternate:optionnaltmuswizzle={tmuswizzlefield}<empty>
sequence:pixelswizzlefield=<field><rgba>
alternate:optionnalpixelswizzle={pixelswizzlefield}<empty>

sequence:programData=<DATA><inparam><integer><outparam>
alternate:vertexAttribReg=<OPOS><WGHT><NRML><COL0><COL1><FOGC><TEX0><TEX1><TEX2><TEX3><TEX4><TEX5><TEX6><TEX7><TMP0><TMP1>
sequence:vertexAttrib=<HPOS><field>{vertexAttribReg}

sequence:localvar=<identifier>
sequence:localswizzlevar=<identifier>{swizzlefield}

alternate:operand={localvar}{programData}{vertexAttrib}{function}{localswizzlevar}
sequence:unaryoperation={operand}<end>
sequence:binaryoperation={operand}{binop}{operand}<end>
sequence:ternaryoperation={operand}<mul>{operand}<add>{operand}<end>

alternate:operation={unaryoperation}{binaryoperation}{ternaryoperation}

alternate:vertextarget=<OPOS><COL0><COL1><BFC0><BFC1><FOGC><PSIZ><TEX0><TEX1><TEX2><TEX3><TEX4><TEX5><TEX6><TEX7>
sequence:vertexresult=<HPOS><field>{vertextarget}
alternate:vertexprogramtarget={localvar}{vertexresult}
alternate:textureshadertarget=<TMU0><TMU1><TMU2><TMU3>
sequence:pixelshadertarget=<PIXEL>

sequence:vertexcommandtarget={vertexprogramtarget}{optionnalswizzle}<set>
sequence:texturecommandtarget={textureshadertarget}{optionnaltmuswizzle}<set>
sequence:pixelcommandtarget={pixelshadertarget}{optionnalpixelswizzle}<set>

alternate:commandtarget={vertexcommandtarget}{texturecommandtarget}{pixelcommandtarget}

sequence:affectation={commandtarget}{operation}
alternate:instruction={affectation}{declaration}
sequence:instructions={instruction}{instructions}

sequence:vertexprogram=<shader><identifier><inparam>{programparameter}<inbloc>{instructions}<outbloc>