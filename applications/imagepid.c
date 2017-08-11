#include "include.h"
#include "imagepid.h"
#include "usart.h"

u8 get_command = 0;
u16 roll_out = 0;
u16 pitch_out = 0;

PRPID pitch_pid ={
    .Kp=0.75,
    .Ki=0,
    .Kd=19.5,
    .integral = 0,
    .act = PITCH_MID,
};
PRPID roll_pid ={
    .Kp=0.75,
    .Ki=0,
    .Kd=19.5,
    .integral = 0,
    .act = ROLL_MID,
};

PRPID yaw_pid={
    .Kp=0,
    .Ki=0,
    .integral = 0,
    .Kd=0,
    .act = 1500,
};


//1
short int far_1 = 50;   //50cm 算远距
short int far_no_pid = 45;  //远距时调整大小

//2


//3
//远距pid
short int far_3 = 50;
PRPID pitch_pid_far ={
    .Kp=0.75,
    .Ki=0,
    .Kd=19.5,
    .integral = 0,
    .act = PITCH_MID,
};
PRPID roll_pid_far ={
    .Kp=0.75,
    .Ki=0,
    .Kd=19.5,
    .integral = 0,
    .act = ROLL_MID,
};


void pid_duty(void)
{
    if(get_command)
    {
        get_command = 0;
        //4种  一种大于某个值直接最大速度   或者吧误差搞成非线性 另外一种切换PID 4直接PID计算调参数
#ifdef UNLINE_PID
        
        //1
        if (RX_auto[0] > (1500+far_1))
            roll_out = ROLL_MID + far_no_pid;
        else if(RX_auto[0] < (1500-far_1))
            roll_out = ROLL_MID - far_no_pid;
        else
            roll_out = ROLL_MID - PID_realize(&roll_pid,0,RX_auto[0]-1500,0);

        if (RX_auto[1] > (1500+far_1))
            pitch_out = PITCH_MID - far_no_pid;
        else if(RX_auto[1] < (1500-far_1))
            pitch_out = PITCH_MID + far_no_pid;
        else
            pitch_out = PITCH_MID + PID_realize(&pitch_pid,0,RX_auto[1]-1500,1); 

        //2
        int error_temp1 = RX_auto[0]-1500;
        if(ABS(error_temp1) > 20)
        {
            if (ABS(error_temp1) > 100)
                error_temp1 = 2*error_temp1;
            else if(ABS(error_temp1) > 80)
                error_temp1 = 1.7*error_temp1;
            else if(ABS(error_temp1) > 60)
                error_temp1 = 1.45*error_temp1;
            else if(ABS(error_temp1) > 40)
                error_temp1 = 1.25*error_temp1;
            else 
                error_temp1 = 1.1*error_temp1;
        }
        RX_auto[0] = error_temp1 + 1500;
        error_temp1 = RX_auto[1] - 1500;
        if(ABS(error_temp1) > 20)
        {
            if (ABS(error_temp1) > 100)
                error_temp1 = 2*error_temp1;
            else if(ABS(error_temp1) > 80)
                error_temp1 = 1.7*error_temp1;
            else if(ABS(error_temp1) > 60)
                error_temp1 = 1.45*error_temp1;
            else if(ABS(error_temp1) > 40)
                error_temp1 = 1.25*error_temp1;
            else 
                error_temp1 = 1.1*error_temp1;
        }
        RX_auto[1] = error_temp1 + 1500;
        roll_out = ROLL_MID - PID_realize(&roll_pid,0,RX_auto[0]-1500,0);
        pitch_out = PITCH_MID + PID_realize(&pitch_pid,0,RX_auto[1]-1500,1);  
        
        //3
        if (RX_auto[0] > (1500+far_3))
        {
          //  roll_pid  可能要消除另外一个PID的积累》
            roll_out = ROLL_MID - PID_realize(&roll_pid_far,0,RX_auto[0]-1500,0);
        }
        else
        {
            roll_out = ROLL_MID - PID_realize(&roll_pid,0,RX_auto[0]-1500,0);
        }
        if (RX_auto[1] > far_3)
        {
            //  roll_pid  可能要消除另外一个PID的积累》
            pitch_out = PITCH_MID - PID_realize(&pitch_pid_far,0,RX_auto[1]-1500,0);
        }
        else
        {
            pitch_out = PITCH_MID - PID_realize(&pitch_pid,0,RX_auto[1]-1500,0);
        }
#else
        //4
        roll_out = ROLL_MID - PID_realize(&roll_pid,0,RX_auto[0]-1500,0);
        pitch_out = PITCH_MID + PID_realize(&pitch_pid,0,RX_auto[1]-1500,1);  
#endif
    }
}

#define PITCH_ROLL_MAX_CTRL_ROAD  55
#define PITCH_ROLL_MAX_CTRL_BOX  55             //75
#define NOT_ACT_ERROR        10


#define PIT_MAX_POINT   100
#define ROL_MAX_POINT   100

#define ROL_MAX_BOX     55
#define PIT_MAX_BOX     55


//roll 0 P 1
float PID_realize(PRPID *a,int setlocation,int nowlocation,char p_r)
{


    a->Error = setlocation- nowlocation;            //增量计算
    //if(a->Error < NOT_ACT_ERROR)
    //    a->act = 0;
    //else

    a->integral = a->integral + a->Error;

    //if(use_i_flag == 1)  //定点的时候要加I
    //    a->Ki = 0.001;
    //else
    //{
    //    a->Ki = 0;
    //    a->integral = 0;
    //}
    a->act= a->Kp * a->Error + a->Ki * a->integral + a->Kd * (a->Error - a->LastError);
    a->LastError = a->Error ;           //存储误差，用于下次计算

    if(p_r == 0)        //roll
    {
        //if(use_i_flag == 1)             //box    定点加I
        //{  
        //    if(a->act > ROL_MAX_BOX)  a->act=ROL_MAX_BOX;
        //    if(a->act < -ROL_MAX_BOX) a->act=-ROL_MAX_BOX;
        //}
        //else        //road
        //{
        if(a->act > ROL_MAX_POINT)  a->act=ROL_MAX_POINT;
        if(a->act < -ROL_MAX_POINT) a->act=-ROL_MAX_POINT;
        // }
    }
    else        //p
    {
        //if(use_i_flag == 1)
        //{
        //    if(a->act > PIT_MAX_BOX)  a->act=PIT_MAX_BOX;
        //    if(a->act < -PIT_MAX_BOX) a->act=-PIT_MAX_BOX;
        //}
        //else
        //{
        if(a->act > PIT_MAX_POINT)  a->act=PIT_MAX_POINT;
        if(a->act < -PIT_MAX_POINT) a->act=-PIT_MAX_POINT;
        //}
    }
    return(a->act);                    //返回增量值
}
