#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-cm4-elf.a

# Object dir must start with underscore!
OBJDIR := _CM4

# Generic Cortex-M4F build: XC32's ARM compiler with core options only
# (no -mprocessor, no device pack). ARMv7E-M, FPv4-SP, hard-float ABI:
#   SAME5x, SAMD51, SAMG, PIC32CX BZ/MT/SG41/SG60/SG61
# Not for Cortex-M4 devices without FPU: use libx2cscope2-cm0-elf.a.
SET_PROCESSOR := -mcpu=cortex-m4
SET_ARGS := -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_PIC32__

OPTIMISATION := -O2
#Compilers to use
CC := $(XC32_ARM_CC)
AR := $(XC32_AR)

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) -C $(OBJDIR) -f "$(CURDIR)/makefile" \
	TARGET_FILENAME=$(TARGET_FILENAME) \
	OBJDIR=$(OBJDIR) \
	CC=$(CC) \
	AR=$(AR) \
	SET_PROCESSOR=$(SET_PROCESSOR) \
	SET_ARGS="$(SET_ARGS)" \
	DEFINE_X2C_MCU_FAMILY=$(DEFINE_X2C_MCU_FAMILY) \
	OPTIMISATION=$(OPTIMISATION) \
	$(MAKECMDGOALS)
	
.PHONY: $(OBJDIR)
$(OBJDIR):
	+@[ -d $(OBJDIR) ] || gnumkdir -p  $(OBJDIR)
	+@$(MAKETARGET)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)
