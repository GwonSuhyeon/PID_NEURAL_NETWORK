

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_LENGTH 500			// �ϳ��� �����ͼ��� ������ ����
#define BATCH_SIZE 50			// �����ͼ��� BATCH_SIZE��ŭ�� ������ �ѹ��� ����Ŭ ���� (DATA_LENGTH / BATCH_SIZE)��ŭ ����ġ ������Ʈ ��
#define EPOCH 1					// �н�(�� ����Ŭ)�� �� EPOCH��ŭ �ݺ���
#define LEARNING_RATE 1e-3		// �н���

#define SET_VALUE_FILE_NAME "r_data2.txt"	// ��ǥ�� �н������� ����
#define TRAIN_FILE_NAME "y_data2.txt"		// ������ �н������� ����


void Learn();

float Foreward_Prop(float, float, int);		// ������
void Backward_Prop(int);					// ������

float P_Activation_Func(float);				// P���� Ȱ��ȭ �Լ�
float I_Activation_Func(float, float);		// I���� Ȱ��ȭ �Լ�
float D_Activation_Func(float, float);		// D���� Ȱ��ȭ �Լ�

void Error_Message(char []);


float r[DATA_LENGTH + 1] = { 0 };			// ��ǥ�� �н�������
float train_y[DATA_LENGTH + 1] = { 0 };		// ������ �н�������

float w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// �Է� ���̾�� ���� ���̾� ������ ����ġ   1���� r�� ����ġ, 2���� y�� ����ġ
float w2[3] = {0.5, 0.3, 0.2};					// ���� ���̾�� ��� ���̾� ������ ����ġ     [0] : P���, [1] : I���, [2] : D���
												// �ʱ� ����ġ�� ���۷�-���ݽ� ������� ����� P, I, D ������ �ʱ�ȭ ��

float i_u1[DATA_LENGTH + 1] = { 0 };	// Input Layer���� ��ǥ��(r) �Է� ������ �Է� ����
float i_u2[DATA_LENGTH + 1] = { 0 };	// Input Layer���� ������(y) �Է� ������ �Է� ����

float i_x1[DATA_LENGTH + 1] = { 0 };	// Input Layer���� ��ǥ��(r) �Է� ������ ��� ����
float i_x2[DATA_LENGTH + 1] = { 0 };	// Input Layer���� ������(y) �Է� ������ ��� ����

float h_u1[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� P������ �Է� ����
float h_u2[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� I������ �Է� ����
float h_u3[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� D������ �Է� ����

float h_x1[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� P������ ��� ����
float h_x2[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� I������ ��� ����
float h_x3[DATA_LENGTH + 1] = { 0 };	// Hidden Layer���� D������ ��� ����

float o_u[DATA_LENGTH + 1] = { 0 };		// Output Layer�� �Է� ����
float o_x[DATA_LENGTH + 1] = { 0 };		// Output Layer�� ��� ����

float v[DATA_LENGTH + 1] = { 0 };	// �ý���(ex. ����)�� �Է����� ���� ��
//float y[DATA_LENGTH];	// �ý���(ex. ����)�� ���ļ� ���� ���� ��°�(= ���������� ��µ� ���� ������ ��)

/*
float u1[3];
float x1[3][DATA_LENGTH];
float u2[3][DATA_LENGTH];
*/



void main(void)
{
	int fp_idx;

	FILE* fp = NULL;



	if (r == NULL)
	{
		Error_Message("null point - main function - variable r");
		return;
	}

	if (train_y == NULL)
	{
		Error_Message("null point - main function - variable train_y");
		return;
	}


	
	// ������ ������ �ε�
	fp = fopen(TRAIN_FILE_NAME, "r");
	if (fp == NULL)
	{
		Error_Message("file open fail - main function - y_data.txt");
		return;
	}

	fp_idx = 1;
	while (fscanf(fp, "%f\n", &train_y[fp_idx]) != EOF)
	{
		fp_idx++;
	}


	// ��ǥ�� ������ �ε�
	fp = fopen(SET_VALUE_FILE_NAME, "r");

	fp_idx = 1;

	if (fp == NULL)
	{
		Error_Message("file open fail - main function - r_data.txt");

		return;
	}

	while (fscanf(fp, "%f\n", &r[fp_idx]) != EOF)
	{
		fp_idx++;
	}



	if (fp != NULL)
	{
		fclose(fp);
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
	if (i_u1 == NULL)
	{
		Error_Message("null point - Learn function - variable i_u1");

		return;
	}

	if (i_u2 == NULL)
	{
		Error_Message("null point - Learn function - variable i_u2");

		return;
	}

	if (o_u == NULL)
	{
		Error_Message("null point - Learn function - variable o_u");

		return;
	}

	if (o_x == NULL)
	{
		Error_Message("null point - Learn function - variable o_x");

		return;
	}

	if (v == NULL)
	{
		Error_Message("null point - Learn function - variable v");

		return;
	}

	/*
	if (y == NULL)
	{
		Error_Message("null point - Learn function - variable y");

		return;
	}
	*/

	for (int epoch = 0; epoch < EPOCH; epoch++)
	{
		for (int i = 1; i <= DATA_LENGTH; i++)
		{
			i_u1[i] = r[i];
			i_u2[i] = train_y[i];

			i_x1[i] = i_u1[i];
			i_x2[i] = i_u2[i];

			o_u[i] = Foreward_Prop(i_x1[i], i_x2[i], i);
			o_x[i] = o_u[i];
			v[i] = o_x[i];
			//y[i] = v[i];

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

	if (i_x1 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable i_x1");

		return;
	}

	if (i_x2 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable i_x2");

		return;
	}

	if (h_u1 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_u1");

		return;
	}

	if (h_u2 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_u2");

		return;
	}

	if (h_u3 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_u3");

		return;
	}

	if (h_x1 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_x1");

		return;
	}

	if (h_x2 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_x2");

		return;
	}

	if (h_x3 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable h_x3");

		return;
	}

	if (w1 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable w1");

		return;
	}

	if (w2 == NULL)
	{
		Error_Message("null point - Foreward_Prop function - variable w2");

		return;
	}
	



	i_x1[idx] = setValue;
	i_x2[idx] = inputValue;

	// �Է� ���̾�� ���� ���̾� ������ ����
	h_u1[idx] = w1[0][0] * i_x1[idx] + w1[1][0] * i_x2[idx];
	h_u2[idx] = w1[0][1] * i_x1[idx] + w1[1][1] * i_x2[idx];
	h_u3[idx] = w1[0][2] * i_x1[idx] + w1[1][2] * i_x2[idx];
	

	// ���� ���̾��� �� �������� Ȱ��ȭ �Լ� ����
	h_x1[idx] = P_Activation_Func(h_u1[idx]);
	h_x2[idx] = I_Activation_Func(h_u2[idx], h_x2[idx - 1]);
	h_x3[idx] = D_Activation_Func(h_u3[idx], h_u3[idx - 1]);


	// �� ���ܿ� ���ؼ� ���� ����
	res = h_x1[idx] * w2[0] + h_x2[idx] * w2[1] + h_x3[idx] * w2[2];
	
	return res;
}


void Backward_Prop(int idx)
{
	float error = 0.0;	// ����(�ð�) t ������ ������(��ǥ�� - ������)
	float y_rate = 0.0;	// ���� t�� t-1������ y�� ��ȭ��
	float v_rate = 0.0;	// ���� t�� t-1������ v�� ��ȭ��
	//float h_u_rate = 0.0;
	//float h_x_rate = 0.0;

	// P, I, D ������ �� ����ġ���� ��ȭ��
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;
	//float w1_gradient = 0.0;


	// w2 ��Ŀ� ���� ����ϰ� ����
	for (int t = idx - BATCH_SIZE + 1; t <= idx; t++)
	{
		error = r[t] - train_y[t];
		//y_rate = y[t] - y[t - 1];
		y_rate = train_y[t] - train_y[t - 1];
		v_rate = v[t] - v[t - 1];
		
		x1_gradient += error * (y_rate / v_rate) * h_x1[t];
		x2_gradient += error * (y_rate / v_rate) * h_x2[t];
		x3_gradient += error * (y_rate / v_rate) * h_x3[t];
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

	

	/*
	// w1 ��Ŀ� ���� ����ϰ� ����
	error = 0.0;
	y_rate = 0.0;
	v_rate = 0.0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			w1_gradient = 0.0;

			for (int t = idx - BATCH_SIZE; t < idx; t++)
			{
				error = r[t] - train_y[t];
				//y_rate = y[t] - y[t - 1];
				y_rate = train_y[t] - train_y[t - 1];
				v_rate = v[t] - v[t - 1];
				
				if (j == 0)
				{
					h_u_rate = h_u1[t] - h_u1[t - 1];
					h_x_rate = h_x1[t] - h_x1[t - 1];
				}
				else if (j == 1)
				{
					h_u_rate = h_u2[t] - h_u2[t - 1];
					h_x_rate = h_x2[t] - h_x2[t - 1];
				}
				else if (j == 2)
				{
					h_u_rate = h_u3[t] - h_u3[t - 1];
					h_x_rate = h_x3[t] - h_x3[t - 1];
				}

				if (i == 0)
					w1_gradient += error * (y_rate / v_rate) * w2[j] * (h_x_rate / h_u_rate) * i_x1[t];
				if(i == 1)
					w1_gradient += error * (y_rate / v_rate) * w2[j] * (h_x_rate / h_u_rate) * i_x2[t];
			}

			w1_gradient *= -2;
			w1_gradient /= BATCH_SIZE;

			w1[i][j] += LEARNING_RATE * w1_gradient;
		}
	}
	// w1 ��Ŀ� ���� ����ϰ� ����
	*/
	



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