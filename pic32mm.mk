#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-32mm-elf.a

# Object dir must start with underscore!
OBJDIR := _32MM

# Compiler specific CPU selection directive (-mcpu=xx -> XC16 -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mprocessor=32MM0256GPM064

# Set MCU family and scope size for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_PIC32__
DEFINE_SCOPE_SIZE := SCOPE_SIZE=5000 

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