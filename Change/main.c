

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_SIZE 500			// 하나의 데이터셋의 데이터 개수
#define BATCH_SIZE 50			// 데이터셋을 BATCH_SIZE만큼씩 나눠서 한번의 사이클 동안 (DATA_SIZE / BATCH_SIZE)만큼 가중치 업데이트 함
#define EPOCH 1					// 학습(한 사이클)을 총 EPOCH만큼 반복함
#define LEARNING_RATE 1e-3		// 학습률

#define INPUT_NEURON_NUMBER 2	// InputLayer의 뉴런 개수
#define HIDDEN_NEURON_NUMBER 3	// HiddenLayer의 뉴런 개수

#define SET_VALUE_FILE_NAME "r_data2.txt"	// 목표값 학습데이터 파일
#define TRAIN_FILE_NAME "y_data2.txt"		// 측정값 학습데이터 파일


void Learn();

float Foreward_Prop(float, float, int);		// 순전파
void Backward_Prop(int);					// 역전파

float P_Activation_Func(float);				// P뉴런 활성화 함수
float I_Activation_Func(float, float);		// I뉴런 활성화 함수
float D_Activation_Func(float, float);		// D뉴런 활성화 함수

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
	float u;	// 뉴런의 입력 변수
	float x;	// 뉴런의 출력 변수

}Layer;

Layer InputLayer[INPUT_NEURON_NUMBER][DATA_SIZE];
Layer HiddenLayer[HIDDEN_NEURON_NUMBER][DATA_SIZE];
Layer OutputLayer[DATA_SIZE];
*/

float r[DATA_SIZE] = { 0 };			// 목표값 학습데이터
float train_y[DATA_SIZE] = { 0 };		// 측정값 학습데이터

float w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// 입력 레이어와 히든 레이어 사이의 가중치   1행은 r의 가중치, 2행은 y의 가중치
float w2[3] = { 0.5, 0.3, 0.2 };				// 히든 레이어와 출력 레이어 사이의 가중치     [0] : P상수, [1] : I상수, [2] : D상수
												// 초기 가중치는 지글러-니콜스 기법으로 계산한 P, I, D 상수들로 초기화 함

float v[DATA_SIZE] = { 0 };	// 시스템(ex. 모터)에 입력으로 들어가는 값




float i_u1[DATA_SIZE] = { 0 };	// Input Layer에서 목표값(r) 입력 뉴런의 입력 변수
float i_u2[DATA_SIZE] = { 0 };	// Input Layer에서 측정값(y) 입력 뉴런의 입력 변수

float i_x1[DATA_SIZE] = { 0 };	// Input Layer에서 목표값(r) 입력 뉴런의 출력 변수
float i_x2[DATA_SIZE] = { 0 };	// Input Layer에서 측정값(y) 입력 뉴런의 출력 변수

float h_u1[DATA_SIZE] = { 0 };	// Hidden Layer에서 P뉴런의 입력 변수
float h_u2[DATA_SIZE] = { 0 };	// Hidden Layer에서 I뉴런의 입력 변수
float h_u3[DATA_SIZE] = { 0 };	// Hidden Layer에서 D뉴런의 입력 변수

float h_x1[DATA_SIZE] = { 0 };	// Hidden Layer에서 P뉴런의 출력 변수
float h_x2[DATA_SIZE] = { 0 };	// Hidden Layer에서 I뉴런의 출력 변수
float h_x3[DATA_SIZE] = { 0 };	// Hidden Layer에서 D뉴런의 출력 변수

float o_u[DATA_SIZE] = { 0 };		// Output Layer의 입력 변수
float o_x[DATA_SIZE] = { 0 };		// Output Layer의 출력 변수


void main(void)
{
	FILE* pSetValue_fp = NULL;
	FILE* pTrainValue_fp = NULL;

	int pSetValue_fp_idx;
	int pTrainValue_fp_idx;


	// 목표값(SetValue) 데이터 로드
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


	// 측정값(TrainValue) 데이터 로드
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
	printf("\n학습 시작\n\n");

	Learn();

	printf("\n학습 종료\n\n");
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

	//i_x1[idx] = setValue;
	//i_x2[idx] = inputValue;

	// 입력 레이어와 히든 레이어 사이의 연산
	//HiddenLayer[NEURON_TYPE_P][idx].u = w1[0][0] * setValue + w1[1][0] * inputValue;
	//HiddenLayer[NEURON_TYPE_I][idx].u = w1[0][1] * setValue + w1[1][1] * inputValue;
	//HiddenLayer[NEURON_TYPE_D][idx].u = w1[0][2] * setValue + w1[1][2] * inputValue;
	h_u1[idx] = w1[0][0] * i_x1[idx] + w1[1][0] * i_x2[idx];
	h_u2[idx] = w1[0][1] * i_x1[idx] + w1[1][1] * i_x2[idx];
	h_u3[idx] = w1[0][2] * i_x1[idx] + w1[1][2] * i_x2[idx];
	

	// 히든 레이어의 각 뉴런들의 활성화 함수 연산
	//HiddenLayer[NEURON_TYPE_P][idx].x = P_Activation_Func(HiddenLayer[NEURON_TYPE_P][idx].u);
	//HiddenLayer[NEURON_TYPE_I][idx].x = I_Activation_Func(HiddenLayer[NEURON_TYPE_I][idx].u, HiddenLayer[NEURON_TYPE_I][idx - 1].x);
	//HiddenLayer[NEURON_TYPE_D][idx].x = D_Activation_Func(HiddenLayer[NEURON_TYPE_D][idx].u, HiddenLayer[NEURON_TYPE_D][idx - 1].u);
	h_x1[idx] = P_Activation_Func(h_u1[idx]);
	h_x2[idx] = I_Activation_Func(h_u2[idx], h_x2[idx - 1]);
	h_x3[idx] = D_Activation_Func(h_u3[idx], h_u3[idx - 1]);


	// 한 스텝에 대해서 최종 연산
	//res = HiddenLayer[NEURON_TYPE_P][idx].x * w2[0] + HiddenLayer[NEURON_TYPE_I][idx].x * w2[1] + HiddenLayer[NEURON_TYPE_D][idx].x * w2[2];
	res = h_x1[idx] * w2[0] + h_x2[idx] * w2[1] + h_x3[idx] * w2[2];
	
	return res;
}


void Backward_Prop(int idx)
{
	float error = 0.0;	// 스텝(시간) t 에서의 에러값(목표값 - 측정값)
	float y_rate = 0.0;	// 스텝 t와 t-1에서의 y의 변화량
	float v_rate = 0.0;	// 스텝 t와 t-1에서의 v의 변화량

	// P, I, D 뉴런의 각 가중치들의 변화율
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;


	// w2 행렬에 대한 경사하강 시작
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
	// w2 행렬에 대한 경사하강 종료
	
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