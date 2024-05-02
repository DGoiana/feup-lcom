#include "rtc.h"
#include "i8254.h"

int (rtc_read_reg)(uint8_t reg, uint8_t *conf) {
	if (sys_outb(RTC_WRITE_DATA_REG, reg) != 0) {
		printf("error writing to rtc register");
		return 0;
	}

	if (util)

}

