#ifndef __WEATHER_H__
#define __WEATHER_H__


typedef struct {
    int high_temp;
    int low_temp;
    char weather_code[4];

}weather_data_pkt_t;

void weather_start(void);

#endif