/*
 * Copyright (c) 2026 ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/usb/usb_device.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zmk_usb_midi_desc);

/**
 * @file usb_midi_descriptors.c
 * @brief USB MIDI device descriptors
 *
 * This file contains the USB device descriptors needed to enumerate
 * as a USB MIDI device. Following USB MIDI 1.0 specification.
 *
 * The device uses USB Audio Class 1.0 with MIDI Streaming interface.
 * Reference: https://www.usb.org/sites/default/files/midi10.pdf
 */

/**
 * @defgroup usb_descriptors USB Descriptor Values
 * @{
 */

/* USB Device Descriptor */
#define DEVICE_CLASS            0x00  /* Use interface descriptor class */
#define DEVICE_SUBCLASS         0x00
#define DEVICE_PROTOCOL         0x00
#define DEVICE_MAX_PACKET_SIZE  64
#define DEVICE_RELEASE          0x0100

/* Vendor and Product IDs (use test IDs - change for production) */
#define VENDOR_ID               0x2341  /* Arduino */
#define PRODUCT_ID              0x0249  /* Arduino test PID */

/* USB Configuration Descriptor */
#define CONFIG_VALUE            1
#define CONFIG_ATTRIBUTES       (USB_SelfPowered | USB_RemoteWakeup)
#define CONFIG_MAX_POWER        250  /* 500mA */

/* USB Audio Control Interface */
#define AC_INTERFACE_NUM        0
#define AC_INTERFACE_ALT        0
#define AC_NUM_ENDPOINTS        0
#define AC_SUBCLASS             0x01  /* Audio Control */
#define AC_PROTOCOL             0x00

/* USB Audio Streaming Interface (MIDI) */
#define AS_INTERFACE_NUM        1
#define AS_INTERFACE_ALT_0      0     /* No endpoints (inactive) */
#define AS_INTERFACE_ALT_1      1     /* With endpoints (active) */
#define AS_SUBCLASS             0x03  /* MIDI Streaming */
#define AS_PROTOCOL             0x00

/* MIDI endpoints */
#define MIDI_OUT_EP             0x01
#define MIDI_OUT_EP_SIZE        64
#define MIDI_IN_EP              0x81
#define MIDI_IN_EP_SIZE         64

/**@}*/

/**
 * @defgroup usb_class_codes USB Class Codes
 * @{
 */

/* USB Audio Class */
#define USB_CLASS_AUDIO         0x01
#define USB_AUDIO_SUBCLASS_CTRL 0x01
#define USB_AUDIO_SUBCLASS_MIDI 0x03

/**@}*/

/**
 * @defgroup usb_descriptor_structures Descriptor Structures
 * @brief USB descriptor definitions for MIDI device
 * @{
 */

/*
 * Device Descriptor
 * 
 * This descriptor identifies the device to the USB host.
 * The class/subclass/protocol fields are set to 0x00 to defer to
 * the interface descriptors for class information.
 */
static const uint8_t device_descriptor[] = {
    0x12,                       /* bLength */
    USB_DEVICE_DESC_TYPE,       /* bDescriptorType */
    0x00, 0x02,                 /* bcdUSB = 2.00 */
    DEVICE_CLASS,               /* bDeviceClass */
    DEVICE_SUBCLASS,            /* bDeviceSubClass */
    DEVICE_PROTOCOL,            /* bDeviceProtocol */
    DEVICE_MAX_PACKET_SIZE,     /* bMaxPacketSize0 */
    LSB(VENDOR_ID),             /* idVendor */
    MSB(VENDOR_ID),
    LSB(PRODUCT_ID),            /* idProduct */
    MSB(PRODUCT_ID),
    LSB(DEVICE_RELEASE),        /* bcdDevice */
    MSB(DEVICE_RELEASE),
    1,                          /* iManufacturer */
    2,                          /* iProduct */
    3,                          /* iSerialNumber */
    1,                          /* bNumConfigurations */
};

/*
 * Configuration and Interface Descriptors
 *
 * USB MIDI requires:
 * 1. Configuration Descriptor
 * 2. Audio Control Interface (no endpoints)
 * 3. MIDI Streaming Interface (with IN/OUT endpoints)
 *
 * The Audio Control interface must be first, followed by MIDI Streaming.
 */
static const uint8_t config_descriptor[] = {
    /* ===== Configuration Descriptor ===== */
    0x09,                       /* bLength */
    USB_CONFIG_DESC_TYPE,       /* bDescriptorType */
    0x65, 0x00,                 /* wTotalLength (101 bytes, calculated) */
    0x02,                       /* bNumInterfaces (AC + MIDI Streaming) */
    CONFIG_VALUE,               /* bConfigurationValue */
    0,                          /* iConfiguration */
    CONFIG_ATTRIBUTES,          /* bmAttributes */
    CONFIG_MAX_POWER,           /* bMaxPower (in 2mA units) */

    /* ===== Audio Control Interface ===== */
    0x09,                       /* bLength */
    USB_INTERFACE_DESC_TYPE,    /* bDescriptorType */
    AC_INTERFACE_NUM,           /* bInterfaceNumber */
    AC_INTERFACE_ALT,           /* bAlternateSetting */
    AC_NUM_ENDPOINTS,           /* bNumEndpoints */
    USB_CLASS_AUDIO,            /* bInterfaceClass (Audio) */
    AC_SUBCLASS,                /* bInterfaceSubClass (Audio Control) */
    AC_PROTOCOL,                /* bInterfaceProtocol */
    0,                          /* iInterface */

    /* Audio Control Descriptor Header */
    0x09,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x01,                       /* bDescriptorSubtype (Header) */
    0x00, 0x01,                 /* bcdADC (Audio Device Class 1.0) */
    0x09, 0x00,                 /* wTotalLength (9 bytes) */
    0x01,                       /* bInCollection (1 MIDI Streaming interface) */
    AS_INTERFACE_NUM,           /* baInterfaceNr[0] (MIDI Streaming Interface) */

    /* ===== MIDI Streaming Interface (Alternate Setting 0) ===== */
    /* No endpoints - inactive */
    0x09,                       /* bLength */
    USB_INTERFACE_DESC_TYPE,    /* bDescriptorType */
    AS_INTERFACE_NUM,           /* bInterfaceNumber */
    AS_INTERFACE_ALT_0,         /* bAlternateSetting (0 - inactive) */
    0,                          /* bNumEndpoints */
    USB_CLASS_AUDIO,            /* bInterfaceClass (Audio) */
    AS_SUBCLASS,                /* bInterfaceSubClass (MIDI Streaming) */
    AS_PROTOCOL,                /* bInterfaceProtocol */
    0,                          /* iInterface */

    /* ===== MIDI Streaming Interface (Alternate Setting 1) ===== */
    /* With endpoints - active */
    0x09,                       /* bLength */
    USB_INTERFACE_DESC_TYPE,    /* bDescriptorType */
    AS_INTERFACE_NUM,           /* bInterfaceNumber */
    AS_INTERFACE_ALT_1,         /* bAlternateSetting (1 - active) */
    2,                          /* bNumEndpoints (IN + OUT) */
    USB_CLASS_AUDIO,            /* bInterfaceClass (Audio) */
    AS_SUBCLASS,                /* bInterfaceSubClass (MIDI Streaming) */
    AS_PROTOCOL,                /* bInterfaceProtocol */
    0,                          /* iInterface */

    /* MIDI Streaming Descriptor Header */
    0x07,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x01,                       /* bDescriptorSubtype (Header) */
    0x00, 0x01,                 /* bcdMSC (MIDI Streaming Class 1.0) */
    0x41, 0x00,                 /* wTotalLength (65 bytes) */

    /* MIDI In Jack Descriptor (embedded) */
    0x06,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x02,                       /* bDescriptorSubtype (MIDI In Jack) */
    0x01,                       /* bJackType (Embedded) */
    0x01,                       /* bJackID */
    0,                          /* iJack */

    /* MIDI Out Jack Descriptor (embedded) */
    0x06,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x02,                       /* bDescriptorSubtype (MIDI In Jack) */
    0x02,                       /* bJackType (External) */
    0x02,                       /* bJackID */
    0,                          /* iJack */

    /* MIDI Out Jack Descriptor (external) */
    0x06,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x03,                       /* bDescriptorSubtype (MIDI Out Jack) */
    0x01,                       /* bJackType (Embedded) */
    0x03,                       /* bJackID */
    0,                          /* iJack */

    /* MIDI In Jack Descriptor (external) */
    0x06,                       /* bLength */
    0x24,                       /* bDescriptorType (Audio Interface) */
    0x03,                       /* bDescriptorSubtype (MIDI Out Jack) */
    0x02,                       /* bJackType (External) */
    0x04,                       /* bJackID */
    0,                          /* iJack */

    /* Element Descriptor (MIDI Out Jack to In Jack connection) */
    0x09,                       /* bLength */
    0x24,                       /* bDescriptorType */
    0x04,                       /* bDescriptorSubtype (Element) */
    0x01,                       /* bElementID */
    0x03,                       /* bSourceID (MIDI Out Jack - Embedded) */
    0x01,                       /* bSourcePin */
    0x01,                       /* bNrInPins */
    0x01,                       /* baSourceID[0] (MIDI In Jack - Embedded) */
    0x01,                       /* baSourcePin[0] */

    /* MIDI OUT Endpoint Descriptor */
    0x09,                       /* bLength */
    USB_ENDPOINT_DESC_TYPE,     /* bDescriptorType */
    MIDI_OUT_EP,                /* bEndpointAddress (OUT) */
    USB_BULK_TRANSFER_TYPE,     /* bmAttributes (Bulk) */
    LSB(MIDI_OUT_EP_SIZE),      /* wMaxPacketSize */
    MSB(MIDI_OUT_EP_SIZE),
    0,                          /* bInterval (ignored for bulk) */
    0,                          /* bRefresh (Audio only) */
    0,                          /* bSynchAddress */

    /* MIDI OUT Jack to Host */
    0x05,                       /* bLength */
    0x25,                       /* bDescriptorType (Audio Endpoint) */
    0x01,                       /* bDescriptorSubtype (General) */
    0x01,                       /* bNumEmbMIDIJack */
    0x01,                       /* baAssocJackID[0] */

    /* MIDI IN Endpoint Descriptor */
    0x09,                       /* bLength */
    USB_ENDPOINT_DESC_TYPE,     /* bDescriptorType */
    MIDI_IN_EP,                 /* bEndpointAddress (IN) */
    USB_BULK_TRANSFER_TYPE,     /* bmAttributes (Bulk) */
    LSB(MIDI_IN_EP_SIZE),       /* wMaxPacketSize */
    MSB(MIDI_IN_EP_SIZE),
    0,                          /* bInterval (ignored for bulk) */
    0,                          /* bRefresh (Audio only) */
    0,                          /* bSynchAddress */

    /* MIDI IN Jack to Device */
    0x05,                       /* bLength */
    0x25,                       /* bDescriptorType (Audio Endpoint) */
    0x01,                       /* bDescriptorSubtype (General) */
    0x01,                       /* bNumEmbMIDIJack */
    0x03,                       /* baAssocJackID[0] */
};

/**@}*/

/**
 * @defgroup usb_strings String Descriptors
 * @{
 */

/*
 * String Descriptors
 * 
 * Index 0: Language ID (English US)
 * Index 1: Manufacturer
 * Index 2: Product
 * Index 3: Serial Number
 */

static const uint8_t string_desc_0[] = {
    0x04,                       /* bLength */
    USB_STRING_DESC_TYPE,       /* bDescriptorType */
    0x09, 0x04,                 /* wLANGID (English US) */
};

static const uint8_t string_desc_1[] = {
    0x10,                       /* bLength */
    USB_STRING_DESC_TYPE,       /* bDescriptorType */
    'Z', 0, 'M', 0, 'K', 0, ' ', 0,
    'T', 0, 'e', 0, 'a', 0, 'm', 0,
};

static const uint8_t string_desc_2[] = {
    0x16,                       /* bLength */
    USB_STRING_DESC_TYPE,       /* bDescriptorType */
    'Z', 0, 'M', 0, 'K', 0, ' ', 0,
    'M', 0, 'I', 0, 'D', 0, 'I', 0,
    ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
};

static const uint8_t string_desc_3[] = {
    0x12,                       /* bLength */
    USB_STRING_DESC_TYPE,       /* bDescriptorType */
    '0', 0, '0', 0, '0', 0, '0', 0,
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
};

/*
 * String descriptor array
 * Index matches USB string descriptor indices
 */
static const uint8_t *string_descriptors[] = {
    string_desc_0,
    string_desc_1,
    string_desc_2,
    string_desc_3,
};

#define STRING_DESC_COUNT ARRAY_SIZE(string_descriptors)

/**@}*/

/**
 * @defgroup usb_descriptor_export Descriptor Export
 * @brief Make descriptors available to USB device stack
 * @{
 */

/**
 * @brief Get device descriptor
 *
 * @return Pointer to device descriptor
 */
const uint8_t *midi_get_device_descriptor(void) {
    return device_descriptor;
}

/**
 * @brief Get configuration descriptor
 *
 * @return Pointer to configuration descriptor
 */
const uint8_t *midi_get_config_descriptor(void) {
    return config_descriptor;
}

/**
 * @brief Get string descriptor by index
 *
 * @param index String descriptor index
 *
 * @return Pointer to string descriptor, or NULL if index invalid
 */
const uint8_t *midi_get_string_descriptor(uint8_t index) {
    if (index >= STRING_DESC_COUNT) {
        return NULL;
    }
    return string_descriptors[index];
}

/**@}*/
