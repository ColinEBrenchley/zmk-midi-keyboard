# ZMK MIDI Keyboard

A complete, self-contained USB MIDI keyboard controller for the nRF52840 (nice_nano_v2).

## Overview

This repository lets you build a fully functional USB MIDI controller using ZMK firmware. Everything is included - no external copying or manual integration required.

**Key features:**
- ✅ USB MIDI 1.0 compliant
- ✅ Composite USB device (HID keyboard + MIDI simultaneously)
- ✅ 4x4 customizable keyboard layout
- ✅ Multiple keymap layers
- ✅ Full MIDI note range (0-127)
- ✅ Configurable MIDI channels (1-16)
- ✅ Self-contained (all code included)
- ✅ Ready to build in minutes

## Architecture

**New Integration (No Manual Copying!)**: This repository now includes the complete USB MIDI implementation in the `app/` directory. Just clone, build, and flash.

```
zmk-midi-keyboard/
├── app/                    ← Complete USB MIDI implementation
│   ├── src/usb/           # USB MIDI core (3 files)
│   ├── include/zmk/       # Public API header
│   ├── src/events/        # MIDI event definitions
│   ├── Kconfig            # Configuration options
│   ├── prj.conf           # Build defaults
│   ├── CMakeLists.txt     # Build system
│   └── midi_keyboard.keymap   # Your keyboard layout
│
├── config/                 ← Build configuration
│   ├── west.yml           # Workspace manifest
│   ├── midi_keyboard.conf # Keyboard-specific settings
│   └── zephyr/module.yml
│
└── Documentation
    ├── README.md          # This file
    ├── QUICK_START.md     # 5-minute setup
    ├── SETUP_GUIDE.md     # Detailed guide
    ├── INTEGRATION_ARCHITECTURE.md  # Technical details
    └── MIDI_NOTE_REFERENCE.md
```

See [INTEGRATION_ARCHITECTURE.md](INTEGRATION_ARCHITECTURE.md) for details on how the build system works.

## Quick Start

### 1. Prerequisites

```bash
# Install required tools
pip3 install west
```

### 2. Clone & Initialize

```bash
git clone https://github.com/zmkfirmware/zmk-midi-keyboard.git
cd zmk-midi-keyboard
west init -l config
west update
```

### 3. Build Firmware

```bash
west build -b nice_nano_v2 --pristine always
```

Output: `build/zephyr/zmk.uf2`

### 4. Flash to Board

```bash
# For nice_nano_v2: Hold BOOT button while plugging in USB
# Device appears as NICENANO drive
cp build/zephyr/zmk.uf2 /media/NICENANO/

# Or use west flash
west flash
```

### 5. Verify

```bash
# Check USB enumeration
lsusb | grep 2341:0249
# Should show: Bus XXX Device YYY: ID 2341:0249 Arduino LLC ...

# Monitor MIDI output (Linux)
aconnect -l
# Your device appears in MIDI output list

# Test with DAW
# - Open your DAW
# - Select "ZMK MIDI Keyboard" as MIDI input
# - Press keys and watch MIDI notes appear
```

## Customization

### Change Your Keyboard Layout

Edit `app/midi_keyboard.keymap`:

```c
keymap {
    midi_layer {
        label = "MIDI";
        bindings = <
            &midi_note C4   &midi_note D4   &midi_note E4   &midi_note F4
            &midi_note G4   &midi_note A4   &midi_note B4   &midi_note C5
            &midi_note C5   &midi_note D5   &midi_note E5   &midi_note F5
            &midi_note G5   &midi_note A5   &midi_note B5   &midi_note C6
        >;
    };
};
```

**Common layouts:**

Pentatonic scale (5-note):
```c
&midi_note C4  &midi_note D4  &midi_note E4  &midi_note G4
&midi_note A4  &midi_note C5  &midi_note D5  &midi_note E5
&midi_note G5  &midi_note A5  &midi_note C6  &midi_note D6
&midi_note E6  &midi_note G6  &midi_note A6  &midi_note C7
```

Drum pads (low notes):
```c
&midi_note C1  &midi_note D1  &midi_note E1  &midi_note F1
&midi_note G1  &midi_note A1  &midi_note B1  &midi_note C2
&midi_note C2  &midi_note D2  &midi_note E2  &midi_note F2
&midi_note G2  &midi_note A2  &midi_note B2  &midi_note C3
```

### Change MIDI Settings

Edit `app/prj.conf`:

```properties
# MIDI Channel (1-16)
CONFIG_ZMK_USB_MIDI_DEFAULT_CHANNEL=1

# Default Velocity (0-127)
CONFIG_ZMK_USB_MIDI_DEFAULT_VELOCITY=64

# Device Name
CONFIG_USB_DEVICE_PRODUCT="My MIDI Keyboard"

# Debug Logging (uncomment to enable)
# CONFIG_ZMK_USB_MIDI_DEBUG=y
```

### Advanced Configuration

Edit `app/Kconfig` for more options:
- Thread stack size
- USB endpoint size
- Initialization priority
- Shell commands support

## MIDI Notes Reference

**Format:** `&midi_note [Note][Octave]`

**All notes:**
- Natural: C, D, E, F, G, A, B
- Sharps: CS, DS, FS, GS, AS
- Examples: C4, CS4, D4, DS4, etc.

**Octave ranges:**
| Octave | Range | Notes | Use Case |
|--------|-------|-------|----------|
| 0-1 | 0-23 | Very low | Sub-bass, drums |
| 2-3 | 24-47 | Low | Bass, low synths |
| 4-5 | 60-83 | Mid | Melody, main range |
| 6-7 | 84-107 | High | Lead, bright sounds |
| 8+ | 108-127 | Very high | Percussion |

**Middle C reference:**
- Note: C4
- MIDI number: 60
- Frequency: 261.63 Hz

## File Guide

| File | Purpose | Customize |
|------|---------|-----------|
| `app/midi_keyboard.keymap` | Keyboard layout | ✏️ Yes |
| `app/prj.conf` | Build settings | ✏️ Yes |
| `app/Kconfig` | Config options | ⚠️ Advanced |
| `config/midi_keyboard.conf` | Keyboard overrides | ✏️ Yes |
| `app/src/usb/*.c` | USB MIDI impl | 🔒 Usually no |

## USB Device Details

```
Vendor ID (VID):  0x2341 (Arduino)
Product ID (PID): 0x0249 (configurable)
Class:            Audio (0x01)
Subclass:         MIDI Streaming (0x03)
Endpoints:        Bulk IN (0x81), OUT (0x01)
Max Packet Size:  64 bytes
Power:            500 mA USB powered
```

## Troubleshooting

### Device doesn't appear in `lsusb`

1. Check USB cable is data cable (try different cable)
2. Verify nice_nano_v2 bootloader is working
   - Plug in USB while holding BOOT button
   - LED should flash
3. Check USB port is functional

### Device appears but MIDI input not working

1. Verify in system MIDI list
   ```bash
   aconnect -l
   ```
2. Check DAW has correct MIDI input selected
3. Enable debug logging:
   ```bash
   # Edit app/prj.conf, uncomment:
   # CONFIG_ZMK_USB_MIDI_DEBUG=y
   # Rebuild and check serial console output
   ```

### Build fails

1. Ensure west is installed:
   ```bash
   pip3 install west
   ```
2. Clear build cache:
   ```bash
   west build -b nice_nano_v2 --pristine always
   ```
3. Check CMakeLists.txt includes USB MIDI sources

### Keymap syntax error

1. Check file encoding is UTF-8
2. Verify no typos in MIDI note names
3. Ensure proper comma separation between notes

## Documentation

- **[QUICK_START.md](QUICK_START.md)** - 5-minute setup overview
- **[SETUP_GUIDE.md](SETUP_GUIDE.md)** - Detailed step-by-step guide
- **[INTEGRATION_ARCHITECTURE.md](INTEGRATION_ARCHITECTURE.md)** - Technical build architecture
- **[MIDI_NOTE_REFERENCE.md](MIDI_NOTE_REFERENCE.md)** - Complete MIDI note chart (all 128 notes)

## References

- [ZMK Documentation](https://zmk.dev/)
- [USB MIDI 1.0 Specification](https://www.usb.org/sites/default/files/midi10.pdf)
- [Zephyr Project](https://www.zephyrproject.org/)

## Hardware Support

| Board | Support | Notes |
|-------|---------|-------|
| nice_nano_v2 | ✅ Full | Recommended, tested |
| nice_nano | ✅ Full | Older variant, still works |
| nRF52840 DK | ✅ Full | Development kit, more expensive |
| Other nRF52840 | ✅ Likely | May need custom board definition |

## License

This project is part of ZMK and follows the same license. See LICENSE file for details.

---

**Ready to build your MIDI controller?**

→ Start with [QUICK_START.md](QUICK_START.md) for a 5-minute setup
→ Or jump to [SETUP_GUIDE.md](SETUP_GUIDE.md) for detailed instructions
→ For technical details, see [INTEGRATION_ARCHITECTURE.md](INTEGRATION_ARCHITECTURE.md)
