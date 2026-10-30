#!/bin/sh
set -eu
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp TaskManager.cpp UI.cpp -o StudentTaskScheduler
echo "Build successful. Starting Student Task Scheduler..."
exec ./StudentTaskScheduler
