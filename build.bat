@echo off
echo Compiling Smart Library Management System...
gcc -Wall -Wextra -std=c99 -pedantic -O2 library.c -o library.exe
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful! Running library.exe...
    library.exe
) else (
    echo Compilation failed.
)
pause
