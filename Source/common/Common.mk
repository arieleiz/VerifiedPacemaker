GCC_BIN = 
NATIVE=../common/native

LIB_OBJECTS = $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/HAL_CM3.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/SVC_Table.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Task.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_MemBox.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Robin.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mailbox.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_System.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_CMSIS.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Event.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Semaphore.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/RTX_Conf_CM.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Time.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mutex.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/rt_List.o $(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/HAL_CM.o $(NATIVE)/mbed-rtos/rtos/RtosTimer.o $(NATIVE)/mbed-rtos/rtos/Thread.o $(NATIVE)/mbed-rtos/rtos/Mutex.o $(NATIVE)/mbed-rtos/rtos/Semaphore.o $(NATIVE)/TextLCD/TextLCD.o
SYS_OBJECTS = $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o 

INCLUDE_PATHS = -I. -I../common/ -I$(NATIVE) -I$(NATIVE)/mbed -I$(NATIVE)/mbed/TARGET_LPC1768 -I$(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM -I$(NATIVE)/mbed/TARGET_LPC1768/TARGET_NXP -I$(NATIVE)/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X -I$(NATIVE)/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 -I./TextLCD -I$(NATIVE)/mbed-rtos -I$(NATIVE)/mbed-rtos/rtx -I$(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M -I$(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3 -I$(NATIVE)/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC -I$(NATIVE)/mbed-rtos/rtos 
LIBRARY_PATHS = -L$(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 
LIBRARIES = -lmbed 
LINKER_SCRIPT = $(NATIVE)/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

############################################################################### 
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE 	= $(GCC_BIN)arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer 
CC_FLAGS += -MMD -MP 
CC_SYMBOLS = -DTARGET_LPC1768 -DTARGET_M3 -DTARGET_CORTEX_M -DTARGET_NXP -DTARGET_LPC176X -DTARGET_MBED_LPC1768 -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -D__CORTEX_M3 -DARM_MATH_CM3 -DMBED_BUILD_TIMESTAMP=1417402140.61 -D__MBED__=1 

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float -u _scanf_float

LD_FLAGS += -Wl,-Map=$(PROJECT).map,--cref -Wl,--gc-sections 
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0 
else
  CC_FLAGS += -DNDEBUG -Os
  LD_FLAGS += -Os
endif

all: $(PROJECT).bin $(PROJECT).hex 

clean:
	rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(OBJECTS) $(DEPS)

cleanall:
	rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(OBJECTS) $(DEPS) $(LIB_OBJECTS)


.s.o:
	$(AS) $(CPU) -o $@ $<

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS) $(LIB_OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)
	$(SIZE) $@

$(PROJECT).bin: $(PROJECT).elf
	@$(OBJCOPY) -O binary $< $@
	../mbedcsum/release/mbedcsum.exe $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROJECT).lst

size:
	$(SIZE) $(PROJECT).elf

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)  $(LIB_OBJECTS:.o=.d)
-include $(DEPS)
