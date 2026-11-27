# X2Cscope LNet Profile

**Profile revision 1** – describes the X2Cscope library v3.2 (monitor version `0x0005`).

This document defines how the X2Cscope library implements LNet. It does **not** repeat the LNet specification.
Read it together with the LCM document, which is not published in this repository (see [`../README.md`](../README.md)):

| Document | Role |
|---|---|
| *LNet Documentation*, Linz Center of Mechatronics GmbH (LCM), LNet version 5, document version 5, March 3 2017 | **Normative base.** Everything not mentioned here behaves as described there. |
| This profile | **Normative for X2Cscope.** Clarifies, restricts or extends LNet v5. If this profile and the LCM document disagree, this profile describes the actual X2Cscope behaviour. |
| *LNet Documentation*, LCM, LNet version 6, document version 9, June 28 2023 | **Informative only.** Not implemented; wire-incompatible with v5. See [`../lnet_v5_v6_comparison.md`](../lnet_v5_v6_comparison.md). |
| [`../scope_streaming.md`](../scope_streaming.md) | Normative for the scope streaming extension (draft, section 9). |

Section numbers like "v5 §3.6" refer to the LCM v5 document.

## 1 Classification

Every item in this profile is marked with one of:

| Mark | Meaning |
|---|---|
| **Conformant** | Implemented as specified in LNet v5. |
| **Clarification** | LNet v5 leaves it open ("depends on target/block type"); this profile fixes it for X2Cscope. |
| **Restriction** | A subset of what LNet v5 allows. |
| **Deviation** | Differs from LNet v5. |
| **Extension** | Not in LNet v5; added by X2Cscope. |
| **Not implemented** | Defined in LNet v5; the service answers `0x21` Service not available, which v5 §4 allows. |
| **Known issue** | Implementation defect, to be fixed; not intended behaviour. |

## 2 Summary

| Area | v5 section | Status |
|---|---|---|
| Frame format, checksum, fill bytes | §3 | Conformant (§3 below) |
| Node identifier, frame size | §3.2, §4.1 | Clarification: node 1, maximum DATA size 255 |
| Unsolicited frames with SYN `0x02` (online scope mode) | §3.1 ("reserved") | **Deviation** (legacy, non-functional) – to be replaced by the streaming extension |
| Services 0x00–0x02, 0x07–0x0A, 0x11, 0x12 | §4 | Implemented, with clarifications (§5) |
| Services 0x04, 0x0B, 0x0C | §4 | Not implemented |
| Service 0x19 Reboot | §4.13 | Implemented through an optional application hook (since v3.2, §5.5) |
| Get/Put RAM Block | §4.7, §4.8 | Restriction (data types 1/2/4); malformed requests → `0x14` (since v3.2) |
| Load/Save Parameter | §4.11, §4.12 | Clarification: parameter ID 1 = scope block, layout in §6 |
| Errors | §7 | Conformant, plus `0x44` (defined only in LNet v6) |
| Scope streaming | – | Extension (draft) |

## 3 Frame (v5 §3)

| Item | Status | X2Cscope behaviour | Code |
|---|---|---|---|
| SYN `0x55` | Conformant | Only `0x55` frames are received. | `LNet.c:164-170` |
| SIZE (1 byte), NODE, DATA, checksum | Conformant | Checksum = sum of all bytes except fill bytes, mod 256. | `LNet.c:92-142` |
| Checksum inversion | Conformant | `0x55 → 0xAA`, `0x02 → 0xFD`, on send and receive. | `LNet.c:138`, `:227` |
| Fill bytes | Conformant | `0x00` after `0x55`/`0x02` in SIZE, NODE and DATA, on send and receive. | `checkFill()`, `LNet.c:260-272` |
| In-frame SYN detection | Conformant | If a `0x55` is not followed by `0x00`, the receiver treats it as the SYN of a new frame. | `LNet.c:206-223` |
| Node identifier | Clarification | Fixed to **1**. Frames for other nodes are ignored without response. | `LNET_NODEID`, `LNet.c:179-189` |
| Maximum DATA size | Clarification | **255 bytes**, reported in Get Device Info. One buffer is shared by RX and TX. | `LNET_BUFFERSIZE` |
| Half duplex | Clarification | While a response is being sent, no bytes are received. The host must wait for the response (one outstanding request). | `LNet.c:88-157` |
| Checksum error | Clarification | Answered with a response frame carrying Error ID `0x13`. | `LNet.c:244-247` |
| Service ID > 0x1A | Clarification | Answered with `0x21`. | `LNet.c:239-242` |
| Unsolicited `0x02` frames (OSM) | **Deviation** | In scope state 4 the device sends frames starting with `0x02` without request. These frames have no fill bytes and can interrupt a response. The mode cannot send in this library (`getTxFifoFree` stub returns 0) and no host uses it. Details: [`../online_scope_mode.md`](../online_scope_mode.md). To be removed; replaced by section 9. | `LNet.c:276-347` |

## 4 Services (v5 §4)

| ID | Service | Status | Notes |
|---|---|---|---|
| 0x00 | Get Device Info | Implemented, clarified | §5.1 |
| 0x01 | Get Target State | Conformant | |
| 0x02 | Set Target State | Conformant | Invalid state → `0x22`. |
| 0x04 | Erase Flash | Not implemented | |
| 0x07 | Get Block Data | Implemented, clarified | §5.2 |
| 0x08 | Put Block Data | Implemented, clarified | §5.2 |
| 0x09 | Get RAM Block | Implemented, restricted | §5.3 |
| 0x0A | Put RAM Block | Implemented, restricted | §5.3 |
| 0x0B | Get Flash Block | Not implemented | |
| 0x0C | Put Flash Block | Not implemented | |
| 0x0F, 0x10 | (LOADEE, SAVEEE) | Not implemented | IDs defined in `Services.h`, not in LNet v5. Reserved. |
| 0x11 | Load Parameter | Implemented, clarified | §5.4, scope block §6 |
| 0x12 | Save Parameter | Implemented, clarified | §5.4, scope block §6 |
| 0x16 | Stream Control | **Extension (draft)** | §9 |
| 0x19 | Reboot | Implemented, clarified (since v3.2) | §5.5 |
| 0x1A | (OSM control) | Not implemented | X2Cscope-internal ID, never registered. Reserved; to be removed with OSM. |

Registration: `addCoreServices()` (`Services.c:567`), `addBlockServices()` and `addExtendedBlockServices()` (`BlockServicesX2C.c:50-68`). All other IDs up to `MAX_SERVICE_ID` (0x1A) answer `0x21`.

## 5 Service details

### 5.1 Get Device Info (v5 §4.1)

Response service data, as sent by `getDeviceInfo()` (`Services.c:268-373`):

| Byte | Field | X2Cscope value | Status |
|---|---|---|---|
| 0–1 | Monitor version | `0x0005` = LNet version implemented | Clarification |
| 2–3 | Application version | `X2Cscope_Config_t.appVersion` (template default `X2CSCOPE_APP_VERSION` = `0x0001`; `0x1000`–`0x3FFF` reserved) | Clarification |
| 4 | Maximum target frame size | `255` | Clarification |
| 5–6 | Processor identifier | Appendix A | Clarification |
| 7–15 | Monitor date | **Build date of the library**, ASCII `MmmDDYYYY` without spaces (from `__DATE__`) | Clarification |
| 16–19 | Monitor time | Build time of the library, ASCII `HHMM` | Clarification |
| 20–28 | Application date | `X2Cscope_Config_t.compilationDate`, same format; 13 × `'-'` (bytes 20–32) if not set | Clarification |
| 29–32 | Application time | Same source, ASCII `HHMM` | Clarification |
| 33 | DSP state | `TableStruct->DSPState` | Conformant |
| 34–35 | Event type | `TableStruct->eventType` (0 unless set by the application) | Conformant |
| 36–39 | Event identifier | `TableStruct->eventId` (0 unless set by the application) | Conformant |
| 40–43 | TableStruct address | 32-bit, also on targets with 16-bit addresses | Clarification |

If the maximum frame size were below 46, the service would answer `0x15`. This cannot happen with the fixed 255-byte buffer.

### 5.2 Get/Put Block Data (v5 §4.5, §4.6)

- **Block address:** 2 bytes on targets with 16-bit addresses, 4 bytes otherwise (Appendix B).
- **Block identification:** the first 16-bit word at the block address is the block ID. It is looked up in the block function table; if not found, the answer is `0x41`.
- **Block data:** produced and consumed by the block's load/save functions. The X2Cscope library contains one block type, the scope block (ID `0xFFFF`), see §6. If a block's save function rejects the data, the answer is `0x14`.

### 5.3 Get/Put RAM Block (v5 §4.7, §4.8)

| Item | Status | X2Cscope behaviour |
|---|---|---|
| Memory address size | Clarification | 2 bytes on targets with 16-bit addresses, 4 bytes otherwise (Appendix B). |
| Number of bytes (Get) | Clarification | 1 byte, at most 253 (frame size − Service ID − Error ID); larger → `0x15`. |
| Data types | **Restriction** | `0x01`, `0x02`, `0x04`. Any other value (including `0x08`, 64-bit) → `0x14` Format error. 64-bit variables can be read as 8 × `0x01`. |
| Byte order | Conformant | Values LSB first. |
| Put: data length | Clarification | Frame size − header; must be a multiple of the data type, else `0x14`. Length 0 is accepted (nothing written). |
| Get: size alignment | Clarification | The number of bytes must be a multiple of the data type, else `0x14`. |
| Request length | Clarification | Get needs Service ID, address, number of bytes and data type; Put needs Service ID, address and data type. Shorter requests → `0x14`. |
| Validation order | Clarification | Request length, then data type, then alignment, then (Get) size limit `0x15`. Nothing is read from or written to target memory for a rejected request. |

**Library v3.1 and earlier** did not validate these requests (issue #27, fixed in v3.2):
- Get could write up to 3 bytes past the 255-byte LNet buffer.
- Put divided by zero for data type `0x00`.
- A Put request without type byte wrote up to 255 stale bytes to the target address.

Hosts must not rely on the old behaviour. The validation is covered by `tests/host/test_ram_block_services.c`.

### 5.4 Load/Save Parameter (v5 §4.11, §4.12)

- **Parameter table:** exactly one entry, **parameter ID 1 = scope block** (`X2C.h:48`).
- **Save Parameter:** if the limit-save table contains the parameter ID, its limit function is used, and failure gives `0x42`. Otherwise the block's save function is used, and failure gives `0x14`. X2Cscope has no limit-save table.
- **Errors:**

| Error | Condition |
|---|---|
| `0x43` | Parameter table not initialised |
| `0x40` | Parameter ID not in the table |
| `0x44` | Function table not initialised. **Not defined in LNet v5**; LNet v6 defines it with the same meaning. |
| `0x41` | Block ID not in the function table |
| `0x42` | Limit-save function failed |
| `0x14` | Block save function rejected the data |

### 5.5 Reboot (v5 §4.13) – since v3.2

The library does not reset the device itself. It calls the application function `bool X2CscopeComm_ResetDevice(void)` (`interface/X2CscopeComm.h`), which resets the device. The library contains a weak default, so implementing the function is optional.

| Situation | Answer |
|---|---|
| Application implements the function, returns `true` | **No response frame**, as LNet v5 requires on success. The device resets now or shortly afterwards. |
| Application returns `false` (the device cannot be reset now) | `0x22` Invalid DSP state (**Clarification**: LNet v5 does not define a refusal) |
| Application does not implement the function | `0x21` Service not available (as before v3.2) |
| Request with service data | `0x14` Format error |

After a service without response, the receiver is ready for the next frame (`LNet.c`). A host must not resend Reboot on a timeout: no answer means success. mchplnet's `LNet.reset_device()` behaves accordingly.

## 6 Scope block (parameter ID 1, block ID `0xFFFF`) – Clarification

LNet v5 leaves block data "depending on block type". This section defines the X2Cscope scope block.

### 6.1 Load Parameter response (`Scope_Main_Load`, `Scope_Main.c:279-316`), 29 bytes

| Byte | Size | Field |
|---|---|---|
| 0 | 1 | Scope state (§6.3) |
| 1 | 1 | Number of channels |
| 2–3 | 2 | Sample time prescaler (0 = every `X2Cscope_Update()` call) |
| 4–7 | 4 | Offline pointer: index of the next free byte in the scope buffer |
| 8–11 | 4 | Scope buffer (`X2CscopeArray`) address |
| 12–15 | 4 | Trigger delay (int32, buffer steps) |
| 16–19 | 4 | Trigger event position (buffer index) |
| 20–23 | 4 | Used length of the scope buffer |
| 24–27 | 4 | Scope buffer size |
| 28 | 1 | `0x80` + scope version (currently `0x82`, version 2) |

### 6.2 Save Parameter request block data (`Scope_Main_Save`, `Scope_Main.c:321-598`)

| Field | Size | Notes |
|---|---|---|
| Scope state | 1 | Applied last, after all other fields (§6.3) |
| Number of channels *n* | 1 | 1 … 8; 0 or > 8 → `0x14` |
| Sample time prescaler | 2 | |
| Channel 1 … *n* | 6 each | Source type (1), source (4), data size in bytes (1) |
| Trigger data type | 1 | Bit 7 = new format, bit 6 = float, bit 5 = signed, bits 0–3 = size (1/2/4/8) |
| Trigger source type | 1 | |
| Trigger source | 4 | |
| Trigger level | size, or 4 | New format: *size* bytes. Old format (bit 7 = 0): 4 bytes, treated as signed. |
| Trigger delay | 4 | int32, in buffer steps (negative = pre-trigger) |
| Trigger edge | 1 | 1 = rising, 0 = falling |
| Trigger mode | 1, optional | 0 = auto, 1 = normal. If omitted, the current trigger value is taken as the last value. |

**Source types (channel and trigger):**

| Value | Source | The 4 source bytes contain |
|---|---|---|
| 0 | Address | Variable address, 32-bit LSB first (also on 16-bit targets) |
| 1 | Control block element | Element ID (2) + parameter ID (2) |
| 2 | Inport block | (2 unused) + parameter ID (2) |
| 3 | Outport block | (2 unused) + parameter ID (2) |

Address sources (0) are what X2Cscope hosts use; types 1–3 belong to X2C model code.

### 6.3 Scope states

| Value | State |
|---|---|
| 0 | Idle |
| 1 | Wait for trigger (offline) |
| 2 | Sampling (offline) |
| 3 | *Reserved for streaming (extension, §9)* |
| 4 | Online sampling (OSM) – **Deviation**, to be removed |
| 5, 6 | Wait for trigger, positive/negative edge (OSM) – **Deviation**, to be removed |
| 7 | Trigger reached, sampling (offline) |
| 8 | Wait for negative trigger delay (offline) |

## 7 Data types, DSP states (v5 §5, §6)

- **Data types:** `0x01`/`0x02`/`0x04` in Get/Put RAM Block (§5.3). Scope channels and trigger accept 1, 2, 4 and 8 bytes. `0x08` exists in `Services.h` (`DATATYPE_64BIT`) but is not accepted by the RAM services.
- **DSP states:** conformant. The library sets nothing by itself; the application or host sets the state.

## 8 Errors (v5 §7)

Error IDs the library sends (verified in `X2CScope/src`):

| ID | Meaning | In LNet v5 |
|---|---|---|
| 0x00 | No error | yes |
| 0x13 | Checksum error | yes |
| 0x14 | Format error | yes |
| 0x15 | Size too large | yes |
| 0x21 | Service not available | yes |
| 0x22 | Invalid DSP state (Set Target State; Reboot refused by the application since v3.2) | yes |
| 0x40 | Invalid parameter ID | yes |
| 0x41 | Invalid block ID | yes |
| 0x42 | Parameter limit error | yes |
| 0x43 | Parameter table not initialised | yes |
| 0x44 | Function table not initialised | **no** (LNet v6) |
| 0x24 | Scope busy (streaming extension, draft) | no (free in v5 and v6) |

## 9 Extensions

### 9.1 Scope streaming (draft) – Extension

Specified in [`../scope_streaming.md`](../scope_streaming.md):
- **Service `0x16`** Stream Control.
- **Stream frames with SYN `0x02`**, the value v5 §3.1 reserves "for future purposes". They follow all v5 frame rules (fill bytes, checksum) and are sent only after the host enabled streaming, on point-to-point links.
- **Error `0x24`** Scope busy.
- **Scope state `3`.** Scope version `3` (Load Parameter byte 28 = `0x83`) signals support.

Service `0x16` and error `0x24` are unused in LNet v5 and v6, so the extension stays valid if X2Cscope moves to v6 framing.

### 9.2 Rules for future extensions

1. Use the frame format of LNet v5 unchanged. Do not raise the monitor version above 5 unless the LNet v6 framing is implemented.
2. Use service and error IDs that are unused in both LNet v5 and v6:
   - Services: `0x03`, `0x05`, `0x06`, `0x0D`, `0x0E`, `0x17`, `0x18` (`0x16` taken by streaming).
   - Errors: e.g. `0x24`–`0x2F`.
3. Signal new capabilities through the scope version (Load Parameter byte 28) or a probe that old firmware answers with `0x21`.
4. Document every extension here and in its own spec in `doc/`.

## 10 Open points

- **Processor ID of the Cortex libraries:** they report `0x8220` (generic PIC32) because they are built with `__GENERIC_MICROCHIP_PIC32__`. The library already defines `0x8310` (generic ARMv7) and `0x8320` (generic ARMv6). Changing it needs host support first (pyX2Cscope uses the processor ID for the address width).
- **Removal of OSM** (§3, §6.3) with the streaming extension.

## Appendix A – Processor identifiers (Microchip targets of this library)

| ID | Define | Library targets |
|---|---|---|
| `0x8210` | `__GENERIC_MICROCHIP_DSPIC__` | `generic-16dsp*.mk`: dsPIC30, dsPIC33F/E/C |
| `0x8220` | `__GENERIC_MICROCHIP_PIC32__` | `pic32m*.mk`: PIC32MX/MZ/MK/MM, and `cortex-*.mk`: all Cortex-M libraries (see §10) |
| `0x8230` | `__GENERIC_MICROCHIP_PIC24__` | `generic-16bit*.mk`: PIC24F/E, PIC24F DA |
| `0x8240` | `X2C_GENERIC_MICROCHIP_DSPIC33A` | `generic-32dsp-ak*.mk`: dsPIC33A |
| `0x8310` | `__GENERIC_ARM_ARMV7__` | defined, not used by the current builds |
| `0x8320` | `__GENERIC_ARM_ARMV6__` | defined, not used by the current builds |
| `0x8410` | `__GENERIC_X86__` | defined (PC build) |

The complete list, including other vendors' devices, is in `Services.c:47-227`.

## Appendix B – Address width per target

| Address width | Targets | Affects |
|---|---|---|
| 16-bit (2 bytes) | dsPIC30, dsPIC33F/E/C, PIC24 (`__GENERIC_MICROCHIP_DSPIC__`, `__GENERIC_MICROCHIP_PIC24__`) | Get/Put RAM Block and Get/Put Block Data addresses |
| 32-bit (4 bytes) | PIC32, dsPIC33A, Cortex-M | same |

Source: `Target.h:47-55`.

Scope channel and trigger addresses (§6.2) and the TableStruct address (§5.1) are always 4 bytes.

## Revision history

| Profile revision | Library | Changes |
|---|---|---|
| 1 | v3.2 | First version: conformance to LNet v5, scope block definition, Get/Put RAM Block validation (#27), Reboot (0x19) through the optional application hook `X2CscopeComm_ResetDevice()` (#29, #31), streaming extension (draft) |
