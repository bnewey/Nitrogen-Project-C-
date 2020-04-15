//============================================================================
// Name        : mode.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles mode related functions
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ctime>

#include "./mode.hpp"


using namespace std;

//Default Constructor
ModeHandler::ModeHandler(){

    //start with stop mode 1
    current_mode=0;
    pressure_high=0;
    saved_pressure_high=0;
    pressure_low=0;

    start_switch=0;

    relay_start=0;
    relay_stop=0;
    relay_bleed=0;
    relay_motor=0;
    relay_pump=0;
    relay_chiller=0;

    timer_mode2_wait =float(0);
    timer_mode4_wait =float(0);
    timer_motor_relay =float(0);
    timer_start_relay =float(0);
    timer_stop_relay =float(0);
    timer_shut_down_counter =float(0);
    timer_bleed_relay =float(0);

    shut_down_counter=0;

    //(un)provided settings
    TIMER_MODE2_WAIT =float(120);
    TIMER_MODE4_WAIT =float(10);
    TIMER_MOTOR_RELAY =float(30);
    TIMER_START_RELAY =float(2);
    TIMER_STOP_RELAY =float(2);
    TIMER_SHUT_DOWN_COUNTER =float(30);
    TIMER_BLEED_RELAY_M45 =float(5);
    TIMER_BLEED_RELAY_M1 =float(2);

    MAX_HIGH_PRESSURE = int(400);
    HIGH_PRESSURE_THRESHOLD = int(350);
    MAX_LOW_PRESSURE = int(94);
    LOW_PRESSURE_THRESHOLD = int(86);
    MIN_LOW_PRESSURE = int(60);

    SHUT_DOWN_COUNTER_MAX = int(2);
    //
}

// Constructor with int  current_mode passed
ModeHandler::ModeHandler( float a, float b, float c, float d, float e, float f, float g,
  float h, int i, int j, int k, int l, int m, int n) 
  : TIMER_MODE2_WAIT(a), TIMER_MODE4_WAIT(b), TIMER_MOTOR_RELAY(c), TIMER_START_RELAY(d), TIMER_STOP_RELAY(e),
    TIMER_SHUT_DOWN_COUNTER(f), TIMER_BLEED_RELAY_M45(g), TIMER_BLEED_RELAY_M1(h), 
    MAX_HIGH_PRESSURE(i), HIGH_PRESSURE_THRESHOLD(j), MAX_LOW_PRESSURE(k), 
    LOW_PRESSURE_THRESHOLD(l), MIN_LOW_PRESSURE(m), SHUT_DOWN_COUNTER_MAX(n)
  {
    //start with stop mode 1
    current_mode=0;
    pressure_high = 0;
    saved_pressure_high=0;
    pressure_low = 0;

    start_switch=0;

    relay_start=0;
    relay_stop=0;
    relay_bleed=0;
    relay_motor=0;
    relay_pump=0;
    relay_chiller=0;

    timer_mode2_wait =float(0);
    timer_mode4_wait =float(0);
    timer_motor_relay =float(0);
    timer_start_relay =float(0);
    timer_stop_relay =float(0);
    timer_shut_down_counter =float(0);
    timer_bleed_relay =float(0);

    shut_down_counter=0;
}



void ModeHandler::updateTimer(float seconds_passed){
    if(timer_mode2_wait > 0){
        timer_mode2_wait-=seconds_passed;
        if(timer_mode2_wait<=0){
            relay_start=1;
            timer_start_relay=float(TIMER_START_RELAY);
        }
    }

    if(timer_mode4_wait>0){
        timer_mode4_wait-=seconds_passed;
        if(timer_mode4_wait<=0){
            relay_pump=1;
        }
    }

    if(timer_motor_relay>0){
        timer_motor_relay-=seconds_passed;
    }

    if(timer_start_relay>0){
        timer_start_relay-=seconds_passed;
        if(timer_start_relay<=0){
            relay_start = 0;
        }
    }

    if(timer_stop_relay>0){
        timer_stop_relay-=seconds_passed;
        if(timer_stop_relay<=0){
            relay_stop = 0;
        }
    }

    if(timer_shut_down_counter>0){
        timer_shut_down_counter-=seconds_passed;
    }
    if(timer_bleed_relay>0){
        timer_bleed_relay-=seconds_passed;
        if(timer_bleed_relay<=0){
            relay_bleed = 0;
        }
    }
} 

void ModeHandler::updateMode(int pressure_l, int pressure_h){
    this->pressure_low = pressure_l;
    this->pressure_high = pressure_h;
    //  cout<<"seconds passed: "<<seconds_passed<<endl;
     // cout<<" pressure_l:"<<pressure_l<<"pressure_h:"<<pressure_h<<endl;
    // cout<<"Current Mode: "<<current_mode<<endl;
    //  cout<<"relay_start: "<<relay_start<<endl;
    //  cout<<"relay_stop : "<<relay_stop<<endl;
    //  cout<<"relay_bleed: "<<relay_bleed<<endl;
    //  cout<<"relay_motor: "<<relay_motor<<endl;
    //  cout<<"relay_pump : "<<relay_pump<<endl;
    //  cout<<"relay_chill: "<<relay_chiller<<endl;

    // cout<<"TIMER_MODE2_WAIT"<<TIMER_MODE2_WAIT<<endl;
    // cout<<"TIMER_MODE4_WAIT"<<TIMER_MODE4_WAIT<<endl;
    // cout<<"TIMER_MOTOR_RELAY"<<TIMER_MOTOR_RELAY<<endl;
    // cout<<"TIMER_START_RELAY"<<TIMER_START_RELAY<<endl;
    // cout<<"TIMER_STOP_RELAY"<<TIMER_STOP_RELAY<<endl;
    // cout<<"TIMER_SHUT_DOWN_COUNTER"<<TIMER_SHUT_DOWN_COUNTER<<endl;
    // cout<<"TIMER_BLEED_RELAY_M45"<<TIMER_BLEED_RELAY_M45<<endl;
    // cout<<"TIMER_BLEED_RELAY_M1"<<TIMER_BLEED_RELAY_M1<<endl;

    // cout<<"MAX_HIGH_PRESSURE"<<MAX_HIGH_PRESSURE<<endl;
    // cout<<"HIGH_PRESSURE_THRESHOLD"<<HIGH_PRESSURE_THRESHOLD<<endl;
    // cout<<"MAX_LOW_PRESSURE"<<MAX_LOW_PRESSURE<<endl;
    // cout<<"LOW_PRESSURE_THRESHOLD"<<LOW_PRESSURE_THRESHOLD<<endl;
    // cout<<"MIN_LOW_PRESSURE"<<MIN_LOW_PRESSURE<<endl;

    // cout<<"SHUT_DOWN_COUNTER_MAX"<<SHUT_DOWN_COUNTER_MAX<<endl;

    //MOVED TO MAIN
    //this->updateTimer(seconds_passed);

    //START REPLACING VARIABLES <----------------------------

    //Mode Logic
    if(start_switch == 1){
        this->current_mode = 0;
        if((pressure_l < MIN_LOW_PRESSURE || pressure_h < HIGH_PRESSURE_THRESHOLD) && relay_chiller ==0){
            this->current_mode = 2;
            //Relays
            this->relay_chiller = 1;
            //Timers
            this->timer_mode2_wait = float(TIMER_MODE2_WAIT); 
        }
        if(pressure_l > MAX_LOW_PRESSURE && relay_chiller == 1){
            this->current_mode = 3;
            //Relays
            this->relay_stop = 1;
            this->relay_chiller = 0;
            //Timers
            timer_stop_relay = float(TIMER_STOP_RELAY);
        }
        if(timer_mode2_wait <= 0 && pressure_h < HIGH_PRESSURE_THRESHOLD && pressure_l > LOW_PRESSURE_THRESHOLD && relay_motor == 0){
            this->current_mode = 4;
            //Relays
            this->relay_bleed = 1;
            this->relay_motor = 1;
            //Timers
            this->timer_bleed_relay = float(TIMER_BLEED_RELAY_M45);
            this->timer_mode4_wait = float(TIMER_MODE4_WAIT);
            this->timer_motor_relay = float(TIMER_MOTOR_RELAY);

            //saving the high pressure value
            this->saved_pressure_high = pressure_h;
        }
        if((pressure_h > MAX_HIGH_PRESSURE || pressure_l < MIN_LOW_PRESSURE) && relay_motor == 1){
            cout<<endl<<endl<<endl<<endl<<endl<<"Low Press@mode5: "<<pressure_l<<endl<<"High Press@mode5: "<<pressure_h<<endl;
            this->current_mode = 5;
            //Relays
            this->relay_bleed = 1;
            this->relay_motor = 0;
            this->relay_pump = 0;
            //Timers
            this->timer_bleed_relay = float(TIMER_BLEED_RELAY_M45);
            this->timer_mode4_wait = float(0);
            
        }
        if(timer_motor_relay <= 0 && timer_shut_down_counter <= 0 && relay_motor ==1){
            this->current_mode = 6;
            cout<<endl<<endl<<endl<<endl<<endl<<"MODE 6!!!"<<endl;
            cout<<" MODE : pressure_l:"<<pressure_l<<"pressure_h:"<<pressure_h<<endl;
            cout<<"Saved H_pressure: "<<saved_pressure_high<<endl;
            cout<<"ShutDown Counter: "<<shut_down_counter<<endl<<"Timer for counter"<<timer_shut_down_counter<<endl;
            if(pressure_h > this->saved_pressure_high){
                //Relays
                //Timers
                this->timer_motor_relay = float(TIMER_MOTOR_RELAY);
                //save the high pressure value
                this->saved_pressure_high = pressure_h;
                this->shut_down_counter = 0;
            }
            else{
                this->shut_down_counter++;
                if(this->shut_down_counter>=SHUT_DOWN_COUNTER_MAX){
                    this->current_mode = 1;
                    //Relays
                    this->relay_start = 0;
                    this->relay_stop = 1;
                    this->relay_bleed = 1;
                    this->relay_motor = 0;
                    this->relay_pump = 0;
                    this->relay_chiller = 0;
                    
                    this->shut_down_counter= 0;
                    //Timers
                    this->timer_mode2_wait = float(0);
                    this->timer_mode4_wait = float(0);
                    this->timer_stop_relay = float(TIMER_STOP_RELAY);
                    this->timer_bleed_relay = float(TIMER_BLEED_RELAY_M1);
                    this->start_switch = 0;
                }
                else{
                    this->timer_shut_down_counter = float(TIMER_SHUT_DOWN_COUNTER);
                }
            }
        }
    }
    else if(start_switch == 0 && current_mode == 1){  //STOPPED
        this->current_mode = 1;
        //Relays
        this->relay_start = 0;
        this->relay_stop = 1;
        this->relay_bleed = 1;
        this->relay_motor = 0;
        this->relay_pump = 0;
        this->relay_chiller = 0;

        this->shut_down_counter= 0;
        //Timers
        this->timer_mode2_wait = float(0);
        this->timer_mode4_wait = float(0);
        this->timer_stop_relay = float(TIMER_STOP_RELAY);
        this->timer_bleed_relay = float(TIMER_BLEED_RELAY_M1);

        this->current_mode = 0;
    }
    
}


//getters
int ModeHandler::getCurrentMode(){
    return current_mode;
}

bool ModeHandler::getStartStopValue(){
    return start_switch;
}

bool ModeHandler::getRelayStart(){
    return relay_start;
}

bool ModeHandler::getRelayStop(){
    return relay_stop;
}

bool ModeHandler::getRelayBleed(){
    return relay_bleed;
}

bool ModeHandler::getRelayMotor(){
    return relay_motor;
}

bool ModeHandler::getRelayPump(){
    return relay_pump;
}

bool ModeHandler::getRelayChiller(){
    return relay_chiller;
}

float ModeHandler::getTimerMode2(){ return timer_mode2_wait;}
float ModeHandler::getTimerMode4(){ return timer_mode4_wait;}
float ModeHandler::getTimerMotorRelay(){ return timer_motor_relay;}
float ModeHandler::getTimerStartRelay(){ return timer_start_relay;}
float ModeHandler::getTimerStopRelay(){ return timer_stop_relay;}
float ModeHandler::getTimerShutDownCounter(){ return timer_shut_down_counter;}
float ModeHandler::getTimerBleedRelay(){ return timer_bleed_relay;}

//setters
void ModeHandler::setCurrentMode(int new_mode){
    if(new_mode < 0 || new_mode > 5){
        return;
    }
    
    this->current_mode = new_mode;
}

void ModeHandler::setStart(){
    this->start_switch = 1;
}

void ModeHandler::setStop(){
    this->start_switch = 0;
    this->current_mode = 1;
}
