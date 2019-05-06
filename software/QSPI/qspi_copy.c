#include "qspi.h"
#define CYPRESS_MEM_ID 0x0102204D
char fail_bit = 0;
int status = 0;
int ar_read=0;
int read_data = 0;
int cypressflashIdentification(){
    if(micron_write_enable(status)){
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();
    printf("\t Read ID \n");
//	printf("\tQSPI: Reading the ID register and discovering the Flash Device\n");
    set_qspi_shakti32(dlr,4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x9F)|CCR_DMODE(SINGLE)));
    int ret = wait_for_tcf(status);
    int value = get_qspi_shakti(dr);

    if(value==CYPRESS_MEM_ID)
        printf("\tCypress: S25FS512S Detected \n");
    else{
        printf("\t Value: %08x",value);
        return -1;
    }

    printf("\tQSPI: Actual Read ID: %08x\n",value);
    reset_interrupt_flags();
    return ret;
}

int setVCR(int crvalue, int srvalue){
    if(micron_write_enable(status)){
        printf("Panic: Write Enable Command Failed to execute");
        return -1;
    }
    reset_interrupt_flags();
    set_qspi_shakti32(dlr,DL(2));
    set_qspi_shakti8(dr,crvalue);  
    set_qspi_shakti8(dr,srvalue); 
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDWR)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0x01)));
    waitfor(50);
    wait_for_tcf(status);
    printf("Status : %08x dcr : %08x cr : %08x ccr : %08x dlr: %08x dr: %08x\n",status,*dcr,*cr,*ccr,*dlr,*dr);
    waitfor(50);  //Giving Micron time to store the data
    reset_interrupt_flags();
    return wait_for_wip(); // Function which checks if WIP is done, indicating completion of Page Program

}

int wait_for_wip(){
    int status1;
    do{
        printf("\t Waiting for Wip \n");
        status1 = cypressReadStatusRegister(0x05);
        if(check_fail_bit())
            return -1;
        if(status1 & 0x40){
            printf("\tQSPI: Programming Error - Diagnose\n");
            return -1;
        }
        waitfor(1000);
    }while(status1 & 0x01);
    return 0;
}

int cypressEnterXIP(int address, int data_length){
    set_qspi_shakti32(dcr,DCR_MODE_BYTE(0xA0)|DCR_FSIZE(27)|DCR_CSHT(0)|DCR_CKMODE);
    set_qspi_shakti32(dlr,data_length); //DLR
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(9)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(0xEC)));
    set_qspi_shakti32(ar,address); //Address where the Config_string is situated in the micron nand flash memory 
    waitfor(1000);
    wait_for_tcf(status);
    printf("Status : %08x dcr : %08x cr : %08x ccr : %08x dlr: %08x ar: %08x\n",status,*dcr,*cr,*ccr,*dlr,*ar);
    
    read_data = get_qspi_shakti(dr);
    //*config_string = get_qspi_shakti(dr);
    printf("\t Read Data is %08x\n",read_data);
    reset_interrupt_flags();
    set_qspi_shakti32(ccr, (CCR_FMODE(CCR_FMODE_MMAPD)|CCR_DMODE(QUAD)|CCR_DUMMY_CONFIRMATION|CCR_DCYC(9)|CCR_ADSIZE(FOURBYTE)|CCR_ADMODE(QUAD)|CCR_IMODE(NDATA))); //Entering MMAPD
    return 0;
}

int cypressExitXIP(){
    set_qspi_shakti32(cr, CR_ABORT);
    printf("\t ABORT GENERATED \n");
    waitfor(100);
    qspi_init(27,0,3,1,15);
    return 0;
}

int cypressReadStatusRegister(int statusReg){
    //printf("\tQSPI: Reading the Status Register with instruction :%08x\n",statusReg);
    set_qspi_shakti32(dlr,0x4);
    set_qspi_shakti32(ccr,(CCR_FMODE(CCR_FMODE_INDRD)|CCR_DMODE(SINGLE)|CCR_IMODE(SINGLE)|CCR_INSTRUCTION(statusReg)));
    waitfor(50);
    int ret_value = wait_for_tcf(status);
    if(ret_value){
        printf("\tQSPI: Read Status Register Failed \n");
        fail_bit = 1;
    }
    reset_interrupt_flags();
    return get_qspi_shakti(dr); 
}

int check_fail_bit(){
  if(fail_bit){
        fail_bit = 0;
        return -1;
  }
    else{
        fail_bit = 0;
        return 0;
    }
}


void jumpToTCM(){
char* tcm_addr = (char*) 0x20000;
printf("\t Fencing Cache and jumping to TCM \n");
asm volatile("fence");
asm volatile( "li x30, 0x20000" "\n\t"
              "jr x30" "\n\t"
              :
              :
              :"x30","cc","memory"
            );
}

int main(){
    uart_init();
    qspi_init(27,0,15,1,15); //qspi0 initialization
    int* xip_address = (int*) 0x90000000;
    int* tcm_address = (int*) 0x20000;
    int i = 0;

    if(cypressflashIdentification()){
        printf("\tQSPI: Device Discovery failed, Diagnose \n");
    }
    //Reading the Written Data
    setVCR(0x02,0x02);  //To Enable Quad Bit

    waitfor(1000); //Arbit Delay -- So that the Quad Bit is actually set -- Empirically this delay is required to guarantee no data-loss
    ar_read = 0;
       
    printf("\t Entering XIP Mode \n");
    cypressEnterXIP(0x0,0x4);

    for(i = 0; i < 32768; ++i){
        read_data = *(xip_address);
        *tcm_address = read_data;
        xip_address++;
        tcm_address++;
        waitfor(100);
    }
 
    cypressExitXIP();
    printf("\t Exiting XIP mode and Jumping to TCM \n");
    jumpToTCM();

}
