#-----------------------------------------------------
# Generic dsPIC33AK 32-bit DSP microcontroller makefile
#-----------------------------------------------------
#TARGET_FILENAME target mk files.
TARGET_FILENAME := libx2cscope-generic-32dsp-dspic33a-elf.a

# Object dir must start with underscore!
# THis is a special directory, obj files will be merged in one common lib by the main makefile combine target.
OBJDIR := _OBJXCDSC

# Compiler specific CPU selection directive (-mcpu=xx -> XC-DSC -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mcpu=generic-32dsp-ak

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_DSPIC32__

OPTIMISATION := -O2
#Compilers to use
CC := $(XCDSC_CC)
AR := $(XCDSC_AR)

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) -C $(OBJDIR) -f "$(CURDIR)/makefile" \
	TARGET_FILENAME=$(TARGET_FILENAME) \
	OBJDIR=$(OBJDIR) \
	CC=$(CC) \
	AR=$(AR) \
	SET_PROCESSOR=$(SET_PROCESSOR) \
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