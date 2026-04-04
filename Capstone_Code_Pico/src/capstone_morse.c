#include <stdio.h>
#include <stdint.h>

#include "capstone_morse.h"
#include "capstone_input.h"

#include "pico/time.h"

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
            test[0] = 'a';
            state = 0;
            break;
        case 2: //input button just released
            test[0] = 'b';
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