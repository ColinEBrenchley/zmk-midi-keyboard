# ZMK MIDI Keyboard

A USB MIDI controller built with ZMK firmware on an nRF52840 microcontroller (nice_nano_v2).

This configuration converts your ZMK keyboard into a fully functional MIDI controller, sending MIDI note messages to any connected DAW or synthesizer.

## Features

✅ USB MIDI 1.0 compliant  
✅ 4x4 keyboard layout (customizable)  
✅ Multiple layers for different note ranges  
✅ Chromatic and natural note modes  
✅ Composite USB device (HID + MIDI simultaneously)  
✅ 127 MIDI channels and notes supported  
✅ Velocity sensitivity  
✅ Channel/CC customization  

## Hardware Requirements

- **Microcontroller**: nice_nano_v2 or nRF52840-based board
- **USB Cable**: High-quality USB-C data cable (not charge-only)
- **Keyboard Matrix**: 4x4 switch matrix (16 switches)
- **Keyswitches**: Any mechanical or membrane switches

## Installation

### Prerequisites

1. ZMK development environment set up
2. nice_nano_v2 bootloader programmed
3. USB cable (data cable, not charge-only)

### Setup Steps

1. **Clone this repository into your zmk-configs**:
   ```bash
   cd ~/zmk-configs
   git clone <this-repo> zmk-midi-keyboard
   cd zmk-midi-keyboard
   ```

2. **Update the MIDI implementation files** (integrate the USB MIDI module):
   - Copy MIDI files from `../zmk_usb_midi/app/include/zmk/usb_midi.h` to ZMK's app/include
   - Copy MIDI source files from `../zmk_usb_midi/app/src/usb/` to ZMK's app/src/usb/
   - Copy MIDI event definitions from `../zmk_usb_midi/app/src/events/midi_event.h` to ZMK's app/src/events/

3. **Build the firmware**:
   ```bash
   west build -b nice_nano_v2
   ```

4. **Flash to your board**:
   ```bash
   # For nice_nano_v2 (uses UF2)
   # Copy build/zephyr/zmk.uf2 to the NICENANO drive that appears
   # Or use: west flash
   ```

## Configuration

### Customize the Keymap

Edit `config/midi_keyboard.keymap` to define your key layout:

```c
keymap {
    midi_layer {
        bindings = <
            &midi_note C4  &midi_note D4  &midi_note E4  &midi_note F4
            /* ... more keys ... */
        >;
    };
};
```

### Available MIDI Notes

All notes from C0 (12) to G8 (127) are supported:

- **C4 = 60** (Middle C - reference note)
- **Octave ranges**:
  - Low: C1-B1 (12-23) - very low, use for drums
  - Mid: C3-B3 (36-47) or C4-B4 (60-71) - common melody range
  - High: C6-B6 (84-95) - bright/lead sounds

### MIDI Note Naming

Use format `&midi_note [Note][Octave]`:

- Natural notes: `C4`, `D4`, `E4`, `F4`, `G4`, `A4`, `B4`
- Sharps: `CS4`, `DS4`, `FS4`, `GS4`, `AS4`
- Example: Middle C = `&midi_note C4`, C# = `&midi_note CS4`

### Configuration Options

Edit `config/midi_keyboard.conf`:

```conf
# Default MIDI channel (1-16)
CONFIG_ZMK_USB_MIDI_DEFAULT_CHANNEL=1

# Default note velocity (0-127)
CONFIG_ZMK_USB_MIDI_DEFAULT_VELOCITY=64

# Enable debug logging
CONFIG_ZMK_USB_MIDI_DEBUG=y

# Enable shell commands for testing
CONFIG_ZMK_USB_MIDI_SHELL=y
```

## Usage

### Connecting to DAW

1. **macOS**: 
   - Open Audio MIDI Setup (Applications → Utilities)
   - Look for "ZMK MIDI Keyboard" in the device list

2. **Windows**:
   - Check Device Manager → Sound devices
   - Should appear as "ZMK MIDI Keyboard"

3. **Linux**:
   ```bash
   aconnect -l  # List MIDI devices
   aseqdump -p [port]  # Monitor MIDI output
   ```

### Testing

1. **Via Shell** (if CONFIG_ZMK_USB_MIDI_SHELL=y):
   ```bash
   uart:~$ midi test 60
   uart:~$ midi cc 7 100  # Control change
   ```

2. **Via MIDI Monitor**:
   - macOS: Audio MIDI Setup or MIDI Monitor app
   - Linux: `aseqdump -p [port]`
   - Windows: MIDI Monitor or Loopmidi

### Example DAW Setup

1. **Ableton Live**:
   - Preferences → MIDI Sync
   - Select "ZMK MIDI Keyboard" as input
   - Create a MIDI track and monitor input

2. **Logic Pro**:
   - Preferences → Audio/MIDI
   - Should auto-detect USB MIDI device

3. **FL Studio**:
   - Options → MIDI Settings
   - Select "ZMK MIDI Keyboard" as input device

## Troubleshooting

### Device Not Appearing

```bash
# Check if device is recognized
lsusb | grep 2341:0249

# If not found:
1. Check USB cable is a data cable (not charge-only)
2. Rebuild with --pristine flag:
   west build -b nice_nano_v2 --pristine always
3. Re-flash the firmware
```

### No MIDI Output

```bash
# Verify MIDI is working:
aseqdump -p [port]  # Press a key - should see output

# Check kernel logs:
dmesg | tail -20

# Enable debug:
# Uncomment CONFIG_ZMK_USB_MIDI_DEBUG=y in midi_keyboard.conf
```

### USB Disconnects

- Verify power supply is sufficient
- Use high-quality USB cable
- Check for loose connections

## Customization Examples

### Pentatonic Scale Layout

```c
pentatonic_layer {
    bindings = <
        &midi_note C4  &midi_note D4  &midi_note E4  &midi_note G4
        &midi_note A4  &midi_note C5  &midi_note D5  &midi_note E5
        &midi_note G5  &midi_note A5  &midi_note C6  &midi_note D6
        &midi_note E6  &midi_note G6  &midi_note A6  &midi_note C7
    >;
};
```

### Drum Pad Layout

```c
drum_layer {
    bindings = <
        &midi_note C1  &midi_note D1  &midi_note E1  &midi_note F1
        &midi_note G1  &midi_note A1  &midi_note B1  &midi_note C2
        &midi_note C2  &midi_note D2  &midi_note E2  &midi_note F2
        &midi_note G2  &midi_note A2  &midi_note B2  &midi_note C3
    >;
};
```

### Chromatic Keyboard

```c
chromatic_layer {
    bindings = <
        &midi_note C3  &midi_note CS3 &midi_note D3  &midi_note DS3
        &midi_note E3  &midi_note F3  &midi_note FS3 &midi_note G3
        &midi_note GS3 &midi_note A3  &midi_note AS3 &midi_note B3
        &midi_note C4  &midi_note CS4 &midi_note D4  &midi_note DS4
    >;
};
```

## MIDI Technical Reference

### Note On/Off

- Pressing a key sends MIDI Note On (velocity 64 by default)
- Releasing sends MIDI Note Off

### Channel

- Default: Channel 1 (configurable in prj.conf)
- Can be changed at runtime via API

### Velocity

- Default: 64 (medium)
- Configurable in prj.conf
- Can implement pressure sensitivity if hardware supports it

## Documentation

For detailed implementation info, see:
- `../zmk_usb_midi/API_SPECIFICATION.md` - Full API reference
- `../zmk_usb_midi/TECHNICAL_REFERENCE.md` - USB MIDI technical details
- `../zmk_usb_midi/BUILD_AND_INTEGRATION.md` - Build system integration

## License

Same as ZMK (MIT License)

## Support

For issues with:
- **ZMK**: See https://zmk.dev/
- **MIDI Implementation**: See `../zmk_usb_midi/` documentation
- **This board config**: File an issue in this repository
