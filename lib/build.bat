gcc -c -mno-clwb lib.c
gcc -shared -o lib.dll lib.o -Wl,--out-implib,lib.dll.a