#ifndef BOARD_H
#define BOARD_H

#define GPIO_BASE       0x01C20800
#define GPIO_E_CTRL_REG (GPIO_BASE + 0x90)

// GPIO functions
// GPIO E function 5 enables UART0 on GPIO 0/1 according to
// https://www.thirtythreeforty.net/media/F1C100s_Datasheet_V1.0.pdf
#define GPIO_E_UART0 0x5

// clock control unit
#define CCU_BASE 0x01C20000

// f1c100s/lichee nano
// see: https://steward-fu.github.io/website/mcu/f1c100s/asm_uart.htm
#define F1C100S_CLK_GATING_REG2 (CCU_BASE + 0x0068)
#define F1C100S_SOFT_RST_REG2   (CCU_BASE + 0x02D0)
#define F1C100S_UART0_BASE      (CCU_BASE + 0x5000)

// TODO: change the UART0 constants to be offsets so that we can use different
// UART base addresses, without having 1 constant for 1 register name per UART.

// UART0
// divisor latch low register
#define UART0_DLL (F1C100S_UART0_BASE + 0x00)
// receive buffer register
#define UART0_RBR (F1C100S_UART0_BASE + 0x00)
// transmit holding register
#define UART0_THR (F1C100S_UART0_BASE + 0x00)
// divisor latch high register
#define UART0_DLH (F1C100S_UART0_BASE + 0x04)
// interrupt enable register
#define UART0_IER (F1C100S_UART0_BASE + 0x04)
// fifo control register
#define UART0_FCR (F1C100S_UART0_BASE + 0x08)
// line control register
#define UART0_LCR (F1C100S_UART0_BASE + 0x0C)
// modem control register
#define UART0_MCR (F1C100S_UART0_BASE + 0x10)
// line status register
#define UART0_LSR (F1C100S_UART0_BASE + 0x14)

// Clock
#define CCU_PLL_PERIPH_CTRL (CCU_BASE + 0x028)
#define CCU_AHB_APB_CFG     (CCU_BASE + 0x054)

#endif
