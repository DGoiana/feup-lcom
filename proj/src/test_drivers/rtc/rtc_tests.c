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
    
}
