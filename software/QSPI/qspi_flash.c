#include "qspi.h"

//Sample Code which loads the config string from flash and puts it on Main Memory and the code begins to execute from there -- Emulating a stripped down scenario of booting from flash by picking the config_string and putting it into main memory
//This code can be run provided the QSPI is coupled with Micron NOR flash model (part number N25Q128A)
//Init parameters -- fsize 2GB Memory space, Chip select high time 0 (might be needed in MM mode), CKMODE 1, Prescaler = 3 (Bus clock/3), All interrupts enabled, FIFO threshold 16, QSPI enabled


int main()
{
    int* config_string = (int*)(0x80000000);                  //location of config_string where the fetched config_string data will be present
    int* read_data = (int*)(0x90100004);                      //Read data register after the first element is read through QUAD mode
    int i = 0;
    waitfor(100);                                            
    int status=0;
    int read_id=0;
    qspi_init(27,0,3,1,15);                                  

//  void qspi_init(int fsize, int csht, int prescaler, int enable_interrupts, int fthreshold){
//     int int_vector = enable_interrupts? (CR_TOIE|CR_SMIE|CR_FTIE|CR_TCIE|CR_TEIE) : 0; 
//     set_qspi_shakti32(dcr,(DCR_FSIZE(fsize)|DCR_CSHT(csht)|DCR_CKMODE)); 
//     set_qspi_shakti32(cr,(CR_PRESCALER(prescaler)|int_vector|CR_FTHRES(fthreshold)|CR_EN));
//  }
//  this function gets the flash memory size(fsize), chip select high time(csht), prescaler and fifo threshold level(fthreshold) values as parameters 
//  It sets the fsize, csht and CKMODE in DCR(Device configuration register) and it sets prescaler, fthreshold and it also enables the QSPI in CR(Control Register)
//  qspi_init()'s 4th parameter is enable_interrupts, since it is given as 1, which means interrupts are to be enabled.
//  This enables timeout, status match, fifo threshold, transfer complete and transfer error interrupts in the control register

    if(micron_write_enable(status)){    
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }   
    reset_interrupt_flags();
//  The next step of this program is to check the working condition of each of the features in the device..
//  In the above segment of code, the flash memory's working condition is checked by enabling it
//  If the function micron_write_enable(status) executes without timeout with status = 0, that means the flash memory is in working condition
//  In the function micron_write_enable(status), the address size is set as 4 bytes, the instruction mode is set as single and an instruction is passed using the communication configuration register  
//  After setting the ccr, this function will wait for the command to complete its execution using the function wait_for_tcf(status) 
//  In the function wait_for_tcf(status), while waiting for the command completion, the parameter 'status' is getting updated by referring to the status register
//  After this, the function will return a true or false value depending on the timeout condition, will return true if timeout occurs, that means the flash memory cannot be used
//  Depending on this returned value, the program will identify if the flash memory is executable or not
//  After enabling the flash memory, the interrupt flags might have been set, that may cause interruptions to the upcoming commands, therefore the interrupt flags have to be cleared

    
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

    if(micron_write_enable(status)){     
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags(); 
//  This part of the code also does similar operation as that of the previous one, it tries to enable the flash memory and if enabled successfully, then reset all the flags
//  But in this case, the status is not zero and it has some value which was assigned from the previous part of the code on waiting for completion of command
//  Now, once we enable the flash memory, again there could've been some interrupts set, we need to clear them for the upcoming commands to execute properly 
 
    micron_read_id_cmd(status,read_id); 
    reset_interrupt_flags();
//  This part of the code is written to check if data can be read from the flash memory 
//  For this, the function micron_read_id_cmmd is used which will set the flash to indirect read mode and the value stored in the data register is noted.
//  This code also used the function wait_for_tcf(status) to indicate the completion of command
//  After this code, the interrupts might have been set, they need to be cleared for the upcoming commands to execute properly.
   
    if(micron_write_enable(status)){     
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();  
//  Enabling the flash memory again and clearing the flags for further execution  
    
    if(micron_enable_4byte_addressing(status)){  
        printf("Panic: Enable 4 byte addressing command failed to execute");
        return -1;
    }
    reset_interrupt_flags();   
//  This part of code checks the working of 4 byte addressing
//  It will set the address size as 4 bytes and gets a single information for testing
//  This also uses wait_for_tcf(status) to indicate the completion of command
//  Finally, the flags are reset for further execution of commands
    
    set_qspi_shakti32(dlr,0x4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(SINGLE)|CCR_DCYC(7)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x0B)));  //line 113 for setting ccr
    set_qspi_shakti32(ar,0x100000); 
    waitfor(1000);
//    printf("Status : %08x dcr : %08x cr : %08x ccr : %08x dlr: %08x ar: %08x",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status); 
    *config_string = get_qspi_shakti(dr);  
    printf("\tRead data before start of XIP is : %08x\n",*config_string);
    reset_interrupt_flags();      
//  This part of the code is to check if the flash memory can be read in indirect mode when clock is enabled
//  In all the previous parts of the code CCR_DCYC was not given, which means the duration of the dummy phase was 0 and the number of clock cycles was taken as 0
//  Here, the number of clock cycles is given as 7 and hence the flash memory is tested in indirect read mode    
//  The data from the data register is printed for verification and the flags are reset for further execution of the code
 
    if(micron_write_enable(status)){     
        printf("Panic: Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();     
//  the flash memory is enabled once again for XIP execution


    int xip_value = 0x93; //Quad I/O Mode with 8 Dummy Cycles. SDR Mode
    if(micron_configure_xip_volatile(status,xip_value)){   
        printf("Panic: XIP Volatile Reg not written, Command Failed");
        return -1;
    }
    reset_interrupt_flags();   
//  For checking if the flash can operate in XIP mode
//  the data register is feeded with value 0x93, which is the value to be written into the VECR register to enable XIP mode  
//  the CCR is set in indirect wrrite mode
//  after checking the flash memory for XIP operation the interrupt flags are reset    

    
    
//  int micron_configure_xip_volatile(int status, int value){
//      printf("\tWrite Volatile Configuration Register\n");
//      set_qspi_shakti32(dlr,DL(1));
//      set_qspi_shakti8(dr,value);  //The value to be written into the VECR register to enable XIP. Indicating XIP to operate in Quad Mode
//      set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDWR)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x81)));
//      waitfor(50);
//      int ret = wait_for_tcf(status);
//      printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d dr: %d\n",status,*dcr,*cr,*ccr,*dlr,*dr);
//      waitfor(50);  //Giving Micron time to store the data
//      return ret;
//  }


    printf("\t Quad I/O Mode with Dummy Confirmation bit to enable XIP\n");
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xEC)));  
    set_qspi_shakti32(dlr,0x4);   
    set_qspi_shakti32(ar,0x100000); 
    printf("Status : %d dcr : %d cr : %d ccr : %d dlr: %d ar: %d",status,*dcr,*cr,*ccr,*dlr,*ar);
    wait_for_tcf(status);  
    *config_string = get_qspi_shakti(dr);  
    printf("\tRead data is : %x\n",*config_string);
    config_string++; //Next location in config string -- 0x1004
    reset_interrupt_flags();
//  This part of code is to check if dummy confirmation bit can be used to enable XIP
//  Dummy confirmation is set to 0 - XIP enabled
//  the config string is read from the data register and it is verified  
//  config_string++ will move the pointer to its next position thereby reading the whole string
//  finally reset the interrupt flags for further execution  
    
    
    printf("\t Trying XIP now\n");
    set_qspi_shakti32(ccr, (CCR_FMODE(CCR_FMODE_MMAPD)|CCR_DMODE(QUAD)|CCR_DCYC(8)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA)));  
    waitfor(25);  //wait for micron to store data
    int dum_data;
    for(i=0;i<67;++i) {
         dum_data = get_qspi_shakti(read_data);   //taking the read_data as dummy data
         waitfor(10);
         *config_string = dum_data;             //setting the dum_data in config string's address
         config_string++;                    //config_string pointer moves to the next position
         read_data++;                        //read_data pointer moves to the next position
         reset_interrupt_flags();
         waitfor(10);                        //wait for micron to store data
    }
//  In this part of the code, the device is set to memory mapped mode with QUADSPI enabled
//  Here, no instruction is passed as the CCR_IMODE is set to 0
    
    
    
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
