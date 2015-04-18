// Micro lex description file
//
// Atoms definitions

atom:integer=[0123456789]+

atom:sign=-
atom:offset=%

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

atom:v=v
atom:c=c
atom:o=o
atom:in={
atom:out=}
atom:dot=.
atom:next=,
atom:endl=endl

atom:HPOS=HPOS
atom:COL0=COL0
atom:COL1=COL1
atom:BFC0=BFC0
atom:BFC1=BFC1
atom:FOGC=FOGC
atom:PSIZ=PSIZ
atom:TEX0=TEX0
atom:TEX1=TEX1
atom:TEX2=TEX2
atom:TEX3=TEX3
atom:TEX4=TEX4
atom:TEX5=TEX5
atom:TEX6=TEX6
atom:TEX7=TEX7
atom:OPOS=OPOS
atom:WGHT=WGHT
atom:NRML=NRML

atom:R0=R0
atom:R1=R1
atom:R2=R2
atom:R3=R3
atom:R4=R4
atom:R5=R5
atom:R6=R6
atom:R7=R7
atom:R8=R8
atom:R9=R9
atom:R10=R10
atom:R11=R11

atom:addrReg=A0.x

atom:TRIop=MAD

atom:MUL=MUL
atom:ADD=ADD
atom:DP3=DP3
atom:DP4=DP4
atom:DST=DST
atom:MIN=MIN
atom:MAX=MAX
atom:SLT=SLT
atom:SGE=SGE

atom:RCP=RCP
atom:RSQ=RSQ
atom:EXP=EXP
atom:LOG=LOG

atom:MOV=MOV
atom:LIT=LIT

atom:ARL=ARL

atom:END=END
atom:START=!!VP1.0

// Rules definitions

alternate:optionalSign=<sign><empty>
alternate:component=<x><y><z><w>

sequence:scalarSuffix=<dot>{component}
sequence:vectorSuffix=<dot>{component}{component}{component}{component}
alternate:swizzleSuffix={scalarSuffix}{vectorSuffix}<empty>

alternate:vertexResultRegName=<HPOS><COL0><COL1><BFC0><BFC1><FOGC><PSIZ><TEX0><TEX1><TEX2><TEX3><TEX4><TEX5><TEX6><TEX7>
sequence:vertexResultReg=<o><in>{vertexResultRegName}<out>

alternate:temporaryReg=<R0><R1><R2><R3><R4><R5><R6><R7><R8><R9><R10><R11>

alternate:progParamOffset=<sign><offset>

sequence:relProgParamRegNoOffset=<addrReg><out>
sequence:relProgParamRegOffset=<addrReg>{progParamOffset}<integer><out>
sequence:absProgParamReg=<integer><out>

alternate:paramReg={absProgParamReg}{relProgParamRegOffset}{relProgParamRegNoOffset}

sequence:progParamReg=<c><in>{paramReg}

alternate:vertexAttribRegNum=<integer><OPOS><WGHT><NRML><COL0><COL1><FOGC><TEX0><TEX1><TEX2><TEX3><TEX4><TEX5><TEX6><TEX7>
sequence:vertexAttribReg=<v><in>{vertexAttribRegNum}<out>

alternate:srcReg={vertexAttribReg}{progParamReg}{temporaryReg}
alternate:dstReg={temporaryReg}{vertexResultReg}

alternate:optionalMask=<x><y><xy><z><xz><yz><xyz><w><xw><yw><xyw><zw><xzw><yzw><xyzw>
sequence:dstRegMsk={dstReg}<dot>{optionalMask}

sequence:scalarSrcReg={optionalSign}{srcReg}{scalarSuffix}
sequence:vectorSrcReg={optionalSign}{srcReg}{vectorSuffix}
sequence:singleSrcReg={optionalSign}{srcReg}
sequence:swizzleSrcReg={optionalSign}{srcReg}{swizzleSuffix}
sequence:maskedDstReg={dstReg}{dstRegMsk}

alternate:BINop=<MUL><ADD><DP3><DP4><DST><MIN><MAX><SLT><SGE>

alternate:SCALARop=<RCP><RSQ><EXP><LOG>

alternate:VECTORop=<MOV><LIT>

sequence:TRIop_instruction=<TRIop>{maskedDstReg}<next>{swizzleSrcReg}<next>{swizzleSrcReg}<next>{swizzleSrcReg}
sequence:BINop_instruction={BINop}{maskedDstReg}<next>{swizzleSrcReg}<next>{swizzleSrcReg}
sequence:SCALARop_instruction={SCALARop}{maskedDstReg}<next>{scalarSrcReg}
sequence:VECTORop_instruction={VECTORop}{maskedDstReg}<next>{swizzleSrcReg}
sequence:ARL_instruction=<ARL><addrReg><next>{scalarSrcReg}

alternate:instruction={ARL_instruction}{VECTORop_instruction}{SCALARop_instruction}{BINop_instruction}{TRIop_instruction}
sequence:instructionLine={instruction}<endl>

sequence:instructionSequence={instructionLine}{instructionSequence}


// Main rule
sequence:main=<START>{instructionSequence}<END>