#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-cm0-elf.a

# Object dir must start with underscore!
OBJDIR := _CM0

# Generic Cortex-M build without FPU: XC32's ARM compiler with core options
# only (no -mprocessor, no device pack). ARMv6-M code with the soft-float ABI
# runs on every Cortex-M core and links into every soft-float project:
#   Cortex-M0+ (SAM D/C/L/R, PIC32CM PL/JH/MC/GV)
#   Cortex-M23 (PIC32CM LE/LS/GC00/SG00, SAML10/11)
#   Cortex-M4 without FPU (PIC32CX-SG)
SET_PROCESSOR := -mcpu=cortex-m0plus
SET_ARGS := -mthumb -mfloat-abi=soft

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
