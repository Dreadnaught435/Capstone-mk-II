#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "capstone_morse.h"
#include "capstone_input.h"
#include "capstone_uart.h"

#include "pico/time.h"

#define DASH_TIME 3000000
#define DOT_TIME 1000000
#define LENIENCY 500000

char message[10];
char letter[4];

int letter_index=0;
int message_index=0;

int64_t press_length;
int64_t unpress_length;

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
        case 1: //input button just pressed, handle last unpress
            //check length of last unpress
            unpress_length = absolute_time_diff_us(end_time,start_time);
            //0.5s leniency, morse in same letter separated by 1 dot, letters by 3 dots, words by 7 dots
            //if same letter, then nothing needs to be done
            //letter done
            if((DOT_TIME*3 - LENIENCY < unpress_length) && (unpress_length < DOT_TIME*3 + LENIENCY))
            {
                //append to message
                message[message_index] = decode(letter,(sizeof(letter)/sizeof(letter[0])));
                message_index++;
                //reset letter
                memset(letter,0,sizeof(letter));
                letter_index = 0;
            } 
            else if((DOT_TIME*7 - LENIENCY < unpress_length) && (unpress_length < DOT_TIME*7 + LENIENCY))
            {
                //append space to message
                message[message_index] = ' ';
                message_index++;
                //reset letter
                memset(letter,0,sizeof(letter));
                letter_index = 0;
            }
            break;
        case 2: //input button just released, handle last press
            //check length of last press
            press_length = absolute_time_diff_us(start_time,end_time);
            //0.5s leniency, dot is 1s, dash is 3s
            if((DOT_TIME - LENIENCY < press_length) && (press_length < DOT_TIME + LENIENCY))
            {
                //add dot to letter
                letter[letter_index] = '.';
                letter_index++;
            }
            else if((DASH_TIME - LENIENCY < press_length) && (press_length < DASH_TIME + LENIENCY))
            {
                //add dash to letter
                letter[letter_index] = '-';
                letter_index++;
            }
            break;
        case 3: //send button just pressed
            //append last letter to message
            message[message_index] = decode(letter,(sizeof(letter)/sizeof(letter[0])));
            //transmit
            uart_putc(UART_ID,'c');
            //reset letter/message
            memset(message,0,sizeof(message));
            message_index = 0;
            memset(letter,0,sizeof(letter));
            letter_index = 0;
            break;
    }
}
int decode(char *let, size_t let_len)
{
    for(size_t i=0;i<27;i++) //iterate through all 26 letters in morse list
    {
        if(!strncmp(let,morse_table[i],let_len)) return alphabet[i]; //if strings match, return
    }
}