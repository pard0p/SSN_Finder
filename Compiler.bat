@ECHO OFF

nasm -f win64 .\Assembly.asm -o .\Assembly.obj
g++ -o malicious.exe main.cpp Assembly.obj
del *.obj
malicious.exe