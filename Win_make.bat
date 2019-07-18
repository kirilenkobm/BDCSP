:: like Makefile but for windows
set "CL_PARAMS=/O2 /Iall /Iyour /Iincludes /D_USRDLL /DLL"
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat
call cl %CL_PARAMS% src\CSP.c src\grid.c /LD /Felibs\CSP.dll /link /DEF:src\CSP.def
DEL ".\CSP.obj" /f /q
DEL ".\grid.obj" /f /q
