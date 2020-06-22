rmdir build /Q /S

mkdir build
cd build
gcc -c -mno-clwb ../src/lib/lib.c -o lib.o
gcc -shared -o lib.dll lib.o -Wl,--out-implib,lib.dll.a

gcc ../src/app.c -o app.exe
gcc ../src/injector.c -o injector.exe

del lib.dll.a /Q
del lib.o /Q