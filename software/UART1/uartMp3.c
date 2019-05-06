/******************************************************************************
 * File Name: uartMp3.c 
 *
 * Purpose  : This code controls the song to be played by MP3 decoder.
 *
 * Author   : Eswar, IITM
 *
 *
 ******************************************************************************/


#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <sys/signal.h>

//#define UART Base address definition
#define UART0_BASE_ADDRESS 0x11200
#define UART1_BASE_ADDRESS 0x11300

// UART TX (W) and RX (R) buffers
#define UART_DATA_REGISTER 0
// UART interrupt enable (RW)
#define UART_INT_ENABLE_REGISTER 1
// UART interrupt identification (R)
#define UART_INT_IDENTI_REGISTER 2
// UART FIFO control (W)
#define UART_FIFO_CTRL_REGISTER 2
// UART Line Control Register (RW)
#define UART_LINE_CTRL_REGISTER 3
// UART Modem Control (W)
#define UART_MODEM_CTRL_REGISTER 4
// UART Line Status (R)
#define UART_LINE_STATUS_REGISTER 5
// UART Modem Status (R)
#define UART_MODEM_STATUS_REGISTER 6
// UART base address of peripheral in NIOS memory map
#define UART_SCRATCH_REGISTER 7

// UART Divisor Registers offsets
#define UART_DIVISOR_LSB_LATCH_REGISTER 0
#define UART_DIVISOR_MSB_LATCH_REGISTER 1


#define INPUT_CLOCK_FREQ 20000000

#define UART0 0
#define UART1 1


int WriteIntoUart(unsigned int offSet, unsigned char uartSel, int writeData);(int *addr, int val)
{
	if(1 == uartSel )
		(unsigned int *)    (UART1_BASE_ADDRESS + offSet)  = writeData;
	else
		(unsigned int *)    (UART0_BASE_ADDRESS + offSet)  = writeData;

}

unsigned int ReadFromUart(unsigned int offset, uartSel)
{
	if(1 == uartSel )
		  return ((unsigned int *) (UART1_BASE_ADDRESS + offSet)) ;
	else
		  return ((unsigned int *) (UART0_BASE_ADDRESS + offSet)) ;
}



#if 0
int uart_init()
{
  asm volatile ("li t1, 0x11300" "\n\t"	//The base address of UART config registers
				"li t2, 0x83" "\n\t"	//Access divisor registers
				"sb t2, 24(t1)" "\n\t"	//Writing to UART_ADDR_LINE_CTRL
				"li t2, 0x0"	"\n\t"	//The upper bits of uart div
				"sb x0, 8(t1)" "\n\t"	//Storing upper bits of uart div
				"li t2, 0xB" "\n\t"	//The lower bits of uart div
				"sb t2, 0(t1)" "\n\t"			
				"li t2, 0x3" "\n\t"
				"sb t2, 24(t1)" "\n\t"
				"li t2, 0x6" "\n\t"
				"sb t2, 16(t1)" "\n\t"
				"sb x0, 8(t1)" "\n\n"
				:
				:
				:"x0","t1","t2", "cc", "memory");
	return 0;
}
#else
Uart1Init(int baud)
{
  int d = INPUT_CLOCK_FREQ / (16 * baud);
  printf("Set divisor to 0x%x\n",d);
#if 1
  WriteIntoUart(UART_LINE_CTRL_REGISTER, UART1, 0x83);
  WriteIntoUart(UART_DIVISOR_MSB_LATCH_REGISTER, UART1, d>>8);
  WriteIntoUart(UART_DIVISOR_LSB_LATCH_REGISTER, UART1, d & 0xff);
  WriteIntoUart(UART_LINE_CTRL_REGISTER, UART1, 0X03);
  WriteIntoUart(UART_FIFO_CTRL_REGISTER, UART1, 0X06);
  WriteIntoUart(UART_INT_ENABLE_REGISTER, UART1, 0X00);
#else
  uart_write_reg(UART_LINE_CTRL,0x83);  // access divisor registers
  uart_write_reg(1,d>>8);
  uart_write_reg(0,d & 0xff);
  uart_write_reg(UART_FIFO_CTRL,0x06);  // interrupt every 1 byte, clear FIFOs
  uart_write_reg(UART_INT_ENABLE,0x00);   // disable interrupts
#endif
}

#endif
unsigned int UartTxReady()
{
	unsigned int readData = 0;
	readData = ReadFromUart(UART_LINE_STATUS_REGISTER, 1);
	return readData;
}

void UartTransmitCharacter(char c)
{
  unsigned int status, tx_empty;

	
	while(1)
	{
		status = UartTxReady();
		if( 1 == ( (status >> 5) & 0x1) )
			break;
	}
  // tx must be empty now so send
	  WriteIntoUart(UART_DATA_REGISTER, UART1, (unsigned int) c);
}


void UartTransmitString(char *s)
{
  for(; *s != NULL; s++)
    UartTransmitCharacter(*s);
}


int UartCheckReceiveReady(void)
{
	unsigned int readData = 0;
	readData = ReadFromUart(UART_LINE_STATUS_REGISTER, UART1);
	return (readData & 0x01);
}


char UartReceiveCharacter(void)
{
  while(!UartCheckReceiveReady()) ;
  return (unsigned char) ReadFromUart(UART_DATA_REGISTER, UART1);
}

#if 0
int
uart_rx_string_check(char *s)
{
  for(; *s != 0; s++) {
    char c = uart_rx_char();
    if(c != *s) {
      if((c<' ') || (*s<' '))
	alt_printf("uart_rx_string_check: received 0x%x but expected 0x%x\n", c, *s);
      else	
	alt_printf("uart_rx_string_check: received %c but expected %c\n", c, *s);
      return (1 == 0);
    }
  }
  return (0==0);
}


void
uart_fifo_flush(void)
{
  int j;
  uart_write_reg(UART_FIFO_CTRL, 0x07 | fifo_trigger_level);  // FIFO enable and reset TX and RX FIFOs
  uart_write_reg(UART_FIFO_CTRL, 0x01 | fifo_trigger_level);  // FIFO enable
  for(j=1000; (j>0); j--)
    if(uart_rx_ready())
      uart_rx_char();
}
#endif






void main()
{
//Initialises UART 0
	Uart0Init();

//Initialises UART 0
	Uart1Init(9600);
	UartTransmitCharacter('T');
	printf("%c", UartReceiveCharacter() );
	UartTransmitCharacter('e');
	printf("%c", UartReceiveCharacter() );
	UartTransmitCharacter('s');
	printf("%c", UartReceiveCharacter() );
	UartTransmitCharacter('t');
	printf("%c", UartReceiveCharacter() );


//MP3 Initialisation
	


}

