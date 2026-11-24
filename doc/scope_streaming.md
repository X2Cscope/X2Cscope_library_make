# X2Cscope Scope Streaming – LNet Extension (Draft 2, push mode)

Status: **draft** for issue #13. Firmware and host (mchplnet / pyX2Cscope) are implemented against this document.
Draft 2 replaces the polled design of draft 1.

## 1 Overview

- **The offline capture and streaming run at the same time:**
  - The **offline capture** (trigger, single shot) works as today, typically on all scope channels at full speed (e.g. prescaler 0, one sample every 50 µs at 20 kHz).
  - **Streaming** sends a **subset of the same scope channels** (bitmask) continuously at its **own prescaler** (e.g. 1, 2 … 100).
- **`X2CscopeArray` is split:** the offline region is at the start and a small **stream ring** at the end. The ring holds a configurable number of stream samples.
- **`X2Cscope_Update()` fills both**, each with its own prescaler. The values are sampled at the exact sample time.
- **The device pushes stream frames** (SYN `0x02`, the value reserved by LNet "for future purposes") as soon as samples are available. The host does not poll.
- **Get/Put RAM and all other services keep working** while streaming (section 8, TX scheduling).
- **The stream prescaler** is set by the host, or the device increases it automatically when samples are lost.
- **Push mode is for point-to-point links only** (UART, TCP/T1S, CAN with a single device), and only after the host enabled it. Hosts that never send the start command never see a `0x02` frame.
- **Host support:** pyX2Cscope (mchplnet) only. The X2Cscope GUI keeps using the offline capture unchanged.
- **Streaming replaces the online scope mode (OSM)** (scope states 4–6, `LNet_OnlineScope`), which is not functional. See `doc/online_scope_mode.md`.

## 2 Compatibility and versioning

| Item | Value |
|---|---|
| LNet framing | **Version 5, unchanged** (`SYN SIZE(1) NODE DATA CHK`, additive checksum, fill bytes per LNet v5 §3.6). Stream frames use the same rules with SYN `0x02` |
| Monitor version (Get Device Info bytes 0–1) | stays `0x0005` |
| New service | `0x16` Stream Control |
| New error | `0x24` Scope busy |
| Scope version (Load Parameter, scope block, last byte) | `SCOPE_VERSION` 2 → **3** (byte value `0x83`) |

- The monitor version is **not** raised to 6. LNet v6 (LCM 2023) has an incompatible frame layout; see `doc/lnet_v5_v6_comparison.md`. Service `0x16` and error `0x24` are unused in both v5 and v6.
- **Old hosts are not affected:** without a Stream Control start, the firmware behaves exactly as today.
- While a stream is configured, Load Parameter reports the **offline region** as the buffer size (section 4), so an old host reads only offline data.

**Feature detection by the host:**
1. Load Parameter (0x11), parameter ID 1: the last byte ≥ `0x83` means streaming is supported.
2. Alternatively, Stream Control "status" (section 6). Firmware without streaming answers `0x21` (Service not available).

## 3 Terms

| Term | Meaning |
|---|---|
| Tick | One call of `X2Cscope_Update()` |
| Tick counter | 32-bit count of ticks since the stream was started |
| Stream channels | Scope channels selected by the stream bitmask (bit *i* = scope channel *i*) |
| Stream sample | One value of every stream channel, in channel order, native byte order (LSB first on all supported targets) |
| `streamSampleSize` | Sum of the data sizes of the stream channels (1 … 64 bytes) |
| Stream prescaler *p* | A stream sample is taken every *p* + 1 ticks |
| Stream ring | `ringSamples` × `streamSampleSize` bytes at the end of `X2CscopeArray` |
| Lost samples | Stream samples that were due but not stored because the ring was full (section 8.3) |

## 4 Memory layout

```
X2CscopeArray, scopeSize bytes
┌──────────────────────────────────────────┬────────────────────────────┐
│ offline region                           │ stream ring                │
│ scopeSize − ringBytes                    │ ringSamples × sampleSize   │
└──────────────────────────────────────────┴────────────────────────────┘
```

- `ringBytes = ringSamples × streamSampleSize`. The offline region is `scopeSize − ringBytes`, rounded down to a multiple of 4 bytes.
- The offline capture uses only the offline region. Its maximum used length (`maxUsedLength`) is computed from the offline region.
- **Load Parameter reports the offline region size** in "array size" (bytes 24–27), and `maxUsedLength` accordingly. Existing hosts therefore read only offline data.
- **When the split changes:**
  - Stream Control **start** sets the ring. It is only allowed while the offline capture is idle (section 6).
  - Stream Control **stop** leaves the split unchanged, so an offline capture in progress is never moved or resized.
  - The next **offline configuration** (Save Parameter) while no stream is running gives the offline capture the full buffer again.

**Example:** `scopeSize` 5000 B, offline 8 channels × 4 B (32 B per sample), stream 3 channels × 4 B (12 B per sample):

| Ring samples | Ring | Offline region | Offline samples |
|---|---|---|---|
| 0 (no stream) | 0 B | 5000 B | 156 |
| 2 | 24 B | 4976 B | 155 |
| 8 | 96 B | 4904 B | 153 |
| 64 | 768 B | 4232 B | 132 |

## 5 Stream channels

- The stream channels are a **subset of the current scope channels**, configured as today with Save Parameter (0x12, parameter ID 1), selected by a bitmask in Stream Control.
- Stream Control start requires the scope channels to be configured: every bit set in the mask must refer to a channel < `noChannels`.
- **Re-arming keeps the stream running:** a Save Parameter with the **same channel list** (count, and for every channel the address and data size) does not affect the stream. pyX2Cscope re-sends the configuration for every capture.
- **A changed channel list stops the stream:** the device sends a final stream frame with flags "stopped" and "configuration changed" (section 7). The host restarts the stream if needed.

## 6 Service 0x16 – Stream Control

Service ID: `0x16`

**Request service data:**

| Data byte | Description |
|---|---|
| 0 | Command: `0x00` stop, `0x01` start, `0x02` set prescaler, `0x03` status |
| 1 … | Command parameters, see below |

| Command | Parameters |
|---|---|
| `0x00` stop | none |
| `0x01` start | byte 1: channel mask · bytes 2–3: prescaler (uint16) · bytes 4–5: ring samples (uint16, ≥ 1) · byte 6: flags (bit 0 = automatic prescaler) · bytes 7–10: keep-alive timeout in ticks (uint32, 0 = off; section 8.6) |
| `0x02` set prescaler | bytes 1–2: prescaler (uint16) |
| `0x03` status | none |

**Response service data** (the same for all commands; describes the state after the command):

| Data byte | Description |
|---|---|
| 0 | Stream state: 0 = stopped, 1 = running |
| 1 | Channel mask |
| 2 … 3 | Stream prescaler in effect (uint16) |
| 4 … 5 | Ring samples (uint16) |
| 6 | `streamSampleSize` in bytes |
| 7 … 10 | Offline region in bytes (uint32) |
| 11 | Flags: bit 0 = automatic prescaler enabled |
| 12 … 15 | Keep-alive timeout in ticks (uint32, 0 = off) |

**Behaviour:**
- **start:** allowed only while the offline capture is idle (scope state 0); otherwise error `0x24`. A running stream is restarted with the new parameters. The tick counter, lost counters and ring are reset.
- **stop:** always allowed. The device sends a final stream frame with the "stopped" flag (section 7).
- **set prescaler:** allowed at any time while running. It takes effect at the next stream sample, and the first frame with the new prescaler has the "prescaler changed" flag.
- **status:** read only.

**Errors:**

| Error ID | Meaning |
|---|---|
| 0x00 | No error |
| 0x14 | Format error: unknown command, wrong length, empty mask, mask bit ≥ `noChannels`, ring samples 0 |
| 0x15 | Size too large: the ring does not leave room for at least one offline sample |
| 0x21 | Service not available (firmware without streaming) |
| 0x24 | **Scope busy:** start while the offline capture is not idle |

## 7 Stream frame (device → host)

```
0x02 | SIZE | NODE | DATA | CHK
```

- Same rules as a standard LNet v5 frame: SIZE counts the data bytes, CHK is the sum of all frame bytes including SYN mod 256, and fill bytes `0x00` follow every `0x55`/`0x02` in SIZE, NODE and DATA.
- The checksum is inverted if it equals `0x55` or `0x02`.

| Data byte | Description |
|---|---|
| 0 | Frame type: `0x01` = stream samples |
| 1 | Flags: bit 0 = samples lost before this frame · bit 1 = prescaler changed · bit 2 = stream stopped (last frame) · bit 3 = stopped because the channel configuration changed · bit 4 = stopped because the host was lost (keep-alive timeout or transport disconnect) |
| 2 … 5 | Tick counter of the first sample in this frame (uint32) |
| 6 … 7 | Stream prescaler *p* of all samples in this frame (uint16) |
| 8 … 9 | Samples lost between the previous frame and this one (uint16, saturating) |
| 10 | Number of samples *k* (0 only in a final "stopped" frame) |
| 11 … | *k* stream samples |

**Guarantees:**
- **No gaps inside a frame:** all samples in a frame are consecutive, have the same prescaler, and sample *i* was taken at tick `timestamp + i × (p + 1)`.
- **Gaps only between frames:** they are visible as `timestamp ≠ previous timestamp + previous k × (p + 1)`. The lost counter gives the number of dropped samples.
- **Samples per frame:** at most 255 − 11 = 244 data bytes, so *k* ≤ `floor(244 / streamSampleSize)` (3 channels × 4 B → 20 samples).
- **Frame size follows the load:** frames carry as many samples as are available when the transmitter becomes free, up to that maximum. At low rates they are small with low latency; at high rates they are full with low overhead.

## 8 Firmware behaviour

### 8.1 `X2Cscope_Update()` (interrupt context)
- The offline capture is unchanged, working on the offline region.
- **Streaming** (when running):
  - Increment the tick counter.
  - Every *p* + 1 ticks a stream sample is due:
    - If the ring has room and is not in drop mode, copy the stream channels into the ring, then publish the new write index.
    - Otherwise count the sample as lost and enter **drop mode** (section 8.3).
  - A pending prescaler change is applied at the next due sample.

### 8.2 `X2Cscope_Communicate()` (main loop): TX scheduling
- **A frame is never interrupted.** When a frame is finished:
  1. If a response is pending, send it. There is at most one, because the host has one outstanding request.
  2. Otherwise, if the ring holds at least one sample, send a stream frame.
- A response is therefore delayed by at most one stream frame. A stream frame waits for at most one response.
- **Stream frames are sent directly from the ring.** The 11-byte header is in its own small buffer; checksum and fill bytes are computed while sending. `bufferLNet` is not used.
- **The read index advances only after the frame has been sent completely,** so the interrupt never overwrites samples that are still being sent.
- **RX:** request bytes are received into `bufferLNet` **while a stream frame is being sent**. This is new: today RX is blocked during every transmission. RX remains blocked while a response is pending or being sent, as today.
- `flush()` is called after every stream frame, as after standard frames, so the TCP and CAN examples send it.

### 8.3 Lost samples and drop mode
- When a stream sample is due and the ring is full, the device enters **drop mode**. It drops all due samples, counting them, **until the ring is empty**, then resumes with the next due sample.
- **Reason:** the gap is then always at a frame boundary, and the first frame after the gap starts with an exact timestamp. No per-sample timestamps are needed (no extra RAM or bandwidth).
- **Cost:** after an overflow, up to one ring's worth of additional samples is dropped. Overflow is an error case that the ring size (section 9) and the automatic prescaler should prevent.

### 8.4 Automatic prescaler (optional, flag bit 0 at start)
- If two consecutive stream frames report lost samples, the device sets `p = 2 × (p + 1) − 1`, which doubles the sample period (limited to 65535).
- It **never decreases automatically.** The host can set a lower prescaler with command `0x02`.
- Every change is reported by the "prescaler changed" flag and the prescaler field.

### 8.6 Keep-alive: stop when the host is gone

The stream stops when the host disconnects. Two mechanisms are available, and either one stops it:

1. **Keep-alive timeout (all transports).**
   - Every complete, valid LNet frame received from the host restarts the timer: any request, e.g. a watch read or Stream Control status.
   - If no valid frame arrives for the configured number of ticks, the device stops the stream.
   - The host must send a request at least once per timeout. Stream Control status (`0x03`) serves as a heartbeat when there is no other traffic.
   - The timeout is given in ticks because the device has no time base other than `X2Cscope_Update()`. The host converts from its sample time, e.g. 2 s at 20 kHz = 40000 ticks.
2. **Transport disconnect (optional, transports that know it).**
   - A new API function `X2Cscope_HostDisconnected()` stops the stream immediately.
   - The TCP example calls it from the connection-closed callback. UART has no notion of a connection and relies on the timeout.

After a stop, the device sends the final frame with flags "stopped" and "host lost". It may not reach the host, and that is harmless. On TCP the user's `X2CscopeComm_Flush()` must drop the data if no connection exists.

### 8.7 Concurrency
- **One writer, one reader, no locks.** The interrupt writes the ring write index; the main loop writes the read index.
- **Indices are 16-bit** (`scopeSize` is ≤ 65535), so accesses are atomic on 16-bit targets.
- **Publish order:** the interrupt writes the sample data before it publishes the write index (`volatile` and access order).
- **No shared read-modify-write of counters:**
  - The lost-sample counter is written only by the interrupt. The main loop keeps its own "last reported" value and sends the difference.
  - Drop-mode recovery and prescaler changes are published by the interrupt as an *epoch*: the tick of the first sample and the ring position, written before an epoch counter. The main loop reads the epoch when it starts a frame and ends frames at epoch boundaries.

## 9 Ring size (host side)

The ring must hold the stream samples taken while the transmitter is busy with something else. The longest such time is one maximum-size frame (a response, e.g. offline buffer readout with Get RAM Block, ≈ 261 bytes on the wire) plus the main-loop delay:

**ringSamples ≥ ceil((maxFrameTime + mainLoopDelay) / streamPeriod) + 1**

| Link | maxFrameTime (≈ 261 B) | 2 samples are enough up to (main-loop delay neglected) | 200 Hz stream (20 kHz, *p* = 99), main-loop delay 1 ms |
|---|---|---|---|
| UART 115200 | ≈ 23 ms | ≈ 40 Hz | ≥ 6 samples |
| UART 921600 | ≈ 2.9 ms | ≈ 340 Hz | ≥ 2 samples |
| T1S 10 Mbit/s | ≈ 0.2 ms | ≈ 5 kHz | ≥ 2 samples |

The host computes the ring size from this rule. The minimum is 1 sample; 2 is recommended. The automatic prescaler is a safety net (e.g. for PC-side hiccups), not the primary sizing.

## 10 Bandwidth

Wire bytes per frame = 4 (SYN, SIZE, NODE, CHK) + 11 (header) + *k* × `streamSampleSize` + fill bytes (≈ 1 % for random data).

Example: 3 channels × 4 B at 200 Hz = 2.4 kB/s of samples.

| Samples per frame *k* | Wire load | UART 115200 (≈ 11.5 kB/s usable) |
|---|---|---|
| 1 | ≈ 5.4 kB/s | 47 % of the link |
| 4 | ≈ 3.2 kB/s | 28 % of the link |
| 20 (full) | ≈ 2.6 kB/s | 23 % of the link |

The host checks `rate × streamSampleSize × overhead` against the measured link speed before starting, and leaves room for the offline readout and watch traffic.

## 11 Host procedure

1. Detect support (section 2).
2. Configure the scope channels and, if needed, an offline capture with Save Parameter, as today.
3. Compute the ring size (section 9) and check the bandwidth (section 10).
4. With the offline capture idle: Stream Control start (mask, prescaler, ring samples, automatic prescaler flag, keep-alive timeout).
5. Send a request at least once per keep-alive timeout; use Stream Control status if there is no other traffic.
6. **Receive frames by SYN:** `0x02` → stream queue, `0x55` → response to the pending request. Resynchronise on SYN after errors. One outstanding request at a time, as today.
7. Offline captures (Save Parameter, Load Parameter, Get RAM Block) and watch reads/writes continue normally. Re-arming with the same channels keeps the stream running.
8. Rebuild the time axis from timestamp, prescaler and *k*; mark gaps from the lost counter.
9. Stream Control stop; wait for the final frame with the "stopped" flag.

## 12 Examples

### 12.1 Stream Control start

Mask `0x07` (channels 0–2), prescaler 100, 8 ring samples, automatic prescaler on, keep-alive 40000 ticks (2 s at 20 kHz).

Request frame:

| Byte | Value | Description |
|---|---|---|
| 0 | 0x55 | SYN |
| 1 | 0x0C | Data frame size (12) |
| 2 | 0x01 | Slave node ID |
| 3 | 0x16 | Service ID 'Stream Control' |
| 4 | 0x01 | Command: start |
| 5 | 0x07 | Channel mask |
| 6 … 7 | 0x64 0x00 | Prescaler = 100 |
| 8 … 9 | 0x08 0x00 | Ring samples = 8 |
| 10 | 0x01 | Flags: automatic prescaler |
| 11 … 14 | 0x40 0x9C 0x00 0x00 | Keep-alive = 40000 ticks |
| 15 | 0xC9 | Checksum |

Response frame (stream sample 3 × 4 B = 12 B; ring 96 B; offline region 5000 − 96 = 4904 B):

| Byte | Value | Description |
|---|---|---|
| 0 | 0x55 | SYN |
| 1 | 0x12 | Data frame size (18) |
| 2 | 0x01 | Slave node ID |
| 3 | 0x16 | Service ID 'Stream Control' |
| 4 | 0x00 | No error |
| 5 | 0x01 | Stream state: running |
| 6 | 0x07 | Channel mask |
| 7 … 8 | 0x64 0x00 | Prescaler = 100 |
| 9 … 10 | 0x08 0x00 | Ring samples = 8 |
| 11 | 0x0C | Stream sample size = 12 |
| 12 … 15 | 0x28 0x13 0x00 0x00 | Offline region = 4904 bytes |
| 16 | 0x01 | Flags: automatic prescaler |
| 17 … 20 | 0x40 0x9C 0x00 0x00 | Keep-alive = 40000 ticks |
| 21 | 0x16 | Checksum |

### 12.2 Stream frame

Two samples, prescaler 100, first sample at tick 1000. Channels: ch0 int32, ch1 float, ch2 int32. *k* = 2 is a reserved value and is followed by a fill byte.

| Byte | Value | Description |
|---|---|---|
| 0 | 0x02 | SYN (stream frame) |
| 1 | 0x23 | Data frame size (35 = 11 header + 2 × 12) |
| 2 | 0x01 | Slave node ID |
| 3 | 0x01 | Frame type: stream samples |
| 4 | 0x00 | Flags: none |
| 5 … 8 | 0xE8 0x03 0x00 0x00 | Timestamp = tick 1000 |
| 9 … 10 | 0x64 0x00 | Prescaler = 100 |
| 11 … 12 | 0x00 0x00 | Lost samples = 0 |
| 13 | 0x02 | *k* = 2 samples |
| 14 | 0x00 | FILL BYTE |
| 15 … 18 | 0xE8 0x03 0x00 0x00 | Sample 0, ch0 = 1000 (tick 1000) |
| 19 … 22 | 0x00 0x00 0xC0 0x3F | Sample 0, ch1 = 1.5 |
| 23 … 26 | 0xFE 0xFF 0xFF 0xFF | Sample 0, ch2 = −2 |
| 27 … 30 | 0xE9 0x03 0x00 0x00 | Sample 1, ch0 = 1001 (tick 1101) |
| 31 … 34 | 0x00 0x00 0xC0 0x3F | Sample 1, ch1 = 1.5 |
| 35 … 38 | 0xFE 0xFF 0xFF 0xFF | Sample 1, ch2 = −2 |
| 39 | 0x43 | Checksum |

## 13 Firmware changes

### 13.1 OSM is removed, not rewritten

Streaming is new code with new names. The OSM code is deleted, because its structure does not fit: it reads values in the main loop, sends with blocking `send()` calls, and needs a hardware FIFO.

| Removed (OSM) | Replaced by |
|---|---|
| `LNet_OnlineScope()` (`LNet.c`) | TX scheduler in `communicate()` that sends stream frames from the ring, e.g. `LNet_SendStreamFrame()` |
| Scope states 4, 5, 6 in `Scope_Main_Update()` | Stream sampling in `Scope_Main_Update()`, independent of the offline state, e.g. `Scope_Stream_Update()` |
| `SCOPE_MAIN.timestamp` (16-bit), `txFrameSize` | Stream fields: mask, prescaler, counters, ring indices, 32-bit tick counter, epoch |
| `getTxFifoFree` in `tInterface`, `SerialGeneric.c` stub | – (not needed) |
| `SV_ID_SVOSMCONTROL` (0x1A) | `SV_ID_STREAMCONTROL` (0x16) |

- **Scope states 4–6 stay reserved.** A Save Parameter with state 4, 5 or 6 is answered with `0x14` Format error. No host uses them.
- **Load Parameter (scope block) keeps its byte layout,** with only the scope-version byte changing to `0x83`. Hosts read serialised fields, not the `SCOPE_MAIN` struct, so the struct can change.
- **`checkFill()` keeps treating `0x02` as a reserved value** in received frames, as LNet §3.6 requires.

### 13.2 New API

| Function | Purpose |
|---|---|
| `X2Cscope_HostDisconnected()` | Called by the user's transport code when the host connection is lost (section 8.6) |

### 13.3 Transport examples

- **TCP:**
  - `X2CscopeComm_IsSendReady()` returns `true` always, and `X2CscopeComm_Flush()` ignores send failures. With push streaming, frames would then be lost silently when the TCP send buffer is full.
  - Fix: return `false` while the TCP stack cannot take a frame (≥ 516 bytes free). Then the ring absorbs the delay, and losses are counted and adapted.
  - Call `X2Cscope_HostDisconnected()` from the connection-closed callback.
- **UART:** no change. The keep-alive timeout handles disconnects.
- **CAN:** single device only; check that the fragmentation handles back-to-back stream frames.

### 13.4 CPU load on fast links

- The TCP example already sends **one TCP segment per LNet frame** (TX buffer of 516 bytes, sent in `X2CscopeComm_Flush()`). That makes the network side efficient.
- It does **not** reduce the CPU work inside the library: LNet still calls the send callback once per byte (two indirect calls through the wrapper), plus checksum and fill check.
- At typical streaming rates (≤ 100 kB/s) this is a few percent of CPU. Only at T1S rates of around 1 MB/s would a block-send callback (pointer and length) be worth adding. Not part of this version.

## 14 Open points

- Polled access (for multi-node buses) is not part of this version.
- **Host side (mchplnet):** split frames by SYN and resynchronise, queue stream frames, parse the scope-version byte. See `doc/online_scope_mode.md` §8.
