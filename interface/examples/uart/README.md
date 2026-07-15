# X2Cscope — UART Communication Example

This example implements the X2Cscope communication layer over a UART peripheral
using an MCC Melody-generated driver.

## Files

| File | Purpose |
|---|---|
| `X2CscopeComm.h` | Comm layer header — copy into your project's X2Cscope folder |
| `X2CscopeComm.c` | Comm layer implementation — copy into your project's X2Cscope folder |
| `../X2Cscope.c` | Scope buffer allocation and `X2Cscope_Init()` — copy from `interface/` |
| `../X2Cscope.h` | Public X2Cscope API — copy from `interface/` |

## Hardware Requirements

- Any Microchip MCU with at least one UART peripheral
- PC connected via USB-UART bridge (e.g. on-board debugger virtual COM port)
- UART configured at a supported baud rate (typically 115200 or 460800 baud)

## MCC Configuration

### UART peripheral (MCC Melody)

1. Add a **UART** peripheral (e.g. UART1) in MCC Melody.
2. Set the baud rate to match the X2Cscope PC tool setting (default **115200**).
3. Enable **RX** and **TX**.
4. No interrupts required — the example uses polling. If your application
   requires interrupt-driven TX (e.g. to avoid blocking the main loop), enable
   the TX interrupt and start it inside `X2CscopeComm_PostInit()`.
5. Generate the driver. The generated files will include:
   - `uart1.h` / `uart1.c` — peripheral driver
   - `extern const struct UART_INTERFACE UART1_Drv;` — driver interface instance

### Adapting `X2CscopeComm.c`

Open `X2CscopeComm.c` and update the two lines marked **Adapt**:

```c
#include "uart/uart1.h"                     /* path to your generated driver */
#define X2CScopeUart  UART1_Drv             /* your UART_INTERFACE instance  */
```

The `UART_INTERFACE` struct exposes `Read`, `Write`, `IsRxReady`, `IsTxReady`
function pointers. If your driver uses a different API (direct function calls
rather than a struct), replace the calls in the four callback bodies accordingly:

```c
// Direct function call alternative (e.g. older MCC Classic drivers):
void sendSerial(uint8_t data)             { UART1_Write(data); }
uint8_t receiveSerial(void)               { return UART1_Read(); }
uint8_t isReceiveDataAvailable(void)      { return (uint8_t)UART1_IsRxReady(); }
uint8_t isSendReady(void)                 { return (uint8_t)UART1_IsTxReady(); }
```

## Application Integration

### 1. Copy files

Copy the following four files into your project's `X2Cscope/` folder:
```
X2CscopeComm.h
X2CscopeComm.c
X2Cscope.h      (from interface/)
X2Cscope.c      (from interface/)
```

### 2. Add to build system

Add `X2CscopeComm.c` and `X2Cscope.c` to your MPLAB X project or CMakeLists.txt.
Add the `X2Cscope/` folder to your compiler include paths.
Link `libx2cscope-<target>.a` from the `dist/` folder.

### 3. Initialise

Call `X2Cscope_Init()` once after all peripherals are initialised:

```c
#include "X2Cscope/X2Cscope.h"

int main(void) {
    SYSTEM_Initialize();    /* MCC-generated system init */
    X2Cscope_Init();        /* start X2Cscope            */
    ...
}
```

### 4. Main loop — communicate

Call `X2Cscope_Communicate()` from your main/idle loop:

```c
while (1) {
    X2Cscope_Communicate();
    /* your application code */
}
```

### 5. Fixed-rate task — update scope

Call `X2Cscope_Update()` from a timer ISR or a fixed-rate task. The period
determines the maximum scope sample rate visible in the X2Cscope PC tool:

```c
void TMR1_TimeoutCallback(void) {
    X2Cscope_Update();
}
```

## Connecting with pyX2Cscope

[pyX2Cscope](https://github.com/X2Cscope/pyx2cscope) is the recommended PC-side tool.
Install it with pip (Python 3.10–3.14):

```
pip install pyx2cscope
```

### Qt GUI

```
pyx2cscope
```

1. Select **Serial** as the interface.
2. Set the COM port (or leave as **AUTO** for automatic detection).
3. Set the baud rate to match your UART configuration.
4. Browse to your firmware `.elf` file.
5. Click **Connect**.

### Web GUI (browser-based)

```
pyx2cscope -w
```

Then open the displayed URL in a browser and follow the same steps as above.

### Scripting / automation

```python
from pyx2cscope.x2cscope import X2CScope

x2c = X2CScope(port="AUTO", elf_file="path/to/firmware.elf")
my_var = x2c.get_variable("my_variable_name")
print(my_var.get_value())
my_var.set_value(42)
```

For the latest release and standalone binaries visit:
https://github.com/X2Cscope/pyx2cscope/releases
