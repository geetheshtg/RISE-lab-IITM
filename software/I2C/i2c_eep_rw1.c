#include "i2c.h"
#include <stdint.h>
#include "write_data.h"

uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
{
  printf("\t Trap Taken: cause: %08x epc: %08x \n",cause,epc);
}

int main(){
    int timeout;

    uart_init();

    printf("\tI2C: Starting Transaction\n");
    //char writebuf[writebytes];
	char writebuf1[2] = {0,0};
    char writebuf[18], writeData = 0x20;
    writebuf[0] = 0;
    writebuf[1] = 0;
	
    char readbuf[32];
    int i = 0, j = 0,  k = 0, status=0;	
	int slaveaddr = 160;
	unsigned char temp = 0;    
    char* source_address = (char*) eeprom_data;
    printf("\t Start of Write Sequence into EEPROM -- Objective: Load the EEPROM with 16KB of Data\n");
    if(shakti_init_i2c())	
    { 
        printf("\tSomething Wrong In Initialization\n"); 
        return 0; 
    }
	else 
        printf("\tIntilization Happened Fine\n");

    //Implementing Read and Write through Polling for now 
    while(wait_for_bb())
    {
     printf("\tError in Waiting for BB\n");
     return 0;
    }
    //printf("\tSetting Slave Address\n");
    //set_i2c_shakti(i2c_data,slaveaddr);
    //printf("\tSlave Address set\n");
    //temp = get_i2c_shakti(i2c_data); //Will this work?
    //printf("\tSet slave address read again, which is 0x%x\n",temp);
    //if(slaveaddr != (int)temp)
    //    printf("\tSomewhere something is wrong with the controller - it is giving out some random address -- Diagnose\n");

//    for(int i = 0; i < 2; ++i){
        for(j = 0; j < 16; ++j){
            printf("\t Value being copied is : %0x\n", writeData); //Prints a lot, still ok!
            writebuf[k++] = writeData++;
        }
        //index = j;
        k = 0;

        printf("\tSetting Slave Address : %d\n", i);
        set_i2c_shakti(i2c_data,slaveaddr);
        printf("\tSlave Address set\n");
        temp = get_i2c_shakti(i2c_data); //Will this work?
        printf("\tSet slave address read again, which is 0x%x\n",temp);
        if(slaveaddr != (int)temp)
            printf("\tSomewhere something is wrong with the controller - it is giving out some random address -- Diagnose\n");
        
        //source_address = source_address + index; //source_address++ will take care of it
        
        i2c_start(); //Asking the controller to send a start signal to initiate the transaction
//	shakti_sendbytes(writebuf1, 2, 0,0); 
        timeout = wait_for_pin(&status);
        if (timeout) {
            printf("\tTimeout happened - Write did not go through the BFM -- Diagnose\n");
            i2c_stop(); //~
            return EREMOTEIO;
           }

        if (status & I2C_SHAKTI_LRB) { // What error is this?
                   i2c_stop();//~
	            printf("\tSome status check failing\n");
		}
/*	
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
*/
        set_i2c_shakti(i2c_data, 0x00);
        timeout = wait_for_pin(&status);
        if (timeout) {
            printf("\tTimeout happened - Write did not go through the BFM -- Diagnose\n");
            i2c_stop(); //~
            return EREMOTEIO;
           }

        if (status & I2C_SHAKTI_LRB) { // What error is this?
                   i2c_stop();//~
	            printf("\tSome status check failing\n");
                    return EREMOTEIO;
                }

        set_i2c_shakti(i2c_data, 0x00);
//        set_i2c_shakti(i2c_data,writebuf[1]);
        timeout = wait_for_pin(&status);
        if (timeout) {
            printf("\tTimeout happened - Write did not go through the BFM -- Diagnose\n");
            i2c_stop(); //~
            return EREMOTEIO;
           }

        if (status & I2C_SHAKTI_LRB) { // What error is this?
                   i2c_stop();//~
	            printf("\tSome status check failing\n");
		}
	
        if(shakti_sendbytes(writebuf, 16,1,0) != 16){
            printf("\tSomething wrong in sending bytes to write -- Diagnose\n");
            return 0;
        }
	    waitfor(9000);  //Necessary evil since you need to give time to the EEPROM to store the data -- It's operating at 100KHz lol :P Actually should be 900000
        writebuf[0]++;
        
        while(wait_for_bb()) //Adding this for completeness -- Not useful until multi masters come into play
        { 
            printf("\twaiting for bb-2\n");
        }  
//    }
    //Write Complete!! Need to Verify if the written contents are right!!!
    set_i2c_shakti(i2c_data,slaveaddr);
    i2c_start();
/*	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
*/
    printf("\tSlave Addr 2 set - This is to increment the memory counter in the EEPROM\n");
	while(wait_for_pin(&status)) 
    { 
        printf("\twaiting for pin\n");
    } 
	if(shakti_sendbytes(writebuf1, 2, 0,0)!=2) 
    { 
        printf("\tSomething wrong in sending bytes to write -- Diagnose\n"); 
        return 0;
    }
	while(wait_for_pin(&status)) 
    { 
        printf("\twaiting for pin-2\n");
    } 
	set_i2c_shakti(i2c_data,slaveaddr + 1); //After repeated start
    
	while(wait_for_pin(&status)) 
    { 
        printf("\twaiting for pin-3\n");
    } 
    
    if(shakti_readbytes( readbuf, 16, 1)!= 16) 
    { 
        printf("\tSomething wrong in reading bytes\n -- Diagnose"); 
        return 0;
    }

   


    return 0;
}
