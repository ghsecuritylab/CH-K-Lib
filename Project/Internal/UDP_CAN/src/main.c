/* * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "enet.h"
#include "pit.h"
#include "enet_phy.h"

// Standard C Included Files
#include <stdio.h>
// lwip Included Files
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

#include "can.h"

#define CAN_TX_ID  0x10
#define CAN_RX_ID  0x56


bool gEnetRev = false;


struct netif fsl_netif0;
extern err_t ethernetif_init(struct netif *netif);
extern err_t ethernetif_input(struct netif *netif);
u8_t timer_expired(u32_t *last_time,u32_t tmr_interval);
extern void PIT_ISR(void);
void LWIP_Polling(void);

uint8_t     gCfgLoca_MAC[] = {0x00, 0xCF, 0x52, 0x35, 0x00, 0x01};

void ENET_ISR(void)
{
    gEnetRev = true;
}




void CAN_ISR(void);
extern void udp_echo_init(void);

int main(void)
{
    int ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    PIT_QuickInit(0, 1000*10);
    PIT_CallbackInstall(0, PIT_ISR);
    PIT_ITDMAConfig(0, kPIT_IT_TOF, true);
    
    printf("ENET LwIP CAN test!\r\n");
    printf("Connect to router (Not PC)!, IP will be display on terimal, UDP PORT=7\r\n");
    printf("This demo will receive UDP data and transmit it back to UDP and also copy data to CAN and UART\r\n");
    
    printf("CAN init\r\n");
    CAN_QuickInit(CAN1_TX_PE24_RX_PE25, kCAN_125K);
    CAN_CallbackInstall(HW_CAN1, CAN_ISR);
    CAN_ITDMAConfig(HW_CAN1,3, kCAN_IT_RX);
    CAN_SetRxMB(HW_CAN1, 3, CAN_RX_ID);
    
    /* enable PinMux */
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
	PORT_PinPullConfig(HW_GPIOB, 0, kPullUp);
	PORT_PinOpenDrainConfig(HW_GPIOB, 0, ENABLE);

    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
    ret = enet_phy_init();
    if(ret)
    {
        printf("enet phy init failed! code:%d\r\n", ret);
        while(1);
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = enet_phy_is_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !enet_phy_is_full_dpx();
    ENET_Init(&ENET_InitStruct1);
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);

    ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

    lwip_init();

#if LWIP_DHCP					 // DHCP
    fsl_netif0_ipaddr.addr=0;
    fsl_netif0_netmask.addr=0;
    fsl_netif0_gw.addr=0;
#else    
    IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
    IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
    IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);
#endif 
  
    netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, ethernet_input);
    netif_set_default(&fsl_netif0);
    netif_set_up(&fsl_netif0);
    
#if LWIP_DHCP	
    printf("dhcp start getting addr...\r\n");
    dhcp_start(&fsl_netif0);
    while((fsl_netif0.dhcp->offered_ip_addr.addr == 0) || (fsl_netif0.dhcp->offered_gw_addr.addr == 0) || (fsl_netif0.dhcp->offered_sn_mask.addr == 0))
    {
        if(gEnetRev)
        {
            ethernetif_input(&fsl_netif0); 
            gEnetRev = false;
        }
    }
    
    dhcp_fine_tmr();
    netif_set_addr(&fsl_netif0, &(fsl_netif0.dhcp->offered_ip_addr), &(fsl_netif0.dhcp->offered_sn_mask), &(fsl_netif0.dhcp->offered_gw_addr));
    netif_set_default(&fsl_netif0);
    netif_set_up(&fsl_netif0);
        
    u8_t *ip = (u8_t*)&fsl_netif0.ip_addr.addr;
    printf("dhcp new ip: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
    
    ip = (u8_t*)&fsl_netif0.dhcp->offered_sn_mask;
    printf("dhcp new net mask: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
    
    ip = (u8_t*)&fsl_netif0.dhcp->offered_gw_addr;
    printf("dhcp new net gw: %u.%u.%u.%u \r\n",ip[0], ip[1],ip[2], ip[3]);
    
#endif

    udp_echo_init();
    while(1)
    {
        if(gEnetRev)
        {
            gEnetRev = false;
            ethernetif_input(&fsl_netif0); 
        }
    }
}

