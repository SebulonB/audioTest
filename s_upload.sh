#!/bin/bash

clear
echo "upload project"

./s_generate_patches.sh
/Applications/Teensyduino.app/Contents/MacOS/Arduino --upload audioTest.ino 
