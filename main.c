/*
 * Title:3 Phase VSI Base Program
 * Author:Chuck Wang
 * Description:�ṩ�������������Ҫ�Ļ�������,��Ҫ����:
 * A���弶Ӳ����ʼ�����򣬰����弶Ӳ����ʼ��������������TPVSI_DSP28335_BSP.c��
 * B���жϳ��򣬰�����������Ҫ���жϺ�����������TPVSI_DSP28335_BSP.c��
 * main.c
 */


#include "DSP28x_Project.h"


/*ȫ�ֱ�������*/
int main(void)
{
	memcpy((Uint16 *)&RamfuncsRunStart,(Uint16 *)&RamfuncsLoadStart, (unsigned long)&RamfuncsLoadSize);
	InitSysCtrl();
	DINT;
	/*���治Ҫ�޸�*/
	/*Begin*/
	//�ж�����
	InitPieCtrl();
	//��������жϱ�־λ
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
	/*End*/
	/*TODO:����ָ���жϺ���*/
	EALLOW;
	PieVectTable.EPWM1_INT = &epwm1_isr;
	EDIS;
	/*TODO:�����ʼ��Ӳ���������㷨�����*/
//	HAL_EPWM_DISABLE();
	HAL_Init();
	AAL_Trans_Init();
	AAL_PLL_Init();
	AAL_Control_ThirdOrderControllerInit();
	HAL_Relay_On();
	/*TODO:���������ж�Դ*/
	IER |= M_INT3;
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	/*���濪���жϣ���Ҫ�޸�*/
	/*Begin*/
	EINT;
	ERTM;
	/*End*/
	for(;;)
	{

	}
	return 0;
}
