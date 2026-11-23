#Change the following according the target MCU family
#---------------------------------------------
#TARGET_FILENAME and OBJDIR must not be the same in target mk files.
TARGET_FILENAME := libx2cscope2-cm33-elf.a

# Object dir must start with underscore!
OBJDIR := _CM33

# Compiler specific CPU selection directive (-mcpu=xx -> XC16 -mprocessor=xx -> XC32 )
# The generated objects are identical for PIC32CK SG01, MC90 and MC80 devices
# (Cortex-M33, FPv5-SP, DSP, hard-float ABI), so one device is enough.
SET_PROCESSOR := -mprocessor=PIC32CK2051SG01064

# Device Family Pack (DFP), downloaded automatically from the public Microchip
# pack server into $(PACKS_DIR). Use DFP_VERSION=latest or pin a version,
# e.g. make DFP_VERSION=1.11.288. Run "make -f cm33_pic32ck.mk update-dfp"
# to replace an already downloaded pack.
DFP_NAME      := PIC32CK-SG_DFP
DFP_SUBFAMILY := PIC32CK-SG01
DFP_VERSION   ?= latest
PACK_SERVER   := https://packs.download.microchip.com
PACKS_DIR     := .packs
DFP_ROOT      := $(PACKS_DIR)/$(DFP_NAME)
DFP_STAMP     := $(DFP_ROOT)/$(DFP_SUBFAMILY)/xc32

# Additional arguments for the compiler (path is relative to $(OBJDIR))
SET_ARGS := -mdfp=../$(DFP_ROOT)/$(DFP_SUBFAMILY)

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
$(OBJDIR): $(DFP_STAMP)
	+@[ -d $(OBJDIR) ] || gnumkdir -p  $(OBJDIR)
	+@$(MAKETARGET)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)

#-----------------------------------------------------
# DFP download
# Windows tar.exe (bsdtar) can extract zip based .atpack files, GNU tar cannot.
ifeq ($(OS),Windows_NT)
WIN_DIR := $(subst \,/,$(or $(SystemRoot),$(SYSTEMROOT),$(windir),C:/Windows))
EXTRACT = "$(WIN_DIR)/System32/tar.exe" -xf $(1) -C $(2)
else
EXTRACT = unzip -q -o $(1) -d $(2)
endif

# Latest version = first <release version="..."> entry of the published .pdsc
dfp_latest = $(patsubst <release@version="%",%,$(firstword $(filter <release@version="%", \
	$(subst <release ,<release@,$(shell curl -sfL $(PACK_SERVER)/Microchip.$(DFP_NAME).pdsc)))))

dfp_version = $(if $(filter latest,$(DFP_VERSION)),$(dfp_latest),$(DFP_VERSION))

$(DFP_STAMP):
	$(eval DFP_RESOLVED := $(dfp_version))
	$(if $(DFP_RESOLVED),,$(error Could not determine the $(DFP_NAME) version from $(PACK_SERVER)))
	$(info Downloading $(DFP_NAME) $(DFP_RESOLVED) from $(PACK_SERVER))
	rm -rf $(DFP_ROOT)
	gnumkdir -p $(DFP_ROOT)
	curl -fL --retry 3 -o $(DFP_ROOT).atpack $(PACK_SERVER)/Microchip.$(DFP_NAME).$(DFP_RESOLVED).atpack
	$(call EXTRACT,$(DFP_ROOT).atpack,$(DFP_ROOT))
	rm -f $(DFP_ROOT).atpack
	echo $(DFP_RESOLVED)> $(DFP_ROOT)/VERSION

.PHONY: dfp update-dfp
dfp: $(DFP_STAMP)

update-dfp:
	rm -rf $(DFP_ROOT)
	+@$(MAKE) -f cm33_pic32ck.mk dfp
