mkdir build
cd build

cmake -G "Visual Studio 15 2017" -A "x64" ^
      -DCMAKE_PREFIX_PATH=%CONDA_PREFIX% ^
      -DCMAKE_TOOLCHAIN_FILE=%CONDA_PREFIX%\h2o.cmake ^
      ../
	  
rem cmake --build . --target ALL_BUILD --config RelWithDebInfo

cd ..