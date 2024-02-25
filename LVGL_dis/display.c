//
// Created by ozcom on 2022/5/9.
//

#include "display.h"

#define USE_HORIZONTAL 0

void DISPLAY_Reset(void) {
    // CS = High (not selected)
    DISPLAY_CS_1;

    // Reset the DISPLAY
    DISPLAY_RESET_0;
    HAL_Delay(10);
    DISPLAY_RESET_1;
    HAL_Delay(10);
}

// Send a byte to the command register
void DISPLAY_WriteCommand(uint8_t cmd) {
    DISPLAY_CS_0; // select DISPLAY
    DISPLAY_DC_0; // command
    HAL_SPI_Transmit(&DISPLAY_SPI_PORT, &cmd, 1, HAL_MAX_DELAY);
//    HAL_SPI_Transmit_DMA(&DISPLAY_SPI_PORT, &cmd, 1);
    DISPLAY_CS_1; // un-select DISPLAY
}

void DISPLAY_WriteByte(uint8_t dat) {
    DISPLAY_CS_0; // select DISPLAY
    DISPLAY_DC_1; // data
    HAL_SPI_Transmit(&DISPLAY_SPI_PORT, &dat, 1, HAL_MAX_DELAY);
//    HAL_SPI_Transmit_DMA(&DISPLAY_SPI_PORT, &dat, 1);
    DISPLAY_CS_1; // un-select DISPLAY
}

// Send data
void DISPLAY_WriteData(uint8_t *buf, size_t len) {
    DISPLAY_CS_0; // select DISPLAY
    DISPLAY_DC_1; // data
    HAL_SPI_Transmit(&DISPLAY_SPI_PORT, buf, len, HAL_MAX_DELAY);
//    HAL_SPI_Transmit_DMA(&DISPLAY_SPI_PORT, buf, len);
    DISPLAY_CS_1; // un-select DISPLAY
}


//set display address
void DISPLAY_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint8_t buf[4];
    DISPLAY_WriteCommand(0x2a);
    buf[0] = x1 >> 8;
    buf[1] = x1;
    buf[2] = x2 >> 8;
    buf[3] = x2;
    DISPLAY_WriteData(buf, 4);
    DISPLAY_WriteCommand(0x2b);
    buf[0] = y1 >> 8;
    buf[1] = y1;
    buf[2] = y2 >> 8;
    buf[3] = y2;
    DISPLAY_WriteData(buf, 4);
    DISPLAY_WriteCommand(0x2c);
}

void DISPLAY_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color) {
    uint16_t i, j;
    DISPLAY_Address_Set(xsta, ysta, xend - 1, yend - 1);
    uint8_t dat[2];
    for (i = ysta; i < yend; i++) {
        for (j = xsta; j < xend; j++) {
            dat[0] = color >> 8;
            dat[1] = color;
            DISPLAY_WriteData(dat, 2);
        }
    }
}

void DISPLAY_Fill_user(lv_disp_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
//    uint16_t i, j;
//    DISPLAY_Address_Set(xsta, ysta, xend - 1, yend - 1);
//    uint8_t dat[2];
//    for (i = ysta; i < yend; i++) {
//        for (j = xsta; j < xend; j++) {
//            dat[0] = *color >> 8;
//            dat[1] = *color;
//            DISPLAY_WriteData(dat, 2);
//        }
//    }
	DISPLAY_CS(0);
	DISPLAY_Address_Set(area->x1, area->y1, area->x2, area->y2);
	int32_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
	DISPLAY_DC(1);
	LV_DRV_DISP_SPI_WR_ARRAY((uint8_t *) color_p, len);
	DISPLAY_CS(1);
	lv_disp_flush_ready(disp_drv); 
}
#ifdef IC_ST7789V2

void DISPLAY_st7789v2_Init(void) {

    DISPLAY_Reset();

    /* Sleep Out */
    DISPLAY_WriteCommand(0x11);
    /* wait for power stability */
    HAL_Delay(120);

    /* Memory Data Access Control */
    DISPLAY_WriteCommand(0x36);
    DISPLAY_WriteByte(0x00);

    /* RGB 5-6-5-bit  */
    DISPLAY_WriteCommand(0x3A);
    DISPLAY_WriteByte(0x65);

    /* Porch Setting */
    DISPLAY_WriteCommand(0xB2);
    DISPLAY_WriteByte(0x0C);
    DISPLAY_WriteByte(0x0C);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x33);
    DISPLAY_WriteByte(0x33);

    /*  Gate Control */
    DISPLAY_WriteCommand(0xB7);
    DISPLAY_WriteByte(0x72);

    /* VCOM Setting */
    DISPLAY_WriteCommand(0xBB);
    DISPLAY_WriteByte(0x3D);   //Vcom=1.625V

    /* LCM Control */
    DISPLAY_WriteCommand(0xC0);
    DISPLAY_WriteByte(0x2C);

    /* VDV and VRH Command Enable */
    DISPLAY_WriteCommand(0xC2);
    DISPLAY_WriteByte(0x01);

    /* VRH Set */
    DISPLAY_WriteCommand(0xC3);
    DISPLAY_WriteByte(0x19);

    /* VDV Set */
    DISPLAY_WriteCommand(0xC4);
    DISPLAY_WriteByte(0x20);

    /* Frame Rate Control in Normal Mode */
    DISPLAY_WriteCommand(0xC6);
    DISPLAY_WriteByte(0x0F);    //60MHZ

    /* Power Control 1 */
    DISPLAY_WriteCommand(0xD0);
    DISPLAY_WriteByte(0xA4);
    DISPLAY_WriteByte(0xA1);

    /* Positive Voltage Gamma Control */
    DISPLAY_WriteCommand(0xE0);
    DISPLAY_WriteByte(0xD0);
    DISPLAY_WriteByte(0x04);
    DISPLAY_WriteByte(0x0D);
    DISPLAY_WriteByte(0x11);
    DISPLAY_WriteByte(0x13);
    DISPLAY_WriteByte(0x2B);
    DISPLAY_WriteByte(0x3F);
    DISPLAY_WriteByte(0x54);
    DISPLAY_WriteByte(0x4C);
    DISPLAY_WriteByte(0x18);
    DISPLAY_WriteByte(0x0D);
    DISPLAY_WriteByte(0x0B);
    DISPLAY_WriteByte(0x1F);
    DISPLAY_WriteByte(0x23);

    /* Negative Voltage Gamma Control */
    DISPLAY_WriteCommand(0xE1);
    DISPLAY_WriteByte(0xD0);
    DISPLAY_WriteByte(0x04);
    DISPLAY_WriteByte(0x0C);
    DISPLAY_WriteByte(0x11);
    DISPLAY_WriteByte(0x13);
    DISPLAY_WriteByte(0x2C);
    DISPLAY_WriteByte(0x3F);
    DISPLAY_WriteByte(0x44);
    DISPLAY_WriteByte(0x51);
    DISPLAY_WriteByte(0x2F);
    DISPLAY_WriteByte(0x1F);
    DISPLAY_WriteByte(0x1F);
    DISPLAY_WriteByte(0x20);
    DISPLAY_WriteByte(0x23);

    /* Display Inversion On */
    DISPLAY_WriteCommand(0x21);
    DISPLAY_WriteCommand(0x29);

}

#endif


#ifdef IC_GC9A01

void DISPLAY_gc9a01_Init(void) {

    DISPLAY_Reset();

    DISPLAY_WriteCommand(0xEF);
    DISPLAY_WriteCommand(0xEB);
    DISPLAY_WriteByte(0x14);

    DISPLAY_WriteCommand(0xFE);
    DISPLAY_WriteCommand(0xEF);

    DISPLAY_WriteCommand(0xEB);
    DISPLAY_WriteByte(0x14);

    DISPLAY_WriteCommand(0x84);
    DISPLAY_WriteByte(0x40);

    DISPLAY_WriteCommand(0x85);
    DISPLAY_WriteByte(0xFF);

    DISPLAY_WriteCommand(0x86);
    DISPLAY_WriteByte(0xFF);

    DISPLAY_WriteCommand(0x87);
    DISPLAY_WriteByte(0xFF);

    DISPLAY_WriteCommand(0x88);
    DISPLAY_WriteByte(0x0A);

    DISPLAY_WriteCommand(0x89);
    DISPLAY_WriteByte(0x21);

    DISPLAY_WriteCommand(0x8A);
    DISPLAY_WriteByte(0x00);

    DISPLAY_WriteCommand(0x8B);
    DISPLAY_WriteByte(0x80);

    DISPLAY_WriteCommand(0x8C);
    DISPLAY_WriteByte(0x01);

    DISPLAY_WriteCommand(0x8D);
    DISPLAY_WriteByte(0x01);

    DISPLAY_WriteCommand(0x8E);
    DISPLAY_WriteByte(0xFF);

    DISPLAY_WriteCommand(0x8F);
    DISPLAY_WriteByte(0xFF);


    DISPLAY_WriteCommand(0xB6);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x20);

    DISPLAY_WriteCommand(0x36);
    if (USE_HORIZONTAL == 0)DISPLAY_WriteByte(0x08);
    else if (USE_HORIZONTAL == 1)DISPLAY_WriteByte(0xC8);
    else if (USE_HORIZONTAL == 2)DISPLAY_WriteByte(0x68);
    else DISPLAY_WriteByte(0xA8);

    DISPLAY_WriteCommand(0x3A);
    DISPLAY_WriteByte(0x05);


    DISPLAY_WriteCommand(0x90);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x08);

    DISPLAY_WriteCommand(0xBD);
    DISPLAY_WriteByte(0x06);

    DISPLAY_WriteCommand(0xBC);
    DISPLAY_WriteByte(0x00);

    DISPLAY_WriteCommand(0xFF);
    DISPLAY_WriteByte(0x60);
    DISPLAY_WriteByte(0x01);
    DISPLAY_WriteByte(0x04);

    DISPLAY_WriteCommand(0xC3);
    DISPLAY_WriteByte(0x13);
    DISPLAY_WriteCommand(0xC4);
    DISPLAY_WriteByte(0x13);

    DISPLAY_WriteCommand(0xC9);
    DISPLAY_WriteByte(0x22);

    DISPLAY_WriteCommand(0xBE);
    DISPLAY_WriteByte(0x11);

    DISPLAY_WriteCommand(0xE1);
    DISPLAY_WriteByte(0x10);
    DISPLAY_WriteByte(0x0E);

    DISPLAY_WriteCommand(0xDF);
    DISPLAY_WriteByte(0x21);
    DISPLAY_WriteByte(0x0c);
    DISPLAY_WriteByte(0x02);

    DISPLAY_WriteCommand(0xF0);
    DISPLAY_WriteByte(0x45);
    DISPLAY_WriteByte(0x09);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x26);
    DISPLAY_WriteByte(0x2A);

    DISPLAY_WriteCommand(0xF1);
    DISPLAY_WriteByte(0x43);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x72);
    DISPLAY_WriteByte(0x36);
    DISPLAY_WriteByte(0x37);
    DISPLAY_WriteByte(0x6F);


    DISPLAY_WriteCommand(0xF2);
    DISPLAY_WriteByte(0x45);
    DISPLAY_WriteByte(0x09);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x26);
    DISPLAY_WriteByte(0x2A);

    DISPLAY_WriteCommand(0xF3);
    DISPLAY_WriteByte(0x43);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x72);
    DISPLAY_WriteByte(0x36);
    DISPLAY_WriteByte(0x37);
    DISPLAY_WriteByte(0x6F);

    DISPLAY_WriteCommand(0xED);
    DISPLAY_WriteByte(0x1B);
    DISPLAY_WriteByte(0x0B);

    DISPLAY_WriteCommand(0xAE);
    DISPLAY_WriteByte(0x77);

    DISPLAY_WriteCommand(0xCD);
    DISPLAY_WriteByte(0x63);


    DISPLAY_WriteCommand(0x70);
    DISPLAY_WriteByte(0x07);
    DISPLAY_WriteByte(0x07);
    DISPLAY_WriteByte(0x04);
    DISPLAY_WriteByte(0x0E);
    DISPLAY_WriteByte(0x0F);
    DISPLAY_WriteByte(0x09);
    DISPLAY_WriteByte(0x07);
    DISPLAY_WriteByte(0x08);
    DISPLAY_WriteByte(0x03);

    DISPLAY_WriteCommand(0xE8);
    DISPLAY_WriteByte(0x34);

    DISPLAY_WriteCommand(0x62);
    DISPLAY_WriteByte(0x18);
    DISPLAY_WriteByte(0x0D);
    DISPLAY_WriteByte(0x71);
    DISPLAY_WriteByte(0xED);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x18);
    DISPLAY_WriteByte(0x0F);
    DISPLAY_WriteByte(0x71);
    DISPLAY_WriteByte(0xEF);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x70);

    DISPLAY_WriteCommand(0x63);
    DISPLAY_WriteByte(0x18);
    DISPLAY_WriteByte(0x11);
    DISPLAY_WriteByte(0x71);
    DISPLAY_WriteByte(0xF1);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x18);
    DISPLAY_WriteByte(0x13);
    DISPLAY_WriteByte(0x71);
    DISPLAY_WriteByte(0xF3);
    DISPLAY_WriteByte(0x70);
    DISPLAY_WriteByte(0x70);

    DISPLAY_WriteCommand(0x64);
    DISPLAY_WriteByte(0x28);
    DISPLAY_WriteByte(0x29);
    DISPLAY_WriteByte(0xF1);
    DISPLAY_WriteByte(0x01);
    DISPLAY_WriteByte(0xF1);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x07);

    DISPLAY_WriteCommand(0x66);
    DISPLAY_WriteByte(0x3C);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0xCD);
    DISPLAY_WriteByte(0x67);
    DISPLAY_WriteByte(0x45);
    DISPLAY_WriteByte(0x45);
    DISPLAY_WriteByte(0x10);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x00);

    DISPLAY_WriteCommand(0x67);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x3C);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x01);
    DISPLAY_WriteByte(0x54);
    DISPLAY_WriteByte(0x10);
    DISPLAY_WriteByte(0x32);
    DISPLAY_WriteByte(0x98);

    DISPLAY_WriteCommand(0x74);
    DISPLAY_WriteByte(0x10);
    DISPLAY_WriteByte(0x85);
    DISPLAY_WriteByte(0x80);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x00);
    DISPLAY_WriteByte(0x4E);
    DISPLAY_WriteByte(0x00);

    DISPLAY_WriteCommand(0x98);
    DISPLAY_WriteByte(0x3e);
    DISPLAY_WriteByte(0x07);

    DISPLAY_WriteCommand(0x35);
    DISPLAY_WriteCommand(0x21);

    DISPLAY_WriteCommand(0x11);
//    HAL_Delay(120);
    DISPLAY_WriteCommand(0x29);
//    HAL_Delay(20);

    DISPLAY_Fill(0, 0, DISPLAY_W, DISPLAY_H, WHITE);

}

#endif


void DISPLAY_Init(void) {

#ifdef IC_ST7789V2
    DISPLAY_st7789v2_Init();
#endif

#ifdef IC_GC9A01
    DISPLAY_gc9a01_Init();
#endif

}