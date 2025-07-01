#include "parser_nmea.h"

static const char *TAG = "PARSER_NMEA_TEST";

static void set_statement(parser_t *parser){
    if(strstr(parser->sentence[0], "GGA")){
        parser->current_statement = GPGGA;
    } else if(strstr(parser->sentence[0], "GSA")){
        parser->current_statement = GPGSA;        
    } else if(strstr(parser->sentence[0], "GSV")){
        parser->current_statement = GPGSV;  
    } else if(strstr(parser->sentence[0], "RMC")){
        parser->current_statement = GPRMC;  
    } else {
        parser->current_statement = UNKNOWN_STATEMENT;
    }
}

void separate_items (parser_t *parser, char * buffer){
    char separator[] = ",";
    char * temp;
    temp = strtok(buffer, separator);
    uint8_t index = 0;
    while(temp != NULL)
    {                    
        parser->sentence[index] = temp;
        temp = strtok( NULL, separator );
        index ++;
    }
    parser->sentence_length = index;
    set_statement(parser);
}

static inline uint8_t convert_two_digit2number(const char *digit_char)
{
    return 10 * (digit_char[0] - '0') + (digit_char[1] - '0');
}

static void parse_utc_time(parser_t *parser)
{
    parser->gps.time.hour = convert_two_digit2number(parser->sentence[1] + 0);
    parser->gps.time.minute = convert_two_digit2number(parser->sentence[1] + 2);
    parser->gps.time.second = convert_two_digit2number(parser->sentence[1] + 4);
}

static float parse_latitude_longitude(parser_t *parser, int lat_long){
    float val = strtof(parser->sentence[lat_long], NULL);
    int deg = val / 100;
    float min = val - (deg * 100);
    val = deg + min / 60.0f;
    return val;
}

static void parse_gga(parser_t *parser, uint8_t item){
    switch (item){
        case 1:
            parse_utc_time(parser);
            break;
        case 2:
            parser->gps.latitude = parse_latitude_longitude(parser, LATITUDE);
            break;
        case 3:
            if(parser->sentence[3] == 'S'){
                parser->gps.latitude *= -1;
            }
            break;
        case 4:
            parser->gps.longitude = parse_latitude_longitude(parser, LONGITUDE);
            break;
        case 5:
            if(parser->sentence[5] == 'W'){
                parser->gps.latitude *= -1;
            }
            break;
        case 6:
            parser->gps.fix_mode = (uint8_t)strtol(parser->sentence[6], NULL, 10);
            break;
        case 7:
            parser->gps.satellites_in_use = (uint8_t)strtol(parser->sentence[7], NULL, 10);
            break;
        case 8:
            parser->gps.hdop = (uint8_t)strtof(parser->sentence[8], NULL);
            break;
        case 9:
            parser->gps.altitude = (uint8_t)strtof(parser->sentence[9], NULL);
            break;
        default:
            break;
    }
}

static void parse_rmc(parser_t *parser, uint8_t item){
    switch(item){
        case 7:
        parser->gps.speed = (strtof(parser->sentence[7], NULL)) * 1.852;
    }
}


void gps_decode(parser_t *parser){
    for(int i = 1; i < parser->sentence_length; i++){
        switch(parser->current_statement){
            case UNKNOWN_STATEMENT:
                break;
            case GPGGA:
                parse_gga(parser, i);
                break;
            case GPGSA:
                //parse_gsa(parser);
                break;
            case GPGSV:
                //parse_gsv(parser);
                break;
            case GPRMC:
                parse_rmc(parser, i);
                break;
            default:
                break;
        }

    }
    
}

void show_data(parser_t *parser){
    ESP_LOGI(TAG, "Longitude: %f", parser->gps.longitude);
    ESP_LOGI(TAG, "Latitude: %f", parser->gps.latitude);
    ESP_LOGI(TAG, "Altitude: %f", parser->gps.altitude);
    ESP_LOGI(TAG, "HDOP: %f", parser->gps.hdop);
    ESP_LOGI(TAG, "Time: %02d:%02d:%02d", parser->gps.time.hour,parser->gps.time.minute, parser->gps.time.second);
    ESP_LOGI(TAG, "Sats in use: %d", parser->gps.satellites_in_use);
    ESP_LOGI(TAG, "Speed: %f", parser->gps.speed);
    if(parser->gps.fix_mode == 1){
        ESP_LOGI(TAG, "GPS_FIX");
    } else if (parser->gps.fix_mode == 2){
        ESP_LOGI(TAG, "DGPS_FIX");
    } else {
        ESP_LOGI(TAG, "INVALID_FIX");
    }
}









