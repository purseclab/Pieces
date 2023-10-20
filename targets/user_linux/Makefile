CC=clang
BIN := firmware.bin
PROJECT := EC
DEMO := LINUX
BUILD_DIR := build

SOURCE_FILES_EC_LINUX = main.c
SOURCE_FILES_EC_LINUX += ./jail.c
SOURCE_FILES_EC_LINUX += ./autogen/autogen_data.c

LINUX_EXE_CFLAGS= -flto=thin -O0 -g --static 

CFLAGS_EC_${DEMO} = $(LINUX_EXE_CFLAGS)

#Finalize configuration variables based on project/demo
CFLAGS = ${CFLAGS_${PROJECT}_${DEMO}}
#LDFLAGS = ${LDFLAGS_${PROJECT}_${DEMO}} //Pretty much every configuration uses the same ld flags, can extend in future.
LDFLAGS = -v 
LDFLAGS += -Xlinker -Map=${BUILD_DIR}/output.map -T ./scripts/elf_x86_64.xce


INCLUDE_DIRS_EC_LINUX += -I./
INCLUDE_DIRS_EC_LINUX += -I ../../partitioner/monitor/include/

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

exec: build/$(BIN).bc 
	llc -filetype=obj build/$(BIN).bc
	$(CC)  -fuse-ld=lld $(LDFLAGS)  -Xlinker -Map=./rtmk.map build/$(BIN).bc  -lgcc -lc  -L ../../partitioner/monitor/ -lmonitor -o ./build/$(BIN)
	size -A -d ./build/$(BIN) | tee ./sizeinfo

ec:
	ansible-playbook ./linux-playbook.yml
rund:
	gdb-multiarch ./build/$(BIN)
