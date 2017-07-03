#ifndef __IMAGEPID_H
#define __IMAGEPID_H
#include "include.h"

typedef struct
{
    float Kp; //比例常数
    float Ki; //积分常数
    float Kd; //微分常数
    float LastError;    //Error[-1]
    float PrevError;    //Error[-2]
    float Error;
    float integral;
    float act;

}PRPID;


extern u8 get_command;
extern u16 roll_out;
extern u16 pitch_out;
extern PRPID pitch_pid,roll_pid,yaw_pid;

void pid_duty(void); 
float PID_realize(PRPID *a,int setlocation,int nowlocation,char p_r);
    
#endif
