; cf. https://github.com/ghaiklor/ghaiklor-os-gcc
; cf. https://github.com/tmathmeyer/sos
global interrupts

extern isr_handler
extern irq_handler

%macro def_isr_handler 1
    global isr%1
    isr%1:
        cli
        mov rdi, dword %1
        jmp isr_common_stub
%endmacro

%macro def_irq_handler 1
    global irq%1
    irq%1:
        cli
        mov rdi, dword (32 + %1)
        jmp irq_common_stub
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
def_isr_handler 0
def_isr_handler 1
def_isr_handler 2
def_isr_handler 3
def_isr_handler 4
def_isr_handler 5
def_isr_handler 6
def_isr_handler 7
def_isr_handler 8
def_isr_handler 9
def_isr_handler 10
def_isr_handler 11
def_isr_handler 12
def_isr_handler 13
def_isr_handler 14
def_isr_handler 15
def_isr_handler 16
def_isr_handler 17
def_isr_handler 18
def_isr_handler 19
def_isr_handler 20
def_isr_handler 21
def_isr_handler 22
def_isr_handler 23
def_isr_handler 24
def_isr_handler 25
def_isr_handler 26
def_isr_handler 27
def_isr_handler 28
def_isr_handler 29
def_isr_handler 30
def_isr_handler 31

; That is for syscalls
def_isr_handler 0x80

irq_common_stub:
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
    call irq_handler

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

; define hardware interruptions
; should be keep in sync with src/core/isr.h
def_irq_handler 0
def_irq_handler 1
def_irq_handler 2
def_irq_handler 3
def_irq_handler 4
