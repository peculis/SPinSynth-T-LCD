//----------------------------------------------------------------------------------
//SPinSynthHMI.cpp - Last update: 28 Jun 2026 - Started: 27 Jun 2026.
//----------------------------------------------------------------------------------
//28 Jun 2026: Implemented LCD presentation, Rotary Encoder navigation, push-button
//commands and synchronization of HMI values with MIDI Control Change messages.
//----------------------------------------------------------------------------------
//SPinSynthHMI operates the LCD and the two Rotary Encoders. The Left Encoder selects
//a parameter and the Right Encoder changes its value. Both buttons return to the
//initial display without losing the current HMI status.
//----------------------------------------------------------------------------------

#include "SPinSynthHMI.h"

#include <stdio.h>
#include <string.h>

SPinSynthHMI::SPinSynthHMI(LiquidCrystal_I2C& display,
                           Encoder& leftEncoder,
                           Encoder& rightEncoder,
                           uint8_t leftButtonPin,
                           uint8_t rightButtonPin):
  mDisplay(display),
  mLeftEncoder(leftEncoder),
  mRightEncoder(rightEncoder),
  mLeftButton{leftButtonPin, HIGH, HIGH, false, false, 0, 0},
  mRightButton{rightButtonPin, HIGH, HIGH, false, false, 0, 0},
  mParameters{
    {"MASTER VOLUME", "Volume", GROUP_GLOBAL, TYPE_NORMAL,
      100, 100, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"MASTER TUNING", "Tune", GROUP_GLOBAL, TYPE_MASTER_TUNE,
      0, 0, -100, 100, 1, 10, VISIBLE_ALWAYS},
    {"REVERB MIX", "Reverb", GROUP_GLOBAL, TYPE_PERCENT,
      0, 0, 0, 100, 1, 10, VISIBLE_ALWAYS},
    {"PORTAMENTO", "Glide", GROUP_GLOBAL, TYPE_MILLISECONDS,
      0, 0, 0, 254, 2, 16, VISIBLE_ALWAYS},
    {"MOD WHEEL", "Func", GROUP_GLOBAL, TYPE_MOD_WHEEL_FUNCTION,
      3, 3, 0, 3, 1, 1, VISIBLE_ALWAYS},

    {"OSCILLATOR WAVE", "Wave", GROUP_OSC_WAVE, TYPE_MAIN_WAVEFORM,
      0, 0, 0, 4, 1, 1, VISIBLE_ALWAYS},
    {"OSCILLATOR WAVE", "Pulse", GROUP_OSC_WAVE, TYPE_DUTY_CYCLE,
      50, 50, 10, 50, 1, 5, VISIBLE_MAIN_PULSE},

    {"OSC XTREME", "XSawL", GROUP_OSC_XTREME, TYPE_NORMAL,
      100, 100, 0, 127, 1, 8, VISIBLE_MAIN_XTREME},
    {"OSC XTREME", "XDet", GROUP_OSC_XTREME, TYPE_DETUNE,
      0, 0, -100, 100, 1, 10, VISIBLE_MAIN_XTREME},
    {"OSC XTREME", "XPulseL", GROUP_OSC_XTREME, TYPE_NORMAL,
      80, 80, 0, 127, 1, 8, VISIBLE_MAIN_XTREME},
    {"OSC XTREME", "XPulseW", GROUP_OSC_XTREME, TYPE_DUTY_CYCLE,
      50, 50, 10, 50, 1, 5, VISIBLE_MAIN_XTREME},
    {"OSC XTREME", "XSubL", GROUP_OSC_XTREME, TYPE_NORMAL,
      80, 80, 0, 127, 1, 8, VISIBLE_MAIN_XTREME},
    {"OSC XTREME", "XSub", GROUP_OSC_XTREME, TYPE_SUB_FACTOR,
      100, 100, 50, 150, 1, 10, VISIBLE_MAIN_XTREME},

    {"OSC LFO WAVE", "Wave", GROUP_OSC_LFO, TYPE_LFO_WAVEFORM,
      0, 0, 0, 4, 1, 1, VISIBLE_ALWAYS},
    {"OSC LFO WAVE", "Pulse", GROUP_OSC_LFO, TYPE_DUTY_CYCLE,
      50, 50, 10, 50, 1, 5, VISIBLE_OSC_LFO_PULSE},
    {"OSC LFO FREQ", "Freq", GROUP_OSC_LFO, TYPE_LFO_FREQUENCY,
      10, 10, 1, 127, 1, 10, VISIBLE_ALWAYS},
    {"OSC LFO LEVEL", "Level", GROUP_OSC_LFO, TYPE_NORMAL,
      0, 0, 0, 127, 1, 8, VISIBLE_ALWAYS},

    {"AMP ENVELOPE", "Attack", GROUP_AMP_ENVELOPE, TYPE_NORMAL,
      10, 10, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"AMP ENVELOPE", "Decay", GROUP_AMP_ENVELOPE, TYPE_NORMAL,
      40, 40, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"AMP ENVELOPE", "Sustain", GROUP_AMP_ENVELOPE, TYPE_NORMAL,
      100, 100, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"AMP ENVELOPE", "Release", GROUP_AMP_ENVELOPE, TYPE_NORMAL,
      30, 30, 0, 127, 1, 8, VISIBLE_ALWAYS},

    {"FILTER", "CutOff", GROUP_FILTER, TYPE_NORMAL,
      64, 64, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"FILTER", "Reso", GROUP_FILTER, TYPE_NORMAL,
      20, 20, 0, 127, 1, 8, VISIBLE_ALWAYS},

    {"FILTER ENVELOPE", "Attack", GROUP_FILTER_ENVELOPE, TYPE_NORMAL,
      10, 10, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"FILTER ENVELOPE", "Decay", GROUP_FILTER_ENVELOPE, TYPE_NORMAL,
      40, 40, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"FILTER ENVELOPE", "Sustain", GROUP_FILTER_ENVELOPE, TYPE_NORMAL,
      100, 100, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"FILTER ENVELOPE", "Release", GROUP_FILTER_ENVELOPE, TYPE_NORMAL,
      30, 30, 0, 127, 1, 8, VISIBLE_ALWAYS},
    {"FILTER ENVELOPE", "Amount", GROUP_FILTER_ENVELOPE, TYPE_BIPOLAR,
      0, 0, -100, 100, 1, 10, VISIBLE_ALWAYS},

    {"FILTER LFO WAVE", "Wave", GROUP_FILTER_LFO, TYPE_LFO_WAVEFORM,
      0, 0, 0, 4, 1, 1, VISIBLE_ALWAYS},
    {"FILTER LFO WAVE", "Pulse", GROUP_FILTER_LFO, TYPE_DUTY_CYCLE,
      50, 50, 10, 50, 1, 5, VISIBLE_FILTER_LFO_PULSE},
    {"FILTER LFO FREQ", "Freq", GROUP_FILTER_LFO, TYPE_LFO_FREQUENCY,
      10, 10, 1, 127, 1, 10, VISIBLE_ALWAYS},
    {"FILTER LFO LEVEL", "Level", GROUP_FILTER_LFO, TYPE_NORMAL,
      0, 0, 0, 127, 1, 8, VISIBLE_ALWAYS}
  },
  mSelectedParameter(PARAM_MASTER_VOLUME),
  mLastLeftPosition(0),
  mLastRightPosition(0),
  mFineMode(true),
  mPulseWidthEditMode(false),
  mHomeVisible(true),
  mButtonChordActive(false),
  mRightLongPressHandled(false),
  mDisplayDirty(false),
  mLastDisplayRefresh(0),
  mParameterChangeHandler(NULL) {
}

void SPinSynthHMI::setParameterChangeHandler(ParameterChangeHandler handler) {
  mParameterChangeHandler = handler;
}

void SPinSynthHMI::setParameterValue(ParameterId parameter, int value) {
  if(parameter < 0 || parameter >= PARAMETER_COUNT){
    return;
  }

  TestParameter& target = mParameters[parameter];
  const int constrainedValue = constrain(value, target.minimumValue, target.maximumValue);
  if(target.value == constrainedValue){
    return;
  }

  target.value = constrainedValue;
  if(parameter == PARAM_OSC_WAVEFORM &&
     constrainedValue != MAIN_WAVE_PULSE){
    mPulseWidthEditMode = false;
  }
  const bool selectedParameterHidden = !isParameterVisible(mSelectedParameter);
  if(selectedParameterHidden){
    mSelectedParameter = int(parameter);
  }
  if((int(parameter) == mSelectedParameter || selectedParameterHidden) && !mHomeVisible){
    // MIDI controllers can generate CC messages faster than an I2C LCD can
    // redraw. Let update() coalesce them while the synth value changes now.
    mDisplayDirty = true;
  }
}

int SPinSynthHMI::getParameterValue(ParameterId parameter) const {
  if(parameter < 0 || parameter >= PARAMETER_COUNT){
    return 0;
  }
  return mParameters[parameter].value;
}

void SPinSynthHMI::begin(bool initializeDisplay) {
  pinMode(mLeftButton.pin, INPUT_PULLUP);
  pinMode(mRightButton.pin, INPUT_PULLUP);

  mLeftButton.rawState = digitalRead(mLeftButton.pin);
  mLeftButton.stableState = mLeftButton.rawState;
  mRightButton.rawState = digitalRead(mRightButton.pin);
  mRightButton.stableState = mRightButton.rawState;

  mLeftEncoder.write(0);
  mRightEncoder.write(0);
  mLastLeftPosition = 0;
  mLastRightPosition = 0;

  if(initializeDisplay){
    mDisplay.init();
    mDisplay.backlight();
    showHome();
  }
}

void SPinSynthHMI::updateButton(Button& button, uint32_t now) {
  button.fell = false;
  button.rose = false;

  const bool reading = digitalRead(button.pin);
  if(reading != button.rawState){
    button.rawState = reading;
    button.changedAt = now;
  }

  if((now - button.changedAt) >= BUTTON_DEBOUNCE_MS &&
     button.stableState != button.rawState){
    button.stableState = button.rawState;
    if(button.stableState == LOW){
      button.fell = true;
      button.pressedAt = now;
    }
    else{
      button.rose = true;
    }
  }
}

void SPinSynthHMI::processEncoders() {
  const long leftPosition = mLeftEncoder.read() / ENCODER_COUNTS_PER_DETENT;
  const long rightPosition = mRightEncoder.read() / ENCODER_COUNTS_PER_DETENT;
  const long leftChange = leftPosition - mLastLeftPosition;
  const long rightChange = rightPosition - mLastRightPosition;

  mLastLeftPosition = leftPosition;
  mLastRightPosition = rightPosition;

  if(leftChange != 0 && !mPulseWidthEditMode){
    if(!mHomeVisible){
      moveSelection(leftChange);
    }
    showParameter();
  }

  if(rightChange != 0){
    const int editedParameter = mPulseWidthEditMode
      ? PARAM_OSC_PULSE_WIDTH
      : mSelectedParameter;
    TestParameter& parameter = mParameters[editedParameter];
    const int step = mFineMode ? parameter.fineStep : parameter.coarseStep;
    const int value = parameter.value + int(rightChange) * step;
    const int constrainedValue = constrain(value, parameter.minimumValue, parameter.maximumValue);
    if(constrainedValue != parameter.value){
      parameter.value = constrainedValue;
      if(mPulseWidthEditMode){
        if(mParameterChangeHandler != NULL){
          mParameterChangeHandler(PARAM_OSC_PULSE_WIDTH, parameter.value);
        }
      }
      else{
        notifyParameterChanged();
      }
    }
    showParameter();
  }
}

void SPinSynthHMI::notifyParameterChanged() {
  if(mParameterChangeHandler != NULL){
    mParameterChangeHandler(ParameterId(mSelectedParameter),
                            mParameters[mSelectedParameter].value);
  }
}

bool SPinSynthHMI::isParameterVisible(int parameterIndex) const {
  const TestParameter& parameter = mParameters[parameterIndex];
  switch(parameter.visibility){
    case VISIBLE_MAIN_PULSE:
      // Main pulse width is edited from the waveform screen's dedicated
      // "PULSE XX% EDIT" mode. Do not expose the same parameter as a second
      // encoder-navigation position.
      return false;
    case VISIBLE_MAIN_XTREME:
      return mParameters[PARAM_OSC_WAVEFORM].value == MAIN_WAVE_XTREME;
    case VISIBLE_OSC_LFO_PULSE:
      return mParameters[PARAM_OSC_LFO_WAVEFORM].value == LFO_WAVE_PULSE;
    case VISIBLE_FILTER_LFO_PULSE:
      return mParameters[PARAM_FILTER_LFO_WAVEFORM].value == LFO_WAVE_PULSE;
    case VISIBLE_ALWAYS:
    default:
      return true;
  }
}

bool SPinSynthHMI::groupHasVisibleParameter(ParameterGroup group) const {
  for(int i = 0; i < PARAMETER_COUNT; i++){
    if(mParameters[i].group == group && isParameterVisible(i)){
      return true;
    }
  }
  return false;
}

void SPinSynthHMI::moveSelection(long change) {
  const int direction = change > 0 ? 1 : -1;
  long movesRemaining = change > 0 ? change : -change;

  while(movesRemaining > 0){
    do{
      mSelectedParameter += direction;
      if(mSelectedParameter >= PARAMETER_COUNT){
        mSelectedParameter = 0;
      }
      else if(mSelectedParameter < 0){
        mSelectedParameter = PARAMETER_COUNT - 1;
      }
    } while(!isParameterVisible(mSelectedParameter));
    movesRemaining--;
  }
}

void SPinSynthHMI::selectNextGroup() {
  ParameterGroup group = mParameters[mSelectedParameter].group;

  do{
    group = ParameterGroup((group + 1) % GROUP_COUNT);
  } while(!groupHasVisibleParameter(group));

  for(int i = 0; i < PARAMETER_COUNT; i++){
    if(mParameters[i].group == group && isParameterVisible(i)){
      mSelectedParameter = i;
      return;
    }
  }
}

void SPinSynthHMI::processButtons(uint32_t now) {
  const bool bothPressed = mLeftButton.stableState == LOW &&
                           mRightButton.stableState == LOW;

  if(bothPressed){
    if(!mButtonChordActive){
      mButtonChordActive = true;
      showHome();
    }
    return;
  }

  if(mButtonChordActive){
    if(mLeftButton.stableState == HIGH && mRightButton.stableState == HIGH){
      mButtonChordActive = false;
      // Pressing rotary encoder shafts can create quadrature counts. Discard
      // everything accumulated during the two-button Home gesture.
      mLastLeftPosition = mLeftEncoder.read() / ENCODER_COUNTS_PER_DETENT;
      mLastRightPosition = mRightEncoder.read() / ENCODER_COUNTS_PER_DETENT;
    }
    return;
  }

  if(mLeftButton.rose){
    if(mPulseWidthEditMode){
      mPulseWidthEditMode = false;
    }
    else if(mSelectedParameter == PARAM_OSC_WAVEFORM &&
            mParameters[PARAM_OSC_WAVEFORM].value == MAIN_WAVE_PULSE){
      mPulseWidthEditMode = true;
    }
    else{
      selectNextGroup();
    }
    showParameter();
  }

  if(mRightButton.fell){
    mRightLongPressHandled = false;
  }

  if(mRightButton.stableState == LOW &&
     !mRightLongPressHandled &&
     (now - mRightButton.pressedAt) >= LONG_PRESS_MS){
    TestParameter& parameter = mParameters[mSelectedParameter];
    if(parameter.value != parameter.defaultValue){
      parameter.value = parameter.defaultValue;
      notifyParameterChanged();
    }
    mRightLongPressHandled = true;
    showParameter();
  }

  if(mRightButton.rose && !mRightLongPressHandled){
    mFineMode = !mFineMode;
    showParameter();
  }
}

void SPinSynthHMI::showHome() {
  mPulseWidthEditMode = false;
  printRow(0, "SPinSynth-T LCD");
  printRow(1, "Ricardo Peculis");
  mHomeVisible = true;
  mDisplayDirty = false;
  mLastDisplayRefresh = millis();
}

void SPinSynthHMI::formatValueRow(char* row, size_t rowSize) const {
  static const char* mainWaveNames[] = {"SINE", "SAW", "SQUARE", "PULSE", "XTREME"};
  static const char* lfoWaveNames[] = {"SINE", "SAW", "INV-SAW", "SQUARE", "PULSE"};

  const TestParameter& parameter = mParameters[mSelectedParameter];
  const char* mode = mFineMode ? "F" : "C";

  switch(parameter.type){
    case TYPE_MAIN_WAVEFORM:
      if(parameter.value == MAIN_WAVE_PULSE){
        snprintf(row,
                 rowSize,
                 mPulseWidthEditMode ? "PULSE %02d%% EDIT" : "PULSE %02d%%",
                 mParameters[PARAM_OSC_PULSE_WIDTH].value);
      }
      else{
        snprintf(row, rowSize, "%s", mainWaveNames[parameter.value]);
      }
      break;
    case TYPE_LFO_WAVEFORM:
      snprintf(row, rowSize, "%s", lfoWaveNames[parameter.value]);
      break;
    case TYPE_DUTY_CYCLE:
      snprintf(row, rowSize, "%s %02d%% %s", parameter.label, parameter.value, mode);
      break;
    case TYPE_DETUNE:
      snprintf(row, rowSize, "XDet %+dct %s", parameter.value, mode);
      break;
    case TYPE_SUB_FACTOR:
      snprintf(row,
               rowSize,
               "XSub %d.%02d %s",
               parameter.value / 100,
               abs(parameter.value % 100),
               mode);
      break;
    case TYPE_MASTER_TUNE:{
      const char sign = parameter.value > 0 ? '+' : (parameter.value < 0 ? '-' : ' ');
      const int magnitude = abs(parameter.value);
      snprintf(row,
               rowSize,
               "Tune %c%d.%02d %s",
               sign,
               magnitude / 100,
               magnitude % 100,
               mode);
      break;
    }
    case TYPE_MILLISECONDS:
      if(parameter.value == 0){
        snprintf(row, rowSize, "%s OFF", parameter.label);
      }
      else{
        snprintf(row, rowSize, "%s %03dms %s", parameter.label, parameter.value, mode);
      }
      break;
    case TYPE_MOD_WHEEL_FUNCTION:{
      static const char* functionNames[] = {
        "CUTOFF", "RESONANCE", "FILTER LFO", "OSC LFO"
      };
      snprintf(row, rowSize, "Func %s", functionNames[parameter.value]);
      break;
    }
    case TYPE_LFO_FREQUENCY:
      snprintf(row,
               rowSize,
               "Freq %d.%dHz %s",
               parameter.value / 10,
               parameter.value % 10,
               mode);
      break;
    case TYPE_BIPOLAR:
      snprintf(row, rowSize, "Amount %+d %s", parameter.value, mode);
      break;
    case TYPE_PERCENT:
      snprintf(row, rowSize, "%s %03d%% %s", parameter.label, parameter.value, mode);
      break;
    case TYPE_NORMAL:
    default:
      snprintf(row, rowSize, "%s %03d %s", parameter.label, parameter.value, mode);
      break;
  }
}

void SPinSynthHMI::showParameter() {
  char valueRow[17];
  formatValueRow(valueRow, sizeof(valueRow));
  printRow(0, mParameters[mSelectedParameter].title);
  printRow(1, valueRow);
  mHomeVisible = false;
  mDisplayDirty = false;
  mLastDisplayRefresh = millis();
}

void SPinSynthHMI::printRow(uint8_t row, const char* text) {
  char padded[17];
  memset(padded, ' ', 16);
  padded[16] = '\0';
  const size_t textLength = strlen(text);
  memcpy(padded, text, textLength < 16 ? textLength : 16);
  mDisplay.setCursor(0, row);
  mDisplay.print(padded);
}

void SPinSynthHMI::update() {
  const uint32_t now = millis();
  updateButton(mLeftButton, now);
  updateButton(mRightButton, now);
  processButtons(now);

  if(!mButtonChordActive){
    processEncoders();
  }

  if(mDisplayDirty &&
     !mHomeVisible &&
     (now - mLastDisplayRefresh) >= MIDI_DISPLAY_REFRESH_MS){
    showParameter();
  }
}
