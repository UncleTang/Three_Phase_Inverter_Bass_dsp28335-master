/*
 * TPVSI_DSP28335_HAL.c
 *
 *  Created on: 2018��3��10��
 *      Author: wangchu
 */
#include "TPVSI_DSP28335_HAL.h"
#include "C28x_FPU_FastRTS.h"


EPWM_Structure _p_epwm1;
EPWM_Structure _p_epwm2;
EPWM_Structure _p_epwm3;

#if DEBUG_DAC_OUTPUT
EPWM_Structure _p_epwm4;
EPWM_Structure _p_epwm5;
EPWM_Structure _p_epwm6;
#endif
Sin_Structure _p_sin1;
Sample_Structure p_sam;

#pragma CODE_SECTION(HAL_Sample,"ramfuncs");


void HAL_Init(void)
{
	HAL_EPWM_Structure_Init();
	HAL_Sample_Init(&p_sam);
	BSP_GPIO_Init();
	BSP_ADC_Init();
	BSP_ePWM_Init();
	BSP_DMA_Init();
	BSP_Driver_Rst();
}
void HAL_EPWM_Structure_Init(void)
{
	_p_epwm1.fre = 50000;
	_p_epwm1.bias = 1500;
	_p_epwm1.cmp_max = 3000;
	_p_epwm1.cmp_min = 0;
	_p_epwm1.cmpa_value = 1500;
	_p_epwm1.cmpb_value = 1500;
	_p_epwm2.fre = 50000;
	_p_epwm2.bias = 1500;
	_p_epwm2.cmp_max = 3000;
	_p_epwm2.cmp_min = 0;
	_p_epwm2.cmpa_value = 1500;
	_p_epwm2.cmpb_value = 1500;
	_p_epwm3.fre = 50000;
	_p_epwm3.bias = 1500;
	_p_epwm3.cmp_max = 3000;
	_p_epwm3.cmp_min = 0;
	_p_epwm3.cmpa_value = 1500;
	_p_epwm3.cmpb_value = 1500;
	_p_epwm4.fre = 50000;
	_p_epwm4.bias = 1500;
	_p_epwm4.cmp_max = 3000;
	_p_epwm4.cmp_min = 0;
	_p_epwm4.cmpa_value = 1500;
	_p_epwm4.cmpb_value = 1500;
	_p_epwm5.fre = 50000;
	_p_epwm5.bias = 1500;
	_p_epwm5.cmp_max = 3000;
	_p_epwm5.cmp_min = 0;
	_p_epwm5.cmpa_value = 1500;
	_p_epwm5.cmpb_value = 1500;
	_p_epwm6.fre = 50000;
    _p_epwm6.bias = 1500;
    _p_epwm6.cmp_max = 3000;
    _p_epwm6.cmp_min = 0;
    _p_epwm6.cmpa_value = 1500;
    _p_epwm6.cmpb_value = 1500;
}

#pragma CODE_SECTION(HAL_PWM_DutyValue_Cal,"ramfuncs");
void HAL_PWM_DutyValue_Cal(EPWM_Structure *ep,float duty,float duty_max,float duty_min)
{
	Uint16 temp;
	if(duty>duty_max) duty = duty_max;
	else if(duty<duty_min) duty = duty_min;
	temp = (Uint16)(duty*((ep->cmp_max) - (ep->cmp_min))/2.0 + (ep->bias));
	if(temp>(ep->cmp_max)) temp = ep->cmp_max;
	else if (temp<(ep->cmp_min)) temp = ep->cmp_min;
	ep->cmpa_value = temp;
}

/*
 * FunName:HAL_Sample_Init
 * Description:��ʼ�������ṹ�壬ϵ��k��������˲������ݵ���ʵ����ת����ϵ�����������ĳ�ʼ��Ӧ��������ݵĶ��ٶ���
 * Input:�����ṹ��ָ��p
 * Output:None
 * */
void HAL_Sample_Init(Sample_Structure *p)
{
	Uint8 i;
	for(i = 0;i<SAMPLE_NUM;i++)
	{
		p->k[i] = 1;
	}
	/*TODO:�����ﶨ�������ϵ��*/
	p->k[0] = 0.0690;
	p->k[1] = 0.0690;
	p->k[2] = 0.0690;
	p->k[3] = -1*7.033e-3;
	p->k[4] = -1*7.033e-3;
	p->k[5] = -1*7.033e-3;
	p->k[CapVoltageA] = 0.0690;
	p->k[CapVoltageB] = 0.0690;
	p->k[CapVoltageC] = 0.0690;
	/*�����ʼ����������*/
	for(i=0;i<SAMPLE_NUM;i++)
	{
		p->data[i] = 0;
	}
}

#pragma CODE_SECTION(HAL_Sample,"ramfuncs");
#pragma CODE_SECTION(HAL_DutySet,"ramfuncs");
/*
 * FunName:HAL_Sample
 * Description:�Բ������ݽ��д���Դ��������DMA����������DMA_Buf��
 * 			   DMA_Buf��һ��ȫ�ֱ�����������TPVSI_DSP28335_BSP.c��
 * Input:�����ṹ��ָ��p
 * Output:None
 * Others:None
 * */
void HAL_Sample(Sample_Structure *p)
{
	static int zero[SAMPLE_NUM];
	Uint8 i = 0;
	for(i = 0;i<SAMPLE_NUM;i++)
	{
		zero[i] = 0;
	}
	/*TODO�������ﶨ��������ƫ����*/
	zero[0] = 1949;
	zero[1] = 1945;
	zero[2] = 1945;
	zero[3] = 1560;
	zero[4] = 1541;
	zero[5] = 1560;
	zero[CapVoltageA] = 1956;
	zero[CapVoltageB] = 1950;
	zero[CapVoltageC] = 1950;
	/*����������������*/
	for(i = 0;i<SAMPLE_NUM;i++)
	{
		p->data[i] = p->k[i]*(float)((int)DMA_Buf[i] - zero[i]);
	}
}



/*
 * FunName:HAL_DutySet
 * Description:����ɿ��Ʊ����ṹ��ָ����ռ�ձ�
 * Input:���Ʊ����ṹ��ָ��p��EPWMģ��ṹ��ָ��epa,epb,epc
 * Output:None
 * Others:None
 * */
void HAL_DutySet(ThreePhase_Data_Structure *p,EPWM_Structure *epa,EPWM_Structure *epb,EPWM_Structure *epc)
{
	HAL_PWM_DutyValue_Cal(epa,p->abc_data[index_a],0.9,-0.9);
	HAL_PWM_DutyValue_Cal(epb,p->abc_data[index_b],0.9,-0.9);
	HAL_PWM_DutyValue_Cal(epc,p->abc_data[index_c],0.9,-0.9);
	EPwm1Regs.CMPA.half.CMPA = epa->cmpa_value;
	EPwm2Regs.CMPA.half.CMPA = epb->cmpa_value;
	EPwm3Regs.CMPA.half.CMPA = epc->cmpa_value;
}

/*
 * FunName:HAL_GetSampleResult
 * Description:��ȡ����ֵ
 * Input:��������index.
 * Output:None
 * Others:None
 * */
float HAL_GetSampleResult(Uint8 index)
{
	if(index >= SAMPLE_NUM) return 0;
	return (p_sam.data[index]);
}


/*
 * FunName:HAL_EPWM_DISABLE
 * Description:�ص�����EPWM���
 * Input:None
 * Output:None
 * Others:None
 * */
void HAL_EPWM_DISABLE(void)
{
	EALLOW;
	//�ص�����EPWM���
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0x00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0x00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0x00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0x00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0x00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0x00;
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 0x01;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 0x01;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 0x01;
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 0x01;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 0x01;
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 0x01;
	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
	EDIS;
}

/*
 * FunName:HAL_Relay_On
 * Description:�����м̵���
 * Input:None
 * Output:None
 * Others:None
 * */
void HAL_Relay_On(void)
{
    EALLOW;
    GpioDataRegs.GPBSET.bit.GPIO33 = 0x01;
    GpioDataRegs.GPBSET.bit.GPIO49 = 0x01;
    GpioDataRegs.GPBSET.bit.GPIO58 = 0x01;
    EDIS;
}


#if _GRAPH_DISPLAY_
float display_buffer[512];
#pragma CODE_SECTION(HAL_PUSH_BUFFER,"ramfuncs");
/*
 * FunName:HAL_PUSH_BUFFER
 * Description:���������ݴ�����ʾ��������
 * Input:����x
 * Output:None
 * */
void HAL_PUSH_BUFFER(float x)
{
    Uint8 i;
    for(i=1;i<512;i++)
    {
        display_buffer[i] = display_buffer[i-1];
    }
    display_buffer[0] = x;
}
#endif
