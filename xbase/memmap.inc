/*, TODO: Derive from memmap.h */

.set	XB_GVRAM_BASE, 0xC00000
.set	XB_TVRAM_BASE, 0xE00000
.set	XB_CRTC_BASE, 0xE80000
.set	XB_VIDCON_GP_PAL_BASE, 0xE82000
.set	XB_VIDCON_TX_PAL_BASE, 0xE82200
.set	XB_VIDCON_PCG_PAL_BASE, 0xE82200
.set	XB_VIDCON_R0, 0xE82400
.set	XB_VIDCON_R1, 0xE82500
.set	XB_VIDCON_R2, 0xE82600
.set	XB_MFP_BASE, 0xE88000

.set	XB_PCG_SPR_TABLE, 0xEB0000
.set	XB_PCG_TILE_DATA, 0xEB8000
.set	XB_PCG_BG0_NAME, 0xEBC000
.set	XB_PCG_BG1_NAME, 0xEBE000
.set	XB_PCG_BG0_XSCRL, 0xEB0800
.set	XB_PCG_BG0_YSCRL, 0xEB0802
.set	XB_PCG_BG1_XSCRL, 0xEB0804
.set	XB_PCG_BG1_YSCRL, 0xEB0806
.set	XB_PCG_BG_CTRL, 0xEB0808
.set	XB_PCG_HTOTAL, 0xEB080A
.set	XB_PCG_HDISP, 0xEB080C
.set	XB_PCG_VDISP, 0xEB080E
.set	XB_PCG_MODE, 0xEB0810

.set	XB_OPM_BASE, 0xE90000
.set	XB_JOY_BASE, 0xE9A001
