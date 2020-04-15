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

#include <pthread.h>

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

pthread_mutex_t retrying_mutex =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t serial_mutex =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inital_write_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t inital_load_cond  =  
               PTHREAD_COND_INITIALIZER;
pthread_cond_t write_cond  =  
PTHREAD_COND_INITIALIZER;

pthread_cond_t retry_cond = PTHREAD_COND_INITIALIZER;

int retrying =  0;

void * mainThreadFunc(void * serialP){
	//initialize socket 
	int server_fd;
	int new_socket = nodeSocket(server_fd);

	int serial_port = *(int *)serialP;

	//Connect to MySQL Database
	MYSQL mysql;
	mysqlConnect(mysql);
	
	//Query for machine names to use with our JsonString
	vector<string> machines;
	vector<string> machine_table_name;		
	if(!(mysqlQuery(mysql, machines, "name"))){
		cout<<"Query to MySQL did not successfully run"<<endl;
	}

	//Get mode_variables 
	vector< vector<string> > mode_variables;
	if(!(mysqlQueryFixed(mysql, mode_variables))){
		cout<<"Query to MySQL did not successfully get mode_variables, default variables applied"<<endl;
	}

	//Set Start time for Main Read Timer
	//std::chrono::steady_clock::time_point readTime1 = std::chrono::steady_clock::now();

	//Set Start time for Timers
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	
	ModeHandler modeHandler(mode_variables[0][1] == "timer_mode2_wait" ? stof(mode_variables[0][0]) : float(120),
							mode_variables[1][1] == "timer_mode4_wait" ? stof(mode_variables[1][0]) : float(10),
							mode_variables[2][1] == "timer_motor_relay" ? stof(mode_variables[2][0]) : float(30),
							mode_variables[3][1] == "timer_start_relay" ? stof(mode_variables[3][0]) : float(2),
							mode_variables[4][1] == "timer_stop_relay" ? stof(mode_variables[4][0]) : float(2),
							mode_variables[5][1] == "timer_shut_down_counter" ? stof(mode_variables[5][0]) : float(30),
							mode_variables[6][1] == "timer_bleed_relay_m45" ? stof(mode_variables[6][0]) : float(5),
							mode_variables[7][1] == "timer_bleed_relay_m1" ? stof(mode_variables[7][0]) : float(2),
							mode_variables[8][1] == "max_high_pressure" ? stoi(mode_variables[8][0]) : 400,
							mode_variables[9][1] == "high_pressure_thres" ? stoi(mode_variables[9][0]) : 350,
							mode_variables[10][1] == "max_low_pressure" ? stoi(mode_variables[10][0]) : 94,
							mode_variables[11][1] == "low_pressure_thres" ? stoi(mode_variables[11][0]) : 86,
							mode_variables[12][1] == "min_low_pressure" ? stoi(mode_variables[12][0]) : 60,
							mode_variables[13][1] == "shut_down_counter" ? stoi(mode_variables[13][0]) : 2); 
	ModeHandler * mh =&modeHandler;
	
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

	//Allocate memory for ui_buf buffer
	char ui_buf[4];
	memset(&ui_buf, '\0', sizeof(ui_buf));

	// //Allocate memory for write_poll_buf buffer
	// char write_poll_buf[4];
	// memset(&write_poll_buf, '\0', sizeof(write_poll_buf));

	//Allocate memory for write_buf command buffer
	char write_buf[29];
	memset(&write_buf, '\0', sizeof(write_buf));

	//ModeHandler modeHandler;
	//Pressure Values
	int low_pressure , high_pressure = 0;


	//numReads: num of reads from port
	//n: num of iterations to read exact num of bits | 0 means nothing read this iteration, > 0 means something has been read 
    int numReads = 0, numIterations = 0, writeResponse = 0;
	int totalReadChars = 0;

	cout<<"THIS IS BEFORE WHILE LOOP"<<endl;
	pthread_cond_signal(&inital_load_cond); 
	int read_count=0;
    while(true){
		read_count++;

			pthread_mutex_lock(&read_lock); 
			pthread_cond_wait(&write_cond, &read_lock);

			pthread_mutex_lock(&serial_mutex);
				//cout<<dec<<read_count<<" Reading..."<<endl;
				totalReadChars = read_bytes(read_buf, serial_port, numIterations);
				//cout<<dec<<read_count<<" Done Reading..."<<endl;
			pthread_mutex_unlock(&serial_mutex);
			

		

		if(numIterations > 0 && totalReadChars > 0){ //if something was successfully polled and read from USB, do stuff with this data
			//cout<<"If poll&read ----"<<"WriteResponse: "<<writeResponse<<"  |  ReadResponse: "<<numIterations<<endl;
			
			
				
			
			

			//Handle Time Updates
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			t1 = std::chrono::steady_clock::now();
			(*mh).updateTimer(time_span.count());


			//Take read data and grab both pressure values
			getPressuresLH(read_buf, low_pressure, high_pressure);
			
			//Call Update to Mode Object
			(*mh).updateMode(low_pressure, high_pressure);
			
			//Edit write_buf with relay_p pointer to array
			editWriteBuf(write_buf, (*mh));

			pthread_mutex_lock(&serial_mutex);
			//Write to Serial Port to Update Relays
			//cout<<"Write Bytes to board...."<<endl;
			write_bytes(serial_port, write_buf);
			//cout<<"Done Write Bytes to board...."<<endl;
			pthread_mutex_unlock(&serial_mutex);
			
			const string tmp2 = createJsonDataString(read_buf, low_pressure, high_pressure, (*mh));
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
					(*mh).setStop();					
					
				}
				if(ui_buf[0]=='0' && ui_buf[1]=='6'){
					(*mh).setStart();					
				}
				if(ui_buf[0]=='9' && ui_buf[1]=='9'){
					//Refetch mode_variables
					if(!(mysqlQueryFixed(mysql, mode_variables))){
						cout<<"Query to MySQL did not successfully get mode_variables, default variables applied"<<endl;
					}
					//create new object 
					ModeHandler modeHandlerNew(mode_variables[0][1] == "timer_mode2_wait" ? stof(mode_variables[0][0]) : float(120),
							mode_variables[1][1] == "timer_mode4_wait" ? stof(mode_variables[1][0]) : float(10),
							mode_variables[2][1] == "timer_motor_relay" ? stof(mode_variables[2][0]) : float(30),
							mode_variables[3][1] == "timer_start_relay" ? stof(mode_variables[3][0]) : float(2),
							mode_variables[4][1] == "timer_stop_relay" ? stof(mode_variables[4][0]) : float(2),
							mode_variables[5][1] == "timer_shut_down_counter" ? stof(mode_variables[5][0]) : float(30),
							mode_variables[6][1] == "timer_bleed_relay_m45" ? stof(mode_variables[6][0]) : float(5),
							mode_variables[7][1] == "timer_bleed_relay_m1" ? stof(mode_variables[7][0]) : float(2),
							mode_variables[8][1] == "max_high_pressure" ? stoi(mode_variables[8][0]) : 400,
							mode_variables[9][1] == "high_pressure_thres" ? stoi(mode_variables[9][0]) : 350,
							mode_variables[10][1] == "max_low_pressure" ? stoi(mode_variables[10][0]) : 94,
							mode_variables[11][1] == "low_pressure_thres" ? stoi(mode_variables[11][0]) : 86,
							mode_variables[12][1] == "min_low_pressure" ? stoi(mode_variables[12][0]) : 60,
							mode_variables[13][1] == "shut_down_counter" ? stoi(mode_variables[13][0]) : 2); 
					mh =&modeHandlerNew;
					cout<<"Mode Variables Changed"<<endl;
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

			//Reset writeResponse & numIterations
			writeResponse =0;
			numIterations = 0;
			
		}

		if( writeResponse == -1 || totalReadChars == -1 ){//if the usb port been disconnected
			

			pthread_mutex_lock(&serial_mutex);
			if(serial_port > 0){
				close(serial_port);
			}
			while(usb_port(serial_port) <=0){
				//Send error to UI
				pthread_mutex_lock(&retrying_mutex);
				retrying = 1;
				//usleep(10000000);
				pthread_mutex_unlock(&retrying_mutex);

				string error_json = createJsonString("{error: 1}");
				char const * stringified_error_json = error_json.c_str();
				int error_size = strlen(stringified_error_json);
				sendNodeSocket(new_socket, stringified_error_json , error_size);
				cout<<"Attempting to reconnect to Port in 5 seconds..."<<endl;
				usleep(5000000);
			}
			//while(retrying == 1)
			//if(usb_port(serial_port) > 0){
				pthread_mutex_lock(&retrying_mutex);
				retrying = 0;
				//pthread_cond_signal(&retry_cond);
				pthread_mutex_unlock(&retrying_mutex);
				numReads = 0;
				cout<<"Successfully reconnecting to Port"<<endl;
				writeResponse = 0;
				totalReadChars = 0;
				//t1 = std::chrono::steady_clock::now();
				//readTime1 = std::chrono::steady_clock::now();
				//Set stop on reconnect
				//(*mh).setStop();
			//}else{	
				
			//}
			pthread_mutex_unlock(&serial_mutex);

			//pthread_mutex_unlock(&read_lock);
		}
        numReads++;
		pthread_mutex_unlock(&read_lock); 
    }

	mysqlCloseConnect(mysql);

	return NULL;
}

void * threadFunc(void * serialP){
	//Allocate memory for write_poll_buf buffer
	// std::cout << "Thread Function :: Start" << std::endl;
	// // Sleep for 2 seconds
	// sleep(2);
	// std::cout << "Thread Function :: End" << std::endl;
	
	int newSerial = *(int *)serialP;
	char write_poll_buf[4];
	memset(&write_poll_buf, '\0', sizeof(write_poll_buf));
	int writeResponse = 0;

	pthread_mutex_lock(&inital_write_lock); 
	pthread_cond_wait(&inital_load_cond, &inital_write_lock); 

	std::chrono::steady_clock::time_point readTime1 = std::chrono::steady_clock::now();

	editPollWriteBuf(write_poll_buf);
	int write_count =0;
	while(true){
		write_count++;
		std::chrono::steady_clock::time_point readTime2 = std::chrono::steady_clock::now();
		std::chrono::duration<double> read_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(readTime2 - readTime1);
		
		while(retrying == 1){
			pthread_cond_wait(&retry_cond, &inital_write_lock);
		}
		
		if(read_time_span.count() >= float(0.20) && read_time_span.count() <= float(.30)){
			readTime1 = std::chrono::steady_clock::now();
			//Poll for card
			
			//just added the missed reads section above instead of below
			//work on adding cond_wait here 
			
			pthread_mutex_lock(&serial_mutex);

				//cout<<write_count<<" Writing...@:"<<read_time_span.count() <<endl;
				writeResponse = write_poll_bytes(newSerial, write_poll_buf);
				//cout<<write_count<<"Done Writing..."<<endl;
				pthread_cond_signal(&write_cond);
			
			pthread_mutex_unlock(&serial_mutex);

			
		}else if(read_time_span.count() > float(.30)){
			readTime1 = std::chrono::steady_clock::now();
			pthread_cond_signal(&write_cond);
		}

		
	}
	pthread_mutex_unlock(&inital_write_lock);

	return NULL;
}

int main() {
	
	
 	
	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	

    //Set / read in settings for our Port
	usb_port(serial_port);

	
	pthread_t writeThread;
	pthread_t mainThread;

	
	pthread_create(&mainThread, NULL, mainThreadFunc, &serial_port);
	//usleep(50000000);
	pthread_create(&writeThread, NULL, threadFunc, &serial_port);

	while(1);
	//Clean up
	// err = pthread_join(threadId, NULL);
	// if (err)
	// 	std::cout << "Failed to join Thread : " << strerror(err) << std::endl;
	pthread_mutex_destroy(&retrying_mutex);
	pthread_mutex_destroy(&serial_mutex);
	pthread_mutex_destroy(&inital_write_lock);
	pthread_mutex_destroy(&read_lock);
	close(serial_port);


}

