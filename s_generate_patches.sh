#!/bin/bash

clear
echo "Generate Patches:"

dir_inc="src/patches/inc/"
dir_src="src/patches/"

declare -a patch_list=("setting.toml")

# Iterate the string array using for loop
for val in ${patch_list[@]}; do
  
  # *.toml -> *.h
  var_d=${val%?????}
  var2=$var_d.h
  echo " > $val"" | $var2"

  #generate char[] { } of *.toml file
  xxd -i $dir_src$val | sed 's/\([0-9a-f]\)$/\0, 0x00/' > $dir_inc$var2

  #remove first line
  tail -n +2 $dir_inc$var2 > "$val.tmp" && mv "$val.tmp" $dir_inc$var2

  #add cpp header defines "#ifndef"
  echo -e "const char patch_str_"$var_d"[] PROGMEM =\n{\n$(cat $dir_inc$var2)" > $dir_inc$var2 
  echo -e "#define patch_"$var_d"_h_\n\n$(cat $dir_inc$var2)" > $dir_inc$var2
  echo -e "#ifndef patch_"$var_d"_h_\n$(cat $dir_inc$var2)" > $dir_inc$var2
  echo "" >> $dir_inc$var2 
  echo "#endif" >> $dir_inc$var2
done
