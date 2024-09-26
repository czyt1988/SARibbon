# 这里演示如何再windows下通过命令行编译
# 如果你的操作系统安装了很多版本的qt和vs，你可以参考下面的命令，这里演示使用vs2017 qt5.14.2来编译qwk，
# qwk要求的cmake版本比较高，vs2017的默认cmake是不支持，这里做个演示如何通过命令行编译，具体路径需要实际使用时更改
"C:\Program Files (x86)\cmake3.27.9\bin\cmake.exe" -B build -S . -G "Visual Studio 15 2017" -A x64 -DQt5_DIR="C:\Qt\Qt5.14.2\5.14.2\msvc2017_64\lib\cmake\Qt5"
"C:\Program Files (x86)\cmake3.27.9\bin\cmake.exe" --build build --target install --config Debug
"C:\Program Files (x86)\cmake3.27.9\bin\cmake.exe" --build build --target install --config Release