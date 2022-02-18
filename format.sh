#!/bin/bash
clang-format -i -style "{IndentWidth: 4, ColumnLimit: 120}" main.cpp src/* include/* tests/*

