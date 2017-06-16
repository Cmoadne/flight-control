/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : ctrl.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\applications
// 作者   : Cmoande
// 日期   : 2017/05/15
// 描述   : 飞控控制
// 备注   :
// 版本   : V0.0 2017.5.15 初始版本
更改CTRL_2  期望XYZ赋值
更改Thr_Ctrl 定高油门赋值
******************************************************************/ 
#include "ctrl.h"
#include "height_ctrl.h"
#include "ultrasonic.h"
#include "include.h"

ctrl_t ctrl_1;
ctrl_t ctrl_2;

void Ctrl_Para_Init()		//设置默认参数
{
    //====================================
    ctrl_1.PID[PIDROLL].kdamp  = 1;
    ctrl_1.PID[PIDPITCH].kdamp = 1;
    ctrl_1.PID[PIDYAW].kdamp 	 = 1;

    ctrl_1.FB = 0.20;   //外  0<fb<1
}

xyz_f_t except_A = {0,0,0};
xyz_f_t ctrl_angle_offset = {0,0,0};
xyz_f_t compensation;

void CTRL_2(float T)
{
    // 	static xyz_f_t acc_no_g;
    // 	static xyz_f_t acc_no_g_lpf;
    //=========================== 期望角度 ========================================
    //6.16更改
    if (NS == NS_FINISH_AUTO)   //完全控制，去掉死区
    {
        except_A.x  = MAX_CTRL_ANGLE  * (CH_filter[ROL]/500.0f );   //30
        except_A.y  = MAX_CTRL_ANGLE  * (-CH_filter[PIT]/500.0f );  //30
    }
    else
    {
        except_A.x  = MAX_CTRL_ANGLE  *( my_deathzoom( ( CH_filter[ROL]) ,30 )/500.0f );   //30
        except_A.y  = MAX_CTRL_ANGLE  *( my_deathzoom( (-CH_filter[PIT]) ,30 )/500.0f );  //30
    }


    int expect_yaw_temp = 0;

    if( Thr_Low == 0 )
    {
        //方案1  直接yaw角期望加90度
        //if (turn_right_flag)
        //{
        //    turn_right_flag = 0;
        //    except_A.z = except_A.z + turn_degree;
        //}
        //else
        //{
        //    if (start_fly_flag && ((NS == NS_FINISH_RASPBERRY) || NS == NS_NOW_TEST))    //起飞不转yaw
        //        expect_yaw_temp = 0;
        //    else
        //        expect_yaw_temp = my_deathzoom_2( (CH_filter[YAW]) ,10 );       //死区
        //    if ( expect_yaw_temp != 0)      
        //        except_A.z += (s16)( MAX_CTRL_YAW_SPEED *( expect_yaw_temp/500.0f ) ) *T ;  //50
        //    else
        //        except_A.z = Yaw;       //可能要改
        //}
        //方案2   yaw角期望缓慢增加90度


        //让YAW马上停止
        static char lock_yaw_flag = 0;
        expect_yaw_temp = my_deathzoom_2( (CH_filter[YAW]) ,20 );       //死区
        if ( expect_yaw_temp != 0)  
        {   
            lock_yaw_flag = 1;
            except_A.z += (s16)( MAX_CTRL_YAW_SPEED *( expect_yaw_temp/500.0f ) ) *T ;  //50
        }
        else if (lock_yaw_flag == 1)
        {
            lock_yaw_flag = 0;
            except_A.z = Yaw;       //可能要改
        }
        else
            except_A.z = except_A.z;

    }
    else
    {
        except_A.z += 1 *3.14 *T *( Yaw - except_A.z );
    }
    //6.16更改结束

    except_A.z = To_180_degrees(except_A.z);
    //==============================================================================
    // 	acc_no_g.x =  mpu6050.Acc.x - reference_v.x *4096;
    // 	acc_no_g.y =  mpu6050.Acc.y - reference_v.y *4096;
    // 	acc_no_g.z =  mpu6050.Acc.z - reference_v.z *4096;
    // 	
    // 	acc_no_g_lpf.x += 0.5f *T *3.14f * ( acc_no_g.x - acc_no_g_lpf.x );
    // 	acc_no_g_lpf.y += 0.5f *T *3.14f * ( acc_no_g.y - acc_no_g_lpf.y );
    // 	acc_no_g_lpf.z += 0.5f *T *3.14f * ( acc_no_g.z - acc_no_g_lpf.z );
    // 	
    // 	compensation.x = LIMIT( 0.003f *acc_no_g_lpf.x, -10,10 );
    // 	compensation.y = LIMIT( 0.003f *acc_no_g_lpf.y, -10,10 );
    // 	compensation.z = LIMIT( 0.003f *acc_no_g_lpf.z, -10,10 );
    //==============================================================================	

    /* 得到角度误差 */
    ctrl_2.err.x =  To_180_degrees( ctrl_angle_offset.x + except_A.x - Roll  );
    ctrl_2.err.y =  To_180_degrees( ctrl_angle_offset.y + except_A.y - Pitch );
    ctrl_2.err.z =  To_180_degrees( ctrl_angle_offset.z + except_A.z - Yaw	 );
    /* 计算角度误差权重 */
    ctrl_2.err_weight.x = ABS(ctrl_2.err.x)/ANGLE_TO_MAX_AS;
    ctrl_2.err_weight.y = ABS(ctrl_2.err.y)/ANGLE_TO_MAX_AS;
    ctrl_2.err_weight.z = ABS(ctrl_2.err.z)/ANGLE_TO_MAX_AS;
    /* 角度误差微分（跟随误差曲线变化）*/
    ctrl_2.err_d.x = 10 *ctrl_2.PID[PIDROLL].kd  *(ctrl_2.err.x - ctrl_2.err_old.x) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.x );
    ctrl_2.err_d.y = 10 *ctrl_2.PID[PIDPITCH].kd *(ctrl_2.err.y - ctrl_2.err_old.y) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.y );
    ctrl_2.err_d.z = 10 *ctrl_2.PID[PIDYAW].kd 	 *(ctrl_2.err.z - ctrl_2.err_old.z) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.z );
    /* 角度误差积分 */
    ctrl_2.err_i.x += ctrl_2.PID[PIDROLL].ki  *ctrl_2.err.x *T;
    ctrl_2.err_i.y += ctrl_2.PID[PIDPITCH].ki *ctrl_2.err.y *T;
    ctrl_2.err_i.z += ctrl_2.PID[PIDYAW].ki 	*ctrl_2.err.z *T;
    /* 角度误差积分分离 */
    ctrl_2.eliminate_I.x = Thr_Weight *CTRL_2_INT_LIMIT;
    ctrl_2.eliminate_I.y = Thr_Weight *CTRL_2_INT_LIMIT;
    ctrl_2.eliminate_I.z = Thr_Weight *CTRL_2_INT_LIMIT;
    /* 角度误差积分限幅 */
    ctrl_2.err_i.x = LIMIT( ctrl_2.err_i.x, -ctrl_2.eliminate_I.x,ctrl_2.eliminate_I.x );
    ctrl_2.err_i.y = LIMIT( ctrl_2.err_i.y, -ctrl_2.eliminate_I.y,ctrl_2.eliminate_I.y );
    ctrl_2.err_i.z = LIMIT( ctrl_2.err_i.z, -ctrl_2.eliminate_I.z,ctrl_2.eliminate_I.z );
    /* 对用于计算比例项输出的角度误差限幅 */
    ctrl_2.err.x = LIMIT( ctrl_2.err.x, -90, 90 );
    ctrl_2.err.y = LIMIT( ctrl_2.err.y, -90, 90 );
    ctrl_2.err.z = LIMIT( ctrl_2.err.z, -90, 90 );
    /* 角度PID输出 */
    ctrl_2.out.x = ctrl_2.PID[PIDROLL].kp  *( ctrl_2.err.x + ctrl_2.err_d.x + ctrl_2.err_i.x );	//rol
    ctrl_2.out.y = ctrl_2.PID[PIDPITCH].kp *( ctrl_2.err.y + ctrl_2.err_d.y + ctrl_2.err_i.y );  //pit
    ctrl_2.out.z = ctrl_2.PID[PIDYAW].kp   *( ctrl_2.err.z + ctrl_2.err_d.z + ctrl_2.err_i.z );
    /* 记录历史数据 */	
    ctrl_2.err_old.x = ctrl_2.err.x;
    ctrl_2.err_old.y = ctrl_2.err.y;
    ctrl_2.err_old.z = ctrl_2.err.z;

}

xyz_f_t except_AS;
float g_old[ITEMS];
void CTRL_1(float T)  //x roll,y pitch,z yaw
{
    xyz_f_t EXP_LPF_TMP;
    /* 给期望（目标）角速度 */
    EXP_LPF_TMP.x = MAX_CTRL_ASPEED *(ctrl_2.out.x/ANGLE_TO_MAX_AS);//*( (CH_filter[0])/500.0f );//
    EXP_LPF_TMP.y = MAX_CTRL_ASPEED *(ctrl_2.out.y/ANGLE_TO_MAX_AS);//*( (CH_filter[1])/500.0f );//
    EXP_LPF_TMP.z = MAX_CTRL_ASPEED *(ctrl_2.out.z/ANGLE_TO_MAX_AS);

    except_AS.x = EXP_LPF_TMP.x;//20 *3.14 *T *( EXP_LPF_TMP.x - except_AS.x );//
    except_AS.y = EXP_LPF_TMP.y;//20 *3.14 *T *( EXP_LPF_TMP.y - except_AS.y );//
    except_AS.z = EXP_LPF_TMP.z;//20 *3.14 *T *( EXP_LPF_TMP.z - except_AS.z );//
    /* 期望角速度限幅 */
    except_AS.x = LIMIT(except_AS.x, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );
    except_AS.y = LIMIT(except_AS.y, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );
    except_AS.z = LIMIT(except_AS.z, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );

    /* 角速度直接微分（角加速度），负反馈可形成角速度的阻尼（阻碍角速度的变化）*/
    ctrl_1.damp.x = ( mpu6050.Gyro_deg.x - g_old[A_X]) *( 0.002f/T );//ctrl_1.PID[PIDROLL].kdamp
    ctrl_1.damp.y = (-mpu6050.Gyro_deg.y - g_old[A_Y]) *( 0.002f/T );//ctrl_1.PID[PIDPITCH].kdamp *
    ctrl_1.damp.z = (-mpu6050.Gyro_deg.z - g_old[A_Z]) *( 0.002f/T );//ctrl_1.PID[PIDYAW].kdamp	 *
    /* 角速度误差 */
    ctrl_1.err.x =  ( except_AS.x - mpu6050.Gyro_deg.x ) *(300.0f/MAX_CTRL_ASPEED);
    ctrl_1.err.y =  ( except_AS.y + mpu6050.Gyro_deg.y ) *(300.0f/MAX_CTRL_ASPEED);  //-y
    ctrl_1.err.z =  ( except_AS.z + mpu6050.Gyro_deg.z ) *(300.0f/MAX_CTRL_ASPEED);	 //-z
    /* 角速度误差权重 */
    ctrl_1.err_weight.x = ABS(ctrl_1.err.x)/MAX_CTRL_ASPEED;
    ctrl_1.err_weight.y = ABS(ctrl_1.err.y)/MAX_CTRL_ASPEED;
    ctrl_1.err_weight.z = ABS(ctrl_1.err.z)/MAX_CTRL_YAW_SPEED;
    /* 角速度微分 */
    ctrl_1.err_d.x = ( ctrl_1.PID[PIDROLL].kd  *( -10 *ctrl_1.damp.x) *( 0.002f/T ) );
    ctrl_1.err_d.y = ( ctrl_1.PID[PIDPITCH].kd *( -10 *ctrl_1.damp.y) *( 0.002f/T ) );
    ctrl_1.err_d.z = ( ctrl_1.PID[PIDYAW].kd   *( -10 *ctrl_1.damp.z) *( 0.002f/T ) );

    //	ctrl_1.err_d.x += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDROLL].kd  *(ctrl_1.err.x - ctrl_1.err_old.x) *( 0.002f/T ) - ctrl_1.err_d.x);
    //	ctrl_1.err_d.y += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDPITCH].kd *(ctrl_1.err.y - ctrl_1.err_old.y) *( 0.002f/T ) - ctrl_1.err_d.y);
    //	ctrl_1.err_d.z += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDYAW].kd   *(ctrl_1.err.z - ctrl_1.err_old.z) *( 0.002f/T ) - ctrl_1.err_d.z);

    /* 角速度误差积分 */
    ctrl_1.err_i.x += ctrl_1.PID[PIDROLL].ki  *(ctrl_1.err.x - ctrl_1.damp.x) *T;
    ctrl_1.err_i.y += ctrl_1.PID[PIDPITCH].ki *(ctrl_1.err.y - ctrl_1.damp.y) *T;
    ctrl_1.err_i.z += ctrl_1.PID[PIDYAW].ki 	*(ctrl_1.err.z - ctrl_1.damp.z) *T;
    /* 角速度误差积分分离 */
    ctrl_1.eliminate_I.x = Thr_Weight *CTRL_1_INT_LIMIT ;
    ctrl_1.eliminate_I.y = Thr_Weight *CTRL_1_INT_LIMIT ;
    ctrl_1.eliminate_I.z = Thr_Weight *CTRL_1_INT_LIMIT ;
    /* 角速度误差积分限幅 */
    ctrl_1.err_i.x = LIMIT( ctrl_1.err_i.x, -ctrl_1.eliminate_I.x,ctrl_1.eliminate_I.x );
    ctrl_1.err_i.y = LIMIT( ctrl_1.err_i.y, -ctrl_1.eliminate_I.y,ctrl_1.eliminate_I.y );
    ctrl_1.err_i.z = LIMIT( ctrl_1.err_i.z, -ctrl_1.eliminate_I.z,ctrl_1.eliminate_I.z );
    /* 角速度PID输出 */
    ctrl_1.out.x = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.x),0,1)*except_AS.x + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDROLL].kp  *( ctrl_1.err.x + ctrl_1.err_d.x + ctrl_1.err_i.x ) );
    //*(MAX_CTRL_ASPEED/300.0f);
    ctrl_1.out.y = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.y),0,1)*except_AS.y + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDPITCH].kp *( ctrl_1.err.y + ctrl_1.err_d.y + ctrl_1.err_i.y ) );
    //*(MAX_CTRL_ASPEED/300.0f);
    ctrl_1.out.z = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.z),0,1)*except_AS.z + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDYAW].kp   *( ctrl_1.err.z + ctrl_1.err_d.z + ctrl_1.err_i.z ) );
    //*(MAX_CTRL_ASPEED/300.0f);
    Thr_Ctrl(T);// 油门控制

    All_Out(ctrl_1.out.x,ctrl_1.out.y,ctrl_1.out.z);


    ctrl_1.err_old.x = ctrl_1.err.x;
    ctrl_1.err_old.y = ctrl_1.err.y;
    ctrl_1.err_old.z = ctrl_1.err.z;

    g_old[A_X] =  mpu6050.Gyro_deg.x ;
    g_old[A_Y] = -mpu6050.Gyro_deg.y ;
    g_old[A_Z] = -mpu6050.Gyro_deg.z ;
}


float thr_value;
u8 Thr_Low;
float Thr_Weight;
void Thr_Ctrl(float T)
{
    static float thr;
    static float Thr_tmp;
    thr = 500 + CH_filter[THR]; //油门值 0 ~ 1000

    //更改6.16
    int ultra_error;
    if ((NS == NS_FINISH_AUTO) && (thr > 150))       //油门大于150开始调节    定高模式  
    {
        ultra_error = ultra_expect_height - ultra_dis_lpf;
        if (ultra_error > HEIGHT_CTRL_DEATHZOOM)   //5cm以内才控制
            thr = HEIGHT_UP_THR;          //高度自控
        else 
            if(ultra_error < -HEIGHT_CTRL_DEATHZOOM)
                thr = HEIGHT_DOWN_THR;
            else thr = HEIGHT_KEEP;
    }
    //结束6.16


    Thr_tmp += 10 *3.14f *T *(thr/400.0f - Thr_tmp); //低通滤波
    Thr_Weight = LIMIT(Thr_tmp,0,1);    							//后边多处分离数据会用到这个值

    if( thr < 100 )
    {
        Thr_Low = 1;
    }
    else
    {
        Thr_Low = 0;
    }

#if(CTRL_HEIGHT)
    Height_Ctrl(T,thr);

    thr_value = Thr_Weight *height_ctrl_out;   //实际使用值
#else
    thr_value = thr;   //实际使用值
#endif

    thr_value = LIMIT(thr_value,0,10 *MAX_THR *MAX_PWM/100);
}


float motor[MAXMOTORS];
float posture_value[MAXMOTORS];
float curve[MAXMOTORS];
void All_Out(float out_roll,float out_pitch,float out_yaw)
{
    s16 motor_out[MAXMOTORS];
    u8 i;
    float posture_value[MAXMOTORS];
    float curve[MAXMOTORS];


    out_yaw = LIMIT( out_yaw , -5*MAX_THR ,5*MAX_THR ); //50%

    posture_value[0] = - out_roll + out_pitch + out_yaw ;
    posture_value[1] = + out_roll + out_pitch - out_yaw ;
    posture_value[2] = + out_roll - out_pitch + out_yaw ;
    posture_value[3] = - out_roll - out_pitch - out_yaw ;

    for(i=0;i<4;i++)
    {
        posture_value[i] = LIMIT(posture_value[i], -1000,1000 );
    }

    curve[0] = (0.55f + 0.45f *ABS(posture_value[0])/1000.0f) *posture_value[0] ;
    curve[1] = (0.55f + 0.45f *ABS(posture_value[1])/1000.0f) *posture_value[1] ;
    curve[2] = (0.55f + 0.45f *ABS(posture_value[2])/1000.0f) *posture_value[2] ;
    curve[3] = (0.55f + 0.45f *ABS(posture_value[3])/1000.0f) *posture_value[3] ;

    motor[0] = thr_value + Thr_Weight *curve[0] ;
    motor[1] = thr_value + Thr_Weight *curve[1] ;
    motor[2] = thr_value + Thr_Weight *curve[2] ;
    motor[3] = thr_value + Thr_Weight *curve[3] ;
    /* 是否解锁 */
    if(fly_ready)
    {
        if( !Thr_Low )  //油门拉起
        {
            for(i=0;i<4;i++)
            {
                motor[i] = LIMIT(motor[i], (10 *READY_SPEED),(10*MAX_PWM) );
            }
        }
        else						//油门低
        {
            for(i=0;i<4;i++)
            {
                motor[i] = LIMIT(motor[i], 0,(10*MAX_PWM) );
            }
        }
    }
    else
    {
        for(i=0;i<4;i++)
        {
            motor[i] = 0;
        }
    }
    /* xxx */
    motor_out[0] = (s16)(motor[0]);  
    motor_out[1] = (s16)(motor[1]);	 
    motor_out[2] = (s16)(motor[2]);
    motor_out[3] = (s16)(motor[3]);


    SetPwm(motor_out,0,1000); //1000

}

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/

