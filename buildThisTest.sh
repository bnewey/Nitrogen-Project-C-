#!/bin/bash

g++ -g /home/bbnewey/visual-workspace/AC_HEAT/com_redox/src/tests.cpp -Wall  -o /home/bbnewey/visual-workspace/AC_HEAT/com_redox/bin/tests -pthread -ljsoncpp -lmysqlclient -lboost_filesystem -I /usr/include/mysql -L/home/bbnewey/visual-workspace/AC_HEAT/redox/build 