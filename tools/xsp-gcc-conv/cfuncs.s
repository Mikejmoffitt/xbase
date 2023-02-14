	.bss

# Storage for a2.
c_a2_sav:	ds.l	1
c_d2_sav:	ds.l	1
c_savret:	ds.l	1

	.text

# Functions that do not require any fussing.
.macro	FuncDirect	fname
	.globl	\fname
	.type	\fname,%function
\fname	=	_\fname
	.endm

# Functions that need A2 and D2 saved.
.macro	FuncA2D2Sav	fname
	.globl	\fname
	.type	\fname,%function
\fname:
	move.l	a2, (c_a2_sav).w
	move.l	d2, (c_d2_sav).w
	move.l	(sp)+, (c_savret).w
	bsr	_\fname
	move.l	(c_a2_sav).w, a2
	move.l	(c_d2_sav).w, d2
	move.l	(c_savret).w, a0
	jmp	(a0)
	rts
	.endm

	FuncDirect	xsp_on
	FuncDirect	xsp_off
	FuncDirect	xsp_pcgdat_set
	FuncDirect	xsp_pcgmask_on
	FuncDirect	xsp_pcgmask_off
	FuncDirect	xsp_mode
	FuncDirect	xsp_objdat_set
	FuncDirect	xsp_vsync
	FuncA2D2Sav	xsp_set
	FuncA2D2Sav	xobj_set
	FuncA2D2Sav	xsp_set_st
	FuncA2D2Sav	xobj_set_st
	FuncDirect	xsp_out
	FuncDirect	xsp_vertical
	FuncDirect	xsp_vsyncint_on
	FuncDirect	xsp_vsyncint_off
	FuncDirect	xsp_hsyncint_on
	FuncDirect	xsp_hsyncint_off
	FuncDirect	xsp_auto_adjust_divy
	FuncDirect	xsp_min_divh_set
	FuncDirect	xsp_divy_get
	FuncDirect	xsp_raster_ofs_for31khz_set
	FuncDirect	xsp_raster_ofs_for31khz_get
	FuncDirect	xsp_raster_ofs_for15khz_set
	FuncDirect	xsp_raster_ofs_for15khz_get
