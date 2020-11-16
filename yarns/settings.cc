// Copyright 2013 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Parameter definitions.

#include "yarns/settings.h"
#include "yarns/resources.h"
#include "yarns/clock_division.h"

#include <algorithm>
#include <cstring>

namespace yarns {

const char* const layout_values[] = {
  "1M", "2M", "4M", "2P", "4P", "2>", "4>", "8>", "4T", "4V", "31", "22", "21", "*2"
};

const char* const midi_out_mode_values[] = {
  "OFF", "THRU", "ARP/SEQ"
};

const char* const boolean_values[] = {
  "OFF", "ON"
};

const char* const voicing_allocation_mode_values[] = {
  "MONO", "POLY", "CYCLIC", "RANDOM", "VELO", "SORTED", "U1 UNISON",
  "U2 UNISON 2", "STEAL MOST RECENT"
};

const char* const sequencer_arp_direction_values[] = {
  "LINEAR", "BOUNCE", "RANDOM", "HIT SEQ", "WRAP SEQ"
};

const char* const voicing_aux_cv_values[] = {
  "VELOCITY", "MODULATION", "AT", "BREATH", "PEDAL", "BEND", "VIBRATO LFO", "LFO"
};

const char* const legato_mode_values[] = {
  "OFF", "AUTO PORTAMENTO", "ON"
};

const char* const voicing_oscillator_values[] = {
  "OF", "\x88\x88", "\x8C\x8C", "\x8C_", "/\\", "SINE", "**"
};

const char* const voicing_allocation_priority_values[] = {
  "LAST", "LOW", "HIGH", "FIRST"
};

const char* const trigger_shape_values[] = {
  "SQ", "LINEAR", "EXPO", "RING", "STEP", "BURST"
};

const char* const note_values[] = {
  "C ", "Db", "D", "Eb", "E ", "F ", "Gb", "G ", "Ab", "A ", "Bb", "B "
};

const char* const tuning_system_values[] = {
  "EQUAL TEMPERAMENT",
  "JUST INTONATION",
  "PYTHAGOREAN",
  "EB 1/4",
  "E 1/4",
  "EA 1/4",
  "01 BHAIRAV",
  "02 GUNAKRI",
  "03 MARWA",
  "04 SHREE",
  "05 PURVI",
  "06 BILAWAL",
  "07 YAMAN",
  "08 KAFI",
  "09 BHIMPALASREE",
  "10 DARBARI",
  "11 BAGESHREE",
  "12 RAGESHREE",
  "13 KHAMAJ",
  "14 MI MAL",
  "15 PARAMESHWARI",
  "16 RANGESHWARI",
  "17 GANGESHWARI",
  "18 KAMESHWARI",
  "19 PA KAFI",
  "20 NATBHAIRAV",
  "21 M.KAUNS",
  "22 BAIRAGI",
  "23 B.TODI",
  "24 CHANDRADEEP",
  "25 KAUSHIK TODI",
  "26 JOGESHWARI",
  "27 RASIA",
  "CUSTOM"
};

const char* const sequencer_play_mode_values[] = {
  "M",
  "A",
  "R",
};

const char* const sequencer_clock_quantization_values[] = {
  "LOOP",
  "STEP"
};

const char* const sequencer_input_response_values[] = {
  "TRANSPOSE", "REPLACE", "DIRECT", "OFF"
};

const char* const sustain_mode_values[] = {
  "NORMAL",
  // "SOSTENUTO",
  "LATCH",
  "MOMENTARY LATCH",
  "OFF"
};

const char* const vibrato_control_source_values[] = {
  "MODWHEEL", "AFTERTOUCH"
};

const char* const tuning_factor_values[] = {
  "OFF",
  "0 ",
  "18 1/8",
  "14 1/4",
  "38 3/8",
  "12 1/2",
  "58 5/8",
  "34 3/4",
  "78 7/8",
  "1  1/1",
  "54 5/4",
  "32 3/2",
  "2  2/1",
  "ALPHA"
};

/* static */
const Setting Settings::settings_[] = {
  {
    "SE", "SETUP",
    SETTING_DOMAIN_MULTI, { 0, 0 },
    SETTING_UNIT_UINT8, 0, 0, NULL,
    0, 0,
  },
  {
    "LA", "LAYOUT",
    SETTING_DOMAIN_MULTI, { MULTI_LAYOUT, 0 },
    SETTING_UNIT_ENUMERATION, LAYOUT_MONO, LAYOUT_LAST - 1, layout_values,
    0, 1,
  },
  {
    "PA", "PART",
    SETTING_DOMAIN_GLOBAL, { GLOBAL_ACTIVE_PART, 0 },
    SETTING_UNIT_INDEX, 0, 3, NULL,
    0, 0,
  },
  {
    "PA", "PART",
    SETTING_DOMAIN_GLOBAL, { GLOBAL_ACTIVE_PART, 0 },
    SETTING_UNIT_INDEX, 0, 2, NULL,
    0, 0,
  },
  {
    "PA", "PART",
    SETTING_DOMAIN_GLOBAL, { GLOBAL_ACTIVE_PART, 0 },
    SETTING_UNIT_INDEX, 0, 1, NULL,
    0, 0,
  },
  {
    "TE", "TEMPO",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_TEMPO, 0 },
    SETTING_UNIT_TEMPO, 39, 240, NULL,
    0, 2,
  },
  {
    "SW", "SWING",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_SWING, 0 },
    SETTING_UNIT_UINT8, 0, 99, NULL,
    0, 3,
  },
  {
    "I/", "INPUT CLK DIV",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_INPUT_DIVISION, 0 },
    SETTING_UNIT_UINT8, 1, 4, NULL,
    0, 0,
  },
  {
    "O/", "OUTPUT CLK DIV",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_OUTPUT_DIVISION, 0 },
    SETTING_UNIT_ENUMERATION, 0, clock_division::count - 1, clock_division::display,
    0, 0,
  },
  {
    "B-", "BAR DURATION",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_BAR_DURATION, 0 },
    SETTING_UNIT_BAR_DURATION, 0, kMaxBarDuration + 1, NULL,
    0, 0,
  },
  {
    "NU", "NUDGE 1ST TICK",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_NUDGE_FIRST_TICK, 0 },
    SETTING_UNIT_ENUMERATION, 0, 1, boolean_values,
    0, 0,
  },
  {
    "MS", "CLOCK MANUAL START",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_MANUAL_START, 0 },
    SETTING_UNIT_ENUMERATION, 0, 1, boolean_values,
    0, 0,
  },
  {
    "C>", "CLOCK OUTPUT",
    SETTING_DOMAIN_MULTI, { MULTI_CLOCK_OVERRIDE, 0 },
    SETTING_UNIT_ENUMERATION, 0, 1, boolean_values,
    0, 0,
  },
  {
    "CH", "CHANNEL",
    SETTING_DOMAIN_PART, { PART_MIDI_CHANNEL, 0 },
    SETTING_UNIT_MIDI_CHANNEL, 0, 16, NULL,
    0, 4,
  },
  {
    "N>", "NOTE>",
    SETTING_DOMAIN_PART, { PART_MIDI_MIN_NOTE, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    16, 5,
  },
  {
    "N<", "NOTE<",
    SETTING_DOMAIN_PART, { PART_MIDI_MAX_NOTE, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    17, 6,
  },
  {
    "NO", "NOTE",
    SETTING_DOMAIN_PART, { PART_MIDI_MIN_NOTE, PART_MIDI_MAX_NOTE },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    0, 0,
  },
  {
    "V>", "VELO>",
    SETTING_DOMAIN_PART, { PART_MIDI_MIN_VELOCITY, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    0, 0,
  },
  {
    "V<", "VELO<",
    SETTING_DOMAIN_PART, { PART_MIDI_MAX_VELOCITY, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    0, 0,
  },
  {
    ">>", "OUTPUT MIDI MODE",
    SETTING_DOMAIN_PART, { PART_MIDI_OUT_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, 2, midi_out_mode_values,
    0, 7,
  },
  {
    "IT", "INPUT TRANSPOSE OCTAVES",
    SETTING_DOMAIN_PART, { PART_MIDI_TRANSPOSE_OCTAVES, 0 },
    SETTING_UNIT_INT8, -10, 10, NULL,
    0, 0,
  },
  {
    "VO", "VOICING",
    SETTING_DOMAIN_PART, { PART_VOICING_ALLOCATION_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, VOICE_ALLOCATION_MODE_LAST - 1,
    voicing_allocation_mode_values,
    18, 8,
  },
  {
    "NP", "NOTE PRIORITY",
    SETTING_DOMAIN_PART, { PART_VOICING_ALLOCATION_PRIORITY, 0 },
    SETTING_UNIT_ENUMERATION, 0, 3, voicing_allocation_priority_values,
    19, 9,
  },
  {
    "PO", "PORTAMENTO",
    SETTING_DOMAIN_PART, { PART_VOICING_PORTAMENTO, 0 },
    SETTING_UNIT_PORTAMENTO, 0, 127, NULL,
    5, 10,
  },
  {
    "LG", "LEGATO MODE",
    SETTING_DOMAIN_PART, { PART_VOICING_LEGATO_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, LEGATO_MODE_LAST - 1, legato_mode_values,
    20, 11,
  },
  {
    "BR", "BEND RANGE",
    SETTING_DOMAIN_PART, { PART_VOICING_PITCH_BEND_RANGE, 0 },
    SETTING_UNIT_UINT8, 0, 24, NULL,
    21, 12,
  },
  {
    "VR", "VIBRATO RANGE",
    SETTING_DOMAIN_PART, { PART_VOICING_VIBRATO_RANGE, 0 },
    SETTING_UNIT_UINT8, 0, 12, NULL,
    22, 13,
  },
  {
    "VS", "VIBRATO SPEED",
    SETTING_DOMAIN_PART, { PART_VOICING_MODULATION_RATE, 0 },
    SETTING_UNIT_VIBRATO_SPEED, 0, 100 + clock_division::count - 1, NULL,
    23, 14,
  },
  {
    "VI", "VIBRATO INITIAL",
    SETTING_DOMAIN_PART, { PART_VOICING_VIBRATO_INITIAL, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    0, 0,
  },
  {
    "VC", "VIBRATO CONTROL SOURCE",
    SETTING_DOMAIN_PART, { PART_VOICING_VIBRATO_CONTROL_SOURCE, 0 },
    SETTING_UNIT_ENUMERATION, 0, VIBRATO_CONTROL_SOURCE_LAST - 1, vibrato_control_source_values,
    0, 0,
  },
  {
    "TT", "TRANSPOSE",
    SETTING_DOMAIN_PART, { PART_VOICING_TUNING_TRANSPOSE, 0 },
    SETTING_UNIT_INT8, -36, 36, NULL,
    24, 15,
  },
  {
    "TF", "FINE TUNING",
    SETTING_DOMAIN_PART, { PART_VOICING_TUNING_FINE, 0 },
    SETTING_UNIT_INT8, -64, 64, NULL,
    25, 16,
  },
  {
    "TR", "TUNING ROOT",
    SETTING_DOMAIN_PART, { PART_VOICING_TUNING_ROOT, 0 },
    SETTING_UNIT_ENUMERATION, 0, 11, note_values,
    26, 17,
  },
  {
    "TS", "TUNING SYSTEM",
    SETTING_DOMAIN_PART, { PART_VOICING_TUNING_SYSTEM, 0 },
    SETTING_UNIT_ENUMERATION, 0, TUNING_SYSTEM_LAST - 1,
    tuning_system_values,
    27, 18,
  },
  {
    "T-", "TRIG DURATION",
    SETTING_DOMAIN_PART, { PART_VOICING_TRIGGER_DURATION, 0 },
    SETTING_UNIT_UINT8, 1, 99, NULL,
    28, 19,
  },
  {
    "T*", "TRIG VELOCITY SCALE",
    SETTING_DOMAIN_PART, { PART_VOICING_TRIGGER_SCALE, 0 },
    SETTING_UNIT_ENUMERATION, 0, 1, boolean_values,
    29, 20,
  },
  {
    "T\x88", "TRIG SHAPE",
    SETTING_DOMAIN_PART, { PART_VOICING_TRIGGER_SHAPE, 0 },
    SETTING_UNIT_ENUMERATION, 0, 5, trigger_shape_values,
    30, 21,
  },
  {
    "CV", "CV OUT",
    SETTING_DOMAIN_PART, { PART_VOICING_AUX_CV, 0 },
    SETTING_UNIT_ENUMERATION, 0, MOD_AUX_LAST - 1, voicing_aux_cv_values,
    31, 22,
  },
  {
    "3>", "CV OUT 3",
    SETTING_DOMAIN_PART, { PART_VOICING_AUX_CV, 0 },
    SETTING_UNIT_ENUMERATION, 0, MOD_AUX_LAST - 1, voicing_aux_cv_values,
    31, 22,
  },
  {
    "4>", "CV OUT 4",
    SETTING_DOMAIN_PART, { PART_VOICING_AUX_CV_2, 0 },
    SETTING_UNIT_ENUMERATION, 0, MOD_AUX_LAST - 1, voicing_aux_cv_values,
    72, 0,
  },
  {
    "OS", "OSC WAVE",
    SETTING_DOMAIN_PART, { PART_VOICING_AUDIO_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, AUDIO_MODE_LAST - 1, voicing_oscillator_values,
    71, 23,
  },
  {
    "PW", "OSC PW INITIAL",
    SETTING_DOMAIN_PART, { PART_VOICING_OSCILLATOR_PW_INITIAL, 0 },
    SETTING_UNIT_UINT8, 0, 127, NULL,
    0, 0,
  },
  {
    "PM", "OSC PW MOD",
    SETTING_DOMAIN_PART, { PART_VOICING_OSCILLATOR_PW_MOD, 0 },
    SETTING_UNIT_INT8, -64, 64, NULL,
    0, 0,
  },
  {
    "\x8F""A", "ENV ATTACK TIME",
    SETTING_DOMAIN_PART, { PART_VOICING_ENVELOPE_ATTACK, 0 },
    SETTING_UNIT_INT8, 0, 127, NULL,
    0, 0,
  },
  {
    "\x8F""D", "ENV DECAY TIME",
    SETTING_DOMAIN_PART, { PART_VOICING_ENVELOPE_DECAY, 0 },
    SETTING_UNIT_INT8, 0, 127, NULL,
    0, 0,
  },
  {
    "\x8F""S", "ENV SUSTAIN LEVEL",
    SETTING_DOMAIN_PART, { PART_VOICING_ENVELOPE_SUSTAIN, 0 },
    SETTING_UNIT_INT8, 0, 127, NULL,
    0, 0,
  },
  {
    "\x8F""R", "ENV RELEASE TIME",
    SETTING_DOMAIN_PART, { PART_VOICING_ENVELOPE_RELEASE, 0 },
    SETTING_UNIT_INT8, 0, 127, NULL,
    0, 0,
  },
  {
    "C/", "CLOCK DIV",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_CLOCK_DIVISION, 0 },
    SETTING_UNIT_ENUMERATION, 0, clock_division::count - 1, clock_division::display,
    102, 24,
  },
  {
    "G-", "GATE LENGTH",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_GATE_LENGTH, 0 },
    SETTING_UNIT_UINT8, 1, 48, NULL,
    103, 25,
  },
  {
    "AR", "ARP RANGE",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_ARP_RANGE, 0 },
    SETTING_UNIT_UINT8, 1, 4, NULL,
    104, 26,
  },
  {
    "AD", "ARP DIRECTION",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_ARP_DIRECTION, 0 },
    SETTING_UNIT_ENUMERATION, 0, ARPEGGIATOR_DIRECTION_LAST - 1,
    sequencer_arp_direction_values,
    0, 0,
  },
  {
    "AP", "ARP PATTERN",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_ARP_PATTERN, 0 },
    SETTING_UNIT_INDEX, 0, LUT_ARPEGGIATOR_PATTERNS_SIZE - 1, NULL,
    106, 28,
  },
  {
    "RP", "RHYTHMIC PATTERN",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_ARP_PATTERN, 0 },
    SETTING_UNIT_INDEX, 0, LUT_ARPEGGIATOR_PATTERNS_SIZE - 1, NULL,
    0, 0,
  },
  {
    "E-", "EUCLIDEAN LENGTH",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_EUCLIDEAN_LENGTH, 0 },
    SETTING_UNIT_UINT8, 0, 32, NULL,
    107, 29,
  },
  {
    "EF", "EUCLIEAN FILL",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_EUCLIDEAN_FILL, 0 },
    SETTING_UNIT_UINT8, 0, 32, NULL,
    108, 30,
  },
  {
    "ER", "EUCLIEAN ROTATE",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_EUCLIDEAN_ROTATE, 0 },
    SETTING_UNIT_UINT8, 0, 32, NULL,
    109, 31,
  },
  {
    "SP", "SEQUENCER PLAY MODE",
    SETTING_DOMAIN_PART, { PART_MIDI_PLAY_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, PLAY_MODE_LAST - 1, sequencer_play_mode_values,
    0, 0,
  },
  {
    "SI", "SEQUENCER INPUT RESPONSE",
    SETTING_DOMAIN_PART, { PART_MIDI_INPUT_RESPONSE, 0 },
    SETTING_UNIT_ENUMERATION, 0, SEQUENCER_INPUT_RESPONSE_LAST - 1, sequencer_input_response_values,
    0, 0,
  },
  {
    "RQ", "RECORD TIME QUANTIZATION",
    SETTING_DOMAIN_PART, { PART_SEQUENCER_CLOCK_QUANTIZATION, 0 },
    SETTING_UNIT_ENUMERATION, 0, 1, sequencer_clock_quantization_values,
    0, 0,
  },
  {
    "SU", "SUSTAIN MODE",
    SETTING_DOMAIN_PART, { PART_MIDI_SUSTAIN_MODE, 0 },
    SETTING_UNIT_ENUMERATION, 0, SUSTAIN_MODE_LAST - 1, sustain_mode_values,
    0, 0,
  },
  {
    "RC", "REMOTE CONTROL CHANNEL",
    SETTING_DOMAIN_MULTI, { MULTI_REMOTE_CONTROL_CHANNEL, 0 },
    SETTING_UNIT_MIDI_CHANNEL_OFF, 0, 16, NULL,
    0, 0,
  },
  {
    "T*", "TUNING FACTOR",
    SETTING_DOMAIN_PART, { PART_VOICING_TUNING_FACTOR, 0 },
    SETTING_UNIT_ENUMERATION, 0, 13,
    tuning_factor_values,
    0, 0,
  }
};

#define MENU_LAYOUT_CLOCK \
  SETTING_LAYOUT, \
  SETTING_CLOCK_TEMPO, \
  SETTING_CLOCK_SWING, \
  SETTING_CLOCK_INPUT_DIVISION, \
  SETTING_CLOCK_OUTPUT_DIVISION, \
  SETTING_CLOCK_BAR_DURATION, \
  SETTING_CLOCK_NUDGE_FIRST_TICK, \
  SETTING_CLOCK_MANUAL_START

#define MENU_MIDI \
  SETTING_MIDI_CHANNEL, \
  SETTING_MIDI_MIN_VELOCITY, \
  SETTING_MIDI_MAX_VELOCITY, \
  SETTING_MIDI_OUT_MODE, \
  SETTING_MIDI_SUSTAIN_MODE

#define MENU_VOICING_ALLOCATION_MONO \
  SETTING_VOICING_ALLOCATION_PRIORITY, \
  SETTING_VOICING_LEGATO_MODE

#define MENU_VOICING_ALLOCATION_POLY \
  SETTING_VOICING_ALLOCATION_MODE, \
  SETTING_VOICING_ALLOCATION_PRIORITY

#define MENU_VOICING_ALLOCATION_MIXED \
  SETTING_VOICING_ALLOCATION_MODE, \
  MENU_VOICING_ALLOCATION_MONO

#define MENU_MODULATION \
  SETTING_VOICING_PITCH_BEND_RANGE, \
  SETTING_VOICING_VIBRATO_RANGE

#define MENU_EUCLIDEAN \
  SETTING_SEQUENCER_EUCLIDEAN_LENGTH, \
  SETTING_SEQUENCER_EUCLIDEAN_FILL, \
  SETTING_SEQUENCER_EUCLIDEAN_ROTATE

#define MENU_OUTPUT \
  SETTING_VOICING_AUDIO_MODE, \
  SETTING_SEQUENCER_CLOCK_QUANTIZATION, \
  SETTING_SEQUENCER_INPUT_RESPONSE, \
  SETTING_VOICING_TUNING_SYSTEM, \
  SETTING_VOICING_TUNING_ROOT, \
  SETTING_VOICING_TUNING_FACTOR

#define MENU_END \
  SETTING_REMOTE_CONTROL_CHANNEL, \
  SETTING_LAST

const SettingIndex menu_live[] = {
  SETTING_SETUP_SUBMENU,
  SETTING_MIDI_TRANSPOSE_OCTAVES,
  SETTING_VOICING_PORTAMENTO,
  SETTING_VOICING_MODULATION_RATE,
  SETTING_VOICING_VIBRATO_INITIAL,
  SETTING_VOICING_OSCILLATOR_PW_INITIAL,
  SETTING_VOICING_OSCILLATOR_PW_MOD,
  SETTING_VOICING_TUNING_TRANSPOSE,
  SETTING_VOICING_TUNING_FINE,
  SETTING_SEQUENCER_CLOCK_DIVISION,
  SETTING_SEQUENCER_GATE_LENGTH,
  SETTING_SEQUENCER_ARP_RANGE,
  SETTING_SEQUENCER_ARP_DIRECTION,
  SETTING_SEQUENCER_ARP_PATTERN,
  MENU_EUCLIDEAN,
  SETTING_LAST
};

const SettingIndex menu_live_para[] = {
  SETTING_SETUP_SUBMENU,
  SETTING_MIDI_TRANSPOSE_OCTAVES,
  SETTING_VOICING_PORTAMENTO,
  SETTING_VOICING_MODULATION_RATE,
  SETTING_VOICING_VIBRATO_INITIAL,
  SETTING_VOICING_OSCILLATOR_PW_INITIAL,
  SETTING_VOICING_OSCILLATOR_PW_MOD,
  SETTING_VOICING_ENVELOPE_ATTACK,
  SETTING_VOICING_ENVELOPE_DECAY,
  SETTING_VOICING_ENVELOPE_SUSTAIN,
  SETTING_VOICING_ENVELOPE_RELEASE,
  SETTING_VOICING_TUNING_TRANSPOSE,
  SETTING_VOICING_TUNING_FINE,
  SETTING_SEQUENCER_CLOCK_DIVISION,
  SETTING_SEQUENCER_GATE_LENGTH,
  SETTING_SEQUENCER_ARP_RANGE,
  SETTING_SEQUENCER_ARP_DIRECTION,
  SETTING_SEQUENCER_ARP_PATTERN,
  MENU_EUCLIDEAN,
  SETTING_LAST
};

const SettingIndex menu_live_quad_triggers[] = {
  SETTING_VOICING_TRIGGER_DURATION,
  SETTING_VOICING_TRIGGER_SCALE,
  SETTING_VOICING_TRIGGER_SHAPE,
  SETTING_SEQUENCER_CLOCK_DIVISION,
  SETTING_SEQUENCER_RHYTHM_PATTERN,
  MENU_EUCLIDEAN,
  SETTING_LAST
};

const SettingIndex mono_menu[] = {
  MENU_LAYOUT_CLOCK,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_MONO,
  MENU_MODULATION,
  SETTING_VOICING_TRIGGER_DURATION,
  SETTING_VOICING_CV_OUT_3,
  SETTING_VOICING_CV_OUT_4,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex dual_mono_menu[] = {
  MENU_LAYOUT_CLOCK,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_MONO,
  MENU_MODULATION,
  SETTING_VOICING_CV_OUT,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex quad_mono_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_CLOCK_OVERRIDE,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_MONO,
  MENU_MODULATION,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex dual_poly_menu[] = {
  MENU_LAYOUT_CLOCK,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_POLY,
  MENU_MODULATION,
  SETTING_VOICING_CV_OUT_3,
  SETTING_VOICING_CV_OUT_4,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex quad_poly_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_CLOCK_OVERRIDE,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_POLY,
  MENU_MODULATION,
  MENU_OUTPUT,
  MENU_END
};

#define MENU_FULL_POLYCHAINED \
  MENU_LAYOUT_CLOCK, \
  MENU_MIDI,\
  SETTING_VOICING_ALLOCATION_PRIORITY, \
  MENU_MODULATION, \
  SETTING_VOICING_CV_OUT_3, \
  SETTING_VOICING_CV_OUT_4, \
  MENU_OUTPUT, \
  MENU_END

const SettingIndex dual_polychained_menu[] = {
  MENU_FULL_POLYCHAINED
};

const SettingIndex quad_polychained_menu[] = {
  MENU_FULL_POLYCHAINED
};

const SettingIndex octal_polychained_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_CLOCK_OVERRIDE,
  MENU_MIDI,
  SETTING_VOICING_ALLOCATION_PRIORITY,
  MENU_MODULATION,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex quad_triggers_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_MIDI_CHANNEL,
  SETTING_MIDI_MIN_VELOCITY,
  SETTING_MIDI_MAX_VELOCITY,
  SETTING_MIDI_NOTE,
  SETTING_MIDI_OUT_MODE,
  SETTING_SEQUENCER_INPUT_RESPONSE,
  MENU_END
};

#define MENU_FULL_HYBRID \
  MENU_LAYOUT_CLOCK, \
  SETTING_CLOCK_OVERRIDE, \
  MENU_MIDI, \
  MENU_VOICING_ALLOCATION_MIXED, \
  MENU_MODULATION, \
  MENU_OUTPUT, \
  MENU_END

const SettingIndex three_one_menu[] = {
  MENU_FULL_HYBRID
};

const SettingIndex two_two_menu[] = {
  MENU_FULL_HYBRID
};

const SettingIndex paraphonic_plus_two_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_CLOCK_OVERRIDE,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_MIXED,
  MENU_MODULATION,
  SETTING_VOICING_CV_OUT_3,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex two_one_menu[] = {
  MENU_LAYOUT_CLOCK,
  MENU_MIDI,
  MENU_VOICING_ALLOCATION_MIXED,
  MENU_MODULATION,
  SETTING_VOICING_CV_OUT_4,
  MENU_OUTPUT,
  MENU_END
};

const SettingIndex quad_voltages_menu[] = {
  MENU_LAYOUT_CLOCK,
  SETTING_CLOCK_OVERRIDE,
  SETTING_MIDI_CHANNEL,
  SETTING_VOICING_CV_OUT,
  MENU_END
};

Settings::MenuCategory Settings::setup_menus = { 0, {
  mono_menu,
  dual_mono_menu,
  quad_mono_menu,
  dual_poly_menu,
  quad_poly_menu,
  dual_polychained_menu,
  quad_polychained_menu,
  octal_polychained_menu,
  quad_triggers_menu,
  quad_voltages_menu,
  three_one_menu,
  two_two_menu,
  two_one_menu,
  paraphonic_plus_two_menu,
}};

Settings::MenuCategory Settings::live_menus = { 0, {
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live_quad_triggers,
  menu_live,
  menu_live,
  menu_live,
  menu_live,
  menu_live_para,
}};

void Settings::Init() {
  global_.active_part = 0;
  
  // Build tables used to convert from a CC to a parameter number.
  std::fill(&part_cc_map_[0], &part_cc_map_[128], 0xff);
  std::fill(&remote_control_cc_map_[0], &remote_control_cc_map_[128], 0xff);
  
  for (uint8_t i = 0; i < SETTING_LAST; ++i) {
    const Setting& setting = settings_[i];
    if (setting.part_cc) {
      if (setting.domain != SETTING_DOMAIN_PART) while (1);
      part_cc_map_[setting.part_cc] = i;
    }
    if (setting.remote_control_cc) {
      uint8_t num_instances = setting.domain == SETTING_DOMAIN_PART ? 4 : 1;
      for (uint8_t j = 0; j < num_instances; ++j) {
        remote_control_cc_map_[setting.remote_control_cc + j * 32] = i;
      }
    }
  }
}

void Settings::Set(uint8_t address, uint8_t value) {
  uint8_t* bytes;
  bytes = static_cast<uint8_t*>(static_cast<void*>(&global_));
  bytes[address] = value;
}

void Settings::SetFromCC(
    uint8_t part_index,
    uint8_t controller,
    uint8_t value) {
  uint8_t* map = part_index == 0xff ? remote_control_cc_map_ : part_cc_map_;
  uint8_t part = part_index == 0xff ? controller >> 5 : part_index;
  uint8_t setting_index = map[controller];
  if (setting_index != 0xff) {
    const Setting& setting = settings_[setting_index];
    Set(setting, &part, setting.Scale(value));
  }
}

void Settings::Set(const Setting& setting, uint8_t* part, uint8_t value) {
  switch (setting.domain) {
    case SETTING_DOMAIN_GLOBAL:
      Set(setting.address[0], value);
      break;

    case SETTING_DOMAIN_MULTI:
      multi.Set(setting.address[0], value);
      if (*part >= multi.num_active_parts()) {
        *part = multi.num_active_parts() - 1;
      }
      break;

    case SETTING_DOMAIN_PART:
      multi.mutable_part(*part)->Set(setting.address[0], value);
      // When the module is configured in *triggers* mode, each part is mapped
      // to a single note. To edit this setting, both the "note min" and
      // "note max" parameters are simultaneously changed to the same value.
      // This is a bit more user friendly than letting the user set note min
      // and note max to the same value.
      if (setting.address[1]) {
        multi.mutable_part(*part)->Set(setting.address[1], value);
      }
      break;
  }
}

void Settings::Set(const Setting& setting, uint8_t value) {
  Set(setting, &global_.active_part, value);
}

uint8_t Settings::Get(const Setting& setting) const {
  uint8_t value = 0;
  switch (setting.domain) {
    case SETTING_DOMAIN_GLOBAL:
      value = Get(setting.address[0]);
      break;
      
    case SETTING_DOMAIN_MULTI:
      value = multi.Get(setting.address[0]);
      break;
      
    case SETTING_DOMAIN_PART:
      value = multi.part(global_.active_part).Get(setting.address[0]);
      break;
  }
  return value;
}

void Settings::Print(const Setting& setting, char* buffer) const {
  uint8_t value = Get(setting);
  switch (setting.unit) {
    case SETTING_UNIT_UINT8:
      PrintInteger(buffer, value);
      break;
      
    case SETTING_UNIT_INT8:
      PrintSignedInteger(buffer, value);
      break;
    
    case SETTING_UNIT_INDEX:
      PrintInteger(buffer, value + 1);
      break;
      
    case SETTING_UNIT_BAR_DURATION:
      if (value <= kMaxBarDuration) {
        PrintInteger(buffer, value);
      } else {
        strcpy(buffer, "oo");
      }
      break;

    case SETTING_UNIT_TEMPO:
      if (value == 39) {
        strcpy(buffer, "EXTERNAL");
      } else {
        PrintInteger(buffer, value);
      }
      break;

    case SETTING_UNIT_MIDI_CHANNEL:
      if (value == 0x10) {
        strcpy(buffer, "ALL");
      } else {
        PrintInteger(buffer, value + 1);
      }
      break;

    case SETTING_UNIT_MIDI_CHANNEL_OFF:
      if (value == 0x00) {
        strcpy(buffer, "OFF");
      } else {
        PrintInteger(buffer, value);
      }
      break;
    
    case SETTING_UNIT_VIBRATO_SPEED:
      if (value < 100) {
        PrintInteger(buffer, value);
      } else {
        strcpy(buffer, clock_division::display[value - 100]);
      }
      break;
      
    case SETTING_UNIT_PORTAMENTO:
      if (value < (LUT_PORTAMENTO_INCREMENTS_SIZE >> 1)) {
        PrintInteger(buffer, value);
      } else {
        PrintInteger(buffer, value - (LUT_PORTAMENTO_INCREMENTS_SIZE >> 1));
      }
      if (buffer[0] == ' ') {
        buffer[0] = (value < (LUT_PORTAMENTO_INCREMENTS_SIZE >> 1)) ? 'T' : 'R';
      }
      break;
      
    case SETTING_UNIT_ENUMERATION:
      strcpy(buffer, setting.values[value]);
      break;
      
    default:
      strcpy(buffer, "??");
  }
}

void Settings::Increment(const Setting& setting, int16_t increment) {
  int16_t value = Get(setting);
  if (setting.unit == SETTING_UNIT_INT8) {
    value = static_cast<int8_t>(value);
  }
  value += increment;
  int16_t min_value = setting.min_value;
  int16_t max_value = setting.max_value;
  if (
    &setting == &settings_[SETTING_VOICING_ALLOCATION_MODE] &&
    multi.part(global_.active_part).num_voices() == 1
  ) {
    max_value = VOICE_ALLOCATION_MODE_MONO;
  }
  if (
    multi.layout() == LAYOUT_PARAPHONIC_PLUS_TWO &&
    global_.active_part == 0 &&
    &setting == &settings_[SETTING_VOICING_AUDIO_MODE]
  ) {
    min_value = AUDIO_MODE_SAW;
  }
  CONSTRAIN(value, min_value, max_value);
  Set(setting, static_cast<uint8_t>(value));
}

/* static */
void Settings::PrintInteger(char* buffer, uint8_t number) {
  buffer[1] = '0' + (number % 10);
  number /= 10;
  buffer[0] = number ? '0' + (number % 10) : ' ';
  number /= 10;
  buffer[2] = '\0';
}

/* static */
void Settings::PrintSignedInteger(char* buffer, int8_t number) {
  if (number >= 0) {
    PrintInteger(buffer, number);
    if (buffer[0] == ' ') {
      buffer[0] = '+';
    }
  } else if (number > -10){
    PrintInteger(buffer, -number);
    buffer[0] = '-';
  } else {
    PrintInteger(buffer, -number);
    buffer[2] = ' ';
    buffer[3] = '-';
    buffer[4] = buffer[0];
    buffer[5] = buffer[1];
    buffer[6] = '\0';
  }
}

/* extern */
Settings settings;

}  // namespace yarns
