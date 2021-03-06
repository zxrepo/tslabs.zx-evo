
set path=%path%;%SDCC%\bin\

sdasz80 -g -o obj\crt0.rel src\crt0.s
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\main.c -o obj\main.rel
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\gcWin.c -o obj\gcWin.rel
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\numbers.c -o obj\numbers.rel
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\tsio.c -o obj\tsio.rel
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\ps2.c -o obj\ps2.rel
if errorlevel 1 pause & exit

sdcc.exe --std-c11 -mz80 --no-std-crt0 --opt-code-speed -c src\mouse.c -o obj\mouse.rel
if errorlevel 1 pause & exit

sdcc.exe -o bin\gcWin.hex --std-c11 -mz80 --no-std-crt0 --opt-code-speed -Wl-b_CODE=0x0200 -Wl-b_DATA=0xB000 obj\crt0.rel obj\main.rel obj\gcWin.rel obj\numbers.rel obj\tsio.rel obj\ps2.rel obj\mouse.rel
if errorlevel 1 pause & exit

if exist bin\gcWin.hex hex2bin -e bin bin\gcWin.hex

spgbld -b res\spg.ini gcWin.spg
