#include <stdio.h>
#include <stdint.h>

#include "capstone_morse.h"
#include "capstone_input.h"

#include "pico/time.h"

#define DASH_TIME 3000000
#define DOT_TIME 1000000
#define LENIENCY 250000

const char* morse_table[] =
{
        " ", //" " (space)
        ".-", //A
        "-...", //B
        "-.-.", //C
        "-..", //D
        ".", //E
        "..-.", //F
        "--.", //G
        "....", //H
        "..", //I
        ".---", //J
        "-.-", //K
        ".-..", //L
        "--", //M
        "-.", //N
        "---", //O
        ".--.", //P
        "--.-", //Q
        ".-.", //R
        "...", //S
        "-", //T
        "..-", //U
        "...-", //V
        ".--", //W
        "-..-", //X
        "-.--", //Y
        "--..", //Z
};

//first char is a space
const char alphabet[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ-.";

void interpret_buttons(int state)
{
    switch (state)
    {
        case 1: //input button just pressed
            //check length of last unpress
            int64_t unpress_length = absolute_time_diff_us(end_time,start_time);
            //0.5s leniency, morse in same letter separated by 1 dot, letters by 3 dots, words by 7 dots
            if(DOT_TIME - LENIENCY < unpress_length < DOT_TIME + LENIENCY)
            {
                //same letter, do nothing
            } 
            else if(DOT_TIME*3 - LENIENCY < unpress_length < DOT_TIME*3 + LENIENCY)
            {
                //append letter to message, clear letter
            } 
            else if(DOT_TIME*7 - LENIENCY < unpress_length < DOT_TIME*7 + LENIENCY)
            {
                //clear letter, append space to message
            } 
            state = 0;
            break;
        case 2: //input button just released
            //check length of last press
            int64_t press_length = absolute_time_diff_us(start_time,end_time);
            //0.5s leniency, dot is 1s, dash is 3s
            if(DOT_TIME - LENIENCY < press_length < DOT_TIME + LENIENCY)
            {
                //do dot
            }
            else if(DASH_TIME - LENIENCY < press_length < DASH_TIME + LENIENCY)
            {
                //do dash
            }
            state = 0;
            break;
        case 3: //send button just pressed
            test[0] = 'c';
            state = 0;
            break;
        default: //holding pattern for startup/after message is sent, waiting for input
            while(state==0);
            break;
    }
}