#include "LegControl_task.h"
#include "gait_prg.h"
#include "cmsis_os.h"
#include "leg.h"
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "bsp.h"
#include "remote.h"
#include "dwt_delay_us.h"

using namespace std;

// ȫ�ֱ���
uint32_t LegControl_round; // �����˻غ���
Leg legs[6];			   // ������
Velocity velocity;		   // �������ٶ�
Gait_prg gait_prg;		   // ��̬�滮
uint32_t round_time;	   // �غ�ʱ��
Thetas leg_offset[6];	   // �Ȳ��ؽڽ�ƫ�ƣ����ڽ������Ի����˱���ĽǶȻ�������Զ������ĽǶ�

// ����
static void Legs_Init(void);
static void Hexapod_remote_deal(void);
static void Hexapod_move(uint32_t round_time);
extern "C"
{
	void LegControl_Task(void const *argument)
	{
		Legs_Init();
		gait_prg.Init();
		osDelay(100);
		static uint32_t code_time_start, code_time_end, code_time; // ���ڼ����������ʱ�䣬��֤�����һ��ʱ����һ��
		while (1)
		{
			code_time_start = xTaskGetTickCount(); // ��ȡ��ǰsystickʱ��

			Hexapod_remote_deal();
			if (velocity.omega >= 0)
				LegControl_round = (++LegControl_round) % N_POINTS; // ���ƻغ�������
			else
			{
				if (LegControl_round == 0)
					LegControl_round = N_POINTS - 1;
				else
					LegControl_round--;
			}

			gait_prg.CEN_and_pace_cal(velocity);
			gait_prg.gait_proggraming();
			round_time = gait_prg.get_pace_time() / N_POINTS;
			Hexapod_move(round_time);
			// �����������ʱ��
			code_time_end = xTaskGetTickCount();		 // ��ȡ��ǰsystickʱ��
			code_time = code_time_end - code_time_start; // �����ȡ��������ʱ��
			if (code_time < round_time)
				osDelay(round_time - code_time); // ��֤����ִ�����ڵ��ڻغ�ʱ��
			else
				osDelay(1); // ������ʱ1ms
		}
	}
}

// ��ʼ���Ȳ���������ʼ�����ڣ�ʹ�ܴ��ڷ���
static void Legs_Init(void)
{
	legs[0] = Leg(&huart1);
	legs[1] = Leg(&huart2);
	legs[2] = Leg(&huart3);
	legs[3] = Leg(&huart4);
	legs[4] = Leg(&huart5);
	legs[5] = Leg(&huart6);
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	MX_USART6_UART_Init();
	leg_offset[0] = Thetas(PI / 4, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
	leg_offset[1] = Thetas(0.0f, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
	leg_offset[2] = Thetas(-PI / 4, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
	leg_offset[3] = Thetas(3 * PI / 4, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
	leg_offset[4] = Thetas(PI, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
	leg_offset[5] = Thetas(-3 * PI / 4, LEG_JOINT2_OFFSET, LEG_JOINT3_OFFSET);
}



// ����������ٶ�
static void Hexapod_velocity_cal(const RC_remote_data_t &remote_data)
{
	velocity.Vx = 0.2 * remote_data.right_HRZC;
	velocity.Vy = 0.2 * remote_data.right_VETC;
	velocity.omega = -0.3 * remote_data.left_HRZC;
}

float height;
static void Hexapod_height_cal(const RC_remote_data_t &remote_data)
{
	height -= 0.03*remote_data.left_VETC;
	value_limit(height,HEXAPOD_MIN_HEIGHT,HEXAPOD_MAX_HEIGHT);
	gait_prg.set_height(height);
}

static void Hexapod_remote_deal(void)
{
	static RC_remote_data_t remote_data;
	remote_data = Remote_read_data();
	Hexapod_velocity_cal(remote_data);
	Hexapod_height_cal(remote_data);
}



/*
 * @brief �û����˶�����
 * @param round_time �غ�ʱ�䣬��λms
 */
static void Hexapod_move(uint32_t round_time)
{
	// ������1-6�ĽǶ�
	for (int i = 0; i < 6; i++)
	{
		legs[i].set_thetas((gait_prg.actions[i].thetas[LegControl_round]) - leg_offset[i]); // ���û�е�ȽǶ�
		legs[i].set_time(round_time);														// ���û�е���ƶ�ʱ��
	}
	// ������5�ĽǶ�
	if (gait_prg.actions[4].thetas[LegControl_round].angle[0] <= 0)
	{
		Thetas theta_temp;
		theta_temp = (gait_prg.actions[4].thetas[LegControl_round]) - leg_offset[4];
		theta_temp.angle[0] += 2 * PI;
		legs[4].set_thetas(theta_temp); // ���û�е�ȽǶ�
	}


	legs[0].move_UART();
	legs[1].move_DMA();
	legs[2].move_DMA();
	legs[3].move_DMA();
	legs[4].move_DMA();
	legs[5].move_DMA();
}
