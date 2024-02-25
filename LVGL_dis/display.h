//
// Created by ozcom on 2022/5/9.
//

#ifndef F4XX_SPI_DISPLAY_H
#define F4XX_SPI_DISPLAY_H

#include "display_conf.h"
#include "spi.h"
#include "gpio.h"
#include "lv_conf_cmsis.h"
#include "lvgl.h"

//DISPLAY屏幕分辨率定义
#define DISPLAY_W   DISPLAY_WIDTH
#define DISPLAY_H  DISPLAY_HEIGHT
//颜色定义
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色
#define GRAYBLUE         0X5458 //灰蓝色
#define LIGHTGREEN       0X841F //浅绿色
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


//selected DISPLAY
#define DISPLAY_CS_0  HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET)
//unselect DISPLAY
#define DISPLAY_CS_1  HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET)

#define DISPLAY_CS(n) (n?DISPLAY_CS_1:DISPLAY_CS_0)

//command
#define DISPLAY_DC_0  HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET)
//data
#define DISPLAY_DC_1  HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET)

#define DISPLAY_DC(n) (n?DISPLAY_DC_1:DISPLAY_DC_0)

//reset DISPLAY
#define DISPLAY_RESET_0  HAL_GPIO_WritePin(DISPLAY_Reset_Port, DISPLAY_Reset_Pin, GPIO_PIN_RESET)
//data
#define DISPLAY_RESET_1  HAL_GPIO_WritePin(DISPLAY_Reset_Port, DISPLAY_Reset_Pin, GPIO_PIN_SET)

#define DISPLAY_RESET(n) (n?DISPLAY_RESET_1:DISPLAY_RESET_0)

#define DISPLAY_WR_BYTE(data) HAL_SPI_Transmit(&DISPLAY_SPI_PORT, &data, 1, HAL_MAX_DELAY)
#define DISPLAY_WR_ARRAY(adr, n) HAL_SPI_Transmit(&DISPLAY_SPI_PORT, adr, n, HAL_MAX_DELAY)
#define LV_DRV_DISP_SPI_WR_ARRAY(adr, n) do{ DISPLAY_CS(0); DISPLAY_WR_ARRAY(adr,n); DISPLAY_CS(1); }while(0) /*spi_wr_mem(adr, n)*/  /*Write 'n' bytes to SPI bus from 'adr'*/

//lcd Command

//#ifdef IC_ST7789V2
//
//#define ST7789V2_SLEEP_OUT 0x11
//#define ST7789V2_MEM_DATA_ACCESS 0x36
//#define ST7789V2_COLOR_SPACE 0x3A
//#define ST7789V2_PORCH 0xB2
//#define ST7789V2_GATE 0xB7
//#define ST7789V2_VCOM_SETTING 0xBB
//#define ST7789V2_LCM 0xC0
//#define ST7789V2_VDV_AND_VRH 0xC2
//#define ST7789V2_VRH_SET 0xC3
//#define ST7789V2_FRAME_RATE 0xC6
//#define ST7789V2_POWER 0xD0
//#define ST7789V2_PV_GAMMA 0xE0
//#define ST7789V2_NV_GAMMA 0xE1
//#define ST7789V2_DISPLAY 0x21
//#define ST7789V2_INVERSION 0x29
//
//#endif
//
//#ifdef IC_ST7735S
//
//#endif
//
//#ifdef IC_GC9A01
//
//#endif

#ifdef IC_ST7789V2

void DISPLAY_st7789v2_Init(void);

#endif

#ifdef IC_GC9A01

void DISPLAY_gc9a01_Init(void);

#endif

void DISPLAY_Init(void);

void DISPLAY_Reset(void);

// Send a byte to the command register
void DISPLAY_WriteCommand(uint8_t cmd);

void DISPLAY_WriteByte(uint8_t dat);

// Send data
void DISPLAY_WriteData(uint8_t *buf, size_t len);

void DISPLAY_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void DISPLAY_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);

void DISPLAY_Fill_user(lv_disp_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
#endif //F4XX_SPI_DISPLAY_H
