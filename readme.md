Read-me File for making X2Cscope Libraries for MCUs 
-----------------------------------------------
This file contains the following sections:
1. Summary
2. How to execute
3. How to create new target
4. Troubleshooting
5. API migration
6. Integration examples

### 1. Summary
This folder contains a makefile and \*.mk files for target specification. 
The makefile compiles the X2C Scope source files for the target MCUs.

### 2. How to execute

#### **Prerequisites**

1. Compilers: XC16 and/or XC32 and/or XC-DSC http://microchip.com/xc
1. GNU Make: MPLAB X IDE contains it: http://microchip.com/mplabx 
1. Modify system path environment to make.exe be available in command line: C:\Program Files (x86)\Microchip\MPLABX\\__*v5.20*__\gnuBins\GnuWin32\bin 

![Path var](doc/UserVariables.png)

4. Modify makefile compiler paths to your XC compiler installation paths:

![Path var](doc/CompilerPaths.png)

#### **Execute**
Open cmd in the root folder and execute "make" command. The main makefile will call all \*.mk file in the folder and compile all targets.

#### **Clean**
Execute "make clean" to delete all obj and lib files.

### 3. How to create new/custom target
If new target or custom configuration(Scope_size) required just duplicate one of the \*.mk file and modify the following variables to the needs:
1. TARGET_FILENAME - The output filename. (Will be located in the dist folder after compilation.)
1. OBJDIR - Folder to store the temporary object files. Must start with underscore(_) and must be different than other .mk files!
1. SET_PROCESSOR - Use XC Readme.html for the list of supported Devices. (xc32\v2.xx\docs)
1. DEFINE_X2C_MCU_FAMILY - Check the X2C Scope header files for details. Keep as it is.
1. OPTIMISATION - Compiler optimisation level

Then just execute the make command in the folder and makefile will call automatically the newly created .mk file.


### 4. Troubleshooting

* 'make' is not recognized as an internal or external command, operable program or batch file.
  * Set Path environment as described in the Prerequisites section. (See at **Prerequisites**)
* The system cannot find the file specified.
  * Edit makefile and set the correct compiler paths according to your system. (See at **Prerequisites**)

### 5. API migration

If updating from a previous version of the X2Cscope interface files, see
`MIGRATION_GUIDE.md` for the full description of the `X2Cscope_InitialiseEx()`
/ `X2CSCOPE_CONFIG_INIT()` API and the required migration steps.

**Note**: The legacy `X2Cscope_HookUARTFunctions()` + `X2Cscope_Initialise()`
two-step API has been removed. Existing code using those functions must migrate
to `X2Cscope_InitialiseEx()`.

### 6. Integration examples

The `interface/examples/` folder contains ready-to-use communication layer
implementations for three transport interfaces. Each example provides
`X2CscopeComm.h` and `X2CscopeComm.c` — the only files that need to be adapted
per project — alongside any required peripheral driver files and a `README.md`
with hardware configuration and integration steps.

| Folder | Interface | Key files included |
|---|---|---|
| `interface/examples/uart/` | UART (MCC Melody driver) | `X2CscopeComm.h`, `X2CscopeComm.c` |
| `interface/examples/can/` | CAN 2.0 (MCC Melody driver) | `X2CscopeComm.h`, `X2CscopeComm.c` |
| `interface/examples/tcpip/` | TCP/IP over 10BASE-T1S (lwIP) | `X2CscopeComm.h`, `X2CscopeComm.c`, `lwip/tcp_server.h/.c`, `hal/dmaspi.h/.c` |

Copy the contents of the relevant example folder into your project's `X2Cscope/`
folder alongside `interface/X2Cscope.h` and `interface/X2Cscope.c`, then follow
the `README.md` in that example folder for MCC configuration and call sequence.

The PC-side tool is [pyX2Cscope](https://github.com/X2Cscope/pyx2cscope/releases):

```
pip install pyx2cscope
pyx2cscope        # Qt GUI
pyx2cscope -w     # Web GUI (browser)
```