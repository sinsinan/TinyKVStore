g++ -I. -I.\utils\ -c .\Position.cpp
if %errorlevel% neq 0 exit /b %errorlevel%
g++ -I. -I.\utils\ -c .\TableMetaInfo.cpp
if %errorlevel% neq 0 exit /b %errorlevel%
g++ -I. -I.\utils\ -c .\SSTable.cpp
if %errorlevel% neq 0 exit /b %errorlevel%
g++ -I. -I.\utils\ -c .\Main.cpp
if %errorlevel% neq 0 exit /b %errorlevel%
g++ TableMetaInfo.o SSTable.o Main.o Position.o -o output
if %errorlevel% neq 0 exit /b %errorlevel%
.\output.exe
if %errorlevel% neq 0 exit /b %errorlevel%