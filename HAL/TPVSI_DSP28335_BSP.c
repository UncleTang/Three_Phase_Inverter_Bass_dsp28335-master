/*
 * TPVSI_DSP28335_BSP.c
 *
 *  Created on: 2018��1��10��
 *      Author: Chuck Wang
 */
#include "TPVSI_DSP28335_BSP.h"

//HSPCLK = SYSCLKOUT/(2*ADC_MODCLK2)
#if (CPU_FRQ_150MHZ)
#define ADC_MODCLK 0x3
#endif
#if (CPU_FRQ_100MHZ)
#define ADC_MODCLK 0x2
#endif


#define ADC_CKPS 	0x1//ADC��ʱ��Ƶ�ʿ��Լ���ΪHSPCLK/(2*ADC_CKPS)
#define ADC_SHCLK 	0xf//�������ֿ��
#define AVG 		1000
#define ZOFFSET 	0x00
#define BUF_SIZE 	40

Uint16 DMA_Buf[14];//DMA����

/*��ʼ��ADC*/
void BSP_ADC_Init(void)
{
	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;
	AdcRegs.ADCTRL3.all = 0x00E0;//����ADC��Դ����׼
	DELAY_US(5000);//�ڿ�ʼת��֮ǰ��ʱһ��ʱ��
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;//ÿ�δ���ֻ����һ��
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;//����SEQ1�жϴ�������
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;//��λSEQ1
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;//����EPWMA����SEQ1����
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2 = 1;//����EPWMB����SEQ2����
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 7;//ADCA����8��ͨ����Ҫ����
	AdcRegs.ADCMAXCONV.bit.MAX_CONV2 = 5;//ADCB����6��ͨ����Ҫ����
#if (INTERNAL_REFERENCE)
	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x00;//�ڲ���׼
#endif
#if (EXTERNAL_REFERENCE)
	AdcRegs.ADCREFSEL.bit.REF_SEL = 0X01;//�ⲿ2.048V��׼
#endif
	/*���涨����ǲ�����˳��������Ҫ��Ķ�*/
	/*BEGIN*/
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x7;
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x8;
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;
	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0xA;
	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0xB;
	AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0xC;
	AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0xD;
	/*END*/
}


/*��ʼ��DMA*/
void BSP_DMA_Init(void)
{
	unsigned char ii;
	for(ii=0;ii<14;ii++)
	{
		DMA_Buf[ii] = 0;
	}
	EALLOW;
	DmaRegs.DMACTRL.bit.HARDRESET = 1;
	asm(" nop");//��DMA��λ����Ҫ��ͣһ��
	DmaRegs.DEBUGCTRL.bit.FREE = 1;//�ڷ�����ͣģʽ�£�DMA��Ȼ��������
	EDIS;
	DMACH1AddrConfig(DMA_Buf,&AdcMirror.ADCRESULT0);
	DMACH1BurstConfig(13,1,1);
	DMACH1TransferConfig(0,0,0);//ÿ�δ���һ�Σ�û�ж��Transfer���䣬��ʹ��Wrapģʽ
	DMACH1WrapConfig(0,0,0,0);
	DMACH1ModeConfig(DMA_SEQ1INT,PERINT_ENABLE,ONESHOT_ENABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,
					 OVRFLOW_DISABLE,SIXTEEN_BIT,CHINT_END,CHINT_DISABLE);//����SEQ1��Ϊ�����������ж�Դ��ONSHOTģʽ���Զ���λTransfer����ͬ����16λ�����ж�
	StartDMACH1();//����DMA
}

/*��ʼ��ePWM*/
void BSP_ePWM_Init(void)
{
	//ʱ������
	EALLOW;
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 3;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;
	//EPWM1
	EPwm1Regs.TBCTL.bit.FREE_SOFT = 0x00;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
	EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;//��EPWMģ����EPWM1ͬ��
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm1Regs.TBPRD = 3000;//150MHz/3000=25KHzPWM
	EPwm1Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
	EPwm1Regs.CMPB = 2500;
	//���Ĵ�����������Ӱ�ӼĴ���ģʽ
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	//AQ�Ĵ�������
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;
	EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;//EPWMA��EPWMB���߻���
	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;//��EPwm1��ΪԴ
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm1Regs.DBFED = 30;//200ns����
	EPwm1Regs.DBRED = 30;
	//EPWM2
	EPwm2Regs.TBCTL.bit.FREE_SOFT = 0x01;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
	EPwm2Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;//��EPWMģ����EPWM1ͬ��
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBPRD = 3000;//150MHz/3000=25KHzPWM
	EPwm2Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
	EPwm2Regs.CMPB = 1500;
	//���Ĵ�����������Ӱ�ӼĴ���ģʽ
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	//����AQ�Ĵ���
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm2Regs.AQCTLA.bit.PRD = AQ_SET;
	EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;
	EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;//EPWMA��EPWMB���߻���
	EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;//��EPwm1��ΪԴ
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm2Regs.DBFED = 30;//200ns����
	EPwm2Regs.DBRED = 30;
	//EPWM3
	EPwm3Regs.TBCTL.bit.FREE_SOFT = 0x01;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
	EPwm3Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;//��EPWMģ����EPWM1ͬ��
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm3Regs.TBPRD = 3000;//150MHz/3000=25KHzPWM
	EPwm3Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
	EPwm3Regs.CMPB = 1500;
	//���Ĵ�����������Ӱ�ӼĴ���ģʽ
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	//����AQ�Ĵ���
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm3Regs.AQCTLA.bit.PRD = AQ_SET;
	EPwm3Regs.AQCTLB.bit.CBU = AQ_SET;
	EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;//EPWMA��EPWMB���߻���
	EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;//��EPwm1��ΪԴ
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm3Regs.DBFED = 30;//200ns����
	EPwm3Regs.DBRED = 30;
	//��EPWM1������ADCת��
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;
	EPwm1Regs.ETSEL.bit.SOCBEN = 1;
	EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTR_ZERO;
	EPwm1Regs.ETPS.bit.SOCBPRD = ET_1ST;
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;//�ж�Դѡ��
	EPwm1Regs.ETSEL.bit.INTEN = 1;//ʹ���ж�
	EPwm1Regs.ETPS.bit.INTPRD = ET_2ND;//ÿ���δ���ִ��һ��

#if DEBUG_DAC_OUTPUT
	//DAC�������
	//EPWM4
	EPwm4Regs.TBCTL.bit.FREE_SOFT = 0x01;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
	EPwm4Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;//��EPWMģ����EPWM1ͬ��
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm4Regs.TBPRD = 3000;//75MHz/3000=25KHzPWM
	EPwm4Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
	EPwm4Regs.CMPB = 1500;
	//���Ĵ�����������Ӱ�ӼĴ���ģʽ
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	//����AQ�Ĵ���
	EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm4Regs.AQCTLA.bit.PRD = AQ_SET;

	//EPWM5
	EPwm5Regs.TBCTL.bit.FREE_SOFT = 0x01;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
	EPwm5Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;//��EPWMģ����EPWM1ͬ��
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm5Regs.TBPRD = 3000;//75MHz/3000=25KHzPWM
	EPwm5Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
	EPwm5Regs.CMPB = 1500;
	//���Ĵ�����������Ӱ�ӼĴ���ģʽ
	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	//����AQ�Ĵ���
	EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm5Regs.AQCTLA.bit.PRD = AQ_SET;

	//EPWM6
	EPwm6Regs.TBCTL.bit.FREE_SOFT = 0x01;//Ϊ�˱�֤��ȫ���ڷ�����ֹͣ��ʱ��Ӧ��֤��MOS�ܹض�
    EPwm6Regs.TBCTL.bit.CLKDIV = 0x0;//����Ƶ
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0x0;//TBCLK = SYSCLKOUT/(HSPCLKDIV x CLKDIV)
    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;//��EPWMģ����EPWM1ͬ��
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
    EPwm6Regs.TBPRD = 3000;//75MHz/3000=25KHzPWM
    EPwm6Regs.CMPA.half.CMPA = 1500;//��ʼ��50%ռ�ձ�
    EPwm6Regs.CMPB = 1500;
    //���Ĵ�����������Ӱ�ӼĴ���ģʽ
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    //����AQ�Ĵ���
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLA.bit.PRD = AQ_SET;
#endif
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;


}

/*����GPIO*/
void BSP_GPIO_Init(void)
{
	EALLOW;
	//����ΪEPWM�������
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0x01;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0x01;
	//������������Ҳ��֪��Ϊʲô�����������У�
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;
	//����GPIO������
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0x00;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 0x01;//����Ϊ���
	GpioDataRegs.GPASET.bit.GPIO24 = 0x01;
	GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0x00;
	GpioCtrlRegs.GPBDIR.bit.GPIO59 = 0x01;
	GpioDataRegs.GPBSET.bit.GPIO59 = 0x00;
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0x00;
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0x01;
	GpioDataRegs.GPBSET.bit.GPIO60 = 0x00;
	//GpioDataRegs.GPACLEAR.bit.GPIO24 = 0x01;\
	//Relay1
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0x00;
	GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0x01;//����Ϊ���
	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 0x01;//Close the relay during power-up
	//Relay2
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0x00;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0x01;//����Ϊ���
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = 0x01;//Close the relay during power-up
	//Relay3
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0x00;
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = 0x01;//����Ϊ���
	GpioDataRegs.GPBCLEAR.bit.GPIO58 = 0x01;//Close the relay during power-up
	//RST
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0x00;
	GpioCtrlRegs.GPADIR.bit.GPIO26 = 0x01;//����Ϊ���
	GpioDataRegs.GPACLEAR.bit.GPIO26 = 0x01;//Close the relay during power-up
	EDIS;
}

void BSP_Driver_Rst(void)
{
    EALLOW;
    GpioDataRegs.GPACLEAR.bit.GPIO26 = 0x01;
    DELAY_US(1000L);
    GpioDataRegs.GPASET.bit.GPIO26 = 0x01;
    EDIS;
}


void BSP_SCI_Init(void);
void BSP_eCAN_Init(void);


