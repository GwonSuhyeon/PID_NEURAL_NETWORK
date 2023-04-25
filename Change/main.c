

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_SIZE 500			// �ϳ��� �����ͼ��� ������ ����
#define BATCH_SIZE 50			// �����ͼ��� BATCH_SIZE��ŭ�� ������ �ѹ��� ����Ŭ ���� (DATA_SIZE / BATCH_SIZE)��ŭ ����ġ ������Ʈ ��
#define EPOCH 1					// �н�(�� ����Ŭ)�� �� EPOCH��ŭ �ݺ���
#define LEARNING_RATE 1e-3		// �н���

#define INPUT_NEURON_NUMBER 2	// InputLayer�� ���� ����
#define HIDDEN_NEURON_NUMBER 3	// HiddenLayer�� ���� ����

#define SET_VALUE_FILE_NAME "r_data2.txt"	// ��ǥ�� �н������� ����
#define TRAIN_FILE_NAME "y_data2.txt"		// ������ �н������� ����


void Learn();

float Foreward_Prop(float, float, int);		// ������
void Backward_Prop(int);					// ������

float P_Activation_Func(float);				// P���� Ȱ��ȭ �Լ�
float I_Activation_Func(float, float);		// I���� Ȱ��ȭ �Լ�
float D_Activation_Func(float, float);		// D���� Ȱ��ȭ �Լ�

void Error_Message(char []);


/*
typedef enum Input_Neuron_Type
{
	NEURON_TYPE_R = 0,
	NEURON_TYPE_Y = 1,

	NEURON_TYPE_P = 0,
	NEURON_TYPE_I = 1,
	NEURON_TYPE_D = 2

}Input_Neuron_Type;


typedef struct Neuron
{
	float u;	// ������ �Է� ����
	float x;	// ������ ��� ����

}Layer;

Layer InputLayer[INPUT_NEURON_NUMBER][DATA_SIZE];
Layer HiddenLayer[HIDDEN_NEURON_NUMBER][DATA_SIZE];
Layer OutputLayer[DATA_SIZE];
*/

float r[DATA_SIZE] = { 0 };			// ��ǥ�� �н�������
float train_y[DATA_SIZE] = { 0 };		// ������ �н�������

float w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// �Է� ���̾�� ���� ���̾� ������ ����ġ   1���� r�� ����ġ, 2���� y�� ����ġ
float w2[3] = { 0.5, 0.3, 0.2 };				// ���� ���̾�� ��� ���̾� ������ ����ġ     [0] : P���, [1] : I���, [2] : D���
												// �ʱ� ����ġ�� ���۷�-���ݽ� ������� ����� P, I, D ������ �ʱ�ȭ ��

float v[DATA_SIZE] = { 0 };	// �ý���(ex. ����)�� �Է����� ���� ��




float i_u1[DATA_SIZE] = { 0 };	// Input Layer���� ��ǥ��(r) �Է� ������ �Է� ����
float i_u2[DATA_SIZE] = { 0 };	// Input Layer���� ������(y) �Է� ������ �Է� ����

float i_x1[DATA_SIZE] = { 0 };	// Input Layer���� ��ǥ��(r) �Է� ������ ��� ����
float i_x2[DATA_SIZE] = { 0 };	// Input Layer���� ������(y) �Է� ������ ��� ����

float h_u1[DATA_SIZE] = { 0 };	// Hidden Layer���� P������ �Է� ����
float h_u2[DATA_SIZE] = { 0 };	// Hidden Layer���� I������ �Է� ����
float h_u3[DATA_SIZE] = { 0 };	// Hidden Layer���� D������ �Է� ����

float h_x1[DATA_SIZE] = { 0 };	// Hidden Layer���� P������ ��� ����
float h_x2[DATA_SIZE] = { 0 };	// Hidden Layer���� I������ ��� ����
float h_x3[DATA_SIZE] = { 0 };	// Hidden Layer���� D������ ��� ����

float o_u[DATA_SIZE] = { 0 };		// Output Layer�� �Է� ����
float o_x[DATA_SIZE] = { 0 };		// Output Layer�� ��� ����


void main(void)
{
	FILE* pSetValue_fp = NULL;
	FILE* pTrainValue_fp = NULL;

	int pSetValue_fp_idx;
	int pTrainValue_fp_idx;


	// ��ǥ��(SetValue) ������ �ε�
	pSetValue_fp = fopen(SET_VALUE_FILE_NAME, "r");

	if (pSetValue_fp == NULL)
	{
		Error_Message("file open fail - main function - r_data.txt");
		return;
	}

	pSetValue_fp_idx = 0;
	while (fscanf(pSetValue_fp, "%f\n", &r[pSetValue_fp_idx]) != EOF)
	{
		pSetValue_fp_idx++;
	}

	if (pSetValue_fp != NULL)
	{
		fclose(pSetValue_fp);
	}


	// ������(TrainValue) ������ �ε�
	pTrainValue_fp = fopen(TRAIN_FILE_NAME, "r");
	if (pTrainValue_fp == NULL)
	{
		Error_Message("file open fail - main function - y_data.txt");
		return;
	}

	pTrainValue_fp_idx = 0;
	while (fscanf(pTrainValue_fp, "%f\n", &train_y[pTrainValue_fp_idx]) != EOF)
	{
		pTrainValue_fp_idx++;
	}

	if (pTrainValue_fp != NULL)
	{
		fclose(pTrainValue_fp);
	}

	
	printf("BATCH SIZE : %d, EPOCH : %d, LEARNING RATE : %f\n", BATCH_SIZE, EPOCH, LEARNING_RATE);
	printf("FILE : %s, %s\n", SET_VALUE_FILE_NAME, TRAIN_FILE_NAME);
	printf("\n�н� ����\n\n");

	Learn();

	printf("\n�н� ����\n\n");
	printf("P : %f, I : %f, D : %f\n", w2[0], w2[1], w2[2]);

	return;
}

void Learn()
{
	for (int epoch = 0; epoch < EPOCH; epoch++)
	{
		for (int i = 0; i < DATA_SIZE; i++)
		{
			//InputLayer[NEURON_TYPE_R][i].u = r[i];
			i_u1[i] = r[i];
			//InputLayer[NEURON_TYPE_Y][i].u = train_y[i];
			i_u2[i] = train_y[i];

			//InputLayer[NEURON_TYPE_R][i].x = InputLayer[NEURON_TYPE_R][i].u;
			i_x1[i] = i_u1[i];
			//InputLayer[NEURON_TYPE_Y][i].x = InputLayer[NEURON_TYPE_Y][i].u;
			i_x2[i] = i_u2[i];


			//OutputLayer[i].u = Foreward_Prop(InputLayer[NEURON_TYPE_R][i].x, InputLayer[NEURON_TYPE_Y][i].x, i);
			o_u[i] = Foreward_Prop(i_x1[i], i_x2[i], i);
			//OutputLayer[i].x = OutputLayer[i].u;
			o_x[i] = o_u[i];
			//v[i] = OutputLayer[i].x;
			v[i] = o_x[i];

			// ��ġ������� ������ ���Ѽ� ����ġ�� ������Ʈ ��
			if (i % BATCH_SIZE == 0 && i > 0)
			{
				Backward_Prop(i);
			}
		}
	}


	return;
}




float Foreward_Prop(float setValue, float inputValue, int idx)
{
	float res = 0.0;

	//i_x1[idx] = setValue;
	//i_x2[idx] = inputValue;

	// �Է� ���̾�� ���� ���̾� ������ ����
	//HiddenLayer[NEURON_TYPE_P][idx].u = w1[0][0] * setValue + w1[1][0] * inputValue;
	//HiddenLayer[NEURON_TYPE_I][idx].u = w1[0][1] * setValue + w1[1][1] * inputValue;
	//HiddenLayer[NEURON_TYPE_D][idx].u = w1[0][2] * setValue + w1[1][2] * inputValue;
	h_u1[idx] = w1[0][0] * i_x1[idx] + w1[1][0] * i_x2[idx];
	h_u2[idx] = w1[0][1] * i_x1[idx] + w1[1][1] * i_x2[idx];
	h_u3[idx] = w1[0][2] * i_x1[idx] + w1[1][2] * i_x2[idx];
	

	// ���� ���̾��� �� �������� Ȱ��ȭ �Լ� ����
	//HiddenLayer[NEURON_TYPE_P][idx].x = P_Activation_Func(HiddenLayer[NEURON_TYPE_P][idx].u);
	//HiddenLayer[NEURON_TYPE_I][idx].x = I_Activation_Func(HiddenLayer[NEURON_TYPE_I][idx].u, HiddenLayer[NEURON_TYPE_I][idx - 1].x);
	//HiddenLayer[NEURON_TYPE_D][idx].x = D_Activation_Func(HiddenLayer[NEURON_TYPE_D][idx].u, HiddenLayer[NEURON_TYPE_D][idx - 1].u);
	h_x1[idx] = P_Activation_Func(h_u1[idx]);
	h_x2[idx] = I_Activation_Func(h_u2[idx], h_x2[idx - 1]);
	h_x3[idx] = D_Activation_Func(h_u3[idx], h_u3[idx - 1]);


	// �� ���ܿ� ���ؼ� ���� ����
	//res = HiddenLayer[NEURON_TYPE_P][idx].x * w2[0] + HiddenLayer[NEURON_TYPE_I][idx].x * w2[1] + HiddenLayer[NEURON_TYPE_D][idx].x * w2[2];
	res = h_x1[idx] * w2[0] + h_x2[idx] * w2[1] + h_x3[idx] * w2[2];
	
	return res;
}


void Backward_Prop(int idx)
{
	float error = 0.0;	// ����(�ð�) t ������ ������(��ǥ�� - ������)
	float y_rate = 0.0;	// ���� t�� t-1������ y�� ��ȭ��
	float v_rate = 0.0;	// ���� t�� t-1������ v�� ��ȭ��

	// P, I, D ������ �� ����ġ���� ��ȭ��
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;


	// w2 ��Ŀ� ���� ����ϰ� ����
	for (int i = idx - BATCH_SIZE; i < idx; i++)
	{
		error = r[i] - train_y[i];
		y_rate = train_y[i] - train_y[i - 1];
		v_rate = v[i] - v[i - 1];
		
		//x1_gradient += error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_P][i].x;
		//x2_gradient += error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_I][i].x;
		//x3_gradient += error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_D][i].x;
		x1_gradient += error * (y_rate / v_rate) * h_x1[i];
		x2_gradient += error * (y_rate / v_rate) * h_x2[i];
		x3_gradient += error * (y_rate / v_rate) * h_x3[i];
	}
	
	x1_gradient *= -2;
	x2_gradient *= -2;
	x3_gradient *= -2;
	
	x1_gradient /= BATCH_SIZE;
	x2_gradient /= BATCH_SIZE;
	x3_gradient /= BATCH_SIZE;
	// w2 ��Ŀ� ���� ����ϰ� ����
	
	w2[0] += LEARNING_RATE * x1_gradient;
	w2[1] += LEARNING_RATE * x2_gradient;
	w2[2] += LEARNING_RATE * x3_gradient;



	return;
}



float P_Activation_Func(float currentValue)
{
	return currentValue;
}



float I_Activation_Func(float currentValue, float accrueValue)
{
	return accrueValue + currentValue;
}



float D_Activation_Func(float currentValue, float pastValue)
{
	return currentValue - pastValue;
}


void Error_Message(char message[])
{
	printf("******* ERROR : %s *******\n", message);

	return;
}