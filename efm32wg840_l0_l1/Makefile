####################################################################
# Makefile                                                         #
####################################################################

.SUFFIXES:				# ignore builtin rules
.PHONY: all debug release clean

####################################################################
# Definitions                                                      #
####################################################################

DEVICE = EFM32WG840F256
PROJECTNAME = efmwg840

OBJ_DIR = build
EXE_DIR = exe
LST_DIR = lst

####################################################################
# Definitions of toolchain.                                        #
# You might need to do changes to match your system setup          #
####################################################################

TOOLDIR    = ../arm-4.8.3-2014q1

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

#
# Add -Wa,-ahld=$(LST_DIR)/$(@F:.o=.lst) to CFLAGS to produce assembly list files
#
override CFLAGS += -D$(DEVICE) -Wall -Wextra  -g -gdwarf-2 -mcpu=cortex-m4 -mthumb '-DEFM32WG840F256=1' \
-Os -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 \
-MMD -MP -ldrd -fomit-frame-pointer $(DEPFLAGS)

override ASMFLAGS += -g -gdwarf-2 -x assembler-with-cpp -D$(DEVICE) -Wall -Wextra -mcpu=cortex-m4 -mthumb


#
# NOTE: The -Wl,--gc-sections flag may interfere with debugging using gdb.
#
override LDFLAGS +=  -g -gdwarf-2 -Xlinker -Map=$(LST_DIR)/$(PROJECTNAME).map -mcpu=cortex-m4 \
-mthumb -T../Device/EFM32WG/Source/efm32wg.ld -Wl,--gc-sections --specs=nano.specs

LIBS = -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group

INCLUDEPATHS += \
-I../Device \
-I../CMSIS \
-I../emlib/inc \
-I../inc \
-I.

####################################################################
# Files                                                            #
####################################################################

C_SRC +=  \
../Device/EFM32WG/Source/system_efm32wg.c \
../src/xmodem.c \
../src/leuart.c \
../src/flash.c \
../src/crc.c \
../src/bootloader.c \
../src/boot.c

s_SRC +=  \

S_SRC += \
../Device/EFM32WG/Source/startup_efm32wg.S

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

vpath %.c $(C_PATHS)
vpath %.s $(S_PATHS)
vpath %.S $(S_PATHS)

# Default build is debug build
all:      release

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

prog:
	start-eACommander.sh --verify --flash exe/efmwg840.bin --reset

clean:
ifeq ($(filter $(MAKECMDGOALS),all debug release),)
	$(RM) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR)
endif

# include auto-generated dependency files (explicit rules)
ifneq (clean,$(findstring clean, $(MAKECMDGOALS)))
-include $(C_DEPS)
endif
