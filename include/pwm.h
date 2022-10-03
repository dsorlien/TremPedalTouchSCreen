#ifndef _pwm
#define _pwm

#include <Arduino.h>
#include "TremPedal.h"

extern void freq2bpm(void);
extern void bpm2freq(void);
extern uint32_t absDiffU32 (uint32_t x, uint32_t y);

IntervalTimer g_pwmUpdateTimer;
IntervalTimer g_lfoCycleTimer;

// in the wave generation functions below, 'offset' sweeps from 0.0 to 'g_period'
// to support tap-sync, a value of 0 corresponds to point in lfo wave that falls on the beat
//   let's call this the 'tap-sync-phase'
//   TODO: make tap-sync-phase adjustable so user can create interesting syncopated rhythms

int waveSin(double offset) {
    // apply min and max limits
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    uint8_t dc = ((gdc > MAX_SIN_DC) ? MAX_SIN_DC : (gdc < MIN_SIN_DC) ? MIN_SIN_DC : gdc);
    // distort the sine wave based on duty cycle setting
    double rads;
    if(offset <= dc / 2.0)
        rads = M_PI * offset / dc;
    else if (offset <= g_period - (dc / 2.0))
        rads = 0.5 * M_PI + M_PI * (offset - dc / 2.0) / (g_period - dc);
    else
        rads = 1.5 * M_PI + M_PI * (offset - g_period + (dc / 2.0)) / dc;
    return map(cos(rads), -1.0, 1.0, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT); 
}
int waveSquare(double offset) {
    // apply min and max limits (to avoid div by zero and other bad things)
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    uint8_t dc = ((gdc > MAX_SQ_DC) ? MAX_SQ_DC : (gdc < MIN_SQ_DC) ? MIN_SQ_DC : gdc);
    // distort square wave based on duty cycle setting - duty cycle becomes pulse width control
    if(offset < dc) return MAX_WAVEGEN_OUT;
    else return MIN_WAVEGEN_OUT;
}
int waveTri(double offset) {
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    uint8_t dc = gdc;
    // here the duty cycle setting transitions the LFO wave from saw (down) to triangle to saw (up)
    //if(offset < dc) return map(offset, 0.0, dc, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT);
    //else return map(offset, dc, g_period, MAX_WAVEGEN_OUT, MIN_WAVEGEN_OUT);
    if(offset <= dc) 
        return map(offset, 0.0, dc, MAX_WAVEGEN_OUT, MIN_WAVEGEN_OUT);
    else 
        return map(offset, dc, g_period, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT);   
}
int waveExp(double offset) {
    // apply min and max limits to duty cycle (to avoid div by zero and other bad things)
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    uint8_t dc = ((gdc > MAX_EXP_DC) ? MAX_EXP_DC : (gdc < MIN_EXP_DC) ? MIN_EXP_DC : gdc);
    double a;
    double val = 0.0;
    if(offset <= dc){
        a = offset / dc; // is is val between 0 and 1
        val = exp(3.0 * a) - 1.0;
        val = 1.0 - val / 17.92;
    }else{
        a = (offset - dc) / (g_period - dc);
        val = exp(3.0 * a) - 1.0;
        val = val / 17.92;
    }
    val = constrain(val, 0.0, 1.00);
    return map(val, 0.0, 1.0, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT);
}
int waveLog(double offset) {
    // apply min and max limits to duty cycle (so dc != 0 and dc != g_period)
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    uint8_t dc = ((gdc > MAX_LOG_DC) ? MAX_LOG_DC : (gdc < MIN_LOG_DC) ? MIN_LOG_DC : gdc);
    double a;
    double val = 0.0;
    if(offset <= dc){
        a = offset / dc; // is is val between 0 and 1
        val = log(a + 0.01);
        val = 1.0 - (val / 4.6 + 1.0);
    }else{
        a = (offset - dc) / (g_period - dc);
        val = log(a + 0.01);
        val = val / 4.6 + 1.0;
    }
    val = constrain(val, 0.0, 1.00);
    return map(val, 0.0, 1.0, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT);
}

void pwmUpdate_ISR(){
    //digitalWrite(DEBUG_OUT, 0);

    static int prev_val = 0;
    int val_out = 0;
    int val = 0;

    // increment the offset based on LFO freq and PWM update interval
    //g_offset += g_period * g_freq_hz / 1000.0;
    g_offset += g_period * g_freq_hz * (PWM_ISR_UPDATE_uS / 1.0E6);  

    // get next LFO waveform point, should return a value between 0 and 255
    switch (g_patternMode ? g_wave_pat : g_wave)
    {
    case 0:
        val = waveSin(g_offset);
        break;
    case 1:
        val = waveTri(g_offset);
        break;
    case 2:
        val = waveSquare(g_offset);
        break;
    case 3:
        val = waveExp(g_offset);
        break;
    case 4:
        val = waveLog(g_offset);
        break;
    default:
        val = waveSin(g_offset);
        break;
    }
    int dpth = g_patternMode ? g_depth_pat : g_depth;
    val_out = val * dpth / MAX_DEPTH; 

    // limit the slew rate
    int slewLim = map(g_slewLimit, MIN_SLEW_LIMIT, MAX_SLEW_LIMIT, MIN_WAVEGEN_OUT, MAX_WAVEGEN_OUT);
    if(g_slewRateLimitOn)
    {
        if(val_out - prev_val > slewLim){
            val_out = prev_val + slewLim;
        }
        if(prev_val - val_out > slewLim){
            val_out = prev_val - slewLim;
        }
    }

    // save value so we can limit slew for next sample
    prev_val = val_out;
    // create inverse wave, adjust for depth, and constrain to safe range
    int val_inv = constrain(((dpth * MAX_ANALOG_WRITE) / MAX_DEPTH) - val_out, MIN_ANALOG_WRITE, MAX_ANALOG_WRITE); 
    
    bool htmode = g_patternMode ? g_harmonicTremMode_pat : g_harmonicTremMode;

    // update PWM duty cycles
    if(g_invertPWM)
    {
        // pattern mode mute
        if(g_patternMode && (g_muteFullRandom == 1)){
            val_out = MAX_ANALOG_WRITE;
            val_inv = MAX_ANALOG_WRITE;
        }
        analogWrite(PWMOUT1_PIN, val_inv); 
        analogWrite(PWMOUT2_PIN, htmode ? val_out : val_inv); 
        analogWrite(PWMOUT3_PIN, val_inv); 
    }
    else
    {
        // pattern mode mute
        if(g_patternMode && (g_muteFullRandom == 1)){
            val_out = MIN_ANALOG_WRITE;
            val_inv = MIN_ANALOG_WRITE;
        }
        analogWrite(PWMOUT1_PIN, val_out); 
        analogWrite(PWMOUT2_PIN, htmode ? val_inv : val_out); 
        analogWrite(PWMOUT3_PIN, val_out); 
    }
}

// interrupt fires at the 'top' of each LFO cycle
void lfoCycle_ISR(){
    //if(!g_patternMode) digitalWrite(DEBUG_OUT, 1);

    /////////// Pattern Tremolo ///////////////
    //static int patStepCtr = 0;
    //static int patRepeatCtr = 0;
    if(g_patternMode)
    {
        PatternSeq sq = g_patternSeqs[g_patternSeqNum];
        
        // Repeat counter
        if(g_patRepeatCtr >= sq.patternSteps[g_patStepCtr].repeats){
            // if we are finished w/ repeats for this step
            g_patRepeatCtr = 0;
            // check if we are on the 'end' step
            if(sq.patternSteps[g_patStepCtr].end == 1){
                g_patStepCtr = NUM_PAT_STEPS - 1; // this will get us back to first step
            }
            // go to next step
            // - we have to skip unused steps, avoid overrrun
            do{
                g_patStepCtr ++;
                if(g_patStepCtr >= NUM_PAT_STEPS){ // if last step
                    g_patStepCtr = 0; // loop around to first step
                    sq.patternSteps[g_patStepCtr].skip = 0; // 1st step cannot be skipped
                }
            } while (sq.patternSteps[g_patStepCtr].skip == 1); 
        }     

        //if(g_patStepCtr == 0 && g_patRepeatCtr == 0) digitalWrite(DEBUG_OUT, 1);

        // keep track of repeats
        g_patRepeatCtr += 1;

        // pointing to correct step now, so set trem params for this cycle
        // required parameters
        PatternStep mystep = sq.patternSteps[g_patStepCtr];
        g_beatDivIdx_pat = constrain(mystep.div, MIN_BEAT_DIV, MAX_BEAT_DIV);
        g_muteFullRandom = constrain(mystep.mute, 0, 3);

        // optional parameters
        if(mystep.harmMode >= 0) 
            g_harmonicTremMode_pat = constrain(mystep.harmMode, 0, 1);
        else
            g_harmonicTremMode_pat = g_harmonicTremMode;

        if(mystep.depth >= 0) 
            g_depth_pat = constrain(mystep.depth, MIN_DEPTH, MAX_DEPTH);
        else
            g_depth_pat = g_depth;

        if(mystep.dutyCycle >= 0) 
            g_dutycycle_pat = constrain(mystep.dutyCycle, MIN_DUTY_CTL, MAX_DUTY_CTL);
        else
            g_dutycycle_pat = g_dutycycle;

        if(mystep.wave >= 0) 
            g_wave_pat = constrain(mystep.wave, MIN_WAVE, MAX_WAVE);
        else
            g_wave_pat = g_wave;

        // need to do these things after changing beatDivIdx
        g_beatdiv = getDivVal(g_beatDivIdx_pat);
        bpm2freq(); 
        g_updateDisplay = true;
    }

    g_pwmUpdateTimer.begin(pwmUpdate_ISR, PWM_ISR_UPDATE_uS);
    g_lfoCycleTimer.begin(lfoCycle_ISR, 1.0E6 / g_freq_hz);
    g_offset = 0.0;

}

void initPwm(){
    analogWriteResolution(PWM_RESOLUTION);
    analogWriteFrequency(PWMOUT1_PIN, PWM_FREQ_Hz); 
    analogWriteFrequency(PWMOUT2_PIN, PWM_FREQ_Hz);  
    analogWriteFrequency(PWMOUT3_PIN, PWM_FREQ_Hz);  
    analogWrite(PWMOUT1_PIN, 128); 
    analogWrite(PWMOUT2_PIN, 128); 
    analogWrite(PWMOUT3_PIN, 128); 
    g_pwmUpdateTimer.begin(pwmUpdate_ISR, PWM_ISR_UPDATE_uS);
    g_lfoCycleTimer.begin(lfoCycle_ISR, 1.0E6 / g_freq_hz);
}

#endif