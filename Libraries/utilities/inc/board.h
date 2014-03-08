#ifndef __BOARDS_H__
#define __BOARDS_H__

//!< ����ļ����岻ͬ�������ϵ�Ӳ����Դ

#ifdef CHK60EVB2

//!< LED �˿� �� ����
#define BOARD_BUZZER_GPIO_BASES     {HW_GPIOA}
#define BOARD_BUZZER_PIN_BASES      {6}
#define BOARD_BUZZER_FTM_MAP        FTM0_CH3_PA06
#define BOARD_LED_GPIO_BASES  {HW_GPIOE, HW_GPIOE, HW_GPIOE, HW_GPIOE}
#define BOARD_LED_PIN_BASES   {6, 7, 11, 12}
//!< ����Ĭ�ϵĴ���
#define BOARD_UART_DEBUG_MAP  UART0_RX_PD06_TX_PD07
//!< ����Ĭ�ϵ�I2C���߽ӿ�
#define BOARD_I2C_MAP         I2C0_SCL_PB02_SDA_PB03
//!< �����ϵ� ����
#define BOARD_KEY_GPIO_BASES  {HW_GPIOE, HW_GPIOE, HW_GPIOE}
#define BOARD_KEY_PIN_BASES   {26, 27, 28}
//!< ADC
#define BOARD_ADC_MAP         ADC0_SE19_DM0
//!< PWM
#define BOARD_PWM_MAP

#elif CHK60EVB1

#define BOARD_UART_DEBUG_MAP  UART4_RX_PC14_TX_PC15

#elif URANUS

#define BOARD_LED_GPIO_BASES {HW_GPIOA}
#define BOARD_LED_PIN_BASES  {1}
#define BOARD_I2C_MAP         I2C0_SCL_PE19_SDA_PE18
#define BOARD_UART_DEBUG_MAP  UART1_RX_PC03_TX_PC04
//!< �����ϵ� ����
#define BOARD_KEY_GPIO_BASES  {HW_GPIOA}
#define BOARD_KEY_PIN_BASES   {0}
#define BOARD_ADC_MAP         ADC0_SE19_DM0

#else

#warning  "Please Define a Board"

#endif



#endif
