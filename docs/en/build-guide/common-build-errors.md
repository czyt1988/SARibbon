# Common Build Errors

## VS 2017 compiler error

> error D8050: cannot execute xxx/c1xx.dll – failed to insert command-line into debug record

If VS is installed correctly but this error still appears, two situations are usually responsible:

1. **Non-ASCII characters in the build path**  
   Early VS versions (e.g. VS 2017) place the build tree under the user's temp directory.  
   When the user name contains Chinese (or other non-ASCII) characters, the compiler fails.  
   Fix: override the build root in `CMakeSettings.json` (or VS settings):

   ```json
   {
     "configurations": [
       {
         "name": "x64-Debug",
         "generator": "Ninja",
         "configurationType": "Debug",
         "inheritEnvironments": [ "msvc_x64" ],
         "buildRoot": "${workspaceRoot}\\build\\x64-Debug"
       },
       {
         "name": "x64-Release",
         "generator": "Ninja",
         "configurationType": "Release",
         "inheritEnvironments": [ "msvc_x64" ],
         "buildRoot": "${workspaceRoot}\\build\\x64-Release"
       }
     ]
   }
   ```

2. **Maximum path length limitation (260 chars)**  
   Windows still enforces the legacy MAX_PATH limit by default.  
   Enable long-path support:

   - Press `Win + R`, type `gpedit.msc`  
   - Navigate to:  
     *Computer Configuration → Administrative Templates → System → Filesystem*  
   - Enable **"Enable Win32 long paths"**  
   - Reboot

   If the problem persists, simply move the project to a shorter location (e.g. `D:\src`) to reduce path length.

## MOC errors during build

When the output shows *moc*-related failures, just run the build again.  
This usually happens on the very first build when many files are MOC-ed simultaneously.  
Keep the existing build folder and re-invoke CMake/ninja/make; the issue disappears on the second or third attempt.