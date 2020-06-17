# Jurassic Park: Trespasser
A git-based fork of the Jurassic Park: Trespasser source code.

## Discord Server
#### Click the banner or link below to join the server
<p align="center">
  <a href="https://discord.gg/5EngSvu">
  <img src="https://github.com/GamerDude27/JurassicParkTrespasser/blob/master/images/JPT_DiscordServer.png">
  </a>
</p>
<p align="center">Link: https://discord.gg/5EngSvu</p>

## Cloning/Checkout instructions
**This is a complex repository using Git Submodules and Git LFS.**

After cloning, please run the script `gitsyncscript.sh` to also checkout the externals and download the LFS data. Use this command (on Windows, use Git Bash): `sh gitsyncscript.sh`

If you are using TortoiseGit to clone this repository, please check the boxes "Recursive" and "LFS" in the cloning dialog.


## Solution Overview
| Projects          | Generates                         | Notes:                                                                                                                                                                                                                 |
| :---------------- | :-------------------------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| AI                | Artificial Intelligence subsystem | A lot of code unused since most traits had to be disabled.                                                                                                                                                             |
| AI Test           | JP2_PC.exe                        | A standalone program allowing to test AI with graphics.                                                                                                                                                                |
| Audio             | Audio.lib                         | The audio sub-system static library featuring the "real-time Foley".                                                                                                                                                   |
| Bug               | Bugs.exe                          | A project concentrating all compiler errors. Since the team switched from VS4 to VS4.1 to VS4.2 to VS6.0 it was useful                                                                                                 |
| CollisionEditor   | CollisionEditor.exe               | Sound effects editor to test the audio engine (very powerful at the time)                                                                                                                                              |
| EntityDBase       | EntityDBase.lib                   | Classes representing all objects in the game.                                                                                                                                                                          |
| File              | File.lib                          | Abstraction classes for File and Images used to build the Groff archives.                                                                                                                                              |
| File Test	        | File	                            | Test for the file and image abstractions.                                                                                                                                                                              |
| Game              | Game.lib	                        | Glue, triggers, Player, Gun classes.                                                                                                                                                                                   |
| GeomDBase         | GeomDBase.lib                     | The 3D representation (Geometry) of all objects defined in EntityDBase.                                                                                                                                                |
| GroffBuild        | GroffBuild.exe                    | The tool in charge of gathering all game assets (3D, sounds, maps) in one GOFF file.                                                                                                                                   |
| GroffExp          | GroffExp.dle	                    | The DLL loaded by 3DS Max that export all data to GOFF sections. This was originally outsourced to another dev and is standalone.                                                                                      |
| GUIApp            | GUIApp.exe                        | A wrapper around the game. The GUI allows changing the game values at runtime for testing. Like the console allowing to change the CVAR in Quake engines.                                                              |
| Loader            | Loader.lib                        | The library loading GOFF assets to RAM.                                                                                                                                                                                |
| Math              | Math.lib                          | The math library (features a fInvSqrt that is not as good as Quake III's InvSqrt since it uses a lookup table but also uses Newton-Raphson).                                                                           |
| Math Test         | MathTest.exe                      | A few functions to test the speed of the math routines.                                                                                                                                                                |
| Physics           | Physics.lib                       | The pelvis heavy, penalty force-based Physic engine library.                                                                                                                                                           |
| PhysicsTest       | PhysicsTest.exe                   | A sandbox level where physic can be tested.                                                                                                                                                                            |
| PipeLineTest      | PipeLineTest.exe                  | Testbed for the rendering pipeline                                                                                                                                                                                     |
| Processor         | Processor.dll                     | Uses CPUID to detect 8086, 80286, 28386 or a 80486, Pentium, K6-3and K7, Detect Floating Point Unit and CPU speed. Loaded at runtime by System project in order to set automatically details level (based on CPU Mhz). |
| QuantizerTool     |	QuantizerTool.exe                 | Aborted project. Does nothing.                                                                                                                                                                                         |
| Render3D          | Render3D.lib                      | The hybrid software/Direct3D renderer.                                                                                                                                                                                 |
| ScreenRenderDWI   |	ScreenRenderDWI.lib               | Pentium, PentiumPro and K6_3D specific code ASM optimized code for scanline and cache rendering. Direct3D code.                                                                                                        |
| Std               |	Std.lib                           | Extension of STL. A horrible mess of specific containers of containers of set of hashmap. Arg.                                                                                                                         |
| System            | System.lib                        | Contains scheduler, Virtual Memory. Thread control. SetupForSelfModifyingCode (via modifying the page tables associated with the application). Many things are not used.                                               |
| trespass          | trespass.exe                      | The game we played.                                                                                                                                                                                                    |
| View              | View.lib                          | Raster to window code. Blitter, DirectDraw, Direct3D, software palette viewers.                                                                                                                                        |
| WaveTest          | WaveTest.exe                      | Shell to test wave modeling.                                                                                                                                                                                           |
| WinShell          | WinShell.lib                      | win32 windows creation and management library                                                                                                                                                                          |

## Production Pipeline
<p align="center">
  <img src="https://github.com/GamerDude27/JurassicParkTrespasser/blob/master/images/ProductionPipeline.png">
</p>

#

Sanglard, F. (2014). "Solution Overview" [list] & "Production Pipeline" [image].  
Available at: http://fabiensanglard.net/trespasser [Accessed 17 Oct. 2018].
