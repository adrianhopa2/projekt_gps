#include "display_data.h"

void init_screen(SSD1306_t * dev)
{
    ssd1306_clear_screen(dev, false);
	i2c_contrast(dev, 0xff);
	ssd1306_display_text(dev, 0, " Starting", 9, false);
}

void display_data(SSD1306_t * dev, parser_t *parser)
{
    char longitude[16] = "";
    char latitude[16] = "";
    char altitude[16] = "";
    char hdop[16] = "";
    char time[16] = "";
    char sats[16] = "";
    char speed[16] = "";
    char fix[16] = "";

    sprintf(longitude, " Long: %.3f", parser->gps.longitude);
    sprintf(latitude, " Lat: %.3f", parser->gps.latitude);
    sprintf(altitude, " Alt: %.3f", parser->gps.altitude);
    sprintf(hdop, " HDOP: %.3f", parser->gps.hdop);
    sprintf(time, " %02d:%02d:%02d", parser->gps.time.hour,parser->gps.time.minute, parser->gps.time.second);
    sprintf(sats, " Sats: %d", parser->gps.satellites_in_use);
    sprintf(speed, " Speed: %.2f", parser->gps.speed);
    if(parser->gps.fix_mode == 1){
         sprintf(fix, " Fix mode: GPS");
    } else if (parser->gps.fix_mode == 2){
         sprintf(fix, " Fix mode: DGPS");
    } else {
         sprintf(fix, " Fix mode: Inv");
    }

    //size max 16 characters
    //max 8 lines

    //ssd1306_clear_screen(dev, false);
	//i2c_contrast(dev, 0xff);

    ssd1306_display_text(dev, 0, longitude, 16, false);
	ssd1306_display_text(dev, 1, latitude, 16, false);
    ssd1306_display_text(dev, 2, altitude, 16, false);
	ssd1306_display_text(dev, 3, hdop, 16, false);
    ssd1306_display_text(dev, 4, time, 16, false);
    ssd1306_display_text(dev, 5, sats, 16, false);
    ssd1306_display_text(dev, 6, speed, 16, false);
    ssd1306_display_text(dev, 7, fix, 16, false);
}