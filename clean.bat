ECHO off
SET "GDK=%cd:\=/%/../../SDKs/SGDK2-CPP"
SET "GDK_WIN=%cd%/../../SDKs/SGDK2-CPP"
SET "PATH=%GDK_WIN%\bin;%GDK_WIN%\m68k-elf-toolchain\bin;%PATH%"

set DIR=game

echo "Cleaning '%DIR%'..."
%GDK_WIN%\bin\make -C "%DIR%" clean -f %GDK_WIN%\makefile.gen
echo "Clean complete."
