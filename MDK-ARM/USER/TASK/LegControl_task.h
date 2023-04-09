#ifndef LEGCONTROL_TASK_H
#define LEGCONTROL_TASK_H

#include "my_math.h"
#include "leg.h"
#include "remote.h"
#include "gait_prg.h"

#define LEG_JOINT2_OFFSET PI / 2
#define LEG_JOINT3_OFFSET -2 * PI / 9

#define HEXAPOD_MIN_HEIGHT -70.0f
#define HEXAPOD_MAX_HEIGHT 70.0f
#define HEXAPOD_MIN_X -40.0f
#define HEXAPOD_MAX_X 40.0f
#define HEXAPOD_MIN_Y -40.0f
#define HEXAPOD_MAX_Y 40.0f


#define HEXAPOD_MIN_X_ROTATE -15.0f / 180 * PI // ��X����ת�Ƕ���СΪ-15��
#define HEXAPOD_MAX_X_ROTATE 15.0f / 180 * PI  // ��X����ת�Ƕ����Ϊ 15��
#define HEXAPOD_MIN_Y_ROTATE -10.0f / 180 * PI // ��X����ת�Ƕ���СΪ-10��
#define HEXAPOD_MAX_Y_ROTATE 10.0f / 180 * PI  // ��X����ת�Ƕ����Ϊ 10��
#define HEXAPOD_MIN_Z_ROTATE -25.0f / 180 * PI // ��X����ת�Ƕ���СΪ-25��
#define HEXAPOD_MAX_Z_ROTATE 25.0f / 180 * PI  // ��X����ת�Ƕ����Ϊ 25��
typedef enum
{
    HEXAPOD_MOVE,
    HEXAPOD_BODY_ANGEL_CONTROL,
    HEXAPOD_BODY_POS_CONTROL,
} Hexapod_mode_e;

class Hexapod
{
public:
    Leg legs[6];                 // ������
    Velocity velocity;           // �������ٶ�
    Hexapod_mode_e mode; // ������ģʽ
    Position3 body_pos;
    Position3 body_angle;
    void Init();          
    void velocity_cal(const RC_remote_data_t &remote_data);
    void body_position_cal(const RC_remote_data_t &remote_data);
    void body_angle_cal(const RC_remote_data_t &remote_data);
    void mode_select(const RC_remote_data_t &remote_data);
    void body_angle_and_pos_zero(const RC_remote_data_t &remote_data);
    void move(uint32_t round_time);
};

#endif
