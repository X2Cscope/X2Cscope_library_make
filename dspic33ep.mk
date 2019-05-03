#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope-33ep-elf.a
# Object dir must start with underscore!
OBJDIR := _33EP
#Compilers to use
CC := "xc16-gcc"
AR := "xc16-ar"
# Compiler specific CPU selection directive (-mcpu=xx -> XC16 -mprocessor=xx -> XC32 )
SET_PROCESSOR := -mcpu=33EP128MC202

# Set MCU family for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_DSPIC__

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) -C $(OBJDIR) -f $(CURDIR)/Makefile \
	TARGET_FILENAME=$(TARGET_FILENAME) \
	OBJDIR=$(OBJDIR) \
	CC=$(CC) \
	AR=$(AR) \
	SET_PROCESSOR=$(SET_PROCESSOR) \
	DEFINE_X2C_MCU_FAMILY=$(DEFINE_X2C_MCU_FAMILY) \
	$(MAKECMDGOALS)
	
.PHONY: $(OBJDIR)
$(OBJDIR):
	+@[ -d $(OBJDIR) ] || gnumkdir -p  $(OBJDIR)
	+@$(MAKETARGET)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)