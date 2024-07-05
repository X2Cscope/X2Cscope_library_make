#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope-33ak_mc512-elf.a

# Object dir must start with underscore!
OBJDIR := _33AK_MC512

# Compiler specific CPU selection directive (-mcpu=xx -> XC-DSC -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mcpu=33AK512MC510

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_DSPIC__
DEFINE_SCOPE_SIZE := SCOPE_SIZE=5000 

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