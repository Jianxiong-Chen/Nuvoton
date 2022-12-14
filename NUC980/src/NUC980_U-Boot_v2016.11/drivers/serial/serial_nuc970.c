/*
 * Copyright (c) 2014	Nuvoton Technology Corp.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Description:   NUC970 UART driver
 */
#include <common.h>
#include <asm/io.h>
#include <watchdog.h>
#include <serial.h>

//#include "serial_nuc970.h"
#include <asm/arch/serial_nuc970.h>
#include <nuc970_keypad.h>


DECLARE_GLOBAL_DATA_PTR;

int nuc970_serial_init (void);
void nuc970_serial_putc (const char ch);
void nuc970_serial_puts (const char *s);
int nuc970_serial_getc (void);
int nuc970_serial_tstc (void);
void nuc970_serial_setbrg (void);

typedef struct
{
	union
	{
		volatile unsigned int RBR;	/*!< Offset: 0x0000   Receive Buffer Register		*/
		volatile unsigned int THR;	/*!< Offset: 0x0000   Transmit Holding Register		*/
	} x;
	volatile unsigned int IER;		/*!< Offset: 0x0004   Interrupt Enable Register		*/
	volatile unsigned int FCR;		/*!< Offset: 0x0008   FIFO Control Register		*/
	volatile unsigned int LCR;		/*!< Offset: 0x000C   Line Control Register		*/
	volatile unsigned int MCR;		/*!< Offset: 0x0010   Modem Control Register		*/
	volatile unsigned int MSR;		/*!< Offset: 0x0014   Modem Status Register		*/
	volatile unsigned int FSR;		/*!< Offset: 0x0018   FIFO Status Register		*/
	volatile unsigned int ISR;		/*!< Offset: 0x001C   Interrupt Status Register		*/
	volatile unsigned int TOR;		/*!< Offset: 0x0020   Time Out Register			*/
	volatile unsigned int BAUD;		/*!< Offset: 0x0024   Baud Rate Divisor Register	*/
	volatile unsigned int IRCR;		/*!< Offset: 0x0028   IrDA Control Register		*/
	volatile unsigned int ALTCON;		/*!< Offset: 0x002C   Alternate Control/Status Register	*/
	volatile unsigned int FUNSEL;		/*!< Offset: 0x0030   Function Select Register		*/
} UART_TypeDef;

#define GCR_BA		0xB0000000 /* Global Control */
#define REG_MFP_GPE_L	(GCR_BA+0x090)  /* GPIOE Low Byte Multiple Function Control Register */
#define UART0_BA	0xB8000000 /* UART0 Control (High-Speed UART) */
#define UART0		((UART_TypeDef *)UART0_BA)
#define REG_GPIOE_PUEN	0xB8003120
#define REG_PCLKEN0	0xB0000218

/*
 * Initialise the serial port with the given baudrate. The settings are always 8n1.
 */

u32 baudrate = CONFIG_BAUDRATE;
u32 ext_clk  = EXT_CLK;

int nuc970_serial_init (void)
{
	__raw_writel(__raw_readl(REG_PCLKEN0) | 0x10008, REG_PCLKEN0); 		// Enable UART and GPIO clock
	__raw_writel(__raw_readl(REG_GPIOE_PUEN) | 0x2, REG_GPIOE_PUEN);	// GPE1 (UART0 Rx) pull up
	__raw_writel((__raw_readl(REG_MFP_GPE_L) & 0xffffff00) | 0x99, REG_MFP_GPE_L); // UART0 multi-function

	/* UART0 line configuration for (115200,n,8,1) */
	UART0->LCR = 0x03;
	UART0->BAUD = 0x30000066;	/* 12MHz reference clock input, 115200 */
	UART0->FCR |=0x02;		// Reset UART0 Rx FIFO
	return 0;
}

void nuc970_serial_putc (const char ch)
{
	while ((UART0->FSR & 0x800000)); //waits for TX_FULL bit is clear
	UART0->x.THR = ch;
	if(ch == '\n')
	{
		while((UART0->FSR & 0x800000)); //waits for TX_FULL bit is clear
		UART0->x.THR = '\r';
	}
}

void nuc970_serial_puts (const char *s)
{
	while (*s) {
		nuc970_serial_putc (*s++);
	}
}

#ifdef CONFIG_CMD_JPEG
extern void JPEG_TRIGGER(void);
#endif
#ifdef CONFIG_KPI_NUC970

unsigned int u32KPI_value = 0;
unsigned int u32Pre_KPI_value = 0;


int nuc970_serial_and_kpi_getc (void)
{
	int i;

	while (1)
	{
		for(i=0;i<0x1000;i++);
		if (!(UART0->FSR & (1 << 14)))
		{
			return (UART0->x.RBR);
		}

        u32KPI_value = nuc970_KPI_Read();
	    if((u32KPI_value > 0) && (u32KPI_value < 33))
	    {
        	if(u32Pre_KPI_value != u32KPI_value)
            {
                u32Pre_KPI_value = u32KPI_value;

		    	if(u32KPI_value == KEY_P_ROW0_COL0)
 			    	return (0x31);
		    	if(u32KPI_value == KEY_P_ROW0_COL1)
 			    	return (0x32);
            }

	    }
	    else if(u32KPI_value >= 33)
	    {
	        if(u32Pre_KPI_value != u32KPI_value)
            {
                u32Pre_KPI_value = u32KPI_value;

		    	if(u32KPI_value == KEY_R_ROW0_COL0)
 			    	return (0x33);
		    	if(u32KPI_value == KEY_R_ROW0_COL1)
 			    	return (0x34);
            }

	    }

	}

}
#else
int nuc970_serial_getc (void)
{
	while (1)
	{
		if (!(UART0->FSR & (1 << 14)))
		{
			return (UART0->x.RBR);
		}
#ifdef CONFIG_CMD_JPEG
		JPEG_TRIGGER();
#endif
		WATCHDOG_RESET();
	}
}
#endif

int nuc970_serial_tstc (void)
{
#ifdef CONFIG_CMD_JPEG
	JPEG_TRIGGER();
#endif
	return (!((__raw_readl(UART0_BASE + REG_COM_MSR) & RX_FIFO_EMPTY)>>14));
}

void nuc970_serial_setbrg (void)
{

        return;
}

static struct serial_device nuc970_serial_drv = {
        .name   = "nuc970_serial",
        .start  = nuc970_serial_init,
        .stop   = NULL,
        .setbrg = nuc970_serial_setbrg,
        .putc   = nuc970_serial_putc,
        .puts   = nuc970_serial_puts,
        #ifdef CONFIG_KPI_NUC970
        .getc   = nuc970_serial_and_kpi_getc,
        #else
        .getc   = nuc970_serial_getc,
        #endif
        .tstc   = nuc970_serial_tstc,
};

void nuc970_serial_initialize(void)
{
        serial_register(&nuc970_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
        return &nuc970_serial_drv;
}
