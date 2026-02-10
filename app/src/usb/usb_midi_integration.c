/*
 * Copyright (c) 2026 ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/dt-bindings/zmk/keys.h>

#include "zmk/usb_midi.h"
#include "zmk/keymap.h"
#include "zmk/events/keycode_state_changed.h"

LOG_MODULE_REGISTER(zmk_usb_midi_integration);

/**
 * @file usb_midi_integration.c
 * @brief ZMK Integration for USB MIDI
 *
 * This file integrates USB MIDI with ZMK's keypress event system.
 * It provides:
 * - MIDI keycode definitions
 * - Event listener for keypress/release
 * - Conversion from ZMK keycodes to MIDI note numbers
 * - Configuration management for MIDI settings
 */

/**
 * @defgroup midi_keycodes MIDI Keycode Definitions
 * @{
 */

/*
 * ZMK Keycode ranges are typically:
 * 0x0000-0x00FF: Basic keycodes
 * 0x0100-0x01FF: Modifier combinations
 * 0x0200-0x0FFF: Reserved/application specific
 * 
 * For MIDI, we can use the application-specific range.
 * Here we define MIDI note keycodes.
 */

#define MIDI_NOTE_BASE 0x4000
#define MIDI_NOTE_C0   (MIDI_NOTE_BASE + 0)
#define MIDI_NOTE_D0   (MIDI_NOTE_BASE + 2)
#define MIDI_NOTE_E0   (MIDI_NOTE_BASE + 4)
#define MIDI_NOTE_F0   (MIDI_NOTE_BASE + 5)
#define MIDI_NOTE_G0   (MIDI_NOTE_BASE + 7)
#define MIDI_NOTE_A0   (MIDI_NOTE_BASE + 9)
#define MIDI_NOTE_B0   (MIDI_NOTE_BASE + 11)

/* Middle C and common octaves */
#define MIDI_NOTE_C4   (MIDI_NOTE_BASE + 60)  /* Middle C */
#define MIDI_NOTE_A4   (MIDI_NOTE_BASE + 69)  /* Concert A */

/**@}*/

/**
 * @defgroup midi_config MIDI Configuration State
 * @{
 */

/* Runtime MIDI configuration */
static struct {
    uint8_t default_channel;
    uint8_t default_velocity;
    bool enabled;
} midi_config = {
    .default_channel = 1,
    .default_velocity = MIDI_VELOCITY_DEFAULT,
    .enabled = IS_ENABLED(CONFIG_ZMK_USB_MIDI),
};

/**@}*/

/**
 * @defgroup midi_conversion Keycode to MIDI Conversion
 * @{
 */

/**
 * @brief Convert ZMK keycode to MIDI note number
 *
 * This function maps ZMK MIDI keycodes to actual MIDI note numbers.
 * For non-MIDI keycodes, returns UINT8_MAX.
 *
 * @param keycode ZMK keycode
 *
 * @return MIDI note number (0-127), or UINT8_MAX if not a MIDI keycode
 */
static uint8_t keycode_to_midi_note(uint16_t keycode) {
    if (keycode < MIDI_NOTE_BASE) {
        return UINT8_MAX;  /* Not a MIDI keycode */
    }
    
    uint8_t note = keycode - MIDI_NOTE_BASE;
    
    if (note <= MIDI_NOTE_MAX) {
        return note;
    }
    
    return UINT8_MAX;  /* Out of range */
}

/**
 * @brief Check if keycode is a MIDI note
 *
 * @param keycode ZMK keycode to check
 *
 * @return true if keycode is a MIDI note, false otherwise
 */
static bool is_midi_keycode(uint16_t keycode) {
    return keycode_to_midi_note(keycode) != UINT8_MAX;
}

/**@}*/

/**
 * @defgroup midi_event_handlers Event Handlers
 * @{
 */

/**
 * @brief Handle keypress/release events for MIDI output
 *
 * This listener is registered with ZMK's event system to intercept
 * keypress and keyrelease events. For MIDI keycodes, it converts them
 * to appropriate MIDI messages.
 *
 * @param listener Event listener reference
 * @param event Keycode state changed event
 *
 * @return ZMK_EV_EVENT_CONTINUE to allow further processing
 */
static int midi_on_keycode_state_changed(const zmk_event_t *eh) {
    if (!midi_config.enabled) {
        return ZMK_EV_EVENT_CONTINUE;
    }
    
    const struct zmk_keycode_state_changed *ev = 
        as_zmk_keycode_state_changed(eh);
    
    if (ev == NULL) {
        return ZMK_EV_EVENT_CONTINUE;
    }
    
    uint8_t note = keycode_to_midi_note(ev->keycode);
    
    if (note == UINT8_MAX) {
        /* Not a MIDI keycode, continue normal processing */
        return ZMK_EV_EVENT_CONTINUE;
    }
    
    /* Send MIDI message based on key state */
    if (ev->state) {
        /* Key pressed - send Note On */
        int ret = zmk_usb_midi_note_on(note, midi_config.default_velocity, 
                                       midi_config.default_channel);
        if (ret != 0) {
            LOG_ERR("Failed to send MIDI Note On: %d", ret);
        } else {
            LOG_DBG("MIDI Note On: note=%d", note);
        }
    } else {
        /* Key released - send Note Off */
        int ret = zmk_usb_midi_note_off(note, MIDI_VELOCITY_DEFAULT,
                                        midi_config.default_channel);
        if (ret != 0) {
            LOG_ERR("Failed to send MIDI Note Off: %d", ret);
        } else {
            LOG_DBG("MIDI Note Off: note=%d", note);
        }
    }
    
    /* Allow HID processing to continue (composite device) */
    return ZMK_EV_EVENT_CONTINUE;
}

/**
 * Register event listener
 * 
 * TODO: In real ZMK integration, this would use:
 * ZMK_LISTENER(midi_listener, midi_on_keycode_state_changed);
 * ZMK_SUBSCRIPTION(midi_listener, zmk_keycode_state_changed);
 */

/**@}*/

/**
 * @defgroup midi_config_api Configuration API
 * @{
 */

/**
 * @brief Set MIDI output channel for keyboard
 *
 * @param channel MIDI channel (1-16)
 */
void zmk_midi_set_channel(uint8_t channel) {
    if (!is_valid_channel(channel)) {
        LOG_ERR("Invalid MIDI channel: %d", channel);
        return;
    }
    
    midi_config.default_channel = channel;
    LOG_INF("MIDI channel set to %d", channel);
}

/**
 * @brief Get current MIDI output channel
 *
 * @return Current MIDI channel (1-16)
 */
uint8_t zmk_midi_get_channel(void) {
    return midi_config.default_channel;
}

/**
 * @brief Set default MIDI velocity for notes
 *
 * @param velocity Velocity (0-127)
 */
void zmk_midi_set_velocity(uint8_t velocity) {
    if (!is_valid_velocity(velocity)) {
        LOG_ERR("Invalid MIDI velocity: %d", velocity);
        return;
    }
    
    midi_config.default_velocity = velocity;
    LOG_INF("MIDI velocity set to %d", velocity);
}

/**
 * @brief Get current default MIDI velocity
 *
 * @return Current velocity (0-127)
 */
uint8_t zmk_midi_get_velocity(void) {
    return midi_config.default_velocity;
}

/**
 * @brief Enable/disable MIDI output
 *
 * @param enable true to enable, false to disable
 */
void zmk_midi_set_enabled(bool enable) {
    midi_config.enabled = enable;
    LOG_INF("MIDI output %s", enable ? "enabled" : "disabled");
}

/**
 * @brief Check if MIDI output is enabled
 *
 * @return true if enabled, false otherwise
 */
bool zmk_midi_is_enabled(void) {
    return midi_config.enabled;
}

/**@}*/

/**
 * @defgroup midi_shell_commands Shell Commands
 * @{
 */

#if IS_ENABLED(CONFIG_ZMK_USB_MIDI_SHELL)

#include <zephyr/shell/shell.h>

static int cmd_midi_channel(const struct shell *sh, size_t argc, char **argv) {
    if (argc > 2) {
        shell_error(sh, "Invalid arguments");
        return -EINVAL;
    }
    
    if (argc == 2) {
        uint8_t channel = strtoul(argv[1], NULL, 10);
        zmk_midi_set_channel(channel);
    }
    
    shell_print(sh, "MIDI Channel: %d", zmk_midi_get_channel());
    return 0;
}

static int cmd_midi_velocity(const struct shell *sh, size_t argc, char **argv) {
    if (argc > 2) {
        shell_error(sh, "Invalid arguments");
        return -EINVAL;
    }
    
    if (argc == 2) {
        uint8_t velocity = strtoul(argv[1], NULL, 10);
        zmk_midi_set_velocity(velocity);
    }
    
    shell_print(sh, "MIDI Velocity: %d", zmk_midi_get_velocity());
    return 0;
}

static int cmd_midi_enable(const struct shell *sh, size_t argc, char **argv) {
    if (argc > 2) {
        shell_error(sh, "Invalid arguments");
        return -EINVAL;
    }
    
    if (argc == 2) {
        bool enable = (argv[1][0] == 'y' || argv[1][0] == '1');
        zmk_midi_set_enabled(enable);
    }
    
    shell_print(sh, "MIDI Output: %s", zmk_midi_is_enabled() ? "enabled" : "disabled");
    return 0;
}

static int cmd_midi_test(const struct shell *sh, size_t argc, char **argv) {
    if (argc != 2) {
        shell_error(sh, "Usage: midi test <note>");
        return -EINVAL;
    }
    
    uint8_t note = strtoul(argv[1], NULL, 10);
    
    /* Send test note on */
    int ret = zmk_usb_midi_note_on(note, MIDI_VELOCITY_DEFAULT,
                                   zmk_midi_get_channel());
    if (ret != 0) {
        shell_error(sh, "Failed to send note on: %d", ret);
        return ret;
    }
    
    /* Wait 100ms */
    k_msleep(100);
    
    /* Send note off */
    ret = zmk_usb_midi_note_off(note, MIDI_VELOCITY_DEFAULT,
                                zmk_midi_get_channel());
    if (ret != 0) {
        shell_error(sh, "Failed to send note off: %d", ret);
        return ret;
    }
    
    shell_print(sh, "MIDI test note %d sent", note);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(midi_cmds,
    SHELL_CMD(channel, NULL, "Get/set MIDI channel [1-16]", cmd_midi_channel),
    SHELL_CMD(velocity, NULL, "Get/set MIDI velocity [0-127]", cmd_midi_velocity),
    SHELL_CMD(enable, NULL, "Enable/disable MIDI output [y/n]", cmd_midi_enable),
    SHELL_CMD(test, NULL, "Send test MIDI note", cmd_midi_test),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(midi, &midi_cmds, "MIDI subsystem commands", NULL);

#endif /* CONFIG_ZMK_USB_MIDI_SHELL */

/**@}*/
