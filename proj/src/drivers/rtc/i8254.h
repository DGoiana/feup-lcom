#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H

#define RTC_ADDR_REG 0x70
#define RTC_WRITE_DATA_REG 0x71
#define RTC_READ_DATA_REG 0x71

#define RTC_SECONDS 0
#define RTC_SECONDS_ALARM 1
#define RTC_MINUTES 2
#define RTC_MINUTES_ALARM 3
#define RTC_HOURS_12 4
#define RTC_HOURS_24 4
#define RTC_HOURS_ALARM_12 5
#define RTC_HOURS_ALARM_24 5
#define RTC_WEEK_DAY 6
#define RTC_MONTH_DATE 7
#define RTC_MONTH 8
#define RTC_YEAR 9
#define RTC_REGISTER_A 10
#define RTC_REGISTER_B 11
#define RTC_REGISTER_C 12
#define RTC_REGISTER_D 13

#define RTC_A_UIP BIT(7)
#define RTC_A_DV2 BIT(6)
#define RTC_A_DV1 BIT(5)
#define RTC_A_DV0 BIT(4)
#define RTC_A_RS3 BIT(3)
#define RTC_A_RS2 BIT(2)
#define RTC_A_RS1 BIT(1)
#define RTC_A_RS0 BIT(0)

#define RTC_B_SET BIT(7)
#define RTC_B_PIE BIT(6)
#define RTC_B_AIE BIT(5)
#define RTC_B_UIE BIT(4)
#define RTC_B_SQUARE_WAVE BIT(3)
#define RTC_B_DM_BINARY BIT(2)
#define RTC_B_DM_BCD 0
#define RTC_B_2412_24 BIT(1)
#define RTC_B_2412_12 0
#define RTC_B_DSE BIT(0)

#define RTC_C_IRQ_ACTIVE BIT(7)
#define RTC_C_PER_INT_PENDING BIT(6)
#define RTC_C_ALARM_INT_PENDING BIT(5)
#define RTC_C_UPDATE_INT_PENDING BIT(4)

#define RTC_D_VALID_RAM_TIME BIT(7)

#endif
