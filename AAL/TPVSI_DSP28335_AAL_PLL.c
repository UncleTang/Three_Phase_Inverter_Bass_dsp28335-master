/*
 * TPVSI_DSP28335_AAL_PLL.c
 *
 *  Created on: 2018��3��16��
 *      Author: Chuck
 */
#include "TPVSI_DSP28335_AAL_PLL.h"


#define GRID_FREQ 50//����Ƶ��
#define ISR_FREQUENCY 25000//�ж�Ƶ��
#define NORMALIZE_COE 0.025//������ֵ��һ����ϵ��

//PLL�ṹ�嶨��
SPLL_3ph_SRF_F spll1;

/*
 * FunName:AAL_PLL_Init
 * Description:PLL�ṹ���ʼ������,���ú궨�巽ʽ�����������
 * Input:None
 * Output:None
 * Others:None
 * */
void AAL_PLL_Init(void)
{
	SPLL_3ph_SRF_F_init(GRID_FREQ,((float)(1.0/ISR_FREQUENCY)),&spll1);
}


/*
 * FunName:AAL_PLL_Cal
 * Description:PLL����
 * Input:�������ݽṹ��ָ�� p
 * Output:None
 * Others:None
 * */
void AAL_PLL_Cal(ThreePhase_Data_Structure *p)
{
	spll1.v_q[0] = (p->dq0_data[index_q])*NORMALIZE_COE;
	SPLL_3ph_SRF_F_MACRO(spll1);
}

/*
 * FunName:AAL_PLL_GetPhase
 * Description:��ȡPLL��ǰ����ĽǶ�ֵ
 * Input:None
 * Output:None
 * Others:None
 * */
float AAL_PLL_GetPhase(void)
{
	return (spll1.theta[0]);
}



