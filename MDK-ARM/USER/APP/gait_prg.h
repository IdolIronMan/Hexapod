#ifndef GAIT_PRG_H
#define GAIT_PRG_H

#include "my_math.h"
#include "main.h"

#define LEG_LEN1 53.f  // �Ȳ���һ���˳��ȣ���λmm��
#define LEG_LEN2 80.f  // �Ȳ��ڶ����˳��ȣ���λmm��
#define LEG_LEN3 144.f // �Ȳ��������˳��ȣ���λmm��

#define CHASSIS_LEN 162.2f        // ���̳��ȣ�y�᷽��
#define CHASSIS_WIDTH 161.5f      // ���̿�ȣ�x�᷽��
#define CHASSIS_FRONT_WIDTH 93.3f // ����ǰ�˿�ȣ�x�᷽��

#define N_POINTS 40                       // ���������������ż����
#define THETA_STAND_2 40.0f / 180.0f * PI // ��е��վ��ʱ��������ؽڵĽǶ�
#define THETA_STAND_3 -110.0f / 180.0f * PI

#define K_CEN 500.0f     // ����ȷ��Բ��ģ����ϵ��
#define KR_1 1           //%���ڼ��㲽����С��ϵ��
#define KR_2 1           //%���ڼ��㲽����С��ϵ��
#define MAX_R_PACE 75.0f // ��󲽷��뾶
#define MAX_SPEED 0.2f * 660

#define MIN_Z_PACE 15.0f

class Velocity
{
public:
    float Vx;    // x���ٶ�
    float Vy;    // y���ٶ�
    float omega; // ���ٶ�
};

typedef struct
{
    Thetas thetas[N_POINTS];
} action;

class Gait_prg
{
private:
    uint32_t pace_time;  // ��һ�����ѵ�ʱ��
    Position3 Pws[6];    // ��е��ĩ��վ��״̬���������ʼ�˵�λ��
    Position3 Pws_default[6]; //Ĭ������»�е��ĩ��վ��״̬���������ʼ�˵�λ��
    Position3 P_legs[6]; // ������е����ʼ������ڻ��������ĵ�����
    Position3 CEN;       // ��Բ�ĵ�����
    float R_pace;        // ������С����λmm��
    Position3 hexapod_rotate(Position3 &point,uint32_t index);
    Position3 rotate_angle;  //������ת�Ƕ�
public:
    action actions[6];
    void Init(); // ��ʼ��
    void CEN_and_pace_cal(Velocity velocity);
    void gait_proggraming();
    uint32_t get_pace_time();
    void set_height(float height);
};

#endif
