####################################################################
# Makefile                                                         #
####################################################################

.SUFFIXES:				# ignore builtin rules
.PHONY: all debug release clean

####################################################################
# Definitions                                                      #
####################################################################

DEVICE = EFM32GG12B110F1024GQ64
PROJECTNAME = efmgg12b
TA_PROJECTNAME = efmgg12bTA

OBJ_DIR = build
EXE_DIR = exe
LST_DIR = lst

####################################################################
# Definitions of toolchain.                                        #
# You might need to do changes to match your system setup          #
####################################################################

TOOLDIR    = ../gnu_arm-7.2_2017q4

RM         := rm -rf
NULLDEVICE := /dev/null
SHELLNAMES := $(ComSpec)$(COMSPEC)


# Create directories and do a clean which is compatible with parallell make
$(shell mkdir $(OBJ_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(EXE_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(LST_DIR)>$(NULLDEVICE) 2>&1)
ifeq (clean,$(findstring clean, $(MAKECMDGOALS)))
  ifneq ($(filter $(MAKECMDGOALS),all debug release),)
    $(shell $(RM) $(OBJ_DIR)/*>$(NULLDEVICE) 2>&1)
    $(shell $(RM) $(EXE_DIR)/*>$(NULLDEVICE) 2>&1)
    $(shell $(RM) $(LST_DIR)/*>$(NULLDEVICE) 2>&1)
  endif
endif


# Create directories and do a clean which is compatible with parallell make
$(shell mkdir ta>$(NULLDEVICE) 2>&1)
$(shell mkdir ta/$(OBJ_DIR)>$(NULLDEVICE) 2>&1)
ifeq (clean,$(findstring clean, $(MAKECMDGOALS)))
  ifneq ($(filter $(MAKECMDGOALS),all debug release),)
	$(shell $(RM) ta/$(OBJ_DIR)/*>$(NULLDEVICE) 2>&1)
  endif
endif
CC      = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-gcc$(QUOTE)
LD      = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-ld$(QUOTE)
AR      = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-ar$(QUOTE)
OBJCOPY = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-objcopy$(QUOTE)
DUMP    = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-objdump$(QUOTE)
SIZE    = $(QUOTE)$(TOOLDIR)/bin/arm-none-eabi-size$(QUOTE)

####################################################################
# Flags                                                            #
####################################################################

# -MMD : Don't generate dependencies on system header files.
# -MP  : Add phony targets, useful when a h-file is removed from a project.
# -MF  : Specify a file to write the dependencies to.
DEPFLAGS = -MMD -MP -MF $(@:.o=.d)

BUILD_NUMBER=$(shell git log --pretty=format:"%h" -1)

#
# Add -Wa,-ahld=$(LST_DIR)/$(@F:.o=.lst) to CFLAGS to produce assembly list files
#
override CFLAGS += -D$(DEVICE) -Wall -Wextra  -g -gdwarf-2 -mcpu=cortex-m4 -mthumb '-DEFM32GG12B110F1024GQ64=1' \
-mfpu=fpv4-sp-d16 -mfloat-abi=softfp \
-Os -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 \
-MMD -MP -ldrd -fomit-frame-pointer $(DEPFLAGS) -D__BUILD_NUMBER=\"$(BUILD_NUMBER)\"

override TA_CFLAGS += -D$(DEVICE) -Wall -Wextra  -g -gdwarf-2 -mcpu=cortex-m4 -mthumb '-DEFM32GG12B110F1024GQ64=1' \
-mfpu=fpv4-sp-d16 -mfloat-abi=softfp \
-Os -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 \
-MMD -MP -ldrd -fomit-frame-pointer $(DEPFLAGS) -D__BUILD_NUMBER=\"TA:$(BUILD_NUMBER)\"

override ASMFLAGS += -g -gdwarf-2 -x assembler-with-cpp -D$(DEVICE) -Wall -Wextra -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 \
 -mfloat-abi=softfp


#
# NOTE: The -Wl,--gc-sections flag may interfere with debugging using gdb.
#
override LDFLAGS +=  -g -gdwarf-2 -Xlinker -Map=$(LST_DIR)/$(PROJECTNAME).map -mcpu=cortex-m4 \
-mthumb -T../Device/EFM32GG12B/Source/efm32gg12b.ld -Wl,--gc-sections --specs=nano.specs
override TA_LDFLAGS +=  -g -gdwarf-2 -Xlinker -Map=$(LST_DIR)/$(PROJECTNAME).map -mcpu=cortex-m4 \
-mthumb -T../Device/EFM32GG12B/Source/efm32gg12b_ta.ld -Wl,--gc-sections --specs=nano.specs

LIBS = -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group

INCLUDEPATHS += \
-I.. \
-I../Device \
-I../CMSIS \
-I../emlib/inc \
-I../inc \
-I.

####################################################################
# Files                                                            #
####################################################################


C_SRC +=  \
../Device/EFM32GG12B/Source/system_efm32gg12b.c \
../src/usart.c \
../src/crc.c \
../src/bootloader.c \
../src/boot.c \
../src/xmodem.c \
../src/flash.c 

s_SRC +=  \

S_SRC += \
../Device/EFM32GG12B/Source/startup_efm32gg12b.S

####################################################################
# Rules                                                            #
####################################################################

C_FILES = $(notdir $(C_SRC) )
S_FILES = $(notdir $(S_SRC) $(s_SRC) )
#make list of source paths, sort also removes duplicates
C_PATHS = $(sort $(dir $(C_SRC) ) )
S_PATHS = $(sort $(dir $(S_SRC) $(s_SRC) ) )

C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
S_OBJS = $(if $(S_SRC), $(addprefix $(OBJ_DIR)/, $(S_FILES:.S=.o)))
s_OBJS = $(if $(s_SRC), $(addprefix $(OBJ_DIR)/, $(S_FILES:.s=.o)))
C_DEPS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.d))
OBJS = $(C_OBJS) $(S_OBJS) $(s_OBJS)

TA_C_OBJS = $(addprefix ta/$(OBJ_DIR)/, $(C_FILES:.c=.o))
TA_S_OBJS = $(if $(S_SRC), $(addprefix ta/$(OBJ_DIR)/, $(S_FILES:.S=.o)))
TA_s_OBJS = $(if $(s_SRC), $(addprefix ta/$(OBJ_DIR)/, $(S_FILES:.s=.o)))
TA_C_DEPS = $(addprefix ta/$(OBJ_DIR)/, $(C_FILES:.c=.d))
TA_OBJS = $(TA_C_OBJS) $(TA_S_OBJS) $(TA_s_OBJS)
vpath %.c $(C_PATHS)
vpath %.s $(S_PATHS)
vpath %.S $(S_PATHS)

# Default build is debug build
all: release testapp


release:  CFLAGS += -g3 -Os
release:  $(EXE_DIR)/$(PROJECTNAME).bin

# Create objects from C SRC files
$(OBJ_DIR)/%.o: %.c
	@echo "Building file: $<"
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Assemble .s/.S files
$(OBJ_DIR)/%.o: %.s
	@echo "Assembling $<"
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.S
	@echo "Assembling $<"
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Link
$(EXE_DIR)/$(PROJECTNAME).out: $(OBJS)
	@echo "Linking target: $@"
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXE_DIR)/$(PROJECTNAME).out


	
# Create binary file
$(EXE_DIR)/$(PROJECTNAME).bin: $(EXE_DIR)/$(PROJECTNAME).out
	@echo "Creating binary file"
	$(OBJCOPY) -O binary $(EXE_DIR)/$(PROJECTNAME).out $(EXE_DIR)/$(PROJECTNAME).bin
# Uncomment next line to produce assembly listing of entire program
	$(DUMP) -h -S -C $(EXE_DIR)/$(PROJECTNAME).out>$(LST_DIR)/$(PROJECTNAME)out.lst
# Print the size
	$(SIZE) $(EXE_DIR)/$(PROJECTNAME).out

	
	
testapp:   $(EXE_DIR)/$(TA_PROJECTNAME).bin

# Create objects from C SRC files
ta/$(OBJ_DIR)/%.o: %.c
	@echo "Building file ta: $<"
	$(CC) $(TA_CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Assemble .s/.S files
ta/$(OBJ_DIR)/%.o: %.s
	@echo "Assembling ta$<"
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

ta/$(OBJ_DIR)/%.o: %.S
	@echo "Assembling ta$<"
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Link
$(EXE_DIR)/$(TA_PROJECTNAME).out: $(TA_OBJS)
	@echo "Linking target: $@"
	$(CC) $(TA_LDFLAGS) $(TA_OBJS) $(LIBS) -o $(EXE_DIR)/$(TA_PROJECTNAME).out
	
# Create binary file
$(EXE_DIR)/$(TA_PROJECTNAME).bin: $(EXE_DIR)/$(TA_PROJECTNAME).out
	@echo "Creating binary file"
	$(OBJCOPY) -O binary $(EXE_DIR)/$(TA_PROJECTNAME).out $(EXE_DIR)/$(TA_PROJECTNAME).bin
# Uncomment next line to produce assembly listing of entire program
	$(DUMP) -h -S -C $(EXE_DIR)/$(TA_PROJECTNAME).out>$(LST_DIR)/$(TA_PROJECTNAME)out.lst
# Print the size
	$(SIZE) $(EXE_DIR)/$(TA_PROJECTNAME).out

test:
	@echo "python ../efm_uploadBootloader_python3/test_uploads.py $(EXE_DIR)/$(PROJECTNAME).bin $(EXE_DIR)/$(TA_PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))"
	@python ../efm_uploadBootloader_python3/test_uploads.py  $(EXE_DIR)/$(PROJECTNAME).bin $(EXE_DIR)/$(TA_PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))

upload_python3:
	@echo "python ../efm_uploadBootloader_python3/efm_uploadBootloader.py  $(EXE_DIR)/$(PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))"
	@python ../efm_uploadBootloader_python3/efm_uploadBootloader.py  $(EXE_DIR)/$(PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))

upload:
#To use upload with bash on windows use sudo and ttyS# where # is the same as the # for the COM port
	@echo "python2 ../efm_uploadBootloader/efm_upload_bootloader.py  $(EXE_DIR)/$(PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))"
	@python2 ../efm_uploadBootloader/efm_upload_bootloader.py  $(EXE_DIR)/$(PROJECTNAME).bin $(filter-out $@,$(MAKECMDGOALS))

%:
	@:

clean:
ifeq ($(filter $(MAKECMDGOALS),all debug release),)
	$(RM) ta/$(OBJ_DIR)
	$(RM) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR)
endif

# include auto-generated dependency files (explicit rules)
ifneq (clean,$(findstring clean, $(MAKECMDGOALS)))
-include $(C_DEPS)
endif
