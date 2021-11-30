.section ".text.boot"

.global start

start:
    b reset
    b hang // undefined instruction
    b hang // software interrupt
    b hang // prefetch abort
    b hang // data abort
    b hang // unused
    b hang // IRQ
    b hang // FIQ

reset:
    // backup r1, which contains the architecture ID
    mov r7, r1
    // backup r2, which contains the atags/device tree address
    mov r8, r2

    // set the cpu to SVC32 mode
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0xD3
    msr cpsr, r0

    // set up the stack
    ldr r4, =__stack_top
    mov sp, r4

    // clear bss section
    ldr r0, =__bss_start
    ldr r2, =__bss_end
    sub r2, r2, r0
    mov r1, #0
    bl memset

    // mimic the convention used to boot ARM Linux
    mov r0, #0
    mov r1, r7
    mov r2, r8
    bl kmain
    b hang

hang:
    b hang
