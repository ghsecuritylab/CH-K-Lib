/**
  ******************************************************************************
  * @file    sys.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   ����K60�̼��� ϵͳ��API����
  ******************************************************************************
  */
#include "sys.h"
#include "libconfig.h"


//! @defgroup CH_Periph_Driver
//! @{
	
//! @defgroup SYS
//! @brief SYS driver modules
//! @{
	

//! @defgroup SYS_Exported_Functions
//! @{



/**
  * @brief  SystemClockSetup, change coreclock freq.other clock freq is also changed 
            according to there max limit.
            Not all core clock can be reached in any clock source option.
            We advised that use DisplayCPUInfo to check out.
  * @param  SYS_ClockSourceSelect_TypeDef 
            @arg kClockSource_IRC  : interal clock is used, run in FLL mode
            @arg kClockSource_EX8M : external 8M OSC is used,run in PLL mode
            @arg kClockSource_EX50M: external 50M OSC is used,run in PLL mode
  * @param  SYS_CoreClockSelect_TypeDef
            @arg kCoreClock_48M    : coreclock = 48MHz
            @arg kCoreClock_64M    : coreclock = 64MHz
            @arg kCoreClock_72M    : coreclock = 72MHz
            @arg kCoreClock_96M    : coreclock = 96MHz
            @arg kCoreClock_100M    : coreclock = 100MHz		
            @arg kCoreClock_200M    : coreclock = 200MHz					
  * @retval None
  */
/*
void SystemClockSetup(SYS_ClockSourceSelect_TypeDef clockSource, SYS_CoreClockSelect_TypeDef coreClock)
{
    //SIM_CLKDIV1_OUTDIV1(0) CORE     CLOCK    UP TO 100M  
    //SIM_CLKDIV1_OUTDIV2(1) BUS      CLOCK    UP TO 50M 
    //SIM_CLKDIV1_OUTDIV3(1) FlexBus  ClOCK    UP TO 50M 
    //SIM_CLKDIV1_OUTDIV4(3) Flash    ClOCK    UP TO 25M 
    if(clockSource == kClockSource_IRC)
    {
        SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu; //set all clock to lowest level
        // FEI mode
        MCG->C1 = (uint8_t)0x06u;
        MCG->C2 = (uint8_t)0x00u;
        MCG->C4 &= ~((1<<6)|(1<<7)|(1<<5));
        switch(coreClock)
        {
            case kCoreClock_96M:
                MCG->C4|= (1<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
                SystemCoreClock	= 96000000;
                break;
            case kCoreClock_72M:
                MCG->C4|= (1<<6)|(1<<7)|(0<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));	
                SystemCoreClock	= 72000000;
                break;
            case kCoreClock_48M:
                MCG->C4|= (0<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
                SystemCoreClock	= 48000000;
                break;
            default: //48M
                MCG->C4|= (0<<6)|(1<<7)|(1<<5);  
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
                SystemCoreClock	= 48000000;
                break;
        }
        MCG->C5 = (uint8_t)0x00u;
		    MCG->C6 = (uint8_t)0x00u; 
	    	while((MCG->S & MCG_S_IREFST_MASK) == 0u);  //check FLL's clock is come from IRClock
	    	while((MCG->S & 0x0Cu) != 0x00u);           //wait for FLL to be locked
    }
    if(clockSource == kClockSource_EX8M || clockSource == kClockSource_EX50M)
    {
        SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu; //set all clock to lowest level
        //FBE mode
        OSC->CR = (uint8_t)0x00u;
        SIM->SOPT2 &= (uint8_t)~(uint8_t)0x01u;
        MCG->C2 = (uint8_t)0x24u;
        MCG->C1 = (uint8_t)0x9Au;
        MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
        MCG->C5 = (uint8_t)0x03u; 
        MCG->C6 = (uint8_t)0x00u;
        while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//check FLL's clock is come from IRClock
        while((MCG->S & MCG_S_IREFST_MASK) != 0u); 
        while((MCG->S & 0x0Cu) != 0x08u);
        if(clockSource == kClockSource_EX8M)
        {
            MCG->C5 = (uint8_t)MCG_C5_PRDIV(3);        //8/4 = 2M
        }
        else if (clockSource == kClockSource_EX50M)
        {
            MCG->C5 = (uint8_t)MCG_C5_PRDIV(24);        //50/25 = 2M
        }
        switch(coreClock)
        {
            case kCoreClock_48M:
				        MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(0));
			        	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));	
			        	SystemCoreClock	= 48000000;
			        	break;		
            case kCoreClock_64M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(8));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));
                SystemCoreClock	= 64000000;
                break;		
            case kCoreClock_72M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(12));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(2));
                SystemCoreClock	= 72000000;
                break;			
            case kCoreClock_96M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(24));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));		
                SystemCoreClock	= 96000000;
                break;		
            case kCoreClock_100M:
                MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(26));
                SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));	
                SystemCoreClock	= 100000000;
                break;	
            case kCoreClock_200M:
                if(clockSource == kClockSource_EX8M)
                {
                    MCG->C5 = (uint8_t)MCG_C5_PRDIV(1);        //8/2 = 4M
                    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(26));
                    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
                }
                else if (clockSource == kClockSource_EX50M)
                {
                    MCG->C5 = (uint8_t)MCG_C5_PRDIV(12);
                    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(28));
                    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
                }
                SystemCoreClock	= 200000000;
                break;			
            default:
			        	MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV(0));
			        	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1));
		        		SystemCoreClock	= 48000000;
			        	break;
		    } 
        while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //wait for PLL
        while((MCG->S & MCG_S_LOCK_MASK) == 0u);    
	    	//turn in to PBE
	    	MCG->C1 = (uint8_t)0x1Au;
	    	while((MCG->S & 0x0Cu) != 0x0Cu);
        while((MCG->S & MCG_S_LOCK_MASK) == 0u);  
    }
		// recalculate all clock
    GetCPUInfo();
}
*/


/**
  * @brief  NVIC_Init, Set a interrupt priority. This function base on Cortex-M core. Independed from peripheral
  * @param  IRQn : IRQ number, located in MKxxxx.h
            PriorityGroup:
						@arg NVIC_PriorityGroup_0 : Group0
						@arg NVIC_PriorityGroup_1 : Group1
						@arg NVIC_PriorityGroup_2 : Group2
						@arg NVIC_PriorityGroup_3 : Group3
						@arg NVIC_PriorityGroup_4 : Group4
  * @param   PreemptPriority�� Preemptive priority value (starting from 0).
  * @param   SubPriority:      Subpriority value (starting from 0).
  * @retval None
  */
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority)
{
    //param check
    //assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
    //assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));  
    //assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
    //NVIC priority grouping
    NVIC_SetPriorityGrouping(PriorityGroup);
    //Set NVIC interrupt priority
    NVIC_SetPriority(IRQn,NVIC_EncodePriority(PriorityGroup,PreemptPriority,SubPriority));
}




/**
 * @brief  Set interrupt table offset
 * @param  offset
 * @retval none
 */
void SetVectorTable(uint32_t VectorOffset)
{
    //param check
    //assert_param(IS_VECTOR_OFFSET(VectorOffset));
    SCB->VTOR = VectorOffset;  // �����ж�������ƫ�Ƶ�ַ
}
/**
 * @brief  Get Fwlib Version
 * @param  None
 * @retval FwLib Version code
 */
uint16_t GetFWVersion(void)
{
    return(CHK_VERSION);
}

//! @}

//! @}

//! @}
