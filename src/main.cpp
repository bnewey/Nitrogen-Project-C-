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
#include <memory>
#include <iostream>
#include <iomanip>
#include <pthread.h>

#include <ctime>
#include <ratio>
#include <chrono>

// Socket header
#include <netinet/in.h> 

#include <mysql/mysql.h>
 
#define PORT 8081 //also defined in functions.cpp


using namespace std;

//BUFF_SIZE defined in functions.cpp
struct arg_struct {
    string start_stop;
	MYSQL mysql;
};
struct arg_struct args;

void * sendToDb(void * arguments){
	struct arg_struct *args1 = (struct arg_struct *)arguments;

	MYSQL mysql = (MYSQL) args1->mysql;
	string start_stop = args1->start_stop;

	//Lights sql
	if(!(mysqlQueryNoReturn(mysql, start_stop))){
		cout<<"Error sending start_stop to DB"<<endl;
	}


	return NULL;
}

pthread_mutex_t UI_args_mutex =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t retrying_mutex = PTHREAD_MUTEX_INITIALIZER;
bool retrying_Ui_reconnect = false;

struct arg_struct_UI {
    int new_socket;
	int server_fd;
};
struct arg_struct_UI args_UI;

void * reconnectUiInNewThread(void * arguments){
	struct arg_struct_UI *args1 = (struct arg_struct_UI *)arguments;

	pthread_mutex_lock(&UI_args_mutex);
	int server_fd = (int) args1->server_fd;
	int new_socket = (int) args1->new_socket;
	pthread_mutex_unlock(&UI_args_mutex);
	

	if(new_socket != -1)
			close(new_socket);
		if(server_fd != -1)
			close(server_fd);

		server_fd = -1;
	new_socket = nodeSocket(server_fd);

	while(new_socket == -1){
		if(new_socket != -1)
			close(new_socket);
		if(server_fd != -1)
			close(server_fd);

		server_fd = -1;
		new_socket = nodeSocket(server_fd);
	
		cout<<"Check ip addr to make sure IP ADDRESS is correct. New socket:"<<new_socket<<endl;
	}
	
	//Set our global struct
	pthread_mutex_lock(&UI_args_mutex);
	args_UI.server_fd = server_fd;
	args_UI.new_socket = new_socket;
	pthread_mutex_unlock(&UI_args_mutex);

	pthread_mutex_lock(&retrying_mutex);
	retrying_Ui_reconnect = false;
	pthread_mutex_unlock(&retrying_mutex);

	return NULL;
}

int main() {
	
	//initialize socket 
	// values are held in struct args_UI above
	args_UI.new_socket = nodeSocket(args_UI.server_fd);
 	
	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

	//Allocate memory for ui_buf buffer
	char ui_buf[4];
	memset(&ui_buf, '\0', sizeof(ui_buf));

	//Allocate memory for write_buf command buffer
	char write_buf[102];
	memset(&write_buf, '\0', sizeof(write_buf));

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

	//Get mode_variables 
	vector< vector<string> > mode_variables;
	if(!(mysqlQueryFixed(mysql, mode_variables))){
		cout<<"Query to MySQL did not successfully get mode_variables, default variables applied"<<endl;
	}

	//Set Start time for Timers
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	//Set Start Time for DB update
	std::chrono::steady_clock::time_point db_timer1 = std::chrono::steady_clock::now();
	
	//ModeHandler
	shared_ptr<ModeHandler> mh(make_shared<ModeHandler>(
			mode_variables[0][1] == "timer_mode2_wait" ? stof(mode_variables[0][0]) : float(120),
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
			mode_variables[13][1] == "shut_down_counter" ? stoi(mode_variables[13][0]) : 2,
			mode_variables[14][1] == "start_stop" ? stoi(mode_variables[14][0]) : 0));

	//Pressure Values
	int low_pressure , high_pressure = 0;
	bool compressor = 0;


	//numReads: num of reads from port
	//n: num of iterations to read exact num of bits | 0 means nothing read this iteration, > 0 means something has been read 
    long numReads = 0;
	long numJsonSends = 0;
	int numIterations = 0;
	int totalReadChars = 0;
	int missedReads =0;

    while(true){
		

		//    the port might have been disconnected, so we will check for reconnection
		if( missedReads > 10){//if the usb port been disconnected
			if(serial_port > 0){
				close(serial_port);
			}

			///
			//python test to reset USB 
			system("python /opt/nitrogen/reset_usb.py pathpci /sys/bus/pci/drivers/xhci_hcd/0000:00:14.0");
			usleep(5000000);

			///
			if(usb_port(serial_port) > 0){
				numReads = 0;
				cout<<"Successfully reconnecting to Port"<<endl;
				missedReads=0;
				totalReadChars = 0;
				numIterations = 0;
				t1 = std::chrono::steady_clock::now();
				//Restart on reconnect
				//stops mh, then updates itsef, then starts
				if((*mh).getStartStopValue()){
					(*mh).restartMH();
					cout<<"RESTARTING MH"<<endl;
				}else{
					(*mh).setStop();
					cout<<"STOPPED AFTER CONNECT"<<endl;
				}
				
			}else{	
				//Send error to UI
				string error_json = createJsonString("{error: 1}");
				char const * stringified_error_json = error_json.c_str();
				int error_size = strlen(stringified_error_json);
				pthread_mutex_lock(&UI_args_mutex);
					sendNodeSocket(args_UI.new_socket, stringified_error_json , error_size);
				pthread_mutex_unlock(&UI_args_mutex);
				cout<<"Attempting to reconnect to Port in 5 seconds..."<<endl;
				usleep(5000000);
			}
				
		}else{ // if usb not disconnected, poll & read
			totalReadChars = read_bytes(read_buf, serial_port, numIterations);	
			cout<<"TotalReadChars: "<<totalReadChars<<endl;
		}

		if( numIterations > 0 && totalReadChars > 0){ //if something was successfully polled and read from USB, do stuff with this data
			//cout<<"If poll&read ----"<<"WriteResponse: "<<writeResponse<<"  |  ReadResponse: "<<numIterations<<endl;
			//print_buf(read_buf, numIterations ,numReads);
			cout<<"READ GOOD STUFF: "<<endl;
			missedReads = 0;
			
			//Handle Time Updates
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			t1 = std::chrono::steady_clock::now();
			(*mh).updateTimer(time_span.count());

			//Take read data and grab both pressure values
			getPressuresLH(read_buf, low_pressure, high_pressure, compressor);
		

			//Call Update to Mode Object
			(*mh).updateMode(low_pressure, high_pressure, compressor);
			
			//Edit write_buf with relay_p pointer to array
			editWriteBuf(write_buf, mh);

			print_write_buff(write_buf, 3 ,3 );

			//Write to Serial Port to Update Relays
			write_bytes(serial_port, write_buf);

			
			numJsonSends++;
			const string tmp2 = createJsonDataString(read_buf, low_pressure, high_pressure, compressor,  mh, numJsonSends);
			//convert string to char array
			char const * stringified_json = tmp2.c_str();
			int size = strlen(stringified_json);

			//make sure client is still connected to socket
			pthread_mutex_lock(&UI_args_mutex);
			int stillAlive = readNodeSocket(args_UI.new_socket, ui_buf);
			pthread_mutex_unlock(&UI_args_mutex);

			//read from node js socket here
			//sterilize string here
			//write to port here

			if(stillAlive > 0  && !retrying_Ui_reconnect){
				//Send out data to NodeJS
				pthread_mutex_lock(&UI_args_mutex);
				sendNodeSocket(args_UI.new_socket, stringified_json, size);
				pthread_mutex_unlock(&UI_args_mutex);

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

					//Reset will destruct old pointers
					mh.reset();
					//create new object and assign to mh shared_ptr
					shared_ptr<ModeHandler> mh_tmp(make_shared<ModeHandler>(mode_variables[0][1] == "timer_mode2_wait" ? stof(mode_variables[0][0]) : float(120),
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
							mode_variables[13][1] == "shut_down_counter" ? stoi(mode_variables[13][0]) : 2,
							mode_variables[14][1] == "start_stop" ? stoi(mode_variables[14][0]) : 0));
					//Set it to our existing name
					mh = mh_tmp;
					cout<<"Mode Variables Changed"<<endl;
				}
			}else{
				//create 'new' socket (should resuse old one) and wait for client to reconnect until timeout
				cout<<"Client disconnected; Waiting for reconnect."<<endl;

				//(*mh).setStop();
				//cout<<"Hitting the STOP button because we lost control of UI..."<<endl;

				if(!retrying_Ui_reconnect){
					cout<<"Trying reconnect in new thread."<<endl;
					pthread_mutex_lock(&retrying_mutex);
					retrying_Ui_reconnect = true;
					pthread_mutex_unlock(&retrying_mutex);

					//Start new thread to try to connect 
					pthread_t ui_thread;

					if (pthread_create(&ui_thread, NULL, &reconnectUiInNewThread, (void *)&args_UI) != 0) {
						printf("Uh-oh! Reconnect UI Thread Failed \n");
						return -1;
					}
					//Detatch, thread will update retrying_Ui_reconnect bool when it finishes
					pthread_detach(ui_thread);	

				}
			}

			//Send Update to DB ever 30 seconds
			std::chrono::steady_clock::time_point db_timer2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> db_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(db_timer2 - db_timer1);
			
			if( db_time_span.count() > 5){

				pthread_t db_thread;

				bool tmp_ss = (*mh).getStartStopValue();
				string tmp_string = "UPDATE mode_variables SET start_stop = ";
				tmp_string +=  tmp_ss ? to_string(1) : to_string(0);

				args.start_stop = tmp_string;

				args.mysql = mysql;

				if (pthread_create(&db_thread, NULL, &sendToDb, (void *)&args) != 0) {
					printf("Uh-oh!\n");
					return -1;
				}
				cout<<"Writing to DB"<<endl;
				db_timer1 = std::chrono::steady_clock::now();
				pthread_detach(db_thread);
			}

			//Reset writeResponse & numIterations
			numIterations = 0;
		}else{
			missedReads++;
		}
        numReads++;
    }
	
	//Clean up
	mysqlCloseConnect(mysql);
	close(serial_port);


}

