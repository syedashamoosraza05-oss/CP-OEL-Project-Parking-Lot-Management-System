
# Parking Lot Management System

## Overview
A console-based parking lot management system written in C. It manages a lot of 50 slots and replaces manual record keeping with automatic slot tracking, fee calculation, and payment history.

Developed for CS-115 Computer Programming (Complex Engineering Problem), NED University of Engineering & Technology.

## Features
- Password-protected admin login
- Park, display, search, update, and check out vehicles
- 10 member slots (every 5th slot) with a 20% member discount
- Automatic fee calculation based on entry and exit date and time
- Number plate validation and duplicate plate detection
- Sort by slot and summary report
- Payment history saved to file
- Data saved to and loaded from file

## Technologies Used
- Language: C
- Compiler: GCC
- Storage: binary files (parking.dat, payments.dat)

## How to Run
1. Download or clone this repository.
2. Compile all the .c files together:

   "gcc parking_main.c input_utils.c date_time_utils.c lookup_utils.c payment_utils.c -o parking"
  
3. Run the program:

   ./parking
  
   (On Windows, use parking.exe.)

## Group Members
- Syeda Shamoos Raza (CS-26115)
- Syeda Aqsa Batool Zaidi (CS-26116)
- Ali Hussain (CS-26137)
- Muhammad Danish Ali (CS-26144)

## Course Information
- CS-115 Computer Programming
- Instructors: Miss Mahnoor Malik and Dr. Hameeza Ahmed
