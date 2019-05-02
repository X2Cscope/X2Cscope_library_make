.SUFFIXES:
#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope-33ck-elf.a
# Object dir must start with underscore!
OBJDIR := _33CK
#Compilers to use
CC := "xc16-gcc"
AR := "xc16-ar"
# Compiler specific CPU selection directive
SET_PROCESSOR := -mcpu=33CK32MP202

# Set MCU family for X2C Scope
DEFINE_X2C_MCU_FAMILY := __GENERIC_MICROCHIP_DSPIC__

export CC AR SET_PROCESSOR DEFINE_X2C_MCU_FAMILY TARGET_FILENAME

#-----------------------------------------------------
# DO not change the rest
MAKETARGET = $(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile SRCDIR=$(CURDIR)/X2CScope/src $(MAKECMDGOALS)

.PHONY: $(OBJDIR)
$(OBJDIR):
	+@[ -d $@ ] || gnumkdir -p  $@
	+@$(MAKETARGET)

makefile : ;
%.mk :: ;

% :: $(OBJDIR) ; :

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)