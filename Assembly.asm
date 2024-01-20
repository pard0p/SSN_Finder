section .text

global GetSSNByFuncAddress

GetSSNByFuncAddress:
    mov ebx, 0xB8D18B4C
    mov rdx, 0x0
    mov rax, [rcx]
    cmp eax, ebx
    je GetSSNByFuncAddress + 0x1B
    add rcx, 0x20
    add rdx, 0x1
    jmp GetSSNByFuncAddress + 0xA
    mov rax, [rcx + 0x4]
    sub rax, rdx
    ret