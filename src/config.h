/**************************************************************************//**
 * @file config.h
 * @brief Bootloader Configuration.
 *    This file defines how the bootloader is set up.
 * @author Silicon Labs
 * @version x.xx
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#pragma once

#include "em_device.h"

#define PORTA    0
#define PORTB    1
#define PORTC    2
#define PORTD    3
#define PORTE    4
#define PORTF    5

#define GPIO_MODE_MASK                          0x0000000FUL  // BIT MASK
#define GPIO_MODE_DISABLED                      0x00000000UL  // DISABLED
#define GPIO_MODE_INPUT                         0x00000001UL  // INPUT
#define GPIO_MODE_INPUTPULL                     0x00000002UL  // INPUTPULL
#define GPIO_MODE_INPUTPULLFILTER               0x00000003UL  // INPUTPULLFILTER
#define GPIO_MODE_PUSHPULL                      0x00000004UL  // PUSHPULL
#define GPIO_MODE_PUSHPULLDRIVE                 0x00000005UL  // PUSHPULLDRIVE
#define GPIO_MODE_WIREDOR                       0x00000006UL  // WIREDOR
#define GPIO_MODE_WIREDORPULLDOWN               0x00000007UL  // WIREDORPULLDOWN
#define GPIO_MODE_WIREDAND                      0x00000008UL  // WIREDAND
#define GPIO_MODE_WIREDANDFILTER                0x00000009UL  // WIREDANDFILTER
#define GPIO_MODE_WIREDANDPULLUP                0x0000000AUL  // WIREDANDPULLUP
#define GPIO_MODE_WIREDANDPULLUPFILTER          0x0000000BUL  // WIREDANDPULLUPFILTER
#define GPIO_MODE_WIREDANDDRIVE                 0x0000000CUL  // WIREDANDDRIVE
#define GPIO_MODE_WIREDANDDRIVEFILTER           0x0000000DUL  // WIREDANDDRIVEFILTER
#define GPIO_MODE_WIREDANDDRIVEPULLUP           0x0000000EUL  // WIREDANDDRIVEPULLUP
#define GPIO_MODE_WIREDANDDRIVEPULLUPFILTER     0x0000000FUL  // WIREDANDDRIVEPULLUPFILTER

#define USART0_BASE_ADDR       (0x4000C000UL)
#define USART1_BASE_ADDR       (0x4000C400UL)
#define USART2_BASE_ADDR       (0x4000C800UL)
 
#define LEUART0_BASE_ADDR      (0x40084000UL)
#define LEUART1_BASE_ADDR      (0x40084400UL)

#define TTY_CMD_RXEN_TXEN      (0x00000005UL)
 
#define RXDATA_REG      6
#define ROUTE_REG       21
 
#define USART_STATUS_REG    4
#define LEUART_STATUS_REG   2
 
#define USART_RXDATAXP_REG  10
#define LEUART_RXDATAXP_REG 8
 
#define USART_TXDATA_REG    13
#define LEUART_TXDATA_REG   10
 
#define USART_CLKDIV_REG    5
#define LEUART_CLKDIV_REG   3

#define USART_IEN_REG       19
#define LEUART_IEN_REG      14
 
#define USART_IFC_REG       18
#define LEUART_IFC_REG      13
 
#define USART_CMD_REG       3
#define LEUART_CMD_REG      1
 
#define EFMZG_LEUART_CLKDIV 0x59D0
#define EFMTG_LEUART_CLKDIV 0x77C0
#define EFMWG_LEUART_CLKDIV 0x77C0
 
#define EFMZG_USART_CLKDIV  0x0A65
#define EFMTG_USART_CLKDIV  0x0E32
#define EFMWG_USART_CLKDIV  3633

#define ROUTE_RXPEN_TXPEN   0x0003

#define READCTRL_MODE_WS1   0x1

#define LEUART_STATUS_TXBL    (0x1UL << 4)
#define USART_STATUS_TXBL   (0x1UL << 6)
 
#define RXDATAXP_FERRP    (0x1UL << 15)
#define USART_CMD_CLEAR   USART_CMD_CLEARRX | USART_CMD_CLEARTX
#define LEUART_CMD_CLEAR  LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX
 
#define CMU_LFBCLKEN_LEUART0      (0x1UL << 0)
#define CMU_LFBCLKEN_LEUART1      (0x1UL << 1)
 
#define TG_ZG_CMU_HFPERCLKEN0_USART0    (0x1UL << 2)
#define TG_ZG_CMU_HFPERCLKEN0_USART1    (0x1UL << 3)
 
#define WG_CMU_HFPERCLKEN0_USART0   (0x1UL << 0)
#define WG_CMU_HFPERCLKEN0_USART1   (0x1UL << 1)
#define WG_CMU_HFPERCLKEN0_USART2   (0x1UL << 2)
 
#define LEUART_IFC_MASK        (0x000007F9UL)
#define USART_IFC_MASK         (0x00001FF9UL)
 
#define IEN_RXDATAV            (0x1UL << 2)
#define IEN_TXBL               (0x1UL << 1)

void ledOn(void);
void ledOff(void);
void led_cycle(uint32_t on_cnt, uint32_t off_cnt);
void GPIO_pinMode(uint32_t port, uint32_t pin, uint32_t mode);


/******************************************************************************
 ** Frequency of the LF clock                                                 *
 ******************************************************************************/
#define LFRCO_FREQ           (32768)

/******************************************************************************
 * The size of the bootloader flash image                                     *
 ******************************************************************************/
#define BOOTLOADER_SIZE      (2048)



/******************************************************************************
 * USART used for communication.                                              *
 ******************************************************************************/
#if defined( _EZR32_LEOPARD_FAMILY )
#define BOOTLOADER_USART           USART1
#define BOOTLOADER_USART_CLOCKEN   CMU_HFPERCLKEN0_USART1
#define BOOTLOADER_USART_LOCATION  USART_ROUTE_LOCATION_LOC2

#elif defined( _EFM32_ZERO_FAMILY ) || defined( EFM32TG108F4 ) || \
      defined( _EFM32_HAPPY_FAMILY )
#define BOOTLOADER_USART           LEUART0
#define BOOTLOADER_USART_CLOCKEN   0
#define BOOTLOADER_LEUART_CLOCKEN  CMU_LFBCLKEN0_LEUART0
#define BOOTLOADER_USART_LOCATION  LEUART_ROUTE_LOCATION_LOC3

#else
#define BOOTLOADER_USART           USART0
#define BOOTLOADER_USART_CLOCKEN   CMU_HFPERCLKEN0_USART0
#define BOOTLOADER_USART_LOCATION  USART_ROUTE_LOCATION_LOC0
#endif

