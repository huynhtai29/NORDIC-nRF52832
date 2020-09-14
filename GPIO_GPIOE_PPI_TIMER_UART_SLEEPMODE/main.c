/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include <nrf52.h>
#include <core_cm4.h>

volatile int val = 0;
uint8_t data_tx[2];
uint8_t data_rx[2];
uint16_t _data;

uint8_t _hour = 17, _min = 21, _ss;
/**
 * @brief Function for application main entry.
 */
void TIMER0_IRQHandler(void)
{

    NRF_GPIOTE->TASKS_OUT[3] = 1;
    NRF_TIMER0->TASKS_CLEAR = 1;
    NRF_TIMER0->TASKS_START = 1;

    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    SCB->SCR = (1 << 1);
    //    __WFE();
}
void GPIOTE_IRQHandler(void)
{

    NRF_P0->OUTSET |= (1 << 17);
    //    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NRF_GPIOTE->EVENTS_IN[0] = 0; // Enable INTERRUPTION EVENT_IN[0]
}

void UARTE0_UART0_IRQHandler(void)
{
    if (NRF_UART0->EVENTS_RXDRDY == 1)
    {
        val = NRF_UART0->RXD;
        NRF_UART0->TXD = val;
    }
    //    NRF_GPIOTE->TASKS_OUT[1] = 1;
    //    NRF_UART0->TASKS_STOPTX = 1;
    NVIC_ClearPendingIRQ(UARTE0_UART0_IRQn);
    NRF_UART0->EVENTS_TXDRDY = 0;
    NRF_UART0->EVENTS_RXDRDY = 0;
}
int main(void)
{

    /* Configure board. */
    //bsp_board_init(BSP_INIT_LEDS);
    NRF_P0->PIN_CNF[18] = 0x03;
    NRF_P0->PIN_CNF[17] = 0x03;
    NRF_P0->PIN_CNF[20] = 0x03;
    NRF_P0->PIN_CNF[19] = 0x03;
    NRF_P0->LATCH = 0xFFFFFFF;

    NRF_P0->PIN_CNF[14] = (3 << 16) | (3 << 2);
    NRF_P0->PIN_CNF[15] = (3 << 16) | (3 << 2);
    NRF_P0->PIN_CNF[16] = (3 << 16) | (3 << 2);
    NRF_P0->PIN_CNF[13] = (3 << 16) | (3 << 2); //    NRF_P0->DIRSET = 0x001E0000;
    NRF_P0->PIN_CNF[6] = 0x03;
    NRF_P0->PIN_CNF[7] = 0;
    // Configure GPIOTE
    //EVENTS
    NRF_GPIOTE->INTENSET = (1 << 0);                          // Enable INTERRUPTION EVENT_IN[0]
    NRF_GPIOTE->CONFIG[0] = (1 << 0) | (13 << 8) | (2 << 16); // Configure EVENT_IN[0]
    NRF_GPIOTE->CONFIG[5] = (1 << 0) | (14 << 8) | (2 << 16); // Configure EVENT_IN[0]
    NRF_GPIOTE->CONFIG[6] = (1 << 0) | (15 << 8) | (2 << 16); // Configure EVENT_IN[0]
    NRF_GPIOTE->CONFIG[7] = (1 << 0) | (16 << 8) | (2 << 16); // Configure EVENT_IN[0]
    // TASKS
    NRF_GPIOTE->CONFIG[1] = (3 << 0) | (17 << 8) | (3 << 16) | (1 << 20);
    NRF_GPIOTE->CONFIG[2] = (3 << 0) | (18 << 8) | (3 << 16) | (1 << 20);
    NRF_GPIOTE->CONFIG[3] = (3 << 0) | (19 << 8) | (3 << 16) | (1 << 20);
    NRF_GPIOTE->CONFIG[4] = (3 << 0) | (20 << 8) | (3 << 16) | (1 << 20);
    //Configure PPI
    //NRF_PPI->CHENSET = (15 << 0);
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[0];
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[1];
    NRF_PPI->CH[1].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[5];
    //    NRF_PPI->CH[1].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[2];
    NRF_PPI->CH[2].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[6];
    NRF_PPI->CH[2].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[3];
    NRF_PPI->CH[3].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[7];
    //    NRF_PPI->CH[3].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[4];
    NRF_PPI->CHG[0] = (15 << 0);
    NRF_PPI->TASKS_CHG[0].EN = 1;

    //Configure TIMER
    NRF_TIMER0->INTENSET = (1 << 16); // Enable INTERRUPTION CC[0]
    NRF_TIMER0->MODE = 0;             // Mode TIMER
    NRF_TIMER0->BITMODE = 3;          // Bitwindt
    NRF_TIMER0->PRESCALER = 4;        // PRESCALER
    NRF_TIMER0->CC[0] = 1000000;      // Value
    NRF_TIMER0->TASKS_START = 1;
    // Configure UART
    NRF_UART0->INTENSET = (1 << 2) | (1 << 7) | (1 << 9) | (1 << 17);
    NRF_UART0->PSELTXD = 6;
    NRF_UART0->PSELRXD = 7;
    NRF_UART0->BAUDRATE = 0x01D7E000;
    NRF_UART0->CONFIG = 0;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->ENABLE = 4;

    // TIMER0_IRQHandler
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 4); //optional: set priority of interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);
    // UARTE0_UART0_IRQn
    NVIC_DisableIRQ(UARTE0_UART0_IRQn);
    NVIC_ClearPendingIRQ(UARTE0_UART0_IRQn);
    NVIC_SetPriority(UARTE0_UART0_IRQn, 4); //optional: set priority of interrupt
    NVIC_EnableIRQ(UARTE0_UART0_IRQn);
    /*
    // Configure NVIC
    NVIC_DisableIRQ(GPIOTE_IRQn);
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_SetPriority(GPIOTE_IRQn, 3); //optional: set priority of interrupt
    NVIC_EnableIRQ(GPIOTE_IRQn);
    */
    //    NRF_GPIOTE->TASKS_CLR[1] = 1;
    __WFE();
    //    NRF_POWER->SYSTEMOFF = 2;

    while (true)
    {
        //        NRF_UART0->TXD = '1';
        nrf_delay_ms(1000);
        NRF_GPIOTE->TASKS_OUT[2] = 1;
    }
}
