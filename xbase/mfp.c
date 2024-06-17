#include "xbase/mfp.h"
#include "xbase/memmap.h"
#include <iocs.h>

// Struct representing MFP registers.
typedef struct XBMFP
{
	uint16_t gpdr; // General Purpose I/O line Data Register (used as interrupt sources on x68000)
	uint16_t aer;  // Active Edge Register (The signal edge the interrupt will be trigerred on)
	uint16_t ddr;  // Data Direction Register (only input on x68000)
	uint16_t iera; // Interrupt Enable Register A
	uint16_t ierb; // Interrupt Enable Register B
	uint16_t ipra; // Interrupt Pending Register A
	uint16_t iprb; // Interrupt Pending Register B
	uint16_t isra; // Interrupt In-Service Register A
	uint16_t isrb; // Interrupt In-Service Register B
	uint16_t imra; // Interrupt Mask Register A
	uint16_t imrb; // Interrupt Mask Register B
	uint16_t vr;   // Vector Register
	uint16_t tacr;
	uint16_t tbcr;
	uint16_t tcdcr;
	uint16_t tadr;
	uint16_t tbdr;
	uint16_t tcdr;
	uint16_t tddr;
	uint16_t scr;
	uint16_t ucr;
	uint16_t rsr;
	uint16_t tsr;
	uint16_t udr;
} XBMFP;

static volatile XBMFP *s_mfp = (volatile XBMFP *)(XB_MFP_BASE);

uint8_t xb_mfp_read_gpdr(void)
{
	return s_mfp->gpdr;
}

void *xb_mfp_set_interrupt(uint16_t vector, void (*interrupt_handler)(void))
{
	return _iocs_b_intvcs(vector, interrupt_handler);
}

void xb_mfp_set_interrupt_enable(uint16_t vector, bool enabled)
{
	if (vector >= XB_MFP_INT_TIMER_B && vector <= XB_MFP_INT_HSYNC)
	{
		// IERA
		const uint8_t bit = vector - XB_MFP_INT_TIMER_B;
		const uint8_t mask = 1 << bit;
		if (enabled)
		{
			s_mfp->iera |= mask;
			s_mfp->imra |= mask;
		}
		else
		{
			s_mfp->iera &= ~mask;
			s_mfp->imra &= ~mask;
		}
	}
	else if (vector >= XB_MFP_INT_RTC_ALARM && vector <= XB_MFP_INT_RTC_CLOCK)
	{
		// IERB
		const uint8_t bit = vector - XB_MFP_INT_RTC_ALARM;
		const uint8_t mask = 1 << bit;
		if (enabled)
		{
			s_mfp->ierb |= mask;
			s_mfp->imrb |= mask;
		}
		else
		{
			s_mfp->ierb &= ~mask;
			s_mfp->imrb &= ~mask;
		}
	}
}
