# ZMK MIDI Keyboard - Setup & Integration Guide

## Overview

You now have a complete ZMK MIDI keyboard board repository ready to use! This guide explains how all the pieces fit together and walks you through the complete setup.

## What You Have

### Board Repository Structure
```
~/zmk-configs/zmk-midi-keyboard/
├── README.md                    # Full documentation
├── QUICK_START.md              # 5-minute setup guide
├── MIDI_NOTE_REFERENCE.md      # Complete note chart
├── SETUP_GUIDE.md              # This file
├── build.yaml                  # GitHub Actions config
├── .gitignore
├── config/
│   ├── west.yml               # Zephyr manifest
│   ├── midi_keyboard.conf     # Build config (USB, MIDI settings)
│   └── midi_keyboard.keymap   # Keyboard layout (4x4 default)
├── boards/                     # Empty (uses nice_nano_v2 from ZMK)
└── zephyr/
    └── module.yml             # Zephyr module configuration
```

### Files from zmk_usb_midi Repository
```
~/zmk-configs/zmk_usb_midi/
├── app/
│   ├── include/zmk/
│   │   └── usb_midi.h          # MIDI API header
│   ├── src/
│   │   ├── usb/
│   │   │   ├── usb_midi.c
│   │   │   ├── usb_midi_descriptors.c
│   │   │   └── usb_midi_integration.c
│   │   └── events/
│   │       └── midi_event.h
│   ├── Kconfig                 # Configuration options
│   ├── prj.conf               # Default settings
│   └── CMakeLists.txt         # Build config
└── (documentation files)
```

## Complete Setup Process

### Step 1: Verify ZMK Installation

Ensure you have ZMK set up in `~/zmk/`:

```bash
cd ~/zmk
ls -la
# Should show: app/ bootloader/ docs/ etc.

cd app
ls -la  
# Should show: src/ include/ CMakeLists.txt etc.
```

### Step 2: Create MIDI Source Directories

Add USB directory structure to ZMK if it doesn't exist:

```bash
mkdir -p ~/zmk/app/src/usb
mkdir -p ~/zmk/app/src/events
```

### Step 3: Copy MIDI Implementation Files

Copy the MIDI USB module files:

```bash
# Copy USB MIDI header
cp ~/zmk-configs/zmk_usb_midi/app/include/zmk/usb_midi.h \
   ~/zmk/app/include/zmk/

# Copy USB MIDI source files
cp ~/zmk-configs/zmk_usb_midi/app/src/usb/usb_midi.c \
   ~/zmk/app/src/usb/

cp ~/zmk-configs/zmk_usb_midi/app/src/usb/usb_midi_descriptors.c \
   ~/zmk/app/src/usb/

cp ~/zmk-configs/zmk_usb_midi/app/src/usb/usb_midi_integration.c \
   ~/zmk/app/src/usb/

# Copy MIDI event definitions
cp ~/zmk-configs/zmk_usb_midi/app/src/events/midi_event.h \
   ~/zmk/app/src/events/

# Copy Kconfig (IMPORTANT!)
cp ~/zmk-configs/zmk_usb_midi/app/Kconfig \
   ~/zmk/app/src/usb/Kconfig.usb_midi
```

### Step 4: Update ZMK Build System

Edit `~/zmk/app/CMakeLists.txt`:

Find the `target_sources(app PRIVATE` section and add MIDI sources:

```cmake
target_sources(app PRIVATE
    # ... existing sources ...
    src/usb/usb_midi.c
    src/usb/usb_midi_descriptors.c
    src/usb/usb_midi_integration.c
)
```

Example location in CMakeLists.txt:
```cmake
target_sources(app PRIVATE
	src/events.c
	src/main.c
	src/matrix.c
	src/combo.c
	src/keymap.c
	src/debounce.c
	src/backlight.c
	src/usb/usb.c
	src/usb/usb_midi.c              # ← ADD THIS
	src/usb/usb_midi_descriptors.c # ← ADD THIS
	src/usb/usb_midi_integration.c # ← ADD THIS
	src/usb/hid.c
	src/rgb_underglow.c
	src/ble.c
	src/battery.c
	src/split/central.c
	src/split/peripheral.c
	src/split/bluetooth.c
)
```

### Step 5: Build the Firmware

Navigate to the MIDI keyboard config and build:

```bash
cd ~/zmk-configs/zmk-midi-keyboard

# Build for nice_nano_v2
west build -b nice_nano_v2 --pristine always

# Output should end with:
# [100%] Linking C executable zephyr/zmk.elf
# [100%] Built target zwsk
```

### Step 6: Flash to Your Board

#### Option A: Using UF2 (nice_nano_v2)
```bash
# Copy UF2 file
cp build/zephyr/zmk.uf2 /mnt/NICENANO/

# Or use west
west flash
```

#### Option B: Using nrfjprog
```bash
# Flash the hex file
nrfjprog -f nrf52 --program build/zephyr/zmk.hex
nrfjprog -f nrf52 --reset
```

### Step 7: Verify Device

Check if MIDI device appears:

```bash
# USB device enumeration
lsusb | grep 2341:0249
# Should show: ID 2341:0249 Arduino ZMK MIDI Keyboard

# MIDI device enumeration (Linux)
aconnect -l
# Should show: ZMK MIDI Keyboard

# Monitor MIDI output
aseqdump -p 24:0  # Port number may vary
```

## Configuration Options

### MIDI Settings (config/midi_keyboard.conf)

```conf
# MIDI Channel (1-16)
CONFIG_ZMK_USB_MIDI_DEFAULT_CHANNEL=1

# Default note velocity (0-127)
CONFIG_ZMK_USB_MIDI_DEFAULT_VELOCITY=64

# Enable composite USB device (keyboard + MIDI)
CONFIG_ZMK_USB_MIDI_COMPOSITE_DEVICE=y

# USB device strings
CONFIG_USB_DEVICE_PRODUCT="ZMK MIDI Keyboard"
CONFIG_USB_DEVICE_MANUFACTURER="ZMK Team"
```

### Debug & Testing

Enable these for development:

```conf
# Shell commands for testing (uart shell)
CONFIG_ZMK_USB_MIDI_SHELL=y

# Debug logging
CONFIG_ZMK_USB_MIDI_DEBUG=y
LOG_LEVEL=4

# Serial console
CONFIG_SERIAL=y
CONFIG_UART_CONSOLE=y
```

### Bluetooth (Optional)

To also enable Bluetooth MIDI:

```conf
CONFIG_ZMK_BLE=y
CONFIG_ZMK_SPLIT=n  # Unless using split keyboard
```

## Keymap Customization

### Default Layout (config/midi_keyboard.keymap)

The default keymap has 3 layers:

1. **MIDI Layer**: C4-C6 chromatic scale
2. **DRUMS Layer**: C1-C3 for drum pads
3. **CHROMATIC Layer**: Includes all sharps/flats

### Creating Your Own Layout

Edit `config/midi_keyboard.keymap`:

```c
/ {
    keymap {
        compatible = "zmk,keymap";

        my_scale {
            label = "Scale";
            bindings = <
                &midi_note C4  &midi_note D4  &midi_note E4  &midi_note F4
                &midi_note G4  &midi_note A4  &midi_note B4  &midi_note C5
                &midi_note C5  &midi_note D5  &midi_note E5  &midi_note F5
                &midi_note G5  &midi_note A5  &midi_note B5  &midi_note C6
            >;
        };
    };
};
```

### Layer Switching

Assign to physical keys (layer modifiers):

```c
&lt 0 C4   // Tap: C4, Hold: Layer 0
&mo 1      // Momentary layer 1
```

## Testing Your MIDI Keyboard

### Method 1: Shell Commands

```bash
# Connect to serial console
picocom /dev/ttyACM0 -b 115200

# Send test MIDI note (if CONFIG_ZMK_USB_MIDI_SHELL=y)
uart:~$ midi test 60
# Response: "MIDI test note 60 sent"

uart:~$ midi cc 7 100
# Response: "MIDI CC 7 = 100 sent"
```

### Method 2: DAW Integration

**Ableton Live:**
1. Preferences → MIDI Sync
2. Inputs: Enable "ZMK MIDI Keyboard"
3. Create MIDI track, select it as input
4. Press keys → should see notes

**Logic Pro:**
1. Window → Show Library
2. MIDI Library → should list device
3. Create software instrument track
4. Should auto-detect USB MIDI input

**FL Studio:**
1. Options → MIDI Settings
2. Find "ZMK MIDI Keyboard"
3. Set as input device
4. Press keys → should trigger samples

### Method 3: MIDI Monitor

**Linux:**
```bash
# Install if needed
sudo apt install alsa-utils

# Monitor MIDI
aconnect -i        # Find input port
aseqdump -p 24:0   # Monitor port 24:0 (may vary)

# Should see MIDI messages when you press keys
```

**macOS:**
1. Open Audio MIDI Setup (Applications → Utilities)
2. Look for "ZMK MIDI Keyboard" in device list
3. Or use MIDI Monitor app from Cycling '74

**Windows:**
1. Download MIDI Monitor or Loopmidi
2. Select USB MIDI input
3. Should show incoming MIDI messages

## Troubleshooting

### Build Issues

**Error: `usb_midi.h: No such file`**
- Ensure you copied `usb_midi.h` to `~/zmk/app/include/zmk/`
- Check file exists: `ls ~/zmk/app/include/zmk/usb_midi.h`

**Error: `CMake` compilation failures**
- Use `--pristine` flag: `west build -b nice_nano_v2 --pristine always`
- Verify all three USB MIDI source files are copied

**Error: `CONFIG_ZMK_USB_MIDI` not recognized**
- Check you copied `Kconfig` to `~/zmk/app/src/usb/Kconfig.usb_midi`
- Verify `CMakeLists.txt` includes new sources

### Runtime Issues

**Device doesn't show in `lsusb`**
- Check USB cable is data-capable (test with another device)
- Verify firmware built successfully
- Re-flash with `--pristine` build

**MIDI output not showing**
- Enable debug: `CONFIG_ZMK_USB_MIDI_DEBUG=y`
- Check serial console: `picocom /dev/ttyACM0 -b 115200`
- Test with: `uart:~$ midi test 60`
- Verify MIDI software sees the device: `aconnect -l`

**Device disconnects repeatedly**
- Use higher quality USB cable
- Ensure adequate power supply
- Check for loose connections

### Software Integration Issues

**DAW doesn't see MIDI input**
- Refresh MIDI device list (often requires restart)
- Check OS recognizes device: `lsusb | grep 2341`
- Verify MIDI software permits USB input
- Try different MIDI routing software (Loopmidi on Windows, etc.)

## File Organization Reference

### Key Files You Edit

1. **Keyboard Layout**: `config/midi_keyboard.keymap`
   - Define which keys send which MIDI notes
   - Support multiple layers

2. **Build Config**: `config/midi_keyboard.conf`
   - MIDI channel, velocity, device names
   - Enable/disable features

3. **ZMK Config**: `config/west.yml`
   - Specifies ZMK version
   - Usually don't need to edit

### Key Files You Don't Edit (Handled by ZMK)

- `boards/` - Empty, ZMK provides nice_nano_v2
- `build/` - Generated during build
- `zephyr/` - Module configuration

### Files from zmk_usb_midi (Copy Once)

- `app/include/zmk/usb_midi.h` → `~/zmk/app/include/zmk/`
- `app/src/usb/*.c` → `~/zmk/app/src/usb/`
- `app/src/events/midi_event.h` → `~/zmk/app/src/events/`

## Next Steps

1. ✅ **Created**: ZMK MIDI keyboard board repo
2. **Next**: Follow "Step 1-7" above to complete integration
3. **Then**: Customize `config/midi_keyboard.keymap` for your layout
4. **Finally**: Test with your DAW or MIDI software

## Documentation Files

- **README.md** - Full feature documentation
- **QUICK_START.md** - 5-minute quick reference
- **MIDI_NOTE_REFERENCE.md** - Complete MIDI note chart
- **SETUP_GUIDE.md** - This comprehensive setup guide

## Support Resources

### ZMK
- Official: https://zmk.dev/
- Docs: https://zmk.dev/docs/
- GitHub: https://github.com/zmkfirmware/zmk

### USB MIDI Implementation
- See: `~/zmk-configs/zmk_usb_midi/TECHNICAL_REFERENCE.md`
- API Docs: `~/zmk-configs/zmk_usb_midi/API_SPECIFICATION.md`

### This Repository
- Issues/Questions: Check README.md troubleshooting section

---

**You're ready to build your ZMK MIDI keyboard!** 🎹🎵

Start with Step 1 above and follow through. Feel free to customize the keymap and configuration to match your preferences.
