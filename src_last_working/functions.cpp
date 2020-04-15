//============================================================================
// Name        : functions.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : All basic functions for port reading / mysql / json sending to UI
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <jsoncpp/json/json.h>


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

// Socket header
#include <netinet/in.h> 

#include <mysql/mysql.h>

#include "./mode.cpp"
 
#define PORT 8081 


using namespace std;

const unsigned short BUFF_SIZE = 12;

void print_buf(char (&read_buf)[BUFF_SIZE], int numIterations, int numReads){
	cout<< numReads << ": "<<endl<<"Print iterations: "<< numIterations<<endl;

	for (int i = 0; i < BUFF_SIZE; ++i)
		cout <<  dec << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[i])) << hex << " ";

	cout <<  endl<<endl;
}

void read_bytes(char  (&read_buf)[BUFF_SIZE],int & serial_port, int & numIterations) {
		/* reading above 255 is tricky, we need to read BUFFSIZE bytes exactly, so this ensures it */
		int totalNeeded = BUFF_SIZE;
		int remaining   = sizeof(read_buf);
		numIterations=0;
		while (remaining > 0){
			try{
				ssize_t readChars = read(serial_port, &read_buf[totalNeeded - remaining], remaining);
				if (!(readChars > 0)){
					return;
				}
				else{
					remaining -= readChars;
					numIterations++;
				}
			} catch(ssize_t readChars){
				cout<<"Read exception caught"<<endl;
			}	
		}
}

void getPressuresLH(char  (&read_buf)[BUFF_SIZE], int & low_pressure, int & high_pressure){
	// Convert both the integers to string 
	int pressure_l = ((int)(*(unsigned char*)(&read_buf[1]))<<8) | ((int)(*(unsigned char*)(&read_buf[2])));
    int pressure_h = ((int)(*(unsigned char*)(&read_buf[3]))<<8) | ((int)(*(unsigned char*)(&read_buf[4])));
	//cout<<"GET BEFORE: low_pressure:"<<pressure_l<<"     high_pressure:"<<pressure_h<<endl;
	low_pressure = pressure_l;
	high_pressure = pressure_h;
    
}

void editWriteBuf(char (&temp)[8], ModeHandler mh){
	temp[0] = '\002';
	temp[1] = (mh.getRelayStart() ? '\001' : '\000');
	temp[2] = (mh.getRelayStop() ? '\001' : '\000');
	temp[3] = (mh.getRelayBleed() ? '\001' : '\000');
	temp[4] = (mh.getRelayMotor() ? '\001' : '\000');
	temp[5] = (mh.getRelayPump() ? '\001' : '\000');
	temp[6] = (mh.getRelayChiller() ? '\001' : '\000');
	temp[7] = '\003';
}

void write_bytes(int & serial_port, char (&temp)[8]){
	write(serial_port, temp, 8);//sizeof(temp));
}

int usb_port(int & serial_port) {
	//TODO: try /dev/ttyUSB* instead of 0, bc linux might overwrite USB0 
	serial_port = open("/dev/ttyUSB0", O_RDWR );
	//Check for errors
	if (serial_port < 0) {
	    printf("Error %i from open: %s\n", errno, strerror(errno));	
		return(serial_port);	
	}
	else{
		// Create new termios struc, we call it 'tty' for convention
		struct termios tty;
		memset(&tty, 0, sizeof tty);

		// Read in existing settings, and handle any error
		if(tcgetattr(serial_port, &tty) != 0) {
		//	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		}

		tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
		tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
		tty.c_cflag |= CS8; // 8 bits per byte (most common)

		//"Enabling this when it should be disabled can result in your serial port receiving no data"
		tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)

		tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

		tty.c_lflag &= ~ICANON; //Canonical mode is disabled

		//If this bit is set, sent characters will be echoed back.
		tty.c_lflag &= ~ECHO; // Disable echo
		tty.c_lflag &= ~ECHOE; // Disable erasure
		tty.c_lflag &= ~ECHONL; // Disable new-line echo

		tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP (We don’t want this with a serial port)

		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control

		tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes, we just want raw data

		tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
		tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
		// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
		// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

		//tty.c_cc[VTIME] = 2;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		//tty.c_cc[VMIN] = 0; //wait for BUFF_SIZE bytes to fill then return
		tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		tty.c_cc[VMIN] = 0; //wait for BUFF_SIZE bytes to fill then return

		// Set in/out baud rate to be 9600
		// If you have no idea what the baud rate is and you are trying to communicate with a 3rd party system,
		//   try B9600, then B57600 and then B115200 as they are the most common rates.
		cfsetispeed(&tty, B57600);
		cfsetospeed(&tty, B57600);

		// Save tty settings, also checking for error
		if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		//	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		}
	
		//prevent other processes from reading/writing to the serial port at the same time you are.
    	if(flock(serial_port, LOCK_EX | LOCK_NB) == -1) {
			throw std::runtime_error("Serial port with file descriptor " +
				std::to_string(serial_port) + " is already locked by another process.");
		}
		return(serial_port);
	}
}

void mysqlConnect(MYSQL & mysql){

	//Connect to MySQL server
	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"nitrogen");
	printf("MYSQL INFO: %s\n", mysql_get_client_info());
	if (!mysql_real_connect(&mysql,"127.0.0.1","root","!Baseball15","db_nitro",0,NULL,0))
	{
		fprintf(stderr, "Failed to connect to database: Error: %s\n",
			mysql_error(&mysql));
		return;
	}
	cout<<"Successfully Connected to MYSQL"<<endl;
	return;
	
}

int mysqlQuery(MYSQL & mysql, vector<string> & machines, const char * field_name){
	//TODO: Are mutliple return point good or bad practice???
	//TODO: Select machine_table_name instead of *
	unsigned int num_fields;
	MYSQL_ROW row;

	if(mysql_query(&mysql, "SELECT * from machines")){ 
		cout<<"MySQL Query Error"<<endl;
		return 0;
	}
	MYSQL_RES *result  = mysql_store_result(&mysql);
	if(!result){
		fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
		return 0;
	}

	//Find index of field_name and field_table_name
	num_fields = mysql_num_fields(result); 
	MYSQL_FIELD *fields;
	fields = mysql_fetch_fields(result);
	
	unsigned int field_name_index;
	bool field_name_index_found = false;
	for(unsigned int i = 0; i < num_fields; i++){
		if(!(strcmp(fields[i].name , field_name))){
			field_name_index = i;
			field_name_index_found = true;
		}
	}	

	if(!field_name_index_found){
		cout<<" Warning on mysql query: Bad field name in mySqlQuery()"<<endl;
	}

	//Search through rows
	while ((row = mysql_fetch_row(result)))	{
		for(unsigned int p = 0; p < num_fields; p++) {	
			//if field matches our field_name or field_table_name from above, record that cell
			if(p == field_name_index){
				string t = row[p];
				machines.push_back(t);
			}
		}
	}

	// //Print Vector
	 for (std::vector<string>::const_iterator i = machines.begin(); i != machines.end(); ++i)
		std::cout << *i << ' ';

	mysql_free_result(result);
	return 1;
	
	
}

void mysqlCloseConnect(MYSQL &mysql){
	mysql_close(&mysql);
}

int readNodeSocket( int & new_socket, char  (&ui_buf)[4] ){
	int valread;
	ui_buf[0] = '\0';
	//valread = read( new_socket , buffer, 1024);
	valread = recv( new_socket, ui_buf,4, 0);

    return(valread);
}

void sendNodeSocket(int & new_socket, char const * data, const unsigned short DATA_SIZE){
	try{
    	send(new_socket , data , DATA_SIZE , 0 ); 
		//printf("Data message sent from C++\n"); 
	}
	catch (int e){
		printf("*** could not send data ***");
	}
}

int nodeSocket(int & server_fd){
	int new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        cout<<"*** Failed to create socket ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 
    cout<< "Created Socket "<<endl;
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        cout<<"*** Failed to run setsockopt() ***"<<endl; 
        exit(EXIT_FAILURE); 
    }
	
	// set a timeout for client to connect / read from 
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)){
		
	}

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    
    // Forcefully attaching socket to the port 8080 
    int error_num = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(error_num < 0 ){ 
       cout<<"*** Bind Failed ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 	
	if (listen(server_fd, 3) < 0) 
    { 
        cout<<"*** listen ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 
	// socket blocks program on accept() until either a client accepts or timeout occurs
    int error_num2 = new_socket = accept(server_fd, (struct sockaddr *)&address,   (socklen_t*)&addrlen);
    if(error_num2 < 0){ 
        cout<<"*** No Client Accepted Connection *** - "<<errno<<endl; 
		return -1;
    } 

	return new_socket;
}

string createJsonDataString(char  (&read_buf)[BUFF_SIZE], int pressure_low, int pressure_high, int current_mode, ModeHandler mh){
	int temp=0;
	float timer_temp=0.00;
	vector<Json::Value> arrayVehicles;
	Json::Value root;
	Json::Value myJson = root["nitrogenData"];

	myJson["pressure_low"] = Json::Value::Int(pressure_low);
	myJson["pressure_high"] = Json::Value::Int(pressure_high);
	
	//pressure_low and pressure_high are 2 bits  
	string data_string_template[6] = {"relay_start", "relay_stop", "relay_bleed","relay_motor", "relay_pump", "relay_chiller"};

	for(int p= 0;p < 6; p++){
		stringstream ss;
		ss.clear();
		ss << hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[p+5])); //offset by 5 to get to the relays readbuf[5-11]
		ss >> temp;
		myJson[data_string_template[p]] = Json::Value::Int(temp);
	}

	//Timers
	timer_temp = mh.getTimerMode2();
	myJson["timer_mode2"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerMode4();
	myJson["timer_mode4"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerStartRelay();
	myJson["timer_start_relay"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerStopRelay();
	myJson["timer_stop_relay"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerBleedRelay();
	myJson["timer_bleed_relay"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerMotorRelay();
	myJson["timer_motor_relay"] = Json::Value::Int(timer_temp);
	timer_temp = mh.getTimerShutDownCounter();
	myJson["timer_shut_down_counter"] = Json::Value::Int(timer_temp);


	myJson["current_mode"] = Json::Value::Int(current_mode);

	arrayVehicles.push_back(myJson);

	Json::FastWriter fastWriter;
	string output = "{ \"nitrogenData\": ";
	for(int i=0; i<1; i++){
		if(i != 0)
			output += ",";
		output += fastWriter.write(arrayVehicles[i]);
	}
	output += " }";
	
	//cout<<"OUTPUT"<<output<<endl;
	return(output);
}

string createJsonString(string message){
	Json::Value root;
	Json::Value myJson = root["nitrogenData"];

	myJson["error"] = Json::Value::Int(1);

	Json::FastWriter fastWriter;
	string output = "{ \"nitrogenData\": ";
	output += fastWriter.write(myJson);
	output += " }";
	return(output);
}

