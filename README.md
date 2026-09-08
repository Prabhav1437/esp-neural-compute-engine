# esp32-neural-compute-engine

trying to build a tiny, transparent neural net / language model inference engine on an esp32 from scratch.

the long term goal isn't just "run an llm on an esp32". it's to actually build and understand every single layer underneath model inference:
- firmware compilation & embedded dev
- host <-> microcontroller serial protocols
- memory management without psram
- tensors & matrix math
- neural network layers
- quantization & streaming model weights over serial
- transformer inference & token generation

eventually, the plan is to use a **raspberry pi 3b+** as the host/controller machine (for model storage & tokenization) and the **esp32** as the compute device.

> **current status:** pi isn't hooked up yet, so right now I'm building and testing everything directly from my Mac over USB serial.

---

## the architecture (eventual plan)

```
                    developer
                       │
                       │ SSH
                       ▼
               Raspberry Pi 3B+
        ┌─────────────────────────┐
        │ controller / host       │
        │ model storage & loader  │
        │ tokenizer / orchestrator│
        └────────────┬────────────┘
                     │
                 USB / Serial
                     │
                     ▼
               ESP32-WROOM-32
        ┌─────────────────────────┐
        │ comms layer             │
        │ memory manager          │
        │ tensor engine           │
        │ NN runtime              │
        │ transformer compute     │
        └─────────────────────────┘
```

The Pi will hold the model weights and stream them over, while the ESP32 does the actual heavy lifting / math execution.

---

## current status & what works today

right now I'm somewhere between phase 1 (pipeline) and phase 2 (compute node).

### working:
- [x] platformio build pipeline (`./pipeline.sh` handles build + flash + monitor in 1 step)
- [x] interactive python terminal client (`terminal.py`) with line buffering & local echo
- [x] modular c++ firmware architecture (`MatrixEngine`, `ScalarMathParser`, `CommandProcessor`)
- [x] real-time execution benchmarking (`micros()`) & heap monitoring
- [x] recursive descent math parser for PEMDAS + scientific functions (`sqrt`, `sin`, `cos`, `log`, etc.)
- [x] 2D matrix engine (parsing, dimension checks, addition, subtraction, multiplication, scalar scaling)

### not built yet:
- [ ] Raspberry Pi host controller integration
- [ ] custom binary protocol over serial
- [ ] full N-dimensional Tensor class
- [ ] neural net layers (Linear, LayerNorm, Softmax, GELU)
- [ ] INT8/INT4 weight quantization
- [ ] streaming weight loader
- [ ] transformer block & tokenizer

---

## hardware & current benchmarks

### setup
* **MCU:** ESP32-WROOM-32 (ESP32-D0WD-V3, Rev 3 @ 240 MHz)
* **RAM:** 520 KB internal SRAM (no external PSRAM attached)
* **Dev machine:** Mac over USB serial (`/dev/cu.usbserial-0001` @ 115200 baud)
* **Planned host:** Raspberry Pi 3B+ (2 GB RAM)

### current build numbers
* **Clean build time:** ~3.7s (PlatformIO Core + Arduino framework)
* **Flash size:** ~303 KB (23.1% of flash)
* **RAM static usage:** ~21 KB (6.6% of RAM)
* **Runtime free heap:** ~350 KB

---

## project structure

```text
esp32/
├── include/
│   ├── MatrixEngine.h          # matrix structs & math declarations
│   ├── ScalarMathParser.h      # recursive descent math parser
│   └── CommandProcessor.h      # serial command handler & logging
├── src/
│   ├── MatrixEngine.cpp        # matrix operations (add, sub, mult, scale)
│   ├── ScalarMathParser.cpp    # math parser & functions
│   ├── CommandProcessor.cpp    # command router & micros() timer
│   └── main.cpp                # minimal setup() and loop()
├── pipeline.sh                 # 1-click build, flash, and terminal script
├── terminal.py                 # interactive mac terminal client
└── platformio.ini              # platformio config
```

---

## how to run it

```bash
# clone repo
git clone https://github.com/Prabhav1437/esp-neural-compute-engine.git
cd esp-neural-compute-engine/esp32

# build, flash to esp32, and open interactive terminal
./pipeline.sh
```

### example session output:

```text
ESP32-Input> 1000 * 90108 + 100194847 - 10999 / 12
--------------------------------------------------
[INPUT RECEIVED]  "1000 * 90108 + 100194847 - 10999 / 12"
[PROCESSING]      Parsing multi-operator expression on ESP32 CPU...
[OUTPUT RESULT]   1000 * 90108 + 100194847 - 10999 / 12 = 190301930.4167
[ESP32 LOG]       Exec Time: 11725 us | Heap: 350436 bytes | Status: OK
--------------------------------------------------

ESP32-Input> [[1, 2], [3, 4]] + [[5, 6], [7, 8]]
--------------------------------------------------
[INPUT RECEIVED]  "[[1, 2], [3, 4]] + [[5, 6], [7, 8]]"
[PROCESSING]      Parsing Matrix Expression on ESP32 CPU...
[OUTPUT RESULT]   Matrix Addition Result (2x2):
                  |      6      8 |
                  |     10     12 |
                  Inline: [[6, 8], [10, 12]]
[ESP32 LOG]       Exec Time: 22496 us | Heap: 350668 bytes | Status: OK
--------------------------------------------------
```

---

## roadmap

### phase 1 — headless dev setup
- [x] PlatformIO compilation & flash script
- [x] interactive terminal client with local echo (`terminal.py`)
- [x] modular c++ architecture
- [ ] run the build & monitor workflow headlessly from Pi over SSH

### phase 2 — compute node
- [ ] binary framing protocol (host <-> esp32)
- [ ] hardware status LEDs (RX, COMPUTE, DONE, ERROR)
- [ ] static memory pool manager
- [ ] generalized N-D Tensor struct
- [ ] SIMD / Xtensa assembly optimizations for dot products

### phase 3 — model inference
- [ ] Linear / Fully Connected layer
- [ ] activations (ReLU, GELU, Softmax, LayerNorm)
- [ ] INT8 / INT4 quantization
- [ ] weight streaming parser
- [ ] tiny transformer block
- [ ] host tokenizer & token generation loop

---

## memory / weight streaming idea

since the ESP32 doesn't have external PSRAM, fitting a whole model (even a tiny one) in 350KB RAM is impossible.

the plan is to stream model weights from the host (Raspberry Pi) layer-by-layer over serial:

```text
Raspberry Pi (Host)                 ESP32 (Compute Node)
┌──────────────────┐               ┌──────────────────┐
│ model.bin        │  ──weights──> │ RAM buffer       │
│ metadata         │   over serial │ compute layer    │
└──────────────────┘               └────────┬─────────┘
                                            │
                                         result
```

it's definitely going to be slow because of serial transfer bottlenecks, but speed isn't the point — learning how to squeeze inference into memory-constrained hardware is.

---

## license

MIT