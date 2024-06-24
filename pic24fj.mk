#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-24fj-elf.a

# Object dir must start with underscore!
OBJDIR := _24FJ

# Compiler specific CPU selection directive (-mcpu=xx -> XC16 -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mcpu=24FJ64GB002

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_DSPIC__
DEFINE_SCOPE_SIZE := SCOPE_SIZE=5000 

OPTIMISATION := -O2
#Compilers to use
CC := $(XC16_CC)
AR := $(XC16_AR)

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) -C $(OBJDIR) -f "$(CURDIR)/makefile" \
	TARGET_FILENAME=$(TARGET_FILENAME) \
	OBJDIR=$(OBJDIR) \
	CC=$(CC) \
	AR=$(AR) \
	SET_PROCESSOR=$(SET_PROCESSOR) \
	DEFINE_X2C_MCU_FAMILY=$(DEFINE_X2C_MCU_FAMILY) \
	DEFINE_SCOPE_SIZE=$(DEFINE_SCOPE_SIZE) \
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