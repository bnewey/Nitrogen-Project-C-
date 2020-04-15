#!/bin/bash

g++ -g /home/bbnewey/visual-workspace/AC_HEAT/com_redox/src/main.cpp -Wall  -o /home/bbnewey/visual-workspace/AC_HEAT/com_redox/bin/main -pthread -ljsoncpp -lmysqlclient -lboost_system -I /usr/include/mysql -L/home/bbnewey/visual-workspace/AC_HEAT/redox/build 