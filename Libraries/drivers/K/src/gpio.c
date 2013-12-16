/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   ����K60�̼��� GPIO ������ API����
  ******************************************************************************
  */
#include "gpio.h"


//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup GPIO
//! @brief GPIO driver modules
//! @{


//! @defgroup GPIO_Exported_Functions
//! @{


//!< Leagacy Support for Kineis Z Version(Inital Version)
#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif

#endif

//!< Gloabl Const Table Defination
static IRQn_Type   const GPIO_IRQBase = PORTA_IRQn;
static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};
static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};


void PORT_PinMuxConfig(uint8_t instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
		PORT_InstanceTable[instance]->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);
		PORT_InstanceTable[instance]->PCR[pinIndex] |=  PORT_PCR_MUX(pinMux);
}

void PORT_PinConfig(uint8_t instance, uint8_t pinIndex, PORT_Pull_Type pull, FunctionalState newState)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
		#ifndef MKL25Z4
		(newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_ODE_MASK);
		#endif
    switch(pull)
		{
        case kPullDisabled:
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PE_MASK;
            break;
        case kPullUp:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PS_MASK;
            break;
        case kPullDown:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
		}
}

void GPIO_PinConfig(uint8_t instance, uint8_t pinIndex, GPIO_PinConfig_Type pull)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (pull == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pinIndex));
}



	/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(GPIO_InitStruct->instance));
		assert_param(IS_PORT_ALL_INSTANCE(GPIO_InitStruct->instance));
		assert_param(IS_GPIO_ALL_PIN(GPIO_InitStruct->pinx));
		//config state
		switch(GPIO_InitStruct->mode)
		{
        case kGPIO_Mode_IFT:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp, ENABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    //config pinMux
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

void GPIO_QuickInit(uint8_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
		GPIO_InitStruct1.instance = instance;
		GPIO_InitStruct1.mode = mode;
		GPIO_InitStruct1.pinx = pinx;
		GPIO_Init(&GPIO_InitStruct1);
}

	/**
  * @brief  GPIO write a single bit to any GPIO pin
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @param  BitVal : bit value
	*         @arg Bit_RESET : low state
	*         @arg Bit_SET   : high state
  * @retval None
  */
void GPIO_WriteBit(uint8_t instance, uint8_t pinIndex, uint8_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pinIndex));
}

uint8_t GPIO_ReadBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    //!< input or output
    if(((GPIO_InstanceTable[instance]->PDDR) >> pinIndex) & 0x01)
		{
        return ((GPIO_InstanceTable[instance]->PDOR >> pinIndex) & 0x01);
		}
		else
		{
        return ((GPIO_InstanceTable[instance]->PDIR >> pinIndex) & 0x01);
		}
}
	/**
  * @brief  Toggle a GPIO single bit
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @retval None
  */
void GPIO_ToggleBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PTOR |= (1 << pinIndex);
}




	/**
  * @brief  Enable or disable GPIO's DMA support
  * @code
  *      // Check if transmit buffer is empty.
  *      if (spi_hal_is_transmit_buffer_empty(0))
  *      {
  *          // Buffer has room, so write the next data value.
  *          spi_hal_write_data(0, byte);
  *      }
  * @endcode
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_DMASelect_TypeDef: DMA trigger source select
	*         @arg kGPIO_DMA_Rising
	*         @arg kGPIO_DMA_Falling
	*         @arg kGPIO_DMA_RisingAndFalling
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @param  NewState : disable or enable
  *         @arg ENABLE
  *         @arg DISABLE
  * @retval None
  */



uint32_t GPIO_ReadByte(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  write GPIO 32 bit data
 * @param  GPIOx: pointer to a GPIO_Type
 *         @arg PTA: A Port
 *         @arg PTB: B Port
 *         @arg PTC: C Port
 *         @arg PTD: D Port
 *         @arg PTE: E Port 
 * @param  PortVal: 32bit data value
 * @retval None
 */
void GPIO_WriteByte(uint8_t instance, uint8_t pinIndex, uint32_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PDOR = data;
}

	/**
  * @brief  default GPIO struct init
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_ITDMAConfig(uint8_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, FunctionalState newState)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    // disable interrupt first
    NVIC_DisableIRQ((IRQn_Type)(GPIO_IRQBase + instance));
    PORT_InstanceTable[instance]->PCR[(uint8_t)pinIndex] &= ~PORT_PCR_IRQC_MASK;
		//config
    PORT_InstanceTable[instance]->PCR[(uint8_t)pinIndex] |= PORT_PCR_IRQC(config);
    //enable interrupt
    (ENABLE == newState)?(NVIC_EnableIRQ((IRQn_Type)(GPIO_IRQBase + instance))):(NVIC_DisableIRQ((IRQn_Type)(GPIO_IRQBase + instance)));
}

void GPIO_CallbackInstall(uint8_t instance, GPIO_CallBackType AppCBFun)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
    if(AppCBFun != NULL)
		{
        GPIO_CallBackTable[instance] = AppCBFun;
		}
}


void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
		if(GPIO_CallBackTable[HW_GPIOA])
		{
        GPIO_CallBackTable[HW_GPIOA](ISFR);
		}	
}

void PORTB_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOB]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOB]->ISFR = 0xFFFFFFFF;
		if(GPIO_CallBackTable[HW_GPIOB])
		{
        GPIO_CallBackTable[HW_GPIOB](ISFR);
		}	
}

void PORTC_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOC]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOC]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOC])
		{
        GPIO_CallBackTable[HW_GPIOC](ISFR);
		}	
}

void PORTD_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOD]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOD]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOD])
		{
        GPIO_CallBackTable[HW_GPIOD](ISFR);
		}	
}

void PORTE_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOE]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOE]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOE])
		{
        GPIO_CallBackTable[HW_GPIOE](ISFR);
		}	
}

#if (defined(MK70F12))
void PORTF_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOF]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOF]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOF])
		{
        GPIO_CallBackTable[HW_GPIOF](ISFR);
		}
}
#endif


//! @}

//! @}

//! @}