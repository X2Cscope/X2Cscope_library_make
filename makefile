#--------------------------------------------
# Makefile based on the following tutorial: http://make.mad-scientist.net/papers/multi-architecture-builds/
# If new target required just duplicate one of the *.mk file and modify it. Then just execute the make command in the folder.
# This makefile will call all *.mk file in the current folder and compile all targets defined in *.mk files.
#--------------------------------------------
# Global config may be changed
NUM_OF_PARALLEL_JOBS := 4
LIB_OUT_DIR_PATH := dist/

# compilers

XC32_CC := "C:\Program Files\Microchip\xc32\v4.35\bin\xc32-gcc.exe"
XC32_AR := "C:\Program Files\Microchip\xc32\v4.35\bin\xc32-ar.exe"

XCDSC_CC := "C:\Program Files\Microchip\xc-dsc\v3.20\bin\xc-dsc-gcc.exe"
XCDSC_AR := "C:\Program Files\Microchip\xc-dsc\v3.20\bin\xc-dsc-ar.exe"

#----------------------------------
# do not change the rest
# Output dirs start with underscore
ifeq (,$(filter _%,$(notdir $(CURDIR))))
# execute all *.mk files
export LIB_OUT_DIR_PATH XC16_CC XC16_AR XC32_CC XC32_AR XCDSC_CC XCDSC_AR
TARGETS = $(wildcard *.mk)

TOPTARGETS := all clean

$(TOPTARGETS): $(TARGETS) combine
$(TARGETS):
	$(MAKE) -f $@ $(MAKECMDGOALS)


#XCDSC specific

combine:
	$(info Combining XCDSC libs)
	$(eval XC_DSC_OBJ_FILES := ${wildcard _OBJXCDSC/*.o})
	$(info obj files to combine: ${XC_DSC_OBJ_FILES})
	${XCDSC_AR} cvq ${LIB_OUT_DIR_PATH}libx2cscope2-generic-pic24-dspic33-elf.a  ${XC_DSC_OBJ_FILES}

.PHONY: $(TOPTARGETS) $(TARGETS)

else
#----- Do not change the rest
# relative to _OBJ dir
SRC_DIR := ../X2Cscope/src
INC_DIR := ../X2Cscope/inc

VPATH = $(SRC_DIR)

# Remove -mcu= and -mprocessor from SET_PROCESSOR and store it in SET_ARCH
SET_ARCH := $(subst -mcpu=,,$(SET_PROCESSOR))
SET_ARCH := $(subst -mprocessor=,,$(SET_ARCH))

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
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
	${CC} ${SET_PROCESSOR} -D${DEFINE_X2C_MCU_FAMILY} -I${INC_DIR} ${OPTIMISATION} -c $< -o $@ 


#----- Begin Boilerplate
endif