#ifndef QSPI_H
#define QSPI_H

#include<stdlib.h>
#include<stdint.h>

#define DEF_TIMEOUT 60

//Memory Maps
#define CR      0x11800 	//Control register
#define DCR     0x11804		//Device configuration register
#define SR      0x11808		//Status register
#define FCR     0x1180c		//Flag clear register
#define DLR     0x11810		//Data length register
#define CCR     0x11814		//communication configuration register
#define AR      0x11818		//address register
#define ABR     0x1181c		//alternate bytes register
#define DR      0x11820		//data register
#define PSMKR   0x11824		//polling status mask register
#define PSMAR   0x11828		//polling status match register
#define PIR     0x1182c		//polling interval register
#define LPRT    0x11830		//low power timeout register
#define STARTMM 0x90000000	
#define ENDMM   0x9FFFFFFF
//Defines for configuring the registers at ease
//Bit vectors for all the parameters in the CR
#define CR_PRESCALER(x)   (x<<24)//prescaler..clock prescaler
#define CR_PMM            (1<<23)//polling match mode..this bit indicates which method should be used for determining a match during automatic polling mode..modified only when BUSY=0
#define CR_APMS           (1<<22)//automatic poll mode stop..this bit determines if automatic polling is stopped after a match..can be modified only when BUSY=0
#define CR_TOIE           (1<<20)//timeout interrupt enable..this bit enables the timeout interrupt
#define CR_SMIE           (1<<19)//status match interrupt enable..this bit enables the status match interrupt
#define CR_FTIE           (1<<18)//fifo threshold interrupt enable..this bit enables the fifo threshold interrupt
#define CR_TCIE           (1<<17)//transfer complete interrupt enable..this bit enables the rransfer complete interrupt
#define CR_TEIE           (1<<16)//transfer error interrupt enable..this bit enables the transfer error interrupt
#define CR_FTHRES(x)      (x<<8 )//fifo threshold level..in indirect mode, this defines the threshold number of bytes in then fifo that will cause the fifo threshold flag(ftf, quadspi_sr[2]) to be set in indirect write mode(fmode=00).. in indirect read mode(fmode=01) if dmaen=1 then the dma controller for the corresponding channel must be disabled before changing the FTHRES value
#define CR_FSEL           (1<<7 )//flash memory selection..this bit selects the flash memory to be addressed in single flash mode(when dfm=0)..modified only when busy=0..ignored when dfm=1
#define CR_DFM            (1<<6 )//dual flash mode..this bit activates the dual flash modde..where two external flsh memories are used simulataneously to double the throughput and capacity..modified only when BUSY=0
#define CR_SSHIFT         (1<<4 )//sample shift..By default, the QUADSPI samples data 1/2 of a CLK cycle after the data is driven by the Flash memory. This bit allows the data is to be sampled later in order to account for external signal delays. Firmware must assure that SSHIFT = 0 when in DDR mode (when DDRM = 1). This field can be modified only when BUSY = 0.
#define CR_TCEN           (1<<3 )//timeout counter enable..This bit is valid only when memory-mapped mode (FMODE = 11) is selected. Activating this bit causes the chip select (nCS) to be released (and thus reduces consumption) if there has not been an access after a certain amount of time, where this time is defined by TIMEOUT[15:0] (QUADSPI_LPTR). Enable the timeout counter. By default, the QUADSPI never stops its prefetch operation, keeping the previous read operation active with nCS maintained low, even if no access to the Flash memory occurs for a long time. Since Flash memories tend to consume more when nCS is held low, the application might want to activate the timeout counter (TCEN = 1, QUADSPI_CR[3]) so that nCS is released after a period of TIMEOUT[15:0] (QUADSPI_LPTR) cycles have elapsed without an access since when the FIFO becomes full with prefetch data. This bit can be modified only when BUSY = 0.
#define CR_DMAEN          (1<<2 )//dma enable..In indirect mode, DMA can be used to input or output data via the QUADSPI_DR register. DMA transfers are initiated when the FIFO threshold flag, FTF, is set.
#define CR_ABORT          (1<<1 )//abort request..This bit aborts the on-going command sequence. It is automatically reset once the abort is complete. This bit stops the current transfer. In polling mode or memory-mapped mode, this bit also resets the APM bit or the DM bit.
#define CR_EN             (1<<0 )//enable..enable the QUADSPI

//Bit vectors for DCR 
#define DCR_FSIZE(x)       (x<<16)//flash memory size. This field defines the size of external memory using the following formula. Number of bytes in Flash memory = 2[FSIZE+1] FSIZE+1 is effectively the number of address bits required to address the Flash memory. The Flash memory capacity can be up to 4GB (addressed using 32 bits) in indirect mode, but the addressable space in memory-mapped mode is limited to 256MB. If DFM = 1, FSIZE indicates the total capacity of the two Flash memories together. This field can be modified only when BUSY = 0.
#define DCR_MODE_BYTE(x)   (x<<21)//not used in the code
#define DCR_CSHT(x)        (x<<8 )//chip select high time.. CSHT+1 defines the minimum number of CLK cycles which the chip select (nCS) must remain high between commands issued to the Flash memory. This field can be modified only when BUSY = 0.
#define DCR_CKMODE         0x1 	  //clock mode..indicates the level that clk takes between command

//Bit vectors for status register
#define SR_FLEVEL(x)      (x<<8)//FIFO level. This field gives the number of valid bytes which are being held in the FIFO. FLEVEL = 0 when the FIFO is empty, and 16 when it is full. In memory-mapped mode and in automatic status polling mode, FLEVEL is zero.
#define SR_TOF            (1<<4)//Timeout flag. This bit is 1 when timeout occurs. It is cleared by writing 1 to CTOF
#define SR_SMF            (1<<3)//Status match flag. This bit is set in automatic polling mode when the unmasked received data matches the corresponding bits in the match register (QUADSPI_PSMAR). It is cleared by writing 1 to CSMF.
#define SR_FTF            (1<<2)//FIFO threshold flag. In indirect mode, this bit is set when the FIFO threshold has been reached, or if there is any data left in the FIFO after reads from the Flash memory are complete. It is cleared automatically as soon as threshold condition is no longer true. In automatic polling mode this bit is set every time the status register is read, and the bit is cleared when the data register is read.
#define SR_TCF            (1<<1)//Transfer complete flag. This bit is set in indirect mode when the programmed number of data has been transferred or in any mode when the transfer has been aborted. It is cleared by writing 1 to CTCF
#define SR_TEF            (1<<0)//Transfer error flag. This bit is set in indirect mode when an invalid address is being accessed in indirect mode. It is cleared by writing 1 to CTEF

//Bit vectors for flag clear register 
#define FCR_CTOF (1<<4)//Clear timeout flag. Writing 1 clears the TOF flag in the QUADSPI_SR register
#define FCR_CSMF (1<<3)//Clear status match flag. Writing 1 clears the SMF flag in the QUADSPI_SR register
#define FCR_CTCF (1<<1)//Clear transfer complete flag. Writing 1 clears the TCF flag in the QUADSPI_SR register
#define FCR_CTEF (1<<0)//Clear transfer error flag. Writing 1 clears the TEF flag in the QUADSPI_SR register

//Bit vectors for DLR
#define DL(x)  x  //Useless -- but for better readability of the code

//Bit vectors for CCR
#define CCR_DDRM                   (1<<31) //Double data rate mode. This bit sets the DDR mode for the address, alternate byte and data phase. This field can be written only when BUSY = 0.
#define CCR_DHHC                   (1<<30) //DDR hold. Delay the data output by 1/4 of the QUADSPI output clock cycle in DDR mode. This feature is only active in DDR mode. This field can be written only when BUSY = 0.
#define CCR_DUMMY_BIT              (1<<29) // Needed by Micron Flash Memories
#define CCR_SIOO                   (1<<28) //Send instruction only once mode. This bit has no effect when IMODE = 00. This field can be written only when BUSY = 0.
#define CCR_FMODE(x)               (x<<26) //Functional mode. This field defines the QUADSPI functional mode of operation. If DMAEN = 1 already, then the DMA controller for the corresponding channel must be disabled before changing the FMODE value. This field can be written only when BUSY = 0.
#define CCR_DMODE(x)               (x<<24) //Data mode. This field defines the data phases mode of operation. This field also determines the dummy phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_DUMMY_CONFIRMATION     (1<<23) // Needed by Micron Flash Memories
#define CCR_DCYC(x)                (x<<18) //Number of dummy cycles. This field defines the duration of the dummy phase. In both SDR and DDR modes, it specifies a number of CLK cycles (0-31). This field can be written only when BUSY = 0.
#define CCR_ABSIZE(x)              (x<<16) //Alternate bytes size. This bit defines alternate bytes size. This field can be written only when BUSY = 0.
#define CCR_ABMODE(x)              (x<<14) //Alternate bytes mode. This field defines the alternate-bytes phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_ADSIZE(x)              (x<<12) //Address size. This bit defines address size. This field can be written only when BUSY = 0.
#define CCR_ADMODE(x)              (x<<10) //Address mode. This field defines the address phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_IMODE(x)               (x<<8 ) //Instruction mode. This field defines the instruction phase mode of operation. This field can be written only when BUSY = 0.
#define CCR_INSTRUCTION(x)         (x<<0 ) //Instruction. Instruction to be sent to the external SPI device. This field can be written only when BUSY = 0.

#define CCR_FMODE_INDWR 0x0 //Indirect write mode
#define CCR_FMODE_INDRD 0x1 //Indirect read mode
#define CCR_FMODE_APOLL 0x2 //Automatic polling mode
#define CCR_FMODE_MMAPD 0x3 //Memory mapped mode
// The above values of CCR_FMODE can only be changed when BUSY=0 and DMAEN=0
#define NDATA  0x0 //for convenience
#define SINGLE 0x1 // "		"
#define DOUBLE 0x2 // "		"
#define QUAD   0x3 // "		"

#define BYTE      0x0 //for convenience
#define TWOBYTE   0x1 // "	"
#define THREEBYTE 0x2 // "	"
#define FOURBYTE  0x3 // "	"

int* cr       =      (const int*) CR;	//Control
int* dcr      =      (const int*) DCR;	//Device configuration
int* sr       =      (const int*) SR;	//status
int* fcr      =      (const int*) FCR;	//flag clear
int* ccr      =      (const int*) CCR;	//comminication configuration
int* ar       =      (const int*) AR;	//address
int* abr      =      (const int*) ABR;	//alternate bytes
int* dr       =      (const int*) DR;	//data
int* dlr      =      (const int*) DLR;	//data length
int* psmkr    =      (const int*) PSMKR;//polling status mask
int* pir      =      (const int*) PIR;	//polling interval
int* lprt     =      (const int*) LPRT;	//low power timeout
int* startmm  =      (const int*) STARTMM;
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

void waitfor(unsigned int secs) {
	unsigned int time = 0;
	while(time++ < secs);
}

/**
 * @brief Initializes various parameters needed by the board.
 *
 * This function only works when the board was successfully detected
 * by Soletta and a corresponding
 * pin multiplexer module was found.
 *
 * @see set_aspi_shakti32().
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
 * Thi function is used to disable all the flags which were enabled when different modes were set
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
 * 
 * @param *addr Address of the memory location 
 * @param val Value to be stored in the location
 *
 * @return Void.
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

int micron_write_enable(int status){
    printf("\tWrite Enable\n");
    set_qspi_shakti32(ccr,(CCR_ADSIZE(FOURBYTE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x06)));
    int ret = wait_for_tcf(status); //Indicating the completion of command -- Currently polling
    printf("Status : %d dcr : %d cr: %d ccr: %d \n",status,*dcr,*cr,*ccr);
    return ret;
}

int micron_enable_4byte_addressing(int status){
    printf("\t Enable 4 byte address \n");
    set_qspi_shakti32(ccr,(CCR_ADSIZE(FOURBYTE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xB7)));
    int ret = wait_for_tcf(status); //Indicating the completion of command -- Currently polling
    printf("Status : %d dcr : %d cr: %d ccr: %d \n",status,*dcr,*cr,*ccr);
    return ret;
}

int micron_configure_xip_volatile(int status, int value){
    printf("\tWrite Volatile Configuration Register\n");
    set_qspi_shakti32(dlr,DL(1));
    set_qspi_shakti8(dr,value);  //The value to be written into the VECR register to enable XIP. Indicating XIP to operate in Quad Mode
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDWR)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x81)));
    waitfor(50);
    int ret = wait_for_tcf(status);
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d dr: %d\n",status,*dcr,*cr,*ccr,*dlr,*dr);
    waitfor(50);  //Giving Micron time to store the data
    return ret;
}

int micron_disable_xip_volatile(int status, int value){
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

int micron_read_id_cmd(int status, int value){
    printf("\tRead ID Command to see if the Protocol is Proper\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x9E)|CCR_DMODE(SINGLE)));
    int ret = wait_for_tcf(status);
    value = get_qspi_shakti(dr);
    printf("Read ID: %08x",value);
    return ret;
}

int micron_read_configuration_register(int status, int value){
    printf("\tRead ID Command to see if the Protocol is Proper\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xBE)|CCR_DMODE(SINGLE)));
    int ret = wait_for_tcf(status);
    value = get_qspi_shakti(dr);
    printf("Configuration Register Value: %08x",value);
    return ret;
}

#endif
