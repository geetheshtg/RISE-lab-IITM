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
#include <unistd.h>
#include <stdlib.h>
#include <sys/signal.h>
#include "uart1mp3.h"

#define DELAY1 1000
#define DELAY2 2000


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




void WriteIntoUart(unsigned char offSet, unsigned char uartSel, unsigned char writeData)
{
	int * writeAddress = NULL;
	if(1 == uartSel )
		writeAddress = (UART1_BASE_ADDRESS + offSet);
	else
		writeAddress = (UART0_BASE_ADDRESS + offSet);

		*writeAddress  = writeData;

}

unsigned char ReadFromUart(unsigned char offSet, unsigned char uartSel)
{
	int * readAddress = NULL;
	if(1 == uartSel )
		readAddress = (UART1_BASE_ADDRESS + offSet);
	else
		readAddress = (UART0_BASE_ADDRESS + offSet);
	return (*readAddress) ;
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

void Uart0InitBcm(int baud)
{
  int d = INPUT_CLOCK_FREQ / (16 * baud);
  printf("Set divisor to 0x%x\n",d);
  WriteIntoUart(UART_LINE_CTRL_REGISTER, UART0, 0x83);
  WriteIntoUart(UART_DIVISOR_MSB_LATCH_REGISTER, UART0, d >> 8);
  WriteIntoUart(UART_DIVISOR_LSB_LATCH_REGISTER, UART0, d & 0xff);
  WriteIntoUart(UART_LINE_CTRL_REGISTER, UART0, 0X03);
  WriteIntoUart(UART_FIFO_CTRL_REGISTER, UART0, 0X06);
  WriteIntoUart(UART_INT_ENABLE_REGISTER, UART0, 0X00);
}
#else

void Uart1Init(int baud)
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



#if 0
/********************************************************************************/
/*Function sendMP3Command: seek for a 'c' command and send it to MP3  */
/*Parameter: c. Code for the MP3 Command, 'h' for help.                                                                                                         */
/*Return:  void                                                                */

void sendMP3Command(char c) {
  
  switch (c) {
    case '?':
    case 'h':
      printf("HELP  ");
      printf(" p = Play");
      printf(" P = Pause");
      printf(" > = Next");
      printf(" < = Previous");
      printfn(" s = Stop Play"); 
      printf(" + = Volume UP");
      printf(" - = Volume DOWN");
      printf(" c = Query current file");
      printf(" q = Query status");
      printf(" v = Query volume");
      printf(" x = Query folder count");
      printf(" t = Query total file count");
      printf(" f = Play folder 1.");
      printf(" S = Sleep");
      printf(" W = Wake up");
      printf(" r = Reset");
      break;


    case 'p':
      printf("Play ");
      sendCommand(CMD_PLAY);
      break;

    case 'P':
      printf("Pause");
      sendCommand(CMD_PAUSE);
      break;

    case '>':
      printf("Next");
      sendCommand(CMD_NEXT_SONG);
      sendCommand(CMD_PLAYING_N); // ask for the number of file is playing
      break;

    case '<':
      printfn("Previous");
      sendCommand(CMD_PREV_SONG);
      sendCommand(CMD_PLAYING_N); // ask for the number of file is playing
      break;

    case 's':
      printfn("Stop Play");
      sendCommand(CMD_STOP_PLAY);
      break;


    case '+':
      printf("Volume Up");
      printf(CMD_VOLUME_UP);
      break;

    case '-':
      printf("Volume Down");
      sendCommand(CMD_VOLUME_DOWN);
      break;

    case 'c':
      printf("Query current file");
      sendCommand(CMD_PLAYING_N);
      break;

    case 'q':
      printf("Query status");
      sendCommand(CMD_QUERY_STATUS);
      break;

    case 'v':
      printf("Query volume");
      sendCommand(CMD_QUERY_VOLUME);
      break;

    case 'x':
      printf("Query folder count");
      sendCommand(CMD_QUERY_FLDR_COUNT);
      break;

    case 't':
      printf("Query total file count");
      sendCommand(CMD_QUERY_TOT_TRACKS);
      break;

    case 'f':
      printf("Playing folder 1");
      sendCommand(CMD_FOLDER_CYCLE, 1, 0);
      break;

    case 'S':
      printf("Sleep");
      sendCommand(CMD_SLEEP_MODE);
      break;

    case 'W':
      printf("Wake up");
      sendCommand(CMD_WAKE_UP);
      break;

    case 'r':
      printf("Reset");
      sendCommand(CMD_RESET);
      break;
  }
}



/********************************************************************************/
/*Function decodeMP3Answer: Decode MP3 answer.                                  */
/*Parameter:-void                                                               */
/*Return: The                                                  */

String decodeMP3Answer() {
  String decodedMP3Answer = "";

  decodedMP3Answer += sanswer();

  switch (ansbuf[3]) {
    case 0x3A:
      decodedMP3Answer += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer += " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer += " -> Error";
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly. ";
      break;

    case 0x42:
      decodedMP3Answer += " -> Status playing: " + String(ansbuf[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(ansbuf[6], DEC);
      break;

    case 0x4C:
      decodedMP3Answer += " -> Playing: " + String(ansbuf[6], DEC);
      break;

    case 0x4E:
      decodedMP3Answer += " -> Folder file count: " + String(ansbuf[6], DEC);
      break;

    case 0x4F:
      decodedMP3Answer += " -> Folder count: " + String(ansbuf[6], DEC);
      break;
  }

  return decodedMP3Answer;
}





/********************************************************************************/
/*Function: Send command to the MP3                                             */
/*Parameter: byte command                                                       */
/*Parameter: byte dat1 parameter for the command                                */
/*Parameter: byte dat2 parameter for the command                                */

void sendCommand(byte command){
  sendCommand(command, 0, 0);
}



/********************************************************************************/
/*Function: sbyte2hex. Returns a byte data in HEX format.                       */
/*Parameter:- uint8_t b. Byte to convert to HEX.                                */
/*Return: String                                                                */


String sbyte2hex(uint8_t b)
{
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}


/********************************************************************************/
/*Function: shex2int. Returns a int from an HEX string.                         */
/*Parameter: s. char *s to convert to HEX.                                      */
/*Parameter: n. char *s' length.                                                */
/*Return: int                                                                   */

int shex2int(char *s, int n){
  int r = 0;
  for (int i=0; i<n; i++){
     if(s[i]>='0' && s[i]<='9'){
      r *= 16; 
      r +=s[i]-'0';
     }else if(s[i]>='A' && s[i]<='F'){
      r *= 16;
      r += (s[i] - 'A') + 10;
     }
  }
  return r;
}


/********************************************************************************/
/*Function: sanswer. Returns a String answer from mp3 UART module.          */
/*Parameter:- uint8_t b. void.                                                  */
/*Return: String. If the answer is well formated answer.                        */

String sanswer(void)
{
  uint8_t i = 0;
  String mp3answer = "";

  // Get only 10 Bytes
  while (mp3.available() && (i < 10))
  {
    uint8_t b = mp3.read();
    ansbuf[i] = b;
    i++;

    mp3answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF))
  {
    return mp3answer;
  }

  return "???: " + mp3answer;
}


#endif

void sendCommand(unsigned char command, unsigned char dat1, unsigned char dat2){
//  delay(20);
  Send_buf[0] = 0x7E;    //
  Send_buf[1] = 0xFF;    //
  Send_buf[2] = 0x06;    // Len
  Send_buf[3] = command; //
  Send_buf[4] = 0x01;    // 0x00 NO, 0x01 feedback
  Send_buf[5] = dat1;    // datah
  Send_buf[6] = dat2;    // datal
  Send_buf[7] = 0xEF;    //
  printf("Sending: ");
  for (uint8_t i = 0; i < 8; i++)
  {
    WriteIntoUart(UART_DATA_REGISTER, UART1, Send_buf[i]) ;
 //   (sbyte2hex(Send_buf[i]));
  }
//  Serial.println();
}



void Delay(uint16_t Cnt1, uint16_t Cnt2)
{
    uint16_t i = 0, j = 0;
    for( i = 0; i < Cnt1; i++)
        for( j = 0; j < Cnt2; j++);
    
}


void pausePlay()
{
  sendCommand(0x0E,0,0);
    Delay(DELAY1,DELAY2);
}


void play()
{
  sendCommand(0x0D,0,1); 
    Delay(DELAY1,DELAY2);
}


void playNext()
{
  sendCommand(0x01,0,1);
    Delay(DELAY1,DELAY2);
}


void playPrevious()
{
  sendCommand(0x02,0,1);
    Delay(DELAY1,DELAY2);
}


void setVolume(int volume)
{
  sendCommand(0x06, 0, volume); // Set the volume (0x00~0x30)
    Delay(DELAY1,DELAY2);
}

void playFirst()
{
  printf("song playing command sent\n");

  sendCommand(0x3F, 0, 0);
    Delay(DELAY1,DELAY2);
  setVolume(30);
    Delay(DELAY1,DELAY2);
  sendCommand(0x11,0,0x1); 
    Delay(DELAY1,DELAY2);
  printf("song playing command sent complete\n");
}

void StopPlay()
{
  sendCommand(0x16, 0, 0);
    Delay(DELAY1,DELAY2);
}
/*
Input clock frequency:
baudrate: 9600, countvalue: 130 (0x82)
baudrate: 19200, countvalue: 65 (0x41)
baudrate: 38400, countvalue: 32 (0x21)
baudrate: 57600, countvalue: 21 (0x15)
baudrate: 115200, countvalue:11  (0xb)

*/



int Uart0InitBcm()
{
  asm volatile ("li t1, 0x11200" "\n\t"	//The base address of UART config registers
				"li t2, 0x83" "\n\t"	//Access divisor registers
				"sb t2, 24(t1)" "\n\t"	//Writing to UART_ADDR_LINE_CTRL
				"li t2, 0x0"	"\n\t"	//The upper bits of uart div
				"sb x0, 8(t1)" "\n\t"	//Storing upper bits of uart div
				"li t2, 0xb" "\n\t"	//The lower bits of uart div
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

void delay(unsigned long cntr1, unsigned long cntr2)
{
    unsigned long tempCntr = 0;
    while(cntr1--)
    {
	tempCntr = cntr2;
	while(tempCntr--);
    }


}

void main()
{
//Initialises UART 0
//	Uart0Init();
//	printf("Test code");
//	exit(1);
//Initialises UART 0
	Uart0InitBcm(115200);

	

//MP3 Initialisation
//	sendCommand(CMD_SET_VOLUME, 0,25);
//	sendCommand(CMD_PLAY, 0, 0);
//	playFirst();

    WriteIntoUart(UART_DATA_REGISTER, UART0, 'G') ;
    delay(10,1000);
    WriteIntoUart(UART_DATA_REGISTER, UART0, '2') ;
    delay(10,1000);
    WriteIntoUart(UART_DATA_REGISTER, UART0, '8') ;
    delay(10,1000);
    WriteIntoUart(UART_DATA_REGISTER, UART0, ' ') ;
    delay(10,1000);
    WriteIntoUart(UART_DATA_REGISTER, UART0, 'X') ;
    delay(10,1000);
//    sleep(30);
//    StopPlay();

}

