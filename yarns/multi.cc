// Copyright 2013 Emilie Gillet.
// Copyright 2020 Chris Rogers.
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
// Multi.

#include "yarns/multi.h"

#include <algorithm>

#include "stmlib/algorithms/voice_allocator.h"

#include "yarns/just_intonation_processor.h"
#include "yarns/midi_handler.h"
#include "yarns/settings.h"
#include "yarns/clock_division.h"
#include "yarns/ui.h"

namespace yarns {
  
using namespace std;
using namespace stmlib;

void Multi::Init(bool reset_calibration) {
  just_intonation_processor.Init();
  
  fill(
      &settings_.custom_pitch_table[0],
      &settings_.custom_pitch_table[12],
      0);
  
  for (uint8_t i = 0; i < kNumParts; ++i) {
    part_[i].Init();
    part_[i].set_custom_pitch_table(settings_.custom_pitch_table);
  }
  for (uint8_t i = 0; i < kNumSystemVoices; ++i) {
    voice_[i].Init();
  }
  for (uint8_t i = 0; i < kNumCVOutputs; ++i) {
    cv_outputs_[i].Init(reset_calibration);
  }
  running_ = false;
  recording_ = false;
  recording_part_ = 0;
  started_by_keyboard_ = true;
  
  // Put the multi in a usable state. Even if these settings will later be
  // overriden with some data retrieved from Flash (presets).
  settings_.clock_tempo = 120;
  settings_.clock_swing = 0;
  settings_.clock_input_division = 1;
  settings_.clock_output_division = clock_division::unity;
  settings_.clock_bar_duration = 4;
  settings_.clock_override = 0;
  settings_.nudge_first_tick = 0;
  settings_.clock_manual_start = 0;

  // A test sequence...
  // seq->num_steps = 4;
  // seq->step[0].data[0] = 48;
  // seq->step[0].data[1] = 0x7f;
  // seq->step[1].data[0] = 72;
  // seq->step[1].data[1] = 0x7f;
  // seq->step[2].data[0] = 60;
  // seq->step[2].data[1] = 0x7f;
  // seq->step[3].data[0] = 72;
  // seq->step[3].data[1] = 0x7f;
  // voicing->oscillator_shape = 1;
  // settings_.clock_tempo = 100;
  // settings_.clock_swing = 99;

  settings_.layout = LAYOUT_MONO;
  AfterDeserialize();
}

void Multi::Clock() {
  if (!running_) {
    return;
  }
  
  uint16_t output_division = clock_division::list[settings_.clock_output_division].num_ticks;
  uint16_t input_division = settings_.clock_input_division;
  
  if (previous_output_division_ &&
      output_division != previous_output_division_) {
    needs_resync_ = true;
  }
  previous_output_division_ = output_division;
  
  // Logic equation for computing a clock output with a 50% duty cycle.
  if (output_division > 1) {
    if (clock_output_prescaler_ == 0 && clock_input_prescaler_ == 0) {
      clock_pulse_counter_ = 0xffff;
    }
    if (clock_output_prescaler_ >= (output_division >> 1) &&
        clock_input_prescaler_ >= (input_division >> 1)) {
      clock_pulse_counter_ = 0;
    }
  } else {
    if (input_division > 1) {
      clock_pulse_counter_ = \
          clock_input_prescaler_ <= (input_division - 1) >> 1 ? 0xffff : 0;
    } else {
      // Because no division is used, neither on the output nor on the input,
      // we don't have a sufficient fast time base to derive a 50% duty cycle
      // output. Instead, we output 5ms pulses.
      clock_pulse_counter_ = 40;
    }
  }
  
  if (!clock_input_prescaler_) {
    midi_handler.OnClock();
    
    ++swing_counter_;
    if (swing_counter_ >= 12) {
      swing_counter_ = 0;
    }
    
    if (song_pointer_) {
      ClockSong();
    } else {
      if (internal_clock()) {
        swing_predelay_[swing_counter_] = 0;
      } else {
        uint32_t interval = midi_clock_tick_duration_;
        midi_clock_tick_duration_ = 0;

        uint32_t modulation = swing_counter_ < 6
            ? swing_counter_ : 12 - swing_counter_;
        swing_predelay_[swing_counter_] = \
            27 * modulation * interval * uint32_t(settings_.clock_swing) >> 13;
      }
    }
    
    ++bar_position_;
    if (bar_position_ >= settings_.clock_bar_duration * 24) {
      bar_position_ = 0;
    }
    if (bar_position_ == 0) {
      reset_pulse_counter_ = settings_.nudge_first_tick ? 9 : 81;
      if (needs_resync_) {
        clock_output_prescaler_ = 0;
        needs_resync_ = false;
      }
    }
    if (settings_.clock_bar_duration > kMaxBarDuration) {
      bar_position_ = 1;
    }
    
    ++clock_output_prescaler_;
    if (clock_output_prescaler_ >= output_division) {
      clock_output_prescaler_ = 0;
    }
  }

  ++clock_input_prescaler_;
  if (clock_input_prescaler_ >= settings_.clock_input_division) {
    clock_input_prescaler_ = 0;
  }
  
  if (stop_count_down_) {
    --stop_count_down_;
    if (!stop_count_down_ && CanAutoStop()) {
      Stop();
    }
  }
}

void Multi::Start(bool started_by_keyboard) {
  started_by_keyboard_ = started_by_keyboard_ && started_by_keyboard;
  if (running_) {
    return;
  }
  if (internal_clock()) {
    internal_clock_ticks_ = 0;
    internal_clock_.Start(settings_.clock_tempo, settings_.clock_swing);
  }
  midi_handler.OnStart();

  running_ = true;
  clock_input_prescaler_ = 0;
  clock_output_prescaler_ = 0;
  stop_count_down_ = 0;
  bar_position_ = -1;
  swing_counter_ = -1;
  previous_output_division_ = 0;
  needs_resync_ = false;
  
  fill(&swing_predelay_[0], &swing_predelay_[12], -1);
  
  for (uint8_t i = 0; i < num_active_parts_; ++i) {
    part_[i].Start();
  }
  song_pointer_ = NULL;
  midi_clock_tick_duration_ = 0;
}

void Multi::Stop() {
  if (!running()) {
    return;
  }
  for (uint8_t i = 0; i < num_active_parts_; ++i) {
    part_[i].Stop();
  }
  midi_handler.OnStop();
  clock_pulse_counter_ = 0;
  reset_pulse_counter_ = 0;
  stop_count_down_ = 0;
  running_ = false;
  started_by_keyboard_ = true;
  song_pointer_ = NULL;
}

void Multi::Refresh() {
  if (clock_pulse_counter_) {
    --clock_pulse_counter_;
  }
  if (reset_pulse_counter_) {
    --reset_pulse_counter_;
  }

  ++midi_clock_tick_duration_;
  for (int i = 0; i < 12; ++i) {
    if (swing_predelay_[i] == 0) {
      for (uint8_t j = 0; j < num_active_parts_; ++j) {
        part_[j].Clock();
      }
    }
    if (swing_predelay_[i] >= 0) {
      --swing_predelay_[i];
    }
  }

  for (uint8_t j = 0; j < num_active_parts_; ++j) {
    Part& part = part_[j];
    part.mutable_looper().Refresh();
    for (uint8_t v = 0; v < part.num_voices(); ++v) {
      part.voice(v)->Refresh(v);
    }
  }

  for (uint8_t i = 0; i < kNumCVOutputs; ++i) {
    cv_outputs_[i].Refresh();
  }
}

bool Multi::Set(uint8_t address, uint8_t value) {
  uint8_t* bytes;
  bytes = static_cast<uint8_t*>(static_cast<void*>(&settings_));
  uint8_t previous_value = bytes[address];
  bytes[address] = value;
  if (value == previous_value) { return false; }
  if (address == MULTI_LAYOUT) {
    ChangeLayout(
        static_cast<Layout>(previous_value),
        static_cast<Layout>(value));
  } else if (address == MULTI_CLOCK_TEMPO) {
    internal_clock_.set_tempo(settings_.clock_tempo);
  } else if (address == MULTI_CLOCK_SWING) {
    internal_clock_.set_swing(settings_.clock_swing);
  }
  return true;
}

void Multi::AssignVoicesToCVOutputs() {
  switch (settings_.layout) {
    case LAYOUT_MONO:
    case LAYOUT_DUAL_POLYCHAINED:
      cv_outputs_[0].assign(&voice_[0], 0);
      cv_outputs_[1].assign(&voice_[0], 0);
      cv_outputs_[2].assign(&voice_[0], 0);
      cv_outputs_[3].assign(&voice_[0], 1);
      break;

    case LAYOUT_DUAL_MONO:
    case LAYOUT_DUAL_POLY:
    case LAYOUT_QUAD_POLYCHAINED:
      cv_outputs_[0].assign(&voice_[0], 0);
      cv_outputs_[1].assign(&voice_[1], 0);
      cv_outputs_[2].assign(&voice_[0], 1);
      cv_outputs_[3].assign(&voice_[1], 1);
      break;

    case LAYOUT_QUAD_MONO:
    case LAYOUT_QUAD_POLY:
    case LAYOUT_OCTAL_POLYCHAINED:
    case LAYOUT_THREE_ONE:
    case LAYOUT_TWO_TWO:
    case LAYOUT_QUAD_TRIGGERS:
    case LAYOUT_QUAD_VOLTAGES:
      for (uint8_t i = 0; i < kNumCVOutputs; ++i) {
        cv_outputs_[i].assign(&voice_[i], 1);
      }
      break;

    case LAYOUT_TWO_ONE:
      cv_outputs_[0].assign(&voice_[0], 1);
      cv_outputs_[1].assign(&voice_[1], 1);
      cv_outputs_[2].assign(&voice_[2], 1);
      cv_outputs_[3].assign(&voice_[2], 0);
      break;

    case LAYOUT_PARAPHONIC_PLUS_TWO:
      cv_outputs_[0].assign(&voice_[0], kNumParaphonicVoices);
      cv_outputs_[1].assign(&voice_[kNumParaphonicVoices], 1);
      cv_outputs_[2].assign(&voice_[kNumParaphonicVoices], 0);
      cv_outputs_[3].assign(&voice_[kNumParaphonicVoices + 1], 1);
      break;
  }
}

void Multi::GetCvGate(uint16_t* cv, bool* gate) {
  switch (settings_.layout) {
    case LAYOUT_MONO:
    case LAYOUT_DUAL_POLYCHAINED:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].velocity_dac_code();
      cv[2] = cv_outputs_[2].aux_cv_dac_code();
      cv[3] = cv_outputs_[3].aux_cv_dac_code_2();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[0].trigger();
      gate[2] = clock();
      gate[3] = reset_or_playing_flag();
      break;
      
    case LAYOUT_DUAL_MONO:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].aux_cv_dac_code();
      cv[3] = cv_outputs_[3].aux_cv_dac_code();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      gate[2] = clock();
      gate[3] = reset_or_playing_flag();
      break;
    
    case LAYOUT_DUAL_POLY:
    case LAYOUT_QUAD_POLYCHAINED:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].aux_cv_dac_code();
      cv[3] = cv_outputs_[3].aux_cv_dac_code_2();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      gate[2] = clock();
      gate[3] = reset_or_playing_flag();
      break;
    
    case LAYOUT_QUAD_MONO:
    case LAYOUT_QUAD_POLY:
    case LAYOUT_OCTAL_POLYCHAINED:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].note_dac_code();
      cv[3] = cv_outputs_[3].note_dac_code();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      if (settings_.clock_override) {
        gate[2] = clock();
        gate[3] = reset_or_playing_flag();
      } else {
        gate[2] = voice_[2].gate();
        gate[3] = voice_[3].gate();
      }
      break;

    case LAYOUT_THREE_ONE:
    case LAYOUT_TWO_TWO:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].note_dac_code();
      cv[3] = cv_outputs_[3].note_dac_code();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      gate[2] = voice_[2].gate();
      if (settings_.clock_override) {
        gate[3] = clock();
      } else {
        gate[3] = voice_[3].gate();
      }
      break;
    
    case LAYOUT_TWO_ONE:
      cv[0] = cv_outputs_[0].note_dac_code();
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].note_dac_code();
      cv[3] = cv_outputs_[3].aux_cv_dac_code_2();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      gate[2] = voice_[2].gate();
      gate[3] = clock();
      break;

    case LAYOUT_PARAPHONIC_PLUS_TWO:
      cv[0] = 0; // Paraphonic output only outputs audio, not CV
      cv[1] = cv_outputs_[1].note_dac_code();
      cv[2] = cv_outputs_[2].aux_cv_dac_code();
      cv[3] = cv_outputs_[3].note_dac_code();
      gate[0] = cv_outputs_[0].gate();
      gate[1] = cv_outputs_[1].gate();
      gate[2] = settings_.clock_override ? clock() : voice_[kNumParaphonicVoices].trigger();
      gate[3] = cv_outputs_[3].gate();
      break;

    case LAYOUT_QUAD_TRIGGERS:
      cv[0] = cv_outputs_[0].trigger_dac_code();
      cv[1] = cv_outputs_[1].trigger_dac_code();
      cv[2] = cv_outputs_[2].trigger_dac_code();
      cv[3] = cv_outputs_[3].trigger_dac_code();
      gate[0] = voice_[0].trigger() && ~voice_[1].gate();
      gate[1] = voice_[0].trigger() && voice_[1].gate();
      gate[2] = clock();
      gate[3] = reset_or_playing_flag();
      break;

    case LAYOUT_QUAD_VOLTAGES:
      cv[0] = cv_outputs_[0].aux_cv_dac_code();
      cv[1] = cv_outputs_[1].aux_cv_dac_code();
      cv[2] = cv_outputs_[2].aux_cv_dac_code();
      cv[3] = cv_outputs_[3].aux_cv_dac_code();
      gate[0] = voice_[0].gate();
      gate[1] = voice_[1].gate();
      if (settings_.clock_override) {
        gate[2] = clock();
        gate[3] = reset_or_playing_flag();
      } else {
        gate[2] = voice_[2].gate();
        gate[3] = voice_[3].gate();
      }
      break;
  }
}

void Multi::GetLedsBrightness(uint8_t* brightness) {
  if (layout_configurator_.learning()) {
    fill(&brightness[0], &brightness[kNumCVOutputs], 0);
    for (uint8_t i = 0; i < layout_configurator_.num_notes(); ++i) {
      brightness[i] = 255;
    }
    return;
  }
  
  switch (settings_.layout) {
    case LAYOUT_MONO:
    case LAYOUT_DUAL_POLYCHAINED:
      brightness[0] = voice_[0].gate() ? 255 : 0;
      brightness[1] = voice_[0].velocity() << 1;
      brightness[2] = voice_[0].aux_cv();
      brightness[3] = voice_[0].aux_cv_2();
      break;
      
    case LAYOUT_DUAL_MONO:
      brightness[0] = voice_[0].gate() ? 255 : 0;
      brightness[1] = voice_[1].gate() ? 255 : 0;
      brightness[2] = voice_[0].aux_cv();
      brightness[3] = voice_[1].aux_cv();
      break;

    case LAYOUT_DUAL_POLY:
    case LAYOUT_QUAD_POLYCHAINED:
      brightness[0] = voice_[0].gate() ? 255 : 0;
      brightness[1] = voice_[1].gate() ? 255 : 0;
      brightness[2] = voice_[0].aux_cv();
      brightness[3] = voice_[1].aux_cv_2();
      break;
      
    case LAYOUT_QUAD_MONO:
    case LAYOUT_QUAD_POLY:
    case LAYOUT_OCTAL_POLYCHAINED:
    case LAYOUT_QUAD_TRIGGERS:
    case LAYOUT_THREE_ONE:
    case LAYOUT_TWO_TWO:
      brightness[0] = voice_[0].gate() ? (voice_[0].velocity() << 1) : 0;
      brightness[1] = voice_[1].gate() ? (voice_[1].velocity() << 1) : 0;
      brightness[2] = voice_[2].gate() ? (voice_[2].velocity() << 1) : 0;
      brightness[3] = voice_[3].gate() ? (voice_[3].velocity() << 1) : 0;
      break;
      
    case LAYOUT_TWO_ONE:
      brightness[0] = voice_[0].gate() ? (voice_[0].velocity() << 1) : 0;
      brightness[1] = voice_[1].gate() ? (voice_[1].velocity() << 1) : 0;
      brightness[2] = voice_[2].gate() ? 255 : 0;
      brightness[3] = clock() ? voice_[2].aux_cv_2() : 0;
      break;

    case LAYOUT_PARAPHONIC_PLUS_TWO:
      {
        const NoteEntry& last_note = part_[0].priority_note(NOTE_STACK_PRIORITY_LAST);
        const uint8_t last_voice_index = part_[0].FindVoiceForNote(last_note.note);
        brightness[0] = (cv_outputs_[0].gate() && last_voice_index != VOICE_ALLOCATION_NOT_FOUND) ? (part_[0].voice(last_voice_index)->velocity() << 1) : 0;
        brightness[1] = voice_[kNumParaphonicVoices].gate() ? (voice_[kNumParaphonicVoices].velocity() << 1) : 0;
        brightness[2] = voice_[kNumParaphonicVoices].aux_cv();
        brightness[3] = voice_[kNumParaphonicVoices + 1].gate() ? (voice_[kNumParaphonicVoices + 1].velocity() << 1) : 0;
      }
      break;

    case LAYOUT_QUAD_VOLTAGES:
      brightness[0] = voice_[0].aux_cv();
      brightness[1] = voice_[1].aux_cv();
      brightness[2] = voice_[2].aux_cv();
      brightness[3] = voice_[3].aux_cv();
      break;
  }
}

void Multi::AllocateParts() {
  // Reset and close all parts and voices.
  for (uint8_t i = 0; i < kNumParts; ++i) {
    part_[i].Reset();
  }
  for (uint8_t i = 0; i < kNumSystemVoices; ++i) {
    voice_[i].NoteOff();
  }
  
  switch (settings_.layout) {
    case LAYOUT_MONO:
    case LAYOUT_DUAL_MONO:
    case LAYOUT_QUAD_MONO:
    case LAYOUT_QUAD_TRIGGERS:
    case LAYOUT_QUAD_VOLTAGES:
      {
        num_active_parts_ = settings_.layout == LAYOUT_MONO ? 1 : \
            (settings_.layout == LAYOUT_DUAL_MONO ? 2 : 4);
        for (uint8_t i = 0; i < num_active_parts_; ++i) {
          part_[i].AllocateVoices(&voice_[i], 1, false);
        }
      }
      break;
    
    case LAYOUT_DUAL_POLY:
    case LAYOUT_QUAD_POLY:
    case LAYOUT_DUAL_POLYCHAINED:
    case LAYOUT_QUAD_POLYCHAINED:
    case LAYOUT_OCTAL_POLYCHAINED:
      {
        uint8_t num_voices = settings_.layout == LAYOUT_DUAL_POLY || \
            settings_.layout == LAYOUT_QUAD_POLYCHAINED ? 2 : \
            (settings_.layout == LAYOUT_DUAL_POLYCHAINED ? 1 : 4);
        part_[0].AllocateVoices(
            &voice_[0],
            num_voices,
            settings_.layout >= LAYOUT_DUAL_POLYCHAINED);
        num_active_parts_ = 1;
      }
      break;
      
    case LAYOUT_THREE_ONE:
    case LAYOUT_TWO_ONE:
      {
        uint8_t num_poly_voices = (settings_.layout == LAYOUT_THREE_ONE) ? 3 : 2;
        part_[0].AllocateVoices(&voice_[0], num_poly_voices, false);
        part_[1].AllocateVoices(&voice_[num_poly_voices], 1, false);
        num_active_parts_ = 2;
      }
      break;
    
    case LAYOUT_TWO_TWO:
      {
        part_[0].AllocateVoices(&voice_[0], 2, false);
        part_[1].AllocateVoices(&voice_[2], 1, false);
        part_[2].AllocateVoices(&voice_[3], 1, false);
        num_active_parts_ = 3;
      }
      break;

    case LAYOUT_PARAPHONIC_PLUS_TWO:
      {
        CONSTRAIN(part_[0].mutable_voicing_settings()->oscillator_mode, OSCILLATOR_MODE_OFF + 1, OSCILLATOR_MODE_LAST - 1);
        part_[0].AllocateVoices(&voice_[0], kNumParaphonicVoices, false);
        part_[1].AllocateVoices(&voice_[kNumParaphonicVoices], 1, false);
        part_[2].AllocateVoices(&voice_[kNumParaphonicVoices + 1], 1, false);
        num_active_parts_ = 3;
      }
      break;

    default:
      break;
  }
  AssignVoicesToCVOutputs();
}

void Multi::ChangeLayout(Layout old_layout, Layout new_layout) {
  AllocateParts();
  for (uint8_t i = 0; i < num_active_parts_; ++i) {
    part_[i].NewLayout();
    part_[i].set_siblings(num_active_parts_ > 1);
  }
  switch (new_layout) {
    case LAYOUT_MONO:
    case LAYOUT_DUAL_MONO:
    case LAYOUT_QUAD_MONO:
    case LAYOUT_QUAD_VOLTAGES:
      {
        // Duplicate uninitialized voices.
        for (uint8_t i = 1; i < num_active_parts_; ++i) {
          uint8_t destination = i;
          uint8_t source = i % num_active_parts_;
          if (destination != source) {
            memcpy(
                part_[destination].mutable_midi_settings(),
                part_[source].mutable_midi_settings(),
                sizeof(MidiSettings));
            memcpy(
                part_[destination].mutable_voicing_settings(),
                part_[source].mutable_voicing_settings(),
                sizeof(VoicingSettings));
            memcpy(
                part_[destination].mutable_sequencer_settings(),
                part_[source].mutable_sequencer_settings(),
                sizeof(SequencerSettings));
          }
        }
      }
      break;

    case LAYOUT_QUAD_TRIGGERS:
      {
        for (uint8_t i = 0; i < num_active_parts_; ++i) {
          MidiSettings* midi = part_[i].mutable_midi_settings();
          if (old_layout != LAYOUT_QUAD_TRIGGERS) {
            midi->min_note = 36 + i * 2;
            midi->max_note = 36 + i * 2;
          }
          midi->channel = part_[0].mutable_midi_settings()->channel;
          midi->out_mode = part_[0].mutable_midi_settings()->out_mode;
        }
        
        // Duplicate sequencer settings.
        for (uint8_t i = 1; i < num_active_parts_; ++i) {
          uint8_t destination = i;
          uint8_t source = i % num_active_parts_;
          if (destination != source) {
            memcpy(
                part_[destination].mutable_sequencer_settings(),
                part_[source].mutable_sequencer_settings(),
                sizeof(SequencerSettings));
          }
        }
      }
      break;

    default:
      break;
  }
}

void Multi::AfterDeserialize() {
  Stop();

  internal_clock_.set_tempo(settings_.clock_tempo);
  AllocateParts();
  
  for (uint8_t i = 0; i < kNumParts; ++i) {
    part_[i].AfterDeserialize();
  }
}


const uint8_t song[] = {
  #include "song/song.h"
  255,
};

void Multi::StartSong() {
  Set(MULTI_LAYOUT, LAYOUT_QUAD_MONO);
  part_[0].mutable_voicing_settings()->oscillator_shape = 0x83;
  part_[1].mutable_voicing_settings()->oscillator_shape = 0x83;
  part_[2].mutable_voicing_settings()->oscillator_shape = 0x84;
  part_[3].mutable_voicing_settings()->oscillator_shape = 0x86;
  AllocateParts();
  settings_.clock_tempo = 140;
  Stop();
  Start(false);
  
  song_pointer_ = &song[0];
  song_clock_ = 0;
  song_delta_ = 0;
}

void Multi::ClockSong() {
  while (song_clock_ >= song_delta_) {
    if (*song_pointer_ == 255) {
      song_pointer_ = &song[0];
    }
    if (*song_pointer_ == 254) {
      song_delta_ += 6;
    } else {
      uint8_t part = *song_pointer_ >> 6;
      uint8_t note = *song_pointer_ & 0x3f;
      if (note == 0) {
        part_[part].AllNotesOff();
      } else {
        part_[part].NoteOn(0, note + 24, 100);
      }
      song_clock_ = 0;
      song_delta_ = 0;
    }
    ++song_pointer_;
  }
  ++song_clock_;
}

void Multi::StartRecording(uint8_t part) {
  if (
    part_[part].midi_settings().play_mode == PLAY_MODE_MANUAL ||
    part >= num_active_parts_
  ) {
    return;
  }
  if (recording_) {
    if (recording_part_ == part) {
      return;
    } else {
      StopRecording(recording_part_);
    }
  }
  if (part_[part].looper_in_use()) {
    // Looper needs a running clock
    Start(false);
  }
  part_[part].StartRecording();
  recording_ = true;
  recording_part_ = part;
}

void Multi::StopRecording(uint8_t part) {
  if (recording_ && recording_part_ == part) {
    part_[part].StopRecording();
    recording_ = false;
  }
}

bool Multi::ControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
  bool thru = true;
  if (
    is_remote_control_channel(channel) &&
    setting_defs.remote_control_cc_map[controller] != 0xff
  ) {
    SetFromCC(0xff, controller, value);
  } else {
    for (uint8_t i = 0; i < num_active_parts_; ++i) {
      if (!part_accepts(i, channel)) { continue; }
      if (controller == kCCRecordOffOn) {
        // Intercept this CC so multi can update its own recording state
        value >= 64 ? StartRecording(i) : StopRecording(i);
        ui.SplashOn(SPLASH_ACTIVE_PART);
      } else if (controller == kCCDeleteRecording) {
        // Splash needs part index
        part_[i].DeleteRecording();
        ui.SetSplashPart(i);
        ui.SplashOn(SPLASH_DELETE_RECORDING);
      } else {
        thru = part_[i].ControlChange(channel, controller, value) && thru;
        SetFromCC(i, controller, value);
      }
    }
  }
  return thru;
}

void Multi::SetFromCC(uint8_t part_index, uint8_t controller, uint8_t value_7bits) {
  uint8_t* map = part_index == 0xff ?
    setting_defs.remote_control_cc_map : setting_defs.part_cc_map;
  uint8_t setting_index = map[controller];
  if (setting_index == 0xff) { return; }
  const Setting& setting = setting_defs.get(setting_index);

  int16_t scaled_value;
  uint8_t range = setting.max_value - setting.min_value + 1;
  scaled_value = range * value_7bits >> 7;
  scaled_value += setting.min_value;
  if (setting.unit == SETTING_UNIT_TEMPO) {
    scaled_value &= 0xfe;
    if (scaled_value < TEMPO_EXTERNAL) {
      scaled_value = TEMPO_EXTERNAL;
    }
  }

  uint8_t part = part_index == 0xff ? controller >> 5 : part_index;
  ApplySettingAndSplash(setting, part, scaled_value);
}

void Multi::ApplySettingAndSplash(const Setting& setting, uint8_t part, int16_t raw_value) {
  ApplySetting(setting, part, raw_value);
  ui.SplashSetting(setting, part);
}

void Multi::ApplySetting(const Setting& setting, uint8_t part, int16_t raw_value) {
  // Apply dynamic min/max as needed
  int16_t min_value = setting.min_value;
  int16_t max_value = setting.max_value;
  if (
    &setting == &setting_defs.get(SETTING_VOICING_ALLOCATION_MODE) &&
    multi.part(part).num_voices() == 1
  ) {
    max_value = VOICE_ALLOCATION_MODE_MONO;
  }
  if (
    multi.layout() == LAYOUT_PARAPHONIC_PLUS_TWO &&
    part == 0 &&
    &setting == &setting_defs.get(SETTING_VOICING_OSCILLATOR_MODE)
  ) {
    min_value = OSCILLATOR_MODE_DRONE;
  }
  CONSTRAIN(raw_value, min_value, max_value);
  uint8_t value = static_cast<uint8_t>(raw_value);

  uint8_t prev_value = GetSetting(setting, part);
  if (prev_value == value) { return; }

  bool layout = &setting == &setting_defs.get(SETTING_LAYOUT);
  bool sequencer_semantics = \
    &setting == &setting_defs.get(SETTING_SEQUENCER_PLAY_MODE) ||
    &setting == &setting_defs.get(SETTING_SEQUENCER_CLOCK_QUANTIZATION) || (
      &setting == &setting_defs.get(SETTING_SEQUENCER_ARP_PATTERN) && (
        prev_value == 0 || value == 0
      )
    );

  if (running_ && layout) { Stop(); }
  if (recording_ && (
    layout || (recording_part_ == part && sequencer_semantics)
  )) { StopRecording(recording_part_); }
  if (sequencer_semantics) { part_[part].StopSequencerArpeggiatorNotes(); }

  switch (setting.domain) {
    case SETTING_DOMAIN_MULTI:
      multi.Set(setting.address[0], value);
      break;
    case SETTING_DOMAIN_PART:
      // When the module is configured in *triggers* mode, each part is mapped
      // to a single note. To edit this setting, both the "note min" and
      // "note max" parameters are simultaneously changed to the same value.
      // This is a bit more user friendly than letting the user set note min
      // and note max to the same value.
      if (setting.address[1]) {
        multi.mutable_part(part)->Set(setting.address[1], value);
      }
      multi.mutable_part(part)->Set(setting.address[0], value);
      break;

    default:
      break;
  }
}

uint8_t Multi::GetSetting(const Setting& setting, uint8_t part) const {
  uint8_t value = 0;
  switch (setting.domain) {
    case SETTING_DOMAIN_MULTI:
      value = multi.Get(setting.address[0]);
      break;
    case SETTING_DOMAIN_PART:
      value = multi.part(part).Get(setting.address[0]);
      break;
  }
  return value;
}

/* extern */
Multi multi;

}  // namespace yarns
