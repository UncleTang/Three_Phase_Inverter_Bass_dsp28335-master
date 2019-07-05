/*
 * TPVSI_DSP28335_Structure_Define.h
 *
 *  Created on: 2018��3��12��
 *      Author: wangchu
 */

#ifndef TPVSI_DSP28335_STRUCTURE_DEFINE_H_
#define TPVSI_DSP28335_STRUCTURE_DEFINE_H_


/*���·��ADCͨ����Ӧ�����ݵĺ���*/
typedef enum{
	VoltageA = 0,
	VoltageB,
	VoltageC,
	CurrentA,
	CurrentB,
	CurrentC,
	VoltageDC,
	CurrentDC,
	CapVoltageA,
	CapVoltageB,
	CapVoltageC,
	InvCurrentA,
	InvCurrentB,
	InvCurrentC,
	SAMPLE_NUM,//���ö�ٱ�������������б�����������Ա�ʶ�������ݵĸ���
}Sample_Index;


/*
 * StructureName:EPWM_Structure
 * Description:EPWMģ��ĳ���ṹ�塣
 * Elements:
 * 	cmp_max->�ȽϼĴ�����������ֵ
 * 	cmp_min->�ȽϼĴ����������Сֵ
 * 	fre->EPWMģ��Ĺ���Ƶ��
 * 	bias->���ȽϼĴ�����ֵʱ��ƫ����
 * 	cmpa_value->�ȽϼĴ���a��ֵ
 * 	cmpb_value->�ȽϼĴ���b��ֵ
 * */
typedef struct{
	Uint16 cmp_max;
	Uint16 cmp_min;
	Uint16 fre;
	Uint16 bias;
	Uint16 cmpa_value;
	Uint16 cmpb_value;
} EPWM_Structure;

/*
 * StructureName:Sample_Structure
 * Description:����ģ��ĳ���ṹ�塣
 * Elements:
 * 	data->�������ݣ�
 * 	k->����ֵ��ʵ�����ݵ�ת��ϵ��������ADC�����õ���ֵΪx(0<=x<=4095)����ʵ������Ϊk*x
 * */
typedef struct{
	float data[SAMPLE_NUM];//����
	float k[SAMPLE_NUM];//ADCת���ı���ϵ��
} Sample_Structure;


/*
 * StructureName:Sin_Structure
 * Description:Sine����ֵ�ṹ�塣
 * Elements:
 * 	Angle->��ǰ�Ƕ�
 * 	sin_value->��ǰ�Ƕȵ�����ֵ
 * */
typedef struct{
  float Angle;
  float sin_value;
} Sin_Structure;

/*
 * StructureName:ThreePhase_Data_Structure
 * Description:�����������ݵĽṹ�壬����������ݷֱ���abc��dq0������0�����������е������д��棬�����ݵ�����ֵ�ο�ö�ٱ�������
 * Elements:
 * 	abc_data->abc�����ݣ�
 *	angle->�Ƕ�����;
 *	dq0_data->dq0����������;
 *	alphabeta0_data->����0����������
 *	clarke_p->��Solar���clark�任�Ľӿڽṹ�壬��Ӧ���ⲿ����
 *	park_p->��Solar���park�任�Ľӿڽṹ�壬��Ӧ���ⲿ����
 *	iclarke_p->��Solar���iclark�任�Ľӿڽṹ�壬��Ӧ���ⲿ����
 *	ipark_p->��Solar���ipark�任�Ľӿڽṹ�壬��Ӧ���ⲿ����
 * */
typedef struct{
	float abc_data[3];
	float angle;
	float dq0_data[3];
	float alphabeta0_data[3];
	CLARKE_F clarke_p;
	PARK_F park_p;
	iCLARKE_F iclarke_p;
	iPARK_F ipark_p;
}ThreePhase_Data_Structure;


/*
 * StructureName:PID_Structure
 * Description:PID�������ṹ��.
 * Elements:
 * 	kp->����ϵ��
 * 	ki->����ϵ��
 * 	Ts->����ʱ��
 * 	output->�������
 * 	err->������
 * 	err_last->��һ�ο���ʱ��������
 * 	output_last->��һ�ο���ʱ���������
 * */
typedef struct{
	float kp;
	float ki;
	float Ts;
	float output;
	float err;
	float err_last;
	float output_last;
}PID_Structure;

/*
 * StructureName:PID_Structure
 * Description:PID�������ṹ��.
 * Elements:
 * 	y2->y(n-2)
 * 	y1->y(n-1)
 * 	y->y(n)
 * 	x2->x(n-2)
 * 	x1->x(n-1)
 * 	x->x(n)
 * */
typedef struct{
	float x[4];
	float y[4];
}ThirdOrder_Controller_Structure;


/*�������ݽṹ������ö�ٶ���*/
/*0->a,1->b,2->c*/
typedef enum{
	index_a = 0,
	index_b,
	index_c,
}abc_Index;

/*0->0,1->d,2->q*/
typedef enum{
	index_d = 1,
	index_q,
}dq0_index;

/*0->0,1->��,2->��*/
typedef enum{
	index_alpha = 1,
	index_beta,
}alphabeta0_index;


#endif /* TPVSI_DSP28335_STRUCTURE_DEFINE_H_ */
