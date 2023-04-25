

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_SIZE 200			// �ϳ��� �����ͼ��� ������ ����
#define BATCH_SIZE 20			// �����ͼ��� BATCH_SIZE��ŭ�� ������ �ѹ��� ����Ŭ ���� (DATA_SIZE / BATCH_SIZE)��ŭ ����ġ ������Ʈ ��
#define EPOCH 1					// �н�(�� ����Ŭ)�� �� EPOCH��ŭ �ݺ���
#define LEARNING_RATE 1e-5		// �н���

#define INPUT_NEURON_NUMBER 2	// InputLayer�� ���� ����
#define HIDDEN_NEURON_NUMBER 3	// HiddenLayer�� ���� ����

#define SET_VALUE_FILE_NAME "goal_rpm.txt"	// ��ǥ�� �н������� ����
#define TRAIN_FILE_NAME "train_y_1.txt"		// ������ �н������� ����


void Learn();

short Foreward_Prop(short, short, int);		// ������
void Backward_Prop(int);					// ������

short P_Activation_Func(short);				// P���� Ȱ��ȭ �Լ�
short I_Activation_Func(short, short);		// I���� Ȱ��ȭ �Լ�
short D_Activation_Func(short, short);		// D���� Ȱ��ȭ �Լ�

void Error_Message(char[]);



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
	short u;	// ������ �Է� ����
	short x;	// ������ ��� ����

}Layer;




Layer InputLayer[INPUT_NEURON_NUMBER][DATA_SIZE];
Layer HiddenLayer[HIDDEN_NEURON_NUMBER][DATA_SIZE];
Layer OutputLayer[DATA_SIZE];


unsigned short r[DATA_SIZE] = { 0 };			// ��ǥ�� �н�������
unsigned short train_y[DATA_SIZE] = { 0 };	// ������ �н�������

int w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// �Է� ���̾�� ���� ���̾� ������ ����ġ   1���� r�� ����ġ, 2���� y�� ����ġ
float w2[3] = { 0.05, 0.05, 0.001 };				// ���� ���̾�� ��� ���̾� ������ ����ġ     [0] : P���, [1] : I���, [2] : D���
												// �ʱ� ����ġ�� ���۷�-���ݽ� ������� ����� P, I, D ������ �ʱ�ȭ ��

float v[DATA_SIZE] = { 0 };	// �ý���(ex. ����)�� �Է����� ���� ��


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
	while (fscanf(pSetValue_fp, "%hu\n", &r[pSetValue_fp_idx]) != EOF)
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
	while (fscanf(pTrainValue_fp, "%hu\n", &train_y[pTrainValue_fp_idx]) != EOF)
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
			InputLayer[NEURON_TYPE_R][i].u = r[i];
			InputLayer[NEURON_TYPE_Y][i].u = train_y[i];

			InputLayer[NEURON_TYPE_R][i].x = InputLayer[NEURON_TYPE_R][i].u;
			InputLayer[NEURON_TYPE_Y][i].x = InputLayer[NEURON_TYPE_Y][i].u;


			OutputLayer[i].u = Foreward_Prop(InputLayer[NEURON_TYPE_R][i].x, InputLayer[NEURON_TYPE_Y][i].x, i);
			OutputLayer[i].x = OutputLayer[i].u;
			v[i] = OutputLayer[i].x;


			// ��ġ������� ������ ���Ѽ� ����ġ�� ������Ʈ ��
			if (((i + 1) % BATCH_SIZE) == 0 && i > 0)
			{
				Backward_Prop(i);
			}
		}
	}


	return;
}




short Foreward_Prop(short setValue, short inputValue, int idx)
{
	short res = 0.0;


	// �Է� ���̾�� ���� ���̾� ������ ����
	HiddenLayer[NEURON_TYPE_P][idx].u = w1[0][0] * setValue + w1[1][0] * inputValue;
	HiddenLayer[NEURON_TYPE_I][idx].u = w1[0][1] * setValue + w1[1][1] * inputValue;
	HiddenLayer[NEURON_TYPE_D][idx].u = w1[0][2] * setValue + w1[1][2] * inputValue;


	// ���� ���̾��� �� �������� Ȱ��ȭ �Լ� ����
	HiddenLayer[NEURON_TYPE_P][idx].x = P_Activation_Func(HiddenLayer[NEURON_TYPE_P][idx].u);

	if (idx != 0)
	{
		HiddenLayer[NEURON_TYPE_I][idx].x = I_Activation_Func(HiddenLayer[NEURON_TYPE_I][idx].u, HiddenLayer[NEURON_TYPE_I][idx - 1].x);
		HiddenLayer[NEURON_TYPE_D][idx].x = D_Activation_Func(HiddenLayer[NEURON_TYPE_D][idx].u, HiddenLayer[NEURON_TYPE_D][idx - 1].u);
	}
	else
	{
		HiddenLayer[NEURON_TYPE_I][idx].x = I_Activation_Func(HiddenLayer[NEURON_TYPE_I][idx].u, (short)0);
		HiddenLayer[NEURON_TYPE_D][idx].x = D_Activation_Func(HiddenLayer[NEURON_TYPE_D][idx].u, (short)0);
	}


	// �� ���ܿ� ���ؼ� ���� ����
	res = HiddenLayer[NEURON_TYPE_P][idx].x * w2[0] + HiddenLayer[NEURON_TYPE_I][idx].x * w2[1] + HiddenLayer[NEURON_TYPE_D][idx].x * w2[2];

	return res;
}


void Backward_Prop(int idx)
{
	short error = 0;	// ����(�ð�) t ������ ������(��ǥ�� - ������)
	short y_rate = 0;	// ���� t�� t-1������ y�� ��ȭ��
	float v_rate = 0.0;	// ���� t�� t-1������ v�� ��ȭ��

	// P, I, D ������ �� ����ġ���� ��ȭ��
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;


	// w2 ��Ŀ� ���� ����ϰ� ����
	for (int i = idx - BATCH_SIZE + 1; i <= idx; i++)
	{
		error = r[i] - train_y[i];

		if (i != 0)
		{
			y_rate = train_y[i] - train_y[i - 1];

			if (v[i] > v[i - 1])
				v_rate = v[i] - v[i - 1];

			else if (v[i - 1] > v[i])
				v_rate = v[i - 1] - v[i];

			else // v[i] == v[i - 1]
				v_rate = 1e-20;
		}
		else
		{
			y_rate = train_y[i];
			v_rate = v[i];
		}
		
		x1_gradient += -2 * error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_P][i].x;
		x2_gradient += -2 * error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_I][i].x;
		x3_gradient += -2 * error * (y_rate / v_rate) * HiddenLayer[NEURON_TYPE_D][i].x;
	}

	/*
	x1_gradient *= -2;
	x2_gradient *= -2;
	x3_gradient *= -2;
	*/

	x1_gradient /= BATCH_SIZE;
	x2_gradient /= BATCH_SIZE;
	x3_gradient /= BATCH_SIZE;
	// w2 ��Ŀ� ���� ����ϰ� ����

	w2[0] += LEARNING_RATE * x1_gradient;
	w2[1] += LEARNING_RATE * x2_gradient;
	w2[2] += LEARNING_RATE * x3_gradient;


	
	return;
}



short P_Activation_Func(short currentValue)
{
	return currentValue;
}



short I_Activation_Func(short currentValue, short accrueValue)
{
	//return accrueValue + (currentValue * 1e-1);
	return accrueValue + currentValue ;
}



short D_Activation_Func(short currentValue, short pastValue)
{
	//return (currentValue - pastValue) / (float)1e-1; // C26451 ���� �߻��ؼ� 1e-1�� 4����Ʈ�� �ٲ���
	return currentValue - pastValue;
}


void Error_Message(char message[])
{
	printf("******* ERROR : %s *******\n", message);

	return;
}