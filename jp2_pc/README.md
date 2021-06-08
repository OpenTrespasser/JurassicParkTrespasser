# Getting started with OpenTrespasser

## General build requirements

Visual Studio 2019 with the C++ Desktop Development workload is required. That includes the C++ compiler and the Windows SDK. You will also need the packages for MFC and ATL libraries.

All other libraries and tools are provided in this repository, either directly or from Git submodules.

You should install the game Trespasser *before* starting development on OpenTrespasser, so that the game directory can be autodetected.

## Generating the VS solution with CMake

The project configuration and build instructions are written in a CMake script.

It is recommended that you generate a VS solution and VS projects and work with these generated files. To do this, run the script `Generate-VS2019-x86.cmd`, the solution and project files will be generated under `Build\cmake-x86`.

If you want more control over CMake, then you can also run CMake-GUI with the launch script `Start-CMake-GUI.cmd`.

You can regenerate the solution with CMake while it is still open in VS. In that case, VS will ask if you want to reload the solution, which you should do.

VS2019 also has built-in support for CMake projects (if you install the package for that), but this has not been tested yet with OpenTrespasser.

### Project configurations

OpenTrespasser knows three project configurations:
- `Debug`: Compiler optimizations are disabled, runtime checks are enabled. All debugging code is enabled.
- `Release`: Compiler optimizations are enabled, runtime checks are reduced to default. Some, but not all, debugging code is disabled.
- `Final`: Same as Release, but all debugging code is disabled.

Note that debug symbols (PDBs) are generated in all three configurations, so using the debugger is always possible.

## Execution and debugging

The game executable `OpenTrespasser.exe` needs to be run in the Trespasser game directory, usually found in the `C:\Program Files (x86)\...` path.
If a Trespasser installation is detected by CMake, then the Trespasser directory is automatically set to be the working directory for debug execution in VS. This means that you can start the game directly from VS.

Also the built `OpenTrespasser.exe` is automatically copied to the Trespasser directory after each build. 

Depending on your system configuration, a program running in the Trespasser directory might require elevated rights to write files in that directory. It is recommended that you start VS with admin rights (see below). You should also configure the program to start with admin rights directly, via the compatibility settings.

### 16bit color depth mode

The game works in both 16bit and 32bit color depth. 
An exception applies to D3D rendering together with exclusive fullscreen mode: this combination only works with 16bit color depth. 

Additional tools like GUIApp have not yet been tested in 32bit color depth and might completely refuse to start in that configuration.

We do not know how to make VS start a program in 16bit color depth mode. Perhaps it can be done by starting VS with 16bit color depth, but that would be very inconvenient.

The recommended procedure for using 16bit color depth is this:
- Open the executable's compatiblity settings. Open its file properties and go to the compatibility tab.
- Set the color mode to 16bit.
- Start the program.
- If you need the debugger, attach it manually as described below.

### Attaching the debugger manually

You can use the VS Debugger without starting a program from VS. In the `Debug` menu in the main menu bar, there is a command to attach to a running process. 
If you attached the debugger once, another command is added to the `Debug` menu to reattach it, so you don't have to reselect the process.

When starting Trespasser, a warning message might pop up, telling you that your graphics driver is not certified for DirectDraw. It is highly recommended that you do *not* disable that message. The moment that warning is on display is an excellent opportunity to manually attach the debugger.

### Output of debug information ###
In any normal Win32 GUI program, including the Trespasser game, output to console via `std::cout` and `std::cerr` does not work because there is no console.
However, Windows offers a special debug output channel, which can be viewed in the `Output` window in Visual Studio. Trespasser gives convenient access to that channel with the global output stream `dout` (declared in `Lib/Sys/DebugConsole.hpp`), which can be used just like `std::cout`.
Note that all output to `dout` is discarded in `Final` builds.

Trespasser also supports other forms of debug output, including log files and writing text directly to the game screen, as demonstrated by the `bones` cheat.

 ## General Tips and Troubleshooting

- Launch Visual Studio with admin rights. Just open the context menu (right-click) for its entry in the start menu and select `Run as Admin`.
Trespasser wants to write files to its own directory under `C:\Program Files (x86)\...`, and that requires elevated rights on modern Windows configurations.
By launching VS with admin rights, it can launch Trespasser with elevated rights as well.

- If you want to make a new tool with the Trespasser code and use MFC/AFX in your project (like the GUIApp), then add this line before you include the MFC headers
(i.e. in `stdafx.h` or `pch.h`):

    - `#define DIRECT3D_VERSION 0x0700`

    - The old Direct3D code in the Windows SDK headers is only activated if your project is based on Direct3D 7.0 or lower. In modern Windows SDKs, the MFC headers include the stuff of newer Direct3D versions by default.

- If you get the error `The command "setlocal` during the build, that means something went wrong in the postbuild step which is supposed to copy the newly built `OpenTrespasser.exe` to the Trespasser directory.
    - Ensure that VS is started with Admin rights (see above)
    - If the problem persists, disable the postbuild step. 
        - To disable it in VS, open the project settings for `trespass`, go to `Build Events` and remove the postbuild command. Note that it will be added again when the solution is regenerated by CMake.
        - To disable the postbuild step in CMake, regenerate the solution, but set the CMake variable `USE_TRESPASSER_DIRECTORY` to `FALSE`. That can be easily done with the CMake GUI.

- See `Documents/MemoryDebugging.md` for instructions on advanced debugging tools for memory corruption.
