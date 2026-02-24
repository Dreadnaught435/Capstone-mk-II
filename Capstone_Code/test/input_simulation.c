/*
Benjamin Mock
Created 29 November 2025

This is test code to help simulate button pressed and their responses on a PC
*/
//any updates after 2/24/26 are by atticus foster


#include "input_simulation.h"

volatile uint32_t millis = 0;

//Raw states from ISR
volatile uint8_t raw_b1 = 1;
volatile uint8_t raw_b2 = 1;

//Debouncing flag
volatile uint8_t deb_flag = 0;

//Timestamp used for debounce filtering
volatile uint32_t last_change_b1 = 0;
volatile uint32_t last_change_b2 = 0;

//For measuring duration of Button 2
uint32_t b2_prev_timestamp = 0;
uint32_t b2_current_timestamp = 0;
uint32_t b2_press_time = 0;//in ms


//Tracker of what button is currently pressed(0=None)
uint8_t button_pressed=0;

//Button reading and software debounce
void read_buttons() {

    //For simulation, now is 25 ms ahead of last_change_b2, prev_timestamp will be 0
    uint32_t now = last_change_b2+25;
    b2_prev_timestamp = 0;



    if(deb_flag) {//Button press detected
        if(last_change_b1 > last_change_b2) {   //Debound button 1
            if ((now - last_change_b1) >= DEBOUNCE_MS) {
                deb_flag = 0;
                button_pressed = button_pressed%1;
            }
        }
        else {  //Debound button 2
            if ((now - last_change_b2) >= DEBOUNCE_MS) {
                deb_flag = 0;
                b2_press_time = last_change_b2-b2_prev_timestamp;
                button_pressed = (button_pressed == 0) ? 2 : 0;
            }
        }
    }
}