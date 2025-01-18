:: Build all games
:: The library must be compiled with ENABLE_NEWLIB and ENABLE_CPLUSPLUS set to 1

ECHO off
SET "GDK=%cd:\=/%/../../SDKs/SGDKplusplus"
SET "GDK_WIN=%cd%/../../SDKs/SGDKplusplus"
SET "XGCC_WIN=%GDK_WIN%\x68k-gcc\bin"
SET "PATH=%GDK_WIN%\bin;%XGCC_WIN%;%PATH%"

set DIRS=games\proof-of-concept

for /D %%i in (%DIRS%) do (
    echo "%%i"
    %GDK_WIN%\bin\make -C "%%i" clean -f %GDK_WIN%\makefilecpp.gen
)  
