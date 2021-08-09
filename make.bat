@set existent=0
@tcc -v > nul 2>&1 && @set "existent=1"
@if "%existent%"=="0" (@echo Could not find any compiler in path) else @(
	tcc newtrodit.c -o newtrodit.exe -O2 -luser32 -lkernel32 -m32  || @(echo compilation error&&pause>nul&&exit /b 1)
	echo compilation successful
	pause>nul
)
