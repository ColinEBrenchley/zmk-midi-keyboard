/*
 * Copyright (c) 2026 ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ZMK_USB_MIDI_H_
#define ZMK_USB_MIDI_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file usb_midi.h
 * @brief USB MIDI device interface for ZMK
 *
 * This module provides functionality to send MIDI messages over USB.
 * It implements the USB MIDI 1.0 specification and integrates with ZMK's
 * keypress event system.
 */

/**
 * @defgroup zmk_usb_midi USB MIDI Interface
 * @{
 */

/**
 * @brief MIDI status bytes
 */
#define MIDI_NOTE_OFF       0x80
#define MIDI_NOTE_ON        0x90
#define MIDI_CONTROL_CHANGE 0xB0
#define MIDI_PROGRAM_CHANGE 0xC0

/**
 * @brief USB MIDI Code Index Numbers (CIN)
 * These identify the MIDI message type in USB MIDI 1.0 packets
 */
#define USB_MIDI_CIN_MISC           0x0
#define USB_MIDI_CIN_CABLE_EVENTS   0x1
#define USB_MIDI_CIN_SYSTEM_COMMON_2 0x2
#define USB_MIDI_CIN_SYSTEM_COMMON_3 0x3
#define USB_MIDI_CIN_SYSTEM_EXCLUSIVE 0x4
#define USB_MIDI_CIN_SYSTEM_COMMON_1 0x5
#define USB_MIDI_CIN_SYSTEM_REALTIME 0x6
#define USB_MIDI_CIN_NOTE_OFF       0x8
#define USB_MIDI_CIN_NOTE_ON        0x9
#define USB_MIDI_CIN_POLY_PRESSURE  0xA
#define USB_MIDI_CIN_CONTROL_CHANGE 0xB
#define USB_MIDI_CIN_PROGRAM_CHANGE 0xC
#define USB_MIDI_CIN_CHANNEL_PRESSURE 0xD
#define USB_MIDI_CIN_PITCH_BEND     0xE
#define USB_MIDI_CIN_SINGLE_BYTE    0xF

/**
 * @brief MIDI velocity range
 */
#define MIDI_VELOCITY_MIN    0
#define MIDI_VELOCITY_MAX    127
#define MIDI_VELOCITY_DEFAULT 64

/**
 * @brief MIDI note range
 */
#define MIDI_NOTE_MIN   0
#define MIDI_NOTE_MAX   127
#define MIDI_NOTE_C0    0
#define MIDI_NOTE_C4    60  /* Middle C */
#define MIDI_NOTE_A4    69

/**
 * @brief MIDI channel range (1-16)
 */
#define MIDI_CHANNEL_MIN 1
#define MIDI_CHANNEL_MAX 16

/**
 * @brief USB MIDI 1.0 Packet structure (4 bytes)
 *
 * Byte 0: [cable_number (4 bits)] [code_index_number (4 bits)]
 * Bytes 1-3: MIDI message bytes (can be 1, 2, or 3 bytes depending on CIN)
 */
typedef struct {
    uint8_t header;      /* [cable_num (upper 4)] [CIN (lower 4)] */
    uint8_t byte1;       /* MIDI status or data */
    uint8_t byte2;       /* MIDI data */
    uint8_t byte3;       /* MIDI data */
} zmk_usb_midi_packet_t;

/**
 * @brief Initialize the USB MIDI subsystem
 *
 * This must be called during ZMK initialization to set up the USB MIDI
 * device descriptor and configure endpoints.
 *
 * @return 0 on success, negative error code on failure
 */
int zmk_usb_midi_init(void);

/**
 * @brief Send a MIDI Note On message
 *
 * @param note      MIDI note number (0-127)
 * @param velocity  Note-on velocity (0-127), 0 means note-off
 * @param channel   MIDI channel (1-16)
 *
 * @return 0 on success, negative error code on failure
 */
int zmk_usb_midi_note_on(uint8_t note, uint8_t velocity, uint8_t channel);

/**
 * @brief Send a MIDI Note Off message
 *
 * @param note      MIDI note number (0-127)
 * @param velocity  Note-off velocity (0-127), typically 64
 * @param channel   MIDI channel (1-16)
 *
 * @return 0 on success, negative error code on failure
 */
int zmk_usb_midi_note_off(uint8_t note, uint8_t velocity, uint8_t channel);

/**
 * @brief Send a Control Change message
 *
 * @param controller Controller number (0-127)
 * @param value      Controller value (0-127)
 * @param channel    MIDI channel (1-16)
 *
 * @return 0 on success, negative error code on failure
 */
int zmk_usb_midi_cc(uint8_t controller, uint8_t value, uint8_t channel);

/**
 * @brief Send a raw MIDI message via USB
 *
 * @param cin       Code Index Number identifying message type
 * @param byte1     First MIDI byte (status)
 * @param byte2     Second MIDI byte (data)
 * @param byte3     Third MIDI byte (data)
 * @param cable     Cable number (typically 0)
 *
 * @return 0 on success, negative error code on failure
 */
int zmk_usb_midi_send(uint8_t cin, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t cable);

/**
 * @brief Get/set the current MIDI output channel
 *
 * @param channel   MIDI channel (1-16), 0 to query current
 *
 * @return Current channel (1-16)
 */
uint8_t zmk_usb_midi_channel(uint8_t channel);

/**
 * @brief Get/set the default MIDI velocity
 *
 * @param velocity  Velocity (0-127), 0 to query current
 *
 * @return Current velocity (0-127)
 */
uint8_t zmk_usb_midi_velocity(uint8_t velocity);

/**
 * @brief Check if MIDI output is enabled
 *
 * @return true if MIDI is enabled, false otherwise
 */
bool zmk_usb_midi_is_enabled(void);

/**
 * @brief Enable or disable MIDI output
 *
 * @param enable true to enable, false to disable
 */
void zmk_usb_midi_set_enabled(bool enable);

/**
 * @brief Check if the USB MIDI device is connected
 *
 * @return true if USB is enumerated and ready, false otherwise
 */
bool zmk_usb_midi_is_connected(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ZMK_USB_MIDI_H_ */
