#!/bin/bash
g++ -std=c++2a main.cpp && ./a.out
if cmp -s "random_file" "output_file"; then
  printf "Files are the same."
else
  printf "Files are DIFFERENT!"
fi

