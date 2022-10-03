#ifndef _ui
#define _ui

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include "XPT2046_Calibrated.h"
#include <Bounce2.h>
#include "TremPedal.h"

extern void lfoCycle_ISR(void);

// rotary encoder globals & consts
IntervalTimer g_encoderTimer1;
IntervalTimer g_encoderTimer2;
IntervalTimer g_encoderTimer3;

// ********************************************************
// misc functions
void freq2bpm(){
    g_bpm = 60.0 * g_freq_hz * g_beatdiv;   // beatdiv: 1.0 = 1/4 note, 0.5 = 1/8 note
}
void bpm2freq(){
    g_freq_hz = g_bpm / (g_beatdiv * 60.0);     
}
uint32_t absDiffU32 (uint32_t x, uint32_t y){
    if(x>=y) return x-y;
    else return y-x;
}
// ********************************************************
// change param value implementations
void changeBeatDiv(int amount, bool accel_flag){
    g_beatDivIdx += amount;
    if(g_beatDivIdx > MAX_BEAT_DIV) g_beatDivIdx = MAX_BEAT_DIV;
    if(g_beatDivIdx < MIN_BEAT_DIV) g_beatDivIdx = MIN_BEAT_DIV;  
    g_beatdiv = getDivVal(g_beatDivIdx);
    bpm2freq(); // update the osc freq now that we've changed beatdiv
    g_updateDisplay = true;
}
void changeSlewRate(int amount, bool accel_flag){
    if(!accel_flag)
        g_slewLimit += amount;
    else
        g_slewLimit += amount * 5;
    if(g_slewLimit > MAX_SLEW_LIMIT) g_slewLimit = MAX_SLEW_LIMIT;
    if(g_slewLimit < MIN_SLEW_LIMIT) g_slewLimit = MIN_SLEW_LIMIT;  
    g_updateDisplay = true;
}
void changeBpm(double amount, bool accel_flag){
    if(accel_flag){
        g_bpm += (5.0 * amount);
        g_bpm = (int)(g_bpm / 5.0) * 5.0; // round to nearest 5??
    }else{
        g_bpm += amount;
    }
    if(g_bpm > MAX_BPM) g_bpm = MAX_BPM;
    if(g_bpm < MIN_BPM) g_bpm = MIN_BPM;
    bpm2freq(); // keep freq and bpm synced
    g_updateDisplay = true;
}
void changeDepth(double amount, bool accel_flag){
    if(accel_flag){
        g_depth += (5 * amount);
        g_depth = (g_depth / 5) * 5; // round to nearest 5
    }else{
        g_depth += amount;
    }
    if(g_depth > MAX_DEPTH) g_depth = MAX_DEPTH;
    if(g_depth < MIN_DEPTH) g_depth = MIN_DEPTH;
    g_updateDisplay = true;
}
void changeDutyCycle(double amount, bool accel_flag){
    if(accel_flag){
        g_dutycycle += (5 * amount);
        g_dutycycle = (g_dutycycle / 5) * 5; // round to nearest 5
    }else{
        g_dutycycle += amount;
    }
    if(g_dutycycle > MAX_DUTY_CTL) g_dutycycle = MAX_DUTY_CTL;
    if(g_dutycycle < MIN_DUTY_CTL) g_dutycycle = MIN_DUTY_CTL;
    g_updateDisplay = true;
}
void changeLfoWaveType(double amount, bool accel_flag){
    g_wave += amount;
    if(g_wave < MIN_WAVE) g_wave = MAX_WAVE;
    if(g_wave > MAX_WAVE) g_wave = MIN_WAVE;
    g_updateDisplay = true;    
}
void changePatternNum(double amount, bool accel_flag){
    g_patternSeqNum += amount;
    if(g_patternSeqNum >= NUM_PAT_SEQS) g_patternSeqNum = 0;
    if(g_patternSeqNum < 0) g_patternSeqNum = NUM_PAT_SEQS - 1;
    g_updateDisplay = true;    
}

// ********************************************************
// Rotary encoder timer ISR fuinctions
void rot1Change(double amount, bool accel_flag){
    if(g_rot1Mode == ROT1_MODE_BPM){
        changeBpm(amount, accel_flag);
    }
    if(g_rot1Mode == ROT1_MODE_SLEW_LIMIT){
        changeSlewRate(amount, accel_flag);
    }
    if(g_rot1Mode == ROT1_MODE_HARMTREM){
        g_harmonicTremMode = !g_harmonicTremMode;
        g_updateDisplay = true;
    }
    if(g_rot1Mode == ROT1_MODE_PATTERN_MODE){
        g_patternMode = !g_patternMode;
        // need to do these things after changing beatDivIdx (happens when entering or exiting pattern mode)
        g_beatdiv = getDivVal(g_patternMode ? g_beatDivIdx_pat :  g_beatDivIdx);
        bpm2freq(); 
        g_updateDisplay = true;
    }
}
void rot2Change(double amount, bool accel_flag){
    if(g_rot2Mode == ROT2_MODE_BEAT_DIV){
        changeBeatDiv(amount, accel_flag);
    }
    if(g_rot2Mode == ROT2_MODE_WAVE_TYPE){
        changeLfoWaveType(amount, accel_flag);
    }
}
void rot3Change(double amount, bool accel_flag){
    if(g_rot3Mode == ROT3_MODE_DEPTH){
        changeDepth(amount, accel_flag);
    }
    if(g_rot3Mode == ROT3_MODE_DUTY_CYCLE){
        changeDutyCycle(amount, accel_flag);
    }    
    if(g_rot3Mode == ROT3_MODE_PATTERN_NUM){
        changePatternNum(amount, accel_flag);
    }    
}

// ********************************************************
// Rotary encoder timer ISR fuinctions
//   timer was started on pin change, these timers provide debounce
void rotaryEnc1_Tmr_ISR()
{
    static int clkPinState = 1;
    static int clkPinPrevState = 1;
    static int dtPinState = 1;
    static uint32_t lastTransitionMillis = 0;
    g_encoderTimer1.end(); // we are done with this timer, release resources
    clkPinState = digitalRead(ENCODER1_CLK);
    dtPinState = digitalRead(ENCODER1_DT);
    uint32_t nowmillis = millis();
    bool accel_flag = nowmillis - lastTransitionMillis <= ROTARY_ACCEL_THRESH_MILLIS;
    if(clkPinPrevState < clkPinState){ // detect rising edge
        if (clkPinState != dtPinState)
            rot1Change(1.0, accel_flag);
		else
            rot1Change(-1.0, accel_flag);
        lastTransitionMillis = nowmillis;
    }
    clkPinPrevState = clkPinState;
}
void rotaryEnc2_Tmr_ISR()
{
    static int clkPinState = 1;
    static int clkPinPrevState = 1;
    static int dtPinState = 1;
    static uint32_t lastTransitionMillis = 0;
    g_encoderTimer2.end(); // we are done with this timer, release resources
    clkPinState = digitalRead(ENCODER2_CLK);
    dtPinState = digitalRead(ENCODER2_DT);
    uint32_t nowmillis = millis();
    bool accel_flag = nowmillis - lastTransitionMillis <= ROTARY_ACCEL_THRESH_MILLIS;
    if(clkPinPrevState < clkPinState){
        if (clkPinState != dtPinState)
            rot2Change(1.0, accel_flag);
		else
            rot2Change(-1.0, accel_flag);
        lastTransitionMillis = nowmillis;
    }
    clkPinPrevState = clkPinState;
}
void rotaryEnc3_Tmr_ISR()
{
    static int clkPinState = 1;
    static int clkPinPrevState = 1;
    static int dtPinState = 1;
    static uint32_t lastTransitionMillis = 0;
    g_encoderTimer3.end(); // we are done with this timer, release resources
    clkPinState = digitalRead(ENCODER3_CLK);
    dtPinState = digitalRead(ENCODER3_DT);
    uint32_t nowmillis = millis();
    bool accel_flag = nowmillis - lastTransitionMillis <= ROTARY_ACCEL_THRESH_MILLIS;
    if(clkPinPrevState < clkPinState){
        if (clkPinState != dtPinState)
            rot3Change(1.0, accel_flag);
		else
            rot3Change(-1.0, accel_flag);
        lastTransitionMillis = nowmillis;
    }
    clkPinPrevState = clkPinState;
}
// ********************************************************
// Rotary encoder pin change ISR functions
//   just start the debounce timer
void rotaryEnc1_ISR(){ g_encoderTimer1.begin(rotaryEnc1_Tmr_ISR, ROTARY_DEBOUNCE_MICROS); }
void rotaryEnc2_ISR(){ g_encoderTimer2.begin(rotaryEnc2_Tmr_ISR, ROTARY_DEBOUNCE_MICROS); }
void rotaryEnc3_ISR(){ g_encoderTimer2.begin(rotaryEnc3_Tmr_ISR, ROTARY_DEBOUNCE_MICROS); }

// ********************************************************
// Handle TAP TEMPO switch
void tapTempoPressed(){
    static uint32_t tapMillis_1 = 0;
    static uint32_t tapMillis_2 = 0;
    static uint32_t tapMillis_3 = 0;
    static uint32_t tapMillis_4 = 0;

    // keep track of timing of last 4 taps
    tapMillis_4 = tapMillis_3;
    tapMillis_3 = tapMillis_2;
    tapMillis_2 = tapMillis_1;
    tapMillis_1 = millis();
    // calculate deltas
    uint32_t tapDelta1 = tapMillis_1 - tapMillis_2;
    uint32_t tapDelta2 = tapMillis_2 - tapMillis_3;
    uint32_t tapDelta3 = tapMillis_3 - tapMillis_4;

    // reject tap if too long or too short between last 2 taps
    if(tapDelta1 > MAX_TAP_MILLIS_DELTA || tapDelta1 < MIN_TAP_MILLIS_DELTA){
        // tap is not valid. Zero out old tap history and return
        tapMillis_2 = 0;
        tapMillis_3 = 0;
        tapMillis_4 = 0;

        // TAP SYNC //
        // we get here if more than MAX_TAP_MILLIS_DELTA pass since last tap
        // e.g. the first tap when setting tempo
        //      or just a single tap intended to sync LFO or pattern
        // if in pattern mode, reset the pattern step & repeat counters
        if(g_patternMode){
            g_patRepeatCtr = 0;
            g_patStepCtr = 0;
        }
        lfoCycle_ISR(); 
        
        return;
    }
    // average consecutive taps
    uint32_t tapDeltaThreshold = tapDelta1 / 10;
    uint32_t tapDeltaAvg = tapDelta1;
    if( absDiffU32(tapDelta1, tapDelta2) <= tapDeltaThreshold )
    { 
        if( absDiffU32(tapDelta1, tapDelta3) <= tapDeltaThreshold )
            // average tempo based on timing of last 4 taps
            tapDeltaAvg = (tapDelta1 + tapDelta2 + tapDelta3) / 3;
        else
            // average tempo based on timing of last 3 taps
            tapDeltaAvg = (tapDelta1 + tapDelta2) / 2;
    }
    g_bpm = 60000.0 / (double)tapDeltaAvg;
    bpm2freq();
    g_updateDisplay = true;
}
// ********************************************************
// Handle tremolo mode switch
void tremModeSwitchPressed(){
    g_harmonicTremMode = !g_harmonicTremMode;
    g_updateDisplay = true;
}
// ********************************************************
// ROT1 switch pressed
void rot1SwitchPressed(){
    g_rot1Mode += 1;
    if(g_rot1Mode > ROT1_MODE_MAX) g_rot1Mode = 0;
    g_updateDisplay = true;
}
// ********************************************************
// ROT2 switch pressed
void rot2SwitchPressed(){
    g_rot2Mode += 1;
    if(g_rot2Mode > ROT2_MODE_MAX) g_rot2Mode = 0;
    g_updateDisplay = true;
}
// ********************************************************
// ROT3 switch pressed
void rot3SwitchPressed(){
    g_rot3Mode += 1;
    if(g_rot3Mode > ROT3_MODE_MAX) g_rot3Mode = 0;
    g_updateDisplay = true;
}

// ********************************************************
void handleButtons(){
    static bool firstTimeHere = true;
    // scan switches
    g_button1.update();
    g_button2.update();
    g_button3.update();
    g_encoder1SW.update();
    g_encoder2SW.update();
    g_encoder3SW.update();
    if(firstTimeHere){
        firstTimeHere = false;
        return;
    }
    
    if (g_button1.fell()) { changeLfoWaveType(1.0, 0); }
    if (g_button2.fell()) { tremModeSwitchPressed(); }    
    if (g_button3.fell()) { tapTempoPressed(); }

    if (g_encoder1SW.fell()) { rot1SwitchPressed(); }
    if (g_encoder2SW.fell()) { rot2SwitchPressed(); }
    if (g_encoder3SW.fell()) { rot3SwitchPressed(); }
}
// ********************************************************

#endif