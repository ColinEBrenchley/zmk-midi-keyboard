# ZMK MIDI Keyboard - Setup Guide

## Complete Setup & Integration (Simplified)

### What's Different Now

✨ **You no longer need to:**
- Manually copy files from zmk_usb_midi
- Edit multiple CMakeLists.txt files
- Set up external dependencies

✨ **Everything is now:**
- Self-contained in this repository
- In the `app/` directory
- Ready to build immediately

---

## Complete Setup Process

### Step 1: Install Prerequisites

**macOS:**
```bash
brew install cmake ninja python3
pip3 install west
```

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake ninja-build python3-pip
pip3 install west
```

**Windows (with WSL2):**
```bash
# In WSL2 Ubuntu terminal:
sudo apt-get install cmake ninja-build python3-pip
pip3 install west
```

### Step 2: Clone the Repository

```bash
cd ~/zmk-configs  # or your preferred location
git clone https://github.com/zmkfirmware/zmk-midi-keyboard.git
cd zmk-midi-keyboard
```

### Step 3: Initialize West Workspace

This command clones/updates ZMK firmware in the parent directory.

```bash
west init -l config
west update
```

**Result:**
```
~/zmk-configs/
├── zmk/                      ← ZMK firmware (auto-cloned)
│   ├── app/
│   ├── bootloader/
│   └── ...
│
└── zmk-midi-keyboard/        ← This repo
    ├── app/
    ├── config/
    └── ...
```

### Step 4: Verify Structure

```bash
# Check that files were set up correctly
ls -la app/
# Should show: CMakeLists.txt, Kconfig, prj.conf, src/, include/

ls -la config/
# Should show: west.yml, midi_keyboard.conf, zephyr/

ls -la app/src/usb/
# Should show: usb_midi.c, usb_midi_descriptors.c, usb_midi_integration.c
```

### Step 5: Configure for Your Board

The default board is `nice_nano_v2`. To use a different board:

**Edit `build.yaml`:**
```yaml
include:
  - board: nrf52840_dk     # Change this line
```

### Step 6: Build Firmware

```bash
# Clean build (recommended first time)
west build -b nice_nano_v2 --pristine always

# Or incremental build (faster for small changes)
west build -b nice_nano_v2
```

**What happens:**
1. CMake generates build files
2. Zephyr build system compiles:
   - USB MIDI implementation (app/src/usb/*.c)
   - Your keymap (app/midi_keyboard.keymap)
   - ZMK firmware (from ../zmk/)
3. Output: `build/zephyr/zmk.uf2`

**Successful build output:**
```
-- Zephyr version: 3.x.x
-- Found west (find-package version): ...
-- Configuration files:
...
[100%] Linking C executable zephyr/zmk.elf
[100%] Built target zephyr_final
```

### Step 7: Flash Firmware

#### For nice_nano_v2 (UF2 Bootloader)

**Method 1: Manual UF2 Copy**
```bash
# 1. Press BOOT button on nice_nano_v2 while plugging in USB
# 2. Wait for NICENANO drive to appear
# 3. Copy firmware:
cp build/zephyr/zmk.uf2 /media/<username>/NICENANO/
# (Replace <username> with your Linux username)
# Device auto-resets and starts running

# On macOS:
cp build/zephyr/zmk.uf2 /Volumes/NICENANO/

# On Windows (WSL):
cp build/zephyr/zmk.uf2 /mnt/d/NICENANO/
# (Adjust drive letter as needed)
```

**Method 2: Using west (Faster)**
```bash
west flash
```
(May prompt to enter bootloader mode)

### Step 8: Verify Installation

```bash
# Check USB enumeration
lsusb | grep 2341:0249
# Output should be:
# Bus 001 Device 005: ID 2341:0249 Arduino LLC

# If not showing, try:
lsusb -d 2341:0249 -v  # Verbose output
```

### Step 9: Test MIDI Output

**Linux:**
```bash
# List MIDI ports
aconnect -l

# Monitor MIDI output (replace port with yours)
aseqdump -p 24:0

# Press keys on your keyboard
# You should see MIDI note messages like:
# 144:0  Note on, ch 1, note 60, velocity 64
```

**macOS:**
```bash
# Use Audio MIDI Setup.app (built-in)
# Or command line:
# Check MIDI ports with: system_profiler SPAudioDataType
```

**Windows:**
```bash
# Use MIDI Monitor from Microsoft Store
# Or DAW's MIDI input monitor
```

### Step 10: Use in Your DAW

1. Open your DAW (Ableton, FL Studio, Logic Pro, etc.)
2. Go to MIDI Input settings
3. Select "ZMK MIDI Keyboard" (or "Arduino LLC")
4. Press keys - MIDI notes should appear in your DAW
5. Enjoy your new MIDI controller!

---

## Configuration & Customization

### Change Your Keyboard Layout

Edit `app/midi_keyboard.keymap`:

**Default (4x4 chromatic):**
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

After editing, rebuild:
```bash
west build -b nice_nano_v2
west flash
```

### Change MIDI Settings

Edit `app/prj.conf`:

```properties
# MIDI channel (1-16, default 1)
CONFIG_ZMK_USB_MIDI_DEFAULT_CHANNEL=1

# Default velocity (0-127, default 64)
CONFIG_ZMK_USB_MIDI_DEFAULT_VELOCITY=64

# USB device name
CONFIG_USB_DEVICE_PRODUCT="My MIDI Keyboard"

# Enable debug logs (for troubleshooting)
# CONFIG_ZMK_USB_MIDI_DEBUG=y

# Enable shell commands (for advanced testing)
# CONFIG_ZMK_USB_MIDI_SHELL=y
```

Then rebuild:
```bash
west build -b nice_nano_v2 --pristine always
```

### Create Multiple Layers

Edit `app/midi_keyboard.keymap` to add layers:

```c
keymap {
    layer_0 {
        label = "Chromatic";
        bindings = <
            &midi_note C4  &midi_note D4  &midi_note E4  &midi_note F4
            // ... rest of layout
        >;
    };
    
    layer_1 {
        label = "Drums";
        bindings = <
            &midi_note C1  &midi_note D1  &midi_note E1  &midi_note F1
            // ... drum pad notes
        >;
    };
};
```

Then rebuild and flash.

---

## Troubleshooting

### Build Errors

**"Command not found: west"**
```bash
pip3 install west
```

**"Cannot find usb_midi.h"**
- Verify `app/include/zmk/usb_midi.h` exists
- Check paths in `app/CMakeLists.txt`
- Try clean rebuild: `west build --pristine always`

**"USB_DEVICE_STACK not found"**
- Ensure `CONFIG_USB_DEVICE_STACK=y` in `app/prj.conf`
- Run: `west build --pristine always`

### Flash Errors

**"Permission denied"**
```bash
# Linux: Run with sudo
sudo cp build/zephyr/zmk.uf2 /media/NICENANO/

# Or add user to group
sudo usermod -a -G dialout $USER
```

**Device won't reset to bootloader**
- Try holding BOOT while pressing RESET
- Or power cycle: unplug USB, wait 2s, plug back in

### MIDI Not Working

**Device shows in lsusb but MIDI not received**

1. Check MIDI port is correct:
   ```bash
   aconnect -l | grep -i zmk
   ```

2. Try system MIDI manager:
   ```bash
   # Install if needed
   sudo apt-get install qjackctl  # Has MIDI monitor
   ```

3. Enable debug logging:
   - Edit `app/prj.conf`
   - Uncomment `CONFIG_ZMK_USB_MIDI_DEBUG=y`
   - Rebuild with: `west build --pristine always`
   - Flash and check serial output:
     ```bash
     miniterm.py /dev/ttyACM0 115200
     ```

**MIDI received but wrong notes**
- Verify note names in `app/midi_keyboard.keymap`
- Check MIDI note reference in [MIDI_NOTE_REFERENCE.md](MIDI_NOTE_REFERENCE.md)
- Rebuild with changes

### Performance Issues

**Latency too high?**

1. Check USB endpoint size in `app/Kconfig`:
   - Larger = better throughput, more latency
   - Default 64 bytes is good for MIDI

2. Check thread priority in `app/prj.conf`:
   - Can adjust `CONFIG_ZMK_USB_MIDI_THREAD_PRIORITY`

3. Disable debugging:
   - Comment out `CONFIG_ZMK_USB_MIDI_DEBUG=y`

---

## File Organization

| File | Purpose | Edit? |
|------|---------|-------|
| `app/midi_keyboard.keymap` | Keyboard layout | ✏️ Yes |
| `app/prj.conf` | Build defaults | ✏️ Yes |
| `app/src/usb/usb_midi.c` | Core MIDI | 🔒 Rarely |
| `app/CMakeLists.txt` | Build config | 🔒 Usually no |
| `config/west.yml` | Workspace setup | 🔒 Usually no |
| `config/midi_keyboard.conf` | Keyboard settings | ✏️ Yes |

---

## Next Steps

### Build Your Hardware

You'll need:
- nice_nano_v2 or compatible nRF52840 board
- 16 switches for 4x4 matrix
- Microcontroller USB cable
- Diodes (optional, for complex layouts)

### Customize Your Layout

Edit `app/midi_keyboard.keymap` with your own note arrangements.

### Add More Features

- Multiple layers with layer switching
- Control Change (CC) messages for parameters
- Different modes (chromatic, drum pads, etc.)

### Integrate with DAW

- Ableton: MIDI Preferences → Inputs
- Logic Pro: Add instrument → Select MIDI input
- FL Studio: Controllers → MIDI Settings

---

## Support

For help:
1. Check [MIDI_NOTE_REFERENCE.md](MIDI_NOTE_REFERENCE.md) for all available notes
2. See [INTEGRATION_ARCHITECTURE.md](INTEGRATION_ARCHITECTURE.md) for technical details
3. Visit [ZMK Discord](https://discord.gg/8nzrMbEKWX) for community support

---

**You're all set! Enjoy your new MIDI keyboard! 🎹**
