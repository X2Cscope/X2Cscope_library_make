# Online Scope Mode (OSM) – How It Works and Why It Is Broken

Background notes for issue #13 and `doc/scope_streaming.md`. OSM is the existing, non-functional live mode of the scope. Scope streaming is meant to replace it.

## Idea

OSM is a **live view without a buffer**. Instead of filling `X2CscopeArray` and letting the host read it later, the device sends the **current values** of the scope channels in a small frame at the configured sample rate, without being asked by the host.

## 1. Start

The host writes the scope configuration with Save Parameter (0x12), as for an offline capture, but with one of these states:

| State | Meaning |
|---|---|
| 4 `SCOPE_SAMPLE_ONLINE` | Send immediately |
| 5 `SCOPE_WAITTRGPOS_ONLINE` / 6 `SCOPE_WAITTRGNEG_ONLINE` | Wait for the trigger, then switch to state 4 (`Scope_Main.c:150-161`) |

`Scope_Main_Save` computes the OSM frame size: `txFrameSize = 2` (timestamp) plus the data size of every channel (`Scope_Main.c:381-382, 445-447`).

Service 0x1A `SVOSMCONTROL` is defined (`Services.h:66`), but no function is registered for it. Start and stop happen only through the scope state.

## 2. Interrupt side – `X2Cscope_Update()`

In state 4 it **does not sample anything**. It only counts (`Scope_Main.c:128-131`):

```c
case SCOPE_SAMPLE_ONLINE:
    pTScope->timestamp++;   // tick counter, sent with every frame
    pTScope->stfCnt++;      // prescaler counter
```

## 3. Main loop side – `X2Cscope_Communicate()`

After the normal RX/TX handling, `communicate()` always calls `LNet_OnlineScope()` (`LNet.c:256`). A frame is sent only if all of these are true (`LNet.c:284-291`):

- the scope is in state 4,
- the TX FIFO has room for the complete frame: `getTxFifoFree() >= txFrameSize + 4`,
- the prescaler has expired: `stfCnt >= stf`.

If so, it resets `stfCnt = 0` and pushes the whole frame with direct `send()` calls, without `isSendReady()`. The FIFO check above is what makes that safe. Otherwise it only increments the debug counter `txBufFull`.

## 4. OSM frame (`LNet.c:295-339`)

```
0x02 | SIZE = txFrameSize | NODE | timestamp LSB | timestamp MSB | ch0 bytes … chN-1 bytes | CHK
```

- **SYN = `0x02`**: the value the LNet document marks as "reserved for future purposes". The host can tell OSM frames (`0x02`) from response frames (`0x55`) by the first byte.
- **Timestamp**: the `X2Cscope_Update()` tick count at send time. The host places the values on the time axis with it; gaps in the timestamp show skipped samples.
- **Channel values**: read at send time in the main loop, directly from the variable addresses. They are not the values from the sample instant.
- **Checksum**: additive sum, like a standard frame, inverted only if it equals `0x02`.

## 5. What it was designed for

The LNet document's examples use a TI TMS320F28035. On a DSP whose UART has a hardware TX FIFO (16 bytes on C2000), the design makes sense:

- one small frame per sample period (e.g. 2 channels × 16 bit = 10 bytes),
- pushed into the FIFO in one go; the hardware sends it while the CPU continues,
- no RAM buffer needed, and "send only if the FIFO has room" provides flow control by skipping frames.

The FIFO limit also explains why frames had to be tiny.

## 6. Why it cannot work in this library

1. **It never sends.** `getTxFifoFree` is the stub in `SerialGeneric.c:88-91` and always returns 0. `initSerial()` does not replace it, and `X2Cscope_Config_t` has no field for it. The FIFO condition is never true; only `txBufFull` counts up.
2. **Jitter and torn values.** The variables are read in the main loop, not in `X2Cscope_Update()`. The timing depends on the main-loop load, and a multi-byte variable can change halfway through being read by the interrupt.
3. **It can corrupt normal responses.** `LNet_OnlineScope()` runs regardless of `ucCommState`, so it could put an OSM frame in the middle of a standard response that is still being sent.
4. **No fill bytes.** It violates LNet §3.6: a data byte `0x55` or `0x02` inside an OSM frame looks like the start of a new frame to the host.
5. **Prescaler race.** `stfCnt` is incremented in the interrupt and reset in the main loop; that read-modify-write from two contexts can lose updates.
6. **Does not fit buffered transports.** OSM frames are never flushed, so the TCP and CAN examples (which collect bytes until `flush()`) would hold them back. Requiring a hardware FIFO does not fit TCP, CAN or DMA anyway.
7. **Untested active code path.** All current library targets build with `__DATA_WIDTH_8BIT__`, and that branch is marked `/* procedure not tested yet */` (`LNet.c:319`).
8. **No host support.** mchplnet and pyX2Cscope never handle SYN `0x02` and do not check the SYN byte at all. An OSM frame would be taken as the response to whatever request is pending.

## 7. What scope streaming keeps and changes

Scope streaming (`doc/scope_streaming.md`, draft 2) is OSM done right.

**Kept:**
- The device pushes frames with SYN `0x02` without being asked.
- A tick timestamp in every frame, so the host places samples on the time axis and sees gaps.

**Changed:**
- **Sampling** in `X2Cscope_Update()` at the exact sample time, into a small ring at the end of `X2CscopeArray`. The values are no longer read later in the main loop.
- **The offline capture keeps running** at the same time, each with its own prescaler.
- **Several samples per frame** when available, sent directly from the ring.
- **Frames follow LNet §3.6** (fill bytes), and are flushed.
- **TX scheduling:** a stream frame never interrupts a response, and requests are received while a stream frame is being sent.
- **Started explicitly by the host** (Stream Control 0x16), point-to-point links only.
- **A timestamp of 32 bits** instead of 16, plus a lost-sample counter and an optional automatic prescaler.

## 8. Related findings on the host side (mchplnet / pyX2Cscope)

Found while analysing the host for streaming. These are separate issues for those repositories:

- **Watch view does not use OSM.** It reads variables with Get RAM Block (0x09) and writes them with Put RAM Block (0x0A), polled by the host: Qt live variables every 500 ms (`gui/qt/workers/data_poller.py:56`), web every 1 s (`gui/web/scope.py:20`).
- **The "continuous" scope re-arms captures.** It re-arms an offline capture every 250 ms (Qt) or 100 ms (web), so samples between captures are lost (`data_poller.py:181-200`). Streaming replaces this.
- **Disabled channels corrupt the scope configuration.** It sends `len(channels)` as the channel count but skips disabled channels, so the count and entries disagree (`mchplnet/services/scope.py:212-219`).
- **Trigger edge mapping is inconsistent.** Rising is 1 in the Qt scope tab and the web form, but 0 in `AppState.configure_scope_trigger` (`app_state.py:639-641`).
- **Bad responses are not rejected.** `LNetFrame.deserialize()` only logs a bad CRC or a service mismatch and parses the frame anyway (`lnetframe.py:178-186`).
- **No SYN check** in any transport, so a stray or partial frame silently desynchronises communication.
- **Serial timeout raises `TypeError` instead of `TimeoutError`,** so the retry in `_xchg_data` never runs (`uart.py:225-250`).
- **Load Parameter (scope block) drops the last byte.** `_deserialize` slices `received[5:-1]` after `frame_integrity()` has already removed the CRC, so the scope-version byte is probably cut. This matters for streaming detection (`SCOPE_VERSION` 3).
- **`mchplnet/doc/LNet.Doc_v6.pdf`** documents LNet v6 (incompatible framing). Firmware and mchplnet implement v5.
