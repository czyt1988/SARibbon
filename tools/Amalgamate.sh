#!/bin/bash

DEST=../src
OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.h $DEST/SARibbon.h
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.cpp $DEST/SARibbon.cpp
#  使用read命令达到类似bat中的pause命令效果
echo 按任意键继续
read -n 1
echo 继续运行