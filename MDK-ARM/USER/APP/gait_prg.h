#ifndef GAIT_PRG_H
#define GAIT_PRG_H

#include "my_math.h"

#define LEG_LEN1 53.f  // �Ȳ���һ���˳��ȣ���λmm��
#define LEG_LEN2 80.f  // �Ȳ��ڶ����˳��ȣ���λmm��
#define LEG_LEN3 144.f // �Ȳ��������˳��ȣ���λmm��

#define CHASSIS_LEN 162.2f        // ���̳��ȣ�y�᷽��
#define CHASSIS_WIDTH 161.5f      // ���̿�ȣ�x�᷽��
#define CHASSIS_FRONT_WIDTH 93.3f // ����ǰ�˿�ȣ�x�᷽��

#define N_POINTS 20                       // �뾶�͵������
#define THETA_STAND_2 40.0f / 180.0f * PI // ��е��վ��ʱ��������ؽڵĽǶ�
#define THETA_STAND_3 -110.0f / 180.0f * PI

#define K_CEN 1 // ����ȷ��Բ��ģ����ϵ��
#define KR_1 1  //%���ڼ��㲽����С��ϵ��
#define KR_2 1  //%���ڼ��㲽����С��ϵ��

class Velocity
{
public:
    float Vx;    // x���ٶ�
    float Vy;    // y���ٶ�
    float omega; // ���ٶ�
};

class Gait_prg
{
public:
    Gait_prg(); // ��ʼ��
    void CEN_and_pace_cal();
    void gait_proggraming();
};

typedef struct
{
    Thetas thetas[N_POINTS];
} action;

#endif
