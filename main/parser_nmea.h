#ifndef MAIN_PARSERNMEA_H
#define MAIN_PARSERNMEA_H

#include "esp_types.h"
#include <string.h>
#include <stdio.h>
#include "esp_log.h"

#define BUFF_SIZE 1024
#define NMEA_MAX_STATEMENT_ITEM_LENGTH 40

typedef enum{
    UNKNOWN_STATEMENT = 0,
    GPGGA,
    GPGSA,
    GPGSV,
    GPRMC,
} nmea_statement_t;

typedef enum{
    LATITUDE = 2,
    LONGITUDE = 4,
} lat_long_t;

typedef struct{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

} gps_time_t;


typedef enum{
    INVALID = 0,
    GPS_FIX,
    DGPS_FIX,
} gps_fix_mode_t;

typedef struct {
    uint8_t num;
    uint8_t elevation;
    uint16_t azimuth;
    uint8_t snr;
} gps_satellite_t;

typedef struct{
    float longitude;
    float latitude;
    float altitude;
    float hdop;
    gps_time_t time;
    uint8_t satellites_in_use;
    float speed;
    gps_fix_mode_t fix_mode;

} gps_t;

typedef struct{
    char* sentence[NMEA_MAX_STATEMENT_ITEM_LENGTH];
    uint8_t sentence_length;
    uint8_t statement;
    uint8_t *buffer;
    uint8_t current_statement;
    gps_t gps;
} parser_t;

void separate_items (parser_t *parser, char * buffer);
void gps_decode(parser_t *parser);
void show_data(parser_t *parser);

#endif