#ifndef QSPI_H
#define QSPI_H

#include<stdlib.h>
#include<stdint.h>

//!Timeout value which will be used to check the completion of command execution.
#define DEF_TIMEOUT 60

/** 
 * @def READ_ID 
 * Instruction to read the ID of memory device
 * @def READ_SR 
 * Instruction to read the status bits of Status register
 * @def WR_EN 
 * Instruction to enable write mode
 * @def FOURBYTE_AD 
 * Instruction to enter 4 byte addressing mode
 * @def WR_VCR 
 * Instruction to write volatile configuration register
 * @def FAST_RD 
 * Instruction to enable Single I/O fast read mode
 * @def QDFAST_RD 
 * Instruction to enable Quad I/O fast read mode
 */

#define MEM_TYPE_N25Q256_ID 0x20BA1910

#define READ_ID 0x9E		
#define READ_SR 0x05		
#define WR_EN 0x06		
#define FOURBYTE_AD 0xB7	
#define WR_VCR 0x81		
#define FAST_RD 0x0B		
#define QDFAST_RD 0xEB		

//Memory Maps
//!Memory location of Control register
#define CR      0x11800 	
//!Memory location of Device configuration register
#define DCR     0x11804		
//!Memory location of Status register
#define SR      0x11808		
//!Memory location of Flag clear register
#define FCR     0x1180c		
//!Memory location of Data length register
#define DLR     0x11810		
//!Memory location of Communication configuration register
#define CCR     0x11814	
//!Memory location of Address register
#define AR      0x11818		
//!Memory location of Alternate bytes register
#define ABR     0x1181c	
//!Memory location of Data register
#define DR      0x11820		
//!Memory location of Polling status mask register
#define PSMKR   0x11824		
//!Memory location of Polling status match register
#define PSMAR   0x11828		
//!Memory location of Polling interval register
#define PIR     0x1182c		
//!Memory location of Low power timeout register
#define LPRT    0x11830		
//!Starting point of memory 
#define STARTMM 0x90000000
//!Ending point of memory
#define ENDMM   0x9FFFFFFF
//Defines for configuring the registers at ease
//Bit vectors for all the parameters in the CR
//!Sets the clock prescaler
#define CR_PRESCALER(x)   (x<<24)
//!Polling match mode. This bit indicates which method should be used for determining a match during automatic polling mode..modified only when BUSY=0
#define CR_PMM            (1<<23)
//!Automatic Poll Mode Stop. This bit determines if automatic polling is stopped after a match..can be modified only when BUSY=0
#define CR_APMS           (1<<22)
//!Timeout interrupt enable. This bit enables the timeout interrupt
#define CR_TOIE           (1<<20)
//!Status Match Interrupt Enable. This bit enables the status match interrupt
#define CR_SMIE           (1<<19)
//!FIFO threshold interrupt enable. This bit enables the fifo threshold interrupt
#define CR_FTIE           (1<<18)
//!Transfer complete interrupt enable. This bit enables the transfer complete interrupt
#define CR_TCIE           (1<<17)
//!Transfer error interrupt enable. This bit enables the transfer error interrupt
#define CR_TEIE           (1<<16)
//!FIFO threshold level. In indirect mode, this defines the threshold number of bytes in then fifo that will cause the fifo threshold flag(ftf, quadspi_sr[2]) to be set in indirect write mode(fmode=00).. in indirect read mode(fmode=01) if dmaen=1 then the dma controller for the corresponding channel must be disabled before changing the FTHRES value
#define CR_FTHRES(x)      (x<<8 )
//!Flash memory selection. This bit selects the flash memory to be addressed in single flash mode(when dfm=0)..modified only when busy=0..ignored when dfm=1
#define CR_FSEL           (1<<7 )
//!Dual flash mode. This bit activates the dual flash modde..where two external flsh memories are used simulataneously to double the throughput and capacity..modified only when BUSY=0
#define CR_DFM            (1<<6 )
//!Sample shift. By default, the QUADSPI samples data 1/2 of a CLK cycle after the data is driven by the Flash memory. This bit allows the data is to be sampled later in order to account for external signal delays. Firmware must assure that SSHIFT = 0 when in DDR mode (when DDRM = 1). This field can be modified only when BUSY = 0.
#define CR_SSHIFT         (1<<4 )
//!Timeout counter enable. This bit is valid only when memory-mapped mode (FMODE = 11) is selected. Activating this bit causes the chip select (nCS) to be released (and thus reduces consumption) if there has not been an access after a certain amount of time, where this time is defined by TIMEOUT[15:0] (QUADSPI_LPTR). Enable the timeout counter. By default, the QUADSPI never stops its prefetch operation, keeping the previous read operation active with nCS maintained low, even if no access to the Flash memory occurs for a long time. Since Flash memories tend to consume more when nCS is held low, the application might want to activate the timeout counter (TCEN = 1, QUADSPI_CR[3]) so that nCS is released after a period of TIMEOUT[15:0] (QUADSPI_LPTR) cycles have elapsed without an access since when the FIFO becomes full with prefetch data. This bit can be modified only when BUSY = 0.
#define CR_TCEN           (1<<3 )
//!DMA enable. In indirect mode, DMA can be used to input or output data via the QUADSPI_DR register. DMA transfers are initiated when the FIFO threshold flag, FTF, is set.
#define CR_DMAEN          (1<<2 )
//!Abort request. This bit aborts the on-going command sequence. It is automatically reset once the abort is complete. This bit stops the current transfer. In polling mode or memory-mapped mode, this bit also resets the APM bit or the DM bit.
#define CR_ABORT          (1<<1 )
//!This bit will enable the QUADSPI
#define CR_EN             (1<<0 )

//Bit vectors for DCR 
//!Flash memory size. This field defines the size of external memory using the following formula. Number of bytes in Flash memory = 2[FSIZE+1] FSIZE+1 is effectively the number of address bits required to address the Flash memory. The Flash memory capacity can be up to 4GB (addressed using 32 bits) in indirect mode, but the addressable space in memory-mapped mode is limited to 256MB. If DFM = 1, FSIZE indicates the total capacity of the two Flash memories together. This field can be modified only when BUSY = 0.
#define DCR_FSIZE(x)       (x<<16)
//!This macro is not used in the code. Probably this can be used to mention the number of bytes taken to set the mode.
#define DCR_MODE_BYTE(x)   (x<<21)
//!Chip Select High Time. This bit defines the minimum number of CLK cycles which the chip select (nCS) must remain high between commands issued to the Flash memory. This field can be modified only when BUSY = 0.
#define DCR_CSHT(x)        (x<<8 )
//!Clock mode. Indicates the level that clk takes between command
#define DCR_CKMODE         0x1 	  

//Bit vectors for status register
//!FIFO level. This field gives the number of valid bytes which are being held in the FIFO. FLEVEL = 0 when the FIFO is empty, and 16 when it is full. In memory-mapped mode and in automatic status polling mode, FLEVEL is zero.
#define SR_FLEVEL(x)      (x<<8)
//!Timeout flag. This bit is 1 when timeout occurs. It is cleared by writing 1 to CTOF
#define SR_TOF            (1<<4)
//!Status match flag. This bit is set in automatic polling mode when the unmasked received data matches the corresponding bits in the match register (QUADSPI_PSMAR). It is cleared by writing 1 to CSMF.
#define SR_SMF            (1<<3)
//!FIFO threshold flag. In indirect mode, this bit is set when the FIFO threshold has been reached, or if there is any data left in the FIFO after reads from the Flash memory are complete. It is cleared automatically as soon as threshold condition is no longer true. In automatic polling mode this bit is set every time the status register is read, and the bit is cleared when the data register is read.
#define SR_FTF            (1<<2)
//!Transfer complete flag. This bit is set in indirect mode when the programmed number of data has been transferred or in any mode when the transfer has been aborted. It is cleared by writing 1 to CTCF
#define SR_TCF            (1<<1)
//!Transfer error flag. This bit is set in indirect mode when an invalid address is being accessed in indirect mode. It is cleared by writing 1 to CTEF
#define SR_TEF            (1<<0)

//Bit vectors for flag clear register 
//!Clear timeout flag. Writing 1 clears the TOF flag in the QUADSPI_SR register
#define FCR_CTOF (1<<4)
//!Clear status match flag. Writing 1 clears the SMF flag in the QUADSPI_SR register
#define FCR_CSMF (1<<3)
//!Clear transfer complete flag. Writing 1 clears the TCF flag in the QUADSPI_SR register
#define FCR_CTCF (1<<1)
//!Clear transfer error flag. Writing 1 clears the TEF flag in the QUADSPI_SR register
#define FCR_CTEF (1<<0)

//Bit vectors for DLR
//!Useless -- but for better readability of the code
#define DL(x)  x  

//Bit vectors for CCR
//!Double data rate mode. This bit sets the DDR mode for the address, alternate byte and data phase. This field can be written only when BUSY = 0.
#define CCR_DDRM                   (1<<31) 
//!DDR hold. Delay the data output by 1/4 of the QUADSPI output clock cycle in DDR mode. This feature is only active in DDR mode. This field can be written only when BUSY = 0.
#define CCR_DHHC                   (1<<30) 
//!Needed by Micron Flash Memories
#define CCR_DUMMY_BIT              (1<<29) 
//!Send instruction only once mode. This bit has no effect when IMODE = 00. This field can be written only when BUSY = 0.
#define CCR_SIOO                   (1<<28) 
//!Functional mode. This field defines the QUADSPI functional mode of operation. If DMAEN = 1 already, then the DMA controller for the corresponding channel must be disabled before changing the FMODE value. This field can be written only when BUSY = 0.
#define CCR_FMODE(x)               (x<<26) 
//!Data mode. This field defines the data phases mode of operation. This field also determines the dummy phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_DMODE(x)               (x<<24) 
//!Needed by Micron Flash Memories
#define CCR_DUMMY_CONFIRMATION     (1<<23) 
//!Number of dummy cycles. This field defines the duration of the dummy phase. In both SDR and DDR modes, it specifies a number of CLK cycles (0-31). This field can be written only when BUSY = 0.
#define CCR_DCYC(x)                (x<<18) 
//!Alternate bytes size. This bit defines alternate bytes size. This field can be written only when BUSY = 0.
#define CCR_ABSIZE(x)              (x<<16) 
//!Alternate bytes mode. This field defines the alternate-bytes phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_ABMODE(x)              (x<<14) 
//!Address size. This bit defines address size. This field can be written only when BUSY = 0.
#define CCR_ADSIZE(x)              (x<<12) 
//!Address mode. This field defines the address phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_ADMODE(x)              (x<<10) 
//!Instruction mode. This field defines the instruction phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_IMODE(x)               (x<<8 )
//!Instruction. Instruction to be sent to the external SPI device. This field can be written only when BUSY = 0.
#define CCR_INSTRUCTION(x)         (x<<0 ) 

//!Indirect write mode
#define CCR_FMODE_INDWR 0x0 
//!Indirect read mode
#define CCR_FMODE_INDRD 0x1 
//!Automatic polling mode
#define CCR_FMODE_APOLL 0x2 
//!Memory mapped mode
#define CCR_FMODE_MMAPD 0x3 

//!Alias for 0x0
#define NDATA  0x0 
//!Alias for 0x1
#define SINGLE 0x1 
//!Alias for 0x2
#define DOUBLE 0x2 
//!Alias for 0x3
#define QUAD   0x3 

//!Alias for 0x0
#define BYTE      0x0 
//!Alias for 0x1
#define TWOBYTE   0x1 
//!Alias for 0x2
#define THREEBYTE 0x2 
//!Alias for 0x3
#define FOURBYTE  0x3 

//!Pointer to the memory location of Control register
int* cr       =      (const int*) CR;	
//!Pointer to the memory location of Device configuration register
int* dcr      =      (const int*) DCR;	
//!Pointer to the memory location of Status register
int* sr       =      (const int*) SR;	
//!Pointer to the memory location of Flag clear register
int* fcr      =      (const int*) FCR;	
//!Pointer to the memory location of Communication configuration register
int* ccr      =      (const int*) CCR;	
//!Pointer to the memory location of Address register
int* ar       =      (const int*) AR;	
//!Pointer to the memory location of Alternate bytes register
int* abr      =      (const int*) ABR;	
//!Pointer to the memory location of Data register
int* dr       =      (const int*) DR;	
//!Pointer to the memory location of Data length register
int* dlr      =      (const int*) DLR;	
//!Pointer to the memory location of Polling status mask register
int* psmkr    =      (const int*) PSMKR;
//!Pointer to the memory location of Polling interval register
int* pir      =      (const int*) PIR;	
//!Pointer to the memory location of Low power timeout register
int* lprt     =      (const int*) LPRT;	
//!Pointer to the starting memory location
int* startmm  =      (const int*) STARTMM;
//!Pointer to the ending memory location
int* endmm    =      (const int*) ENDMM;

/**
 * @brief Set the value at a memory location.
 * 
 * This function is used in several places to set different modes in the board by enabling various bits and passing certain instructions.
 * This function is particularly used for 32 bit memories.
 * 
 * @param *addr Address of the memory location 
 * @param val Value to be stored in the location
 *
 * @return Void.
 */
void set_qspi_shakti32(int* addr, int val)
{
    *addr = val;
}

/**
 * @brief Set the value at a memory location.
 *
 * This function is used in several places to set different modes in the board by enabling various bits and passing certain instructions.
 * This function is particularly used for 16 bit memories.
 * 
 * @param *addr Address of the memory location 
 * @param val Value to be stored in the location
 *
 * @return Void.
 */
void set_qspi_shakti16(int16_t* addr, int16_t val)
{
    *addr = val;
}

/**
 * @brief Set the value at a memory location.
 *
 * This function is used in several places to set different modes in the board by enabling various bits and passing certain instructions.
 * This function is particularly used for 8 bit memories.
 * 
 * @param *addr Address of the memory location 
 * @param val Value to be stored in the location
 *
 * @return Void.
 */
void set_qspi_shakti8(char* addr, char val)
{
    *addr= val;
}

/**
 * @brief Get the value at a memory location.
 *
 * This function is used to get the value from a particular address of a memory location.
 * 
 * @param *addr Address of the memory location 
 *
 * @return @a *addr The value at @a addr memory location.
 */
int get_qspi_shakti(int* addr)
{
 return *addr;
}

/**
 * @brief Delay function
 *
 * This function uses a loop which does no particular operation and simply delays the program for the previous instruction to execute completely.
 * It uses an unsigned integer variable @a time which gets incremented i the loop till it reaches the given time to be delayed.
 *
 * @param secs The amount of time by which the program should be delayed.
 *
 * @return Void 
 */
void waitfor(unsigned int secs) {
	unsigned int time = 0;
	while(time++ < secs);
}

/**
 * @brief Initializes various parameters needed by the device on startup.
 *
 * This function enables all the interrupts by setting the interrupt bits in the Control register. It also sets the Flash emory size, Chip select high time and the clock mode in the Device configuration register.
 * It also sets the Prescaler, FIFO threshold level and also enables the device from the Control register.
 * 
 * @see set_qspi_shakti32().
 *
 * @param fsize Flash memory size.
 * @param csht Chip select high time.
 * @param prescaler Clock prescaler.
 * @param enable_interrupts Will enable interrupts 
 * @param fthreshold FIFO threshold
 *
 * @return Void.
 */
void qspi_init(int fsize, int csht, int prescaler, int enable_interrupts, int fthreshold){
    int int_vector = enable_interrupts? (CR_TOIE|CR_SMIE|CR_FTIE|CR_TCIE|CR_TEIE) : 0; 
    set_qspi_shakti32(dcr,(DCR_FSIZE(fsize)|DCR_CSHT(csht)|DCR_CKMODE)); 
    set_qspi_shakti32(cr,(CR_PRESCALER(prescaler)|int_vector|CR_FTHRES(fthreshold)|CR_EN));
}

/**
 * @brief Clear all the flags from the status register.
 *
 * This function is used to disable all the flags which were enabled when different modes were set.
 * This function simply enables the bits of the Flag clear register which will reset all the flags.
 *
 * @return Void.
 */
void reset_interrupt_flags(){
    set_qspi_shakti32(fcr,(FCR_CTOF|FCR_CSMF|FCR_CTCF|FCR_CTEF)); //Resetting all the flags
}

/**
 * @brief Waits for a command to complete execution.
 *
 * This function is used to check if the memory device is feasible to complete an instruction.
 * It uses a variable @a timeout which will initially be assigned to value @a DEF_TIMEOUT and it gets decremented inside a loop where the status of the device will be updated at every step and the updated status will be stored in @a status.
 * In case, the request gets timed out i.e if @a timeout becomes 0 then the function 
 * 
 * @param *status Status is used just as a variable and it will get updated at each cycle of the loop. 
 *
 * @see set_qspi_shakti32()
 *
 * @return 0 on success, -1 otherwise.
 */
int wait_for_tcf(int status){
    int timeout = DEF_TIMEOUT; 

    status = get_qspi_shakti(sr);
    
    //printf("status : %d looping?",status);
    
    while(!( status & 0x02 ) && --timeout){
        waitfor(1000);
        status=get_qspi_shakti(sr);
    }
    if(timeout==0){
        printf("Request Timed out");
        return -1;
    }
    return 0;
}

/**
 * @brief To write enable by passing the instruction 0x06 to the memory device.
 *
 * This function is used to check if the memory device can be used to write data into it.
 * This function sets the Communication configuration register in single write mode and waits for the completion of execution.
 * 
 * @param status Status of the memory device
 *
 * @see set_qspi_shakti32(), wait_for_tcf()
 *
 * @return @a ret Value that is returned by wait_for_tcf().
 */
int micron_write_enable(int status){
    printf("\tWrite Enable\n");
    set_qspi_shakti32(ccr,(CCR_ADSIZE(FOURBYTE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(WR_EN)));
    int ret = wait_for_tcf(status); //Indicating the completion of command -- Currently polling
    printf("Status : %d dcr : %d cr: %d ccr: %d \n",status,*dcr,*cr,*ccr);
    return ret;
}

/**
 * @brief To enable 4 byte addressing mode.
 *
 * This function is used to check if 4 byte memory addressing can be enabled
 * The instruction passed is 0xB7.
 * 
 * @param status Status of the memory device
 *
 * @see set_qspi_shakti32(), wait_for_tcf()
 *
 * @return @a ret Value that is returned by wait_for_tcf().
 */
int micron_enable_4byte_addressing(int status){
    printf("\t Enable 4 byte address \n");
    set_qspi_shakti32(ccr,(CCR_ADSIZE(FOURBYTE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(FOURBYTE_AD)));
    int ret = wait_for_tcf(status); //Indicating the completion of command -- Currently polling
    printf("Status : %d dcr : %d cr: %d ccr: %d \n",status,*dcr,*cr,*ccr);
    return ret;
}

/**
 * @brief To enable XIP using Volatile configuration register.
 *
 * This function is used to enable XIP from the volatile configuration register.
 * 
 * In order to write the volatile configuration register, the memory device is passed with instruction 0x81.
 * To enable XIP the value 0x93 has to be written into the volatile configuration register.
 *
 * @param status Status of the memory device
 * @param value The value to be written into the VECR register to enable XIP. Indicating XIP to operate in Quad Mode.
 *
 * @see set_qspi_shakti8(), set_qspi_shakti32(), wait_for_tcf()
 *
 * @return @a ret Value that is returned by wait_for_tcf().
 */
int micron_configure_xip_volatile(int status, int value){
    printf("\tWrite Volatile Configuration Register\n");
    set_qspi_shakti32(dlr,DL(1));
    set_qspi_shakti8(dr,value);  //The value to be written into the VECR register to enable XIP. Indicating XIP to operate in Quad Mode
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDWR)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(WR_VCR)));
    waitfor(50);
    int ret = wait_for_tcf(status);
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d dr: %d\n",status,*dcr,*cr,*ccr,*dlr,*dr);
    waitfor(50);  //Giving Micron time to store the data
    return ret;
}

/*int micron_disable_xip_volatile(int status, int value){
    printf("\tWrite Volatile Configuration Register to exit XIP\n");
    set_qspi_shakti32(cr,(CR_PRESCALER(0x3)|CR_TOIE|CR_TCIE|CR_TEIE|CR_SMIE|CR_FTIE|CR_ABORT|CR_EN));
    waitfor(30);
    qspi_init(27,0,3,1,15);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_BIT|CCR_DUMMY_CONFIRMATION|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA)));
    set_qspi_shakti32(ar,0x00000); //Dummy address 
    set_qspi_shakti32(dlr,1);
    int ret = wait_for_tcf(status);
    waitfor(50);
    reset_interrupt_flags();
    return ret;
}
*/

/**
 * @brief Function to read the ID of memory device.
 *
 * This function sets the memory device to indirect read mode and it passes the instruction 0x9E which gives the ID of memory device. 
 * This function also ensures the device to complete execution of a command.
 * It will return 0 if device is detected and will proceed further execution, and will return -1 otherwise.
 * 
 * @see set_qspi_shakti32(), wait_for_tcf().
 * 
 * @return 0 on success, -1 otherwise.
 */
int micron_read_id_cmd(int status, int value){
    printf("\tRead ID Command to see if the Protocol is Proper\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(READ_ID)|CCR_DMODE(SINGLE)));
    int ret = wait_for_tcf(status);
    value = get_qspi_shakti(dr);
    printf("Read ID: %08x",value);
    return ret;
}

/*int micron_read_configuration_register(int status, int value){
    printf("\tRead ID Command to see if the Protocol is Proper\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xBE)|CCR_DMODE(SINGLE)));
    int ret = wait_for_tcf(status);
    value = get_qspi_shakti(dr);
    printf("Configuration Register Value: %08x",value);
    return ret;
}*/

#endif
