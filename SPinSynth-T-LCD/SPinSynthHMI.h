//----------------------------------------------------------------------------------
//SPinSynthHMI.h - Last update: 28 Jun 2026 - Started: 27 Jun 2026.
//----------------------------------------------------------------------------------
//28 Jun 2026: Implemented the SPinSynth HMI using an I2C LCD 16x2 and two Rotary
//Encoders with push-button function.
//----------------------------------------------------------------------------------
//SPinSynthHMI defines the parameter tree and manages parameter selection, value
//editing, Fine/Coarse modes, conditional parameters and the initial display.
//Parameter changes are sent to the synthesizer using a callback function.
//----------------------------------------------------------------------------------

#ifndef SPIN_SYNTH_HMI_H
#define SPIN_SYNTH_HMI_H

#include <Arduino.h>
#include <Encoder.h>
#include <LiquidCrystal_I2C.h>

class SPinSynthHMI {
public:
  enum ParameterId {
    PARAM_MASTER_VOLUME,
    PARAM_MASTER_TUNING,
    PARAM_REVERB_MIX,
    PARAM_PORTAMENTO,
    PARAM_MOD_WHEEL_FUNCTION,
    PARAM_OSC_WAVEFORM,
    PARAM_OSC_PULSE_WIDTH,
    PARAM_X_SAW_LEVEL,
    PARAM_X_DETUNE,
    PARAM_X_PULSE_LEVEL,
    PARAM_X_PULSE_WIDTH,
    PARAM_X_SUB_LEVEL,
    PARAM_X_SUB_FACTOR,
    PARAM_OSC_LFO_WAVEFORM,
    PARAM_OSC_LFO_PULSE_WIDTH,
    PARAM_OSC_LFO_FREQUENCY,
    PARAM_OSC_LFO_LEVEL,
    PARAM_AMP_ATTACK,
    PARAM_AMP_DECAY,
    PARAM_AMP_SUSTAIN,
    PARAM_AMP_RELEASE,
    PARAM_FILTER_CUTOFF,
    PARAM_FILTER_RESONANCE,
    PARAM_FILTER_ATTACK,
    PARAM_FILTER_DECAY,
    PARAM_FILTER_SUSTAIN,
    PARAM_FILTER_RELEASE,
    PARAM_FILTER_ENVELOPE_AMOUNT,
    PARAM_FILTER_LFO_WAVEFORM,
    PARAM_FILTER_LFO_PULSE_WIDTH,
    PARAM_FILTER_LFO_FREQUENCY,
    PARAM_FILTER_LFO_LEVEL,
    PARAMETER_COUNT
  };

  typedef void (*ParameterChangeHandler)(ParameterId parameter, int value);

  SPinSynthHMI(LiquidCrystal_I2C& display,
               Encoder& leftEncoder,
               Encoder& rightEncoder,
               uint8_t leftButtonPin,
               uint8_t rightButtonPin);

  void begin(bool initializeDisplay = true);
  void update();
  void setParameterChangeHandler(ParameterChangeHandler handler);
  void setParameterValue(ParameterId parameter, int value);
  int getParameterValue(ParameterId parameter) const;

private:
  enum ParameterGroup {
    GROUP_GLOBAL,
    GROUP_OSC_WAVE,
    GROUP_OSC_XTREME,
    GROUP_OSC_LFO,
    GROUP_AMP_ENVELOPE,
    GROUP_FILTER,
    GROUP_FILTER_ENVELOPE,
    GROUP_FILTER_LFO,
    GROUP_COUNT
  };

  enum ParameterType {
    TYPE_NORMAL,
    TYPE_MAIN_WAVEFORM,
    TYPE_LFO_WAVEFORM,
    TYPE_DUTY_CYCLE,
    TYPE_DETUNE,
    TYPE_SUB_FACTOR,
    TYPE_MASTER_TUNE,
    TYPE_MILLISECONDS,
    TYPE_MOD_WHEEL_FUNCTION,
    TYPE_LFO_FREQUENCY,
    TYPE_BIPOLAR,
    TYPE_PERCENT
  };

  enum VisibilityCondition {
    VISIBLE_ALWAYS,
    VISIBLE_MAIN_PULSE,
    VISIBLE_MAIN_XTREME,
    VISIBLE_OSC_LFO_PULSE,
    VISIBLE_FILTER_LFO_PULSE
  };

  struct TestParameter {
    const char* title;
    const char* label;
    ParameterGroup group;
    ParameterType type;
    int value;
    int defaultValue;
    int minimumValue;
    int maximumValue;
    int fineStep;
    int coarseStep;
    VisibilityCondition visibility;
  };

  struct Button {
    uint8_t pin;
    bool rawState;
    bool stableState;
    bool fell;
    bool rose;
    uint32_t changedAt;
    uint32_t pressedAt;
  };

  static const int ENCODER_COUNTS_PER_DETENT = 4;
  static const uint32_t BUTTON_DEBOUNCE_MS = 25;
  static const uint32_t LONG_PRESS_MS = 800;
  static const uint32_t MIDI_DISPLAY_REFRESH_MS = 40;
  static const int MAIN_WAVE_PULSE = 3;
  static const int MAIN_WAVE_XTREME = 4;
  static const int LFO_WAVE_PULSE = 4;

  LiquidCrystal_I2C& mDisplay;
  Encoder& mLeftEncoder;
  Encoder& mRightEncoder;
  Button mLeftButton;
  Button mRightButton;
  TestParameter mParameters[PARAMETER_COUNT];
  int mSelectedParameter;
  long mLastLeftPosition;
  long mLastRightPosition;
  bool mFineMode;
  bool mPulseWidthEditMode;
  bool mHomeVisible;
  bool mButtonChordActive;
  bool mRightLongPressHandled;
  bool mDisplayDirty;
  uint32_t mLastDisplayRefresh;
  ParameterChangeHandler mParameterChangeHandler;

  void updateButton(Button& button, uint32_t now);
  void processEncoders();
  void processButtons(uint32_t now);
  void notifyParameterChanged();
  bool isParameterVisible(int parameterIndex) const;
  bool groupHasVisibleParameter(ParameterGroup group) const;
  void moveSelection(long change);
  void selectNextGroup();
  void showHome();
  void showParameter();
  void formatValueRow(char* row, size_t rowSize) const;
  void printRow(uint8_t row, const char* text);
};

#endif
