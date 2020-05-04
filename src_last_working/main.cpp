//============================================================================
// Name        : com.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <jsoncpp/json/json.h>

//Functions File
#include "./functions.cpp"



// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/file.h>
#include <sys/socket.h>

#include <vector>
#include <iostream>
#include <iomanip>

#include <ctime>
#include <ratio>
#include <chrono>

// Socket header
#include <netinet/in.h> 

#include <mysql/mysql.h>
 
#define PORT 8081 //also defined in functions.cpp


using namespace std;

//BUFF_SIZE defined in functions.cpp

int main() {
	
	//initialize socket 
	int server_fd;
	int new_socket = nodeSocket(server_fd);
 	
	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

	//Allocate memory for ui_buf buffer
	char ui_buf[4];
	memset(&ui_buf, '\0', sizeof(ui_buf));

	//Allocate memory for write_buf buffer
	char write_buf[8];
	memset(&write_buf, '\0', sizeof(ui_buf));

    //Set / read in settings for our Port
	usb_port(serial_port);

	//Connect to MySQL Database
	MYSQL mysql;
	mysqlConnect(mysql);
	
	//Query for machine names to use with our JsonString
	vector<string> machines;
	vector<string> machine_table_name;		
	if(!(mysqlQuery(mysql, machines, "name"))){
		cout<<"Query to MySQL did not successfully run"<<endl;
	}


	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    


	ModeHandler modeHandler; 

	//Pressure Values
	int low_pressure , high_pressure = 0;


	
	//numReads: num of reads from port
	//n: num of iterations to read exact num of bits | 0 means nothing read this iteration, > 0 means something has been read 
    int numReads = 0, numIterations = 0, missed_reads = 0;

    while(true){


		
		//read_bytes() might miss once or twice every once in a while, if it misses more than 10 times in a row,
		//    the port might have been disconnected, so we will check for reconnection
		if(missed_reads > 10){ //if the usb port been disconnected
			if(serial_port > 0){
				close(serial_port);
			}
			
			if(usb_port(serial_port) > 0){
				numReads = 0;
				cout<<"Successfully reconnecting to Port"<<endl;
				missed_reads = 0;
			}else{	
				string error_json = createJsonString("{error: 1}");
				char const * stringified_error_json = error_json.c_str();
				int error_size = strlen(stringified_error_json);
				sendNodeSocket(new_socket, stringified_error_json , error_size);
				cout<<"Attempting to reconnect to Port in 5 seconds..."<<endl;
				usleep(5000000);
			}
				
		}else{ // if usb not disconnected, read
			read_bytes(read_buf, serial_port, numIterations);
		}
	    
		if(numIterations > 0){ //if something was read from USB, do stuff with this data
			missed_reads = 0;

			int current_mode = modeHandler.getCurrentMode();
			//Take read data and grab both pressure values
			getPressuresLH(read_buf, low_pressure, high_pressure);
			
			//Handle Time Updates
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			t1 = std::chrono::steady_clock::now();

			modeHandler.updateMode(low_pressure, high_pressure, time_span.count());
			
			//Edit write_buf with relay_p pointer to array
			editWriteBuf(write_buf, modeHandler);

			//Write to Serial Port to Update Relays
			write_bytes(serial_port, write_buf);

			//print_buf(read_buf, numIterations, numReads); 
			
			const string tmp2 = createJsonDataString(read_buf, low_pressure, high_pressure,current_mode, modeHandler);
			//convert string to char array
			char const * stringified_json = tmp2.c_str();
			int size = strlen(stringified_json);

			//make sure client is still connected to socket
			int stillAlive = readNodeSocket(new_socket, ui_buf);

			//read from node js socket here
			//sterilize string here
			//write to port here

			if(stillAlive > 0){
				sendNodeSocket(new_socket, stringified_json, size);
				//cout<<" ------ " << stillAlive<<" ------------" <<endl;
				if(ui_buf[0]=='0' && ui_buf[1]=='5'){
					modeHandler.setStop();					
					
				}
				if(ui_buf[0]=='0' && ui_buf[1]=='6'){
					modeHandler.setStart();					
					
				}
			}else{
				//create 'new' socket (should resuse old one) and wait for client to reconnect until timeout
				cout<<"Client disconnected; Waiting for reconnect."<<endl;
	
				if(new_socket != -1)
					close(new_socket);
				if(server_fd != -1)
					close(server_fd);

				server_fd = -1;
				new_socket = nodeSocket(server_fd);
				
			}
		}else{
			missed_reads++;			
		}
        numReads++;
    }
	
	//Clean up
	mysqlCloseConnect(mysql);
	close(serial_port);


}

