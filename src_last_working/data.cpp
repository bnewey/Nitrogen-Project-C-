//============================================================================
// Name        : data.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles data related functions
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>


#include "./data.hpp"


using namespace std;

//Default Constructor
DataHandler::DataHandler() : buff_size(12) {
   
}

// Constructor with int  current_mode passed
DataHandler::DataHandler(int buff_size) : buff_size(buff_size){
  

}




void DataHandler::updateData(char (&read_buf)[buff_size]){
   
  
    
}
