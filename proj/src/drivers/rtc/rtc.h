

#include <lcom/lcf.h>
#include "i8254.h"
#include "../utils/utils.h"

typdef struct DATE{
    uint8_t year;
    uint8_t month;
    uint8_t day_of_month;
    uint8_t day_of_the_week;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t hours;
} DATE;

int (rtc_read_reg)(uint8_t reg, uint8_t *conf);
int (rtc_get_date)(DATE *date);
int (rtc_get_year)(uint8_t *y);
int (rtc_get_month)(uint8_t *m);
int (rtc_get_day_of_month)(uint8_t *d);
int (rtc_get_day_of_week)(uint8_t *d);
int (rtc_get_minutes)(uint8_t *m);
int (rtc_get_seconds)(uint8_t *s);
int (rtc_get_hours)(uint8_t *h);
