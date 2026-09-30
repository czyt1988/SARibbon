#!/bin/bash

DEST=../src
OPTS='-i "../src/widgets" -i "../src/widgets/colorWidgets" -i "../src/core" -i "_amalg_include" -w "*.cpp;*.h;*.hpp" -s'
# 3.0 namespace-include mirror: <SARibbonCore/xxx> only exists in build-tree
# sync dir / install tree, mirror it here so forwarding headers resolve
rm -rf _amalg_include && mkdir -p _amalg_include/SARibbonCore
cp ../src/core/SARibbonCoreGlobal.h ../src/core/SARibbonQt5Compat.hpp _amalg_include/SARibbonCore/
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.h $DEST/SARibbon.h
./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.cpp $DEST/SARibbon.cpp
rm -rf _amalg_include   # transient; never committed


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
    # Amalgamate.exe cannot resolve "../X.h" includes when the including
    # file was reached through a ".." path (tool limitation, verified by
    # experiment). The content of SARibbonWidgetsGlobal.h is already
    # inlined via the flat chain (SARibbonGlobal.h -> flat include), and
    # its include guard guarantees single inclusion, so the leftover line
    # in the product is safe to strip (otherwise the product does not
    # compile: the relative path does not exist next to the single file).
    sed -i '/#include "\.\.\/SARibbonWidgetsGlobal.h"/d' "$DEST/SARibbon.h" "$DEST/SARibbon.cpp"

else
    echo "Warning: SARibbon.cpp file does not exist"
fi
#  使用read命令达到类似bat中的pause命令效果
echo 按任意键继续
read -n 1
echo 继续运行