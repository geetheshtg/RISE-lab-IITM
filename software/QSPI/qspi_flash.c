#include "qspi.h"

//Sample Code which loads the config string from flash and puts it on Main Memory and the code begins to execute from there -- Emulating a stripped down scenario of booting from flash by picking the config_string and putting it into main memory
//This code can be run provided the QSPI is coupled with Micron NOR flash model (part number N25Q128A)
//Init parameters -- fsize 2GB Memory space, Chip select high time 0 (might be needed in MM mode), CKMODE 1, Prescaler = 3 (Bus clock/3), All interrupts enabled, FIFO threshold 16, QSPI enabled


int main()
{
    int* config_string = (int*)(0x80000000);                  //location of config_string where the fetched config_string data will be present
    int* read_data = (int*)(0x90100004);                      //Read data register after the first element is read through QUAD mode
    int i = 0;
    waitfor(100);                                             //Time for Micron Flash to get ready
    int status=0;
    int read_id=0;
    qspi_init(27,0,3,1,15);                                   //Read ID Command

//  void qspi_init(int fsize, int csht, int prescaler, int enable_interrupts, int fthreshold){
//     int int_vector = enable_interrupts? (CR_TOIE|CR_SMIE|CR_FTIE|CR_TCIE|CR_TEIE) : 0; 
//     set_qspi_shakti32(dcr,(DCR_FSIZE(fsize)|DCR_CSHT(csht)|DCR_CKMODE)); 
//     set_qspi_shakti32(cr,(CR_PRESCALER(prescaler)|int_vector|CR_FTHRES(fthreshold)|CR_EN));
//  }
//  this function gets the flash memory size(fsize), chip select high time(csht), prescaler and fifo threshold level(fthreshold) values as parameters 
//  It sets the fsize, csht and CKMODE in DCR(Device configuration register) and it sets prescaler, fthreshold and it also enables the QSPI in CR(Control Register)
//  qspi_init()'s 4th parameter is enable_interrupts, since it is given as 1, which means interrupts are to be enabled.
//  This enables timeout, status match, fifo threshold, transfer complete and transfer error interrupts in the control register

    if(micron_write_enable(status)){     //line 166 of qspi.h for
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
//  int micron_write_enable(int status){
//     printf("\tWrite Enable\n");
//     set_qspi_shakti32(ccr,(CCR_ADSIZE(FOURBYTE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x06)));
//     int ret = wait_for_tcf(status); //Indicating the completion of command -- Currently polling
//     printf("Status : %d dcr : %d cr: %d ccr: %d \n",status,*dcr,*cr,*ccr);
//     return ret;
//  }   

//   int wait_for_tcf(int status){
//     int timeout = DEF_TIMEOUT;               //DEF_TIMEOUT is 60
//     status = get_qspi_shakti(sr);            //get the status from the address of status register..initially the status is 0
//     while(!( status & 0x02 ) && --timeout){  //
//         waitfor(1000);
//         status=get_qspi_shakti(sr);
//     }
//     if(timeout==0){
//         printf("Request Timed out");
//         return -1;
//     }
//     return 0;
//   }  
//     
//     
    reset_interrupt_flags(); //line 144 of qspi.h for resetting all the flags
    if(micron_write_enable(status)){     //line 166 of qspi.h for
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags(); // line 144
    micron_read_id_cmd(status,read_id); //line 208 of qspi.h to read ID command to see if protocol is proper
    reset_interrupt_flags(); //line 144
    
    if(micron_write_enable(status)){     //line 166
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();  //line 144
    
    if(micron_enable_4byte_addressing(status)){  //line 174 to enable 4 byte address
        printf("Panic: Enable 4 byte addressing command failed to execute");
        return -1;
    }
    reset_interrupt_flags();     //line 144
    
    
    set_qspi_shakti32(dlr,0x4);   //line 113 of qspi.h for setting 0x04 to dlr i.e setting address
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(SINGLE)|CCR_DCYC(7)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x0B)));  //line 113 for setting ccr
    set_qspi_shakti32(ar,0x100000); // line 113 for setting ar...i.e Address where the Config_string is situated in the micron nand flash memory waitfor(1000);
//    printf("Status : %08x dcr : %08x cr : %08x ccr : %08x dlr: %08x ar: %08x",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status);   //line 148 for indicating the completion of command
    
    //int read_data = get_qspi_shakti(dr);
    *config_string = get_qspi_shakti(dr);  //line 128 for getting the address of the config string
    printf("\tRead data before start of XIP is : %08x\n",*config_string);
    reset_interrupt_flags();      //line 144
 
    if(micron_write_enable(status)){     //line 166
        printf("Panic: Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();     //line 144



    int xip_value = 0x93; //Quad I/O Mode with 8 Dummy Cycles. SDR Mode
    if(micron_configure_xip_volatile(status,xip_value)){    //line 182 to write the volatile configuration register
        printf("Panic: XIP Volatile Reg not written, Command Failed");
        return -1;
    }

    reset_interrupt_flags();     //line 144



    printf("\t Quad I/O Mode with Dummy Confirmation bit to enable XIP\n");
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xEC)));   //line 113 set ccr
    set_qspi_shakti32(dlr,0x4);   //line 113 to set ox4 to dlr
    set_qspi_shakti32(ar,0x100000); //line 113 to set ar...i.e the Address where the Config_string is situated in the micron nand flash memory
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d ar: %d",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status);      //line 148 for checking the completion of command
    //int read_data = get_qspi_shakti(dr);
    *config_string = get_qspi_shakti(dr);  // line 128 getting the address of the config string 
    printf("\tRead data is : %x\n",*config_string);
    config_string++; //Next location in config string -- 0x1004
    reset_interrupt_flags();
    printf("\t Trying XIP now\n");
    set_qspi_shakti32(ccr, (CCR_FMODE(CCR_FMODE_MMAPD)|CCR_DMODE(QUAD)|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA)));   //line 113 for setting ccr
    waitfor(25);  //wait for micron to store data
    int dum_data;
    for(i=0;i<67;++i) {
         dum_data = get_qspi_shakti(read_data);   //taking the read_data as dummy data
         waitfor(10);
         *config_string = dum_data;             //setting the dum_data in config string's address
         config_string++;                    //go to the next address
         read_data++;                        //read_data address also to the next position
         reset_interrupt_flags();
         waitfor(10);                        //wait for micron to store data
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
