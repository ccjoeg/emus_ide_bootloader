//*****************************************************************************
// Copyright (c) 2015-2016 Engimusing LLC.  All right reserved.
//
// Modifications by Engimusing are released as a library
// that is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// 
//*****************************************************************************

#pragma once

volatile unsigned long *TTY0;
volatile unsigned long *TTY1;
volatile unsigned long *TTY2;
volatile unsigned long *TTY3;
volatile unsigned long *TTY4;

#define BOARD_TYPE "EFM32ZGUSB"

#define LED_PORT   PORTA
#define LED_PIN        8

#define TTY0_UART         LEUART0_BASE_ADDR
#define TTY0_CLK_REG      LFBCLKEN0
#define TTY0_CLKEN        CMU_LFBCLKEN_LEUART0
#define TTY0_PORT         PORTB
#define TTY0_TX_PIN          13
#define TTY0_RX_PIN          14
#define TTY0_LOCATION     LEUART_ROUTE_LOCATION_LOC1
#define TTY0_CKDIV_REG    LEUART_CLKDIV_REG
#define TTY0_CLKDIV       EFMZG_LEUART_CLKDIV
#define TTY0_CMD_REG      LEUART_CMD_REG
#define TTY0_CMD_CLR      LEUART_CMD_CLEAR
#define TTY0_RXDATAXP_REG LEUART_RXDATAXP_REG
#define TTY0_TXDATA_REG LEUART_TXDATA_REG
#define TTY0_STATUS_REG LEUART_STATUS_REG
#define TTY0_STATUS_RXDATAV LEUART_STATUS_RXDATAV
#define TTY0_STATUS_TXBL LEUART_STATUS_TXBL

#define TTY1_UART         0
#define TTY1_CLK_REG      HFPERCLKEN0
#define TTY1_CLKEN        TG_ZG_CMU_HFPERCLKEN0_USART1
#define TTY1_PORT         PORTC
#define TTY1_TX_PIN           0
#define TTY1_RX_PIN           1
#define TTY1_LOCATION     USART_ROUTE_LOCATION_LOC0
#define TTY1_CKDIV_REG    USART_CLKDIV_REG
#define TTY1_CLKDIV       EFMZG_USART_CLKDIV
#define TTY1_CMD_REG      USART_CMD_REG
#define TTY1_CMD_CLR      USART_CMD_CLEAR
#define TTY1_RXDATAXP_REG USART_RXDATAXP_REG
#define TTY1_TXDATA_REG USART_TXDATA_REG
#define TTY1_STATUS_REG USART_STATUS_REG
#define TTY1_STATUS_RXDATAV USART_STATUS_RXDATAV
#define TTY1_STATUS_TXBL USART_STATUS_TXBL

__STATIC_INLINE void CONFIG_UsartSetup(void)
{
  GPIO->P[TTY0_PORT].DOUT  = (1 << TTY0_TX_PIN);  // To avoid false start, configure output TX as high
  GPIO_pinMode(TTY0_PORT,  TTY0_TX_PIN, GPIO_MODE_PUSHPULL);
  GPIO_pinMode(TTY0_PORT,  TTY0_RX_PIN, GPIO_MODE_INPUT);
  TTY0 = (unsigned long *) TTY0_UART;

  CMU->TTY0_CLK_REG |=  TTY0_CLKEN;
  TTY0[TTY0_CKDIV_REG] = TTY0_CLKDIV;
  TTY0[ROUTE_REG] = TTY0_LOCATION | ROUTE_RXPEN_TXPEN;
  TTY0[TTY0_CMD_REG] = TTY_CMD_RXEN_TXEN;

  TTY1 = 0;//no tty1 on this board.
  TTY2 = 0;//no tty2 on this board.
  TTY3 = 0;//no tty3 on this board.
  TTY4 = 0;//no tty4 on this board.
}
