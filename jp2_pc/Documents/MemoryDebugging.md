## Debugging Memory access violations (MAVs)

As is the case with many C/C++ programs, OpenTrespasser has frequent problems with memory access violations, also known as segmentation faults. Specific types of observed MAVs include `nullptr` dereferencing, use-after-free, dangling pointers and out-of-bounds array access.

Here is an example: the game used to crash after reloading the level after death. The crashes were in seemingly random places: opening a file, reading the registry, resizing a vector of UI elements, and more. Some of these spots were within Win32 API calls. Mitigating those exceptions in place just shifted the problem to somewhere else.

What all those places had in common was a memory deallocation operation. Clearly, this was a case of heap corruption. Execution with FPHV (see below) revealed the cause: illegal memory writes outside of array boundaries in the pathfinding for the dinosaur AI.

OpenTrespasser should be checked regularly for MAVs that are not caught be the default runtime checks. Tools and libraries exist to raise errors immediately when they occur. However, these techniques come with a significant performance penalty for both speed and memory.

#### Full Page Heap Verification (FPHV)
This technique catches illegal memory reads and writes as soon as they occur. It works by reserving an entire 'memory page' for each dynamically allocated memory block. Furthermore, it surrounds each allocation with inaccessible memory pages. Thus an illegal memory access is more likely to also hit an invalid address and raise an exception.

How to use FPHV:

 1. Open a command line *with admin rights* and navigate to `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64`.
 1. Run the command `gflags /p /enable OpenTrespasser.exe /full`.  You do *not* have to specify the full path to `OpenTrespasser.exe`. Instead, this setting will affect *any* copy of the program.
 1. Verify that the setting was applied with `gflags /p`.
 1. Start the game. If you are starting it from outside Visual Studio, remember to attach the debugger manually.
 1. Test the game normally and wait for an exception to be raised by a MAV.
 1. After you are done with debugging, disable FPHV with `gflags /p /disable OpenTrespasser.exe`.
 
If `gflags` is missing in the Windows SDK, [download the installer for the latest Windows SDK from Microsoft](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/) and install the subpackage "Debugging Tools for Windows".

[For a more detailed explanation of `gflags` and FPHV, please refer to the documentation by Microsoft.](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/example-12---using-page-heap-verification-to-find-a-bug)

#### VS AddressSanitizer
Visual Studio 2019 offers an AddressSanitizer option, based on the Clang/LLVM address sanitizer library. It needs to be installed as an extra VS package.
[For more details, please refer to the documentation by Microsoft.](https://devblogs.microsoft.com/cppblog/addresssanitizer-asan-for-windows-with-msvc/)

Set the CMake variable `ENABLE_SANITIZERS` to `TRUE` to activate AddressSanitizer.

If the runtime libraries are statically linked (which is the default for OTP), then using AddressSanitizer in combination with MFC will result in linker errors because both libraries want to implement the operators `new` and `delete`. This affects certain MFC-based tools like *GUIApp*, but not the main game.

If instead the runtime libraries are dynamically linked and you want to launch `OpenTrespasser.exe` or other applications outside of Visual Studio, you may need to copy the AddressSanitizer DLLs to your execution directory. The DLLs (`clang_rt.asan*.dll`) can be found in the `bin` directory of the MSVC tools directory.  
