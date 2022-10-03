#ifndef _TremPedal
#define _TremPedal

// Declarations for better debugging
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debughex(x) Serial.print(x, HEX)
#define debuglnhex(x) Serial.println(x, HEX)
#else
#define debug(x)
#define debugln(x)
#define debughex(x) 
#define debuglnhex(x) 
#endif

//enum lfoType { wave_sine, wave_triangle, wave_square, wave_exp, wave_log };
#define MIN_WAVE 0
#define MAX_WAVE 4
#define WAVE_SINE 0
#define WAVE_TRIANGLE 1
#define WAVE_SQUARE 2
#define WAVE_EXP 3
#define WAVE_LOG 4

// pin assignments
#define TS_CS_PIN  6
#define TFT_DC 9
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_SCK 13

#define PWMOUT1_PIN 3
#define PWMOUT2_PIN 4
#define PWMOUT3_PIN 5

//#define DEBUG_OUT 13

#define I2C_SDA 18
#define I2C_SCL 19

#define SW1_PIN 27
#define SW2_PIN 28
#define SW3_PIN 29

#define ENCODER1_SW 30
#define ENCODER1_DT 31
#define ENCODER1_CLK 32

#define ENCODER2_CLK 33
#define ENCODER2_DT 34
#define ENCODER2_SW 35

#define ENCODER3_CLK 36
#define ENCODER3_DT 37
#define ENCODER3_SW 38

// display settings
static uint16_t const SCREEN_WIDTH    = 320;
static uint16_t const SCREEN_HEIGHT   = 240;
static uint8_t  const SCREEN_ROTATION = 3U;

#define MIN_OLED_UPDATE_INTERVAL_MS 2000

// rotary encoder 
#define ROTARY_ACCEL_THRESH_MILLIS 75
#define ROTARY_DEBOUNCE_MICROS 2000

#define ROT1_MODE_BPM 0
#define ROT1_MODE_HARMTREM 1
#define ROT1_MODE_PATTERN_MODE 2
#define ROT1_MODE_SLEW_LIMIT 3
#define ROT1_MODE_MAX 3

#define ROT2_MODE_BEAT_DIV 0
#define ROT2_MODE_WAVE_TYPE 1
#define ROT2_MODE_MAX 1

#define ROT3_MODE_DEPTH 0
#define ROT3_MODE_DUTY_CYCLE 1
#define ROT3_MODE_PATTERN_NUM 2
#define ROT3_MODE_MAX 2

// depth control range
#define MIN_DEPTH 0
#define MAX_DEPTH 100

// slew control range
#define MIN_SLEW_LIMIT 0
#define MAX_SLEW_LIMIT 255

// duty cycle control range
#define MIN_DUTY_CTL 0
#define MAX_DUTY_CTL 100
#define MIN_SQ_DC 5
#define MAX_SQ_DC 95
#define MIN_SIN_DC 10
#define MAX_SIN_DC 90
#define MIN_TRI_DC 1
#define MAX_TRI_DC 99
#define MIN_EXP_DC 1
#define MAX_EXP_DC 99
#define MIN_LOG_DC 1
#define MAX_LOG_DC 99

// pwm
#define MIN_ANALOG_WRITE 0
#define MAX_ANALOG_WRITE 255
#define MIN_WAVEGEN_OUT 0
#define MAX_WAVEGEN_OUT 255
#define ENABLE_PWM_SLEW false
#define MAX_PWM_SLEW 20

// ideal 8bit PWM freq for Teensy 4.1 @ 600MHz, see https://www.pjrc.com/teensy/td_pulse.html
#define PWM_FREQ_Hz 585937.5
#define PWM_RESOLUTION 8
#define PWM_ISR_UPDATE_uS 200

// SPI 
#define SPICLK 20000000

// beat div control range
#define MIN_BEAT_DIV 0
#define MAX_BEAT_DIV 22

// tap tempo range: 40 BPM to 300 BPM
#define MAX_TAP_MILLIS_DELTA 1500
#define MIN_TAP_MILLIS_DELTA 200
#define MAX_BPM 300
#define MIN_BPM 40

#define NUM_PAT_SEQS 32
#define NUM_PAT_STEPS 16
#define MIN_REPEATS 1
#define MAX_REPEATS 64

// display colors
#define WAVEFORM_COLOR ILI9341_YELLOW
#define TEXT_LEGEND_COLOR ILI9341_ORANGE
#define TEXT_PARAM_COLOR ILI9341_YELLOW
#define BACKGROUND_COLOR ILI9341_NAVY
 
struct PatternStep{
    int16_t repeats;    // range 1 to 64(?), values <= 0 not allowed
    int16_t div;        // see beatdiv.h for range, values < 0 not allowed
    int16_t mute;       // mute the output for this step
    int16_t skip;       // skip this step
    int16_t end;        // this step is the end of the sequence

    int16_t depth;      // -1 : use global depth setting
    int16_t dutyCycle;  // -1 : use global dutycycle setting
    int16_t harmMode;   // -1 : use global harmMode setting
    int16_t wave;       // -1 : use global wave setting 
};
struct PatternSeq{
    PatternStep patternSteps[NUM_PAT_STEPS];
};
extern PatternSeq g_patternSeqs[NUM_PAT_SEQS];
extern int g_patternSeqNum;
extern bool g_patternMode;
extern int g_patStepCtr;
extern int g_patRepeatCtr;

extern Bounce g_button1;
extern Bounce g_button2;
extern Bounce g_button3;
extern Bounce g_encoder1SW;
extern Bounce g_encoder2SW;
extern Bounce g_encoder3SW;
#define PUSH_SW_DEBOUNCE 10
#define ROT_SW_DEBOUNCE 10

extern IntervalTimer g_encoderTimer1;
extern IntervalTimer g_encoderTimer2;
extern IntervalTimer g_encoderTimer3;
extern IntervalTimer g_lfoCycleTimer;

extern XPT2046_Calibrated ts; 
extern Adafruit_ILI9341 tft;

extern const double g_period;
extern double g_offset;
extern double g_freq_hz;
extern double g_bpm;
extern double g_beatdiv;

extern int g_beatDivIdx; 
extern int g_depth; 
extern int g_dutycycle;
extern int g_wave;
extern int8_t g_harmonicTremMode;
extern int8_t g_muteFullRandom;

extern int g_beatDivIdx_pat; 
extern int g_depth_pat; 
extern int g_dutycycle_pat;
extern int g_wave_pat; 
extern int8_t g_harmonicTremMode_pat;
extern int8_t g_muteFullRandom_pat;

extern int g_slewLimit;

extern int g_rot1Mode;
extern int g_rot2Mode;
extern int g_rot3Mode;

extern bool g_updateDisplay;
extern bool g_slewRateLimitOn;
extern bool g_invertPWM;

#endif




