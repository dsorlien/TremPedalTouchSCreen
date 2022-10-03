#ifndef _beatdiv
#define _beatdiv

#include <Arduino.h>
#include "TremPedal.h"

String getDivString(int beatDivIdx)
{
    if(beatDivIdx ==  0)       return "4       ";
    else if(beatDivIdx ==  1)  return "4 Trp   ";
    else if(beatDivIdx ==  2)  return "2 Dot   ";    
    else if(beatDivIdx ==  3)  return "2       ";    
    else if(beatDivIdx ==  4)  return "2 Trp   ";
    else if(beatDivIdx ==  5)  return "1 Dot   ";
    else if(beatDivIdx ==  6)  return "1       ";
    else if(beatDivIdx ==  7)  return "1 Trp   ";
    else if(beatDivIdx ==  8)  return "1/2 Dot ";
    else if(beatDivIdx ==  9)  return "1/2     ";
    else if(beatDivIdx ==  10) return "1/2 Trp ";
    else if(beatDivIdx ==  11) return "1/4 Dot ";
    else if(beatDivIdx ==  12) return "1/4     ";
    else if(beatDivIdx ==  13) return "1/4 Trp ";
    else if(beatDivIdx ==  14) return "1/8 Dot ";
    else if(beatDivIdx ==  15) return "1/8     ";
    else if(beatDivIdx ==  16) return "1/8 Trp ";
    else if(beatDivIdx ==  17) return "1/16 Dot";
    else if(beatDivIdx ==  18) return "1/16    ";
    else if(beatDivIdx ==  19) return "1/16 Trp";
    else if(beatDivIdx ==  20) return "1/32 Dot";
    else if(beatDivIdx ==  21) return "1/32    ";
    else if(beatDivIdx ==  22) return "1/32 Trp";
    return "err";
}
String getDivStringA(int beatDivIdx)
{
    if(beatDivIdx ==  0)       return "4";
    else if(beatDivIdx ==  1)  return "4";
    else if(beatDivIdx ==  2)  return "2";    
    else if(beatDivIdx ==  3)  return "2";    
    else if(beatDivIdx ==  4)  return "2";
    else if(beatDivIdx ==  5)  return "1";
    else if(beatDivIdx ==  6)  return "1";
    else if(beatDivIdx ==  7)  return "1";
    else if(beatDivIdx ==  8)  return "1/2 ";
    else if(beatDivIdx ==  9)  return "1/2";
    else if(beatDivIdx ==  10) return "1/2 ";
    else if(beatDivIdx ==  11) return "1/4";
    else if(beatDivIdx ==  12) return "1/4";
    else if(beatDivIdx ==  13) return "1/4";
    else if(beatDivIdx ==  14) return "1/8";
    else if(beatDivIdx ==  15) return "1/8";
    else if(beatDivIdx ==  16) return "1/8";
    else if(beatDivIdx ==  17) return "1/16";
    else if(beatDivIdx ==  18) return "1/16";
    else if(beatDivIdx ==  19) return "1/16";
    else if(beatDivIdx ==  20) return "1/32";
    else if(beatDivIdx ==  21) return "1/32";
    else if(beatDivIdx ==  22) return "1/32";
    return "err";
}
String getDivStringB(int beatDivIdx)
{
    if(beatDivIdx ==  0)       return "";
    else if(beatDivIdx ==  1)  return "TRP";
    else if(beatDivIdx ==  2)  return "DOT";    
    else if(beatDivIdx ==  3)  return "";    
    else if(beatDivIdx ==  4)  return "TRP";
    else if(beatDivIdx ==  5)  return "DOT";
    else if(beatDivIdx ==  6)  return "";
    else if(beatDivIdx ==  7)  return "TRP";
    else if(beatDivIdx ==  8)  return "DOT";
    else if(beatDivIdx ==  9)  return "";
    else if(beatDivIdx ==  10) return "TRP";
    else if(beatDivIdx ==  11) return "DOT";
    else if(beatDivIdx ==  12) return "";
    else if(beatDivIdx ==  13) return "TRP";
    else if(beatDivIdx ==  14) return "DOT";
    else if(beatDivIdx ==  15) return "";
    else if(beatDivIdx ==  16) return "TRP";
    else if(beatDivIdx ==  17) return "DOT";
    else if(beatDivIdx ==  18) return "";
    else if(beatDivIdx ==  19) return "TRP";
    else if(beatDivIdx ==  20) return "DOT";
    else if(beatDivIdx ==  21) return "";
    else if(beatDivIdx ==  22) return "TRP";
    return "err";
}

double getDivVal(int beatDivIdx)
{
    if(beatDivIdx ==  0)          return 16.0;  
    else if(beatDivIdx ==  1)     return 10.666;
    else if(beatDivIdx ==  2)     return 12.0;    
    else if(beatDivIdx ==  3)     return 8.0;    
    else if(beatDivIdx ==  4)     return 5.333;   
    else if(beatDivIdx ==  5)     return 6.0;     
    else if(beatDivIdx ==  6)     return 4.0;     
    else if(beatDivIdx ==  7)     return 2.666;   
    else if(beatDivIdx ==  8)     return 3.0;     
    else if(beatDivIdx ==  9)     return 2.0;     
    else if(beatDivIdx ==  10)    return 1.333;   
    else if(beatDivIdx ==  11)    return 1.5;     
    else if(beatDivIdx ==  12)    return 1.0;     
    else if(beatDivIdx ==  13)    return 0.666;   
    else if(beatDivIdx ==  14)    return 0.75;    
    else if(beatDivIdx ==  15)    return 0.5;     
    else if(beatDivIdx ==  16)    return 0.333;   
    else if(beatDivIdx ==  17)    return 0.375;   
    else if(beatDivIdx ==  18)    return 0.25;    
    else if(beatDivIdx ==  19)    return 0.1666;  
    else if(beatDivIdx ==  20)    return 0.1875;  
    else if(beatDivIdx ==  21)    return 0.125;   
    else if(beatDivIdx ==  22)    return 0.08333; 
    return 0.0;
}

#endif