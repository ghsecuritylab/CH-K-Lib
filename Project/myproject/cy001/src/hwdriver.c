#include "hwdriver.h"

static void hwGprsPowerGpioInit(void);
static void hwGprsPowerEn(bool en);
static void hwGprsPowerKeyGpioInit(void);
static void hwGprsPowerKey(bool active);
static void hwCmosPwdnGpioInit(void);
static void hwCmosResetGpioInit(void);

void hwDriverInit(void)
{
    hwGprsPowerGpioInit();
    hwGprsPowerKeyGpioInit();
    hwCmosPwdnGpioInit();
    hwCmosResetGpioInit();
    DelayMs(100);
    hwGprsPowerOn();
    hsCmosPowerOn();
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ��ʼ��GPRSģ���Դ���ƽţ�����͹رյ�Դ
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(GPRSPOWER_OFF);
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
static void hwGprsPowerEn(bool en)
{
    GPIO_WriteBit(GPRSPOWER_GPIO_PORT, GPRSPOWER_GPIO_PIN, en);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ��ʼ��GPRSģ�鿪�ؿ��ƽţ�����ߵ�ƽ
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerKeyGpioInit(void)
{
    GPIO_QuickInit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, kGPIO_Mode_OPP);//��������ʹ��kGPIO_Mode_OOD���͹���
    hwGprsPowerKey(GPRSPOWERKEY_OFF);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ����½���ʹGPRSģ�鿪�����߹ػ�������ʱ���ٳ�������͵�ƽ1s
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwGprsPowerKey(bool active)
{
    GPIO_WriteBit(GPRSPOWERKEY_GPIO_PORT, GPRSPOWERKEY_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   GPRS����ʱ��
 *   GPRS����100ms���ٽ�PowerKey�����
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

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ��ʼ��CMOS PWDN���ƽţ�����߲�����
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosPwdnGpioInit(void)
{
    GPIO_QuickInit(CMOSPWDN_GPIO_PORT, CMOSPWDN_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(CMOSPWDN_ACTIVE);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ����͵�ƽʹCMOS����
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosPwdn(bool active)
{
    GPIO_WriteBit(CMOSPWDN_GPIO_PORT, CMOSPWDN_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ��ʼ��CMOS Reset���ƽţ�����Ͳ�����
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosResetGpioInit(void)
{
    GPIO_QuickInit(CMOSRESET_GPIO_PORT, CMOSRESET_GPIO_PIN, kGPIO_Mode_OPP);
    hwGprsPowerEn(CMOSRESET_ACTIVE);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   ����͵�ƽ��λCMOS
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
static void hwCmosReset(bool active)
{
    GPIO_WriteBit(CMOSRESET_GPIO_PORT, CMOSRESET_GPIO_PIN, active);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 *   CMOS����ʱ��
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
void hsCmosPowerOn(void)
{
    hwCmosPwdn(CMOSPWDN_NEGATIVE);
    hwCmosReset(CMOSRESET_NEGATIVE);
}