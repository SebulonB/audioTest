#!/bin/bash

clear
echo "Generate Patches:"

declare -a patch_list=("setting.toml")

# Iterate the string array using for loop
for val in ${patch_list[@]}; do
  echo " > $val"

  #generate char[] { } of *.toml file
  xxd -i $val | sed 's/\([0-9a-f]\)$/\0, 0x00/' > "$val".h

  #remove first line
  tail -n +2 "$val".h > "$val.tmp" && mv "$val.tmp" "$val".h

  #add cpp header defines "#ifndef"
  echo -e "const char patch_str_"$val"[] PROGMEM =\n{\n$(cat "$val".h)" > "$val".h 
  echo -e "#define "$val"_h_\n\n$(cat "$val".h)" > "$val".h 
  echo -e "#ifndef "$val"_h_\n$(cat "$val".h)" > "$val".h 
  echo "" >> "$val".h 
  echo "#endif" >> "$val".h
done

