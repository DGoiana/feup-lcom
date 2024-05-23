#include "rtc.h"
#include "i8254.h"

int (rtc_read_reg)(uint8_t reg, uint8_t *conf) {
	if (sys_outb(RTC_ADDR_REG, reg) != 0) {
		printf("error writing to rtc register, trying to read %d", reg);
		return 1;
	}

	if (util_sys_inb(RTC_READ_DATA_REG, conf) != 0) {
		printf("error reading rtc data reg");
		return 1;
	}

	return 0;
}

