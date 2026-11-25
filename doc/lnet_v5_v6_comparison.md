# LNet v5 vs. v6 – Comparison and Migration Effort

Sources:
- *LNet Documentation*, LCM, March 3 2017 (LNet version 5, document version 5)
- *LNet Documentation*, LCM, June 28 2023 (LNet version 6, document version 9)

The LCM documents are not published in this repository (see [`README.md`](README.md)).
- What X2Cscope implements on top of v5: [`lnet/x2cscope_lnet_profile.md`](lnet/x2cscope_lnet_profile.md)

Current implementations:
- **Firmware (this repo): LNet v5.** `DEVINFO_MONITOR_VERSION 0x0005` (`Services.c:45`), frame order `SYN SIZE NODE` (`LNet.h:50`), 8-bit SIZE, additive checksum.
- **mchplnet / pyX2Cscope: LNet v5.** 1-byte SIZE, additive checksum. The v6 PDF is only bundled as a document.

## 1 Summary

- **v6 adds 3 services:** `0x13` Load I/O Parameter, `0x14` Load Mask Parameter, `0x15` Save Mask Parameter. All three access X2C *model* tables (Inport, Outport, Mask Parameter blocks). The X2Cscope library has none of these tables, so for X2Cscope they would only answer "Service not available" and bring **no new function**.
- **What actually matters in v6 is the frame format:** 16-bit SIZE (frames up to 65535 data bytes), CRC-8 instead of a byte sum, and NODE moved before SIZE. It is **not wire-compatible** with v5. A v6 device cannot talk to a v5 host, and the reverse is also true.
- **The main benefit for X2Cscope is large frames:** reading the offline scope buffer in a few round trips instead of 20. With push streaming (issue #13, `doc/scope_streaming.md` draft 2) there is no round trip per stream frame, so for streaming v6 only reduces the per-frame overhead (≈ 6 % with 255-byte frames).
- **Rough effort:** 3–4 weeks for firmware, mchplnet, pyX2Cscope and tests, if both versions must be supported at runtime so the X2Cscope GUI keeps working.

## 2 Frame format

| Item | v5 | v6 | Impact |
|---|---|---|---|
| Layout | `SYN SIZE NODE DATA CRC` | `SYN NODE SIZE DATA CRC` | Parser state machine on both sides |
| SYN | `0x55` (`0x02` reserved) | same | – |
| NODE | 1 byte | 1 byte, moved before SIZE ("to be able to return size errors correctly") | – |
| SIZE | 1 byte, max 255 data bytes | **2 bytes LSB first**, max 65535 data bytes | All size variables and buffers |
| Max service data | request 254, response 253 | request 65534, response 65533 | – |
| Checksum | sum of all frame bytes mod 256 | **CRC-8**, polynomial `0x07`, start value `0xFF`, over all frame bytes | New checksum routine on both sides |
| Checksum inversion | `0x55 → 0xAA`, `0x02 → 0xFD` | same | – |
| Fill bytes | `0x00` after `0x55`/`0x02` in SIZE, NODE, DATA; not counted, not in checksum | same (both SIZE bytes are checked) | – |
| "Timeout option" | – | listed in the v6 history, but **not described anywhere** in the document | Must be clarified with LCM |

## 3 Services

| ID | Service | v5 | v6 | Firmware today | Change in v6 |
|---|---|---|---|---|---|
| 0x00 | Get Device Info | ✓ | ✓ | ✓ | Max frame size 1 → 2 bytes; all later fields shift by +1 (section 4.1) |
| 0x01 | Get Target State | ✓ | ✓ | ✓ | – |
| 0x02 | Set Target State | ✓ | ✓ | ✓ | – |
| 0x04 | Erase Flash | ✓ | ✓ | – | – |
| 0x07 | Get Block Data | ✓ | ✓ | ✓ | – (responses may be larger) |
| 0x08 | Put Block Data | ✓ | ✓ | ✓ | – (requests may be larger) |
| 0x09 | Get RAM Block | ✓ | ✓ | ✓ | "Number of bytes to read" 1 → **2 bytes** |
| 0x0A | Put RAM Block | ✓ | ✓ | ✓ | – (data length follows from the frame size) |
| 0x0B | Get Flash Block | ✓ | ✓ | – | "Number of bytes" 1 → 2 bytes |
| 0x0C | Put Flash Block | ✓ | ✓ | – | – |
| 0x11 | Load Parameter | ✓ | ✓ | ✓ | – |
| 0x12 | Save Parameter | ✓ | ✓ | ✓ | – |
| **0x13** | **Load I/O Parameter** | – | **new** | – | Reads Inport/Outport block data |
| **0x14** | **Load Mask Parameter** | – | **new** | – | Reads Mask Parameter block data |
| **0x15** | **Save Mask Parameter** | – | **new** | – | Writes Mask Parameter block data |
| 0x19 | Reboot | ✓ | ✓ | – | – |
| 0x1A | (OSM control) | – | – | defined, not registered | Not in either document (firmware-internal) |

Not used by either version: `0x03, 0x05, 0x06, 0x0D, 0x0E, 0x0F, 0x10, 0x16, 0x17, 0x18`.

> `0x0F` and `0x10` are defined in `Services.h` as `LOADEE`/`SAVEEE` but are not in either document.

### 3.1 New services in v6

**0x13 Load I/O Parameter:** reads Inport or Outport data.

| Request byte | Description |
|---|---|
| 0 … 1 | Parameter ID |
| 2 | I/O type: 0 = Inport, 1 = Outport |

| Response byte | Description |
|---|---|
| 0 … n | I/O data (length depends on the I/O size) |

**0x14 Load Mask Parameter:** reads Mask Parameter block data.

| Request byte | Description |
|---|---|
| 0 … 1 | Parameter ID |

| Response byte | Description |
|---|---|
| 0 … n | Mask Parameter block data |

**0x15 Save Mask Parameter:** writes Mask Parameter block data.

| Request byte | Description |
|---|---|
| 0 … 1 | Parameter ID |
| 2 … n | Mask Parameter block data |

Response: no service data.

**Relevance for X2Cscope:** these services belong to X2C model code (block diagrams with Inport/Outport blocks and masked parameters). `tTableStruct` in this library (`CommonFcts.h:259-278`) has only a parameter table, a block function table and a limit-save table, with no Inport, Outport or Mask Parameter tables. A v6 X2Cscope would answer them with `0x21` Service not available, which v6 explicitly allows ("some services may not be available on all target types").

### 3.2 Get Device Info layout

| Field | v5 bytes | v6 bytes |
|---|---|---|
| Monitor (v5) / Bootloader (v6) version | 0–1 | 0–1 |
| Application version | 2–3 | 2–3 |
| Maximum target frame size | **4** (1 byte) | **4–5** (2 bytes) |
| Processor identifier | 5–6 | 6–7 |
| Monitor/Bootloader date | 7–15 | 8–16 |
| Monitor/Bootloader time | 16–19 | 17–20 |
| Application date | 20–28 | 21–29 |
| Application time | 29–32 | 30–33 |
| DSP state | 33 | 34 |
| Event type | 34–35 | 35–36 |
| Event identifier | 36–39 | 37–40 |
| TableStruct address | 40–43 | 41–44 |
| Service data length | 44 | 45 |

v6 adds a note: the maximum target frame size is what the slave supports. Larger requests or responses must be answered with `0x15` (Size too large). It is not the protocol maximum.

### 3.3 Get RAM Block / Get Flash Block request

| Field | v5 | v6 |
|---|---|---|
| Memory address | 0 … n (target address width) | same |
| Number of bytes to read | n+1 (1 byte) | **n+1 … n+2 (2 bytes)** |
| Value data type | n+2 | n+3 |

## 4 Data types, DSP states, errors

**Data types:**

| Value | Width | v5 | v6 | Firmware Get/Put RAM Block |
|---|---|---|---|---|
| 0x01 | 8 bit | ✓ | ✓ | ✓ |
| 0x02 | 16 bit | ✓ | ✓ | ✓ |
| 0x04 | 32 bit | ✓ | ✓ | ✓ |
| 0x08 | 64 bit | – | **new** | – (64-bit variables are read as 8 × 8 bit today) |

**DSP states:** the same values 0–5. State 0 is renamed from MONITOR to BOOTLOADER.

**Errors:**

| ID | Description | v5 | v6 | Firmware |
|---|---|---|---|---|
| 0x00 | No error | ✓ | ✓ | ✓ |
| 0x13 | Checksum error | ✓ | ✓ | ✓ |
| 0x14 | Format error | ✓ | ✓ | ✓ |
| 0x15 | Size too large | ✓ | ✓ | ✓ |
| 0x21 | Service not available | ✓ | ✓ | ✓ |
| 0x22 | Invalid DSP state | ✓ | ✓ | ✓ |
| **0x23** | **Invalid data type** | – | new | – (uses 0x14 Format error) |
| 0x30 | Flash write error | ✓ | ✓ | – |
| 0x31 | Flash write protect error | ✓ | ✓ | – |
| **0x32** | **Flash erase error** | – | new | – |
| **0x33** | **Flash address alignment error** | – | new | – |
| **0x34** | **Flash data error** | – | new | – |
| 0x40 | Invalid Parameter ID | ✓ | ✓ | ✓ |
| 0x41 | Invalid Block ID | ✓ | ✓ | ✓ |
| 0x42 | Parameter limit error | ✓ | ✓ | ✓ |
| 0x43 | Parameter table not initialized | ✓ | ✓ | ✓ |
| **0x44** | **Function table not initialized** | – | new | ✓ (already sent today, although not in v5) |
| **0x45** | **Inport Parameter table not initialized** | – | new | – |
| **0x46** | **Outport Parameter table not initialized** | – | new | – |
| **0x47** | **Mask Parameter table not initialized** | – | new | – |
| **0x48** | **Save Mask Parameter error** | – | new | – |
| **0x49** | **Load Mask Parameter error** | – | new | – |
| 0x50 | Power-on error | ✓ | ✓ | – |
| **0xFF** | **Unknown error** | – | new | – |

The "Firmware" column marks the IDs this library's services actually send, through the `ERROR…` and `SvError…` constants in `Services.h:78-91`: 0x00, 0x13, 0x14, 0x15, 0x21, 0x22, 0x40, 0x41, 0x42, 0x43, 0x44. The header also defines 0x30, 0x31 and 0x50, which are unused. See also `doc/lnet/x2cscope_lnet_profile.md` §8.

## 5 What v6 would bring to X2Cscope

| Benefit | v5 today | v6 (example: 2048-byte frames) |
|---|---|---|
| Read the 5000-byte scope buffer | 20 round trips of 253 bytes | 3 round trips |
| Streaming efficiency, polled (draft 1, T1S/TCP with 0.5–1 ms round trip) | ~20–30 % | ~65–75 % |
| Streaming efficiency, **push** (draft 2, any link; frame overhead only) | ~94 % (15 B per 255 B frame) | ~99 % |
| Error detection | 8-bit sum (misses e.g. swapped bytes) | CRC-8 |
| Size errors | Node byte comes after SIZE, so an oversized frame is rejected before the node is known | Reported correctly |

**Cost:** the LNet buffer is shared between RX and TX and must hold one maximum frame. 255 bytes today; e.g. 2048 bytes with v6. On small devices (SAMD21, PIC32CM, dsPIC) the buffer size must be configurable, not fixed in the library.

## 6 Effort to update to v6

### 6.1 Compatibility strategy (decision needed first)

The X2Cscope GUI and all released pyX2Cscope/mchplnet versions speak v5 only.

| Option | Description | Pros | Cons |
|---|---|---|---|
| A. v6 only | Replace v5 | Simplest firmware | **Breaks the X2Cscope GUI** and every existing pyX2Cscope installation |
| B. Build-time choice | v5 or v6 per library build | Small runtime code | Doubles the archives, or forces users to choose; GUI users need the v5 build |
| C. Runtime dual stack | Firmware detects the version from the host's first valid frame and answers in the same version | No user choice; GUI and new pyX2Cscope both work | More firmware code and testing |

**Detection for option C** is unambiguous. Example Get Device Info request:
- v5: `55 01 01 00 57` (checksum `0x57` at byte 4)
- v6: `55 01 01 00 00 53` (2-byte SIZE, CRC-8 `0x53`)

The firmware validates the frame both ways, and only one checksum matches.

### 6.2 Firmware (this repo)

| Task | Effort (days) |
|---|---|
| LNet frame parser/sender: NODE before SIZE, 16-bit SIZE (2 states, both with fill-byte handling), CRC-8 (table or bitwise; see CPU note below) | 2–3 |
| Dual stack (option C): version detection and lock, both checksum paths | 2–3 |
| Widen sizes: `tProtocol.ucFRAMESize` / `ucMaxCommSize` (`uint8`, `CommonFcts.h:178-179`), `tLNet`, the frame size in every service, `getRAMBlock`/`putRAMBlock` size variables, block API `pFSave(..., uint8 ucFrameLength)` (`CommonFcts.h:136`) | 1–2 |
| Configurable LNet buffer (pointer and size in `X2Cscope_Config_t` instead of the fixed `bufferLNet[255]`): API change, templates, `MIGRATION_GUIDE.md` | 1–2 |
| Get Device Info v6 layout; Get RAM Block 16-bit size field; data type 0x08; error 0x23; monitor version 6 in v6 mode | 1 |
| Services 0x13–0x15: register as "not available" (no X2C model tables) | 0.5 |
| Transport examples: TCP TX buffer (516 bytes today) and CAN fragmentation for larger frames | 1 |
| Tests: all 18 archives, UART/TCP/CAN examples, v5 and v6 hosts | 3–5 |
| **Firmware total** | **≈ 12–18** |

**CPU note:** CRC-8 per byte costs more than the current addition. A 256-byte lookup table keeps it at one table access per byte. A bitwise loop takes 8 iterations per byte, which matters on fast links (T1S) where LNet already handles every byte through a callback.

### 6.3 Host (mchplnet / pyX2Cscope)

| Task | Effort (days) |
|---|---|
| mchplnet frame builder/parser: layout, 16-bit SIZE, CRC-8 (`lnetframe.py`) | 1–2 |
| Transports: `read()` in UART/TCP/CAN parses `SYN NODE SIZE16` (today `SYN SIZE`, no SYN check) | 1 |
| Version negotiation: try v6 Get Device Info, fall back to v5 | 0.5–1 |
| Get Device Info v6 parser; Get RAM Block 16-bit size | 0.5 |
| pyX2Cscope: replace the hard-coded 253-byte chunks (`x2cscope.py:337`, `variable/variable.py:133`) with the reported max frame size | 0.5 |
| Tests against v5 and v6 firmware | 1–2 |
| **Host total** | **≈ 5–7** |

### 6.4 Memory impact on the microcontroller

Library memory today (measured with `xc32-size` on the built archives; the user scope buffer `X2CscopeArray`, 5000 B by default, is not included):

| Target | Library flash | Library RAM | LNet code | LNet buffer |
|---|---|---|---|---|
| Cortex-M0+ (`cm0`) | 7.1 KB | 451 B | 1.1 KB | 255 B |
| Cortex-M4F (`cm4`) | 6.8 KB | 451 B | 1.0 KB | 255 B |
| PIC32MZ | 12.0 KB | 468 B | 2.0 KB | 255 B |

Estimated flash increase for v6 (not measured; Cortex-M0+ as reference):

| Change | Flash |
|---|---|
| CRC-8 | +256 B lookup table (fast), or about +40 B computed bit by bit (8 iterations per byte) |
| NODE before SIZE, 16-bit SIZE with fill-byte checks | about +50–150 B |
| 16-bit sizes in services, Get Device Info / Get RAM Block layouts | about +50 B |
| Services 0x13–0x15 | 0 B (default "not available" handler) |
| **v6 only** | **about +0.3–0.5 KB (≈ +5–7 % of the library)** |
| **v5 + v6 dual stack** | **about +0.7–1.2 KB (≈ +10–17 %)** |

**RAM:**
- v6 does **not** require a larger buffer: the maximum target frame size is set per device, so a v6 device can keep 255 bytes and RAM grows only by a few bytes for the 16-bit size fields.
- The throughput gain of v6 requires a larger buffer, because RX and TX share one buffer that must hold a full frame. For example, a 2048-byte frame means about +1.8 KB RAM. The size must therefore be user-configurable.

For comparison, streaming on v5 (issue #13): about 0.5–1 KB flash (estimate) and only about 20–30 B RAM for indices and counters, because it reuses `X2CscopeArray`.

### 6.5 Total

**≈ 17–25 working days (3–5 weeks)**, plus clarification of the undocumented v6 "Timeout option" with LCM. Option A (v6 only) saves about 3–5 days but breaks the X2Cscope GUI.

**Recommendation:** implement streaming (issue #13) on v5 first, as specified in `doc/scope_streaming.md`; it does not depend on v6. With push streaming (draft 2), v6 brings little for streaming. Consider v6 (option C) afterwards mainly if the offline buffer readout time becomes the limit. The service ID `0x16` and error `0x24` chosen for streaming are free in both versions, so streaming would move to v6 unchanged.

## 7 Firmware bugs found during this comparison (independent of v6)

1. **Division by zero in `putRAMBlock`** (`BlockServicesX2C.c:105-108`): `size % type` is evaluated before `type` is validated. A request with data type `0` divides by zero (UsageFault on Cortex-M with divide-by-zero trapping enabled; undefined behaviour otherwise).
2. **Buffer overflow in `getRAMBlock`** (`BlockServicesX2C.c:181-235`): `size % type` is not checked. With type 2 or 4 and a size that is not a multiple of it, the last loop iteration writes past `size`. Example: size 253, type 4 → writes `ucFRAMEData[254 … 257]`, but `bufferLNet[LNET_BUFFERSIZE]` has 255 bytes (indices 0–254, `X2CscopeWrapper.h:53-58`), so 3 bytes are written past the end.
3. **Syntax error in the 8-bit address path** (`BlockServicesX2C.c:191`): `(void*) ((uint8) protocol->ucFRAMEData[1];` has a missing parenthesis. It is never compiled, because no target uses `__ADDRESS_WIDTH_8BIT__`.
