# X2Cscope Protocol Documentation

This folder is the **single source of truth** for the protocol between the X2Cscope library (firmware) and its hosts (mchplnet / pyX2Cscope, X2Cscope GUI).

## Documents

| Document | Status | Content |
|---|---|---|
| *LNet Documentation*, LCM, LNet v5 (not in this repository, see below) | Normative base (external) | Linz Center of Mechatronics GmbH (LCM), LNet version 5, document version 5, March 3 2017. The protocol X2Cscope implements. |
| [`lnet/x2cscope_lnet_profile.md`](lnet/x2cscope_lnet_profile.md) | **Normative** | What X2Cscope implements of LNet v5, how it fills the open points (scope block, processor IDs, address widths), where it deviates, and which extensions it adds. |
| [`scope_streaming.md`](scope_streaming.md) | Normative, **draft** | Extension: scope streaming (Stream Control `0x16`, stream frames with SYN `0x02`), issue #13. |
| *LNet Documentation*, LCM, LNet v6 (not in this repository, see below) | Informative (external) | LNet version 6, document version 9, June 28 2023. Not implemented; wire-incompatible with v5. |
| [`lnet_v5_v6_comparison.md`](lnet_v5_v6_comparison.md) | Informative | Differences between LNet v5 and v6, migration effort, memory impact. |
| [`online_scope_mode.md`](online_scope_mode.md) | Informative | How the legacy online scope mode works and why it is broken; host-side findings. |

## Precedence

1. **LNet v5 (LCM)** defines the protocol.
2. **The X2Cscope profile** clarifies, restricts, deviates from and extends it. Where the two disagree, the profile describes the actual X2Cscope behaviour.
3. **Extension specs** (e.g. `scope_streaming.md`) define one extension each and are listed in the profile, section 9.

All X2Cscope-specific statements are in the Markdown documents of this folder.

## LCM documents

The LNet documents are © Linz Center of Mechatronics GmbH and are **not published in this repository**. They are referenced by title, LNet version, document version and date only. Contributors with access obtain them internally (X2C documentation). Do not commit them; `doc/lnet/*.pdf` is ignored by `.gitignore` so that a local copy can be kept next to the profile.

The X2Cscope documents are written so that the protocol as implemented by X2Cscope can be understood from the profile together with the LCM document. Section references such as "v5 §3.6" point into the LCM document.

## Changing the protocol

- **Every protocol change updates the profile:** classify it (clarification, restriction, deviation, extension, known issue) and add a line to its revision history. A new extension also gets its own spec in this folder.
- **Firmware and host are implemented against these documents**, not against each other.
- **Example frames** in the documents are verified byte by byte. mchplnet's `tests/test_framing.py` uses the frames from the LNet v5 document and from `scope_streaming.md`.
- **Behaviour described in the profile is tested** where possible by the host tests in `tests/host/` (run on every pull request; see `tests/host/README.md`).
