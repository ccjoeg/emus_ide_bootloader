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

#define BOARD_TYPE "EFM32G232"

#define LED_PORT   PORTA
#define LED_PIN       9

#define TTY0_UART         USART0_BASE_ADDR
#define TTY0_CLK_REG      HFPERCLKEN0
#define TTY0_CLKEN        CMU_HFPERCLKEN0_USART0
#define TTY0_PORT         PORTE
#define TTY0_TX_PIN           10
#define TTY0_RX_PIN           11
#define TTY0_LOCATION     USART_ROUTE_LOCATION_LOC0
#define TTY0_CKDIV_REG    USART_CLKDIV_REG
#define TTY0_CLKDIV       EFMZG_USART_CLKDIV
#define TTY0_CMD_REG      USART_CMD_REG
#define TTY0_CMD_CLR      USART_CMD_CLEAR
#define TTY0_RXDATAXP_REG USART_RXDATAXP_REG
#define TTY0_TXDATA_REG USART_TXDATA_REG
#define TTY0_STATUS_REG USART_STATUS_REG
#define TTY0_STATUS_RXDATAV USART_STATUS_RXDATAV
#define TTY0_STATUS_TXBL USART_STATUS_TXBL

#define TTY1_UART         LEUART0_BASE_ADDR
#define TTY1_CLK_REG      LFBCLKEN0
#define TTY1_CLKEN        CMU_LFBCLKEN_LEUART0
#define TTY1_PORT         PORTE
#define TTY1_TX_PIN          14
#define TTY1_RX_PIN          15
#define TTY1_LOCATION     LEUART_ROUTE_LOCATION_LOC2
#define TTY1_CKDIV_REG    LEUART_CLKDIV_REG
#define TTY1_CLKDIV       EFMZG_LEUART_CLKDIV
#define TTY1_CMD_REG      LEUART_CMD_REG
#define TTY1_CMD_CLR      LEUART_CMD_CLEAR
#define TTY1_RXDATAXP_REG LEUART_RXDATAXP_REG
#define TTY1_TXDATA_REG LEUART_TXDATA_REG
#define TTY1_STATUS_REG LEUART_STATUS_REG
#define TTY1_STATUS_RXDATAV LEUART_STATUS_RXDATAV
#define TTY1_STATUS_TXBL LEUART_STATUS_TXBL

#define TTY2_UART         LEUART1_BASE_ADDR
#define TTY2_CLK_REG      LFBCLKEN0
#define TTY2_CLKEN        CMU_LFBCLKEN_LEUART1
#define TTY2_PORT         PORTC
#define TTY2_TX_PIN           6
#define TTY2_RX_PIN           7
#define TTY2_LOCATION     LEUART_ROUTE_LOCATION_LOC0
#define TTY2_CKDIV_REG    LEUART_CLKDIV_REG
#define TTY2_CLKDIV       EFMZG_LEUART_CLKDIV
#define TTY2_CMD_REG      LEUART_CMD_REG
#define TTY2_CMD_CLR      LEUART_CMD_CLEAR
#define TTY2_RXDATAXP_REG LEUART_RXDATAXP_REG
#define TTY2_TXDATA_REG LEUART_TXDATA_REG
#define TTY2_STATUS_REG LEUART_STATUS_REG
#define TTY2_STATUS_RXDATAV LEUART_STATUS_RXDATAV
#define TTY2_STATUS_TXBL LEUART_STATUS_TXBL

#define TTY3_UART         USART1_BASE_ADDR
#define TTY3_CLK_REG      HFPERCLKEN0
#define TTY3_CLKEN        CMU_HFPERCLKEN0_USART1
#define TTY3_PORT         PORTC
#define TTY3_TX_PIN           0
#define TTY3_RX_PIN           1
#define TTY3_LOCATION     USART_ROUTE_LOCATION_LOC0
#define TTY3_CKDIV_REG    USART_CLKDIV_REG
#define TTY3_CLKDIV       EFMZG_USART_CLKDIV
#define TTY3_CMD_REG      USART_CMD_REG
#define TTY3_CMD_CLR      USART_CMD_CLEAR
#define TTY3_RXDATAXP_REG USART_RXDATAXP_REG
#define TTY3_TXDATA_REG USART_TXDATA_REG
#define TTY3_STATUS_REG USART_STATUS_REG
#define TTY3_STATUS_RXDATAV USART_STATUS_RXDATAV
#define TTY3_STATUS_TXBL USART_STATUS_TXBL

#define TTY4_UART         USART2_BASE_ADDR
#define TTY4_CLK_REG      HFPERCLKEN0
#define TTY4_CLKEN        CMU_HFPERCLKEN0_USART2
#define TTY4_PORT         PORTC
#define TTY4_TX_PIN           2
#define TTY4_RX_PIN           3
#define TTY4_LOCATION     USART_ROUTE_LOCATION_LOC0
#define TTY4_CKDIV_REG    USART_CLKDIV_REG
#define TTY4_CLKDIV       EFMZG_USART_CLKDIV
#define TTY4_CMD_REG      USART_CMD_REG
#define TTY4_CMD_CLR      USART_CMD_CLEAR
#define TTY4_RXDATAXP_REG USART_RXDATAXP_REG
#define TTY4_TXDATA_REG USART_TXDATA_REG
#define TTY4_STATUS_REG USART_STATUS_REG
#define TTY4_STATUS_RXDATAV USART_STATUS_RXDATAV
#define TTY4_STATUS_TXBL USART_STATUS_TXBL

//setup the 2 UARTS
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

  GPIO->P[TTY1_PORT].DOUT  = (1 << TTY1_TX_PIN);  // To avoid false start, configure output TX as high
  GPIO_pinMode(TTY1_PORT,  TTY1_TX_PIN, GPIO_MODE_PUSHPULL);
  GPIO_pinMode(TTY1_PORT,  TTY1_RX_PIN, GPIO_MODE_INPUT);
  TTY1 = (unsigned long *) TTY1_UART;

  CMU->TTY1_CLK_REG |=  TTY1_CLKEN;
  TTY1[TTY1_CKDIV_REG] = TTY1_CLKDIV;
  TTY1[ROUTE_REG] = TTY1_LOCATION | ROUTE_RXPEN_TXPEN;
  TTY1[TTY1_CMD_REG] = TTY_CMD_RXEN_TXEN;
  
  GPIO->P[TTY2_PORT].DOUT  = (1 << TTY2_TX_PIN);  // To avoid false start, configure output TX as high
  GPIO_pinMode(TTY2_PORT,  TTY2_TX_PIN, GPIO_MODE_PUSHPULL);
  GPIO_pinMode(TTY2_PORT,  TTY2_RX_PIN, GPIO_MODE_INPUT);
  TTY2 = (unsigned long *) TTY2_UART;

  CMU->TTY2_CLK_REG |=  TTY2_CLKEN;
  TTY2[TTY2_CKDIV_REG] = TTY2_CLKDIV;
  TTY2[ROUTE_REG] = TTY2_LOCATION | ROUTE_RXPEN_TXPEN;
  TTY2[TTY2_CMD_REG] = TTY_CMD_RXEN_TXEN;
  
  GPIO->P[TTY3_PORT].DOUT  = (1 << TTY3_TX_PIN);  // To avoid false start, configure output TX as high
  GPIO_pinMode(TTY3_PORT,  TTY3_TX_PIN, GPIO_MODE_PUSHPULL);
  GPIO_pinMode(TTY3_PORT,  TTY3_RX_PIN, GPIO_MODE_INPUT);
  TTY3 = (unsigned long *) TTY3_UART;

  CMU->TTY3_CLK_REG |=  TTY3_CLKEN;
  TTY3[TTY3_CKDIV_REG] = TTY3_CLKDIV;
  TTY3[ROUTE_REG] = TTY3_LOCATION | ROUTE_RXPEN_TXPEN;
  TTY3[TTY3_CMD_REG] = TTY_CMD_RXEN_TXEN;
  
  GPIO->P[TTY4_PORT].DOUT  = (1 << TTY4_TX_PIN);  // To avoid false start, configure output TX as high
  GPIO_pinMode(TTY4_PORT,  TTY4_TX_PIN, GPIO_MODE_PUSHPULL);
  GPIO_pinMode(TTY4_PORT,  TTY4_RX_PIN, GPIO_MODE_INPUT);
  TTY4 = (unsigned long *) TTY4_UART;

  CMU->TTY4_CLK_REG |=  TTY4_CLKEN;
  TTY4[TTY4_CKDIV_REG] = TTY4_CLKDIV;
  TTY4[ROUTE_REG] = TTY4_LOCATION | ROUTE_RXPEN_TXPEN;
  TTY4[TTY4_CMD_REG] = TTY_CMD_RXEN_TXEN;
  
}

