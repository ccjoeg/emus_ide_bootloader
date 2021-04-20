/**************************************************************************//**
 * @file boot.c
 * @brief Boot Loader
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

#include <stdio.h>
#include <stdbool.h>

#include "xmodem.h"
#include "em_device.h"
#include "boot.h"

#ifndef NDEBUG
bool printedPC = false;
#endif

extern uint32_t flashSize;


#define CPU_USER_PROGRAM_STARTADDR_PTR    ((uint32_t)(BOOTLOADER_SIZE + 0x00000004))
#define CPU_USER_PROGRAM_VECTABLE_OFFSET  ((uint32_t)BOOTLOADER_SIZE)
#define SCB_VTOR    (*((volatile uint32_t *) 0xE000ED08))
#define APPLICATION_START_ADDR (BOOTLOADER_SIZE)

/**************************************************************************//**
 * @brief Checks to see if the reset vector of the application is valid
 * @return false if the firmware is not valid, true if it is.
 *****************************************************************************/
bool BOOT_checkFirmwareIsValid(void)
{
  uint32_t pc;

  pc = *((uint32_t *)APPLICATION_START_ADDR + 1);

#ifndef NDEBUG
  if (!printedPC)
  {
    printedPC = true;
    printf("Application Reset vector = 0x%x \r\n", pc);
  }
#endif
  if (pc < flashSize)
    return true;
  return false;
}


/**************************************************************************//**
 * @brief This function sets up the Cortex M-3 with a new SP and PC.
 *****************************************************************************/
RAMFUNC void BOOT_jump(uint32_t sp, uint32_t pc)
{
  (void) sp;
  (void) pc;
  /* Set new MSP, PSP based on SP (r0)*/
  __asm("msr msp, r0");
  __asm("msr psp, r0");

  /* Jump to PC (r1)*/
  __asm("mov pc, r1");
}


/**************************************************************************//**
 * @brief Boots the application
 *****************************************************************************/
RAMFUNC void BOOT_boot(void)
{
  //  uint32_t pc, sp;
 void (*pProgResetHandler)(void);
  /* Reset registers */

#if defined(USART_OVERLAPS_WITH_BOOTLOADER) \
  && !defined(_SILICON_LABS_32B_SERIES_1)
  CMU->LFBCLKEN0    = _CMU_LFBCLKEN0_RESETVALUE;
  GPIO->ROUTE       = _GPIO_ROUTE_RESETVALUE;
  GPIO->EXTIPSELL   = _GPIO_EXTIPSELL_RESETVALUE;
  GPIO->EXTIFALL    = _GPIO_EXTIFALL_RESETVALUE;
  GPIO->IEN         = _GPIO_IEN_RESETVALUE;
  GPIO->IFC         = 0xFFFFFFFF;
#endif

 /* Reset GPIO settings */
  GPIO->P[5].MODEL  = _GPIO_P_MODEL_RESETVALUE;

  /* Clear all interrupts set. */
  NVIC->ICER[0]     = 0xFFFFFFFF;
#if ( __CORTEX_M != 0 )
  NVIC->ICER[1]     = 0xFFFFFFFF;
#endif
#if defined(_SILICON_LABS_32B_SERIES_1)
  //BOOTLOADER_USART->CLKDIV    = _USART_CLKDIV_RESETVALUE;
  //BOOTLOADER_USART->ROUTEPEN  = _USART_ROUTEPEN_RESETVALUE;
  //BOOTLOADER_USART->ROUTELOC0 = _USART_ROUTELOC0_RESETVALUE;

  LDMA->CTRL = 0x80000000;                    // Reset LDMA peripheral.
  LDMA->CTRL = _LDMA_CTRL_RESETVALUE;

  RTCC->IEN        = _RTCC_IEN_RESETVALUE;
  RTCC->IFC        = 0xFFFFFFFF;
  RTCC->CC[1].CCV  = _RTCC_CC_CCV_RESETVALUE;
  RTCC->CC[1].CTRL = _RTCC_CC_CTRL_RESETVALUE;
  RTCC->CTRL       = _RTCC_CTRL_RESETVALUE;

  MSC->WDATA     = _MSC_WDATA_RESETVALUE;
  MSC->ADDRB     = _MSC_ADDRB_RESETVALUE;
  MSC->WRITECTRL = _MSC_WRITECTRL_RESETVALUE;
  MSC->LOCK      = MSC_LOCK_LOCKKEY_LOCK;

  CMU->LFECLKSEL   = _CMU_LFECLKSEL_RESETVALUE;
  CMU->LFECLKEN0   = _CMU_LFECLKEN0_RESETVALUE;
  CMU->HFBUSCLKEN0 = _CMU_HFBUSCLKEN0_RESETVALUE;
  CMU->HFPERCLKEN0 = _CMU_HFPERCLKEN0_RESETVALUE;
  CMU->OSCENCMD    = CMU_OSCENCMD_LFRCODIS;
  CMU->CTRL        = _CMU_CTRL_RESETVALUE;
  // Change to default clock frequency.
  CMU->HFRCOCTRL   = *(uint32_t *)(&DEVINFO->HFRCOCAL8);

#else
  RTC->CTRL         = _RTC_CTRL_RESETVALUE;
  RTC->COMP0        = _RTC_COMP0_RESETVALUE;
  RTC->IEN          = _RTC_IEN_RESETVALUE;

  /* Disable RTC clock */
  CMU->LFACLKEN0    = _CMU_LFACLKEN0_RESETVALUE;
  CMU->LFCLKSEL     = _CMU_LFCLKSEL_RESETVALUE;
  /* Disable LFRCO */
  CMU->OSCENCMD     = CMU_OSCENCMD_LFRCODIS;
  /* Disable LE interface */
  CMU->HFCORECLKEN0 = _CMU_HFCORECLKEN0_RESETVALUE;
  /* Reset clocks */
  CMU->HFPERCLKDIV  = _CMU_HFPERCLKDIV_RESETVALUE;
  CMU->HFPERCLKEN0  = _CMU_HFPERCLKEN0_RESETVALUE;
#endif


  /* Set new vector table */
  //SCB->VTOR = (uint32_t)APPLICATION_START_ADDR;

  /* Read new SP and PC from vector table */
  //sp = *((uint32_t *)APPLICATION_START_ADDR);
  //pc = *((uint32_t *)APPLICATION_START_ADDR + 1);

  
#if defined(_SILICON_LABS_32B_SERIES_1)
  /* Invalidate I-cache before executing from main flash. */
  MSC->CACHECMD = MSC_CACHECMD_INVCACHE;
#endif


  //BOOT_jump(sp, pc);
  SCB_VTOR = CPU_USER_PROGRAM_VECTABLE_OFFSET & (uint32_t)0x1FFFFF80;
  pProgResetHandler = (void(*)(void))(*((uint32_t*)CPU_USER_PROGRAM_STARTADDR_PTR));
  pProgResetHandler();

}
