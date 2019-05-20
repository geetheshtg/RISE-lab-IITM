/**
 * @file qspi_micron.c
 * This file checks the working of QUADSPI using XIP
 * The ID of the N25Q256 memory device used is 0x20BA1910
 */
#include "qspi.h"

// Definitions of macros taken from qspi.h, written here for cross referencing
// #define READ_ID 0x9E		/**Read ID of memory device*/
// #define READ_SR 0x05		/**Read status register*/
// #define WR_EN 0x06		/**Write enable*/
// #define FOURBYTE_AD 0xB7	/**Enter 4 byte addressing mode*/
// #define WR_VCR 0x81		/**Write Volatile configuration register*/
// #define FAST_RD 0x0B		/**Fast read*/
// #define QDFAST_RD 0xEB	/**Quad I/O fast read*/

/**
 * @brief Function to ensure the working of memory device.
 *
 * This function sets the memory device to indirect read mode and it passes the instruction 0x9E which gives the ID of memory device. 
 * This function also ensures the device to complete execution of a command.
 * The ID received from the memory device will then be checked with the original ID of the device.
 * It will return 0 if device is detected and will proceed further execution, and will return 0 otherwise.
 * 
 * @see set_qspi_shakti32(), wait_for_tcf(), reset_interrupt_flags().
 * 
 * @return 0 on success, -1 otherwise.
 */
int flashIdentificationDevice(){
	printf("\tReading the ID register and discovering the Flash Device\n");
	set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(READ_ID)|CCR_DMODE(SINGLE)));
    //!Variable is passed as a parameter in wait_for_tcf() function. It carries no value, but it gets assigned with the address of status register.
    int status = 0; 
    int ret = wait_for_tcf(status);
    int value = get_qspi_shakti(dr);
    reset_interrupt_flags();
    if(value == MEM_TYPE_N25Q256_ID){
    	printf("\tN25Q256 Device Detected\n");
    	return 0;
    }
    else{
    	printf("\t Device Not Detected - Diagnose %08x",value);
    	return -1;
    }
}

/**
 * @brief To detect the presence of memory device.
 *
 * This function calls flashIdentificationDevice() and again returns the same value in main(). This function is redundant as it returns the function value without any changes.
 * 
 * @see flashIdentificationDevice().
 *
 * @return 0 if Device detected, -1 otherwise.
 */
int flashMemInit(){  
	int ret = flashIdentificationDevice();
	if(ret==-1){
		printf("Flash Mem Init Failed -- Quitting Program, Diagnose");
		return ret;
	}
    return 0;
}

/**
 * @brief To read the status bits of the flash.
 *
 * This function reads the status bits of the flash from the status register
 * It uses the instruction 0x05 to read the status register.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32()
 * @return @a value on success, -1 otherwise.
 */
int flashReadStatusRegister(){
    printf("\tReading the Status bits of the Flash\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(READ_SR)|CCR_DMODE(SINGLE)));
    int status = 0;
    int ret = wait_for_tcf(status);
    waitfor(100);
    int value = get_qspi_shakti(dr);
    reset_interrupt_flags();
    if(ret){
        printf("\tRead Status Register Failed\n");
        return -1;
        }
    else 
    	return value;
}


/**Function to enable single write mode*/
/**Instruction passed - 0x06 - Write enable*/
/**
 * @brief To enable single write mode.
 *
 * This function passes the instruction 0x06 to the memory device which will Write enable it.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32()
 * @return an integer value returned by wait_for_tcf().
 */
int flashWriteEnable(){
    printf("\tWrite Enable\n");
    set_qspi_shakti32(ccr,(CCR_IMODE(SINGLE)|CCR_INSTRUCTION(WR_EN)));
    int ret = wait_for_tcf(0); //Indicating the completion of command -- Currently polling
    reset_interrupt_flags();
    return ret; 
}

/**Function to enable 4 byte addressing mode in memory device*/
/**Instruction passed - 0xB7 - Enter 4 byte addressing mode*/
/**
 * @brief To read the status bits of the flash.
 *
 * This function enables 4 byte addressing mode in the memory device
 * It uses the instruction 0xB7 to enter 4 byte addressing mode.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32()
 * @return -1 on failure.
 */
int flashEnable4ByteAddressingMode(){  //Enable 4-byte addressing Mode and read the status to verify that it has happened correctly
    if(flashWriteEnable()){
        printf("\t Write Enable Failed \n");
        return -1;
    }
    waitfor(100);
    set_qspi_shakti32(ccr,(CCR_IMODE(SINGLE)|CCR_INSTRUCTION(FOURBYTE_AD)));
    int status =0; 
    int ret = wait_for_tcf(status);
    reset_interrupt_flags();
    waitfor(100);
    //Checking Phase
    status = flashReadStatusRegister();
    printf("\t Status Value: %08x\n",status);
    if(status & 1)
        printf("\t 4-Byte Addressing Mode is Enabled\n");
    else
        printf("\t 4-byte Addressing mode not Enabled\n");
}

/**Function called in main(), necessary instruction has to be passed in order to work properly*/
/**Function call from main() - flashReadSingleSPI(7,ar_read,0x0B,4,THREEBYTE);*/
/**Instruction passed - 0x0B - Fast read*/
/**Will enable the flash memory in Single SPI fast read mode*/
/**
 * @brief To read data in single SPI mode.
 *
 * This function passes various parameters that are needed to read data in single SPI mode
 * The instruction 0x0B is to be passed for entering Single SPI fast read mode.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32()
 *
 * @param dummy_cycles Number of dummy cycles.
 * @param read_address Address from which data is to be read.
 * @param instruction Instruction to be passed to the memory device.
 * @param data_words Length of the data to be passed into DLR.
 * @param adsize Size of the address.
 *
 * @return @a value on success,-1 on failure.
 */
int flashReadSingleSPI(int dummy_cycles, int read_address, int instruction, int data_words, int adsize){
    set_qspi_shakti32(dlr,data_words);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(SINGLE)|CCR_DCYC(dummy_cycles)|CCR_ADMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_ADSIZE(adsize)|CCR_INSTRUCTION(instruction)));
    set_qspi_shakti32(ar,read_address);
    int status = 0;
    int ret = wait_for_tcf(status);
    int value = get_qspi_shakti(dr);
    printf("\tThe Read Value: %08x\n",value);
    if(ret){
        printf("\t Read Value Failed \n");
        return -1;
    }
    reset_interrupt_flags();
    return value;
}


/**
 * @brief To read data in Quad SPI mode.
 *
 * This function passes various parameters that are needed to read data in quad SPI mode
 * The instruction 0xEB is to be passed for entering Ouad I/O fast read mode.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32()
 *
 * @param[in] dummy_cycles Number of dummy cycles.
 * @param[in] read_address Address from which data is to be read.
 * @param[in] instruction Instruction to be passed to the memory device.
 * @param[in] data_words Length of the data to be passed into DLR.
 * @param[in] adsize Size of the address.
 *
 * @return @a value on success,-1 on failure.
 */
int flashReadQuadSPI(int dummy_cycles, int read_address, int instruction, int data_words, int adsize){
     set_qspi_shakti32(dlr,data_words);
     set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DCYC(dummy_cycles)|CCR_ADSIZE(adsize)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(instruction)));
     set_qspi_shakti32(ar,read_address);
     int status = 0;
     int ret = wait_for_tcf(status);
     int value = get_qspi_shakti(dr);
     printf("\tThe Read Value: %08x\n",value);
     if(ret){
         printf("\t Read Value Failed \n");
         return -1; 
     }   
     reset_interrupt_flags();
     return value;
 }

/**
 * @brief To write into volatile configuration register and enable XIP.
 *
 * This function passes the value 0x93, into the data register which enables Quad I/O mode with 8 dummy cycles in SDR mode
 * The instruction 0x81 is to be passed to write volatile configuration register.
 * 
 * @see wait_for_tcf(), set_qspi_shakti32(), set_qspi_shakti8().
 *
 * @return @a value on success,-1 on failure.
 */
int flashWriteVolatileConfigReg(int value){
    printf("\t Setting Volatile Configuration Register with the Value: %08x",value);
    set_qspi_shakti32(dlr,DL(1));
    set_qspi_shakti8(dr,value);  //The value to be written into the VECR register to enable XIP. Indicating XIP to operate in Quad Mode
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDWR)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(WR_VCR)));
    waitfor(50);
    int status=0;
    int ret = wait_for_tcf(status);
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d dr: %d\n",status,*dcr,*cr,*ccr,*dlr,*dr);
    reset_interrupt_flags();
    waitfor(50);  //Giving Micron time to store the data
    return ret;
}
 
/**
 * @brief Program to test the working of QUADSPI
 * 
 * This program focuses mainly on the working of QUADSPI by making some initial conditions and then executes 2 scenarios
 *
 * In the first step, we set the flash memory size, chip select high time, prescaler and FIFO threshold to the registers.
 * All the interrupts are enabled. After this, it is checked if the flash memory is working properly.
 *
 * Scenarios:
 *
 * 1. Fast read command (Single SPI Read) working with 7 Dummy cycles : 
 * 
 * This scenario is just to test the working of fast read in SPI mode. This is done using Three byte address size and it works with 7 dummy cycles. The instruction passed to enable fast read mode is 0x0B.
 * 
 * 2. Quad fast read I/O (Quad SPI Read) working with 9 Dummy cycles
 *
 * Similar to the previous scenario, here the device is enabled in Quad I/O fast read mode with 3 bytes address size and 9 dummy cycles. The instruction passed to enable Quad I/O fast read mode is 0xEB.
 *
 * After these two scenarios, now the device is set in QUADSPI mode. The next step is to enable XIP and see if the QUADSPI mode works properly by writing and verifying a data from it.
 * To enable XIP, the volatile configuration register has to be written with value 0x93 and to write the VCR, the instruction 0x0x81 is passed.
 * Once the VCR is written with this value, a dummy confirmation bit is created to ensure the enabling of XIP. This bit can be used to enable or disable XIP.
 * Now we check if the data can be properly read from the data register. The data read is put into a config_string and verified.
 * 
 * Then, XIP is enabled in memory mapped mode without passing any instruction. Here, a loop is performed where the read data from the data register is put into the config_string bit by bit and finally this data is also printed and verified.
 */
int main()
{
	
    qspi_init(27,0,3,1,15);
    uart_init();		
    int ar_read,i,j;
    waitfor(100); //Time for Micron to start, maybe?
    if(flashMemInit()) //Because of STARTUPE2 primitive, the run fails for the first time it is programmed since three clock cycles are skipped. Run again
        return -1;  //Didn't work

    //Scenarios
    // Fast Read Command (Single SPI Read) is working with 7 Dummy Cycles
    // Quad Fast Read I/O (Quad SPI Read) is working with 9 Dummy Cycles
    
    waitfor(100);
    flashEnable4ByteAddressingMode();

    //Scenario-1

    printf("\t SINGLE SPI read with Three Byte with 7 Dummy Cycles\n");

    ar_read=0;
    for(i=0;i<512;++i){
        flashReadSingleSPI(7,ar_read,FAST_RD,4,THREEBYTE);
        waitfor(100);
        ar_read+=4;
    }

    printf("\n\n\n");

    //Scenario-2
	
    printf("\t Quad SPI read, Three Byte Address with 9 Dummy Cycles 0XEB as the instruction\n");

    ar_read=0;
    for(i=0;i<512;++i){
        flashReadQuadSPI(9,ar_read,QDFAST_RD,4,THREEBYTE);
        waitfor(100);
        ar_read+=4;
    }

    printf("\n\n\n");

    //XIP Mode
    int xip_value = XIP_VAL;
    if(flashWriteVolatileConfigReg(xip_value)){
        printf("\t Volatile Configuration Register not Set -- Diagnose\n");
        return -1;
    }
    reset_interrupt_flags();
    printf("\t Quad I/O Mode with Dummy Confirmation bit to enable XIP\n");
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(9)|CCR_ADSIZE(THREEBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xEB)));
    set_qspi_shakti32(dlr,0x4);
    set_qspi_shakti32(ar,0x000000); //Address where the Config_string is situated in the micron nand flash memory
    int status=0;
    wait_for_tcf(status);
    waitfor(100); 
    
    int *config_string = (const int*)0x40000000;
    int* read_data = (int*)(0x90000004); //Read data register after the first element is read through QUAD mode
    *config_string = get_qspi_shakti(dr);
    printf("\tRead data is : %08x\n",*config_string);
    config_string++;
    reset_interrupt_flags(); 
    printf("\t Trying XIP now\n");
    set_qspi_shakti32(ccr, (CCR_FMODE(CCR_FMODE_MMAPD)|CCR_DMODE(QUAD)|CCR_DCYC(8)|CCR_ADSIZE(THREEBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA)));
    waitfor(25);
    int dum_data;

    for(i=0;i<67;++i) {
         dum_data = get_qspi_shakti(read_data);
         waitfor(100);
         *config_string = dum_data;
         config_string++;
         read_data++;
         waitfor(100);
         reset_interrupt_flags();
         waitfor(10);
    }
    config_string = (const int*)(0x40000000);
    for(i=0;i<67;++i){
        printf("Data: %08x Address: %08x \n",*config_string, config_string);
        config_string++;
    }
//    uart_finish();
	return 0;
}
