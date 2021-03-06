/*******************************************************************************
 * Copyright (C) 2016-2017 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 *  Simone Giacomucci <simone.giacomucci@gmail.com>
 *  Marco Giammarini <m.giammarini@warcomeb.it>
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

#ifdef LIBOHIBOARD_ETHERNET_LWIP_1_4_1

#include "sys_arch.h"
#include "lwip/sys.h"

static LWIPPorting_TimerCallback LWIPPorting_timerCallback;

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert (const char *msg)
{
//FSL:only needed for debugging
//uint8_t stringBuffer[50];
//sprintf(stringBuffer, msg);
#ifdef LWIP_DEBUG
//    PRINTF(msg);
//    PRINTF("\n\r");
#endif

    for(;;);
}

void LWIPPorting_setTimerCallback (LWIPPorting_TimerCallback callback)
{
    LWIPPorting_timerCallback = callback;
}

u32_t sys_now (void)
{
    return (u32_t)LWIPPorting_timerCallback();
}

#endif /* LIBOHIBOARD_ETHERNET_LWIP_1_4_1 */
