# Common Build Errors

- ✅ **VS2017 path issues**: solutions for non-ASCII paths and MAX_PATH limitations
- ✅ **MOC build glitches**: simple fix for first-build batch MOC failures
- ✅ **QWindowKit dependency missing**: configuration troubleshooting for third-party library

| Error Symptom | Cause | Solution |
|---------------|-------|----------|
| `error D8050: cannot execute c1xx.dll` | Build path contains non-ASCII characters or exceeds 260 chars | Set English build path, enable Win32 long paths |
| MOC-related compile errors | First-build batch MOC operations glitch | Keep build directory, rebuild 1-2 times |
| `Could not find QWindowKit` | QWindowKit source missing or a wrong package path | Initialize the submodule / provide the source, or set `-DQWindowKit_DIR` |

!!! bug "VS 2017 compiler error"

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
    Enable long-path support via Group Policy Editor:

    ```txt
    Press Win + R, type gpedit.msc.

    Navigate to:
    Computer Configuration > Administrative Templates > System > Filesystem
    Enable "Enable Win32 long paths".

    Reboot the system.
    ```

    If the problem persists, simply move the project to a shorter location (e.g. `D:\src`) to reduce path length.

!!! bug "MOC errors during build"

When the output shows *moc*-related failures, just run the build again.  
This usually happens on the very first build when many files are MOC-ed simultaneously.  
Keep the existing build folder and re-invoke CMake/ninja/make; the issue disappears on the second or third attempt.

!!! bug "QWindowKit not found"

If you enabled `SARIBBON_USE_FRAMELESS_LIB=ON` but get `Could not find a package configuration file provided by "QWindowKit"`, CMake could not obtain QWindowKit from any of these sources:

1. **Bundled source (default, recommended)** — `src/SARibbonBar/3rdparty/qwindowkit` is built as a subdirectory, no separate install needed. When the source is missing, CMake runs `git submodule update --init --recursive` automatically; source ZIP downloads must provide the source manually.
2. **A package specified by `-DQWindowKit_DIR`** — takes priority when QWindowKit is already installed.
3. **Local install directory / system path** — a previously built and installed QWindowKit.

Common causes: the submodule or its nested dependencies are missing and auto-initialization failed (network/proxy), the source tree was moved (use `-DSARIBBON_QWK_SOURCE_DIR=...`), CMake is older than 3.19 with no installed package, or `QWindowKit_DIR` points to the wrong directory.

To point at a prebuilt package manually:

```shell
cmake -S . -B build -DSARIBBON_USE_FRAMELESS_LIB=ON -DQWindowKit_DIR=<install-path>/lib/cmake/QWindowKit
```

If you don't need QWindowKit, set `SARIBBON_USE_FRAMELESS_LIB` to `OFF`.