#ifndef MODEHANDLER_HPP
#define MODEHANDLER_HPP


// Fraction class
class ModeHandler {

  // private: no access from outside
  private:
    int current_mode;
    int pressure_high;
    int saved_pressure_high;
    int pressure_low;
    bool compressor;
    
    //
    bool start_switch;

    bool relay_start;
    bool relay_stop;
    bool relay_bleed;
    bool relay_motor;
    bool relay_pump;
    bool relay_chiller;

    float timer_mode2_wait;
    float timer_mode4_wait;
    float timer_motor_relay;
    float timer_start_relay;
    float timer_stop_relay;
    float timer_shut_down_counter;
    float timer_bleed_relay;

    int shut_down_counter;

    //
    //provided settings
    float TIMER_MODE2_WAIT;
    float TIMER_MODE4_WAIT;
    float TIMER_MOTOR_RELAY;
    float TIMER_START_RELAY;
    float TIMER_STOP_RELAY;
    float TIMER_SHUT_DOWN_COUNTER;
    float TIMER_BLEED_RELAY_M45;
    float TIMER_BLEED_RELAY_M1;

    int MAX_HIGH_PRESSURE;
    int HIGH_PRESSURE_THRESHOLD;
    int MAX_LOW_PRESSURE;
    int LOW_PRESSURE_THRESHOLD;
    int MIN_LOW_PRESSURE;

    int SHUT_DOWN_COUNTER_MAX;
    //

  //public interface

  public:
    // default constructor
    ModeHandler();

    // constructor from int (denominator)
    ModeHandler( float a, float b, float c, float d, float e,
    float f, float g,
    float h, int i, int j, int k, int l, int m, int n);

    //Copy constructor
    ModeHandler( const ModeHandler &cp);

    //Copy assignment
    ModeHandler& operator=(const ModeHandler& cp);

    // update
    void updateMode(int, int, bool);

    void restartMH();

    //Timer 
    void changeTime(const boost::system::error_code& /*e*/);
    void updateTimer(float);
    

    //getters
    int getCurrentMode();
    int getLowPressure();
    int getHighPressure();
    bool getStartStopValue();
    bool getRelayStart();
    bool getRelayStop();
    bool getRelayBleed();
    bool getRelayMotor();
    bool getRelayPump();
    bool getRelayChiller();

    float getTimerMode2();
    float getTimerMode4();
    float getTimerMotorRelay();
    float getTimerStartRelay();
    float getTimerStopRelay();
    float getTimerShutDownCounter();
    float getTimerBleedRelay();

    //setters
    void setCurrentMode(int);
    void setStart();
    void setStop();

  

};

#endif    /* MODEHANDLER_HPP */