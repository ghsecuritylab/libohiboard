/* Copyright (C) 2014-2015 A. C. Open Hardware Ideas Lab
 * 
 * Authors:
 *  Alessio Paolucci <a.paolucci89@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/**
 * @file libohiboard/source/interrupt_k64f.c
 * @author Alessio Paolucci <a.paolucci89@gmail.com>
 * @Clock implementations for FRDMK64.
 */

#include "platforms.h"

#if defined(MK64F12) || defined(FRDMK64F)

#include "interrupt.h"

#define NVIC_NUM_CORE_VECTORS           16
#define NVIC_NUM_MCU_VECTORS            86
#define NVIC_NUM_VECTORS                NVIC_NUM_CORE_VECTORS + NVIC_NUM_MCU_VECTORS

System_Errors Interrupt_enable (Interrupt_Vector vectorNumber)
{
    uint16_t div;

    /* Make sure that the IRQ is an allowable number. */
    if (vectorNumber > NVIC_NUM_MCU_VECTORS)
        return ERRORS_IRQ_NUM_VECTOR_WRONG;

    /* Determine which of the NVICISERs corresponds to the irq */
    div = vectorNumber/32;

    switch (div)
    {
    case 0x0:
        NVICICPR0 = 1 << (vectorNumber%32);
        NVICISER0 = 1 << (vectorNumber%32);
        break;
    case 0x1:
        NVICICPR1 = 1 << (vectorNumber%32);
        NVICISER1 = 1 << (vectorNumber%32);
        break;
    case 0x2:
        NVICICPR2 = 1 << (vectorNumber%32);
        NVICISER2 = 1 << (vectorNumber%32);
        break;
    }

    return ERRORS_NO_ERROR;
}

System_Errors Interrupt_disable (Interrupt_Vector vectorNumber)
{
    uint16_t div;

    /* Make sure that the IRQ is an allowable number. */
    if (vectorNumber > NVIC_NUM_MCU_VECTORS)
        return ERRORS_IRQ_NUM_VECTOR_WRONG;

    /* Determine which of the NVICISERs corresponds to the irq */
    div = vectorNumber/32;

    switch (div)
    {
    case 0x0:
        NVICICER0 = 1 << (vectorNumber%32);
        break;
    case 0x1:
        NVICICER1 = 1 << (vectorNumber%32);
        break;
    case 0x2:
        NVICICER2 = 1 << (vectorNumber%32);
        break;
    }

    return ERRORS_NO_ERROR;
}

#endif




