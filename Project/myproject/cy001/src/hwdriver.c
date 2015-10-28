#include "hwdriver.h"

static void hwGprsPowerGpioInit(void);
static void hwGprsPowerKeyGpioInit(void);
static void hwGprsPowerEn(bool en);
static void hwGprsPowerKey(bool active);

void hwDriverInit(void)
{
    hwGprsPowerGpioInit();
    hwGprsPowerKeyGpioInit();
    DelayMs(100);
    hwGprsPowerOn();
}

void hwGprsPowerGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(GPRSPOWER_OFF);
}

void hwGprsPowerKeyGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, kGPIO_Mode_OPP);//��������ʹ��kGPIO_Mode_OOD���͹���
    hwGprsPowerKey(GPRSPOWERKEY_OFF);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ����ߵ�ƽ����GPRSģ���Դ
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerEn(bool en)
{
    GPIO_WriteBit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, en);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ����͵�ƽʹGPRSģ�鿪��
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerKey(bool active)
{
    GPIO_WriteBit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   GPRS����ʱ��
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hwGprsPowerOn(void)
{
//Ԥ��������
//    hwGprsPowerEn(GPRSPOWER_OFF);
//    hwGprsPowerKey(GPRSPOWERKEY_OFF);
//    DelayMs(100);
    hwGprsPowerEn(GPRSPOWER_ON);
    DelayMs(100);
    hwGprsPowerKey(GPRSPOWERKEY_ON);
}

