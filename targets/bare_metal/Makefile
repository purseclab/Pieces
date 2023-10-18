CC=clang
BIN := firmware.bin
PROJECT := EC
DEMO := BM
BUILD_DIR := build

SOURCE_FILES_EC_BM = main.c
SOURCE_FILES_EC_BM += jail.c






#Startup/boot related code, no need to look into it.
SOURCE_FILES_EC_BM += ./autogen/autogen_data.c
SOURCE_FILES_EC_BM += ./startup.c
SOURCE_FILES_EC_BM += ./libs/STM32F4xx_StdPeriph_Driver/src/misc.c
SOURCE_FILES_EC_BM += ./startup/system_stm32f4xx.c
SOURCE_FILES_EC_BM += ./libs/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SOURCE_FILES_EC_BM += ./libs/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
SOURCE_FILES_EC_BM += ./libs/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SOURCE_FILES_EC_BM += ./libs/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
#SOURCE_FILES_EC_BM += ./board/iar_stm32f407zg_sk.c


BM_EXE_CFLAGS=-flto=thin --target=arm-none-eabi -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -g -DSTM32F4xx 

CFLAGS_EC_${DEMO} = $(BM_EXE_CFLAGS)

#Finalize configuration variables based on project/demo
CFLAGS = ${CFLAGS_${PROJECT}_${DEMO}}
#LDFLAGS = ${LDFLAGS_${PROJECT}_${DEMO}} //Pretty much every configuration uses the same ld flags, can extend in future.
LDFLAGS = -v -T ./scripts/stm32_flash.ld  -specs=nano.specs --specs=rdimon.specs #-lc -lrdimon
LDFLAGS += -Xlinker -Map=${BUILD_DIR}/output.map
LDFLAGS += -L/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7-m/nofp -L/usr/lib/gcc/arm-none-eabi/9.2.1 -L/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib -L./ -v


INCLUDE_DIRS_EC_BM += -I./
INCLUDE_DIRS_EC_BM += -I ../../partitioner/monitor/include/
INCLUDE_DIRS_EC_BM += -I./CMSIS/Device/ST/STM32F4xx/Include/
INCLUDE_DIRS_EC_BM += -I./CMSIS/Include/
INCLUDE_DIRS_EC_BM += -I./libs/STM32F4xx_StdPeriph_Driver/inc/
INCLUDE_DIRS_EC_BM += -I./board/ -I /usr/lib/gcc/arm-none-eabi/9.2.1/include -I  /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/include/ -I /home/arslan/projects/LBC/checkedC-12/src/package/lib/clang/12.0.0/include/
INCLUDE_DIRS_EC_BM += -I /home/arslan/projects/LBC/stm32f429/00-STM32F429_LIBRARIES/ -I ./sdcard_app/ -I /home/arslan/projects/LBC/stm32f429/00-STM32F429_LIBRARIES/fatfs/ -I /home/arslan/projects/LBC/stm32f429/00-STM32F429_LIBRARIES/fatfs/drivers/

SOURCE_FILES = ${SOURCE_FILES_${PROJECT}_${DEMO}}
INCLUDE_DIRS = ${INCLUDE_DIRS_${PROJECT}_${DEMO}}


$(warn ${SOURCE_FILES})  


#Prepare variables for build rules
OBJ_FILES := $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)
CFLAGS += $(INCLUDE_DIRS)



.PHONY: clean

$(BUILD_DIR)/$(BIN) : $(OBJ_FILES) $(BUILD_DIR)/reg.o
	$(CC) -ffunction-sections -fdata-sections $(CFLAGS) $(LDFLAGS) $+ -o $(@)

%.d: %.c
	@set -e; rm -f $@; \
    $(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

INCLUDES := $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.d)
-include $(INCLUDES)

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/reg.o : reg.S
	$(CC) -c -ffunction-sections -fdata-sections $(CFLAGS)  $+ -o $(@)

clean:
	-rm -rf build

bc: $(OBJ_FILES)  
	llvm-link  -v $+ -o $(BUILD_DIR)/$(BIN).bc
	llvm-dis $(BUILD_DIR)/$(BIN).bc

exec: build/$(BIN).bc build/reg.o 
	llc -filetype=obj build/$(BIN).bc
	$(CC)  --target=arm-none-eabi -mthumb -mcpu=cortex-m4 $(LDFLAGS)  -Xlinker -Map=./rtmk.map build/$(BIN).bc $(BUILD_DIR)/reg.o -L ../../partitioner/monitor/ -lmonitor -o ./build/$(BIN)
	size -A -d ./build/$(BIN) | tee ./sizeinfo

ec:
	ansible-playbook ./bm-playbook.yml
rund:
	gdb-multiarch ./build/$(BIN)

run:
	../../emulators/qemu-arm -M stm32-p103 -semihosting -nographic -kernel ./build/firmware.bin -s -S 
