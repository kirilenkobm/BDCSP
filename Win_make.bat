:: doesnt actually work as expected
:: AttributeError: function 'solve_CSP' not found
set vcvars="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
set cl_params = "/O2 /Iall /Iyour /Iincludes /D_USRDLL /DLL"
%vcvars% && cl %cl_params% src\CSP.c src\grid.c /LD /Felibs\CSP.dll /link

:: actually should delete this!
del ".\CSP.obj"
del ".\grid.obj"
