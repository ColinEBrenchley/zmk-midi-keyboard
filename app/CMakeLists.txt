cmake_minimum_required(VERSION 3.20.0)

project(zmk_usb_midi)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# USB MIDI Source Files
set(MIDI_SOURCES
    src/usb/usb_midi.c
    src/usb/usb_midi_descriptors.c
    src/usb/usb_midi_integration.c
)

# Add USB MIDI module
target_sources(app PRIVATE ${MIDI_SOURCES})

# Include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Zephyr Configuration
# These settings ensure proper USB device stack configuration
set(CONF_FILE prj.conf)
