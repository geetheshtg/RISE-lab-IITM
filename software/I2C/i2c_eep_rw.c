#include "i2c.h"
#include <stdint.h>
#include "write_data.h"
/*
   uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32])
   {
   printf("\t Trap Taken: cause: %08x epc: %08x \n",cause,epc);
   }
 */
/*
 * Doing an initialization sequence as how PCF8584 was supposed to be initialized
 * The Initialization Sequence is as follows
 * Reset Minimum 30 Clock Cycles -- Not necessary in our case 
 * Load Byte 80H into Control
 * Load Clock Register S2 */ /* We are doing the opposite -- Setting the clock and then the registers -- Doesn't really matter actually
 * Send C1H to S1 - Set I2C to Idle mode -- SDA and SCL should be high
			      */
int ConfigureI2cController(int prescalerClock, int sclkFrequency)
{
	unsigned char temp = 0;
	printf("\n\tI2C: Initializing the Controller");

	//Set Prescaler Clock
	set_i2c_shakti(i2c_prescale, prescalerClock);  //Setting the I2C clock value to be 1, which will set the clock for module and prescaler clock

	//Verify the value
	temp = get_i2c_shakti(i2c_prescale);
	if(temp != prescalerClock)
	{
		printf("\n\t Error in setting prescaler clock; Wr. Value: 0x%02x; Read Value: 0x%02x", prescalerClock, temp);

	}

	//Set I2C Serial clock frequency
	set_i2c_shakti(i2c_scl, sclkFrequency);  
	//Setting the I2C clock value to be 1, which will set the clock for module and prescaler clock


	//Read the value
	temp = get_i2c_shakti(i2c_scl);
	/* Just reading the written value to see if all is well -- Compiler should not optimize this load!!! Compiler can just optimize the store to pointer address followed by load pointer to a register to just an immediate load to the register since clock register is not used anywhere -- but the purpose is lost. Don't give compiler optimizations */

	if(temp != sclkFrequency){
		printf("\n\tClock initialization failed Write Value: 0x%02x; read Value: 0x%02x", sclkFrequency, temp); 
		return -ENXIO;
	}
	else{
		printf("\tClock successfully initalized\n");
	}



	/* S1=0x80 S0 selected, serial interface off */
	printf("\tSetting Control Register with 0x80 \n");
	set_i2c_shakti(i2c_control, I2C_SHAKTI_PIN);

	printf("\tControl Register Successfully set \n");

	// Reading set control Register Value to ensure sanctity
	printf("\tReading Control Register \n");
	temp = get_i2c_shakti(i2c_control);
	printf("\tControl Register is Written with 0x%x \n", temp);

	if((temp & 0x7f) != 0){
		printf("\tDevice Not Recognized\n");
		return -ENXIO;
	}

	printf("\tWaiting for a specified time\n ");
	waitfor(900); //1 Second software wait -- Should be 900000 but setting to 900 now since simulation is already slow
	printf("\tDone Waiting \n ");

	i2c_stop();

	/* Enable Serial Interface */
	printf("\n Making the I2C chip in idle State");
	set_i2c_shakti(i2c_control, I2C_SHAKTI_IDLE);

	printf("\n\tWaiting for a specified time After setting idle\n ");
	waitfor(900); //1 Second software wait -- Should be 900000 but setting to 900 now since simulation is already slow
	printf("\tDone Waiting \n ");

	temp = get_i2c_shakti(i2c_status); 
	printf("\tStatus Reg value is : 0x%x \n",temp);

	/* Check to see if I2C is really in Idle and see if we can access the status register -- If not something wrong in initialization. This also verifies if Control is properly written since zero bit will be initialized to zero*/
	if(temp != (I2C_SHAKTI_PIN | I2C_SHAKTI_BB)){
		printf("\tInitialization failed\n");
		return -ENXIO;
	}
	else 
		printf("\tAll is well till here \n");

	printf("\tI2C successfully initialized\n");
}


/*Test function*/
int ConfigureI2cController1(int prescalerClock, int sclkFrequency)
{
	unsigned char temp = 0;
	printf("\n\tI2C: Initializing the Controller");

	//Set Prescaler Clock
	set_i2c_shakti(i2c_led, prescalerClock);  //Setting the I2C clock value to be 1, which will set the clock for module and prescaler clock

	//Verify the value
	temp = get_i2c_shakti(i2c_led);
	if(temp != prescalerClock)
	{
		printf("\n\t Error in setting prescaler clock; Wr. Value: 0x%02x; Read Value: 0x%02x", prescalerClock, temp);

	}
	//Set I2C Serial clock frequency
	set_i2c_shakti(i2c_led, sclkFrequency);
	//Setting the I2C clock value to be 1, which will set the clock for module and prescaler clock


	//Read the value
	temp = get_i2c_shakti(i2c_led);
	/* Just reading the written value to see if all is well -- Compiler should not optimize this load!!! Compiler can just optimize the store to pointer address followed by load pointer to a register to just an immediate load to the register since clock register is not used anywhere -- but the purpose is lost. Don't give compiler optimizations */

	if(temp != sclkFrequency){
		printf("\n\tClock initialization failed Write Value: 0x%02x; read Value: 0x%02x", sclkFrequency, temp);
		return -ENXIO;
	}
	else{
		printf("\tClock successfully initalized\n");
	}
}
/*Test function*/


int WriteSlaveAddress(unsigned char slaveAddress)
{
	int timeout;
	unsigned char temp = 0;
	int status = 0;
	/* Writes the slave address to I2C controller */
	printf("\n\tSetting Slave Address : 0x%02x\n", slaveAddress);
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

	Uart0Init();

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
	//    char* source_address = (char*) eeprom_data;
	printf("\t Start of Write Sequence into EEPROM -- Objective: Load the EEPROM with 16KB of Data\n");



	/*    set_i2c_shakti(i2c_led,0x1F);  //Setting the I2C clock value to be 1, which will set the clock for module and prescaler clock
	      printf("\nRead value is :%d",  get_i2c_shakti(i2c_led) ) ;
	 */
	if( ConfigureI2cController(0x0F, 0x51))	
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
	for(j = 0; j < 16; ++j)
	{
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

	waitfor(9000);  //Necessary evil since you need to give time to the EEPROM to store the data -- It's operating at 100KHz lol :P Actually should be 900000

	while(wait_for_bb()) //Adding this for completeness -- Not useful until multi masters come into play
	{ 
		printf("\twaiting for bb-2\n");
	}  

	printf("\tSlave Addr 2 set - This is to increment the memory counter in the EEPROM\n");

	WriteSlaveAddress(I2C_SLAVE_ADDRESS);


	while(wait_for_pin(&status)) 
	{ 
		printf("\twaiting for pin\n");
	} 



//Read part

	SetEepromReadOrWriteStartAddress(0x0000);

	WriteSlaveAddress(I2C_SLAVE_ADDRESS + 1);

	if(I2CEepromReadData( readbuf, 16, 1) != 16) 
	{ 
		printf("\n\tSomething wrong in reading bytes\n -- Diagnose"); 
		return 0;
	}
	printf("\n I2C ACCESS CHECK is done.");    

	printf("\n I2C ACCESS CHECK - 11600");

	if(ConfigureI2cController1(0x0F, 0x51))
	{
		printf("\tSomething Wrong In Initialization\n");
		return 0;
	}
	else
		printf("\tIntilization Happened Fine\n");

	return 0;
}
