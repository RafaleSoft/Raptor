.386
.model flat,C

include 3d.inc


data segment
	extrn cosinus:qword		;deux variables externes pour
	extrn sinus:qword		;avoir acces a la table angulaire
	extrn scalar:qword		;resultat de produit scalaire
	extrn _3Dinitialised:word
	extrn debug:qword
	extrn debug2:dword

	cos dq 360 dup (0)		;table locale de cosinus en 64bits
	sin dq 360 dup (0)		;table locale de sinus en 64bits
	c_uvn     CAMERA ?		;camera virtuelle globale en coordonnees WC
	c_vc      VERTEX ?		;centre de projection en VC
	ikinematx MATRIX ?		;matrice de cinematique inverse

	v1 VERTEX ?				;vertex tampon
	m1 MATRIX ?				;matrice tampon
	v2 VERTEX ?				;vertex tampon

	CosTheta dq ?			;paramatres rotation camera
	SinTheta dq ?
	CosPhi dq ?
	SinPhi dq ?
	CosGamma dq ?
	SinGamma dq ?

	P dq ?					;parametres cinematique inverse
	G dq ?
	K	dq ?
	Ct2 dq ?
	St2 dq ?
	Cg2 dq ?
	Sg2 dq ?

	fpupop dq ? 			;tampon pour registre FPU
							;a cause du bug cyrix sur fdecstp
	wfactor dq ?			;facteur de correction de oow pour
							;ramener 1/h dans [0..1] sur l'intervalle
							;z'=[FOCALDIST..-infini]
	halfwidth dq ?			;demi dimension de l'ecran
	halfheight dq ?
	fullwidth dd ?
	fullheight dd ?
	
data ends

code segment
assume cs:code,ds:data

.486P


VC2WC	proc	C	o:dword
; resultat stocke dans source
; ne tient pas compte de l'espace homogene
	mov edi,[o]
	fld qword ptr [edi]			; charge x,y,z
	fld qword ptr [edi+8]
	fld qword ptr [edi+16]

	; [x y z]'=Mt.[x y z]+r
	;calcul de x'
	fld c_uvn.vpu.x
	fmul st,st(3)
	fld c_uvn.vpv.x
	fmul st,st(3)
	faddp
	fld c_uvn.vpn.x
	fmul st,st(2)
	faddp
	fadd c_uvn.vrp.x
	fstp qword ptr [edi]
	
	;calcul de y'
	fld c_uvn.vpu.y
	fmul st,st(3)
	fld c_uvn.vpv.y
	fmul st,st(3)
	faddp
	fld c_uvn.vpn.y
	fmul st,st(2)
	faddp
	fadd c_uvn.vrp.y
	fstp qword ptr [edi+8]

	;calcul de z'
	fld c_uvn.vpu.z
	fmul st,st(3)
	fld c_uvn.vpv.z
	fmul st,st(3)
	faddp
	fld c_uvn.vpn.z
	fmul st,st(2)
	faddp
	fadd c_uvn.vrp.z
	fstp qword ptr [edi+16]

	fstp fpupop
	fstp fpupop
	fstp fpupop
	ret
VC2WC	endp

;pointcameraat	proc	pascal	FAR	s:word,o:dword
;	mov es,[s]
;	mov edi,[o]
;	;calcul de n
;	push es
;	push edi
;	call far ptr normalizevertex
;	fld qword ptr es:[edi]
;	fchs
;	fstp c_uvn.vpn.x
;	fld qword ptr es:[edi+8]
;	fchs
;	fstp c_uvn.vpn.y
;	fld qword ptr es:[edi+16]
;	fchs
;	fstp c_uvn.vpn.z
;	fld1
;	fstp c_uvn.vpn.h
;
;	;calcul de u
;	;verification vz<>0
;	fld c_uvn.vpn.z
;	ftst
;	fnstsw ax
;	fwait
;	shr ah,3
;	cmp ah,7
;	je @nozero
;	fld1
;	fstp c_uvn.vpu.z
;	fldz
;	fst c_uvn.vpu.x
;	fstp c_uvn.vpu.y
;	jmp @cross
;
;	@nozero:
;	fld1
;	fstp c_uvn.vpu.x
;	fldz
;	fstp c_uvn.vpu.y
;	fld c_uvn.vpn.x
;	fdiv c_uvn.vpn.z
;	fchs
;	fstp c_uvn.vpu.z
;	push ds
;	xor esi,esi
;	mov si,offset c_uvn.vpu
;	push esi
;	call far ptr normalizevertex
;	;calcul de v=n*u
;	@cross:
;	fld c_uvn.vpn.x
;	fmul c_uvn.vpu.y
;	fld c_uvn.vpu.x
;	fmul c_uvn.vpn.y
;	fsubp
;	fstp c_uvn.vpv.z
;	fld c_uvn.vpn.y
;	fmul c_uvn.vpu.z
;	fld c_uvn.vpu.y
;	fmul c_uvn.vpn.z
;	fsubp
;	fstp c_uvn.vpv.x
;	fld c_uvn.vpn.z
;	fmul c_uvn.vpu.x
;	fld c_uvn.vpu.z
;	fmul c_uvn.vpn.x
;	fsubp
;	fstp c_uvn.vpv.y
;
;	call far ptr changehomogenousspace
;	call far ptr perspectivecorrection
;
;	ret
;pointcameraat	endp


getcos proc C	angle:word
	xor edi,edi
	mov di,[angle]
	mov ebx,offset cos
	shl edi,3
	mov esi,offset cosinus
	; optimisation mmx -> movq
	mov eax,[ebx+edi]
	mov [esi],eax
	mov eax,[ebx+edi+4]
	mov [esi+4],eax
	ret
getcos endp

getsin proc  C	 angle:word
	xor edi,edi
	mov di,[angle]
	mov ebx,offset sin
	shl edi,3
	mov esi,offset sinus
	; optimisation mmx -> movq
	mov eax,[ebx+edi]
	mov [esi],eax
	mov eax,[ebx+edi+4]
	mov [esi+4],eax
	ret
getsin endp

getcameraVRP proc C	o:dword
	mov edi,[o]
	mov esi,offset c_uvn.vrp
	_ddmove 8 vrp
	ret
getcameraVRP endp

getcameraWRP proc C	o:dword
	mov edi,[o]
	mov esi,offset c_uvn.wrp
	_ddmove 8 wrp
	ret
getcameraWRP endp

getcameraHpersp proc C o:dword
	mov edi,[o]
	mov esi,offset c_uvn.hpersp
	_ddmove 8 hpersp
	ret
getcameraHpersp endp



getcameraVPU proc C	o:dword
	mov edi,[o]
	mov esi,offset c_uvn.vpu
	_ddmove 8 vpu
	ret
getcameraVPU endp


getcameraVPV proc C o:dword
	mov edi,[o]
	mov esi,offset c_uvn.vpv
	_ddmove 8 vpv
	ret
getcameraVPV endp


getcameraVPN proc C o:dword
	mov edi,[o]
	mov esi,offset c_uvn.vpn
	_ddmove 8 vpn
	ret
getcameraVPN endp


init3d proc C	screenw:dword,screenh:dword
local extrafpureg:dword
	finit
	fldpi
	mov ecx,360
	mov eax,180
	mov [extrafpureg],eax
	fild [extrafpureg]
	xor eax,eax
	xor edi,edi
	mov ebx,offset cos
	@cosbcl:
		mov [extrafpureg],eax
		fild [extrafpureg]
		fmul st(0),st(2)
		fdiv st(0),st(1)
		fcos
		fstp qword ptr [ebx+edi]
		fild [extrafpureg]
		fmul st(0),st(2)
		fdiv st(0),st(1)
		fsin
		fstp qword ptr [ebx+edi+0B40h]
		inc eax
		add edi,8
		dec ecx
		jnz @cosbcl
	fstp fpupop
	fstp fpupop
	fldz
	fst c_vc.x
	fst c_vc.y
	fst c_uvn.vrp.x
	fst c_uvn.vrp.y
	fst c_uvn.vrp.z
	fst c_uvn.vpu.y
	fst c_uvn.vpu.z
	fst c_uvn.vpv.x
	fst c_uvn.vpv.z
	fst c_uvn.vpn.x
	fstp c_uvn.vpn.y
	fld1
	fst c_vc.h
	fst c_uvn.vrp.h
	fst c_uvn.vpu.h
	fst c_uvn.vpv.h
	fst c_uvn.vpn.h
	fst c_uvn.vpu.x
	fst c_uvn.vpv.y
	fstp c_uvn.vpn.z
	mov eax,FOCALDIST
	mov [extrafpureg],eax
	fild [extrafpureg]
	fstp c_vc.z

	mov eax,2
	mov [extrafpureg],eax
	fild [screenw]
	fist fullwidth
	fild [extrafpureg]
	fdivp 
	fstp halfwidth

	fild [screenh]
	fist fullheight
	fild [extrafpureg]
	fdivp 
	fstp halfheight

	fld1
	mov eax,WFACTOR
	mov [extrafpureg],eax
	fild [extrafpureg]
	fdivp
	fstp wfactor
	mov ax,1
	mov [_3Dinitialised],ax
	ret
init3d endp


vertexpermatrix proc C	ov:dword,om:dword,odv:dword

	mov esi,[ov]
	mov ebx,[om]
	mov edi,[odv]

	;calcul bestial
	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx]	;m1.rowx.x
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+8]	;m1.rowx.y
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+16]	;m1.rowx.z
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+24]	;m1.rowx.h
	faddp
	fstp qword ptr [edi]	;v2.x

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+32]	;m1.rowy.x
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+40]	;m1.rowy.y
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+48]	;m1.rowy.z
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+56]	;m1.rowy.h
	faddp
	fstp qword ptr [edi+8]	;v2.y

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+64]	;m1.rowz.x
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+72]	;m1.rowz.y
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+80]	;m1.rowz.z
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [esi+88]	;m1.rowz.h
	faddp
	fstp qword ptr [edi+16]	;v2.z

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+96]	;m1.rowh.x
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+104];m1.rowh.y
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+112];m1.rowh.z
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+120];m1.rowh.h
	faddp
	fstp qword ptr [edi+24]	;v2.h

	ret
vertexpermatrix endp


matrixpervertex proc C	ov:dword,om:dword,odv:dword

	mov esi,[ov]
	mov ebx,[om]
	mov edi,[odv]

	;calcul bestial
	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx]	;m1.rowx.x
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+32]	;m1.rowy.x
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+64]	;m1.rowz.x
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+96]	;m1.rowh.x
	faddp
	fstp qword ptr [edi]	;v2.x

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+8]	;m1.rowx.y
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+40]	;m1.rowy.y
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+72]	;m1.rowz.y
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+104];m1.rowh.y
	faddp
	fstp qword ptr [edi+8]	;v2.y

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+16]	;m1.rowx.z
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+48]	;m1.rowy.z
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+80]	;m1.rowz.z
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+112];m1.rowh.z
	faddp
	fstp qword ptr [edi+16]	;v2.z

	fld qword ptr [esi]		;v1.x
	fmul qword ptr [ebx+24]	;m1.rowx.h
	fld qword ptr [esi+8]	;v1.y
	fmul qword ptr [ebx+56]	;m1.rowy.h
	faddp
	fld qword ptr [esi+16]	;v1.z
	fmul qword ptr [ebx+88]	;m1.rowz.h
	faddp
	fld qword ptr [esi+24]	;v1.h
	fmul qword ptr [ebx+120];m1.rowh.h
	faddp
	fstp qword ptr [edi+24]	;v2.h

	ret
matrixpervertex endp

vertexpervertex proc 	C	ov1:dword,ov2:dword
	mov esi,[ov1]
	mov edi,[ov2]
	; x1 * x2
	fld qword ptr [esi]
	add esi,8
	fmul qword ptr [edi]
	add edi,8
	; y1 * y2
	fld qword ptr [esi]
	add esi,8
	fmul qword ptr [edi]
	add edi,8
	faddp
	; z1 * z2
	fld qword ptr [esi]
	add esi,8
	fmul qword ptr [edi]
	add edi,8
	faddp
	; specialement pour le moteur3d, la quatrieme
	; conposante reste inchangee ( h )
	;fld qword ptr [esi]
	;fmul qword ptr [edi]
	;faddp
	fstp scalar
	ret
vertexpervertex endp


homogenizevertex proc C		ov:dword
	mov edi,[ov]
	fld1
	fdiv qword ptr es:[edi+24]
	fld qword ptr [edi]
	fmul st(0),st(1)
	fstp qword ptr [edi]
	fld qword ptr [edi+8]
	fmul st(0),st(1)
	fstp qword ptr [edi+8]
	fld qword ptr [edi+16]
	fmul st(0),st(1)
	fstp qword ptr [edi+16]
	fld1
	fstp qword ptr [edi+24]
	ret
homogenizevertex endp


normalizevertex proc C	ov:dword
	mov edi,[ov]
	fld1
	fld qword ptr [edi]
	fmul qword ptr [edi]
	fld qword ptr [edi+8]
	fmul qword ptr [edi+8]
	faddp
	fld qword ptr [edi+16]
	fmul qword ptr [edi+16]
	faddp
	fsqrt
	fdivp
	fld qword ptr [edi]
	fmul st(0),st(1)
	fstp qword ptr [edi]
	fld qword ptr [edi+8]
	fmul st(0),st(1)
	fstp qword ptr [edi+8]
	fld qword ptr [edi+16]
	fmul st(0),st(1)
	fstp qword ptr [edi+16]
	fstp fpupop
	ret
normalizevertex endp



; effectue la correction de perspective
; en ramenant les coordonnees dans
; l'espace homogene H= C_UVN.hpersp.h
; et calcule le facteur de projection
; hpersp.x ,.y et  .z
perspectivecorrection	proc 	
	;calcul de la perspective
	fld1
	fchs
	fdiv c_vc.z
	fld c_uvn.vpn.x
	fmul st(0),st(1)
	fstp c_uvn.hpersp.x
	fld c_uvn.vpn.y
	fmul st(0),st(1)
	fstp c_uvn.hpersp.y
	fld c_uvn.vpn.z
	fmul st(0),st(1)
	fstp c_uvn.hpersp.z
	fld1
	fld c_uvn.wrp.z
	fmul st(0),st(2)
	faddp
	fstp c_uvn.hpersp.h
	fstp fpupop
	ret
perspectivecorrection endp


; calcule le nouveau vecteur de position
; r' ainsi que le nouvel espace homogene
; H = r'.h = WRP.h
changehomogenousspace	proc	
	;calcul de wrp=r'
	fld c_uvn.vrp.x
	fmul c_uvn.vpu.x
	fld c_uvn.vrp.y
	fmul c_uvn.vpu.y
	faddp
	fld c_uvn.vrp.z
	fmul c_uvn.vpu.z
	faddp
	fchs
	fstp c_uvn.wrp.x

	fld c_uvn.vrp.x
	fmul c_uvn.vpv.x
	fld c_uvn.vrp.y
	fmul c_uvn.vpv.y
	faddp
	fld c_uvn.vrp.z
	fmul c_uvn.vpv.z
	faddp
	fchs
	fstp c_uvn.wrp.y

	fld c_uvn.vrp.x
	fmul c_uvn.vpn.x
	fld c_uvn.vrp.y
	fmul c_uvn.vpn.y
	faddp
	fld c_uvn.vrp.z
	fmul c_uvn.vpn.z
	faddp
	fchs
	fstp c_uvn.wrp.z
	ret
changehomogenousspace	endp


; Translation dans l'espace de visualisation:
;	r(x,y,z)=Mt.o(u,v,n)+r(x,y,z)	( o = vecteur translation )
;	puis calcul de r'(u,v,n)=WRP et de c_uvn.hpersp
translatecamera proc C o:dword
	mov edi,[o]
	fld qword ptr [edi]
	fmul c_uvn.vpu.x
	fadd c_uvn.vrp.x
	fstp c_uvn.vrp.x
	fld qword ptr [edi]
	fmul c_uvn.vpu.y
	fadd c_uvn.vrp.y
	fstp c_uvn.vrp.y
	fld qword ptr [edi]
	fmul c_uvn.vpu.z
	fadd c_uvn.vrp.z
	fstp c_uvn.vrp.z
	
	fld qword ptr [edi+8]
	fmul c_uvn.vpv.x
	fadd c_uvn.vrp.x
	fstp c_uvn.vrp.x
	fld qword ptr [edi+8]
	fmul c_uvn.vpv.y
	fadd c_uvn.vrp.y
	fstp c_uvn.vrp.y
	fld qword ptr [edi+8]
	fmul c_uvn.vpv.z
	fadd c_uvn.vrp.z
	fstp c_uvn.vrp.z

	fld qword ptr [edi+16]
	fmul c_uvn.vpn.x
	fadd c_uvn.vrp.x
	fstp c_uvn.vrp.x
	fld qword ptr [edi+16]
	fmul c_uvn.vpn.y
	fadd c_uvn.vrp.y
	fstp c_uvn.vrp.y
	fld qword ptr [edi+16]
	fmul c_uvn.vpn.z
	fadd c_uvn.vrp.z
	fstp c_uvn.vrp.z

	call changehomogenousspace
	call perspectivecorrection

	ret
translatecamera endp


rotatecamera proc C		theta:dword,phi:dword,gamma:dword
	;initialisation de la matrice de rotation
	fld dword ptr [theta]
	fsincos
	fstp CosTheta
	fstp SinTheta
	fld dword ptr [phi]
	fsincos
	fstp CosPhi
	fstp SinPhi	
	fld dword ptr [gamma]
	fsincos
	fstp CosGamma
	fstp SinGamma

	; calcul de u'
	fld CosTheta
	fmul CosGamma
	fstp c_uvn.vpu.x
	fld SinTheta
	fmul CosGamma
	fstp c_uvn.vpu.y
	fld SinGamma
	fchs
	fstp c_uvn.vpu.z

	;calcul de v'
	fld CosTheta
	fmul SinGamma
	fmul SinPhi
	fld SinTheta
	fmul CosPhi
	fsubp st(1),st(0)
	fstp c_uvn.vpv.x
	fld CosPhi
	fmul CosTheta
	fld SinPhi
	fmul SinGamma
	fmul SinTheta
	faddp st(1),st(0)
	fstp c_uvn.vpv.y
	fld CosGamma
	fmul SinPhi
	fstp c_uvn.vpv.z

	;calcul de n'
	fld CosPhi
	fmul SinGamma
	fmul CosTheta
	fld SinPhi
	fmul SinTheta
	faddp st(1),st(0)
	fstp c_uvn.vpn.x
	fld SinTheta
	fmul SinGamma
	fmul CosPhi
	fld	CosTheta
	fmul SinPhi
	fsubp st(1),st(0)
	fstp c_uvn.vpn.y
	fld CosPhi
	fmul CosGamma
	fstp c_uvn.vpn.z

	call changehomogenousspace
	call perspectivecorrection

	ret
rotatecamera endp


Xrotation proc C	phi:qword
	fld qword ptr [phi]
	fsincos

	;calcul de u'
	fld c_uvn.vpu.y
	fld c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpu.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpu.z
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpu.z

	;calcul de v'
	fld c_uvn.vpv.y
	fld c_uvn.vpv.y
	fmul st(0),st(2)
	fld c_uvn.vpv.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpv.y
	fmul st(0),st(2)
	fld c_uvn.vpv.z
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpv.z

	;calcul de n'
	fld c_uvn.vpn.y
	fld c_uvn.vpn.y
	fmul st(0),st(2)
	fld c_uvn.vpn.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpn.y
	fmulp st(2),st(0)
	fld c_uvn.vpn.z
	fmulp st(1),st(0)
	faddp
	fstp c_uvn.vpn.z

	call changehomogenousspace
	call perspectivecorrection

	ret
Xrotation endp


Yrotation proc C	gamma:qword
	fld qword ptr [gamma]
	fsincos

	;calcul de u'
	fld c_uvn.vpu.x
	fld c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpu.z
	fmul st(0),st(4)
	faddp
	fstp c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpu.z
	fmul st(0),st(2)
	fsub st(0),st(1)
	fstp c_uvn.vpu.z
	fstp fpupop

	;calcul de v'
	fld c_uvn.vpv.x
	fld c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpv.z
	fmul st(0),st(4)
	faddp
	fstp c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpv.z
	fmul st(0),st(2)
	fsub st(0),st(1)
	fstp c_uvn.vpv.z
	fstp fpupop

	;calcul de n'
	fld c_uvn.vpn.x
	fld c_uvn.vpn.x
	fmul st(0),st(2)
	fld c_uvn.vpn.z
	fmul st(0),st(4)
	faddp
	fstp c_uvn.vpn.x
	fmulp st(2),st(0)
	fld c_uvn.vpn.z
	fmulp st(1),st(0)
	fsub st(0),st(1)
	fstp c_uvn.vpn.z
	fstp fpupop

	call changehomogenousspace
	call perspectivecorrection

	ret
Yrotation endp


Zrotation proc C	theta:qword
	fld qword ptr [theta]
	fsincos

	;calcul de u'
	fld c_uvn.vpu.x
	fld c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpu.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpu.y
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpu.y

	;calcul de v'
	fld c_uvn.vpv.x
	fld c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpv.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpv.y
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpv.y

	;calcul de n'
	fld c_uvn.vpn.x
	fld c_uvn.vpn.x
	fmul st(0),st(2)
	fld c_uvn.vpn.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpn.x
	fmulp st(2),st(0)
	fld c_uvn.vpn.y
	fmulp st(1),st(0)
	faddp
	fstp c_uvn.vpn.y

	call changehomogenousspace
	call perspectivecorrection

	ret
Zrotation endp


Vrotation	proc C	phi:qword
	fld qword ptr [theta]
	fsincos

	; calcul de x'
	fld c_uvn.vpu.x
	fld c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpn.x
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpn.x
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpn.x

	;calcul de y'
	fld c_uvn.vpu.y
	fld c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpn.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpn.y
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpn.y

	;calcul de z'
	fld c_uvn.vpu.z
	fld c_uvn.vpu.z
	fmul st(0),st(2)
	fld c_uvn.vpn.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.z
	fmulp st(2),st(0)
	fld c_uvn.vpn.z
	fmulp st(1),st(0)
	faddp
	fstp c_uvn.vpn.z

	call changehomogenousspace
	call perspectivecorrection

	ret
Vrotation	endp


Nrotation	proc C	phi:qword
	fld qword ptr [theta]
	fsincos

	; calcul de x'
	fld c_uvn.vpu.x
	fld c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpv.x
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.x
	fmul st(0),st(2)
	fld c_uvn.vpv.x
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpv.x

	;calcul de y'
	fld c_uvn.vpu.y
	fld c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpv.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.y
	fmul st(0),st(2)
	fld c_uvn.vpv.y
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpv.y

	;calcul de z'
	fld c_uvn.vpu.z
	fld c_uvn.vpu.z
	fmul st(0),st(2)
	fld c_uvn.vpv.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpu.z
	fmulp st(2),st(0)
	fld c_uvn.vpv.z
	fmulp st(1),st(0)
	faddp
	fstp c_uvn.vpv.z

	call changehomogenousspace
	call perspectivecorrection

	ret
Nrotation	endp


Urotation	proc C	phi:qword
	fld qword ptr [theta]
	fsincos

	; calcul de x'
	fld c_uvn.vpv.x
	fld c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpn.x
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpv.x
	fmul st(0),st(2)
	fld c_uvn.vpn.x
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpn.x

	;calcul de y'
	fld c_uvn.vpv.y
	fld c_uvn.vpv.y
	fmul st(0),st(2)
	fld c_uvn.vpn.y
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpv.y
	fmul st(0),st(2)
	fld c_uvn.vpn.y
	fmul st(0),st(2)
	faddp
	fstp c_uvn.vpn.y

	;calcul de z'
	fld c_uvn.vpv.z
	fld c_uvn.vpv.z
	fmul st(0),st(2)
	fld c_uvn.vpn.z
	fmul st(0),st(4)
	fsubp
	fstp c_uvn.vpv.z
	fmulp st(2),st(0)
	fld c_uvn.vpn.z
	fmulp st(1),st(0)
	faddp
	fstp c_uvn.vpn.z

	call changehomogenousspace
	call perspectivecorrection

	ret
Urotation	endp


projectvertices	proc C		vertices:dword,hvertices:dword,numvertex:dword
local temph:dword
	mov ecx,[numvertex]
	mov esi,[vertices]
	mov edi,[hvertices]
	@vertexloop:
		;calcul de la coordonnee homogene
		fld1
		fld qword ptr [esi+24]
		fmul c_uvn.hpersp.h
		fld qword ptr [esi]
		fmul c_uvn.hpersp.x
		faddp
		fld qword ptr [esi+8]
		fmul c_uvn.hpersp.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.hpersp.z
		faddp
		; calculer 1/h pour eviter div !!!
		fist temph
		mov dword ptr [edi+44],1
		fdivp
		cmp temph,0
		jle	@out
		cmp temph,65535
		jg @out
		jmp @next
		@out:
		mov dword ptr [edi+44],0
		@next:

		;calcul de x'
		fld qword ptr [esi]
		fmul c_uvn.vpu.x
		fld qword ptr [esi+8]
		fmul c_uvn.vpu.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.vpu.z
		faddp
		fld qword ptr [esi+24]
		fmul c_uvn.wrp.x
		faddp
		fmul st(0),st(1)
		fadd halfwidth
		fist temph
		fstp dword ptr [edi]
		cmp temph,0
		jl	@xout
		mov eax,temph
		cmp eax,fullwidth
		jg @xout
		jmp @xnext
		@xout:
		mov dword ptr [edi+44],0
		@xnext:
		
		;calcul de y'
		fld qword ptr [esi]
		fmul c_uvn.vpv.x
		fld qword ptr [esi+8]
		fmul c_uvn.vpv.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.vpv.z
		faddp
		fld qword ptr [esi+24]
		fmul c_uvn.wrp.y
		faddp
		fmul st(0),st(1)
		fadd halfheight
		fist temph
		fstp dword ptr [edi+4]
		cmp temph,0
		jl	@yout
		mov eax,temph
		cmp eax,fullheight
		jg @yout
		jmp @ynext
		@yout:
		mov dword ptr [edi+44],0
		@ynext:

		;calcul de s
		fld qword ptr [esi+32]
		fmul st(0),st(1)
		fst dword ptr [edi+28]
		fstp dword ptr [edi+36]

		;calcul de t
		fld qword ptr [esi+40]
		fmul st(0),st(1)
		fst dword ptr [edi+32]
		fstp dword ptr [edi+40]

		;calcul de h' -> w buffer
		fmul wfactor
		fstp dword ptr [edi+8]

		add esi,48
		add edi,48
		dec ecx
		jz @finish
		jmp near ptr @vertexloop
	@finish:
	ret
projectvertices	endp


GLprojectvertices	proc C		vertices:dword,hvertices:dword,numvertex:dword
local temph:dword
	mov ecx,[numvertex]
	mov esi,[vertices]
	mov edi,[hvertices]
	@GLvertexloop:
		;calcul de la coordonnee homogene
		fld1
		fld qword ptr [esi+24]
		fmul c_uvn.hpersp.h
		fld qword ptr [esi]
		fmul c_uvn.hpersp.x
		faddp
		fld qword ptr [esi+8]
		fmul c_uvn.hpersp.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.hpersp.z
		faddp
		; calculer 1/h pour eviter div !!!
		fist temph
		mov dword ptr [edi+44],1
		fdivp
		cmp temph,0
		jle	@GLout
		cmp temph,65535
		jg @GLout
		jmp @GLnext
		@GLout:
		mov dword ptr [edi+44],0
		@GLnext:

		;calcul de x'
		fld qword ptr [esi]
		fmul c_uvn.vpu.x
		fld qword ptr [esi+8]
		fmul c_uvn.vpu.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.vpu.z
		faddp
		fld qword ptr [esi+24]
		fmul c_uvn.wrp.x
		faddp
		fmul st(0),st(1)
		fadd halfwidth
		fist temph
		fstp dword ptr [edi]
		cmp temph,0
		jl	@GLxout
		mov eax,temph
		cmp eax,fullwidth
		jg @GLxout
		jmp @GLxnext
		@GLxout:
		mov dword ptr [edi+44],0
		@GLxnext:
		
		;calcul de y'
		fld qword ptr [esi]
		fmul c_uvn.vpv.x
		fld qword ptr [esi+8]
		fmul c_uvn.vpv.y
		faddp
		fld qword ptr [esi+16]
		fmul c_uvn.vpv.z
		faddp
		fld qword ptr [esi+24]
		fmul c_uvn.wrp.y
		faddp
		fmul st(0),st(1)
		fadd halfheight
		fist temph
		fstp dword ptr [edi+4]
		cmp temph,0
		jl	@GLyout
		mov eax,temph
		cmp eax,fullheight
		jg @GLyout
		jmp @GLynext
		@GLyout:
		mov dword ptr [edi+44],0
		@GLynext:

		;calcul de s
		fld qword ptr [esi+32]
		fst dword ptr [edi+28]
		fstp dword ptr [edi+36]

		;calcul de t
		fld qword ptr [esi+40]
		fst dword ptr [edi+32]
		fstp dword ptr [edi+40]

		;calcul de h' -> w buffer
		fmul wfactor
		fstp dword ptr [edi+8]

		add esi,48
		add edi,48
		dec ecx
		jz @GLfinish
		jmp near ptr @GLvertexloop
	@GLfinish:
	ret
GLprojectvertices	endp

code ends

end

