
/*

XBase IRQ registration (irq)
c. Michael Moffitt 2021-2022

*/
#ifndef XBASE_IRQ_H
#define XBASE_IRQ_H

enum XBIrqType
{
	XB_IRQ_MFP_RTC_ALARM                  = 0x40,
	XB_IRQ_MFP_EXT_POWER_OFF              = 0x41,
	XB_IRQ_MFP_FRONT_SWITCH_OFF           = 0x42,
	XB_IRQ_MFP_FM_SOUND_SOURCE            = 0x43,
	XB_IRQ_MFP_TIMER_D                    = 0x44,  // For BG processing.
	XB_IRQ_MFP_TIMER_C                    = 0x45,  // For mouse, cursor, FDD
	XB_IRQ_MFP_VDISP                      = 0x46,
	XB_IRQ_MFP_RTC_CLOCK                  = 0x47,
	XB_IRQ_MFP_TIMER_B                    = 0x48,
	XB_IRQ_KEY_SERIAL_OUT_ERROR           = 0x49,
	XB_IRQ_KEY_SERIAL_OUT_CTS             = 0x4A,
	XB_IRQ_KEY_SERIAL_IN_ERROR            = 0x4B,
	XB_IRQ_KEY_SERIAL_IN_DATA_PRESENT     = 0x4C,
	XB_IRQ_MFP_TIMER_A                    = 0x4D,
	XB_IRQ_MFP_CRTC_IRQ                   = 0x4E,
	XB_IRQ_MFP_HSYNC                      = 0x4F,
	XB_IRQ_SCC_B_TX_BUFFER_EMPTY          = 0x50,
	XB_IRQ_SCC_B_TX_BUFFER_EMPTY_2        = 0x51,
	XB_IRQ_SCC_B_EXTERNAL_STATUS_CHANGE   = 0x52,
	XB_IRQ_SCC_B_EXTERNAL_STATUS_CHANGE_2 = 0x53,
	XB_IRQ_SCC_B_RECEIVED_CHARACTER       = 0x54,  // Mouse 1 byte
	XB_IRQ_SCC_B_RECEIVED_CHARACTER_2     = 0x55,
	XB_IRQ_SCC_B_SPECIAL_RX_CONDITION     = 0x56,
	XB_IRQ_SCC_B_SPECIAL_RX_CONDITION_2   = 0x57,
	XB_IRQ_SCC_A_TX_BUFFER_EMPTY          = 0x58,
	XB_IRQ_SCC_A_TX_BUFFER_EMPTY_2        = 0x59,
	XB_IRQ_SCC_A_EXTERNAL_STATUS_CHANGE   = 0x5A,
	XB_IRQ_SCC_A_EXTERNAL_STATUS_CHANGE_2 = 0x5B,
	XB_IRQ_SCC_A_RECEIVED_CHARACTER       = 0x5C,  // RS-232C byte
	XB_IRQ_SCC_A_RECEIVED_CHARACTER_2     = 0x5D,
	XB_IRQ_SCC_A_SPECIAL_RX_CONDITION     = 0x5E,
	XB_IRQ_SCC_A_SPECIAL_RX_CONDITION_2   = 0x5F,
	XB_IRQ_IO_FDC_STATUS                  = 0x60,
	XB_IRQ_IO_FDC_INSERT_EJECT            = 0x61,
	XB_IRQ_IO_HDC_STATUS                  = 0x62,
	XB_IRQ_IO_PRINTER_READY               = 0x63,
	XB_IRQ_DMAC_0_END                     = 0x64,  // FDD
	XB_IRQ_DMAC_0_ERROR                   = 0x65,
	XB_IRQ_DMAC_1_END                     = 0x66,  // SASI
	XB_IRQ_DMAC_1_ERROR                   = 0x67,
	XB_IRQ_DMAC_2_END                     = 0x68,  // IOCS DMAMOVE/A/L
	XB_IRQ_DMAC_2_ERROR                   = 0x69,
	XB_IRQ_DMAC_3_END                     = 0x6A,  // ADPCM
	XB_IRQ_DMAC_3_ERROR                   = 0x6B,
	XB_IRQ_SPC_SCSI                       = 0x6C,  // Internal SCSI
	XB_IRQ_SPC_SCSI_2                     = 0xF6,  // SCSI board
};

// Register a user IRQ handler for a user interrupt. It is suggested to pass a
// value from XBIrqType for the vector.
// Register NULL to remove an association.
void xb_irq_register(uint8_t vector, void (*function)(void));

#endif  // XBASE_IRQ_H
