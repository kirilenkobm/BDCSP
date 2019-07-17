:: like Makefile but for windows
set vcvars = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
set cl_params = "/O2 /Iall /Iyour /Iincludes /D_USRDLL /DLL"
:: %vcvars% && cl %cl_params% src\CSP.c src\grid.c /LD /Felibs\CSP.dll /link /DEF:src\CSP.def
:: at some point I'll understand how to replace this bulky command with something nicer
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat" && cl /O2 /Iall /Iyour /Iincludes /D_USRDLL /DLL src\CSP.c src\grid.c /LD /Felibs\CSP.dll /link /DEF:src\CSP.def
del ".\CSP.obj" /f /q
del ".\grid.obj" /f /q
