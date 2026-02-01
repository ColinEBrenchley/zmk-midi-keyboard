# MIDI Note Reference for ZMK

Complete reference for all MIDI notes (0-127) and their ZMK notation.

## Quick Reference - Common Octaves

### Octave 1 (MIDI 12-23) - Very Low
```
C1=12  CS1=13 D1=14  DS1=15 E1=16  F1=17  FS1=18 G1=19  GS1=20 A1=21  AS1=22 B1=23
```

### Octave 2 (MIDI 24-35) - Low
```
C2=24  CS2=25 D2=26  DS2=27 E2=28  F2=29  FS2=30 G2=31  GS2=32 A2=33  AS2=34 B2=35
```

### Octave 3 (MIDI 36-47) - Mid-Low
```
C3=36  CS3=37 D3=38  DS3=39 E3=40  F3=41  FS3=42 G3=43  GS3=44 A3=45  AS3=46 B3=47
```

### Octave 4 (MIDI 60-71) - Middle C Range ⭐
```
C4=60  CS4=61 D4=62  DS4=63 E4=64  F4=65  FS4=66 G4=67  GS4=68 A4=69  AS4=70 B4=71
```

### Octave 5 (MIDI 72-83) - Mid-High
```
C5=72  CS5=73 D5=74  DS5=75 E5=76  F5=77  FS5=78 G5=79  GS5=80 A5=81  AS5=82 B5=83
```

### Octave 6 (MIDI 84-95) - High
```
C6=84  CS6=85 D6=86  DS6=87 E6=88  F6=89  FS6=90 G6=91  GS6=92 A6=93  AS6=94 B6=95
```

### Octave 7 (MIDI 96-107) - Very High
```
C7=96  CS7=97 D7=98  DS7=99 E7=100 F7=101 FS7=102 G7=103 GS7=104 A7=105 AS7=106 B7=107
```

## ZMK Note Names

### Syntax
```
&midi_note [NOTE][OCTAVE]
```

### Natural Notes
- C, D, E, F, G, A, B (same in all octaves)

### Sharps
- CS, DS, FS, GS, AS (C#, D#, F#, G#, A# in standard notation)
- Note: E# = F, B# = C (enharmonically)

### Flats
- No flat notation - use enharmonic equivalent sharps
- Example: Db = CS, Eb = DS, etc.

## Common Use Cases

### Piano/Keyboard Feel
```
Left hand:  C3, D3, E3, F3, G3, A3, B3, C4
Right hand: C5, D5, E5, F5, G5, A5, B5, C6
```

### Pentatonic Scale (C Major)
```
C4, D4, E4, G4, A4, C5, D5, E5, G5, A5, C6, D6, E6, G6, A6, C7
```

### Chromatic (all notes)
```
C3, CS3, D3, DS3, E3, F3, FS3, G3, GS3, A3, AS3, B3, C4, ...
```

### Drum Pads (low notes)
```
C1, D1, E1, F1, G1, A1, B1, C2, D2, E2, F2, G2, A2, B2, C3, D3
```

### Bass Notes
```
C2, D2, E2, F2, G2, A2, B2, C3
```

### Lead/Melody
```
C5, D5, E5, F5, G5, A5, B5, C6, D6, E6, F6, G6, A6, B6, C7
```

## All 128 MIDI Notes

```
Octave -1 (MIDI 0-11):
C-1=0   CS-1=1  D-1=2   DS-1=3  E-1=4   F-1=5   FS-1=6  G-1=7   GS-1=8  A-1=9   AS-1=10 B-1=11

Octave 0 (MIDI 12-23):
C0=12   CS0=13  D0=14   DS0=15  E0=16   F0=17   FS0=18  G0=19   GS0=20  A0=21   AS0=22  B0=23

Octave 1 (MIDI 24-35):
C1=24   CS1=25  D1=26   DS1=27  E1=28   F1=29   FS1=30  G1=31   GS1=32  A1=33   AS1=34  B1=35

Octave 2 (MIDI 36-47):
C2=36   CS2=37  D2=38   DS2=39  E2=40   F2=41   FS2=42  G2=43   GS2=44  A2=45   AS2=46  B2=47

Octave 3 (MIDI 48-59):
C3=48   CS3=49  D3=50   DS3=51  E3=52   F3=53   FS3=54  G3=55   GS3=56  A3=57   AS3=58  B3=59

Octave 4 (MIDI 60-71):  ← MIDDLE C = 60
C4=60   CS4=61  D4=62   DS4=63  E4=64   F4=65   FS4=66  G4=67   GS4=68  A4=69   AS4=70  B4=71

Octave 5 (MIDI 72-83):
C5=72   CS5=73  D5=74   DS5=75  E5=76   F5=77   FS5=78  G5=79   GS5=80  A5=81   AS5=82  B5=83

Octave 6 (MIDI 84-95):
C6=84   CS6=85  D6=86   DS6=87  E6=88   F6=89   FS6=90  G6=91   GS6=92  A6=93   AS6=94  B6=95

Octave 7 (MIDI 96-107):
C7=96   CS7=97  D7=98   DS7=99  E7=100  F7=101  FS7=102 G7=103  GS7=104 A7=105  AS7=106 B7=107

Octave 8 (MIDI 108-119):
C8=108  CS8=109 D8=110  DS8=111 E8=112  F8=113  FS8=114 G8=115  GS8=116 A8=117  AS8=118 B8=119

Octave 9 (MIDI 120-127):
C9=120  CS9=121 D9=122  DS9=123 E9=124  F9=125  FS9=126 G9=127  (end of MIDI range)
```

## Frequency Reference

Note frequencies (in Hz) for reference:

```
C4 (60)  = 261.63 Hz  (Middle C)
A4 (69)  = 440.00 Hz  (Concert pitch reference)
C5 (72)  = 523.25 Hz
C6 (84)  = 1046.50 Hz
```

## ZMK Keymap Examples

### Example 1: Simple Chromatic 4x4
```c
midi_layer {
    bindings = <
        &midi_note C4  &midi_note CS4  &midi_note D4  &midi_note DS4
        &midi_note E4  &midi_note F4   &midi_note FS4 &midi_note G4
        &midi_note GS4 &midi_note A4   &midi_note AS4 &midi_note B4
        &midi_note C5  &midi_note CS5  &midi_note D5  &midi_note DS5
    >;
};
```

### Example 2: Pentatonic Scale
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

### Example 3: Drum Pads
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

## Tips & Tricks

1. **Middle C Reference**: C4 (MIDI 60) is always the reference point
2. **Octave Math**: Each octave = +12 MIDI notes
3. **Semitone**: S = +1 MIDI note (e.g., C to C# = +1)
4. **Enharmonic**: Db = CS, Eb = DS, Gb = FS, Ab = GS, Bb = AS
5. **Layer Transposition**: Use same intervals, different starting notes

## For Musicians

If you know music theory:
- **Octave 4** (60-71) = Standard notation middle octave
- **A4** = 69 (concert pitch, 440 Hz)
- **Semitone** = 1 MIDI step
- **Whole step** = 2 MIDI steps
- **Perfect 4th** = 5 MIDI steps
- **Perfect 5th** = 7 MIDI steps

## Calculator

To find the MIDI note number:
```
MIDI Note = (Octave + 1) × 12 + [C=0, CS=1, D=2, DS=3, E=4, F=5, FS=6, G=7, GS=8, A=9, AS=10, B=11]

Example: C4
= (4 + 1) × 12 + 0 = 60 ✓

Example: A4
= (4 + 1) × 12 + 9 = 69 ✓

Example: G3
= (3 + 1) × 12 + 7 = 55 ✓
```
