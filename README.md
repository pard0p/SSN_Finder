# SSN_Finder
This is my implementation of the HellsGate algorithm for SSN lookup. It works as follows:

1. I get the base address of the NTDLL in memory using the PBE.
2. I get the address of the NTDLL export table.
3. I look up the address of the corresponding function by its name.
4. I extract the SSN using a simple Assembly routine.

This is the simplest way I could design. It should work perfectly for all versions of Windows.

## To compile:

```bash
nasm -f win64 .\Assembly.asm -o .\Assembly.obj
g++ -o malicious.exe main.cpp Assembly.obj
```

## Resources
https://unprotect.it/media/archive/2023/04/02/HellsGate.pdf
https://redops.at/en/blog/exploring-hells-gate
