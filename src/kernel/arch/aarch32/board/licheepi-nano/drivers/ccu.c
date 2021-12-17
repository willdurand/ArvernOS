#include "ccu.h"
#include <board.h>
#include <core/mmio.h>

void ccu_init()
{
  // PLL_PERIPH = 600MHz
  //
  // PLL = (24MHz * N * K) / M
  //
  // 0x80041800:
  //  K = 1
  //  M = 1
  //  N = 24
  mmio_write(CCU_PLL_PERIPH_CTRL, 0x80041800);

  while (!(mmio_read(CCU_PLL_PERIPH_CTRL) & 0x10000000)) {
    __asm__("nop");
  }

  // PLL_PERIPH = 24MHz * 25 = 600MHz
  // AHB_CLK    = PLL_PERIPH / (AHB_PRE_DIV * AHB_CLK_DIV_RATIO)
  //            = 600MHz / (3 * 1) = 200MHz
  // APB_CLK    = AHB_CLK / APB_CLK_RATIO = 200MHz / 2 = 100MHz
  //
  // 0x00003180:
  //  AHB_PRE_DIV       = 3
  //  APB_CLK_RATIO     = 2
  //  AHB_CLK_DIV_RATIO = 1
  //
  // Baudrate = APB_CLK / (16 * divisor) = 100MHz / (16 * 54) = 115741 ~= 115200
  mmio_write(CCU_AHB_APB_CFG, 0x00003180);
}
