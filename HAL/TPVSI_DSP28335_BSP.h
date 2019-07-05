/*
 * TPVSI_DSP28335_BSP.h
 *
 *  Created on: 2018��1��10��
 *      Author: Chuck Wang
 */

#ifndef TPVSI_DSP28335_BSP_H_
#define TPVSI_DSP28335_BSP_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_DMA.h"
#include "DSP2833x_EPwm.h"

void BSP_ADC_Init(void);
void BSP_ePWM_Init(void);
void BSP_GPIO_Init(void);
void BSP_SCI_Init(void);
void BSP_eCAN_Init(void);
void BSP_DMA_Init(void);
void BSP_Driver_Rst(void);

#define INTERNAL_REFERENCE 1
#define EXTERNAL_REFERENCE 0
//��������ĺ�Ϊ1���Դ�DAC������ܣ������ڵ��Ե�ʱ��۲첨��
#define DEBUG_DAC_OUTPUT 1

extern Uint16 DMA_Buf[14];
#endif /* TPVSI_DSP28335_BSP_H_ */
