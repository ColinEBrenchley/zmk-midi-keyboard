# ZMK MIDI Keyboard - Build Instructions

## Build Commands

### With Zephyr SDK installed globally:
```bash
west build -b nice_nano -s zmk/app --pristine always
```

### With local Zephyr SDK (if not installed system-wide):
```bash
export ZEPHYR_SDK_INSTALL_DIR=/path/to/zephyr-sdk-0.16.8
west build -b nice_nano -s zmk/app --pristine always
```

## Custom Configuration Files

Your keyboard configurations are stored in your local `config/` directory:

- **config/midi_keyboard.conf** - Build configuration (USB settings, etc.)
- **config/midi_keyboard.keymap** - Keymap definitions (reference copy)
- **config/boards/nicekeyboards/nice_nano/midi_keyboard.keymap** - Active keymap
- **config/boards/nicekeyboards/nice_nano/nice_nano_2_0_0.overlay** - Hardware matrix configuration

The active files used during build are in:
- `zmk/app/boards/nicekeyboards/nice_nano/midi_keyboard.keymap`
- `zmk/app/boards/nicekeyboards/nice_nano/nice_nano_2_0_0.overlay`

These files are mirrors of your config/ directory files for compatibility with ZMK's build system.

## Output

Build artifacts are generated in `build/zephyr/`:
- `zmk.uf2` - Firmware file ready to flash to nice!nano
- `zmk.hex` - Hex format (alternative)
- `zmk.bin` - Binary format (alternative)
