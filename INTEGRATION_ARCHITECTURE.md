# ZMK MIDI Keyboard - Integration Architecture

## Overview

The `zmk-midi-keyboard` repository is now **fully self-contained** with the USB MIDI implementation embedded directly. This eliminates external dependencies and makes building simpler.

## Repository Structure

```
zmk-midi-keyboard/                    ← Your keyboard repo
├── Documentation
│   ├── README.md                      # Full docs
│   ├── SETUP_GUIDE.md                # Setup steps
│   ├── MIDI_NOTE_REFERENCE.md        # MIDI notes
│   ├── INTEGRATION_ARCHITECTURE.md   # This file
│   └── COMPLETION_CHECKLIST.md
│
├── Configuration (Build System)
│   ├── west.yml                       # Zephyr workspace manifest
│   ├── build.yaml                     # GitHub Actions matrix
│   └── zephyr/module.yml              # Board root configuration
│
├── Application Code (NEW)
│   └── app/
│       ├── CMakeLists.txt             # Build configuration
│       ├── Kconfig                    # Configuration options
│       ├── prj.conf                   # Default build settings
│       ├── midi_keyboard.keymap       # Keyboard layout (customizable)
│       │
│       ├── include/zmk/
│       │   └── usb_midi.h             # USB MIDI API header
│       │
│       └── src/
│           ├── usb/
│           │   ├── usb_midi.c         # Core MIDI implementation
│           │   ├── usb_midi_descriptors.c   # USB descriptors
│           │   └── usb_midi_integration.c   # ZMK event integration
│           │
│           └── events/
│               └── midi_event.h       # MIDI event definitions
│
├── Keyboard Layout Configuration
│   ├── config/midi_keyboard.keymap    # Duplicate of app/midi_keyboard.keymap
│   ├── config/midi_keyboard.conf      # Additional MIDI settings
│   └── config/west.yml                # Imports app/ as main project
│
└── Board Definitions
    └── boards/                        # Empty (uses nice_nano_v2 from ZMK)
```

## How It Works

### 1. **Workspace Structure**

When you run `west init`, it creates this layout:

```
~/zmk-configs/
├── zmk-midi-keyboard/                ← This repo (self.path: config)
│   ├── config/                        ← West manifest points here
│   └── app/                           ← Your app code (imported from zmk/app/west.yml)
│
├── zmk/                               ← Downloaded from github.com/zmkfirmware/zmk
│   ├── app/                           ← Main ZMK application
│   ├── bootloader/
│   └── docs/
│
└── (other repos imported by zmk/app/west.yml)
```

### 2. **Build Process**

```bash
cd ~/zmk-configs/zmk-midi-keyboard

# Initialize workspace (if not already done)
west init -l config

# Build firmware
west build -b nice_nano_v2 --pristine always

# The build system automatically:
# 1. Reads config/west.yml
# 2. Clones/updates ZMK firmware to ../zmk/
# 3. Imports app/ as the application
# 4. Compiles app/src/usb/*.c into firmware
# 5. Applies app/midi_keyboard.keymap
# 6. Outputs firmware to build/zephyr/zmk.uf2
```

### 3. **Configuration Hierarchy**

Settings are applied in this order (highest priority last):

1. **Zephyr defaults** (hardcoded in Zephyr)
2. **app/Kconfig** (framework configuration options)
3. **app/prj.conf** (project-level defaults)
4. **config/midi_keyboard.conf** (keyboard-specific overrides)
5. **Command-line flags** (highest priority)

### 4. **Integrated USB MIDI Module**

The USB MIDI implementation is now part of `app/` and includes:

**Headers:**
- `app/include/zmk/usb_midi.h` - Public API

**Implementation:**
- `app/src/usb/usb_midi.c` - Core MIDI functions
- `app/src/usb/usb_midi_descriptors.c` - USB device descriptors
- `app/src/usb/usb_midi_integration.c` - ZMK event system integration

**Events:**
- `app/src/events/midi_event.h` - MIDI event definitions

## Advantages of This Structure

### ✅ Self-Contained
- No external copying required
- Everything needed to build is in the repo
- No manual integration steps

### ✅ Easy to Maintain
- Changes to USB MIDI don't require updating multiple locations
- Clear ownership of files
- Single source of truth

### ✅ Simple Build Process
```bash
git clone https://github.com/your-user/zmk-midi-keyboard
cd zmk-midi-keyboard
west init -l config
west build -b nice_nano_v2 --pristine always
west flash
```

### ✅ IDE-Friendly
- All source code visible in repository
- Proper include paths configured
- No "magic" external dependencies

### ✅ Extensible
- Easy to add more app-level code
- Can customize USB MIDI without modifying Zephyr
- Clear separation of concerns

## Configuration Guide

### MIDI Settings (app/prj.conf)

```properties
# USB Configuration
CONFIG_USB_DEVICE_STACK=y
CONFIG_USB_DEVICE_MANUFACTURER="ZMK Team"
CONFIG_USB_DEVICE_PRODUCT="ZMK MIDI Keyboard"
CONFIG_USB_DEVICE_PRODUCT_ID=0x0249
CONFIG_USB_DEVICE_VID=0x2341

# USB MIDI Configuration
CONFIG_ZMK_USB_MIDI=y
CONFIG_ZMK_USB_MIDI_DEFAULT_CHANNEL=1
CONFIG_ZMK_USB_MIDI_DEFAULT_VELOCITY=64
CONFIG_ZMK_USB_MIDI_COMPOSITE_DEVICE=y
```

### Keyboard Layout (app/midi_keyboard.keymap)

Edit this file to customize your keyboard layout:

```c
keymap {
    compatible = "zmk,keymap";
    
    midi_layer {
        label = "MIDI";
        bindings = <
            &midi_note C4   &midi_note D4   &midi_note E4   &midi_note F4
            &midi_note G4   &midi_note A4   &midi_note B4   &midi_note C5
            // ... more keys
        >;
    };
};
```

### Per-Keyboard Settings (config/midi_keyboard.conf)

Override settings specific to this keyboard:

```properties
# Optional: Keyboard name
CONFIG_ZMK_KEYBOARD_NAME="My MIDI Keyboard"

# Optional: Enable debug logging
# CONFIG_ZMK_USB_MIDI_DEBUG=y
```

## Source of Truth

⚠️ **Important**: Keep `app/midi_keyboard.keymap` and `config/midi_keyboard.keymap` synchronized. They serve different purposes:

- **app/midi_keyboard.keymap** - Used during compilation
- **config/midi_keyboard.keymap** - Reference copy for documentation

## Related Repositories

This repo is part of a larger ecosystem:

- **zmk-usb-midi/** - Source of the USB MIDI implementation (upstream)
  - Contains original implementation and documentation
  - Used as reference for updates
  
- **zmk-midi-keyboard/** - This repo (self-contained application)
  - Includes embedded copy of USB MIDI implementation
  - Ready-to-build keyboard configuration
  
- **zmk/** - Official ZMK firmware (git submodule via west)
  - Core ZMK firmware
  - Matrix scanning, debouncing, event system
  - Automatically fetched by west

## Updating USB MIDI Implementation

If you need to update the USB MIDI code:

1. Update source in `zmk-usb-midi/` (upstream)
2. Copy updated files:
   ```bash
   cp ~/zmk-configs/zmk_usb_midi/app/src/usb/*.c \
      ~/zmk-configs/zmk-midi-keyboard/app/src/usb/
   cp ~/zmk-configs/zmk_usb_midi/app/include/zmk/usb_midi.h \
      ~/zmk-configs/zmk-midi-keyboard/app/include/zmk/
   ```
3. Rebuild:
   ```bash
   cd ~/zmk-configs/zmk-midi-keyboard
   west build -b nice_nano_v2 --pristine always
   ```

## Build Architecture

```
┌─────────────────────────────────────────┐
│  zmk-midi-keyboard Repository           │
│  ┌─────────────────────────────────────┐│
│  │  app/ (Your Application)            ││
│  │  ┌──────────────────────────────┐  ││
│  │  │ USB MIDI Implementation      │  ││
│  │  │ - usb_midi.c                 │  ││
│  │  │ - usb_midi_descriptors.c     │  ││
│  │  │ - usb_midi_integration.c     │  ││
│  │  └──────────────────────────────┘  ││
│  │  ┌──────────────────────────────┐  ││
│  │  │ Configuration                │  ││
│  │  │ - Kconfig                    │  ││
│  │  │ - prj.conf                   │  ││
│  │  │ - CMakeLists.txt             │  ││
│  │  └──────────────────────────────┘  ││
│  │  ┌──────────────────────────────┐  ││
│  │  │ Keyboard Definitions         │  ││
│  │  │ - midi_keyboard.keymap       │  ││
│  │  └──────────────────────────────┘  ││
│  └─────────────────────────────────────┘│
│  ┌─────────────────────────────────────┐│
│  │  config/ (Build Configuration)      ││
│  │  - west.yml (manifests app/)        ││
│  │  - midi_keyboard.conf (overrides)   ││
│  │  - zephyr/module.yml (board root)   ││
│  └─────────────────────────────────────┘│
└─────────────────────────────────────────┘
         │
         │ west build -b nice_nano_v2
         │
         ▼
┌─────────────────────────────────────────┐
│  ZMK Firmware (auto-fetched via west)   │
│  ├── Core event system                  │
│  ├── Matrix scanning                    │
│  ├── USB HID (if composite device)      │
│  └── Bootloader                         │
└─────────────────────────────────────────┘
         │
         │ Cmake + Zephyr build system
         │
         ▼
┌─────────────────────────────────────────┐
│  Compiled Firmware (zmk.uf2)            │
│  - USB MIDI stack                       │
│  - Your keymap                          │
│  - All configuration                    │
└─────────────────────────────────────────┘
```

## Troubleshooting

### "Command not found: west"
```bash
pip3 install west
```

### "Cannot find usb_midi.h"
Ensure `app/include/zmk/usb_midi.h` exists and CMakeLists.txt includes:
```cmake
target_include_directories(app PRIVATE include)
```

### "Kconfig not found"
Check that `app/Kconfig` exists and `CMakeLists.txt` references it.

### Build fails with USB errors
Ensure in `app/prj.conf`:
```properties
CONFIG_USB_DEVICE_STACK=y
CONFIG_ZMK_USB_MIDI=y
```

## References

- [ZMK Documentation](https://zmk.dev/docs)
- [Zephyr Build System](https://docs.zephyrproject.org/latest/build/index.html)
- [West Workspace Management](https://docs.zephyrproject.org/latest/guides/west/index.html)
- [USB MIDI 1.0 Specification](https://www.usb.org/sites/default/files/midi10.pdf)

---

**Summary**: This architecture provides a complete, self-contained ZMK MIDI keyboard that's easy to build, maintain, and customize.
