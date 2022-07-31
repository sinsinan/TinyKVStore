g++ -I. -I.\utils\ -c .\TableMetaInfo.cpp
@REM if %errorlevel% neq 0 exit /b %errorlevel%
g++ -I. -I.\utils\ -c .\SSTable.cpp
@REM if %errorlevel% neq 0 exit /b %errorlevel%
g++ -I. -I.\utils\ -c .\Main.cpp
@REM if %errorlevel% neq 0 exit /b %errorlevel%
g++ TableMetaInfo.o SSTable.o Main.o -o output
@REM if %errorlevel% neq 0 exit /b %errorlevel%
.\output.exe
@REM if %errorlevel% neq 0 exit /b %errorlevel%