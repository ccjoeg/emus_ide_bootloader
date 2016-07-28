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

uint8_t bl_name[] = "EFM32WG842_U1_L0";

#define LED_PORT   PORTA
#define LED_PIN       15

#define TTY0_UART         USART1_BASE_ADDR
#define TTY0_CLK_REG      HFPERCLKEN0
#define TTY0_CLKEN        TG_ZG_CMU_HFPERCLKEN0_USART1
#define TTY0_PORT         PORTC
#define TTY0_TX_PIN           0
#define TTY0_RX_PIN           1
#define TTY0_LOCATION     USART_ROUTE_LOCATION_LOC0
#define TTY0_CKDIV_REG    USART_CLKDIV_REG
#define TTY0_CLKDIV       EFMZG_USART_CLKDIV
#define TTY0_CMD_REG      USART_CMD_REG
#define TTY0_CMD_CLR      USART_CMD_CLEAR
#define TTY0_RXDATAXP_REG USART_RXDATAXP_REG

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




