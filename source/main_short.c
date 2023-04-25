

#pragma warning(disable : 4996)

#include <stdio.h>

#define DATA_SIZE 200			// 하나의 데이터셋의 데이터 개수
#define BATCH_SIZE 20			// 데이터셋을 BATCH_SIZE만큼씩 나눠서 한번의 사이클 동안 (DATA_SIZE / BATCH_SIZE)만큼 가중치 업데이트 함
#define EPOCH 1					// 학습(한 사이클)을 총 EPOCH만큼 반복함
#define LEARNING_RATE 1e-5		// 학습률

#define INPUT_NEURON_NUMBER 2	// InputLayer의 뉴런 개수
#define HIDDEN_NEURON_NUMBER 3	// HiddenLayer의 뉴런 개수

#define SET_VALUE_FILE_NAME "goal_rpm.txt"	// 목표값 학습데이터 파일
#define TRAIN_FILE_NAME "train_y_1.txt"		// 측정값 학습데이터 파일


void Learn();

short Foreward_Prop(short, short, int);		// 순전파
void Backward_Prop(int);					// 역전파

short P_Activation_Func(short);				// P뉴런 활성화 함수
short I_Activation_Func(short, short);		// I뉴런 활성화 함수
short D_Activation_Func(short, short);		// D뉴런 활성화 함수

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
	short u;	// 뉴런의 입력 변수
	short x;	// 뉴런의 출력 변수

}Layer;




Layer InputLayer[INPUT_NEURON_NUMBER][DATA_SIZE];
Layer HiddenLayer[HIDDEN_NEURON_NUMBER][DATA_SIZE];
Layer OutputLayer[DATA_SIZE];


unsigned short r[DATA_SIZE] = { 0 };			// 목표값 학습데이터
unsigned short train_y[DATA_SIZE] = { 0 };	// 측정값 학습데이터

int w1[2][3] = { {1, 1, 1}, {-1, -1, -1} };	// 입력 레이어와 히든 레이어 사이의 가중치   1행은 r의 가중치, 2행은 y의 가중치
float w2[3] = { 0.05, 0.05, 0.001 };				// 히든 레이어와 출력 레이어 사이의 가중치     [0] : P상수, [1] : I상수, [2] : D상수
												// 초기 가중치는 지글러-니콜스 기법으로 계산한 P, I, D 상수들로 초기화 함

float v[DATA_SIZE] = { 0 };	// 시스템(ex. 모터)에 입력으로 들어가는 값


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
	while (fscanf(pSetValue_fp, "%hu\n", &r[pSetValue_fp_idx]) != EOF)
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
			InputLayer[NEURON_TYPE_R][i].u = r[i];
			InputLayer[NEURON_TYPE_Y][i].u = train_y[i];

			InputLayer[NEURON_TYPE_R][i].x = InputLayer[NEURON_TYPE_R][i].u;
			InputLayer[NEURON_TYPE_Y][i].x = InputLayer[NEURON_TYPE_Y][i].u;


			OutputLayer[i].u = Foreward_Prop(InputLayer[NEURON_TYPE_R][i].x, InputLayer[NEURON_TYPE_Y][i].x, i);
			OutputLayer[i].x = OutputLayer[i].u;
			v[i] = OutputLayer[i].x;


			// 배치사이즈마다 역전파 시켜서 가중치를 업데이트 함
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


	// 입력 레이어와 히든 레이어 사이의 연산
	HiddenLayer[NEURON_TYPE_P][idx].u = w1[0][0] * setValue + w1[1][0] * inputValue;
	HiddenLayer[NEURON_TYPE_I][idx].u = w1[0][1] * setValue + w1[1][1] * inputValue;
	HiddenLayer[NEURON_TYPE_D][idx].u = w1[0][2] * setValue + w1[1][2] * inputValue;


	// 히든 레이어의 각 뉴런들의 활성화 함수 연산
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


	// 한 스텝에 대해서 최종 연산
	res = HiddenLayer[NEURON_TYPE_P][idx].x * w2[0] + HiddenLayer[NEURON_TYPE_I][idx].x * w2[1] + HiddenLayer[NEURON_TYPE_D][idx].x * w2[2];

	return res;
}


void Backward_Prop(int idx)
{
	short error = 0;	// 스텝(시간) t 에서의 에러값(목표값 - 측정값)
	short y_rate = 0;	// 스텝 t와 t-1에서의 y의 변화량
	float v_rate = 0.0;	// 스텝 t와 t-1에서의 v의 변화량

	// P, I, D 뉴런의 각 가중치들의 변화율
	float x1_gradient = 0.0;
	float x2_gradient = 0.0;
	float x3_gradient = 0.0;


	// w2 행렬에 대한 경사하강 시작
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
	// w2 행렬에 대한 경사하강 종료

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
	//return (currentValue - pastValue) / (float)1e-1; // C26451 에러 발생해서 1e-1을 4바이트로 바꿔줌
	return currentValue - pastValue;
}


void Error_Message(char message[])
{
	printf("******* ERROR : %s *******\n", message);

	return;
}