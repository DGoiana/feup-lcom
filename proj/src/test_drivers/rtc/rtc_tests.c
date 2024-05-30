#include "rtc_tests.h"

int (rtc_test_conf)(void) {
    uint8_t ra,rb,rc,rd;

    if ((rtc_read_reg(RTC_REGISTER_A, &ra) ||
    rtc_read_reg(RTC_REGISTER_B, &rb) ||
    rtc_read_reg(RTC_REGISTER_C, &rc) ||
    rtc_read_reg(RTC_REGISTER_D, &rd)) != 0) {
        return 1; 
    }

    printf("Register value: %x\n", ra);
    printf("Register value: %x\n", rb);
    printf("Register value: %x\n", rc);
    printf("Register value: %x\n", rd);

    return 0;
} 

int (rtc_test_date)(void) {
    DATE d;

    if (rtc_get_date_decimal(&d) != 0) {
        printf("Error when getting date...\n");
        return 1;
    }

    printf("%d:%d:%d\n", d.hours, d.minutes, d.seconds);
    printf("Day %d of the %d month, year 20%d\n", d.day_of_month, d.month, d.year);

    return 0;
    
}
