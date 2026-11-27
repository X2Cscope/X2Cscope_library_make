#--------------------------------------------
# Makefile based on the following tutorial: http://make.mad-scientist.net/papers/multi-architecture-builds/
# If new target required just duplicate one of the *.mk file and modify it. Then just execute the make command in the folder.
# This makefile will call all *.mk file in the current folder and compile all targets defined in *.mk files.
#--------------------------------------------
# Global config may be changed
NUM_OF_PARALLEL_JOBS := 4
LIB_OUT_DIR_PATH := dist/

# compilers (can be overridden on the command line, e.g. make XC32_CC=...)
# scripts/make_release.ps1 checks that these versions are installed
XC32_VERSION  := v4.60
XCDSC_VERSION := v4.00

# XC32_ARM_CC is XC32's ARM compiler (pic32c-gcc). xc32-gcc selects it only
# when a device is given with -mprocessor; calling it directly allows generic
# Cortex-M builds with -mcpu/-mfpu/-mfloat-abi and no device pack.
ifeq ($(OS),Windows_NT)
XC32_CC := "C:\Program Files\Microchip\xc32\$(XC32_VERSION)\bin\xc32-gcc.exe"
XC32_AR := "C:\Program Files\Microchip\xc32\$(XC32_VERSION)\bin\xc32-ar.exe"
XC32_ARM_CC := "C:\Program Files\Microchip\xc32\$(XC32_VERSION)\bin\bin\pic32c-gcc.exe"

XCDSC_CC := "C:\Program Files\Microchip\xc-dsc\$(XCDSC_VERSION)\bin\xc-dsc-gcc.exe"
XCDSC_AR := "C:\Program Files\Microchip\xc-dsc\$(XCDSC_VERSION)\bin\xc-dsc-ar.exe"
else
XC32_CC := /opt/microchip/xc32/$(XC32_VERSION)/bin/xc32-gcc
XC32_AR := /opt/microchip/xc32/$(XC32_VERSION)/bin/xc32-ar
XC32_ARM_CC := /opt/microchip/xc32/$(XC32_VERSION)/bin/bin/pic32c-gcc

XCDSC_CC := /opt/microchip/xc-dsc/$(XCDSC_VERSION)/bin/xc-dsc-gcc
XCDSC_AR := /opt/microchip/xc-dsc/$(XCDSC_VERSION)/bin/xc-dsc-ar
endif

#----------------------------------
# do not change the rest
# Output dirs start with underscore
ifeq (,$(filter _%,$(notdir $(CURDIR))))
# execute all *.mk files
export LIB_OUT_DIR_PATH XC16_CC XC16_AR XC32_CC XC32_AR XC32_ARM_CC XCDSC_CC XCDSC_AR
TARGETS = $(wildcard *.mk)

TOPTARGETS := all clean release

all: $(TARGETS) combine

# Full rebuild of every archive in dist/. Used by scripts/make_release.ps1.
release:
	$(MAKE) clean
	$(MAKE)
	$(info Release archives:)
	ls -l $(LIB_OUT_DIR_PATH)*.a
clean: $(TARGETS)
	$(info Removing all build directories _*/)
	$(foreach d,$(wildcard _*/),$(shell rm -rf $(d)))
	rm -f $(LIB_OUT_DIR_PATH)$(COMBINED_LIB)
$(TARGETS):
	$(MAKE) -f $@ $(MAKECMDGOALS)


#XCDSC specific
# Combined archive of all objects in _OBJXCDSC (not owned by any *.mk file)
COMBINED_LIB := libx2cscope2-generic-pic24-dspic33-elf.a

combine:
	$(info Combining XCDSC libs)
	$(eval XC_DSC_OBJ_FILES := ${wildcard _OBJXCDSC/*.o})
	$(info obj files to combine: ${XC_DSC_OBJ_FILES})
	rm -f ${LIB_OUT_DIR_PATH}$(COMBINED_LIB)
	${XCDSC_AR} cvq ${LIB_OUT_DIR_PATH}$(COMBINED_LIB)  ${XC_DSC_OBJ_FILES}

.PHONY: $(TOPTARGETS) $(TARGETS) combine

else
#----- Do not change the rest
# relative to _OBJ dir
# Folder names are case sensitive on Linux (CI)
SRC_DIR := ../X2CScope/src
INC_DIR := ../X2CScope/inc -I../interface

VPATH = $(SRC_DIR)

# Remove -mcu= and -mprocessor from SET_PROCESSOR and store it in SET_ARCH
SET_ARCH := $(subst -mcpu=,,$(SET_PROCESSOR))
SET_ARCH := $(subst -mprocessor=,,$(SET_ARCH))

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
$(if $(SRC_FILES),,$(error No source files found in $(SRC_DIR)))
SRC_FILES_WODIR := $(notdir ${SRC_FILES})
OBJ_FILES := $(SRC_FILES_WODIR:%.c=${SET_ARCH}%.o)

$(info OBJ_FILES ${OBJ_FILES})


$(TARGET_FILENAME): $(OBJ_FILES)
	$(info ${OBJ_FILES})
# relative to obj dir	
	gnumkdir -p ../$(LIB_OUT_DIR_PATH)
	${AR} -omf=elf -r ../$(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)  $^  

#create MCU specific obj files
${SET_ARCH}%.o: %.c
	$(info Compiling)
	${CC} ${SET_PROCESSOR} ${SET_ARGS} -D${DEFINE_X2C_MCU_FAMILY} -I${INC_DIR} ${OPTIMISATION} -c $< -o $@ 


#----- Begin Boilerplate
endif