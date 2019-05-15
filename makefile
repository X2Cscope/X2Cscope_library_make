#--------------------------------------------
# Makefile based on the following tutorial: http://make.mad-scientist.net/papers/multi-architecture-builds/
# If new target required just duplicate one of the *.mk file and modify it. Then just execute the make command in the folder.
# This makefile will call all *.mk file in the current folder and compile all targets defined in *.mk files.
#--------------------------------------------
# Global config may be changed
NUM_OF_PARALLEL_JOBS := 4
LIB_OUT_DIR_PATH := dist/
#----------------------------------
# do not change the rest
# Output dirs start with underscore
ifeq (,$(filter _%,$(notdir $(CURDIR))))
# execute all *.mk files
export LIB_OUT_DIR_PATH
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