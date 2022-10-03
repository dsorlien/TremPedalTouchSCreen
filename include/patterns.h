#ifndef _patterns
#define _patterns

#include <Arduino.h>
#include "TremPedal.h"

int16_t factoryPatterns[] = {
//  rpt div mut skp end wav dty dep hrm     PATTERN 1
    4,  12, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/4
    8,  15, 0,  0,  1,  -1, -1, -1, -1,     // 8 repeats of 1/8 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 2
    4,  15, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/8
    8,  18, 0,  0,  1,  -1, -1, -1, -1,     // 8 repeats of 1/16 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 3
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    2,  12, 0,  0,  1,  -1, -1, -1, -1,     // 2 repeats of 1/4 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 4
    1,  15, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/8
    2,  18, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/16
    2,  15, 0,  0,  1,  -1, -1, -1, -1,     // 2 repeats of 1/8 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 5
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    4,  18, 0,  0,  1,  -1, -1, -1, -1,     // 4 repeats of 1/16 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 6
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    2,  12, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/4
    8,  15, 0,  0,  1,  -1, -1, -1, -1,     // 8 repeats of 1/8 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 7
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    2,  12, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/4
    6,  13, 0,  0,  1,  -1, -1, -1, -1,     // 6 repeats of 1/4 TRIP - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 8
    2,  12, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/4
    3,  13, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/4 TRIP
    4,  15, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/8
    6,  16, 0,  0,  1,  -1, -1, -1, -1,     // 6 repeats of 1/8 TRIP - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 9
    6,  16, 0,  0,  0,  -1, -1, -1, -1,     // 6 repeats of 1/8 TRIP
    4,  15, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/8
    3,  13, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/4 TRIP
    2,  12, 0,  0,  1,  -1, -1, -1, -1,     // 2 repeats of 1/4 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 10
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    6,  19, 0,  0,  0,  -1, -1, -1, -1,     // 6 repeats of 1/16 TRIP
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    3,  16, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/8 TRIP
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  1,  -1, -1, -1, -1,     // 2 repeats of 1/8 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 11
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    8,  21, 0,  0,  1,  -1, -1, -1, -1,     // 8 repeats of 1/32 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 12
    8,  21, 0,  0,  0,  -1, -1, -1, -1,     // 8 repeats of 1/32
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    1,  12, 0,  0,  1,  -1, -1, -1, -1,     // 1 repeats of 1/4 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 13
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    3,  16, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/8 TRIP
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    6,  19, 0,  0,  1,  -1, -1, -1, -1,     // 6 repeats of 1/16 TRIP - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 14
    3,  15, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/8
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    1,  15, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/8
    1,  12, 0,  0,  1,  -1, -1, -1, -1,     // 1 repeats of 1/4
//  rpt div mut skp end wav dty dep hrm     PATTERN 15
    1,  12, 0,  0,  0,  -1, -1, -1, -1,     // 1 repeats of 1/4
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    8,  21, 0,  0,  1,  -1, -1, -1, -1,     // 8 repeats of 1/32 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 16
    8,  21, 0,  0,  0,  -1, -1, -1, -1,     // 8 repeats of 1/32
    4,  18, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/8
    1,  12, 0,  0,  1,  -1, -1, -1, -1,     // 1 repeats of 1/4 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 17
    8,  21, 0,  0,  0,   0, -1, -1, -1,     // 8 repeats of 1/32
    4,  18, 0,  0,  0,   1, -1, -1, -1,     // 4 repeats of 1/16
    2,  15, 0,  0,  0,   2, -1, -1, -1,     // 2 repeats of 1/8
    8,  21, 0,  0,  0,   3, -1, -1, -1,     // 8 repeats of 1/32
    4,  18, 0,  0,  1,   4, -1, -1, -1,     // 4 repeats of 1/16 - END
//  rpt div mut skp end wav dty dep hrm     PATTERN 18
    4,  15, 0,  0,  0,  -1, -1, -1, -1,     // 4 repeats of 1/16
    1,  15, 1,  0,  0,  -1, -1, -1, -1,     // mute 1/16
    3,  15, 0,  0,  0,  -1, -1, -1, -1,     // 3 repeats of 1/16
    1,  15, 1,  0,  0,  -1, -1, -1, -1,     // mute 1/16
    2,  15, 0,  0,  0,  -1, -1, -1, -1,     // 2 repeats of 1/16 
    1,  15, 1,  0,  1,  -1, -1, -1, -1,     // mute 1/16         - END
};
const int factPatLen = sizeof(factoryPatterns)/sizeof(factoryPatterns[0]);

void loadDefaultPatterns(){ 
    int seq = 0;
    int step = 0;

    // parse array
    for(int i = 0; i + 9 <= factPatLen; i += 9)
    {
        // extraxt step params from array
        g_patternSeqs[seq].patternSteps[step].repeats = constrain(factoryPatterns[i], MIN_REPEATS, MAX_REPEATS);
        g_patternSeqs[seq].patternSteps[step].div = constrain(factoryPatterns[i+1], MIN_BEAT_DIV, MAX_BEAT_DIV);
        g_patternSeqs[seq].patternSteps[step].mute = constrain(factoryPatterns[i+2], 0, 1);
        g_patternSeqs[seq].patternSteps[step].skip = constrain(factoryPatterns[i+3], 0, 1);
        g_patternSeqs[seq].patternSteps[step].end = constrain(factoryPatterns[i+4], 0, 1);
        g_patternSeqs[seq].patternSteps[step].wave = constrain(factoryPatterns[i+5], -1, MAX_WAVE);
        g_patternSeqs[seq].patternSteps[step].dutyCycle = constrain(factoryPatterns[i+6], -1, MAX_DUTY_CTL);
        g_patternSeqs[seq].patternSteps[step].depth = constrain(factoryPatterns[i+7], -1, MAX_DEPTH);
        g_patternSeqs[seq].patternSteps[step].harmMode = constrain(factoryPatterns[i+8], -1, 1);

        if(step + 1 == NUM_PAT_STEPS){
            // last step before overrun - force it to be end of sequence
            g_patternSeqs[seq].patternSteps[step].end = 1;
        }
        if(g_patternSeqs[seq].patternSteps[step].end == 1){
            // end of sequence. point to 1st step in next sequence
            seq += 1;
            step = 0;
            // don't overrun sequence memory
            if(seq >= NUM_PAT_SEQS) return;
        } 
        else {
            // not the end of the sequence
            step += 1;
        }
    }
}
    
void initPatternStep(int seq, int step){
    // required params
    g_patternSeqs[seq].patternSteps[step].repeats = 1;
    g_patternSeqs[seq].patternSteps[step].div = 12;
    g_patternSeqs[seq].patternSteps[step].mute = 0;
    g_patternSeqs[seq].patternSteps[step].skip = 0;
    g_patternSeqs[seq].patternSteps[step].end = 1;

    // optional params - set to -1 to use global setting(s)
    g_patternSeqs[seq].patternSteps[step].wave = -1;
    g_patternSeqs[seq].patternSteps[step].dutyCycle = -1;
    g_patternSeqs[seq].patternSteps[step].depth = -1;
    g_patternSeqs[seq].patternSteps[step].harmMode = -1;
}

void initPatterns(){
    // init 
    for(int seq=0; seq<NUM_PAT_SEQS; seq++){
        for(int step=0; step<NUM_PAT_STEPS; step++){
            initPatternStep(seq, step);
        }
    }
    loadDefaultPatterns();
}


#endif