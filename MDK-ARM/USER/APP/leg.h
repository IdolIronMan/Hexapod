#ifndef LEG_H
#define LEG_H

#include "Servo.h"
#include "usart.h"
#include "my_math.h"

class Leg : public Servo_Broad_Cast
{
private:
	Servo servos[3];
	Thetas theta;

public:
	UART_HandleTypeDef *huart;
	Leg(UART_HandleTypeDef *huart); // ���캯��
	Leg(){};//�޲ι���
	void set_thetas(Thetas thetas);	// ���û�е�ȵĽǶ�
	void set_time(uint16_t tims);	// ���û�е���ƶ���ʱ��
	void move();					// ��е���ƶ�����
	void move_wait();				//���û�е�ȽǶȣ�����Ҫ�ȴ���ʼ������ƶ�
	void move_start();              // �û�е�ȿ�ʼ�˶�
	
};

#endif
