/*
 * TPVSI_DSP28335_AAL_Control.c
 *
 *  Created on: 2018��3��12��
 *      Author: Chuck
 */

#include "TPVSI_DSP28335_AAL_Control.h"




ThirdOrder_Controller_Structure PR_ControllerA;
ThirdOrder_Controller_Structure PR_ControllerB;
ThirdOrder_Controller_Structure PR_ControllerC;

/*
 * FunName:AAL_Control_PID_DeInit
 * Description:��PID�ṹ���ʼ��Ϊȱʡֵ
 * Input:PID�ṹ��ָ��p
 * Output:None
 * Others:None
 * */
void AAL_Control_PID_DeInit(PID_Structure *p)
{
	p->Ts = 0;
	p->err = 0;
	p->err_last = 0;
	p->ki = 0;
	p->kp = 0;
	p->output = 0;
	p->output_last = 0;
}

/*
 * FunName:AAL_Control_PID_Init
 * Description:��PID�ṹ�尴�ո���������ʼ��
 * Input:PID�ṹ��ָ��p,����ʱ��Ts,����ϵ��kp,����ϵ��ki
 * Output:None
 * Others:None
 * */
void AAL_Control_PID_Init(PID_Structure *p,float Ts,float kp,float ki)
{
	AAL_Control_PID_DeInit(p);
	p->Ts = Ts;
	p->kp = kp;
	p->ki = ki;
}


/*
 * FunName:AAL_Control_PID
 * Description:��ѹ�ջ�pid����
 * Input:PID�ṹ��ָ��p,�������err
 * Output:None
 * Others:None
 * */
void AAL_Control_PID(PID_Structure *p,float err)
{
	//��������
	p->err_last = p->err;
	p->err = err;
	p->output_last = p->output;
	//����PID��ŷ������
	p->output = p->output_last + p->kp*(p->err - p->err_last) + p->ki*p->Ts*p->err;
	if(p->output>1)
		p->output = 0.99;
	else if(p->output<-1)
		p->output = -0.99;
}


void AAL_Control_ThirdOrderControllerInit(void)
{
	int i = 0;
	for(i = 0;i<4;i++)
	{
		PR_ControllerA.x[i] = 0;
		PR_ControllerA.y[i] = 0;
		PR_ControllerB.x[i] = 0;
		PR_ControllerB.y[i] = 0;
		PR_ControllerC.x[i] = 0;
		PR_ControllerC.y[i] = 0;
	}
}
void AAL_Control_ThirdOrderController(ThirdOrder_Controller_Structure *p,float x)
{
	float temp;
	p->x[0] = x;
	//PR������
//	p->y[0] = 1.999*(p->y[1]) - 0.9996*(p->y[2]) + 0.653*(p->x[0]) - 1.2656*(p->x[1]) + 0.61278*(p->x[2]);
	//Hinf������
	temp = 1.563*(p->y[1]) - 0.1267*(p->y[2]) - 0.4366*(p->y[3])
			+ 0.2849*(p->x[0]) - 0.2615*(p->x[1]) - 0.2842*(p->x[2]) + 0.262*(p->x[3]);
	if(temp>10)
		temp = 10;
	if(temp<-10)
		temp = -10;
	p->y[0] = temp;
	//��������
	p->x[3] = p->x[2];
	p->x[2] = p->x[1];
	p->x[1] = p->x[0];
	p->y[3] = p->y[2];
	p->y[2] = p->y[1];
	p->y[1] = p->y[0];
}

void AAL_Control_CurrentLoop(ThreePhase_Data_Structure *p_i,
		ThreePhase_Data_Structure *p_icap,ThreePhase_Data_Structure *p_con,
		ThreePhase_Data_Structure *p_target)
{
	//PR������
	AAL_Control_ThirdOrderController(&PR_ControllerA,
				(p_target->abc_data[index_a] - p_i->abc_data[index_a]));
	AAL_Control_ThirdOrderController(&PR_ControllerB,
				(p_target->abc_data[index_b] - p_i->abc_data[index_b]));
	AAL_Control_ThirdOrderController(&PR_ControllerC,
				(p_target->abc_data[index_c] - p_i->abc_data[index_c]));
	//������Դ����
	p_con->abc_data[index_a] = 1.26*((PR_ControllerA.y[0])- (p_icap->abc_data[index_a]));
	p_con->abc_data[index_b] = 1.26*((PR_ControllerB.y[0])- (p_icap->abc_data[index_b]));//1.26
	p_con->abc_data[index_c] = 1.26*((PR_ControllerC.y[0])- (p_icap->abc_data[index_c]));//ʵ������Ϊԭ��Դ����ϵ������1.5
	//�͵�ѹ���Ե�ʱ������ϵ��Ϊ1.5*1.1667
}
