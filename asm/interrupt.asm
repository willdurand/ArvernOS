; cf. https://github.com/ghaiklor/ghaiklor-os-gcc
; cf. https://github.com/tmathmeyer/sos
global interrupt
extern isr_handler

%macro interrupt_handler 1
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
interrupt_handler 0
interrupt_handler 1
interrupt_handler 2
interrupt_handler 3
interrupt_handler 4
interrupt_handler 5
interrupt_handler 6
interrupt_handler 7
interrupt_handler 8
interrupt_handler 9
interrupt_handler 10
interrupt_handler 11
interrupt_handler 12
interrupt_handler 13
interrupt_handler 14
interrupt_handler 15
interrupt_handler 16
interrupt_handler 17
interrupt_handler 18
interrupt_handler 19
interrupt_handler 20
interrupt_handler 21
interrupt_handler 22
interrupt_handler 23
interrupt_handler 24
interrupt_handler 25
interrupt_handler 26
interrupt_handler 27
interrupt_handler 28
interrupt_handler 29
interrupt_handler 30
interrupt_handler 31
