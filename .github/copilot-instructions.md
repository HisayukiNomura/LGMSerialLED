# LGMSerialLED - Taiwan Little Green Man LED Matrix

Taiwan's "Little Green Man" (小緑人) pedestrian crossing signal animation display for Raspberry Pi Pico2 using WS2812 LED matrix. This C++ project creates an animated LED display showing character walking animations with button controls for starting/stopping and character selection.

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the information here.

## Working Effectively

### Dependencies and Setup
Install required dependencies and toolchain:
- `sudo apt update`
- `sudo apt install -y gcc-arm-none-eabi build-essential git cmake`

### Build Process
**CRITICAL BUILD TIMING: NEVER CANCEL builds or long-running commands.**

#### Initial Build (Fresh Clone)
- `cd /path/to/LGMSerialLED`
- `rm -rf build && mkdir build`
- `cd build`
- `export PICO_SDK_FETCH_FROM_GIT=1`
- `cmake ..` -- takes 30 seconds. NEVER CANCEL. Set timeout to 60+ minutes for safety.
- `make -j$(nproc)` -- takes 32 seconds. NEVER CANCEL. Set timeout to 60+ minutes for safety.

Total initial build time: ~62 seconds including SDK download and compilation.

#### Incremental Builds (After Changes)
- `cd build`
- `make -j$(nproc)` -- takes <0.4 seconds for small changes.

#### Clean Rebuild
- `rm -rf build && mkdir build && cd build`
- `export PICO_SDK_FETCH_FROM_GIT=1 && cmake .. && make -j$(nproc)`

### Output Files
The build generates these important files in `build/`:
- `LGMSerialLED.uf2` - Firmware file for uploading to Pico2 (drag-and-drop to Pico in BOOTSEL mode) - ~153KB
- `LGMSerialLED.elf` - ELF binary for debugging
- `LGMSerialLED.hex` - Hex format binary
- `LGMSerialLED.bin` - Raw binary format

## Validation
**HARDWARE VALIDATION REQUIRED:** This project requires physical hardware for proper validation.

### Manual Hardware Validation Scenarios
Since there are no automated tests, ALWAYS validate changes with hardware when possible:

1. **Basic Functionality Test:**
   - Upload `LGMSerialLED.uf2` to Pico2 in BOOTSEL mode
   - Connect WS2812 16x16 LED matrix to GPIO 22
   - Connect button switches to GPIO 27 (character select) and GPIO 28 (start/stop)
   - Power on: Should show hibernation state (dim/off)

2. **State Transition Test:**
   - Press GPIO 28 button: Should transition to stop state (red signal)
   - Press GPIO 28 again: Should start walking animation (green little green man)
   - Verify 50-second walk phase followed by 10-second running phase
   - Verify automatic return to stop state, then hibernation after 30 seconds

3. **Character Selection Test:**
   - In stop state, press GPIO 27: Should cycle through different characters
   - Available characters: Little Green Man, Mario, Zelda, Kirby, Dragon Quest 3 hero
   - Verify each character displays correctly and animates properly

### Build Validation
- ALWAYS run a complete build after making changes
- Check that `LGMSerialLED.uf2` file is generated and has reasonable size (~153KB)
- Verify no build warnings or errors

## Code Structure

### Key Source Files
- `LGMSerialLED.cpp` - Main application with state machine and button handling
- `PatManager.cpp` - Pattern management with gamma/brightness/contrast controls
- `Pat*.cpp` files - Character animation patterns (PatSignal.cpp, PatMario.cpp, etc.)
- `WS2812/source/WS2812.cpp` - LED matrix driver using PIO for precise timing
- `WS2812/source/ws2812.pio` - Programmable I/O assembly for WS2812 protocol

### Important Hardware Constants
- `PIN_WS2812_1 22` - GPIO 22 for LED matrix data
- `BUTTON_PIN_ENTER 28` - GPIO 28 for start/stop button  
- `BUTTON_PIN_SET 27` - GPIO 27 for character selection
- Target board: `pico2` (Raspberry Pi Pico2 / RP2350)

### Build Configuration
- CMake minimum version: 3.13
- C standard: C11
- C++ standard: C++17
- UART output enabled, USB output disabled
- PIO hardware used for precise WS2812 timing control

## Common Tasks

### After Making Code Changes
1. `cd build && make -j$(nproc)` - Fast incremental build
2. Check `LGMSerialLED.uf2` is updated (check timestamp)
3. If possible, test on hardware using the validation scenarios above
4. For major changes, consider clean rebuild

### Troubleshooting Build Issues
- If CMake fails: Ensure `PICO_SDK_FETCH_FROM_GIT=1` is set
- If ARM compiler not found: `sudo apt install gcc-arm-none-eabi`
- If build hangs: Wait at least 60 minutes before canceling (SDK download can be slow)
- For permission errors: Check that build directory is writable

### Working with Patterns
- Pattern files are in `Pat*.cpp` - each defines animation frames as uint32_t arrays
- Colors are in GRB format (0x00GGRRBB) for WS2812 compatibility
- Matrix is 16x16 pixels, stored row-major order
- PatManager handles gamma correction, brightness, and contrast adjustments

### LED Matrix Configuration
- Default configuration assumes serpentine wiring (zigzag connection pattern)
- Starting position configurable (typically top-left or top-right)
- Supports multiple panel configurations via constructor parameters

## Project-Specific Details

### WS2812 Timing Requirements
- This project uses PIO (Programmable I/O) for precise timing control
- WS2812 requires 1.25μs ±600ns timing precision per bit
- PIO code in `ws2812.pio` handles the critical timing automatically
- Do not modify PIO timing without understanding WS2812 protocol requirements

### No Automated Testing
- This project has no unit tests or automated test framework
- All validation must be done manually with physical hardware
- Focus on building successfully and verifying output file generation
- Use hardware validation scenarios above when changes affect functionality

### Memory and Performance
- 16x16 LED matrix = 256 LEDs × 24 bits = 6,144 bits per frame
- Display refresh rate: ~129 FPS (sufficient for smooth animation)
- Uses VRAM (video RAM) pattern for double-buffering
- Gamma correction and brightness controls available through PatManager

This project demonstrates advanced embedded programming concepts including:
- PIO assembly programming for precise hardware timing
- State machine design for user interface
- Hardware debouncing and button handling
- Color space management and gamma correction
- Multi-character animation system