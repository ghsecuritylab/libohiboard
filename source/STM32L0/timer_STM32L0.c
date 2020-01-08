/*
 * This file is part of the libohiboard project.
 *
 * Copyright (C) 2020 A. C. Open Hardware Ideas Lab
 *
 * Authors:
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
 */

/**
 * @file libohiboard/include/STM32L0/timer_STM32L0.c
 * @author Marco Giammarini <m.giammarini@warcomeb.it>
 * @brief Timer implementations for STM32L0 Series
 */

#if defined (LIBOHIBOARD_TIMER)

#ifdef __cplusplus
extern "C" {
#endif

#include "timer.h"

#include "platforms.h"
#include "utility.h"
#include "system.h"
#include "clock.h"
#include "interrupt.h"

#if defined (LIBOHIBOARD_STM32L0)

typedef struct _Timer_Device
{
    TIM_TypeDef* regmap;                         /**< Device memory pointer */

    volatile uint32_t* rccRegisterPtr;      /**< Register for clock enabling. */
    uint32_t rccRegisterEnable;        /**< Register mask for current device. */

    Timer_Pins pins[TIMER_MAX_PINS];/**< List of the pin for the timer channel. */
    Timer_Channels pinsChannel[TIMER_MAX_PINS];
    Gpio_Pins pinsGpio[TIMER_MAX_PINS];
    Gpio_Alternate pinsMux[TIMER_MAX_PINS];

    Interrupt_Vector isrNumber;                       /**< ISR vector number. */

    Timer_ActiveChannels currentChannel;

    void (* freeCounterCallback)(struct _Timer_Device *dev);
    void (* pwmPulseFinishedCallback)(struct _Timer_Device *dev);
    void (* outputCompareCallback)(struct _Timer_Device *dev);
    void (* inputCaptureCallback)(struct _Timer_Device *dev);

    uint32_t inputClock;                            /**< Current CK_INT value */

    Timer_Mode mode;
    Timer_ClockSource clockSource;
    /**< Define the counter type for a specific operational mode */
    Timer_CounterMode counterMode;

    bool autoreload; /**< Auto-reload preload enable, ARR register is buffered */

    Timer_DeviceState state;                   /**< Current peripheral state. */

} Timer_Device;

#if defined (LIBOHIBOARD_STM32L0x2) || \
    defined (LIBOHIBOARD_STM32L0x3)

#define TIMER_IS_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)   || \
                                 ((DEVICE) == OB_TIM3)   || \
                                 ((DEVICE) == OB_TIM6)   || \
                                 ((DEVICE) == OB_TIM7)   || \
                                 ((DEVICE) == OB_TIM21)  || \
                                 ((DEVICE) == OB_TIM22))

#define TIMER_IS_DEVICE_COUNTER_MODE(DEVICE) (((DEVICE) == OB_TIM2)   || \
                                              ((DEVICE) == OB_TIM3)   || \
                                              ((DEVICE) == OB_TIM4)   || \
                                              ((DEVICE) == OB_TIM5)   || \
                                              ((DEVICE) == OB_TIM8))

#define TIMER_IS_APB1_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)   || \
                                      ((DEVICE) == OB_TIM3)   || \
                                      ((DEVICE) == OB_TIM6)   || \
                                      ((DEVICE) == OB_TIM7))

#define TIMER_IS_APB2_DEVICE(DEVICE) (((DEVICE) == OB_TIM21)  || \
                                      ((DEVICE) == OB_TIM22))

//#define TIMER_IS_32BIT_COUNTER_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)   || \
//                                               ((DEVICE) == OB_TIM5))

#define TIMER_IS_OC_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)   || \
                                    ((DEVICE) == OB_TIM3)   || \
                                    ((DEVICE) == OB_TIM21)  || \
                                    ((DEVICE) == OB_TIM22))

#define TIMER_IS_IC_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)   || \
                                    ((DEVICE) == OB_TIM3)   || \
                                    ((DEVICE) == OB_TIM21)  || \
                                    ((DEVICE) == OB_TIM22))

#define TIMER_IS_CHANNEL1_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)  || \
                                          ((DEVICE) == OB_TIM3)  || \
                                          ((DEVICE) == OB_TIM21) || \
                                          ((DEVICE) == OB_TIM22))

#define TIMER_IS_CHANNEL2_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)  || \
                                          ((DEVICE) == OB_TIM3)  || \
                                          ((DEVICE) == OB_TIM21) || \
                                          ((DEVICE) == OB_TIM22))

#define TIMER_IS_CHANNEL3_DEVICE(DEVICE) (((DEVICE) == OB_TIM2)  || \
                                          ((DEVICE) == OB_TIM3))

#define TIMER_IS_CHANNEL4_DEVICE(DEVICE) (((DEVICE) == OB_TIM2) || \
                                          ((DEVICE) == OB_TIM3))

#define TIMER_IS_CHANNEL5_DEVICE(DEVICE) (false)

#define TIMER_IS_CHANNEL6_DEVICE(DEVICE) (false)

#define TIMER_IS_CHANNEL_DEVICE(DEVICE, CHANNEL)                                          \
                                                ((((DEVICE) == OB_TIM2) &&                \
                                                 (((CHANNEL) == TIMER_CHANNELS_CH1) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH2) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH3) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH4)))  || \
                                                 (((DEVICE) == OB_TIM3) &&                \
                                                 (((CHANNEL) == TIMER_CHANNELS_CH1) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH2) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH3) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH4)))  || \
                                                 (((DEVICE) == OB_TIM21) &&               \
                                                 (((CHANNEL) == TIMER_CHANNELS_CH1) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH2)))  || \
                                                 (((DEVICE) == OB_TIM22) &&               \
                                                 (((CHANNEL) == TIMER_CHANNELS_CH1) ||    \
                                                  ((CHANNEL) == TIMER_CHANNELS_CH2))))

#define TIMER_IS_NCHANNEL_DEVICE(DEVICE, CHANNEL) (false)

static Timer_Device tim2 =
{
        .regmap              = TIM2,

        .rccRegisterPtr      = &RCC->APB1ENR,
        .rccRegisterEnable   = RCC_APB1ENR_TIM2EN,

        .pins             =
        {
                               TIMER_PINS_PA0,
                               TIMER_PINS_PA1,
                               TIMER_PINS_PA2,
                               TIMER_PINS_PA3,
                               TIMER_PINS_PA5,
#if defined (LIBOHIBOARD_STM32L072KxT) || \
    defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PA15,
                               TIMER_PINS_PB3,
#endif
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PB10,
                               TIMER_PINS_PB11,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PE9,
                               TIMER_PINS_PE10,
                               TIMER_PINS_PE11,
                               TIMER_PINS_PE12,
#endif
        },
        .pinsChannel      =
        {
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
                               TIMER_CHANNELS_CH1,
#if defined (LIBOHIBOARD_STM32L072KxT) || \
    defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#endif
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
#endif
        },
        .pinsGpio         =
        {
                               GPIO_PINS_PA0,
                               GPIO_PINS_PA1,
                               GPIO_PINS_PA2,
                               GPIO_PINS_PA3,
                               GPIO_PINS_PA5,
#if defined (LIBOHIBOARD_STM32L072KxT) || \
    defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PA15,
                               GPIO_PINS_PB3,
#endif
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PB10,
                               GPIO_PINS_PB11,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PE9,
                               GPIO_PINS_PE10,
                               GPIO_PINS_PE11,
                               GPIO_PINS_PE12,
#endif
        },
        .pinsMux          =
        {
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_5,
#if defined (LIBOHIBOARD_STM32L072KxT) || \
    defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_5,
                               GPIO_ALTERNATE_2,
#endif
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
#endif
        },

        .isrNumber           = INTERRUPT_TIM2,
};
Timer_DeviceHandle OB_TIM2 = &tim2;

static Timer_Device tim3 =
{
        .regmap              = TIM3,

        .rccRegisterPtr      = &RCC->APB1ENR,
        .rccRegisterEnable   = RCC_APB1ENR_TIM3EN,

        .pins             =
        {
                               TIMER_PINS_PA6,
                               TIMER_PINS_PA7,
                               TIMER_PINS_PB0,
                               TIMER_PINS_PB1,
                               TIMER_PINS_PB4,
                               TIMER_PINS_PB5,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PC6,
                               TIMER_PINS_PC7,
                               TIMER_PINS_PC8,
                               TIMER_PINS_PC9,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PE3,
                               TIMER_PINS_PE4,
                               TIMER_PINS_PE5,
                               TIMER_PINS_PE6,
#endif
        },
        .pinsChannel      =
        {
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH3,
                               TIMER_CHANNELS_CH4,
#endif
        },
        .pinsGpio         =
        {
                               GPIO_PINS_PA6,
                               GPIO_PINS_PA7,
                               GPIO_PINS_PB0,
                               GPIO_PINS_PB1,
                               GPIO_PINS_PB4,
                               GPIO_PINS_PB5,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PC6,
                               GPIO_PINS_PC7,
                               GPIO_PINS_PC8,
                               GPIO_PINS_PC9,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PE3,
                               GPIO_PINS_PE4,
                               GPIO_PINS_PE5,
                               GPIO_PINS_PE6,
#endif
        },
        .pinsMux          =
        {
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_4,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
                               GPIO_ALTERNATE_2,
#endif
        },

        .isrNumber           = INTERRUPT_TIM3,
};
Timer_DeviceHandle OB_TIM3 = &tim3;

static Timer_Device tim6 =
{
        .regmap              = TIM6,

        .rccRegisterPtr      = &RCC->APB1ENR,
        .rccRegisterEnable   = RCC_APB1ENR_TIM6EN,

        .pins                =
        {
                               TIMER_PINS_NONE,
        },
        .pinsChannel         =
        {
                               TIMER_CHANNELS_NONE,
        },
        .pinsGpio            =
        {
                               GPIO_PINS_NONE,
        },
        .pinsMux             =
        {
                               GPIO_ALTERNATE_ANALOG,
        },

        .isrNumber           = INTERRUPT_TIM6_DAC,
};
Timer_DeviceHandle OB_TIM6 = &tim6;

static Timer_Device tim7 =
{
        .regmap              = TIM7,

        .rccRegisterPtr      = &RCC->APB1ENR,
        .rccRegisterEnable   = RCC_APB1ENR_TIM7EN,

        .pins                =
        {
                               TIMER_PINS_NONE,
        },
        .pinsChannel         =
        {
                               TIMER_CHANNELS_NONE,
        },
        .pinsGpio            =
        {
                               GPIO_PINS_NONE,
        },
        .pinsMux             =
        {
                               GPIO_ALTERNATE_ANALOG,
        },

        .isrNumber           = INTERRUPT_TIM6_DAC,
};
Timer_DeviceHandle OB_TIM7 = &tim7;

static Timer_Device tim21 =
{
        .regmap              = TIM21,

        .rccRegisterPtr      = &RCC->APB2ENR,
        .rccRegisterEnable   = RCC_APB2ENR_TIM21EN,

        .pins                =
        {
                               TIMER_PINS_PA2,
                               TIMER_PINS_PA3,
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PB13,
                               TIMER_PINS_PB14,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PD0,
                               TIMER_PINS_PD7,
                               TIMER_PINS_PE5,
                               TIMER_PINS_PE6,
#endif
        },
        .pinsChannel         =
        {
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#endif
        },
        .pinsGpio            =
        {
                               GPIO_PINS_PA2,
                               GPIO_PINS_PA3,
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PB13,
                               GPIO_PINS_PB14,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PD0,
                               GPIO_PINS_PD7,
                               GPIO_PINS_PE5,
                               GPIO_PINS_PE6,
#endif
        },
        .pinsMux             =
        {
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
#if defined (LIBOHIBOARD_STM32L072CxT) || \
    defined (LIBOHIBOARD_STM32L072CxY) || \
    defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_6,
                               GPIO_ALTERNATE_6,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_1,
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
#endif
        },

        .isrNumber           = INTERRUPT_TIM21,
};
Timer_DeviceHandle OB_TIM21 = &tim21;

static Timer_Device tim22 =
{
        .regmap              = TIM22,

        .rccRegisterPtr      = &RCC->APB2ENR,
        .rccRegisterEnable   = RCC_APB2ENR_TIM22EN,

        .pins                =
        {
                               TIMER_PINS_PA6,
                               TIMER_PINS_PA7,
                               TIMER_PINS_PB4,
                               TIMER_PINS_PB5,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PC6,
                               TIMER_PINS_PC7,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_PINS_PE3,
                               TIMER_PINS_PE4,
#endif
        },
        .pinsChannel         =
        {
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               TIMER_CHANNELS_CH1,
                               TIMER_CHANNELS_CH2,
#endif
        },
        .pinsGpio            =
        {
                               GPIO_PINS_PA6,
                               GPIO_PINS_PA7,
                               GPIO_PINS_PB4,
                               GPIO_PINS_PB5,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PC6,
                               GPIO_PINS_PC7,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_PINS_PE3,
                               GPIO_PINS_PE4,
#endif
        },
        .pinsMux             =
        {
                               GPIO_ALTERNATE_5,
                               GPIO_ALTERNATE_5,
                               GPIO_ALTERNATE_4,
                               GPIO_ALTERNATE_4,
#if defined (LIBOHIBOARD_STM32L072RxT) || \
    defined (LIBOHIBOARD_STM32L072RxI) || \
    defined (LIBOHIBOARD_STM32L072RxH) || \
    defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
#endif
#if defined (LIBOHIBOARD_STM32L072VxT) || \
    defined (LIBOHIBOARD_STM32L072VxI)
                               GPIO_ALTERNATE_0,
                               GPIO_ALTERNATE_0,
#endif
        },

        .isrNumber           = INTERRUPT_TIM22,
};
Timer_DeviceHandle OB_TIM22 = &tim22;

#endif

#endif // LIBOHIBOARD_STM32L0

#ifdef __cplusplus
}
#endif

#endif // LIBOHIBOARD_TIMER
