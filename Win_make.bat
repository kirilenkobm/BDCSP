:: like Makefile but for Windows
SET "CL_PARAMS=/O2 /Iall /Iyour /Iincludes"
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat
CALL cl %CL_PARAMS% src\CSP.c ^
                    src\arrstuff.c ^
                    src\patterns.c ^
                    src\read_input.c ^
                    src\render.c ^
                    src\traverse.c ^
                    /link /out:CSP.exe
DEL ".\*.obj" /f /q
