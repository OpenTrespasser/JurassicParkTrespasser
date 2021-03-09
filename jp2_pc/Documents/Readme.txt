For easier viewing of this file, select "WordWrap" from the Edit menu.

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
Additional Trespasser Notes
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

This file contains additions to the Trespasser Manual made after the manual was printed, and detailed information about Trespasser features and support.


Special Note for AMD K6 2 Users
-------------------------------

Some optimizations for the AMD K6 2 have been implemented in Trespasser. We intend to do further optimizations to increase performance which will be made available in the future. Watch www.trespasser.com for developments.



Table of Contents
-------------------------------------------------------------------------------

	Section 1:  Configuration Issues (includes 'Safe Mode')
	Section 2:  Hardware Acceleration
	Section 3:  Audio
	Section 4:  Keyboard Conflicts
	Appendix A: Websites
	Appendix B: Legal Notice



-------------------------------------------------------------------------------
Section 1: Configuration Issues
-------------------------------------------------------------------------------

Driver Certification
--------------------

A number of drivers tested with Trespasser have not been DirectX certified by Microsoft. Issues concerning drivers, including performance, compatibility and incidental behavior should be referred to your hardware/driver vendor. Electronic Arts and DreamWorks Interactive cannot be held responsible for driver problems that occur with non-certified drivers.

You may run Trespasser, however, with uncertified drivers; if difficulties occur while using hardware accelerated rendering or audio, it is recommended that you switch back to software mode.


Initial Defaults
----------------

The first time Trespasser runs it will attempt to configure itself to your machine by automatically selecting screen resolution and render quality. In cases where an automatically selected screen resolution is not found, a lower resolution may be used. For example if Trespasser tries to set your screen resolution to 400x300, but that mode is not available, it will default to 320x240. To change your screen resolution, go to 'Options' and select 'Video.'


Viewing in Letterbox Mode
-------------------------

If you would like to view Trespasser in a cinematic environment, the game screen can be adjusted to your personal preference.  To decrease the screen height, press SHIFT then "+" or "-".  To decrease screen width, press CTRL then "+" or "-".

On some machines, letterbox mode may restrict the mouse cursor in menu screens.  If you run into this problem, just press ESC to go back into the game, then change your screen back to full size.


Adjusting your Monitor
----------------------

When the video signal to the monitor changes, either through changing resolutions or through changing video cards (i.e., switching from your primary video card to a Voodoo add-on card), the position of the image may shift left or right. After setting up Trespasser the way you want it (e.g., using software or hardware rendering, or set to a particular resolution) use your monitor controls to center the image. For further information consult the manual that came with your monitor.


Special Note for ATI Rage II Owners
-----------------------------------

Some versions of ATI's drivers do not work correctly in 400x300-screen resolution.


Special Note for Voodoo & Voodoo 2 Owners
-----------------------------------------

3DFX drivers have the default gamma setting of the card too high for Trespasser's outdoor environment. It is strongly recommended that you change the gamma of your card to 1.0. This setting is usually adjustable from a tab on the Display Properties dialog (right click on your desktop and select 'Properties'). Please consult your user's guide for more information.


Recovery from a Crash and 'Safe Mode'
-------------------------------------

Every effort has been made to engineer Trespasser to run on a wide variety of platforms without difficulty. Should you experience a crash, however, please note the following:

    * After crashing, Trespasser will ask if you wish to run in "Safe Mode" after relaunching. Safe mode will select the best settings for compatibility; but you will loose your screen resolution setting, your video driver setting and all audio will be disabled. After verifying that Trespasser works, you should restore these settings one-by-one.

    * When restarting Trespasser, ensure that your distribution CD-ROM is in the drive.

    * Ensure that you are using the latest drivers from your sound and video card manufacturers. For more information, visit the related websites in Appendix A. Also check for specific information about your video card under 'Section 2: Hardware Acceleration' and for your sound card under 'Section 3: Audio.'

    * Make sure that Windows has at least 32 MB free hard disk space for a swapfile. To view or change your Windows swapfile settings do the following: From the 'Start' button select 'Settings' and 'Control Panel.' In the control panel, click/double click on 'System.' In the system dialog select the 'Performance' tab and click on the 'Virtual Memory' button. It is recommended that the 'Let Windows manage my virtual memory settings' option be selected. If you are very low on hard disk space, try removing unused files.


Trespasser Requires 32 Bit Disk Drivers
---------------------------------------

NOTE: This section does not apply to Windows NT as all drivers on Windows NT are 32 Bit.

In order for Trespasser to function you must have 32 bit disk drivers for your hard disk and CD-ROM drive. Most systems will have IDE or SCSI drives so will therefore have 32 bit drivers. Some computers, especially lap top computers, contain CD drives with proprietary interfaces and you may not have 32 bit drivers for these devices. You can check if your file system is 32 bit by going to the 'System Properties' dialog (Right click on 'My Computer' and select 'Properties' from the menu) and clicking on the 'Performance' tab. It should read 'File System: 32 Bit'.

If you have any further concerns or problems, please contact your manufacturer.


-------------------------------------------------------------------------------
Section 2: Hardware Acceleration
-------------------------------------------------------------------------------

Video cards that have been tested and work correctly with Trespasser are listed under "Supported Video Cards." If your card is not listed, you may still try to run Trespasser with it. If it does not work or work well, however, you should switch back to Trespasser's software rendering mode.

If you select a resolution higher than can be supported by your hardware (due to memory limitations of the video card), Trespasser will automatically revert to its software renderer.


Hardware Issues
---------------

    * Only video cards listed in the "Supported Video Cards" section are known to work correctly and are officially supported by Trespasser. If you have difficulties with a supported video card, try installing updated drivers (see "General Recommendations").

    * When starting the application for the very first time, Trespasser occasionally will not find a Voodoo or Voodoo 2 card. If you have trouble when using either of these video cards, try closing Trespasser, rebooting your machine and rerunning Trespasser.

    * If you are able to select to hardware accelerated rendering, but Trespasser continues to use its software renderer, try switching back to software and then switch again to hardware; this will sometimes correct this problem.

    * Occasionally switching between video cards (e.g., Voodoo 2 and a NVidia Riva 128) will produce rendering artifacts. If this occurs, try exiting and rerunning Trespasser.

    * Video cards based on the ATI "Rage" II and Pro, and video cards based on the NVidia Riva 128 currently have serious performance problems with Trespasser. We are working with the manufacturers of these video cards to resolve these problems.

    * Trespasser will not work on Creative Labs Graphics Blaster PCI and other video cards based on the Rendition Verite 1000 chipset at 512x384 screen resolution (this resolution is suppressed by Trespasser to avoid crashes). This problem is not specific to Trespasser and occurs with many games. Video cards based on the Rendition Verite 1000 chipset do not support hardware accelerated rendering in Trespasser.


General Recommendations
-----------------------

    * Always start with lower screen resolutions and try higher resolutions later. Video cards with a limited amount of memory may not perform well at higher resolutions (see the section on 'Screen Resolution' in 'Hardware Options'). Also, always start with 'Triple Buffering' off, as triple buffering uses more video card memory.

    * It is strongly recommended that users who wish to use hardware accelerated rendering have 64 MB or more system memory installed; this is typical of many new 3D games.

    * Check your video card manufacturer's web site for driver upgrades; Trespasser supported video cards were tested with the latest drivers. See Appendix A.

    * In general, you should experiment with different hardware options to achieve the best balance between playability and appearance. See the section on "Upgrade Recommendations" if you are considering upgrading your system.


Hardware Options
----------------

You will have the following options when choosing a hardware device on the 'Video Driver Setup dialog':

    * Screen Resolution.
      Select the screen resolution. It is recommended that you select a lower resolution to start with, particularly if you are using a video card that has combined texture and video memory (e.g. a 4 MB Hercules Thriller). You can always increase the resolution later using the in game 'Video Options' screen.

    * Texture Resolution.
      Set this to 'medium' or 'low' for better performance if you have 32MB of system memory, a 4MB video card or if you are experiencing "stutter" while playing the game. Trespasser will automatically set this to medium for certain video cards.

    * Hardware Water.
      If enabled, water will be rendered using hardware acceleration.

    * Triple Buffering.
      If your video card has sufficient memory, turning this option 'on' may help Trespasser run faster. If your video card has 4MB or less memory, turning this option 'off' may help overall performance.

    * Dithering.
      This may either improve or harm the appearance of Trespasser depending on the video card you are using.

    * Page Managed.
      On some video cards allowing Trespasser to page manage textures will lead to an overall performance increase. Other video cards may perform better without Trespasser's page manager.


Supported Video Cards
---------------------

Due to the wide variety of features and performance differences between video cards, Trespasser may only support a subset of cards that work under Direct3D. In addition, the features enabled and performance gained from hardware accelerated rendering varies significantly from video card to video card.

Trespasser has been tested successfully with the following video cards (for more information about a specific video card see the notes associated with that card):

      NAME                            COMPANY, CHIPSET
    - 3D Blaster Voodoo 2             Creative Labs, Voodoo Graphics 2
    - ATI Rage Pro                    ATI, ATI Rage Pro, see notes
    - Diamond Viper 330               Diamond, NVidia Riva 128, see notes on the NVidia Riva 128
    - Diamond Viper 550               Diamond, NVidia Riva TNT, see notes on the NVidia Riva TNT
    - FireGL 1000 Pro                 Diamond, 3dLabs Permedia 2
    - Graphics Blaster Extreme        Creative Labs, 3dLabs Permedia 2
    - Intel Express 3D Graphics Card  Intel, Intel i740, see notes on the i740
    - Intense 3D Voodoo               Intergraph, Voodoo Rush
    - Matrox Productiva G100          Matrox, MGA-G100
    - Matrox Millenium G200           Matrox, MGA-G200, see notes on the Matrox G200
    - Maxi Gamer 3D                   Guillemot, Voodoo Graphics, see notes on the Voodoo 1
    - Maxi Gamer 3D 2                 Guillemot, Voodoo Graphics 2
    - Monster 3D                      Diamond, Voodoo Graphics, see notes on the Voodoo 1
    - Monster 3D II                   Diamond, Voodoo Graphics 2
    - Starfighter                     Real3D, Intel i740, see notes on the i740
    - STB Nitro DVD AGP               STB, Chromatic Research MPact 2, see notes
    - STB Velocity 128                STB, NVidia Riva 128, see notes on the NVidia Riva 128
    - Stingray 128 3D                 Hercules, Voodoo Rush
    - Thriller 3D                     Hercules, Rendition Verite 2200


The following chipsets are used by video cards supported by Trespasser (drivers for video cards using these chipsets may vary, however):

    - Intel i740
    - Matrox MGA-G100
    - Matrox MGA-G200
    - NVidia Riva 128 (see tweaks in notes section)
    - Permedia 2
    - Rendition Verite 2100
    - Rendition Verite 2200
    - Voodoo Graphics
    - Voodoo Graphics 2
    - Voodoo Rush

The following newly released chipsets work with Trespasser with the manufacturers' reference drivers:

   - 3DFX Banshee
   - NVidia Riva TNT (See notes on this chipset)

At the time of Trespasser's release retail boards based on the above two chipsets were not available for test. Trespasser has been tested with the S3 Savage chipset and did not work correctly. This may have been caused by problems with S3's drivers. This may be resolved in future releases of these drivers.

Trespasser currently is known NOT to work correctly with the following video cards and/or chipsets (this list is not comprehensive):

    - Matrox Mystique, Mystique 220 & Millenium II
    - PowerVR 1 & 2 boards (Matrox m3D and the VideoLogic Apocalypse 3Dx, 5D and 5D Sonic boards)
    - S3 Virge-based video cards (STB Nitro, Diamond Stealth 2000 & 3000, etc.)
    - Video cards based on the Rendition Verite 1000 chipset (3D Blaster PCI, Sierra Screamin' 3D, etc.)
    - Video cards based on the Trident 3DImage 975 and 985 chipsets (Jaton Video-87 and Jaton Video-97 AGP video cards, etc.)
    - Video cards and motherboards that use the SiS family of video chipsets
    - Number Nine Revolution 'Ticket to Ride' cards (see notes)


ATI Rage Pro
------------

It is strongly recommended that the texture resolution in the 'Video' selection dialog be set to 'medium' or 'low' to counter slower texture upload times on this card.

Trespasser requires Rage Pro drivers version 5.20 or later to be installed. You can download new drivers from ATI's web page.

The Rage Pro chipset does not support certain alpha effects used in Trespasser for rendering (e.g. water). Alpha textures may look blocky, the extent of the problem varying with different implementations of the chipset. This is a limitation in the Rage Pro's chipset and cannot be overcome in Trespasser. If you do not like the look of the water with the Rage Pro, disable the "Hardware Water" check box. The water may look better, but you will have reduced the frame rate of Trespasser around water.


Intel i740
----------

Occasional compatibility problems have shown up on a small number of computers. For those users who have problems running Trespasser in hardware accelerated rendering mode with an i740, it is recommended you contact your hardware manufacturer.
 

Matrox Productiva G100
----------------------

Due to this video card's use of stippling to simulate alpha texturing, alpha effects may not appear as they would with other video cards. Some rendering anomalies have been observed.


Matrox G200
-----------

The latest versions of the drivers for this video card are required for Trespasser (Version 4.24.013 or later).


Number Nine Revolution
----------------------

Some of these video cards report the required capabilities for Direct3D, but they do not work correctly with Trespasser. The rendered environment will appear jumbled, and the game will crash within a few seconds of play. We hope the problem will be corrected in future driver releases of this video card. For updated driver information, see the manufacturer's website at www.nine.com.


NVidia Riva 128
---------------

Trespasser does not function correctly using NVidia's reference drivers version 2.77. It has been tested successfully using the latest drivers from STB and Diamond. For the STB Velocity 128 this is driver release 1.80 dated 05-14-98, available on STB's website. For the Diamond Viper 330 this is driver version 4.10.01.0128 dated 08-26-98. A list of websites is included in Appendix A.

Difficulties with these manufacturer's drivers released after these dates should be referred to the manufacturer.

    - A lack of subpixel accuracy with this video card (affecting the Z buffer) may cause sorting anomalies.

    - Consider running in 400x300-screen resolution to get the best balance of performance/resolution with this video card.

    - Consider selecting lower texture resolutions to achieve better performance with this video card.


NVidia Riva TNT
---------------

It is recommended that 'Auto Mipmapping' be turned off. Consult the documentation of your video card for disabling this feature.


STB Nitro DVD AGP
-----------------

This video card supports alpha rendering through a technique called stippling, which uses a pattern to simulate varying translucency. This causes alpha effects to appear differently with this video card compared to other video cards supported by Trespasser. Also there are slight anomalies when running Trespasser using its software renderer (although hardware accelerated rendering is recommended for owners of this video card).


Voodoo
------

Older drivers for the original Voodoo may not work well with Trespasser. Problems may include difficulty detecting the video card, flashing of the sky and a lack of fogging on distant objects.

The Voodoo does work correctly with Trespasser using drivers dated 7-7-98 or later. If your video card manufacturer does not have recent drivers, try using the reference drivers from 3DFX's website (go to www.3dfx.com, go to 'Downloads' and download 'Voodoo Graphics Drivers'). Questions about updates of your video card manufacturer's drivers should be referred to the manufacturer.


Upgrade Recommendations
-----------------------

If you are planning to upgrade your computer in the near future, consider the following:

    * If you have 32 MB of system memory, purchasing at least an additional 32 MB is the best value for dollar upgrade you can make. Many new games, new applications and Windows 98 realize significant performance gains with 64 MB or more system memory.

    * If you have a slower computer and wish to add a 3D accelerator, choose a model that handles as much of the processing itself rather than relying on the main CPU. Trespasser does not endorse specific video cards. We suggest checking out magazine reviews.

    * Consider upgrading system memory and/or your video card before upgrading your CPU.


-------------------------------------------------------------------------------
Section 3: Audio
-------------------------------------------------------------------------------

Trespasser fully supports Creative's "EAX" - Environmental Audio Extensions to DirectSound 3D. Visit Creative's web site for more information on environmental audio and the Sound Blaster Live!. Trespasser also fully supports Aureal's A3D Interactive 3D positional audio technology. For sound cards featuring A3D, and for driver updates, please visit the A3D web site. 

Trespasser has been tested successfully with the following sound cards (for more information about a specific sound card see the notes associated with that sound card). It is not known to fail with any sound card and should work with future sound cards:

      NAME                                  BUS      COMPANY
    - AudioPCI 64                           PCI      Ensoniq,Creative Labs
    - AudioPCI 64 with EAX Drivers          PCI      Ensoniq,Creative Labs [board version 1370]
    - Game Theater 64                       ISA      Guillemot
    - Maxi Sound Game Theater 64            ISA      Guillemot
    - Microsoft Sound System                ISA      Microsoft
    - Pro Audio Spectrum 16                 ISA      Media Vision
    - Sound Blaster (all varieties)         ISA      Creative Labs
    - Sound Blaster Live! (all varieties)   PCI      Creative Labs
    - Tropez Plus                           ISA      Turtle Beach
    - Diamond Monster Sound                 PCI      Diamond
    - Montego A3D XStream                   PCI      Turtle Beach
    - Sonic Impact                          PCI      Diamond

Trespasser's audio system will take full advantage of any sound card that accelerates DirectSound 3D, including multiple speakers. In the unlikely event that this acceleration causes Trespasser not to function correctly, try the latest drivers for your specific card. If this fails there is an option within the game's 'Audio' dialog to disable 3D acceleration. Selecting this option will cause all 3D sounds to be mixed by the CPU, which may result in loss of performance and lower quality audio. If your PC has rear speakers this option will disable them and all audio will come from the front speakers.

The entire audio system can be disabled. On the CD in the Setup directory there is an program called DisableAudio.exe. Running this program will disable the whole audio system within Trespasser. In the same location is another program called EnableAudio.exe, which will re-enable the audio system.

If you choose the 'Minimal' option of the Trespasser installation, many of the Audio files remain on the CD. This may cause problems such as voice overs and music breaking up if your CD drive has a high seek time. If these symptoms appear it is recommended that you re-install Trespasser and select one of the two larger install options. This will put more audio data on the hard disk.


-------------------------------------------------------------------------------
Section 4: Keyboard Conflicts
-------------------------------------------------------------------------------

If you wish to customize your controls for Trespasser, you should note that many keyboards experience conflicts when using certain key combinations. This results in a key command being ignored. The conflict usually occurs when three keys are pressed simultaneously, and the third key is ignored. Below is a list of key conflicts we have found during our testing. Please check with your keyboard manufacturer for any specific information on keyboard conflicts with your configuration.

    I + J + U
    O + K + I
    P + L + O
    R + D + E
    E + S + W
    W + D + E
    U + K + I
    E + F + R
    I + L + O
    O + ; + P
    W + A + Q
    numpad8 + numpad4 + numpad7
    numpad8 + numpad6 + numpad9


-------------------------------------------------------------------------------
Appendix A: Websites
-------------------------------------------------------------------------------

Windrivers.com:     www.windrivers.com
This is a resource for finding the latest drivers for any piece of hardware in your computer.

3dfx:               www.3dfx.com
3dLabs:             www.3dlabs.com
ATI:                www.atitech.com
Aureal:             www.aureal.com
Creative Labs:      www.soundblaster.com
Diamond Multimedia: www.diamondmm.com
ESS Technologies:   www.esstech.com
Guillemot:          www.guillemot.com
Hercules:           www.hercules.com
Intel:              www.intel.com
Matrox:             www.matrox.com
Number Nine:        www.nine.com
NVidia:             www.nvidia.com
Real3D:             www.real3d.com
Rendition:          www.rendition.com
STB:                www.stb.com
Trident:            www.tridentmicro.com


-------------------------------------------------------------------------------
Appendix B: Legal Notice
-------------------------------------------------------------------------------

©1998 DreamWorks Interactive L.L.C. All rights reserved.  DreamWorks Interactive is a trademark of DreamWorks L.L.C. The Lost World: Jurassic Park™ & © 1998 Universal City Studios, Inc. and Amblin Entertainment, Inc. Trespasser is a trademark of Universal City Studios, Inc. and Amblin Entertainment, Inc. Licensed by Universal Studios Licensing, Inc. All rights reserved. .Electronic Arts and the Electronic Arts logo are trademarks or registered trademarks of Electronic Arts in the U.S. and/or other countries.  All other trademarks are properties of their respective owners.  All rights reserved.

