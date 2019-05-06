#include "i2c.h"
#include <stdint.h>
#include "write_data.h"

uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
{
  printf("\t Trap Taken: cause: %08x epc: %08x \n",cause,epc);
}
int WriteSlaveAddress(unsigned char slaveAddress)
{
    int timeout;
	unsigned char temp = 0;
	int status = 0;
/* Writes the slave address to I2C controller*/
        printf("\tSetting Slave Address : 0x%02x\n", slaveAddress);
        set_i2c_shakti(i2c_data,slaveAddress);
        printf("\tSlave Address set\n");
//Reads the slave address from I2C controller
        temp = get_i2c_shakti(i2c_data); //Will this work?
        printf("\tSet slave address read again, which is 0x%x\n",temp);
        if(slaveAddress != (int)temp)
            printf("\tSlave address is not matching; Written Add. Value: 0x%02x; Read Add. Value: 0x%02x\n", slaveAddress, temp);
        
//Sending the slave address to the I2C slave        
        i2c_start(); //Asking the controller to send a start signal to initiate the transaction
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
}


int SetEepromReadOrWriteStartAddress(unsigned int startAddress)
{
    int timeout;
	unsigned char temp = 0;
	int status = 0;
	set_i2c_shakti(i2c_data, (startAddress >> 8) & 0xFF);
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

	set_i2c_shakti(i2c_data, (startAddress >> 0) & 0xFF);
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
}

int WaitForReadOrWriteComplete(int *status)
{
	int timeout = DEF_TIMEOUT;

	*status = get_i2c_shakti(i2c_status);

	while ((*status & I2C_SHAKTI_PIN) && --timeout) {
		waitfor(10000); /* wait for 100 us */
		*status = get_i2c_shakti(i2c_status);
	}
	if (timeout == 0){
        printf("\tWait for pin timed out\n");
		return ETIMEDOUT;
    }

	return 0;


}


int I2CEepromWriteData( const char *buf, int count, int last, int eni)
{
	int wrcount, status, timeout;
	int i = 0;
    printf("\tStarting Write Transaction -- Did you create tri1 nets for SDA and SCL in verilog?\n");
	for (i = 0; i < count; ++i) 
	{
		printf("\n\t Writing the value 0x%02x into EEPROM", buf[i]);
		set_i2c_shakti(i2c_data, buf[i]);
		if( ETIMEDOUT == WaitForReadOrWriteComplete(&status))
		{
			printf("\n I2C Write Timed out");
			i2c_stop(); //~
			return EREMOTEIO; 
		}
		if (status & I2C_SHAKTI_LRB) 
		{ 
			i2c_stop();//~
            printf("\tSome status check failing\n");
			return EREMOTEIO; 
		}
	}

	if (last){
        printf("\n\tLast byte sent : Issue a stop\n");
		i2c_stop();
    }
	else
	{
        printf("\n\tSending Rep Start and doing some other R/W transaction\n");
		if(!eni)
            i2c_repstart();
        else
            i2c_repstart_eni();
    }

	return i;
}

int I2CEepromReadData(char *buf, int count, int last)
{
	int i, status;
	int wfp;
    int read_value = 0;

	/* increment number of bytes to read by one -- read dummy byte */
	for (i = -1; i < count; i++) 
	{
		if( ETIMEDOUT == WaitForReadOrWriteComplete(&status))
		{
			printf("\n I2C Read Timed out");
			i2c_stop(); //~
			return EREMOTEIO; 
		}
		if (status & I2C_SHAKTI_LRB) 
		{ 
			i2c_stop();//~
            printf("\tSome status check failing\n");
			return EREMOTEIO; 
		}

//Do Dummy Read initially	    
        if (-1 != i)
		{		
			buf[i] = get_i2c_shakti(i2c_data);
			printf("\n\t Read Address Offset: %d; Value: %x", i, buf[i]);
		}

//Start reading the data
		else
		{
			printf("\n\t Dummy Read Value: 0x%02x", get_i2c_shakti(i2c_data) ); /* dummy read */
		}

		if (i == count - 1) 
		{
			set_i2c_shakti(i2c_control, I2C_SHAKTI_ESO);
		} 
		else if (i == count) 
		{
			if (last)
			{
				printf("\n\t Read is complete; Sending Stop signal");
				i2c_stop();
			}
			else
			{
				printf("\n\t Sending I2C Repeat Start");
				i2c_repstart();
			}
		}

	}
	printf("\n\t Read %d Bytes from EEPROM", i);
	return i; //excluding the dummy read
}




int main(){
    int timeout;

    uart_init();

    printf("\tI2C: Starting Transaction\n");
    //char writebuf[writebytes];
	char writebuf1[2] = {0,0};
    char writebuf[18], writeData = 0x30;
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
        for(j = 0; j < 16; ++j){
//            printf("\t Value being copied is : %0x\n", writeData); //Prints a lot, still ok!
            writebuf[k++] = writeData++;
        }
        k = 0;

		WriteSlaveAddress(I2C_SLAVE_ADDRESS);
		SetEepromReadOrWriteStartAddress(0x0000);
		if( 16 != I2CEepromWriteData(writebuf, 16,1,0) )
		{
			printf("\n Error occured when writing into EEPROM");
			return 0;
		}	
/*
        if(shakti_sendbytes(writebuf, 16,1,0) != 16){
            printf("\tSomething wrong in sending bytes to write -- Diagnose\n");
            return 0;
        }
*/
	    waitfor(9000);  //Necessary evil since you need to give time to the EEPROM to store the data -- It's operating at 100KHz lol :P Actually should be 900000
//        writebuf[0]++;
        
        while(wait_for_bb()) //Adding this for completeness -- Not useful until multi masters come into play
        { 
            printf("\twaiting for bb-2\n");
        }  
//    }
    //Write Complete!! Need to Verify if the written contents are right!!!
/*    set_i2c_shakti(i2c_data,I2C_SLAVE_ADDRESS);
    i2c_start();
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
	    waitfor(9000);  
*/
    printf("\tSlave Addr 2 set - This is to increment the memory counter in the EEPROM\n");

		WriteSlaveAddress(I2C_SLAVE_ADDRESS);


	while(wait_for_pin(&status)) 
    { 
        printf("\twaiting for pin\n");
    } 

/*
	if(shakti_sendbytes(writebuf1, 2, 0,0)!=2) 
    { 
        printf("\tSomething wrong in sending bytes to write -- Diagnose\n"); 
        return 0;
    }
*/

	while(wait_for_pin(&status)) 
    { 
        printf("\twaiting for pin-2\n");
    } 
	SetEepromReadOrWriteStartAddress(0x0000);

	WriteSlaveAddress(I2C_SLAVE_ADDRESS + 1);

    
    
    if(I2CEepromReadData( readbuf, 16, 1) != 16) 
    { 
        printf("\n\tSomething wrong in reading bytes\n -- Diagnose"); 
        return 0;
    }

   


    return 0;
}
