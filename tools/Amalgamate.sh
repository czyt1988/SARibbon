#!/bin/bash

DEST=../src
OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.h $DEST/SARibbon.h
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.cpp $DEST/SARibbon.cpp

if [ -f "$DEST/SARibbon.cpp" ]; then
    # Convert line endings from LF to CRLF
    convert_to_crlf() {
        local file="$1"
        if [ -f "$file" ]; then
            awk '{sub(/$/, "\r"); print}' "$file" > "${file}.tmp"
            mv "${file}.tmp" "$file"
            echo "Converted line endings to CRLF for $file"
        fi
    }
    
    convert_to_crlf "$DEST/SARibbon.cpp"
    convert_to_crlf "$DEST/SARibbon.h"
else
    echo "Warning: SARibbon.cpp file does not exist"
fi
#  使用read命令达到类似bat中的pause命令效果
echo 按任意键继续
read -n 1
echo 继续运行