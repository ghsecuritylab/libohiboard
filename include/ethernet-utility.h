/*
 * This file is part of the libohiboard project.
 *
 * Copyright (C) 2016-2017 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 *  Matteo Civale
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
 * @file libohiboard/include/ethernet-utility.h
 * @author Matteo Civale
 * @brief network utility.
 */

#ifdef LIBOHIBOARD_ETHERNET

#ifndef __ETHERNET_UTILITY_H
#define __ETHERNET_UTILITY_H

/*******************************************************************************
 * These utility function is based on lwIP library. You must put the library in
 * the source folder and add some path to include library. Furthermore you must
 * include the files needed for the specific microcontroller. At the end, you
 * must define a global #define to detect the correct library version end enable
 * PIT module.
 * lwIP 1.4.1:
 *   - /Sources/<lwip-name-folder>/src/include
 *   - /Sources/<lwip-name-folder>/src/include/ipv4
 *   - /Sources/<lwip_port-name-folder>
 *   - LIBOHIBOARD_ETHERNET_LWIP_1_4_1
 *******************************************************************************/

#if defined (LIBOHIBOARD_ETHERNET_LWIP_1_4_1) || \
	defined (LIBOHIBOARD_ETHERNET_LWIP_2_0_3)

#include "arch/sys_arch.h"
// lwip includes
#include "lwip/tcp.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#ifdef LIBOHIBOARD_ETHERNET_LWIP_1_4_1
	#include "lwip/timers.h"
	#include "lwip/tcp_impl.h"
#endif
#ifdef LIBOHIBOARD_ETHERNET_LWIP_2_0_3
	#include "lwip/timeouts.h"
#endif
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

//porting includes
#include "lwip_port.h"

#endif

typedef struct _Ethernet_NetworkConfig
{
    ip_addr_t ip;
    ip_addr_t mask;
    ip_addr_t gateway;

    Ethernet_MacAddress mac;

#if defined (LIBOHIBOARD_ETHERNET_LWIP_1_4_1) || \
	defined (LIBOHIBOARD_ETHERNET_LWIP_2_0_3)

    void (*phyCallback) (Ethernet_DeviceHandle dev);
    uint32_t (*timerCallback) (void);

#if LWIP_NETIF_STATUS_CALLBACK==1
    netif_status_callback_fn	netif_status_callback;
#endif

#if LWIP_NETIF_LINK_CALLBACK==1
    netif_status_callback_fn	netif_link_callback;
#endif
#endif

} Ethernet_NetworkConfig;

void Ethernet_networkConfig (struct netif *netif, Ethernet_NetworkConfig *config);

#endif /* __ETHERNET_UTILITY_H */

#endif // LIBOHIBOARD_ETHERNET
