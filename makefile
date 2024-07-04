#--------------------------------------------
# Makefile based on the following tutorial: http://make.mad-scientist.net/papers/multi-architecture-builds/
# If new target required just duplicate one of the *.mk file and modify it. Then just execute the make command in the folder.
# This makefile will call all *.mk file in the current folder and compile all targets defined in *.mk files.
#--------------------------------------------
# Global config may be changed
NUM_OF_PARALLEL_JOBS := 4
LIB_OUT_DIR_PATH := dist/

# compilers

XC16_CC := "C:\Program Files\Microchip\xc16\v1.60\bin\xc16-gcc.exe"
XC16_AR := "C:\Program Files\Microchip\xc16\v1.60\bin\xc16-ar.exe"

XC32_CC := "C:\Program Files\Microchip\xc32\v3.00\bin\xc32-gcc.exe"
XC32_AR := "C:\Program Files\Microchip\xc32\v3.00\bin\xc32-ar.exe"

XC_DSC_CC := "C:\Program Files\Microchip\xc-dsc\v3.10\bin\xc32-gcc.exe"
XC_DSC_AR := "C:\Program Files\Microchip\xc-dsc\v3.10\bin\xc32-ar.exe"

#----------------------------------
# do not change the rest
# Output dirs start with underscore
ifeq (,$(filter _%,$(notdir $(CURDIR))))
# execute all *.mk files
export LIB_OUT_DIR_PATH XC16_CC XC16_AR XC32_CC XC32_AR
TARGETS = $(wildcard *.mk)

TOPTARGETS := all clean

$(TOPTARGETS): $(TARGETS)
$(TARGETS):
	$(MAKE) -f $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(TARGETS)

else
#----- Do not change the rest
# relative to _OBJ dir
SRC_DIR := ../X2CScope/src
INC_DIR := ../X2CScope/inc

VPATH = $(SRC_DIR)

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
SRC_FILES_WODIR := $(notdir ${SRC_FILES})
OBJ_FILES := $(SRC_FILES_WODIR:%.c=%.o)

$(TARGET_FILENAME): $(OBJ_FILES)
	$(info ${OBJ_FILES})
# relative to obj dir	
	gnumkdir -p ../$(LIB_OUT_DIR_PATH)
	${AR} -omf=elf -r ../$(LIB_OUT_DIR_PATH)$(TARGET_FILENAME)  $^  

%.o: %.c
	$(info Compiling)
	${CC} ${SET_PROCESSOR} -D${DEFINE_X2C_MCU_FAMILY} -D${DEFINE_SCOPE_SIZE}  -I${INC_DIR} ${OPTIMISATION} -c $< -o $@ 


#----- Begin Boilerplate
endif