#!/bin/bash
cd build && cmake .. && make -j3 && ./tests
./copier ../random_file ../output_file
cd ..
if cmp -s "random_file" "output_file"; then
  printf "Files are the same."
else
  printf "Files are DIFFERENT!"
fi

