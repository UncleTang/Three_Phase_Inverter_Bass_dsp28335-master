/*
 * TPVSI_DSP28335_AAL_Math.c
 *
 *  Created on: 2018��3��15��
 *      Author: Chuck
 */
#include "TPVSI_DSP28335_AAL_Math.h"


/*
 * FunName:AAL_FirstOrder_Diff
 * Description:һ�ײ�ֺ���
 * Input:��ǰ����ֵ
 * Output:����һ�ײ��ֵ
 * Others:None
 * */
float AAL_FirstOrder_Diff(float data)
{
	static float diff_last_data;
	float temp;
	temp = data - diff_last_data;
	diff_last_data = data;
	return temp;
}


