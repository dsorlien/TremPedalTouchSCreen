#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Bounce2.h>
#include "Adafruit_ILI9341.h"
#include "XPT2046_Calibrated.h"
#include "TremPedal.h"
#include "display.h"
#include "pwm.h"
#include "ui.h"
#include "patterns.h"

// TFT display and touchscreen controllers use SPI bus
XPT2046_Calibrated ts = XPT2046_Calibrated(TS_CS_PIN); 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// using the Bounce library to handle switches
Bounce g_button1 = Bounce(SW1_PIN, PUSH_SW_DEBOUNCE);  
Bounce g_button2 = Bounce(SW2_PIN, PUSH_SW_DEBOUNCE);  
Bounce g_button3 = Bounce(SW3_PIN, PUSH_SW_DEBOUNCE);  
Bounce g_encoder1SW = Bounce(ENCODER1_SW, ROT_SW_DEBOUNCE);  
Bounce g_encoder2SW = Bounce(ENCODER2_SW, ROT_SW_DEBOUNCE);  
Bounce g_encoder3SW = Bounce(ENCODER3_SW, ROT_SW_DEBOUNCE);  

// LFO 
const double g_period = 100.0; 
double g_offset = 0.0;
double g_freq_hz = 2.0;
double g_bpm = 120.0;
double g_beatdiv = 1.0; 

// pattern trem
PatternSeq g_patternSeqs[NUM_PAT_SEQS];
int g_patternSeqNum = 0;
bool g_patternMode = false;
int g_patStepCtr = 0;
int g_patRepeatCtr = 0;

// these params have separate storage for normal and pattern mode
//  so when you exit pattern mode, the old settings are used
int g_beatDivIdx = 12; 
int g_depth = MAX_DEPTH; 
int g_dutycycle = 50;
int g_wave = 0;
int8_t g_harmonicTremMode = true;
int8_t g_muteFullRandom = false;


int g_beatDivIdx_pat = 12; 
int g_depth_pat = MAX_DEPTH; 
int g_dutycycle_pat = 50;
int g_wave_pat = 0;
int8_t g_harmonicTremMode_pat = true;
int8_t g_muteFullRandom_pat = false;

int g_slewLimit = 255;

// rotary encoder 
int g_rot1Mode = 0;
int g_rot2Mode = 0;
int g_rot3Mode = 0;

// 
bool g_updateDisplay = true;
bool g_slewRateLimitOn = true;
bool g_invertPWM = true;


//**************************************************************************
//**************************************************************************
enum class PointID { NONE = -1, A, B, C, COUNT };

// source points used for calibration
static TS_Point _screenPoint[] = {
  TS_Point( 13,  11), // point A
  TS_Point(312, 113), // point B
  TS_Point(167, 214)  // point C
};

#define VERIFY_CALIBRATION
#ifdef VERIFY_CALIBRATION

// touchscreen points used for calibration verification
static TS_Point _touchPoint[] = {
  TS_Point(3795, 3735), // point A
  TS_Point( 482, 2200), // point B
  TS_Point(2084,  583), // point C
};

static TS_Calibration cal(
    _screenPoint[(int)PointID::A], _touchPoint[(int)PointID::A],
    _screenPoint[(int)PointID::B], _touchPoint[(int)PointID::B],
    _screenPoint[(int)PointID::C], _touchPoint[(int)PointID::C],
    SCREEN_WIDTH,
    SCREEN_HEIGHT
);

#endif // VERIFY_CALIBRATION

//**************************************************************************
//**************************************************************************
// -- UTILITY ROUTINES (DECLARATION) --
int16_t fillThenPrint (String &s, uint16_t fillColor, int16_t r_edge);
int16_t fillThenPrintln (String &s, uint16_t fillColor, int16_t r_edge);
inline bool touched();
void crosshair(TS_Point p);
uint16_t distance(TS_Point a, TS_Point b);
void drawMapping(PointID n, TS_Point p);
//void updateScreenEdges(TS_Point p);
PointID nearestScreenPoint(TS_Point touch);

int16_t g_rightEdgeLine1 = -1;
int16_t g_rightEdgeLine2 = -1;


//**************************************************************************
//**************************************************************************

void setup() {
    Serial.begin(9600);
    Serial.println("Touchscreen Effects Pedal v0x00"); 

    pinMode(LED_BUILTIN, OUTPUT);
    //pinMode(DEBUG_OUT, OUTPUT);

    pinMode(SW1_PIN, INPUT_PULLUP);
    pinMode(SW2_PIN, INPUT_PULLUP);
    pinMode(SW3_PIN, INPUT_PULLUP);

    pinMode(PWMOUT1_PIN, OUTPUT);
    pinMode(PWMOUT2_PIN, OUTPUT);
    pinMode(PWMOUT3_PIN, OUTPUT);

	pinMode(ENCODER1_CLK,INPUT);
	pinMode(ENCODER1_DT,INPUT);
	pinMode(ENCODER1_SW, INPUT_PULLUP);

	pinMode(ENCODER2_CLK,INPUT);
	pinMode(ENCODER2_DT,INPUT);
	pinMode(ENCODER2_SW, INPUT_PULLUP);

	pinMode(ENCODER3_CLK,INPUT);
	pinMode(ENCODER3_DT,INPUT);
	pinMode(ENCODER3_SW, INPUT_PULLUP);

    Wire.begin();

    tft.begin(SPICLK);
    ts.begin();

    initPwm();
    initPatterns();

    attachInterrupt(ENCODER1_CLK, rotaryEnc1_ISR, CHANGE);
    attachInterrupt(ENCODER2_CLK, rotaryEnc2_ISR, CHANGE);
    attachInterrupt(ENCODER3_CLK, rotaryEnc3_ISR, CHANGE);
 
    tft.setRotation(SCREEN_ROTATION);
    tft.fillScreen(BACKGROUND_COLOR);

    ts.setRotation(SCREEN_ROTATION);

    #ifdef VERIFY_CALIBRATION
        ts.calibrate(cal);
    #endif
}

//**************************************************************************
//**************************************************************************

void loop() {

    static uint32_t nextButtonUpdate = 0;
    static uint32_t nextDisplayUpdate = 0;
    static uint32_t nextTSUpdate = 0;
    uint32_t millis_now = millis();

    if(millis_now >= nextButtonUpdate)
    {
        handleButtons();
        nextButtonUpdate = millis_now + 10;
    }
    if(millis_now >= nextDisplayUpdate)
    {
        updateDisplay();
        nextDisplayUpdate = millis_now + 20;
    }
    if(millis_now >= nextTSUpdate)
    {
        if(ts.touched()){
            TS_Point p = ts.getPoint();
            tft.setCursor(0, 0);
            tft.setTextColor(TEXT_LEGEND_COLOR);  
            tft.setTextSize(0);
            String sx = String(p.x);
            String sy = String(p.y);

            tft.print("X: "); 
            g_rightEdgeLine1 = fillThenPrintln(sx, BACKGROUND_COLOR, g_rightEdgeLine1);
            tft.print("Y: "); 
            g_rightEdgeLine2 = fillThenPrintln(sy, BACKGROUND_COLOR, g_rightEdgeLine2);
        }
        nextTSUpdate = millis_now + 20;
    }
}

//**************************************************************************
//**************************************************************************

inline bool touched() {
#ifdef TIRQ_PIN
    if (ts.tirqTouched()) {
        return ts.touched();
    }
    return false;
#else
    return ts.touched();
#endif
}

void crosshair(TS_Point p) {
    tft.drawCircle   (p.x,     p.y,     6, ILI9341_WHITE);
    tft.drawFastHLine(p.x - 4, p.y,     9, ILI9341_WHITE);
    tft.drawFastVLine(p.x,     p.y - 4, 9, ILI9341_WHITE);
}

uint16_t distance(TS_Point a, TS_Point b) {
    // calculate the distance between points a and b in whatever 2D coordinate
    // system they both exist. returns an integer distance with naive rounding.
    static uint16_t const MAX = ~((uint16_t)0U);
    int32_t dx = b.x - a.x;
    int32_t dy = b.y - a.y;
    uint32_t dsq = (uint32_t)sq(dx) + (uint32_t)sq(dy);
    double d = sqrt(dsq); // add 1/2 for rounding
    if (d > ((double)MAX - 0.5))
        { return MAX; }
    else
        { return (uint16_t)(d + 0.5); } // poor-man's rounding
}

void drawMapping(PointID n, TS_Point tp) {

    static uint8_t const BUF_LEN = 64;
    static char buf[BUF_LEN] = { '\0' };

    static uint16_t lineHeight = (uint16_t)(1.5F * 8.0F + 0.5F);
    static uint16_t lineSpace  = 1U;

    int16_t posX, posY;
    uint16_t sizeW, sizeH;
    uint16_t posLeft = 6U;
    uint16_t posTop =
        SCREEN_HEIGHT - (3U - (uint16_t)n) * (lineHeight + lineSpace);

    TS_Point sp = _screenPoint[(int)n];

    // construct the line buffer
    snprintf(buf, BUF_LEN, "%c (%u,%u) = (%u,%u)",
        (uint8_t)n + 'A', sp.x, sp.y, tp.x, tp.y);

    // print the current line to serial port for debugging
    Serial.printf("%s\n", buf);

    // erase the previous line
    tft.getTextBounds(buf, posLeft, posTop, &posX, &posY, &sizeW, &sizeH);
    tft.fillRect(posX, posY, sizeW, sizeH, ILI9341_BLACK);

    // draw the current line
    tft.setCursor(posLeft, posTop);
    tft.printf("%s", buf);
}

PointID nearestScreenPoint(TS_Point touch) {

#ifdef VERIFY_CALIBRATION
    // the input point is already in screen coordinates because the touchscreen
    // has been calibrated. no need to perform translation.
    TS_Point tp = touch;
#else
    // translate the input point (in touch coordinates) to screen coordinates
    // using the hardcoded ranges defined in these macros. not particularly
    // accurate, but it doesn't need to be.
    TS_Point tp = (SCREEN_ROTATION & 1U)
        ? MAP_2D_LANDSCAPE(touch.x, touch.y)
        : MAP_2D_PORTRAIT(touch.x, touch.y);
    Serial.printf(
        "Touch{%u, %u} => Screen{%u, %u}\n", touch.x, touch.y, tp.x, tp.y);
#endif

    PointID  near = PointID::NONE;
    uint16_t dist = 0U;

    for (int id = (int)PointID::NONE + 1; id < (int)PointID::COUNT; ++id) {
        // compute the distance from our (translated) input touch point to each
        // screen point to determine minimum distance.
        uint16_t d = distance(tp, _screenPoint[id]);
        if ((near == PointID::NONE) || (d < dist)) {
        // new minimum distance, this is the nearest point to our touch (so far)
        near = (PointID)id;
        dist = d;
        }
    }

    return near;
}

//**************************************************************************
// text drawing util - probably not needed

int16_t fillThenPrint (String &s, uint16_t fillColor, int16_t r_edge){
    int16_t x1, y1;
    uint16_t w1, h1;
    tft.getTextBounds(s, tft.getCursorX(), tft.getCursorY(), &x1, &y1, &w1, &h1);
    if(r_edge > x1)
        w1 = r_edge - x1;
    tft.fillRect(x1, y1, w1, h1, fillColor);
    tft.print(s);
    return x1 + w1; // horiz position of right edge of text just drawn
}
int16_t fillThenPrintln (String &s, uint16_t fillColor, int16_t r_edge){
    int16_t x1, y1;
    uint16_t w1, h1;
    tft.getTextBounds(s, tft.getCursorX(), tft.getCursorY(), &x1, &y1, &w1, &h1);
    if(r_edge > x1)
        w1 = r_edge - x1;
    tft.fillRect(x1, y1, w1, h1, fillColor);
    tft.println(s);
    return x1 + w1; // horiz position of right edge of text just drawnt  
}
//**************************************************************************
