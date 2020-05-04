#include "./functions.cpp"

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

#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/included/unit_test.hpp>


using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(check_socket_is_valid){
    
    //initialize socket 
	int server_fd;
    int new_socket = socket(server_fd);

    BOOST_CHECK(new_socket > 0);

    close(new_socket);
}
