/*

XBase MFP functions, including interrupt registration (mfp)
c. Michael Moffitt 2021-2022

Limited code for interacting with the MFP, which controls the X68000's
interrupt sources.

*/
#ifndef XBASE_MFP_H
#define XBASE_MFP_H

#include <stdbool.h>
#include <stdint.h>

enum XBMFPInt
{
	// The following inerrupt sources may be enabled and disabled.
	XB_MFP_INT_RTC_ALARM                      = 0x40,
	XB_MFP_INT_EXT_POWER_OFF                  = 0x41,
	XB_MFP_INT_POWER_SWITCH_OFF               = 0x42,
	XB_MFP_INT_FM_SOUND_SOURCE                = 0x43,
	XB_MFP_INT_TIMER_D                        = 0x44,  // For BG processing.
	XB_MFP_INT_TIMER_C                        = 0x45,  // For mouse, cursor, FDD
	XB_MFP_INT_VDISP                          = 0x46,
	XB_MFP_INT_RTC_CLOCK                      = 0x47,
	XB_MFP_INT_TIMER_B                        = 0x48,
	XB_MFP_INT_KEY_SERIAL_OUT_ERROR           = 0x49,
	XB_MFP_INT_KEY_SERIAL_OUT_BUFFER_EMPTY    = 0x4A,
	XB_MFP_INT_KEY_SERIAL_IN_ERROR            = 0x4B,
	XB_MFP_INT_KEY_SERIAL_IN_BUFFER_FULL      = 0x4C,
	XB_MFP_INT_TIMER_A                        = 0x4D,
	XB_MFP_INT_CRTC_IRQ                       = 0x4E,
	XB_MFP_INT_HSYNC                          = 0x4F,
	XB_MFP_INT_SCC_B_TX_BUFFER_EMPTY          = 0x50,
	XB_MFP_INT_SCC_B_TX_BUFFER_EMPTY_2        = 0x51,
	XB_MFP_INT_SCC_B_EXTERNAL_STATUS_CHANGE   = 0x52,
	XB_MFP_INT_SCC_B_EXTERNAL_STATUS_CHANGE_2 = 0x53,
	XB_MFP_INT_SCC_B_RECEIVED_CHARACTER       = 0x54,  // Mouse 1 byte
	XB_MFP_INT_SCC_B_RECEIVED_CHARACTER_2     = 0x55,
	XB_MFP_INT_SCC_B_SPECIAL_RX_CONDITION     = 0x56,
	XB_MFP_INT_SCC_B_SPECIAL_RX_CONDITION_2   = 0x57,
	XB_MFP_INT_SCC_A_TX_BUFFER_EMPTY          = 0x58,
	XB_MFP_INT_SCC_A_TX_BUFFER_EMPTY_2        = 0x59,
	XB_MFP_INT_SCC_A_EXTERNAL_STATUS_CHANGE   = 0x5A,
	XB_MFP_INT_SCC_A_EXTERNAL_STATUS_CHANGE_2 = 0x5B,
	XB_MFP_INT_SCC_A_RECEIVED_CHARACTER       = 0x5C,  // RS-232C byte
	XB_MFP_INT_SCC_A_RECEIVED_CHARACTER_2     = 0x5D,
	XB_MFP_INT_SCC_A_SPECIAL_RX_CONDITION     = 0x5E,
	XB_MFP_INT_SCC_A_SPECIAL_RX_CONDITION_2   = 0x5F,
	XB_MFP_INT_IO_FDC_STATUS                  = 0x60,
	XB_MFP_INT_IO_FDC_INSERT_EJECT            = 0x61,
	XB_MFP_INT_IO_HDC_STATUS                  = 0x62,
	XB_MFP_INT_IO_PRINTER_READY               = 0x63,
	XB_MFP_INT_DMAC_0_END                     = 0x64,  // FDD
	XB_MFP_INT_DMAC_0_ERROR                   = 0x65,
	XB_MFP_INT_DMAC_1_END                     = 0x66,  // SASI
	XB_MFP_INT_DMAC_1_ERROR                   = 0x67,
	XB_MFP_INT_DMAC_2_END                     = 0x68,  // IOCS DMAMOVE/A/L
	XB_MFP_INT_DMAC_2_ERROR                   = 0x69,
	XB_MFP_INT_DMAC_3_END                     = 0x6A,  // ADPCM
	XB_MFP_INT_DMAC_3_ERROR                   = 0x6B,
	XB_MFP_INT_SPC_SCSI                       = 0x6C,  // Internal SCSI
	XB_MFP_INT_SPC_SCSI_2                     = 0xF6,  // SCSI board
};

// Bit definitions for the MFP's GPDR I/O.
typedef enum XBMFPGPDR
{
	XB_MFP_GPDR_ALARM  = 0x01,
	XB_MFP_GPDR_EXPON  = 0x02,
	XB_MFP_GPDR_POWSW  = 0x04,
	XB_MFP_GPDR_OPMIRQ = 0x08,
	XB_MFP_GPDR_VDISP  = 0x10,
	// TODO: Is this bit hooked up?
	XB_MFP_GPDR_VSYNC  = 0x40,
	XB_MFP_GPDR_HSYNC  = 0x80,
} XBMFPGPDR;


// Bit mask values used to enable / disable interrupt sources.
// These values correspond to the MFP IERA register.
typedef enum XBMFPIntMaskA
{
	XB_MFP_MASK_TIMER_B                 = 0x01,
	XB_MFP_MASK_SERIAL_OUT_ERROR        = 0x02,
	XB_MFP_MASK_SERIAL_OUT_BUFFER_EMPTY = 0x04,
	XB_MFP_MASK_SERIAL_IN_ERROR         = 0x08,
	XB_MFP_MASK_SERIAL_IN_BUFFER_FULL   = 0x10,
	XB_MFP_MASK_TIMER_A                 = 0x20,
	XB_MFP_MASK_CRTC_IRQ                = 0x40,
	XB_MFP_MASK_HSYNC                   = 0x80,
} XBMFPIntMaskA;

// These values correspond to the MFP IERB register.
typedef enum XBMFPIntMaskB
{
	XB_MFP_MASK_RTC_ALARM               = 0x01,
	XB_MFP_MASK_EXT_POWER_OFF           = 0x02,
	XB_MFP_MASK_POWER_SWITCH_OFF        = 0x04,
	XB_MFP_MASK_FM_SOUND_SOURCE         = 0x08,
	XB_MFP_MASK_TIMER_D                 = 0x10,
	XB_MFP_MASK_TIMER_C                 = 0x20,
	XB_MFP_MASK_VDISP                   = 0x40,
	XB_MFP_MASK_RTC_CLOCK               = 0x80,
} XBMFPIntMaskB;

// Read from the MFP's general purpose data register. AND the result with
// XBMFPGPDR values to test bits.
uint8_t xb_mfp_read_gpdr(void);

// Register a user IRQ handler for a user interrupt. It is suggested to pass a
// value from XBMFPInt for the vector.
// Register NULL to remove an association.
void xb_mfp_set_interrupt(uint8_t vector, void (*function)(void));

// Enable or disable interrupt generation for a vector.
void xb_mfp_set_interrupt_enable(uint8_t vector, bool enabled);

#endif  // XBASE_MFP_H
