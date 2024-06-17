// XBase MFP functions, including interrupt registration (mfp)
// (c) Michael Moffitt 2021-2024
// 
// Limited code for interacting with the MFP, which controls the X68000's
// interrupt sources.
#pragma once

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>
#endif

// The following interrupt sources may be enabled and disabled.
#define XB_MFP_INT_RTC_ALARM                      0x40
#define XB_MFP_INT_EXT_POWER_OFF                  0x41
#define XB_MFP_INT_POWER_SWITCH_OFF               0x42
#define XB_MFP_INT_FM_SOUND_SOURCE                0x43
#define XB_MFP_INT_TIMER_D                        0x44  // For BG processing.
#define XB_MFP_INT_TIMER_C                        0x45  // For mouse cursor FDD
#define XB_MFP_INT_VDISP                          0x46
#define XB_MFP_INT_RTC_CLOCK                      0x47
#define XB_MFP_INT_TIMER_B                        0x48
#define XB_MFP_INT_KEY_SERIAL_OUT_ERROR           0x49
#define XB_MFP_INT_KEY_SERIAL_OUT_BUFFER_EMPTY    0x4A
#define XB_MFP_INT_KEY_SERIAL_IN_ERROR            0x4B
#define XB_MFP_INT_KEY_SERIAL_IN_BUFFER_FULL      0x4C
#define XB_MFP_INT_TIMER_A                        0x4D
#define XB_MFP_INT_CRTC                           0x4E
#define XB_MFP_INT_HSYNC                          0x4F
#define XB_MFP_INT_SCC_B_TX_BUFFER_EMPTY          0x50
#define XB_MFP_INT_SCC_B_TX_BUFFER_EMPTY_2        0x51
#define XB_MFP_INT_SCC_B_EXTERNAL_STATUS_CHANGE   0x52
#define XB_MFP_INT_SCC_B_EXTERNAL_STATUS_CHANGE_2 0x53
#define XB_MFP_INT_SCC_B_RECEIVED_CHARACTER       0x54  // Mouse 1 byte
#define XB_MFP_INT_SCC_B_RECEIVED_CHARACTER_2     0x55
#define XB_MFP_INT_SCC_B_SPECIAL_RX_CONDITION     0x56
#define XB_MFP_INT_SCC_B_SPECIAL_RX_CONDITION_2   0x57
#define XB_MFP_INT_SCC_A_TX_BUFFER_EMPTY          0x58
#define XB_MFP_INT_SCC_A_TX_BUFFER_EMPTY_2        0x59
#define XB_MFP_INT_SCC_A_EXTERNAL_STATUS_CHANGE   0x5A
#define XB_MFP_INT_SCC_A_EXTERNAL_STATUS_CHANGE_2 0x5B
#define XB_MFP_INT_SCC_A_RECEIVED_CHARACTER       0x5C  // RS-232C byte
#define XB_MFP_INT_SCC_A_RECEIVED_CHARACTER_2     0x5D
#define XB_MFP_INT_SCC_A_SPECIAL_RX_CONDITION     0x5E
#define XB_MFP_INT_SCC_A_SPECIAL_RX_CONDITION_2   0x5F
#define XB_MFP_INT_IO_FDC_STATUS                  0x60
#define XB_MFP_INT_IO_FDC_INSERT_EJECT            0x61
#define XB_MFP_INT_IO_HDC_STATUS                  0x62
#define XB_MFP_INT_IO_PRINTER_READY               0x63
#define XB_MFP_INT_DMAC_0_END                     0x64  // FDD
#define XB_MFP_INT_DMAC_0_ERROR                   0x65
#define XB_MFP_INT_DMAC_1_END                     0x66  // SASI
#define XB_MFP_INT_DMAC_1_ERROR                   0x67
#define XB_MFP_INT_DMAC_2_END                     0x68  // IOCS DMAMOVE/A/L
#define XB_MFP_INT_DMAC_2_ERROR                   0x69
#define XB_MFP_INT_DMAC_3_END                     0x6A  // ADPCM
#define XB_MFP_INT_DMAC_3_ERROR                   0x6B
#define XB_MFP_INT_SPC_SCSI                       0x6C  // Internal SCSI
#define XB_MFP_INT_SPC_SCSI_2                     0xF6  // SCSI board

// GPDR Bit values.
#define XB_MFP_GPDR_ALARM  0
#define XB_MFP_GPDR_EXPON  1
#define XB_MFP_GPDR_POWSW  2
#define XB_MFP_GPDR_OPMIRQ 3
#define XB_MFP_GPDR_VDISP  4
// TODO: Is bit 7 hooked upp?
#define XB_MFP_GPDR_VSYNC  6
#define XB_MFP_GPDR_HSYNC  7

// Bits used to enable / disable interrupt sources.
// These values correspond to the MFP IERA register.
#define XB_MFP_MASK_TIMER_B                 0x01
#define XB_MFP_MASK_SERIAL_OUT_ERROR        0x02
#define XB_MFP_MASK_SERIAL_OUT_BUFFER_EMPTY 0x04
#define XB_MFP_MASK_SERIAL_IN_ERROR         0x08
#define XB_MFP_MASK_SERIAL_IN_BUFFER_FULL   0x10
#define XB_MFP_MASK_TIMER_A                 0x20
#define XB_MFP_MASK_CRTC_IRQ                0x40
#define XB_MFP_MASK_HSYNC                   0x80
// These values correspond to the MFP IERB register.
#define XB_MFP_MASK_RTC_ALARM               0x01
#define XB_MFP_MASK_EXT_POWER_OFF           0x02
#define XB_MFP_MASK_POWER_SWITCH_OFF        0x04
#define XB_MFP_MASK_FM_SOUND_SOURCE         0x08
#define XB_MFP_MASK_TIMER_D                 0x10
#define XB_MFP_MASK_TIMER_C                 0x20
#define XB_MFP_MASK_VDISP                   0x40
#define XB_MFP_MASK_RTC_CLOCK               0x80

#ifndef __ASSEMBLER__

// Read from the MFP's general purpose data register. AND the result with
// XBMFPGPDR BITVAL values to test bits.
uint8_t xb_mfp_read_gpdr(void);

// Register a user IRQ handler for a user interrupt. It is suggested to pass a
// value from XB_MFP_INT_* for the vector.
// Register NULL to remove an association.
// This function is a proper interrupt handler, so please ensure it returns
// with `rte`. For C functions, use XB_ISR (which is __attribute__(("interrupt"))).
// Returns a pointer to whatever was previously registered.
void *xb_mfp_set_interrupt(uint16_t vector, void (*interrupt_handler)(void));

// Enable or disable interrupt generation for a vector.
void xb_mfp_set_interrupt_enable(uint16_t vector, bool enabled);

#endif
