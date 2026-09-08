# ESP32 Neural Compute Engine

An experimental, systems-first project exploring bare-metal machine learning primitives, constrained memory management, and distributed compute orchestration on an **ESP32-WROOM-32** microcontroller.

---

## What is this?

The **ESP32 Neural Compute Engine** is an exploratory effort to build a transparent, low-level neural network and language-model inference runtime from scratch.

Rather than importing high-level AI frameworks or opaque third-party inference engines, this project implements the fundamental systems layer-by-layer:

- **Embedded Firmware Development & Toolchains**
- **Host ↔ Microcontroller Communication & Protocols**
- **Constrained Memory Allocation & Heap Management**
- **Tensor Operations & Matrix Linear Algebra**
- **Neural Network Layers & Activation Functions**
- **Weight Quantization & Memory Streaming**
- **Transformer Inference & Token Generation**

---

## Motivation

Modern machine learning stacks abstract away hardware execution behind deep layers of framework code (PyTorch, C++ runtimes, CUDA drivers). 

The goal of this project is **educational systems engineering**: to understand what happens underneath high-level ML abstractions when execution is pushed down to hardware with severe resource constraints (no GPU, ~350 KB free RAM, no external PSRAM).

The project progressively unpacks the entire stack:
$$\text{Source Code} \longrightarrow \text{Compiler} \longrightarrow \text{Firmware} \longrightarrow \text{CPU Execution} \longrightarrow \text{Memory Management} \longrightarrow \text{Tensors} \longrightarrow \text{Matrix Algebra} \longrightarrow \text{Transformer Layers} \longrightarrow \text{Token Generation}$$

---

## Target Architecture

The long-term deployment architecture separates model orchestration/storage (host machine) from raw matrix and tensor execution (microcontroller compute node):

```text
                    Developer
                       │
                       │ SSH
                       ▼
               Raspberry Pi
        ┌─────────────────────────┐
        │ terminal/controller     │
        │ model storage           │
        │ model loader            │
        │ tokenizer/orchestration │
        └────────────┬────────────┘
                     │
                 USB / Serial
                     │
                     ▼
               ESP32-WROOM-32
        ┌─────────────────────────┐
        │ communication layer     │
        │ memory manager          │
        │ tensor engine           │
        │ neural network runtime  │
        │ transformer inference   │
        └─────────────────────────┘
```

> **Note on Current Integration Status:**  
> The Raspberry Pi host controller is planned for Phase 2. Development is currently conducted directly from a **Mac workstation** connected via serial USB to the ESP32.

---

## Current Status

The project is currently between **Phase 1 (Development Pipeline)** and **Phase 2 (Compute Node Foundations)**.

### Working Today
- [x] Bare-metal PlatformIO build pipeline (`pipeline.sh`).
- [x] Interactive Mac host terminal (`terminal.py`) with line-buffered input and local echo.
- [x] Modular C++ firmware architecture (`include/` and `src/`).
- [x] Real-time hardware telemetry (`micros()` execution timing & free heap tracking).
- [x] Recursive-descent scalar math parser (PEMDAS & scientific functions).
- [x] 2D Matrix engine (parsing, dimension checking, addition, subtraction, multiplication, scalar scaling).

### Not Yet Implemented
- [ ] Raspberry Pi host orchestration.
- [ ] Compact binary communication protocol.
- [ ] N-dimensional Tensor abstractions.
- [ ] Neural network layer primitives (Dense, LayerNorm, Softmax, GELU).
- [ ] Quantization (INT8 / INT4 weight representation).
- [ ] Weight streaming & chunked memory loader.
- [ ] Transformer block inference & Tokenizer.

---

## Hardware Specifications & Measured Telemetry

### Hardware
* **Microcontroller:** ESP32-WROOM-32 (ESP32-D0WD-V3, Revision 3)
* **CPU:** Xtensa® Dual-Core 32-bit LX6 @ 240 MHz
* **Current Host:** Mac Workstation (USB Serial `/dev/cu.usbserial-0001` @ 115200 baud)
* **Planned Host:** Raspberry Pi 3B+ (2 GB RAM)
* **External RAM / PSRAM:** None (Operating strictly within 520 KB internal SRAM)

### Development Build Measurements
* **Build Toolchain:** PlatformIO Core (Espressif 32 Platform / Arduino Framework)
* **Clean Build Time:** ~3.7 seconds
* **Flash Usage:** ~303 KB / 1,310,720 bytes (23.1%)
* **RAM Static Allocation:** ~21 KB / 327,680 bytes (6.6%)
* **Observed Runtime Free Heap:** ~350 KB

---

## Software Stack & Project Structure

```text
esp32/
├── include/
│   ├── MatrixEngine.h          # Matrix data structures & 2D algebra declarations
│   ├── ScalarMathParser.h      # Recursive descent PEMDAS math parser class
│   └── CommandProcessor.h      # Serial command router & telemetry logger
│
├── src/
│   ├── MatrixEngine.cpp        # Matrix parsing, addition, subtraction, multiplication
│   ├── ScalarMathParser.cpp    # Scalar math & scientific functions (sqrt, sin, log, etc.)
│   ├── CommandProcessor.cpp    # Command dispatcher and response logger
│   └── main.cpp                # Lightweight setup() & loop() entry point
│
├── pipeline.sh                 # Single-command build, flash, and terminal monitor script
├── terminal.py                 # Interactive host terminal client (Line-buffered)
├── platformio.ini              # PlatformIO project configuration & terminal filters
└── README.md
```

### Module Responsibilities

1. **`MatrixEngine` ([`include/MatrixEngine.h`](file:///Users/apple/codes/timepass/esp32/include/MatrixEngine.h) / [`src/MatrixEngine.cpp`](file:///Users/apple/codes/timepass/esp32/src/MatrixEngine.cpp))**
   - Matrix representations up to 16x16.
   - Dual-syntax string parsing: nested arrays (`[[1,2],[3,4]]`) and MATLAB semicolon format (`[1 2; 3 4]`).
   - Dimension compatibility validation.
   - Matrix operations: Addition ($A+B$), Subtraction ($A-B$), Matrix Multiplication ($A \times B$), and Scalar Scaling ($k \cdot A$).
   - Serves as the mathematical foundation for upcoming Tensor data structures.

2. **`ScalarMathParser` ([`include/ScalarMathParser.h`](file:///Users/apple/codes/timepass/esp32/include/ScalarMathParser.h) / [`src/ScalarMathParser.cpp`](file:///Users/apple/codes/timepass/esp32/src/ScalarMathParser.cpp))**
   - Recursive-descent parser enforcing operator precedence (PEMDAS: `+`, `-`, `*`, `/`, `%`, `^`, `( )`).
   - Built-in scientific functions: `sqrt`, `abs`, `sin`, `cos`, `tan`, `log`, `exp`.
   - Used for hardware telemetry verification and arithmetic execution.

3. **`CommandProcessor` ([`include/CommandProcessor.h`](file:///Users/apple/codes/timepass/esp32/include/CommandProcessor.h) / [`src/CommandProcessor.cpp`](file:///Users/apple/codes/timepass/esp32/src/CommandProcessor.cpp))**
   - Handles incoming line-buffered serial data.
   - Built-in commands (`help`, `info`, `ping`).
   - Routes inputs to Matrix or Scalar engines.
   - Logs input, execution time in microseconds (`micros()`), and remaining free heap memory.

4. **`main.cpp` ([`src/main.cpp`](file:///Users/apple/codes/timepass/esp32/src/main.cpp))**
   - Minimal entry point responsible for Serial interface initialization (115200 baud) and delegating incoming serial streams to `CommandProcessor`.

---

## Development Pipeline

The project features a terminal-driven development pipeline independent of IDE GUIs:

```bash
# Clone the repository
git clone https://github.com/your-username/esp32-neural-compute-engine.git
cd esp32-neural-compute-engine/esp32

# Build firmware, upload to ESP32, and launch interactive terminal
./pipeline.sh
```

### Interactive Serial Terminal Session

```text
==================================================
    ESP32 Interactive Build, Upload & Console    
==================================================
==> Step 1/2: Compiling & Uploading Firmware...
[SUCCESS] Took 7.19 seconds

==> Step 2/2: Launching Interactive Console...
Connected! Type expressions and press Enter.

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

## Project Roadmap

### Phase 1 — Headless Development Pipeline
- [x] PlatformIO automated compilation.
- [x] USB serial flashing & reset automation.
- [x] Host interactive terminal client with local echo and send-on-enter line buffering.
- [x] Modular C++ architecture (`MatrixEngine`, `ScalarMathParser`, `CommandProcessor`).
- [ ] Migrate build, flash, and monitor pipeline to run headlessly on a Raspberry Pi over SSH.

### Phase 2 — Distributed Compute Node
- [ ] Define compact binary frame protocol (Host ↔ ESP32).
- [ ] Hardware LED status indicators (Receiving, Computing, Complete, Error).
- [ ] Memory pool manager for static dynamic allocation avoidance.
- [ ] Generalized N-dimensional Tensor struct.
- [ ] SIMD / Xtensa ISA optimizations for vector dot-products.

### Phase 3 — Neural Network & Transformer Inference
- [ ] Linear (Fully-Connected) layer implementation.
- [ ] Activation functions (ReLU, GELU, Softmax, LayerNorm).
- [ ] Quantization support (INT8 / INT4 de-quantization on the fly).
- [ ] Weight chunk streaming parser.
- [ ] Single-head & Multi-head self-attention module.
- [ ] Host-side tokenizer & autoregressive generation loop.

---

## Memory / Weight Streaming Experiment

A core objective of this project is investigating **inference on models exceeding the MCU's total internal RAM**.

Because an ESP32-WROOM-32 lacks external PSRAM, storing an entire model (even a 1M to 10M parameter model) inside SRAM is impossible.

### Weight Streaming Concept

```text
           Host (Raspberry Pi / Mac)
          ┌─────────────────────────┐
          │ model.bin (Quantized)   │
          └────────────┬────────────┘
                       │
             Layer Weight Chunks
                       │
                       ▼
                 ESP32 SRAM
          ┌─────────────────────────┐
          │ Ping-Pong Buffer        │
          │ Current Layer Computation│
          └────────────┬────────────┘
                       │
             Hardware Execution
                       │
                       ▼
                 Layer Output
```

- **Chunked Transfer:** The host streams layer weights in small chunks over Serial/SPI to a double-buffered RAM region on the ESP32.
- **On-the-Fly Compute:** The ESP32 computes matrix-vector multiplications for the current layer and discards weights before receiving the next layer's parameters.
- **Trade-off:** Communication bandwidth becomes the primary bottleneck. The goal is to measure and analyze this exact communication-vs-compute trade-off under real hardware constraints.

---

## Design Philosophy

1. **Transparency Over Black Boxes:** No pre-packaged machine learning libraries. Every matrix multiplication, parser, and buffer is written explicitly.
2. **Systems-First:** Prioritize memory layout, execution timing (`micros()`), and hardware telemetry.
3. **Honest Benchmarks:** Measure execution latency and heap usage empirically.
4. **Constrained Exploration:** Embrace hardware constraints to understand core computational limits.

---

## Future Experiments

- **Custom Binary Protocol:** Replace ASCII serial strings with a packed binary framing protocol (`SLIP` / `COBS`).
- **Hardware Status LEDs:** Map GPIO pins to state indicators (RGB LEDs for RX, COMPUTE, DONE, ERROR).
- **Weight Quantization Benchmarks:** Compare FP32 vs. INT8 matrix multiplication performance and precision loss.

---