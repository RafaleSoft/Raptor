LANG			equ C

FOCALDIST		equ 400
SCALEFACTOR		equ 0.25
FRONTCLIPPINGPLANE	equ 0
FARCLIPPINGPLANE	equ -500
HALFSCREENWIDTH		equ 400
HALFSCREENHEIGHT	equ 300
WFACTOR			equ 2

_4DVERTEX STRUC
	x DD ?
	y DD ?
	z DD ?
	h DD ?
_4DVERTEX ENDS

TEXVERTEX STRUC
	u DD ?
	v DD ?
TEXVERTEX ENDS

VERTEX STRUC
	coords _4DVERTEX <0,0,0,1>
	texcoords TEXVERTEX <0,0>
VERTEX ENDS

MATRIX STRUC
	rowx _4DVERTEX ?
	rowy _4DVERTEX ?
	rowz _4DVERTEX ?
	rowh _4DVERTEX ?
MATRIX ENDS

CAMERA STRUC
	vrp _4DVERTEX <0,0,0,1>
	vpu _4DVERTEX <1,0,0,1>
	vpv _4DVERTEX <0,1,0,1>
	vpn _4DVERTEX <0,0,1,1>
	wrp _4DVERTEX <0,0,0,1>
	hpersp _4DVERTEX <0,0,0,1>
CAMERA ENDS


_ddmove MACRO	nb,index
	mov cx,nb
	ddbcl&index&:
		mov eax,[esi]
		add esi,4
		mov [edi],eax
		add edi,4
		dec cx
		jnz ddbcl&index&
ENDM

public LANG normalise
public LANG initfpu
public LANG matrixpervertex

;public LANG init3d
;public LANG getcos
;public LANG getsin
;public LANG getcameraVRP
;public LANG getcameraWRP
;public LANG getcameraVPU
;public LANG getcameraVPV
;public LANG getcameraVPN
;public LANG getcameraHpersp
;public LANG translatecamera
;public LANG VC2WC
;public LANG vertexpervertex
;public LANG vertexpermatrix
;public LANG homogenizevertex
;public LANG normalizevertex
;public LANG projectvertices
;public LANG GLprojectvertices
;public LANG rotatecamera
;public LANG Xrotation
;public LANG Yrotation
;public LANG Zrotation
;public LANG Vrotation
;public LANG Nrotation
;public LANG Urotation


