#include "rtc.h"
#include "i8254.h"

uint8_t (bcd_to_bin)(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

int (rtc_get_year)(uint8_t *y) {
	if (rtc_read_reg(RTC_YEAR, y) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_month)(uint8_t *m) {
	if (rtc_read_reg(RTC_MONTH, m) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_day_of_month)(uint8_t *d) {
	if (rtc_read_reg(RTC_MONTH_DATE, d) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_day_of_week)(uint8_t *d) {
	if (rtc_read_reg(RTC_WEEK_DAY, d) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_minutes)(uint8_t *m) {
	if (rtc_read_reg(RTC_MINUTES, m) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_seconds)(uint8_t *s) {
	if (rtc_read_reg(RTC_SECONDS, s) != 0) {
		return 1;
	}
	return 0;
}

int (rtc_get_hours)(uint8_t *h) {
	if (rtc_read_reg(RTC_HOURS_24, h) != 0) {
		return 1;
	}
	return 0;
}

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

int (rtc_get_date_bcd)(DATE *date) {
	if (rtc_get_hours(&date->hours) != 0) {
		printf("Error fetching hours...");
		return 1;
	}

	if (rtc_get_seconds(&date->seconds) != 0) {
		printf("Error fetching seconds...");
		return 1;
	}

	if (rtc_get_minutes(&date->minutes) != 0) {
		printf("Error fetching minutes...");
		return 1;
	}

	if (rtc_get_day_of_week(&date->day_of_the_week) != 0) {
		printf("Error fetching day of the week...");
		return 1;
	}

	if (rtc_get_day_of_month(&date->day_of_month) != 0) {
		printf("Error fetching day of the month...");
		return 1;
	}

	if (rtc_get_month(&date->month) != 0) {
		printf("Error fetching month...");
		return 1;
	}

	if (rtc_get_year(&date->year) != 0) {
		printf("Error fetching year...");
		return 1;
	}

	return 0;
}

int (rtc_get_date_decimal)(DATE *date) {
    if (rtc_get_hours(&date->hours) != 0) {
        printf("Error fetching hours...");
        return 1;
    }
    date->hours = bcd_to_bin(date->hours);

    if (rtc_get_seconds(&date->seconds) != 0) {
        printf("Error fetching seconds...");
        return 1;
    }
    date->seconds = bcd_to_bin(date->seconds);

    if (rtc_get_minutes(&date->minutes) != 0) {
        printf("Error fetching minutes...");
        return 1;
    }
    date->minutes = bcd_to_bin(date->minutes);

    if (rtc_get_day_of_week(&date->day_of_the_week) != 0) {
        printf("Error fetching day of the week...");
        return 1;
    }
    date->day_of_the_week = bcd_to_bin(date->day_of_the_week);

    if (rtc_get_day_of_month(&date->day_of_month) != 0) {
        printf("Error fetching day of the month...");
        return 1;
    }
    date->day_of_month = bcd_to_bin(date->day_of_month);

    if (rtc_get_month(&date->month) != 0) {
        printf("Error fetching month...");
        return 1;
    }
    date->month = bcd_to_bin(date->month);

    if (rtc_get_year(&date->year) != 0) {
        printf("Error fetching year...");
        return 1;
    }
    date->year = bcd_to_bin(date->year);

    return 0;
}

char *rtc_get_date_str() {
	static char date_str[19]; // static to return the address of the array

	DATE date;
	if (rtc_get_date_decimal(&date) != 0) {
		printf("Error fetching current date...\n");
		return "";
	}

	date_str[0] = '[';

	// Format hours
	if (date.hours < 10) {
		date_str[1] = '0';
		date_str[2] = (char)(date.hours + '0');
	} else {
		date_str[1] = (char)((date.hours / 10) + '0');
		date_str[2] = (char)((date.hours % 10) + '0');
	}

	date_str[3] = '-';

	// Format minutes
	if (date.minutes < 10) {
		date_str[4] = '0';
		date_str[5] = (char)(date.minutes + '0');
	} else {
		date_str[4] = (char)((date.minutes / 10) + '0');
		date_str[5] = (char)((date.minutes % 10) + '0');
	}

	date_str[6] = ' ';

	// Format day of month
	if (date.day_of_month < 10) {
		date_str[7] = '0';
		date_str[8] = (char)(date.day_of_month + '0');
	} else {
		date_str[7] = (char)((date.day_of_month / 10) + '0');
		date_str[8] = (char)((date.day_of_month % 10) + '0');
	}

	date_str[9] = '-';

	// Format month
	if (date.month < 10) {
		date_str[10] = '0';
		date_str[11] = (char)(date.month + '0');
	} else {
		date_str[10] = (char)((date.month / 10) + '0');
		date_str[11] = (char)((date.month % 10) + '0');
	}

	date_str[12] = '-';

	// Format year
	int year = date.year;
	date_str[16] = (char)((year % 10) + '0');
	year /= 10;
	date_str[15] = (char)((year % 10) + '0');
	year /= 10;
	date_str[14] = '0';
	year /= 10;
	date_str[13] = '2';

	date_str[17] = ']';

	date_str[18] = '\0';

	return date_str;
}
