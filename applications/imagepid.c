#include "include.h"
#include "imagepid.h"
#include "usart.h"

u8 get_command = 0;
u16 roll_out = 0;
u16 pitch_out = 0;

PRPID pitch_pid ={
    .Kp=0.65,
    .Ki=0,
    .Kd=18,
    .integral = 0,
    .act = 1500,
};
PRPID roll_pid ={
    .Kp=0.65,
    .Ki=0,
    .Kd=18,
    .integral = 0,
    .act = 1500,
};
PRPID yaw_pid={
    .Kp=0,
    .Ki=0,
    .integral = 0,
    .Kd=0,
    .act = 1500,
};

void pid_duty(void)
{
    if(get_command)
    {
        get_command = 0;

        roll_out = 1500 - PID_realize(&roll_pid,0,RX_auto[0],0);
        pitch_out = 1500 + PID_realize(&pitch_pid,0,RX_auto[1],1);    
    }
}

#define PITCH_ROLL_MAX_CTRL_ROAD  55
#define PITCH_ROLL_MAX_CTRL_BOX  55             //75
#define NOT_ACT_ERROR        10


#define PIT_MAX_ROAD    55
#define ROL_MAX_ROAD   55

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
        if(a->act > ROL_MAX_ROAD)  a->act=ROL_MAX_ROAD;
        if(a->act < -ROL_MAX_ROAD) a->act=-ROL_MAX_ROAD;
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
        if(a->act > PIT_MAX_ROAD)  a->act=PIT_MAX_ROAD;
        if(a->act < -PIT_MAX_ROAD) a->act=-PIT_MAX_ROAD;
        //}
    }
    return(a->act);                    //返回增量值
}
