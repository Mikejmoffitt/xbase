/*
Sprite Management System "XSP"
Ver 2.02 by Yosshin
https://github.com/yosshin4004/x68k_xsp
Translation and formatting by Mike Moffitt 2022
*/

#ifndef XSP2LIB_H
#define XSP2LIB_H

// xsp_set_st, xobj_set_st の引数構造体
// Argument struct for xsp_set_st and xobj_set_st.
typedef struct
{
	short x;              // X 座標 Position
	short y;              // Y 座標 Position
	short pt;             // スプライトパターン Sprite pattern no.
	short info;           // 反転コード・色・優先度を表わすデータ
	                      // Reflection code, color, priority data
} XSP_SET_ARG;

// 複合スプライトのフレームデータ構造体
// Composite sprite frame data struct.
typedef struct
{
	short vx;  // 相対座標データ Relative X coordinate data
	short vy;  // 相対座標データ Relative Y coordinate data
	short pt;  // スプライトパターン Sprite pattern no.
	short rv;  // 反転コード Reflection code
} XOBJ_FRM_DAT;

// 複合スプライトのリファレンスデータ構造体
// Composite sprite reference data struct.
typedef struct
{
	short num;    // 合成スプライト数 Composite sprite FRM data count
	void *ptr;    // 開始位置のポインタ Pointer to start of FRM data
	short unused; //（未使用）(Unused)
} XOBJ_REF_DAT;

// ラスター割り込み処理のタイムチャート
// Raster interrupt processing time chart.
typedef struct
{
	short ras_no;        // 割り込みラスタナンバー Raster interrupt number
	                     // Use -1 to mark the end of the time chart.
	void (*proc)(void);  // 割り込み処理関数のポインタ
	                     // Raster interrupt routine pointer
} XSP_TIME_CHART;

// Initializes XSP. Previously set V/H int handlers will be cancelled.
void xsp_on();

// Terminates XSP. Must be called at shutdown to avoid runaway interrupts.
// Interrupt vectors and other state are restored to their state before.
void xsp_off();

// Registers PCG pattern data and PCG management work memory with XSP.
//      pcg_dat: PCG pattern data pointer
//      pcg_alt: PCG placement work memory
//     alt_size: Length of pcg_alt, in bytes. Should be pattern count + 1.
void xsp_pcgdat_set(const void *pcg_dat, char *pcg_alt, short alt_size);

// Marks an area of PCG for XSP to not used.
//     start_no: Start PCG code number
//       end_no: End PCG code number
void xsp_pcgmask_on(short start_no, short end_no);

// Unmarks an area previously marked for disuse.
//     start_no: Start PCG code number
//       end_no: End PCG code number
void xsp_pcgmask_off(short start_no, short end_no);

// Selects the sprite doubler algorithm.
//      mode_no: Algorithm number.
//               1: Flickering 128 sprites to achieve 384
//               2: Maximum of 512 sprites (default)
//               3: Maximum of 512 sprites, with priority error mitigation
void xsp_mode(short mode_no);

// Specifies composite sprite mapping data.
//      sp_ref: Pointer to XOBJ reference data.
void xsp_objdat_set(const XOBJ_REF_DAT *sp_ref);

// Wait for vertical sync.
//            n: Number of vertical blank intervals to wait.
//               If 0 is specified, a frame of advance processing is done.
//               Advance processing may smooth a choppy game but will incur a
//               frame of latency.
// Returns the number of missed vertical blank periods.
short xsp_vsync(short n);

// Register a single-pattern sprite for display.
//            x: Sprite X coordinate
//            y: Sprite Y coordinate
//           pt: PCG pattern number (0-0x7FFF)
//         info: Attribute info (see readme)
// Returns 0 if the sprite's coordinates were off-screen.
short xsp_set(short x, short y, short pt, short info);

// Register a composite sprite for display.
//            x: Composite Sprite X coordinate
//            y: Composite Sprite Y coordinate
//           pt: Composite sprite mapping (REF) number (0-0x0FFF)
//         info: Attribute info (see readme)
void xobj_set(short x, short y, short pt, short info);

// Register a single-pattern sprite for display.
//          arg: Object set argument pointer
// Returns 0 if the sprite's coordinates were off-screen.
short xsp_set_st(const XSP_SET_ARG *arg);

// Register a composite sprite for display.
//          arg: Object set argument pointer
void xobj_set_st(const XSP_SET_ARG *arg);

// Displays sprites registered with batch processing.
// Returns the number of registered sprites displayed.
short xsp_out(void);

// Enables or disables vertical / portrait / tate mode.
//    vertical: 1 Vertical mode enabled
//              0 Vertical mode disabled.
void xsp_vertical(short flag);

// Registers a vertical sync interval callback function.
//       proc: Pointer to function (normal, not an int handler).
void xsp_vsyncint_on(void (*proc)(void));

// Clears registration of a vertical sync interval callback function.
void xsp_vsyncint_off(void);

// Registers a table of horizontal sync interupt callback functions.
// time_chart: Pointer to raster callback time chart (see readme).
void xsp_hsyncint_on(const XSP_TIME_CHART *time_chart);

// Clears registration of horizontal sync interval callback function(s).
void xsp_hsyncint_off(void);

// Enables or disables automatic raster division calcluation.
//      flag: 1 Automatic raster division enabled
//            0 Automatic raster division disabled
void xsp_auto_adjust_divy(short flag);

// Specifies a minimum height of raster division blocks.
//         h: height. Valid values are 24, 28, or 32.
void xsp_min_divh_set(short h);

// Returns the minimum raster division height.
//         i: Division block index (0 - 6).
short xsp_divy_get(short i);

// Sets offset value for sprite transfer raster line for 31KHz.
//      offs: Transfer raster line offset
void xsp_raster_ofs_for31khz_set(short ofs);

// Returns the sprite transfer raster line for 31KHz.
short xsp_raster_ofs_for31khz_get(void);

// Sets offset value for sprite transfer raster line for 15KHz.
//      offs: Transfer raster line offset
void xsp_raster_ofs_for15khz_set(short ofs);

// Returns the sprite transfer raster line for 15KHz.
short xsp_raster_ofs_for15khz_get(void);

#endif
