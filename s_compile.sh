#!/bin/bash

clear
echo "compile project"

./s_generate_patches.sh
/Applications/Teensyduino.app/Contents/MacOS/Arduino --verify audioTest.ino 
