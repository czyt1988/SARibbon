#!/bin/bash

DEST=../src
OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.h $DEST/SARibbon.h
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.cpp $DEST/SARibbon.cpp
#  ʹ��read����ﵽ����bat�е�pause����Ч��
echo �����������
read -n 1
echo ��������