/* definitions generated by preprocessor, copy into defines.h */

#ifndef	PPINC
#define	_ATMEGA1281	// device select: _ATMEGAxxxx
#define	_B1024	// boot size select: _Bxxxx (words), powers of two only
#include	<avr/io.h>

/* baud rate register value calculation */
#define	CPU_FREQ	7300000
#define	BAUD_RATE	19200


/* definitions for UART control */
#define	BAUD_RATE_LOW_REG	UBRR0L
#define	UART_CONTROL_REG	UCSR0B
#define	ENABLE_TRANSMITTER_BIT	TXEN0
#define	ENABLE_RECEIVER_BIT	RXEN0
#define	UART_STATUS_REG	UCSR0A
#define	TRANSMIT_COMPLETE_BIT	TXC0
#define	RECEIVE_COMPLETE_BIT	RXC0
#define	UART_DATA_REG	UDR0

/* definitions for SPM control */
#define	SPMCR_REG	SPMCSR
#define	PAGESIZE	128
#define	APP_END	100000
#define	LARGE_MEMORY

/* definitions for device recognition */
#define	PARTCODE	
#define	SIGNATURE_BYTE_1	0x1E
#define	SIGNATURE_BYTE_2	0x97
#define	SIGNATURE_BYTE_3	0x04

/* indicate that preprocessor result is included */
#define	PPINC
#endif

#ifndef __COUNTER_
#define __COUNTER_
uint32_t counter;
#endif

int BRREG_VALUE;

enum {
    /* This is expected number of cycles per jiffy at the platform's
       specified MHz. Assumes PLATFORM_MHZ == 1, 2, 4, 8 or 16. */
    MAGIC = 488 / (16 / (CPU_FREQ / 1000000))
  };

 uint16_t cycles;

void init(void);
int baudrateRegister(uint32_t baudrate);
