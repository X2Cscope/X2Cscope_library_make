#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-cm33-elf.a

# Object dir must start with underscore!
OBJDIR := _CM33

# Compiler specific CPU selection directive (-mcpu=xx -> XC16 -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mprocessor=PIC32CK2051SG01144

# Additional arguments for the compiler
SET_ARGS := "-mdfp="

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_PIC32__

OPTIMISATION := -O2
#Compilers to use
CC := $(XC32_CC)
AR := $(XC32_AR)

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) -C $(OBJDIR) -f "$(CURDIR)/makefile" \
	TARGET_FILENAME=$(TARGET_FILENAME) \
	OBJDIR=$(OBJDIR) \
	CC=$(CC) \
	AR=$(AR) \
	SET_PROCESSOR=$(SET_PROCESSOR) \
	SET_ARGS=$(SET_ARGS) \
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