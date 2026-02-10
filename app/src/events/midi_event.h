/*
 * Copyright (c) 2026 ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ZMK_EVENTS_MIDI_EVENT_H_
#define ZMK_EVENTS_MIDI_EVENT_H_

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file midi_event.h
 * @brief MIDI event definitions for ZMK
 *
 * These events are triggered when MIDI messages need to be sent,
 * allowing other ZMK subsystems to respond to MIDI activity.
 */

/**
 * @brief MIDI Note On event
 *
 * Posted when a note-on message is sent to the MIDI output.
 * Can be used by listeners to perform additional actions or logging.
 */
struct zmk_midi_note_on_event {
    uint8_t note;
    uint8_t velocity;
    uint8_t channel;
};

/**
 * @brief MIDI Note Off event
 *
 * Posted when a note-off message is sent to the MIDI output.
 */
struct zmk_midi_note_off_event {
    uint8_t note;
    uint8_t velocity;
    uint8_t channel;
};

/**
 * @brief MIDI CC (Control Change) event
 *
 * Posted when a control change message is sent to the MIDI output.
 */
struct zmk_midi_cc_event {
    uint8_t controller;
    uint8_t value;
    uint8_t channel;
};

/**
 * @brief Generic MIDI message event
 *
 * Posted for any raw MIDI message sent via USB MIDI.
 */
struct zmk_midi_message_event {
    uint8_t cin;     /* Code Index Number */
    uint8_t byte1;   /* MIDI status byte */
    uint8_t byte2;   /* MIDI data */
    uint8_t byte3;   /* MIDI data */
    uint8_t cable;   /* Cable number */
};

#ifdef __cplusplus
}
#endif

#endif /* ZMK_EVENTS_MIDI_EVENT_H_ */
