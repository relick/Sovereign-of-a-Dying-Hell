# Sovereign of a Dying Hell
A Touhou Project fanwork visual novel for the Sega Mega Drive.

Game playable on web and with a ROM download: [Sovereign of a Dying Hell](https://relick.itch.io/sovereign-of-a-dying-hell)
v0.2 was submitted to the [Touhou Fan Game Jam 15](https://itch.io/jam/touhou-jam-15).

# Build
Requires a checkout of [SGDK2-CPP](https://github.com/relick/SGDK2-CPP) at `../../SDKs/SGDK2-CPP` and an install at `../../SDKs/SGDK2-CPP/install`.
There's no extra build prerequisites than what SGDK2-CPP requires.

# VS Code
The VS Code workspace is fully set up to work with the CMake Tools and C/C++ Extensions.
Use the CMake plugin to select configuration.
The default build action (CMake or tasks) will build the active configuration.

## Blastem
If [blastem](https://www.retrodev.com/blastem/) in installed at `../../SDKs/blastem/blastem.exe` on Windows, then the workspace can use it for running and debugging.

Build and run by using the task "Run Without Debugging" (not the CMake menu item).

Build and debug using VS Code hosting gdb by using the "Run -> Start Debugging" menu item.