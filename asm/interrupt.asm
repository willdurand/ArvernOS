; cf. https://github.com/ghaiklor/ghaiklor-os-gcc
; cf. https://github.com/tmathmeyer/sos
global interrupt
extern isr_handler

%macro isr_handler 1
    global isr%1
    isr%1:
        cli
        mov rdi, dword %1
        jmp isr_common_stub
%endmacro

isr_common_stub:
    ; save registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rsi, rsp

    ; call handler
    call isr_handler

    ; restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    sti
    iretq

; define interruptions
; should be keep in sync with src/core/isr.h
isr_handler 0
isr_handler 1
isr_handler 2
isr_handler 3
isr_handler 4
isr_handler 5
isr_handler 6
isr_handler 7
isr_handler 8
isr_handler 9
isr_handler 10
isr_handler 11
isr_handler 12
isr_handler 13
isr_handler 14
isr_handler 15
isr_handler 16
isr_handler 17
isr_handler 18
isr_handler 19
isr_handler 20
isr_handler 21
isr_handler 22
isr_handler 23
isr_handler 24
isr_handler 25
isr_handler 26
isr_handler 27
isr_handler 28
isr_handler 29
isr_handler 30
isr_handler 31
