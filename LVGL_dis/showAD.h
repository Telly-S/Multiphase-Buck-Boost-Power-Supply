#ifndef SHOWAD_H
#define SHOWAD_H

#include "lvgl.h"
#include "lv_conf_cmsis.h"
//#include "main.h"
#include "lv_port_disp_template.h"



void change_AD_data(int position,float AD_data);
void AD_label_event_init(void);
#endif
