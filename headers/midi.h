/*
 * midi.h
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#ifndef HEADERS_MIDI_H_
#define HEADERS_MIDI_H_

#include <stdio.h>
#include <stdint.h>

typedef enum MIDIStatus_t
{
    MIDI_NOTE_OFF        = 0x80,
    MIDI_NOTE_ON         = 0x90,
    MIDI_KEY_PRESSURE    = 0xA0,
    MIDI_CONTROL_CHANGE  = 0xB0,
    MIDI_PROGRAM_CHANGE  = 0xC0,
    MIDI_CHANEL_PRESSURE = 0xD0,
    MIDI_PITCH_BEND      = 0xE0,
    MIDI_SYSTEM          = 0xF0
} MIDIStatus_t;

typedef enum MIDISystem_t
{
    MIDI_SYSTEM_EXCLUSIVE      = 0xF0,
    MTC_QUARTER_FRAME          = 0xF1,
    MIDI_SONG_POSITION_POINTER = 0xF2,
    MIDI_SONG_SELECT           = 0xF3,
    MIDI_TUNE_REQUEST          = 0xF6,
    MIDI_EOX                   = 0xF7,
    MIDI_TIMING_CLOCK          = 0xF8,
    MIDI_START                 = 0xFA,
    MIDI_CONTINUE              = 0xFB,
    MIDI_STOP                  = 0xFC,
    MIDI_ACTIVE_SENSING        = 0xFE,
    MIDI_SYSTEM_RESET          = 0xFF
} MIDISystemCommon_t;

typedef enum MIDISysExID_t
{
    MIDI_ID_YAMAHA         = 0x43,
    MIDI_ID_NON_COMMERCIAL = 0x7D,
    MIDI_ID_NON_REAL_TIME  = 0x7E,
    MIDI_ID_REAL_TIME      = 0x7F
} MIDISysExID_t;

uint8_t* get_sysex_payload(FILE* file, int* size_p);


#endif /* HEADERS_MIDI_H_ */
