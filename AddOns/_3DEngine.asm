.386P
.model flat,C

include _3DEng~1.inc

;
;	initialiser le fpu a precision = 32bits
;	( pour PIII, vecteurs de 4*32bits )

data segment
	fpucw	dw	?
	fpu24	dw	?
data ends

code segment
assume cs:code,ds:data

.486P

initfpu proc C
	finit
	fclex
	;mov ax,037DH
	;mov [fpucw],ax
	;fldcw fpucw
	ret
initfpu endp

matrixpervertex proc	C	vect:dword,mat:dword,res:dword
	push esi
	push edi
	mov esi,[vect]
	mov edi,[res]
	
	fld dword ptr [esi]			;st(2)
	fld dword ptr [esi+4]	;st(1)
	fld dword ptr [esi+8]	;st(0)
	mov esi,[mat]

	fld dword ptr [esi]			 
	fmul st(0),st(3)
	fld dword ptr [esi+4]	
	fmul st(0),st(3)
  	fld dword ptr [esi+8]	
	fmul st(0),st(3)
	faddp
	faddp
	add esi,16
	fstp dword ptr [edi]

	fld dword ptr [esi]			 
	fmul st(0),st(3)
	fld dword ptr [esi+4]	
	fmul st(0),st(3)
  	fld dword ptr [esi+8]	
	fmul st(0),st(3)
	faddp
	faddp
	add esi,16
	fstp dword ptr [edi+4]

	fld dword ptr [esi]			 
	fmulp st(3),st(0)
	fld dword ptr [esi+4]	
	fmulp st(2),st(0)
  	fld dword ptr [esi+8]	
	fmulp st(1),st(0)
	faddp
	faddp
	fstp dword ptr [edi+8]
	fld1
	fstp dword ptr [edi+12]
	
	pop edi
	pop esi
	ret
matrixpervertex endp

normalise proc	C	vect:dword,norm:dword
	push esi
	mov esi,[vect]
	fld dword ptr [esi]				; st2
	fld dword ptr [esi+4]		; st1
  	fld dword ptr [esi+8]		; st0
	fld st(2)
	fmul st(0),st(3)
	fld st(2)
	fmul st(0),st(3)
	faddp
	fld st(1)
	fmul st(0),st(2)
	faddp
	fsqrt
	fdivr dword ptr [norm]

	fmul st(3),st(0)
	fmul st(2),st(0)
	fmulp 
	fstp dword ptr [esi+8]
	fstp dword ptr [esi+4]
	fstp dword ptr [esi]
	pop esi
	ret
normalise endp

code ends

end