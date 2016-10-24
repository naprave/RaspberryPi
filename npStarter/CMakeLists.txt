cmake_minimum_required(VERSION 2.6)

project(npstarter)

# add_definitions( -DUSEWIRING ) 
# set(llibs wiringPi)

add_executable(npstarter main.cpp) 
target_link_libraries (npstarter m ${llibs})

