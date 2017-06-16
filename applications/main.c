/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : main.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\applications
// 作者   : Cmoande
// 日期   : 2017/05/10
// 描述   : 主循环
// 备注   :
// 版本   : V0.0  2017.5.10  Cmoadne  初始代码
******************************************************************/ 
#include "include.h"

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {

    }
}
#endif
//=======================================================================================


//=======================================================================================
u8 Init_Finish = 0;
int main(void)
{
    Init_Finish = All_Init();		
    while(1)
    {
        Duty_Loop(); 
    }
}
/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/
