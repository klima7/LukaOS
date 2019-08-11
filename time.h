#ifndef __TIME_H__
#define __TIME_H__

#include <stddef.h>
#include <stdint.h>

// Interpretacja zawartości rejestru statusu
#define STATUS_REGISTERB_24_HOUR_FORMAT 0x02
#define STATUS_REGISTERB_BINARY_MODE 0x04

// Interpretacja rejestru godziny
#define HOURS_AM_PM 0x80
#define HOURS_HOURS 0x7F

// Dni tygodnia
enum weekday_t { SUNDAY=1, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY };

// Miesiące
enum month_t { JANUARY=1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER };

// Czas
struct time_t
{
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
    enum weekday_t weekday;
    uint32_t day_of_month;
    enum month_t month;
    uint32_t year;
};

// Prototypy
void time_initialize(void);
void get_time(struct time_t *time);
void time_display(struct time_t *time);
uint32_t time_hash(void);

#endif
