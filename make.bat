@echo off
set existent=0
for %%a in (gcc tdm-gcc tcc) do (%%a -v > nul 2>&1 && set "existent=1" && set "compiler=%%a")
if "%existent%"=="0" (echo Could not find any compiler in path) else (
	for %%X in (32 64) do (

		%compiler% src/newtrodit.c -o newtrodit_x%%X.exe -O2 -luser32 -lkernel32 -m%%X  || (echo compilation error&&pause>nul&&exit /b 1)
		echo Compilation successful for Newtrodit x%%X
	)
	pause>nul
)
