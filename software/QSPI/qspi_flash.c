#include "qspi.h"

//Sample Code which loads the config string from flash and puts it on Main Memory and the code begins to execute from there -- Emulating a stripped down scenario of booting from flash by picking the config_string and putting it into main memory
//This code can be run provided the QSPI is coupled with Micron NOR flash model (part number N25Q128A)
//Init parameters -- fsize 2GB Memory space, Chip select high time 0 (might be needed in MM mode), CKMODE 1, Prescaler = 3 (Bus clock/3), All interrupts enabled, FIFO threshold 16, QSPI enabled


int main()
{
    int* config_string = (int*)(0x80000000);  //location of config_string where the fetched config_string data will be present
    int* read_data = (int*)(0x90100004); //Read data register after the first element is read through QUAD mode
    int i = 0;
    waitfor(100); //Time for Micron Flash to get ready
    int status=0;
    int read_id=0;
    qspi_init(27,0,3,1,15);
    //Read ID Command

    if(micron_write_enable(status)){
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();
    if(micron_write_enable(status)){
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();
    micron_read_id_cmd(status,read_id);
    reset_interrupt_flags();
    
    if(micron_write_enable(status)){
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();
    
    if(micron_enable_4byte_addressing(status)){
        printf("Panic: Enable 4 byte addressing command failed to execute");
        return -1;
    }
    reset_interrupt_flags();        
    
    
    set_qspi_shakti32(dlr,0x4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(SINGLE)|CCR_DCYC(7)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x0B)));
    set_qspi_shakti32(ar,0x100000); //Address where the Config_string is situated in the micron nand flash memory waitfor(1000);
//    printf("Status : %08x dcr : %08x cr : %08x ccr : %08x dlr: %08x ar: %08x",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status);
    
    //int read_data = get_qspi_shakti(dr);
    *config_string = get_qspi_shakti(dr);
    printf("\tRead data before start of XIP is : %08x\n",*config_string);
    reset_interrupt_flags();
 
    if(micron_write_enable(status)){
        printf("Panic: Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();



    int xip_value = 0x93; //Quad I/O Mode with 8 Dummy Cycles. SDR Mode
    if(micron_configure_xip_volatile(status,xip_value)){
        printf("Panic: XIP Volatile Reg not written, Command Failed");
        return -1;
    }

    reset_interrupt_flags();



    printf("\t Quad I/O Mode with Dummy Confirmation bit to enable XIP\n");
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xEC)));
    set_qspi_shakti32(dlr,0x4);
    set_qspi_shakti32(ar,0x100000); //Address where the Config_string is situated in the micron nand flash memory
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d ar: %d",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status);
    //int read_data = get_qspi_shakti(dr);
    *config_string = get_qspi_shakti(dr);
    printf("\tRead data is : %x\n",*config_string);
    config_string++; //Next location in config string -- 0x1004
    reset_interrupt_flags();
    printf("\t Trying XIP now\n");
    set_qspi_shakti32(ccr, (CCR_FMODE(CCR_FMODE_MMAPD)|CCR_DMODE(QUAD)|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA)));
    waitfor(25);
    int dum_data;
    for(i=0;i<67;++i) {
         dum_data = get_qspi_shakti(read_data);
         waitfor(10);
         *config_string = dum_data;
         config_string++;
         read_data++;
         reset_interrupt_flags();
         waitfor(10);
    }
    config_string = (const int*)(0x80000000);
    //Should exit XIP mode and go to Idle state
    xip_value = 1;  //XIP value to terminate from Micron
   /* if(micron_disable_xip_volatile(status,xip_value)){
        printf("Panic: XIP failed to terminate, Command Failed, Diagnose");
        return -1;
    }*/
    printf("\t Printing Read Data \n");
    for(i=0;i<67;++i){
        printf("Data: %x Address: %x \n",*config_string, config_string);
        config_string++;
    }
    printf("\t Read Data Successfully \n");
    return 0;
}
