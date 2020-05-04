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


  //public interface

  public:
    // default constructor
    ModeHandler();

    // constructor from int (denominator)
    ModeHandler(int, int);

    // update
    void updateMode(int, int, float);

    //Timer 
    void changeTime(const boost::system::error_code& /*e*/);
    void updateTimer(float);
    

    //getters
    int getCurrentMode();
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