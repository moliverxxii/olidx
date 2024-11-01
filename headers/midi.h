/*
 * midi.h
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#ifndef HEADERS_MIDI_H_
#define HEADERS_MIDI_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MIDI_DATA_MASK 0x7F
#define MIDI_DATA_BITS    7

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

typedef enum Note_t
{
 NOTE_C = 0,
 NOTE_CS,
 NOTE_D,
 NOTE_DS,
 NOTE_E,
 NOTE_F,
 NOTE_FS,
 NOTE_G,
 NOTE_GS,
 NOTE_A,
 NOTE_AS,
 NOTE_B,
 NOTE_COUNT
} Note_t;

//TODO: MIDI NOTES
typedef enum MIDINote_t
{
    MIDI_NOTE_C_m2 = 0,
    MIDI_NOTE_Csm2,
    MIDI_NOTE_D_m2,
    MIDI_NOTE_Dsm2,
    MIDI_NOTE_E_m2,
    MIDI_NOTE_F_m2,
    MIDI_NOTE_Fsm2,
    MIDI_NOTE_G_m2,
    MIDI_NOTE_Gsm2,
    MIDI_NOTE_A_m2,
    MIDI_NOTE_Asm2,
    MIDI_NOTE_B_m2,
    MIDI_NOTE_C_m1,
    MIDI_NOTE_Csm1,
    MIDI_NOTE_D_m1,
    MIDI_NOTE_Dsm1,
    MIDI_NOTE_E_m1,
    MIDI_NOTE_F_m1,
    MIDI_NOTE_Fsm1,
    MIDI_NOTE_G_m1,
    MIDI_NOTE_Gsm1,
    MIDI_NOTE_A_m1,
    MIDI_NOTE_Asm1,
    MIDI_NOTE_B_m1,
    MIDI_NOTE_C__0,
    MIDI_NOTE_Cs_0,
    MIDI_NOTE_D__0,
    MIDI_NOTE_Ds_0,
    MIDI_NOTE_E__0,
    MIDI_NOTE_F__0,
    MIDI_NOTE_Fs_0,
    MIDI_NOTE_G__0,
    MIDI_NOTE_Gs_0,
    MIDI_NOTE_A__0,
    MIDI_NOTE_As_0,
    MIDI_NOTE_B__0,
    MIDI_NOTE_C__1,
    MIDI_NOTE_Cs_1,
    MIDI_NOTE_D__1,
    MIDI_NOTE_Ds_1,
    MIDI_NOTE_E__1,
    MIDI_NOTE_F__1,
    MIDI_NOTE_Fs_1,
    MIDI_NOTE_G__1,
    MIDI_NOTE_Gs_1,
    MIDI_NOTE_A__1,
    MIDI_NOTE_As_1,
    MIDI_NOTE_B__1,
    MIDI_NOTE_C__2,
    MIDI_NOTE_Cs_2,
    MIDI_NOTE_D__2,
    MIDI_NOTE_Ds_2,
    MIDI_NOTE_E__2,
    MIDI_NOTE_F__2,
    MIDI_NOTE_Fs_2,
    MIDI_NOTE_G__2,
    MIDI_NOTE_Gs_2,
    MIDI_NOTE_A__2,
    MIDI_NOTE_As_2,
    MIDI_NOTE_B__2,
    MIDI_NOTE_C__3,
    MIDI_NOTE_Cs_3,
    MIDI_NOTE_D__3,
    MIDI_NOTE_Ds_3,
    MIDI_NOTE_E__3,
    MIDI_NOTE_F__3,
    MIDI_NOTE_Fs_3,
    MIDI_NOTE_G__3,
    MIDI_NOTE_Gs_3,
    MIDI_NOTE_A__3,
    MIDI_NOTE_As_3,
    MIDI_NOTE_B__3,
    MIDI_NOTE_C__4,
    MIDI_NOTE_Cs_4,
    MIDI_NOTE_D__4,
    MIDI_NOTE_Ds_4,
    MIDI_NOTE_E__4,
    MIDI_NOTE_F__4,
    MIDI_NOTE_Fs_4,
    MIDI_NOTE_G__4,
    MIDI_NOTE_Gs_4,
    MIDI_NOTE_A__4,
    MIDI_NOTE_As_4,
    MIDI_NOTE_B__4,
    MIDI_NOTE_C__5,
    MIDI_NOTE_Cs_5,
    MIDI_NOTE_D__5,
    MIDI_NOTE_Ds_5,
    MIDI_NOTE_E__5,
    MIDI_NOTE_F__5,
    MIDI_NOTE_Fs_5,
    MIDI_NOTE_G__5,
    MIDI_NOTE_Gs_5,
    MIDI_NOTE_A__5,
    MIDI_NOTE_As_5,
    MIDI_NOTE_B__5,
    MIDI_NOTE_C__6,
    MIDI_NOTE_Cs_6,
    MIDI_NOTE_D__6,
    MIDI_NOTE_Ds_6,
    MIDI_NOTE_E__6,
    MIDI_NOTE_F__6,
    MIDI_NOTE_Fs_6,
    MIDI_NOTE_G__6,
    MIDI_NOTE_Gs_6,
    MIDI_NOTE_A__6,
    MIDI_NOTE_As_6,
    MIDI_NOTE_B__6,
    MIDI_NOTE_C__7,
    MIDI_NOTE_Cs_7,
    MIDI_NOTE_D__7,
    MIDI_NOTE_Ds_7,
    MIDI_NOTE_E__7,
    MIDI_NOTE_F__7,
    MIDI_NOTE_Fs_7,
    MIDI_NOTE_G__7,
    MIDI_NOTE_Gs_7,
    MIDI_NOTE_A__7,
    MIDI_NOTE_As_7,
    MIDI_NOTE_B__7,
    MIDI_NOTE_C__8,
    MIDI_NOTE_Cs_8,
    MIDI_NOTE_D__8,
    MIDI_NOTE_Ds_8,
    MIDI_NOTE_E__8,
    MIDI_NOTE_F__8,
    MIDI_NOTE_Fs_8,
    MIDI_NOTE_G__8,
    MIDI_NOTE_COUNT
} MIDINote_t;

#define MIDI_NOTE_NUMBER(NOTE,OCTAVE)\
    (NOTE_COUNT * OCTAVE + MIDI_NOTE_C__0 + NOTE)

#define NOTE_NUMBER(MIDI_NOTE)\
    (MIDI_NOTE%NOTE_COUNT)

//TODO: check that crap. haha!
#define NOTE_OCTAVE(MIDI_NOTE)\
    ((MIDI_NOTE - MIDI_NOTE%NOTE_COUNT - MIDI_NOTE_C__O)/NOTE_COUNT)

extern const char* const MIDI_SYSEX_EXTENSION;

/**
 * returns contents between the MIDI SysEx start and EOX bytes (excluded).
 * @param file_p: the input stream.
 * @param size_p: if SysEx: the length of the payload,
 *                if file not over: 0,
 *                if end of file: EOF.
 */
uint8_t* midi_get_next_sysex_payload(FILE* file_p, int* size_p);

/**
 * returns number of byt written
 */
int midi_write_sysex_payload(FILE* file_p,
                        const uint8_t* payload,
                        size_t payload_length);

#endif /* HEADERS_MIDI_H_ */
