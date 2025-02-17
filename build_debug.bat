:: Build all games
:: The library must be compiled with ENABLE_NEWLIB and ENABLE_CPLUSPLUS set to 1

ECHO off
SET "GDK=%cd:\=/%/../../SDKs/SGDKplusplus"
SET "GDK_WIN=%cd%/../../SDKs/SGDKplusplus"
SET "XGCC_WIN=%GDK_WIN%\x68k-gcc\bin"
SET "PATH=%GDK_WIN%\bin;%XGCC_WIN%;%PATH%"

set DIR=game

echo "Building '%DIR%' in debug..."
%GDK_WIN%\bin\make -C "%DIR%" debug -f %GDK_WIN%\makefilecpp.gen
echo "Build complete."
