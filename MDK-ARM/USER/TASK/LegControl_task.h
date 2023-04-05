#ifndef LEGCONTROL_TASK_H
#define LEGCONTROL_TASK_H

#define LEG_JOINT2_OFFSET PI/2
#define LEG_JOINT3_OFFSET -2 * PI / 9
#define HEXAPOD_MAX_HEIGHT 70.0f
#define HEXAPOD_MIN_HEIGHT -70.0f

#define HEXAPOD_MIN_X_ROTATE -15.0f/180*PI //��X����ת�Ƕ���СΪ-15��
#define HEXAPOD_MAX_X_ROTATE 15.0f/180*PI  //��X����ת�Ƕ����Ϊ 15��
#define HEXAPOD_MIN_Y_ROTATE -10.0f/180*PI //��X����ת�Ƕ���СΪ-10��
#define HEXAPOD_MAX_Y_ROTATE 10.0f/180*PI  //��X����ת�Ƕ����Ϊ 10��
#define HEXAPOD_MIN_Z_ROTATE -25.0f/180*PI //��X����ת�Ƕ���СΪ-25��
#define HEXAPOD_MAX_Z_ROTATE 25.0f/180*PI  //��X����ת�Ƕ����Ϊ 25��
typedef enum
{
    HEXAPOD_MOVE,
    HEXAPOD_DANCE,
}Hexapod_mode_e;

#endif
