
#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP


// Fraction class
class DataHandler {

  // private: no access from outside
  private:
    const int buff_size;


  //public interface

  public:
    // default constructor
    DataHandler();

    // constructor from int (denominator)
    DataHandler(int);

    // update
    void updateData();


    //Getters
    //setters

};

#endif    /* DATAHANDLER_HPP */