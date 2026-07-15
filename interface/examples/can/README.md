# X2Cscope — CAN Communication Example

This example implements the X2Cscope communication layer over a CAN 2.0
peripheral using an MCC Melody-generated CAN driver.

## Files

| File | Purpose |
|---|---|
| `X2CscopeComm.h` | Comm layer header — copy into your project's X2Cscope folder |
| `X2CscopeComm.c` | Comm layer implementation — copy into your project's X2Cscope folder |
| `../X2Cscope.c` | Scope buffer allocation and `X2Cscope_Init()` — copy from `interface/` |
| `../X2Cscope.h` | Public X2Cscope API — copy from `interface/` |

## Protocol Overview

X2Cscope uses a byte-stream protocol (LNet). CAN carries fixed-size frames,
so the byte stream is packetized:

- **TX:** `sendSerial()` accumulates bytes into an 8-byte buffer. A full CAN
  frame is transmitted automatically every 8 bytes. `flushSerial()` sends any
  remaining bytes as a shorter frame at the end of each LNet frame.
- **RX:** `X2CscopeComm_Communicate()` drains incoming CAN frames from the
  hardware FIFO into a 32-byte software ring buffer. `receiveSerial()` pops one
  byte at a time from that buffer when the library is ready to process it.

> **Important:** Call `X2CscopeComm_Communicate()` from your main loop instead
> of `X2Cscope_Communicate()` directly. The CAN wrapper drains the hardware RX
> FIFO before running the protocol engine.

## CAN Message IDs

| Direction | ID (default) | Description |
|---|---|---|
| Device → PC | `0x100` | Frames sent by the firmware (RX on X2Cscope PC tool) |
| PC → Device | `0x110` | Frames sent by the PC tool (TX on X2Cscope PC tool) |

Change `X2CSCOPE_CAN_TX_ID` and `X2CSCOPE_CAN_RX_ID` in `X2CscopeComm.c` to
match your network. Configure the CAN peripheral RX filter/mask in MCC to
accept frames with your chosen RX ID.

## Hardware Requirements

- dsPIC33C, dsPIC33A, or compatible device with CAN peripheral
- CAN transceiver (e.g. ATA6563) connected to the CAN bus
- PC with a USB-CAN adapter (e.g. PEAK PCAN-USB, Microchip ATA6563-XPRO)
- X2Cscope PC tool with CAN support

## MCC Configuration

### CAN peripheral (MCC Melody)

1. Add **CAN1** in MCC Melody.
2. Set the bit rate to match the X2Cscope PC tool (typical: **500 kbps** or **1 Mbps**).
3. Configure one **TX FIFO** (TX Queue, `CAN1_TXQ`).
4. Configure one **RX FIFO** with a filter/mask accepting `X2CSCOPE_CAN_RX_ID`
   (default `0x110`, standard frame, data frame).
5. Interrupts are not required — the example uses polling.
6. Generate the driver. The generated files include:
   - `can1.h` / `can1.c`
   - `can_types.h` — `CAN_MSG_OBJ`, `CAN_TX_FIFO_STATUS`, `CAN_MSG_FIELD`

### Adapting `X2CscopeComm.c`

Open `X2CscopeComm.c` and update the two lines marked **Adapt**:

```c
#include "can/can1.h"       /* path to your MCC-generated CAN driver */
#include "can/can_types.h"  /* CAN_MSG_OBJ, CAN_TX_FIFO_STATUS, etc. */

#define X2CSCOPE_CAN_TX_ID  (0x100U)    /* device → PC */
#define X2CSCOPE_CAN_RX_ID  (0x110U)    /* PC → device */
```

If your project uses **CAN FD** hardware but communicates in classical CAN 2.0
mode (as is typical for X2Cscope), no changes to the framing logic are needed —
the example already sets `CAN_2_0_FORMAT` and `CAN_NON_BRS_MODE` explicitly.

## Application Integration

### 1. Copy files

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

```c
#include "X2Cscope/X2Cscope.h"

int main(void) {
    SYSTEM_Initialize();
    X2Cscope_Init();        /* hooks CAN callbacks, starts TCP server */
    ...
}
```

### 4. Main loop — communicate

Call `X2CscopeComm_Communicate()` (**not** `X2Cscope_Communicate()`) from your
main/idle loop:

```c
while (1) {
    X2CscopeComm_Communicate();   /* drains CAN RX FIFO + runs protocol */
    /* your application code */
}
```

### 5. Fixed-rate task — update scope

```c
void TMR1_TimeoutCallback(void) {
    X2Cscope_Update();
}
```

## Connecting with pyX2Cscope

[pyX2Cscope](https://github.com/X2Cscope/pyx2cscope) v0.7.0 and later supports CAN.
Install it with pip (Python 3.10–3.14):

```
pip install pyx2cscope
```

### Qt GUI

```
pyx2cscope
```

1. Select **CAN** as the interface.
2. Select your USB-CAN adapter from the list.
3. Set the bit rate to match your CAN peripheral configuration.
4. Set the **TX ID** to `0x110` (frames sent from PC to device).
5. Set the **RX ID** to `0x100` (frames sent from device to PC).
6. Browse to your firmware `.elf` file.
7. Click **Connect**.

### Web GUI (browser-based)

```
pyx2cscope -w
```

Then open the displayed URL in a browser and follow the same steps as above.

### Scripting / automation

```python
from pyx2cscope.x2cscope import X2CScope
from pyx2cscope.interface import InterfaceType

x2c = X2CScope(
    elf_file="path/to/firmware.elf",
    interface=InterfaceType.CAN,
    # adapter and bit-rate options depend on your USB-CAN adapter
)
my_var = x2c.get_variable("my_variable_name")
print(my_var.get_value())
```

For the latest release and standalone binaries visit:
https://github.com/X2Cscope/pyx2cscope/releases
