#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-cm33-elf.a

# Object dir must start with underscore!
OBJDIR := _CM33

# Generic Cortex-M33 build: XC32's ARM compiler with core options only
# (no -mprocessor, no device pack). ARMv8-M mainline, FPv5-SP, DSP extension,
# hard-float ABI:
#   PIC32CK SG/GC (TrustZone secure, non-secure or without TrustZone)
SET_PROCESSOR := -mcpu=cortex-m33
SET_ARGS := -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16

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
