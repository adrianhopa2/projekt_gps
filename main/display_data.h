#ifndef MAIN_DISPLAYDATA_H
#define MAIN_DISPLAYDATA_H

#include "ssd1306.h"

#include "parser_nmea.h"

#include "font8x8_basic.h"

static const char *SSDTAG = "SSD1306";

void init_screen(SSD1306_t * dev);

void display_data(SSD1306_t * dev, parser_t *parser);

#endif