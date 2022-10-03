#ifndef _oled
#define _oled

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include "XPT2046_Calibrated.h"
#include "TremPedal.h"
#include "beatdiv.h"

void printStringCentered(String s, uint8_t fontSizeX, uint8_t fontSizeY, int16_t xpos, int16_t ypos, int16_t width) {
    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;
    tft.setTextSize(fontSizeX, fontSizeY);
    tft.getTextBounds(s, xpos, ypos, &x1, &y1, &w, &h);
    tft.setCursor(xpos + (width - w) / 2, ypos);
    tft.println(s);
}

void drawLFOWave(int16_t xpos, int16_t ypos, int16_t width, int16_t height){
    uint16_t pixel_y = 0, pixel_x = 0;
    uint16_t pixel_y_old = ypos + height / 2;
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;

    //if(g_wave==4) pixel_y_old = ypos + height;
    if(g_wave==0 || g_wave==1 || g_wave==3 || g_wave==4) pixel_y_old = ypos;

    for(int16_t i = 0; i < width; i++){
        double offs = (double)i / ((double)width - 1) * g_period;
        if(g_wave == 0){ // sine
            // use same math as used above for LFO wave generation
            uint8_t dc = ((gdc > MAX_SIN_DC) ? MAX_SIN_DC : (gdc < MIN_SIN_DC) ? MIN_SIN_DC : gdc);
            double rads;
            if(offs <= dc / 2.0)
                rads = M_PI * offs / dc;
            else if (offs <= g_period - (dc / 2.0))
                rads = 0.5 * M_PI + M_PI * (offs - dc / 2.0) / (g_period - dc);
            else
                rads = 1.5 * M_PI + M_PI * (offs - g_period + (dc / 2.0)) / dc;
            pixel_y = ypos + map(cos(rads), -1.0, 1.0, height, 0); 
        }
        else if (g_wave == 1){ // tri
            // use same math as used above for LFO wave generation
            uint8_t dc = ((gdc > MAX_TRI_DC) ? MAX_TRI_DC : (gdc < MIN_TRI_DC) ? MIN_TRI_DC : gdc);
            if(offs <= dc) 
                pixel_y = ypos + map(offs, 0.0, dc, 0, height);
            else 
                pixel_y = ypos + map(offs, dc, g_period, height, 0);            
        }
        else if (g_wave == 2){ // square
            // apply min and max limits
            uint8_t dc = ((gdc > MAX_SQ_DC) ? MAX_SQ_DC : (gdc < MIN_SQ_DC) ? MIN_SQ_DC : gdc);
            if(offs < dc) 
                pixel_y = ypos;
            else if(offs >= g_period)
                pixel_y = ypos + height / 2;
            else 
                pixel_y = ypos + height;
        }
        else if (g_wave == 3){ // exp
            // apply min and max limits to duty cycle (to avoid div by zero and other bad things)
            uint8_t dc = ((gdc > MAX_EXP_DC) ? MAX_EXP_DC : (gdc < MIN_EXP_DC) ? MIN_EXP_DC : gdc);
            double a;
            double val = 0.0;
            if(offs <= dc){
                a = offs / dc; // is is val between 0 and 1
                val = exp(3.0 * a) - 1.0;
                val = val / 17.92;
            }else{
                a = (offs - dc) / (g_period - dc);
                val = exp(3.0 * a) - 1.0;
                val = 1.0 - val / 17.92;
            }
            val = constrain(val, 0.0, 1.0);
            pixel_y = ypos +  map(val, 0.0, 1.0, 0, height);
        }
        else if (g_wave == 4){ // log
            // apply min and max limits
            uint8_t dc = ((gdc > MAX_LOG_DC) ? MAX_LOG_DC : (gdc < MIN_LOG_DC) ? MIN_LOG_DC : gdc);
            double a;
            double val = 0.0;
            if(offs <= dc){
                a = offs / dc; // is is val between 0 and 1
                val = log(a + 0.01);
                val = val / 4.6 + 1.0;
            }else{
                a = (offs - dc) / (g_period - dc);
                val = log(a + 0.01);
                val = 1.0 - (val / 4.6 + 1.0);
            }
            val = constrain(val, 0.0, 1.0);
            pixel_y = ypos +  map(val, 0.0, 1.0, 0, height);
        }
        pixel_x = xpos + i + 1;        
        
        if(pixel_y > pixel_y_old){
            for(uint16_t y = pixel_y_old + 1; y <= pixel_y; y++)
                tft.drawPixel(pixel_x, y, WAVEFORM_COLOR);
        }
        else if(pixel_y < pixel_y_old){
            for(uint16_t y = pixel_y_old - 1; y >= pixel_y; y--)
                tft.drawPixel(pixel_x, y, WAVEFORM_COLOR);
        }
        else if (pixel_y == pixel_y_old){
            tft.drawPixel(pixel_x, pixel_y, WAVEFORM_COLOR);
        }

        pixel_y_old = pixel_y;
    }
}

void updateDisplay() {
    static uint32_t lastDisplayUpdate;
    int gdc = g_patternMode ? g_dutycycle_pat : g_dutycycle;
    bool htmode = g_patternMode ? g_harmonicTremMode_pat : g_harmonicTremMode;
    int wav = g_patternMode ? g_wave_pat : g_wave;
    int dpth = g_patternMode ? g_depth_pat : g_depth;
    int bdiv = g_patternMode ? g_beatDivIdx_pat :  g_beatDivIdx;

    if(millis() - lastDisplayUpdate > MIN_OLED_UPDATE_INTERVAL_MS) g_updateDisplay = true; 
    if(g_updateDisplay) {
        g_updateDisplay = false;
        lastDisplayUpdate = millis();
        tft.fillScreen(BACKGROUND_COLOR);

        // define location items displayed on display
        const int16_t HORIZ_TAB_0 = 0; 
        const int16_t HORIZ_TAB_1 = 106; 
        const int16_t HORIZ_TAB_2 = 212; 

        const int16_t VERT_SMALL_LEGEND = 190; 
        const int16_t VERT_PARAM = 210; 

        const int16_t H_POS_WAVEDISPLAY = 100; 
        const int16_t V_POS_WAVEDISPLAY = 32; 
        const int16_t H_SIZE_WAVEDISPLAY = 200; 
        const int16_t V_SIZE_WAVEDISPLAY = 100; 
        
        const int16_t VERT_TINY_SPACING = 20; 
        const int16_t H_TINY_TAB0 = 0; 
        const int16_t H_TINY_TAB1 = 50; 

        const int16_t FONTSIZE_TINY = 2;
        const int16_t FONTSIZE_MED = 2;
        const int16_t FONTSIZE_BIG = 3;

        ////////////////////////////////////////////////////////
        // for now, waste some space at top of display
        tft.setTextSize(FONTSIZE_BIG);                 
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(0,0);                 // Start at top-left corner
        tft.println("Teensy 4.1 Trem");

        ////////////////////////////////////////////////////////
        // upper part of display shows alt params
        tft.setTextSize(FONTSIZE_TINY);
        int16_t vpos = 32;

        if(1){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.print(htmode ? "HAR" : "TRM"); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println(g_patternMode ? " PAT" : " "); 
            vpos += VERT_TINY_SPACING;
        }
        if(g_rot1Mode != ROT1_MODE_BPM){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.println("BPM"); 
            tft.setCursor(H_TINY_TAB1, vpos); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println((int)g_bpm); 
            vpos += VERT_TINY_SPACING;
        }
        //if(g_rot1Mode != ROT1_MODE_HARMTREM){
        //    g_OLED.setCursor(H_TINY_TAB0, vpos); 
        //    g_OLED.println("MODE"); 
        //    g_OLED.setCursor(H_TINY_TAB1, vpos); 
        //    g_OLED.println(htmode ? "HAR" : "TRM"); 
        //    vpos += VERT_TINY_SPACING;
        //}
        if(g_rot2Mode != ROT2_MODE_BEAT_DIV){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println(getDivString(bdiv)); 
            vpos += VERT_TINY_SPACING;
        }
        if(g_rot2Mode != ROT2_MODE_WAVE_TYPE){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.println("WAVE"); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.setCursor(H_TINY_TAB1, vpos); 
            if(wav == 0) tft.println("SIN");
            if(wav == 1) tft.println("TRI");
            if(wav == 2) tft.println("SQU");
            if(wav == 3) tft.println("EXP");
            if(wav == 4) tft.println("LOG");
            vpos += VERT_TINY_SPACING;
        }
        if(g_rot3Mode != ROT3_MODE_DEPTH){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.println("DEP"); 
            tft.setCursor(H_TINY_TAB1, vpos); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println(dpth); 
            vpos += VERT_TINY_SPACING;
        }
        if(g_rot3Mode != ROT3_MODE_DUTY_CYCLE){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.println("DUTY"); 
            tft.setCursor(H_TINY_TAB1, vpos); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println(gdc); 
            vpos += VERT_TINY_SPACING;
        }
        if(g_patternMode && g_rot3Mode != ROT3_MODE_PATTERN_NUM){
            tft.setCursor(H_TINY_TAB0, vpos); 
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            tft.println("PAT#"); 
            tft.setCursor(H_TINY_TAB1, vpos); 
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            tft.println(g_patternSeqNum + 1); 
            vpos += VERT_TINY_SPACING;
        }

        
        // debug

        //if(g_rot1Mode != ROT1_MODE_SLEW_LIMIT){
        //    g_OLED.setCursor(H_TINY_TAB0, vpos); 
        //    g_OLED.println("SLEW"); 
        //    g_OLED.setCursor(H_TINY_TAB1, vpos); 
        //    g_OLED.println(g_slewLimit); 
        //    vpos += VERT_TINY_SPACING;
        //}
        
        //g_OLED.setCursor(H_TINY_TAB0, vpos); 
        //g_OLED.println("SLEW"); 
        //g_OLED.setCursor(H_TINY_TAB1, vpos); 
        //g_OLED.println(slewRateLimitOn ? "ON" : "OFF"); 
        //vpos += VERT_TINY_SPACING;

        //g_OLED.setCursor(H_TINY_TAB0, vpos); 
        //g_OLED.println("INT"); 
        //g_OLED.setCursor(H_TINY_TAB1, vpos); 
        //g_OLED.println(enablePwmCycleInt ? "ON" : "OFF");
        //vpos += VERT_TINY_SPACING;

        ////////////////////////////////////////////////////////
        // lower 1/3rd of display shows rotary assignments

        if(g_rot1Mode == ROT1_MODE_BPM){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("BPM", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_0, VERT_SMALL_LEGEND, HORIZ_TAB_1 - HORIZ_TAB_0);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered((int)g_bpm, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_0, VERT_PARAM, HORIZ_TAB_1 - HORIZ_TAB_0);
        }
        if(g_rot1Mode == ROT1_MODE_HARMTREM){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("MODE", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_0, VERT_SMALL_LEGEND, HORIZ_TAB_1 - HORIZ_TAB_0);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(htmode ? "HAR" : "TRM", FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_0, VERT_PARAM, HORIZ_TAB_1 - HORIZ_TAB_0);
        }        
        if(g_rot1Mode == ROT1_MODE_PATTERN_MODE){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("PAT", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_0, VERT_SMALL_LEGEND, HORIZ_TAB_1 - HORIZ_TAB_0);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(g_patternMode ? "ON" : "OFF", FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_0, VERT_PARAM, HORIZ_TAB_1 - HORIZ_TAB_0);
        }        
        if(g_rot1Mode == ROT1_MODE_SLEW_LIMIT){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("SLEW", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_0, VERT_SMALL_LEGEND, HORIZ_TAB_1 - HORIZ_TAB_0);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(g_slewLimit, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_0, VERT_PARAM, HORIZ_TAB_1 - HORIZ_TAB_0);
        }
        if(g_rot2Mode == ROT2_MODE_BEAT_DIV){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("DIV", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_1, VERT_SMALL_LEGEND, HORIZ_TAB_2 - HORIZ_TAB_1);
            String divStrB = getDivStringB(bdiv);
            String s = getDivStringA(bdiv) + divStrB;
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            if(divStrB == "")
                printStringCentered(s, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_1, VERT_PARAM, HORIZ_TAB_2 - HORIZ_TAB_1);
            else
                printStringCentered(s, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_1, VERT_PARAM, HORIZ_TAB_2 - HORIZ_TAB_1);
        }
        if(g_rot2Mode == ROT2_MODE_WAVE_TYPE){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("WAVE", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_1, VERT_SMALL_LEGEND, HORIZ_TAB_2 - HORIZ_TAB_1);
            String s = "";
            if(wav == 0) s = "SIN";
            if(wav == 1) s = "TRI";
            if(wav == 2) s = "SQU";
            if(wav == 3) s = "EXP";
            if(wav == 4) s = "LOG";            
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(s, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_1, VERT_PARAM, HORIZ_TAB_2 - HORIZ_TAB_1);
        }
        if(g_rot3Mode == ROT3_MODE_DEPTH){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("DEPTH", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_2, VERT_SMALL_LEGEND, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(dpth, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_2, VERT_PARAM, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
        }
        if(g_rot3Mode == ROT3_MODE_DUTY_CYCLE){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("DUTY", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_2, VERT_SMALL_LEGEND, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(gdc, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_2, VERT_PARAM, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
        }
        if(g_rot3Mode == ROT3_MODE_PATTERN_NUM){
            tft.setTextColor(TEXT_LEGEND_COLOR);    // t
            printStringCentered("PAT#", FONTSIZE_MED, FONTSIZE_MED, HORIZ_TAB_2, VERT_SMALL_LEGEND, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
            tft.setTextColor(TEXT_PARAM_COLOR);    // t
            printStringCentered(g_patternSeqNum + 1, FONTSIZE_BIG, FONTSIZE_BIG, HORIZ_TAB_2, VERT_PARAM, SCREEN_WIDTH - 1 - HORIZ_TAB_2);
        }

        // draw the waveform twice
        drawLFOWave(H_POS_WAVEDISPLAY, V_POS_WAVEDISPLAY, H_SIZE_WAVEDISPLAY / 2, V_SIZE_WAVEDISPLAY);
        drawLFOWave(H_POS_WAVEDISPLAY + H_SIZE_WAVEDISPLAY / 2, V_POS_WAVEDISPLAY, H_SIZE_WAVEDISPLAY / 2, V_SIZE_WAVEDISPLAY);
        
        //tft.display();
    }
}


#endif
