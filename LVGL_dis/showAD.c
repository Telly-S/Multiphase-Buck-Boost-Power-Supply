#include "showAD.h"
#include "main.h"
lv_obj_t *ALL_Voltage_IN_Name_label;
lv_obj_t *ALL_Current_IN_Name_label;
lv_obj_t *P1_Voltage_IN_Name_label;
lv_obj_t *P1_Voltage_OUT_Name_label;
lv_obj_t *P1_Current_IN_Name_label;
lv_obj_t *P1_Current_OUT_Name_label;
lv_obj_t *P2_Voltage_IN_Name_label;
lv_obj_t *P2_Voltage_OUT_Name_label;
lv_obj_t *P2_Current_IN_Name_label;
lv_obj_t *P2_Current_OUT_Name_label;
lv_obj_t *P3_Voltage_IN_Name_label;
lv_obj_t *P3_Voltage_OUT_Name_label;
lv_obj_t *P3_Current_IN_Name_label;
lv_obj_t *P3_Current_OUT_Name_label;
lv_obj_t *ALL_Voltage_OUT_Name_label;
lv_obj_t *ALL_Current_OUT_Name_label;

const int ALL_Voltage_IN = 0;
const int ALL_Current_IN = 1;
const int P1_Voltage_IN = 2;
const int P1_Voltage_OUT = 3;
const int P1_Current_IN = 4;
const int P1_Current_OUT = 5;
const int P2_Voltage_IN = 6;
const int P2_Voltage_OUT = 7;
const int P2_Current_IN = 8;
const int P2_Current_OUT = 9;
const int ALL_Voltage_OUT = 10;
const int ALL_Current_OUT = 11;
lv_obj_t *myscr;
extern float ADval;

void AD_label_event_init(void)
{
	/*建立屏幕*/
	myscr = lv_scr_act();
	/*输入总电压*/
	ALL_Voltage_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(ALL_Voltage_IN_Name_label,50,180);
	lv_obj_set_pos(ALL_Voltage_IN_Name_label,50,30);
	lv_label_set_text_fmt(ALL_Voltage_IN_Name_label,"All_Vol_IN:%.2f",ADval);
	/*输入总电流*/
	ALL_Current_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(ALL_Current_IN_Name_label,50,120);
	lv_obj_set_pos(ALL_Current_IN_Name_label,150,30);
	lv_label_set_text_fmt(ALL_Current_IN_Name_label,"All_Cur_IN:%.2f",ADval);
	/*第一相输入电压*/
	P1_Voltage_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P1_Voltage_IN_Name_label,50,120);
	lv_obj_set_pos(P1_Voltage_IN_Name_label,50,75);
	lv_label_set_text_fmt(P1_Voltage_IN_Name_label,"P1_Vol_IN:%.2f",ADval);
	/*第一相输入电流*/
	P1_Current_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P1_Current_IN_Name_label,50,120);
	lv_obj_set_pos(P1_Current_IN_Name_label,150,75);
	lv_label_set_text_fmt(P1_Current_IN_Name_label,"P1_Cur_IN:%.2f",ADval);
	/*第一相输出电压*/
	P1_Voltage_OUT_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P1_Voltage_OUT_Name_label,50,120);
	lv_obj_set_pos(P1_Voltage_OUT_Name_label,50,120);
	lv_label_set_text_fmt(P1_Voltage_OUT_Name_label,"P1_Vol_OUT:%.2f",ADval);
	/*第一相输出电流*/
	P1_Current_OUT_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P1_Current_OUT_Name_label,50,120);
	lv_obj_set_pos(P1_Current_OUT_Name_label,150,120);
	lv_label_set_text_fmt(P1_Current_OUT_Name_label,"P1_Cur_OUT:%.2f",ADval);
	/*第二相输入电压*/
	P2_Voltage_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P2_Voltage_IN_Name_label,50,120);
	lv_obj_set_pos(P2_Voltage_IN_Name_label,50,165);
	lv_label_set_text_fmt(P2_Voltage_IN_Name_label,"P2_Vol_IN:%.2f",ADval);
	/*第二相输入电流*/
	P2_Current_IN_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P2_Current_IN_Name_label,50,120);
	lv_obj_set_pos(P2_Current_IN_Name_label,150,165);
	lv_label_set_text_fmt(P2_Current_IN_Name_label,"P2_Cur_IN:%.2f",ADval);
	/*第二相输出电压*/
	P2_Voltage_OUT_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P2_Voltage_OUT_Name_label,50,120);
	lv_obj_set_pos(P2_Voltage_OUT_Name_label,50,210);
	lv_label_set_text_fmt(P2_Voltage_OUT_Name_label,"P2_Vol_OUT:%.2f",ADval);
	/*第二相输出电流*/
	P2_Current_OUT_Name_label = lv_label_create(myscr);
	lv_obj_set_size(P2_Current_OUT_Name_label,50,120);
	lv_obj_set_pos(P2_Current_OUT_Name_label,150,210);
	lv_label_set_text_fmt(P2_Current_OUT_Name_label,"P2_Cur_OUT:%.2f",ADval);
	
//	ALL_Voltage_OUT_Name_label = lv_label_create(myscr);
//	lv_obj_set_size(ALL_Voltage_OUT_Name_label,50,80);
//	lv_obj_set_pos(ALL_Voltage_OUT_Name_label,50,200);
//	lv_label_set_text_fmt(ALL_Voltage_OUT_Name_label,"All_Vol_OUT:%.2f",ADval);
//	
//	ALL_Current_OUT_Name_label = lv_label_create(myscr);
//	lv_obj_set_size(ALL_Current_OUT_Name_label,50,80);
//	lv_obj_set_pos(ALL_Current_OUT_Name_label,160,200);
//	lv_label_set_text_fmt(ALL_Current_OUT_Name_label,"All_Cur_OUT:%.2f",ADval);
	
}

void change_AD_data(int position,float AD_data)
{
	switch (position)
	{
		case ALL_Voltage_IN:
			lv_label_set_text_fmt(ALL_Voltage_IN_Name_label,"All_Vol_IN:%.2f",AD_data);break;
		case ALL_Current_IN:
			lv_label_set_text_fmt(ALL_Current_IN_Name_label,"All_Cur_IN:%.2f",AD_data);break;
		case P1_Voltage_IN:
			lv_label_set_text_fmt(P1_Voltage_IN_Name_label,"P1_Vol_IN:%.2f",AD_data);break;
		case P1_Voltage_OUT:
			lv_label_set_text_fmt(P1_Voltage_OUT_Name_label,"P1_Vol_OUT:%.2f",AD_data);break;
		case P1_Current_IN:
			lv_label_set_text_fmt(P1_Current_IN_Name_label,"P1_Cur_IN:%.2f",AD_data);break;
		case P1_Current_OUT:
			lv_label_set_text_fmt(P1_Current_OUT_Name_label,"P1_Cur_OUT:%.2f",AD_data);break;
		case P2_Voltage_IN:
			lv_label_set_text_fmt(P2_Voltage_IN_Name_label,"P2_Vol_IN:%.2f",AD_data);break;
		case P2_Voltage_OUT:
			lv_label_set_text_fmt(P2_Voltage_OUT_Name_label,"P2_Vol_OUT:%.2f",AD_data);break;
		case P2_Current_IN:
			lv_label_set_text_fmt(P2_Current_IN_Name_label,"P2_Cur_IN:%.2f",AD_data);break;
		case P2_Current_OUT:
			lv_label_set_text_fmt(P2_Current_OUT_Name_label,"P2_Cur_OUT:%.2f",AD_data);break;
		case ALL_Voltage_OUT:
			lv_label_set_text_fmt(ALL_Voltage_OUT_Name_label,"All_Vol_OUT:%.2f",AD_data);break;
		case ALL_Current_OUT: 
			lv_label_set_text_fmt(ALL_Current_OUT_Name_label,"All_Cur_OUT:%.2f",AD_data);break;
		
	}
		
}




