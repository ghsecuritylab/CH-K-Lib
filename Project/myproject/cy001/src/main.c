#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"
#include "main.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/* UART ���ٳ�ʼ���ṹ��֧�ֵ�����* ʹ��ʱ�����Ƽ���׼��ʼ�� */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
 /*
     ʵ�����ƣ�UART��ӡ��Ϣ
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ�ô���UART��оƬ�ĳ�����Ϣ��оƬ�ϵ���ͳ�ȥ
        ������Ϻ󣬽���while�У�ִ��С����˸Ч��
*/

/* ���ڽ����жϻص�����
   �ں�����д�ж���Ҫ��������
*/
static void UART_RX_ISR(uint16_t byteReceived)
{
    /* �����յ������ݷ��ͻ�ȥ */
    UART_WriteByte(HW_UART1, byteReceived);
}

int main(void)
{
    uint32_t UID_buf[4];
    uint8_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 1, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);//��ӡ��Ϣ�ڣ�printf���Զ�ѡ���һ����ʼ���Ĵ���

    UART_QuickInit(UART1_RX_PE01_TX_PE00, 115200);
    /*  ����UART �ж����� �򿪽����ж� ��װ�жϻص����� */
    UART_CallbackRxInstall(HW_UART1, UART_RX_ISR);
    /* �򿪴��ڽ����жϹ��� IT �����жϵ���˼*/
    UART_ITDMAConfig(HW_UART1, kUART_IT_Rx, true);

    DelayMs(10);
    /* ��ӡоƬ��Ϣ */
    LOG("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* ��ӡʱ��Ƶ�� */
    LOG("core clock:%dHz\r\n", GetClock(kCoreClock));
    LOG("bus clock:%dHz\r\n", GetClock(kBusClock));
    CPUIDY_GetUID(UID_buf);
    LOG("UID:0x");
    for(i=0;i<4;i++)
    {
        LOG("%04x", UID_buf[3-i]);
    }
    LOG("\r\n");
    
    while(1)
    {
        /* ��˸С�� */
        GPIO_ToggleBit(HW_GPIOC, 1);
        DelayMs(500);
    }
}


