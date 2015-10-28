#ifndef _MAIN_H_
#define _MAIN_H_

#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "cpuidy.h"
#include "i2c.h"
#include "dma.h"

/*-*-* ���к꿪�� *-*-*/

#define DEBUG_LOG//����˺꿪�����ڴ�ӡ�����д�ӡ����ʹ��LOG()

#ifdef DEBUG_LOG
#define LOG(format,args...)  printf(format,##args)
#else //DEBUG_LOG
#define LOG(format,args...)
#endif //DEBUG_LOG

#endif //_MAIN_H_
