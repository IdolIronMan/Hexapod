#include "gait_prg.h"
#include "cmsis_os.h"
#include <cmath>
#include "remote.h"
#include "my_math.h"
#include "arm_math.h"
using namespace std;

// ȫ�ֱ���
action actions[6];            // ������
static Position3 Pws[6];             // ��е��ĩ��վ��״̬���������ʼ�˵�λ��
static Position3 P_legs[6];          // ������е����ʼ������ڻ��������ĵ�����
static Velocity velocity;            // �������ٶȽṹ��
static RC_remote_data_t remote_data; // ң��������
static Position3 CEN;                // ��Բ�ĵ�����
static float R_pace;                 // ������С����λmm��
uint32_t tic,toc;

extern uint32_t LegControl_round;   //���ƻغ�

// ����
static Position3 fkine(Thetas thetas);
static Thetas ikine(Position3 pos);

Gait_prg::Gait_prg()
{
    // �����е���������ʼ�˵�ĩ������
    Pws[0] = fkine(Thetas(PI / 4, THETA_STAND_2, THETA_STAND_3));
    Pws[1] = fkine(Thetas(0, THETA_STAND_2, THETA_STAND_3));
    Pws[2] = fkine(Thetas(-PI / 4, THETA_STAND_2, THETA_STAND_3));
    Pws[3] = fkine(Thetas(3 * PI / 4, THETA_STAND_2, THETA_STAND_3));
    Pws[4] = fkine(Thetas(PI, THETA_STAND_2, THETA_STAND_3));
    Pws[5] = fkine(Thetas(5 * PI / 4, THETA_STAND_2, THETA_STAND_3));
    // ���������е����ʼ������ڻ��������ĵ�����
    P_legs[0] = Position3(CHASSIS_FRONT_WIDTH / 2, CHASSIS_LEN / 2, 0);
    P_legs[1] = Position3(CHASSIS_WIDTH / 2, 0, 0);
    P_legs[2] = Position3(CHASSIS_FRONT_WIDTH / 2, -CHASSIS_LEN / 2, 0);
    P_legs[3] = Position3(-CHASSIS_FRONT_WIDTH / 2, CHASSIS_LEN / 2, 0);
    P_legs[4] = Position3(-CHASSIS_WIDTH / 2, 0, 0);
    P_legs[5] = Position3(-CHASSIS_FRONT_WIDTH / 2, -CHASSIS_LEN / 2, 0);
}


/*
 * ���˶�����
 */
static Position3 fkine(Thetas thetas)
{
    Position3 position3(cos(thetas.angle[0]) * (LEG_LEN1 + LEG_LEN3 * cos(thetas.angle[1] + thetas.angle[2]) + LEG_LEN2 * cos(thetas.angle[1])),
                        sin(thetas.angle[0]) * (LEG_LEN1 + LEG_LEN3 * cos(thetas.angle[1] + thetas.angle[2]) + LEG_LEN2 * cos(thetas.angle[1])),
                        LEG_LEN3 * sin(thetas.angle[1] + thetas.angle[2]) + LEG_LEN2 * sin(thetas.angle[1]));

    return position3;
}

/*
 * ���˶�����
 */
static Thetas ikine(Position3 pos)
{
    float R = sqrt(pow(pos.x, 2) + pow(pos.y, 2));
    float Lr = sqrt(pow(pos.z, 2) + pow(pos.y, 2));
    float alpha_r = atan(-pos.z / (R - LEG_LEN1));
    float alpha1 = acos((pow(LEG_LEN2, 2) + pow(Lr, 2) - pow(LEG_LEN3, 2)) / (2 * Lr * LEG_LEN2));
    float alpha2 = acos((pow(Lr,2)+pow(LEG_LEN3,2)-pow(LEG_LEN2,2))/(2*Lr*LEG_LEN3));
    Thetas thetas(atan2(pos.y, pos.x), alpha1 - alpha_r, -(alpha1 + alpha2));
    
    return thetas;
}
/*
 * ����Բ��λ�úͲ�����С
 */
void Gait_prg::CEN_and_pace_cal()
{
    velocity.Vx = remote_data.right_HRZC;
    velocity.Vy = remote_data.right_VETC;
    velocity.omega = remote_data.left_HRZC;
    // ����Ԥ�����������0
    if (velocity.Vx == 0)
        velocity.Vx += 0.001f;
    if (velocity.Vy == 0)
        velocity.Vy += 0.001f;
    if (velocity.omega == 0)
        velocity.omega += 0.001f;
    // ����Բ��ģ��
    float module_CEN = K_CEN / velocity.omega * sqrt(pow(velocity.Vx, 2) + pow(velocity.Vy, 2));
    if (velocity.omega > 0)
    {
        if (velocity.Vx * velocity.Vy > 0) // �ٶ�������1,3����
            CEN.x = -sqrt(pow(module_CEN, 2) / (1 + pow(velocity.Vx, 2) / pow(velocity.Vy, 2)));
        else // �ٶ�������2,4����
            CEN.x = sqrt(pow(module_CEN, 2) / (1 + pow(velocity.Vx, 2) / pow(velocity.Vy, 2)));
    }
    else
    {
        if (velocity.Vx * velocity.Vy > 0) // �ٶ�������1,3����
            CEN.x = sqrt(pow(module_CEN, 2) / (1 + pow(velocity.Vx, 2) / pow(velocity.Vy, 2)));
        else // �ٶ�������2,4����
            CEN.x = -sqrt(pow(module_CEN, 2) / (1 + pow(velocity.Vx, 2) / pow(velocity.Vy, 2)));
    }
    // ���㲽����С
    R_pace = KR_1 * abs(velocity.omega) + KR_2 * sqrt(pow(velocity.Vx, 2) + pow(velocity.Vy, 2));
    if (R_pace > 50)
        R_pace = 50; // ���Ʋ�����С
    CEN.y = -CEN.x * velocity.Vx / velocity.Vy;
}

/*
 * ��̬�滮
 */
void Gait_prg::gait_proggraming()
{
    Position3 Vec_CEN2leg_ends[6];    // Բ�ĵ��Ȳ�ĩ�˵�����
    float angle_off[6];               // Բ�����е��ĩ�˵ļн�
    float norm_CEN2legs[6];           // Բ�ĵ���е��ĩ�˵�ģ��
    float Rp_ratios[6];               // ������е�Ȳ�̬�滮�Ĵ�С����
    Position3 Vec_Leg_Start2CEN_s[6]; // �Ȳ���ʼ�˵�Բ����ʼ�˵�����
    for (int i = 0; i < 6; i++)
    {
        Vec_CEN2leg_ends[i] = Pws[i] + P_legs[i] - CEN;                                         // ����Բ�ĵ�ÿ���Ȳ�ĩ�˵�����
        angle_off[i] = atan2(Vec_CEN2leg_ends[i].y, Vec_CEN2leg_ends[i].x);                     // ����Բ�����е��ĩ�˵ļн�
        norm_CEN2legs[i] = sqrt(pow(Vec_CEN2leg_ends[i].x, 2) + pow(Vec_CEN2leg_ends[i].y, 2)); // ����Բ�����е��ĩ�˵�ģ��
        Vec_Leg_Start2CEN_s[i] = CEN - P_legs[i];                                               // �����Ȳ���ʼ�˵�Բ����ʼ�˵�����
    }
    float max_norm_CEN2legs = 0;
    for (int i = 0; i < 6; i++)
        if (norm_CEN2legs[i] > max_norm_CEN2legs)
            max_norm_CEN2legs = norm_CEN2legs[i]; // ѡ�����ģ��

    float R_paces[6]; // ������е�ȵĲ���
    for (int i = 0; i < 6; i++)
    {
        Rp_ratios[i] = norm_CEN2legs[i] / max_norm_CEN2legs; // ���������е�Ȳ�̬�滮�Ĵ�С����
        R_paces[i] = Rp_ratios[i] * R_pace;                  // ���������е�Ȳ�̬�Ĵ�С
    }
    float d_theta = 2 * R_paces[0] / norm_CEN2legs[0]; // �����е����һ����Բ�ĹյĽǶȣ������һ�������������
    float step_size = d_theta / (N_POINTS / 2);


    /*********�ȶ���1��3��5����̬�滮***********/
    float angle_t; // ���ڼ���õ�ĽǶ�
    float y_temp;  // ���ڼ���z��߶ȵ���ʱ����
    Position3 point; //���ڴ洢ĩ�������
		tic = xTaskGetTickCount();
    for (int i = 0; i < 5; i += 2)
    {
        if (LegControl_round < N_POINTS / 2) // 0-9, ���°�Բ
        {
            angle_t = angle_off[i] + d_theta / 2 - step_size * LegControl_round;  // ���������ĽǶ�
            point.x = Vec_Leg_Start2CEN_s[i].x + norm_CEN2legs[i] * cos(angle_t); // ����������x������(����ڻ�е����ʼ��)
            point.y = Vec_Leg_Start2CEN_s[i].y + norm_CEN2legs[i] * sin(angle_t); // ����������y������(����ڻ�е����ʼ��)
            point.z = Pws[i].z;                                                   // ����ǰ�벿�����ŵ��棬��ȡվ��ʱ��z������
        }
        else // 10-19�����ϰ�Բ
        {
            angle_t = angle_off[i] - d_theta / 2 + step_size * (LegControl_round - N_POINTS/2); // ���������ĽǶ�
            point.x = Vec_Leg_Start2CEN_s[i].x + norm_CEN2legs[i] * cos(angle_t);       // ����������x������(����ڻ�е����ʼ��)
            point.y = Vec_Leg_Start2CEN_s[i].y + norm_CEN2legs[i] * sin(angle_t);       // ����������y������(����ڻ�е����ʼ��)
            y_temp = -R_pace + (LegControl_round - N_POINTS/2) * (R_pace * 4 / N_POINTS);
            // ����Բ�Ĵ�С��Сz��߶�,��Ǩ������ϵ����е��ĩ��,��Ϊվ��ʱz�ᶼ��һ���ģ����������һ��Pw����
            point.z = sqrt(pow(R_pace, 2) - pow(y_temp, 2)) * Rp_ratios[i] + Pws[i].z;
        }
        actions[i].thetas[LegControl_round] = ikine(point);
    }
		
    /*********����2��4��6����̬�滮***********/
    for(int i = 1; i <=5;i+=2)
    {
        if(LegControl_round < N_POINTS / 2) // 0-9, ���ϰ�Բ
        {
            angle_t = angle_off[i] - d_theta / 2 + step_size * LegControl_round; // ���������ĽǶ�
            point.x = Vec_Leg_Start2CEN_s[i].x + norm_CEN2legs[i] * cos(angle_t);       // ����������x������(����ڻ�е����ʼ��)
            point.y = Vec_Leg_Start2CEN_s[i].y + norm_CEN2legs[i] * sin(angle_t);       // ����������y������(����ڻ�е����ʼ��)
            y_temp = -R_pace + LegControl_round * (R_pace * 4 / N_POINTS);
            // ����Բ�Ĵ�С��Сz��߶�,��Ǩ������ϵ����е��ĩ��,��Ϊվ��ʱz�ᶼ��һ���ģ����������һ��Pw����
            point.z = sqrt(pow(R_pace, 2) - pow(y_temp, 2)) * Rp_ratios[i] + Pws[i].z;
        }
        else // 10-19, ���°�Բ
        {
            angle_t = angle_off[i] + d_theta / 2 - step_size * (LegControl_round-N_POINTS/2);  // ���������ĽǶ�
            point.x = Vec_Leg_Start2CEN_s[i].x + norm_CEN2legs[i] * cos(angle_t); // ����������x������(����ڻ�е����ʼ��)
            point.y = Vec_Leg_Start2CEN_s[i].y + norm_CEN2legs[i] * sin(angle_t); // ����������y������(����ڻ�е����ʼ��)
            point.z = Pws[i].z; 
        }
        actions[i].thetas[LegControl_round] = ikine(point);
    }
		toc = xTaskGetTickCount()-tic;
}
