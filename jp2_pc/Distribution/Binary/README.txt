Installation:
- Copy all files to the Trespasser installation directory.

Execution:
- The Release build produces log files and has some additional internal checks. The Final build has neither, increasing performance.

- In some situations, especially when the game is running from C:\Program Files (x86)\..., elevated permissions are required to write files to the installation directory, especially temporary files and savefiles. OpenTrespasser will detect such situations and trigger the elevated permission request automatically at the start.
    - If the detection fails, you can configure OpenTrespasser to always require admin permissions in the compatibility settings.

- Apart from a Trespasser installation (demo suffices) and SMACKW32.DLL, no additional runtime packages are required.

Settings:
- Make sure that the settings "Installed Directory" and "Data Drive" are correct.

- The WindowMode setting has 3 possible states:
   - 1 = FRAMED: A bordered window
   - 2 = BORDERLESS: A borderless window. Can be like fullscreen if the window resolution matches the screen resolution
   - 3 = EXCLUSIVE: Exclusive fullscreen
   - all other values, including 0, result in undefined behavior

- Adjust "Width" and "Height" to change the window size. If you use exclusive mode and your configured window size is not supported by your system, the behavior is undefined.

- The bundled tool "RegToIniConverter.exe" generates a new OpenTrespasser.ini from the current Trespasser registry settings, or updates an existing configuration file.

- Direct3D mode requires various system-specific settings to work. The current recommendation is that you use vanilla Trespasser to configure Direct3D mode and generate the correct settings, then rebase your INI file onto those settings.