#!/bin/bash
# Must run with tools/ as cwd:  bash Amalgamate.sh
# NOTE: this file is intentionally ASCII-only. It used to be the repo's
# single GBK-encoded file (NOTES B4); ASCII bytes are identical in GBK and
# UTF-8, so no editor/agent can corrupt it by encoding any more.
set -e
DEST=../src

# --- namespaced-include mirror (plan-01 S8, extended plan-03 S1) ---
# Forwarding headers in src/widgets and cross-includes inside src/core use
# <SARibbonCore/xxx.h>; that layout only exists in the build-tree sync dir /
# install tree, so mirror it here (flat form; core headers physically live in
# src/core/<subsystem>/).
rm -rf _amalg_include
mkdir -p _amalg_include/SARibbonCore
find ../src/core -type f \( -name '*.h' -o -name '*.hpp' \) -exec cp {} _amalg_include/SARibbonCore/ \;
# SARibbonCoreConfig.h is generated into the build tree (plan-01 S4/S6.2).
# Pick it up if a configured build tree is present.
for cfg in ../build*/include/SARibbonCore/SARibbonCoreConfig.h; do
    if [ -f "$cfg" ]; then cp "$cfg" _amalg_include/SARibbonCore/; break; fi
done

# --- SARibbonCore single file (core only) ---
# Both passes keep the mirror: core sources include each other via the quoted
# same-dir form (dedup path identical to the template path) and via the
# <SARibbonCore/X.h> flat form (dedup path identical to the mirror path), and
# no core header carries Q_OBJECT into the .cpp product.
OPTS_CORE='-i "../src/core" -i "_amalg_include" -i "_amalg_include/SARibbonCore" -w "*.cpp;*.h;*.hpp" -s'
./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.h   $DEST/SARibbonCore.h
./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.cpp $DEST/SARibbonCore.cpp

# --- SARibbonWidgets single file (core + widgets) ---
# The .h pass needs the mirror to resolve <SARibbonCore/...> forwarding includes.
# The .cpp pass deliberately omits it: the angle includes stay raw and are
# rewritten to "SARibbonWidgets.h" by the sed below (inlining through the mirror
# would duplicate Q_OBJECT headers into the .cpp product and break AUTOMOC;
# verified in plan-02 S2, NOTES B23).
OPTS_WIDGETS_H='-i "../src/widgets" -i "../src/widgets/colorWidgets" -i "../src/core" -i "_amalg_include" -w "*.cpp;*.h;*.hpp" -s'
OPTS_WIDGETS_CPP='-i "../src/widgets" -i "../src/widgets/colorWidgets" -i "../src/core" -w "*.cpp;*.h;*.hpp" -s'
./Amalgamate.exe $OPTS_WIDGETS_H ./amalgamate/SARibbonWidgetsAmalgamTemplate.h   $DEST/SARibbonWidgets.h
./Amalgamate.exe $OPTS_WIDGETS_CPP ./amalgamate/SARibbonWidgetsAmalgamTemplate.cpp $DEST/SARibbonWidgets.cpp

# --- artifact sanity: all 4 files must exist (set -e + explicit check) ---
for f in SARibbonCore.h SARibbonCore.cpp SARibbonWidgets.h SARibbonWidgets.cpp; do
    if [ ! -f "$DEST/$f" ]; then
        echo "ERROR: missing artifact $DEST/$f" >&2
        rm -rf _amalg_include
        exit 1
    fi
done

# --- post-processing ---
# Amalgamate.exe cannot resolve "../X.h" includes when the including file was
# reached through a ".." path, and cannot deduplicate angle <SARibbonCore/...>
# includes left raw by the cpp pass (both verified by experiment, NOTES B17/B23).
# The content of every core header is already inside the product headers, so:
# 1) strip the leftover "../SARibbonWidgetsGlobal.h" lines (guard-identical content)
# 2) point raw <SARibbonCore/X.h> lines at the product headers
sed -i '/#include "\.\.\/SARibbonWidgetsGlobal.h"/d' "$DEST/SARibbonCore.h" "$DEST/SARibbonCore.cpp"                                              "$DEST/SARibbonWidgets.h" "$DEST/SARibbonWidgets.cpp"
sed -i 's|#include <SARibbonCore/[A-Za-z0-9_]*\.h*>|#include "SARibbonCore.h"|g' "$DEST/SARibbonCore.cpp"
sed -i 's|#include <SARibbonCore/[A-Za-z0-9_]*\.h*>|#include "SARibbonWidgets.h"|g' "$DEST/SARibbonWidgets.cpp"

# LF -> CRLF (same awk logic as the 2.9.5 script, extended to 4 artifacts)
convert_to_crlf() {
    local file="$1"
    if [ -f "$file" ]; then
        awk '{sub(/$/, ""); print}' "$file" > "${file}.tmp"
        mv "${file}.tmp" "$file"
        echo "Converted line endings to CRLF for $file"
    fi
}
for f in SARibbonCore.h SARibbonCore.cpp SARibbonWidgets.h SARibbonWidgets.cpp; do
    convert_to_crlf "$DEST/$f"
done

rm -rf _amalg_include   # transient; never committed

# Wait for a keypress only on an interactive terminal (the 2.9.5 script had
# an unconditional 'read -n 1' which hangs CI).
if [ -t 0 ]; then
    echo "Press any key to continue"
    read -r -n 1
fi
