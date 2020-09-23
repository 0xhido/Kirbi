#include "InjectedShellcode.h"

/*

BITS 64

section .text
global _main

; the main function
_main:
    mov rax, rsp
    and rsp, 0xffffffffffffff00
    add rsp, 8
    push rax

    call get_rip

get_rip:
    pop r15

    mov rcx, 0x6ddb9555	; djb2("KERNEL32.DLL")
    call get_module_base
    mov r12, rax

    cmp rax, 0
    je end

    mov rcx, 0x5fbff0fb	; djb2("LoadLibraryA")
    mov rdx, r12
    call resolve_import

    cmp rax, 0
    je end

    sub rsp, 16

    lea rcx, [r15+dll_path-get_rip]
    call rax			; LoadLibraryA("c:\\hooking.dll")

    add rsp, 16

    jmp end

; djb2 hash function
; rcx - the address of the string (must be null-terminated)
; rdx - the char size (1 for ascii, 2 for widechar)
djb2:
    push rbx
    push rdi

    mov eax, 5381

.hash_loop:
    cmp byte [rcx], 0
    je .return_from_func

    mov ebx, eax
    shl eax, 5
    add eax, ebx
    movzx rdi, byte [rcx]
    add eax, edi

    add rcx, rdx

    jmp .hash_loop

.return_from_func:
    pop rdi
    pop rbx

    ret

; module resolving function
; rcx - module name djb2 hash
get_module_base:
    push rbx

    mov r11, [gs:0x60]
    mov r11, [r11+0x18]

    lea r11, [r11+0x20]
    mov rbx, [r11]

.find_module_loop:
    push rcx

    mov rcx, [rbx+0x50]
    mov rdx, 2
    call djb2

    pop rcx

    cmp rax, rcx
    je .module_found

    mov rbx, [rbx]

    cmp rbx, r11
    jne .find_module_loop

.module_not_found:
    mov rax, 0
    jmp .return_from_func

.module_found:
    mov rax, [rbx+0x20]

.return_from_func:
    pop rbx
    ret

; import resolving function
; rcx - import name djb2 hash
; rdx - module base address
resolve_import:
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbx

.parse_pe_export_headers:
    mov rbx, rdx            ; ImageBase

    movzx rdx, word [rbx+0x3c]
    add rdx, rbx

    mov edx, dword [rdx+0x88]
    add rdx, rbx

    mov r11d, [rdx+0x1c]    ; AddressOfFunctions
    add r11, rbx

    mov r12d, [rdx+0x20]    ; AddressOfNames
    add r12, rbx

    mov r13d, [rdx+0x24]    ; AddressOfNameOrdinals
    add r13, rbx

    mov r14d, [rdx+0x14]    ; NumberOfFunctions

    mov r15, 0

.resolve_import_loop:
    push rcx

    mov ecx, dword [r12+r15*4]
    add rcx, rbx
    mov rdx, 1
    call djb2

    pop rcx

    cmp rax, rcx
    je .import_found

    inc r15
    cmp r15, r14
    jne .resolve_import_loop

    mov rax, 0
    jmp .return_from_func

.import_found:
    movzx rax, word [r13+r15*2]
    mov eax, dword [r11+rax*4]
    add rax, rbx

.return_from_func:
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11

    ret

dll_path:
    db "c:\\hooking.dll", 0

end:
    pop rsp
    ret

*/

unsigned char InjectedShellcode[] = "\x48\x89\xe0\x48\x81\xe4\x00\xff\xff\xff\x48\x83\xc4\x08\x50\xe8"
"\x00\x00\x00\x00\x41\x5f\xb9\x55\x95\xdb\x6d\xe8\x5b\x00\x00\x00"
"\x49\x89\xc4\x48\x83\xf8\x00\x0f\x84\x1b\x01\x00\x00\xb9\xfb\xf0"
"\xbf\x5f\x4c\x89\xe2\xe8\x80\x00\x00\x00\x48\x83\xf8\x00\x0f\x84"
"\x04\x01\x00\x00\x48\x83\xec\x10\x49\x8d\x8f\x24\x01\x00\x00\xff"
"\xd0\x48\x83\xc4\x10\xe9\xee\x00\x00\x00\x53\x57\xb8\x05\x15\x00"
"\x00\x80\x39\x00\x74\x12\x89\xc3\xc1\xe0\x05\x01\xd8\x48\x0f\xb6"
"\x39\x01\xf8\x48\x01\xd1\xeb\xe9\x5f\x5b\xc3\x53\x65\x4c\x8b\x1c"
"\x25\x60\x00\x00\x00\x4d\x8b\x5b\x18\x4d\x8d\x5b\x20\x49\x8b\x1b"
"\x51\x48\x8b\x4b\x50\xba\x02\x00\x00\x00\xe8\xbb\xff\xff\xff\x59"
"\x48\x39\xc8\x74\x0f\x48\x8b\x1b\x4c\x39\xdb\x75\xe3\xb8\x00\x00"
"\x00\x00\xeb\x04\x48\x8b\x43\x20\x5b\xc3\x41\x53\x41\x54\x41\x55"
"\x41\x56\x41\x57\x53\x48\x89\xd3\x48\x0f\xb7\x53\x3c\x48\x01\xda"
"\x8b\x92\x88\x00\x00\x00\x48\x01\xda\x44\x8b\x5a\x1c\x49\x01\xdb"
"\x44\x8b\x62\x20\x49\x01\xdc\x44\x8b\x6a\x24\x49\x01\xdd\x44\x8b"
"\x72\x14\x41\xbf\x00\x00\x00\x00\x51\x43\x8b\x0c\xbc\x48\x01\xd9"
"\xba\x01\x00\x00\x00\xe8\x50\xff\xff\xff\x59\x48\x39\xc8\x74\x0f"
"\x49\xff\xc7\x4d\x39\xf7\x75\xe0\xb8\x00\x00\x00\x00\xeb\x0d\x4b"
"\x0f\xb7\x44\x7d\x00\x41\x8b\x04\x83\x48\x01\xd8\x5b\x41\x5f\x41"
"\x5e\x41\x5d\x41\x5c\x41\x5b\xc3\x63\x3a\x5c\x5c\x68\x6f\x6f\x6b"
"\x69\x6e\x67\x2e\x64\x6c\x6c\x00\x5c\xc3";