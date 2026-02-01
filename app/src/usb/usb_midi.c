/*
 * Copyright (c) 2026 ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_audio.h>
#include <zephyr/logging/log.h>

#include "zmk/usb_midi.h"

LOG_MODULE_REGISTER(zmk_usb_midi);

/**
 * @brief USB MIDI subsystem implementation
 *
 * This module implements USB MIDI 1.0 device class support for ZMK.
 * It provides the USB device descriptors, endpoints, and interface
 * definitions required for a class-compliant USB MIDI device.
 */

/**
 * @defgroup usb_midi_descriptors USB MIDI Descriptors
 * @brief USB class, subclass, and protocol definitions for MIDI
 * @{
 */

/* USB Audio Class codes */
#define USB_AUDIO_CLASS             0x01
#define USB_AUDIO_SUBCLASS_CONTROL  0x01
#define USB_AUDIO_SUBCLASS_STREAMING 0x02
#define USB_AUDIO_PROTOCOL_UNDEFINED 0x00

/* USB MIDI Streaming subclass */
#define USB_MIDI_STREAMING_SUBCLASS 0x03
#define USB_MIDI_STREAMING_PROTOCOL 0x00

/**@}*/

/**
 * @defgroup usb_midi_state Module State
 * @{
 */

/* USB MIDI device configuration state */
static struct {
    uint8_t channel;                      /* Current MIDI channel (1-16) */
    uint8_t velocity;                     /* Default MIDI velocity */
    bool enabled;                         /* MIDI output enabled */
    bool usb_connected;                   /* USB device enumerated */
    struct k_mutex lock;                  /* Thread-safe access */
} zmk_midi_state = {
    .channel = 1,
    .velocity = MIDI_VELOCITY_DEFAULT,
    .enabled = true,
    .usb_connected = false,
};

/**@}*/

/**
 * @defgroup usb_midi_endpoints USB Endpoints
 * @{
 */

/* Endpoint definitions for MIDI */
#define MIDI_IN_EP      0x81
#define MIDI_IN_EP_SIZE 64

#define MIDI_OUT_EP     0x01
#define MIDI_OUT_EP_SIZE 64

/**@}*/

/**
 * @defgroup usb_midi_helpers Helper Functions
 * @{
 */

/**
 * @brief Validate MIDI note number
 *
 * @param note Note number to validate
 *
 * @return true if valid, false otherwise
 */
static inline bool is_valid_note(uint8_t note) {
    return note <= MIDI_NOTE_MAX;
}

/**
 * @brief Validate MIDI velocity
 *
 * @param velocity Velocity to validate
 *
 * @return true if valid (0-127), false otherwise
 */
static inline bool is_valid_velocity(uint8_t velocity) {
    return velocity <= MIDI_VELOCITY_MAX;
}

/**
 * @brief Validate MIDI channel
 *
 * @param channel Channel to validate (1-16)
 *
 * @return true if valid, false otherwise
 */
static inline bool is_valid_channel(uint8_t channel) {
    return (channel >= MIDI_CHANNEL_MIN) && (channel <= MIDI_CHANNEL_MAX);
}

/**
 * @brief Construct a USB MIDI packet
 *
 * @param cin       Code Index Number
 * @param byte1     First MIDI byte
 * @param byte2     Second MIDI byte
 * @param byte3     Third MIDI byte
 * @param cable     Cable number (usually 0)
 *
 * @return Constructed USB MIDI packet
 */
static zmk_usb_midi_packet_t midi_packet_construct(uint8_t cin, uint8_t byte1, 
                                                    uint8_t byte2, uint8_t byte3,
                                                    uint8_t cable) {
    zmk_usb_midi_packet_t packet;
    
    /* Construct header: [cable_number (upper 4 bits)] [CIN (lower 4 bits)] */
    packet.header = ((cable & 0x0F) << 4) | (cin & 0x0F);
    packet.byte1 = byte1;
    packet.byte2 = byte2;
    packet.byte3 = byte3;
    
    return packet;
}

/**@}*/

/**
 * @defgroup usb_midi_core Core MIDI Functions
 * @{
 */

/**
 * @brief Send a raw USB MIDI packet
 *
 * This is the low-level function that sends actual USB MIDI packets.
 * In a real ZMK integration, this would write to the USB endpoint.
 *
 * @param packet Pointer to USB MIDI packet to send
 *
 * @return 0 on success, negative error code on failure
 */
static int midi_send_packet(const zmk_usb_midi_packet_t *packet) {
    if (!zmk_midi_state.usb_connected) {
        LOG_DBG("USB MIDI not connected");
        return -ENOTCONN;
    }

    if (packet == NULL) {
        LOG_ERR("Null packet pointer");
        return -EINVAL;
    }

    /* 
     * TODO: In a real ZMK integration, this would:
     * 1. Get the USB device endpoint
     * 2. Call usb_ep_write() with the packet data (4 bytes)
     * 3. Handle the async callback or wait for completion
     * 
     * For now, we'll just log the packet and validate it
     */
    
    LOG_DBG("MIDI packet - header: 0x%02X, b1: 0x%02X, b2: 0x%02X, b3: 0x%02X",
            packet->header, packet->byte1, packet->byte2, packet->byte3);
    
    return 0;
}

/**@}*/

/**
 * @defgroup usb_midi_public Public API
 * @{
 */

int zmk_usb_midi_init(void) {
    LOG_INF("Initializing USB MIDI subsystem");
    
    k_mutex_init(&zmk_midi_state.lock);
    
    /*
     * TODO: In a real ZMK integration, this would:
     * 1. Register USB device descriptor with MIDI class codes
     * 2. Configure USB Audio class interface
     * 3. Configure MIDI Streaming endpoints
     * 4. Register USB callbacks (attach/detach)
     * 5. Enable USB device stack
     * 
     * Steps would be similar to:
     * - Configure USB device speed (usually USB 2.0 full speed)
     * - Add configuration descriptor with Audio Control Interface
     * - Add MIDI Streaming Interface with bulk/interrupt endpoints
     * - Register endpoint callbacks
     * - Call usb_device_register() and usb_enable()
     */
    
    LOG_INF("USB MIDI subsystem initialized");
    
    return 0;
}

int zmk_usb_midi_send(uint8_t cin, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t cable) {
    if (!zmk_midi_state.enabled) {
        LOG_DBG("MIDI output disabled");
        return -EACCES;
    }
    
    zmk_usb_midi_packet_t packet = midi_packet_construct(cin, byte1, byte2, byte3, cable);
    
    return midi_send_packet(&packet);
}

int zmk_usb_midi_note_on(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!is_valid_note(note)) {
        LOG_ERR("Invalid note: %d", note);
        return -EINVAL;
    }
    
    if (!is_valid_velocity(velocity)) {
        LOG_ERR("Invalid velocity: %d", velocity);
        return -EINVAL;
    }
    
    if (!is_valid_channel(channel)) {
        LOG_ERR("Invalid channel: %d", channel);
        return -EINVAL;
    }
    
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    
    /* 
     * MIDI Note On format:
     * Status: 0x90 | (channel - 1)
     * Byte 2: Note number
     * Byte 3: Velocity
     */
    uint8_t status = MIDI_NOTE_ON | (channel - 1);
    
    int ret = zmk_usb_midi_send(USB_MIDI_CIN_NOTE_ON, status, note, velocity, 0);
    
    k_mutex_unlock(&zmk_midi_state.lock);
    
    if (ret == 0) {
        LOG_DBG("Note On: note=%d, velocity=%d, channel=%d", note, velocity, channel);
    }
    
    return ret;
}

int zmk_usb_midi_note_off(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!is_valid_note(note)) {
        LOG_ERR("Invalid note: %d", note);
        return -EINVAL;
    }
    
    if (!is_valid_velocity(velocity)) {
        LOG_ERR("Invalid velocity: %d", velocity);
        return -EINVAL;
    }
    
    if (!is_valid_channel(channel)) {
        LOG_ERR("Invalid channel: %d", channel);
        return -EINVAL;
    }
    
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    
    /*
     * MIDI Note Off format:
     * Status: 0x80 | (channel - 1)
     * Byte 2: Note number
     * Byte 3: Velocity (typically 64)
     */
    uint8_t status = MIDI_NOTE_OFF | (channel - 1);
    
    int ret = zmk_usb_midi_send(USB_MIDI_CIN_NOTE_OFF, status, note, velocity, 0);
    
    k_mutex_unlock(&zmk_midi_state.lock);
    
    if (ret == 0) {
        LOG_DBG("Note Off: note=%d, velocity=%d, channel=%d", note, velocity, channel);
    }
    
    return ret;
}

int zmk_usb_midi_cc(uint8_t controller, uint8_t value, uint8_t channel) {
    if (controller > MIDI_VELOCITY_MAX) {
        LOG_ERR("Invalid controller: %d", controller);
        return -EINVAL;
    }
    
    if (!is_valid_velocity(value)) {
        LOG_ERR("Invalid CC value: %d", value);
        return -EINVAL;
    }
    
    if (!is_valid_channel(channel)) {
        LOG_ERR("Invalid channel: %d", channel);
        return -EINVAL;
    }
    
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    
    /*
     * MIDI Control Change format:
     * Status: 0xB0 | (channel - 1)
     * Byte 2: Controller number
     * Byte 3: Value
     */
    uint8_t status = MIDI_CONTROL_CHANGE | (channel - 1);
    
    int ret = zmk_usb_midi_send(USB_MIDI_CIN_CONTROL_CHANGE, status, controller, value, 0);
    
    k_mutex_unlock(&zmk_midi_state.lock);
    
    if (ret == 0) {
        LOG_DBG("CC: controller=%d, value=%d, channel=%d", controller, value, channel);
    }
    
    return ret;
}

uint8_t zmk_usb_midi_channel(uint8_t channel) {
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    
    if (is_valid_channel(channel)) {
        zmk_midi_state.channel = channel;
        LOG_DBG("MIDI channel set to %d", channel);
    }
    
    uint8_t current = zmk_midi_state.channel;
    
    k_mutex_unlock(&zmk_midi_state.lock);
    
    return current;
}

uint8_t zmk_usb_midi_velocity(uint8_t velocity) {
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    
    if (is_valid_velocity(velocity)) {
        zmk_midi_state.velocity = velocity;
        LOG_DBG("MIDI velocity set to %d", velocity);
    }
    
    uint8_t current = zmk_midi_state.velocity;
    
    k_mutex_unlock(&zmk_midi_state.lock);
    
    return current;
}

bool zmk_usb_midi_is_enabled(void) {
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    bool enabled = zmk_midi_state.enabled;
    k_mutex_unlock(&zmk_midi_state.lock);
    
    return enabled;
}

void zmk_usb_midi_set_enabled(bool enable) {
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    zmk_midi_state.enabled = enable;
    LOG_INF("MIDI output %s", enable ? "enabled" : "disabled");
    k_mutex_unlock(&zmk_midi_state.lock);
}

bool zmk_usb_midi_is_connected(void) {
    k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
    bool connected = zmk_midi_state.usb_connected;
    k_mutex_unlock(&zmk_midi_state.lock);
    
    return connected;
}

/**@}*/

/**
 * @defgroup usb_midi_initialization Module Initialization
 * @{
 */

/**
 * @brief USB status callback
 *
 * This callback is registered with the Zephyr USB device stack to be notified
 * of USB enumeration and disconnection events.
 *
 * @param cb_usb_status USB status code
 */
static void midi_usb_status_cb(enum usb_dc_status_code status) {
    switch (status) {
        case USB_DC_CONNECTED:
            LOG_INF("USB MIDI device connected");
            k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
            zmk_midi_state.usb_connected = true;
            k_mutex_unlock(&zmk_midi_state.lock);
            break;
            
        case USB_DC_DISCONNECTED:
            LOG_INF("USB MIDI device disconnected");
            k_mutex_lock(&zmk_midi_state.lock, K_FOREVER);
            zmk_midi_state.usb_connected = false;
            k_mutex_unlock(&zmk_midi_state.lock);
            break;
            
        case USB_DC_RESET:
            LOG_DBG("USB MIDI device reset");
            break;
            
        case USB_DC_SUSPEND:
            LOG_DBG("USB MIDI device suspended");
            break;
            
        case USB_DC_RESUME:
            LOG_DBG("USB MIDI device resumed");
            break;
            
        default:
            LOG_DBG("USB MIDI status: %d", status);
            break;
    }
}

/**
 * @brief Initialize USB MIDI at boot time
 *
 * Uses SYS_INIT to initialize the USB MIDI subsystem during kernel initialization.
 */
static int zmk_usb_midi_init_late(void) {
    LOG_INF("Late USB MIDI initialization");
    
    /*
     * TODO: In real implementation:
     * 1. Register USB status callback: usb_device_register_status_callback(&midi_usb_status_cb)
     * 2. Configure USB descriptors with MIDI class codes
     * 3. Configure endpoints
     * 4. Enable USB device
     * 
     * Example:
     *   usb_device_register_status_callback(&midi_usb_status_cb);
     *   return usb_device_init_action_on_bind();
     */
    
    return 0;
}

/**
 * Register USB MIDI initialization at CONFIG level
 * (after post-kernel initialization)
 */
SYS_INIT(zmk_usb_midi_init_late, POST_KERNEL, CONFIG_ZMK_USB_MIDI_INIT_PRIORITY);

/**@}*/
