SHELL := /bin/bash # Use bash syntax
XLEN=64

UART1_DIR:=./UART1/
TEST_DIR:=./TEST/
I2C_DIR:=./I2C/
DMA_DIR:=./DMA/
TEST_OUT:=./TEST/outputs/
UART1_OUT:=./UART1/outputs/
I2C_OUT:=./I2C/outputs/
I2C_DMA_OUT:=./I2C/dma_outputs/
DMA_OUT:=./DMA/outputs/
CommonDIR:=./Common/
QSPI_DIR:=./QSPI/
QSPI_OUT:=./QSPI/outputs
PLIC_DIR:=./PLIC/
PLIC_OUT:=./PLIC/outputs
BOOT_DIR:=./boot/
BOOT_OUT:=./boot/outputs
HF_DIR:=./HYFLASH/
HF_OUT:=./HYFLASH/outputs/

#ASCII Escape codes
RED='\033[0;31m' #Red
GREEN='\033[0;32m' #Green
NC='\033[0m' # No Color

all: create_dir	
	make test.riscv testuart1.riscv uart1mp3.riscv i2c.riscv qspi.riscv dma_mem2mem i2c_dma.riscv hfread.riscv hyperbusi.riscv i2c_eeprom_write.riscv i2c_eep_rw.riscv uart1bcm.riscv uart0bcm.riscv

create_dir:
	@mkdir -p ./TEST/outputs/
	@mkdir -p ./UART1/outputs/
	@mkdir -p ./I2C/outputs/
	@mkdir -p ./QSPI/outputs/
	@mkdir -p ./DMA/outputs/
	@mkdir -p ./I2C/dma_outputs/
	@mkdir -p ./PLIC/outputs/
	@mkdir -p ./UART/outputs/
	@mkdir -p ./HYFLASH/outputs/

test.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling TEST code $(NC)"
	@echo -e "$(RED) Caveat: Test Code starts at 0xC0000000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(TEST_DIR)/test.c -o $(TEST_OUT)/test.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(TEST_OUT)/test.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(TEST_OUT)/test.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(TEST_OUT)/test.shakti > $(TEST_OUT)/test.dump


testuart1.riscv: crt.o syscalls1.shakti create_dir
	@echo -e "$(GREEN) Compiling TEST code $(NC)"
	@echo -e "$(RED) Caveat: Test Code starts at 0xC0000000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(TEST_DIR)/testuart1.c -o $(TEST_OUT)/testuart1.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(TEST_OUT)/testuart1.o $(CommonDIR)/syscalls1.shakti $(CommonDIR)/crt.o -o $(TEST_OUT)/testuart1.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(TEST_OUT)/testuart1.shakti > $(TEST_OUT)/testuart1.dump



uart1mp3.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling UART 1 MP3 code $(NC)"
	@echo -e "$(RED) Caveat: UART 1 MP3 Code starts at 0xC0000000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf -g -c $(UART1_DIR)/uart1mp3.c -o $(UART1_OUT)/uart1mp3.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(UART1_OUT)/uart1mp3.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(UART1_OUT)/uart1mp3.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(UART1_OUT)/uart1mp3.shakti > $(UART1_OUT)/uart1mp3.dump
#	@elf2hex 8 32768 $(UART1_OUT)/uart1mp3.shakti 2147483648 > $(UART1_OUT)/code.mem
#	@cut -c1-8 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.MSB
#	@cut -c9-16 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.LSB


uart0bcm.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling UART 0 bcm code $(NC)"
	@echo -e "$(RED) Caveat: UART 0 bcm Code starts at 0xC0000000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf -g -c $(UART1_DIR)/uart0bcm.c -o $(UART1_OUT)/uart0bcm.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(UART1_OUT)/uart0bcm.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(UART1_OUT)/uart0bcm.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(UART1_OUT)/uart0bcm.shakti > $(UART1_OUT)/uart0bcm.dump
#	@elf2hex 8 32768 $(UART1_OUT)/uart1mp3.shakti 2147483648 > $(UART1_OUT)/code.mem
#	@cut -c1-8 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.MSB
#	@cut -c9-16 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.LSB

uart1bcm.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling UART 1 bcm code $(NC)"
	@echo -e "$(RED) Caveat: UART 1 bcm Code starts at 0xC0000000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf -g -c $(UART1_DIR)/uart1bcm.c -o $(UART1_OUT)/uart1bcm.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(UART1_OUT)/uart1bcm.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(UART1_OUT)/uart1bcm.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(UART1_OUT)/uart1bcm.shakti > $(UART1_OUT)/uart1bcm.dump
#	@elf2hex 8 32768 $(UART1_OUT)/uart1mp3.shakti 2147483648 > $(UART1_OUT)/code.mem
#	@cut -c1-8 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.MSB
#	@cut -c9-16 $(UART1_OUT)/code.mem > $(UART1_OUT)/code.mem.LSB

i2c.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling I2C Polling code $(NC)"
	@echo -e "$(RED) Caveat: I2C Code starts at 0x100000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(I2C_DIR)/i2c.c -o $(I2C_OUT)/i2c.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(I2C_DIR)/i2clink.ld $(I2C_OUT)/i2c.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(I2C_OUT)/I2C.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(I2C_OUT)/I2C.shakti > $(I2C_OUT)/I2C.dump
	@elf2hex 8 32768 $(I2C_OUT)/I2C.shakti 16777216 > $(I2C_OUT)/code.mem
	@cut -c1-8 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.MSB
	@cut -c9-16 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.LSB


i2c_eep_rw.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling I2C Polling code $(NC)"
	@echo -e "$(RED) Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(I2C_DIR)/i2c_eep_rw.c -o $(I2C_OUT)/i2c_eep_rw.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(I2C_OUT)/i2c_eep_rw.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(I2C_OUT)/i2c_eep_rw.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(I2C_OUT)/i2c_eep_rw.shakti > $(I2C_OUT)/i2c_eep_rw.dump
	@elf2hex 8 32768 $(I2C_OUT)/i2c_eep_rw.shakti 2147483648 > $(I2C_OUT)/code.mem
	@cut -c1-8 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.MSB
	@cut -c9-16 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.LSB


i2c_eeprom_write.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling I2C Polling code $(NC)"
	@echo -e "$(RED) Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(I2C_DIR)/i2c_eeprom_write.c -o $(I2C_OUT)/i2c_eeprom_write.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(I2C_OUT)/i2c_eeprom_write.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(I2C_OUT)/i2c_eeprom_write.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(I2C_OUT)/i2c_eeprom_write.shakti > $(I2C_OUT)/i2c_eeprom_write.dump
	@elf2hex 8 32768 $(I2C_OUT)/i2c_eeprom_write.shakti 2147483648 > $(I2C_OUT)/code.mem
	@cut -c1-8 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.MSB
	@cut -c9-16 $(I2C_OUT)/code.mem > $(I2C_OUT)/code.mem.LSB

boot.riscv: create_dir 	
	@echo -e "$(GREEN) Compiling BOOT Polling code $(NC)"
	@echo -e "$(RED) Caveat: BOOT Code starts at 0x2000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(BOOT_DIR)/boot.c -o $(BOOT_OUT)/boot.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf -D__ASSEMBLY__=1 -c $(BOOT_DIR)/crt.S -o $(BOOT_DIR)/crt.o
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf  -c $(BOOT_DIR)/syscalls.c -o $(BOOT_DIR)/syscalls.shakti
	@riscv$(XLEN)-unknown-elf-gcc -T $(BOOT_DIR)/bootlink.ld  $(BOOT_OUT)/boot.o $(BOOT_DIR)/syscalls.shakti $(BOOT_DIR)/crt.o -o $(BOOT_OUT)/boot.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(BOOT_OUT)/boot.shakti > $(BOOT_OUT)/boot.dump
	@elf2hex 8 8192 $(BOOT_OUT)/boot.shakti 8192 > $(BOOT_OUT)/code.mem
	#@cat $(BOOT_DIR)/dts.hex $(BOOT_OUT)/boot.mem > $(BOOT_OUT)/code.mem


qspi.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling QSPI Pollingcode $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(QSPI_DIR)/qspi_flash.c -o $(QSPI_OUT)/qspi.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(QSPI_OUT)/qspi.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(QSPI_OUT)/qspi.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(QSPI_OUT)/qspi.shakti > $(QSPI_OUT)/qspi.dump
	@elf2hex 8 65536 $(QSPI_OUT)/qspi.shakti 2147483648 > $(QSPI_OUT)/code.mem
	@cut -c1-8 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.MSB
	@cut -c9-16 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.LSB

qspi_copy.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling QSPI Pollingcode $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(QSPI_DIR)/qspi_copy.c -o $(QSPI_OUT)/qspi.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf -D__ASSEMBLY__=1 -c $(BOOT_DIR)/crt.S -o $(BOOT_DIR)/crt.o
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf  -c $(BOOT_DIR)/syscalls.c -o $(BOOT_DIR)/syscalls.shakti
	@riscv$(XLEN)-unknown-elf-gcc -T $(BOOT_DIR)/bootlink.ld $(QSPI_OUT)/qspi.o $(BOOT_DIR)/syscalls.shakti $(BOOT_DIR)/crt.o -o $(QSPI_OUT)/qspi.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(QSPI_OUT)/qspi.shakti > $(QSPI_OUT)/qspi.dump
	@elf2hex 8 8192 $(QSPI_OUT)/qspi.shakti 4096 > $(QSPI_OUT)/code.mem
	@cut -c1-8 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.MSB
	@cut -c9-16 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.LSB

qspi_dma.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling QSPI Pollingcode $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(QSPI_DIR)/qspi_dma.c -o $(QSPI_OUT)/qspi_dma.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(QSPI_OUT)/qspi_dma.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(QSPI_OUT)/qspi_dma.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(QSPI_OUT)/qspi_dma.shakti > $(QSPI_OUT)/qspi_dma.dump
	@elf2hex 8 65536 $(QSPI_OUT)/qspi_dma.shakti 2147483648 > $(QSPI_OUT)/code.mem
	@cut -c1-8 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.MSB
	@cut -c9-16 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.LSB

qspi_cypress.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling QSPI Pollingcode $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(QSPI_DIR)/qspi_cypress.c -o $(QSPI_OUT)/qspi.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/qspilink.ld $(QSPI_OUT)/qspi.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(QSPI_OUT)/qspi_cypress_write.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(QSPI_OUT)/qspi_cypress_write.shakti > $(QSPI_OUT)/qspi_cypress.dump
	#@elf2hex 8 32768 $(QSPI_OUT)/qspi_cypress.shakti 2147483648 > $(QSPI_OUT)/code.mem
	#@cut -c1-8 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.MSB
	#@cut -c9-16 $(QSPI_OUT)/code.mem > $(QSPI_OUT)/code.mem.LSB

plic.riscv: crt.o syscalls.shakti create_dir
	@echo -r "$(GREEN) Compiling PLIC Code $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf -I./$(CommonDIR)/ -c $(PLIC_DIR)/global_interrupts.c -o $(PLIC_OUT)/global_interrupts.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(PLIC_OUT)/global_interrupts.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(PLIC_OUT)/global_interrupts.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(PLIC_OUT)/global_interrupts.shakti > $(PLIC_OUT)/global_interrupts.dump
	@elf2hex 8 32768 $(PLIC_OUT)/global_interrupts.shakti 2147483648 > $(PLIC_OUT)/code.mem
	@cut -c1-8 $(PLIC_OUT)/code.mem > $(PLIC_OUT)/code.mem.MSB
	@cut -c9-16 $(PLIC_OUT)/code.mem >$(PLIC_OUT)/code.mem.LSB

dma_mem2mem: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling DMA Memory to Memory transfer code $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(DMA_DIR)/mem_to_mem_transferDma.c -o $(DMA_OUT)/mem_to_mem_transfer.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(DMA_OUT)/mem_to_mem_transfer.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(DMA_OUT)/mem_to_mem_transfer.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(DMA_OUT)/mem_to_mem_transfer.shakti > $(DMA_OUT)/mem_to_mem_transfer.dump
#	@elf2hex 8 65536 $(DMA_OUT)/mem_to_mem_transfer.shakti 2147483648 > $(DMA_OUT)/code.mem
#	@cut -c1-8 $(DMA_OUT)/code.mem > $(DMA_OUT)/code.mem.MSB
#	@cut -c9-16 $(DMA_OUT)/code.mem > $(DMA_OUT)/code.mem.LSB

i2c_dma.riscv: crt.o syscalls.shakti create_dir
	@echo -e "$(GREEN) Compiling I2C with DMA code $(NC)"
	@echo -e "$(RED) Caveat: I2C Code starts at 0x100000. Configure RTL appropriately $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(I2C_DIR)/i2c_dma.c -o $(I2C_DMA_OUT)/i2c_dma.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(I2C_DIR)/i2clink.ld $(I2C_DMA_OUT)/i2c_dma.o $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(I2C_DMA_OUT)/I2C_DMA.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(I2C_DMA_OUT)/I2C_DMA.shakti > $(I2C_DMA_OUT)/I2C_DMA.dump
	@elf2hex 8 32768 $(I2C_DMA_OUT)/I2C_DMA.shakti 16777216 > $(I2C_DMA_OUT)/code.mem
	@cut -c1-8 $(I2C_DMA_OUT)/code.mem > $(I2C_DMA_OUT)/code.mem.MSB
	@cut -c9-16 $(I2C_DMA_OUT)/code.mem > $(I2C_DMA_OUT)/code.mem.LSB

hfread.riscv: crt.o syscalls.shakti
	@echo -e "$(GREEN) Compiling Hype flash rread Code $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -g -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(HF_DIR)/hfread.c -o $(HF_OUT)/hfread.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(HF_OUT)/hfread.o  $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(HF_OUT)/hfread.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(HF_OUT)/hfread.shakti > $(HF_OUT)/hfread.dump

hyperbusi.riscv: crt.o syscalls.shakti
	@echo -e "$(GREEN) Compiling Hype flash rread Code $(NC)"
	@riscv$(XLEN)-unknown-elf-gcc -w -mcmodel=medany -static -std=gnu99 -fno-builtin-printf  -c $(HF_DIR)/hyperbusi1.c -o $(HF_OUT)/hyperbusi.o -march=rv$(XLEN)imafd -lm -lgcc
	@riscv$(XLEN)-unknown-elf-gcc -T $(CommonDIR)/link.ld $(HF_OUT)/hyperbusi.o  $(CommonDIR)/syscalls.shakti $(CommonDIR)/crt.o -o $(HF_OUT)/hyperbusi.shakti -static -nostartfiles -lm -lgcc
	@riscv$(XLEN)-unknown-elf-objdump -D $(HF_OUT)/hyperbusi.shakti > $(HF_OUT)/hyperbusi.dump



crt.o:
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf -D__ASSEMBLY__=1 -c $(CommonDIR)/crt.S -o $(CommonDIR)/crt.o

syscalls.shakti:
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf  -c $(CommonDIR)/syscalls.c -o $(CommonDIR)/syscalls.shakti

syscalls1.shakti:
	@riscv$(XLEN)-unknown-elf-gcc -march=rv$(XLEN)imafd  -mcmodel=medany -static -std=gnu99 -fno-common -fno-builtin-printf  -c $(CommonDIR)/syscalls1.c -o $(CommonDIR)/syscalls1.shakti

clean: 
	@rm -rf $(HF_OUT)/* $(I2C_OUT)/* $(I2C_DMA_OUT)/* $(DMA_OUT)/* $(CommonDIR)/*.o $(CommonDIR)/*.shakti $(QSPI_OUT)/* $(PLIC_OUT)/* $(DMA_OUT)/* $(BOOT_OUT)/* $(BOOT_DIR)/*.o $(BOOT_DIR)/*.shakti
	@find -type l -delete
	@echo "cleaned"
