	.text

# Macro to expose a function.
.macro	FuncDirect	fname
	.globl	\fname
	.type	\fname,%function
\fname	=	_\fname
	.endm

	FuncDirect	xsp_on
	FuncDirect	xsp_off
	FuncDirect	xsp_pcgdat_set
	FuncDirect	xsp_pcgmask_on
	FuncDirect	xsp_pcgmask_off
	FuncDirect	xsp_mode
	FuncDirect	xsp_objdat_set
	FuncDirect	xsp_vsync
	FuncDirect	xsp_set
	FuncDirect	xobj_set
	FuncDirect	xsp_set_st
	FuncDirect	xobj_set_st
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
