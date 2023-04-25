

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_LENGTH 500			// 하나의 데이터셋의 데이터 개수
#define BATCH_SIZE 50			// 데이터셋을 BATCH_SIZE만큼씩 나눠서 한번의 사이클 동안 (DATA_LENGTH / BATCH_SIZE)만큼 가중치 업데이트 함
#define EPOCH 1					// 학습(한 사이클)을 총 EPOCH만큼 반복함
#define LEARNING_RATE 1e-3		// 학습률

#define SET_VALUE_FILE_NAME "r_data2.txt"	// 목표값 학습데이터 파일
#define TRAIN_FILE_NAME "y_data2.txt"		// 측정값 학습데이터 파일


void Learn();

float Foreward_Prop(float, float, int);		// 순전파
void Backward_Prop(int);					// 역전파

float P_Activation_Func(float);				// P뉴런 활성화 함수
float I_Activation_Func(float, float);		// I뉴런 활성화 함수
float D_Activation_Func(float, float);		// D뉴런 활성화 함수

void Error_Message(char []);


float r[DATA_LENGTH + 1] = { 0 };			// 목표값 학습데이터
float train_y[DATA_LENGTH + 1] = { 0 };		// 측정값 학습데이터

float w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// 입력 레이어와 히든 레이어 사이의 가중치   1행은 r의 가중치, 2행은 y의 가중치
float w2[3] = {0.5, 0.3, 0.2};					// 히든 레이어와 출력 레이어 사이의 가중치     [0] : P상수, [1] : I상수, [2] : D상수
												// 초기 가중치는 지글러-니콜스 기법으로 계산한 P, I, D 상수들로 초기화 함

float i_u1[DATA_LENGTH + 1] = { 0 };	// Input Layer에서 목표값(r) 입력 뉴런의 입력 변수
float i_u2[DATA_LENGTH + 1] = { 0 };	// Input Layer에서 측정값(y) 입력 뉴런의 입력 변수

float i_x1[DATA_LENGTH + 1] = { 0 };	// Input Layer에서 목표값(r) 입력 뉴런의 출력 변수
float i_x2[DATA_LENGTH + 1] = { 0 };	// Input Layer에서 측정값(y) 입력 뉴런의 출력 변수

float h_u1[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 P뉴런의 입력 변수
float h_u2[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 I뉴런의 입력 변수
float h_u3[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 D뉴런의 입력 변수

float h_x1[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 P뉴런의 출력 변수
float h_x2[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 I뉴런의 출력 변수
float h_x3[DATA_LENGTH + 1] = { 0 };	// Hidden Layer에서 D뉴런의 출력 변수

float o_u[DATA_LENGTH + 1] = { 0 };		// Output Layer의 입력 변수
float o_x[DATA_LENGTH + 1] = { 0 };		// Output Layer의 출력 변수

float v[DATA_LENGTH + 1] = { 0 };	// 시스템(ex. 모터)에 입력으로 들어가는 값
//float y[DATA_LENGTH];	// 시스템(ex. 모터)을 거쳐서 나온 최종 출력값(= 최종적으로 출력된 값을 측정한 값)

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


	
	// 측정값 데이터 로드
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


	// 목표값 데이터 로드
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
	printf("\n학습 시작\n\n");

	Learn();

	printf("\n학습 종료\n\n");
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

			// 배치사이즈마다 역전파 시켜서 가중치를 업데이트 함
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

	// 입력 레이어와 히든 레이어 사이의 연산
	h_u1[idx] = w1[0][0] * i_x1[idx] + w1[1][0] * i_x2[idx];
	h_u2[idx] = w1[0][1] * i_x1[idx] + w1[1][1] * i_x2[idx];
	h_u3[idx] = w1[0][2] * i_x1[idx] + w1[1][2] * i_x2[idx];
	

	// 히든 레이어의 각 뉴런들의 활성화 함수 연산
	h_x1[idx] = P_Activation_Func(h_u1[idx]);
	h_x2[idx] = I_Activation_Func(h_u2[idx], h_x2[idx - 1]);
	h_x3[idx] = D_Activation_Func(h_u3[idx], h_u3[idx - 1]);


	// 한 스텝에 대해서 최종 연산
	res = h_x1[idx] * w2[0] + h_x2[idx] * w2[1] + h_x3[idx] * w2[2];
	
	return res;
}


void Backward_Prop(int idx)
{
	float error = 0.0;	// 스텝(시간) t 에서의 에러값(목표값 - 측정값)
	float y_rate = 0.0;	// 스텝 t와 t-1에서의 y의 변화량
	float v_rate = 0.0;	// 스텝 t와 t-1에서의 v의 변화량
	//float h_u_rate = 0.0;
	//float h_x_rate = 0.0;

	// P, I, D 뉴런의 각 가중치들의 변화율
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;
	//float w1_gradient = 0.0;


	// w2 행렬에 대한 경사하강 시작
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
	// w2 행렬에 대한 경사하강 종료
	
	w2[0] += LEARNING_RATE * x1_gradient;
	w2[1] += LEARNING_RATE * x2_gradient;
	w2[2] += LEARNING_RATE * x3_gradient;

	

	/*
	// w1 행렬에 대한 경사하강 시작
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
	// w1 행렬에 대한 경사하강 종료
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