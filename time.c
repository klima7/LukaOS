#include <stddef.h>
#include <stdint.h>
#include "time.h"
#include "cmos.h"
#include "acpi.h"
#include "heap.h"
#include "shell.h"
#include "clib/stdio.h"

// Funkcje statyczne
static uint8_t convert_bcd_to_bin(uint8_t bcd);
static uint8_t convert_hour_to_24_format(uint8_t hour12, uint8_t pm);
static void convert_time_to_bin(struct time_t *time_bcd);
static uint8_t compare_time(struct time_t *t1, struct time_t *t2);
static inline void __get_time(struct time_t *time);
static uint32_t calc_full_year_v1(uint8_t year);
static uint32_t calc_full_year_v2(uint8_t year);
static void time_command(const char* tokens, uint32_t tokens_count);

// Nazwy miesięcy i dni tygodnia
const char *months_names[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "Noveber", "December"};
const char *weekdays_names[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Po inicjalizacji zawiera informacje o obcności rejestru wieku - Century
uint8_t century_register = 0;

// Po inicjalizacji zawiera informacje o formacje czasu
uint8_t binary_mode = 0;
uint8_t hour_format_24 = 0;

// Inicjalizacja
void time_initialize(void)
{
    printf("Time initialization\n");
    // Sprawdza obecność rejestru wieku
    struct FADT *fadt = (struct FADT*)find_table("FACP");
    if(fadt == NULL) century_register = 0;
    else century_register = fadt->Century;

    // Uzupełnia informacje o formacie godziny
    uint8_t status = cmos_read_register(CMOS_REGISTER_STATUS_B);
    if(status & STATUS_REGISTERB_BINARY_MODE) binary_mode = 1;
    if(status & STATUS_REGISTERB_24_HOUR_FORMAT) hour_format_24 = 1;

    register_command("time", "Display current time", time_command);

    printf("\n______________Current_Time______________\n");
    struct time_t *time = NULL;
    get_time(time);
    time_display(time);
    printf("\n\n");
}

// Wypełnia strukturę aktualnym czasem
void get_time(struct time_t *time)
{
    struct time_t time1, time2;

    // Nie mamy pewności czy nie trwa aktualizacja czasu
    // Trzeba się upewnić czy odczytujemy poprawny czas
    do
    {
        __get_time(&time1);
        __get_time(&time2);

    } while(compare_time(&time1, &time2));

    // Zapisanie i usunięcie bitu pory dnia AM/PM
    uint8_t pm = time1.hours & HOURS_AM_PM;
    time1.hours = time1.hours & HOURS_HOURS;

    // Ewentualna konwersja do liczb binarnych
    if(!binary_mode) convert_time_to_bin(&time1);

    // Obliczenie pełnego roku
    if(century_register) time1.year = calc_full_year_v2(time1.year);
    else calc_full_year_v1(time1.year);

    // Poprawka dla tryby 12 godinnego
    if(!hour_format_24) time1.hours = convert_hour_to_24_format(time1.hours, pm);

    *time = time1;
}

// Wyświetla podaną godzinę
void time_display(struct time_t *time)
{
    printf("%s, %s %u ", weekdays_names[time->weekday], months_names[time->month], time->day_of_month);
    if(time->hours < 10) printf("0");
    printf("%u:", time->hours);
    if(time->minutes < 10) printf("0");
    printf("%u:", time->minutes);
    if(time->seconds < 10) printf("0");
    printf("%u ", time->seconds);
    printf("%u", time->year);
}

// Zamienia wartość zapiwaną w BCD na wartość binarną
static uint8_t convert_bcd_to_bin(uint8_t bcd)
{
    uint8_t binary = ((bcd & 0xF0)>>1)+((bcd & 0xF0)>>3)+(bcd & 0xf);
    return binary;
}

// Zamienia godzine z formatu 12 na 24
static uint8_t convert_hour_to_24_format(uint8_t hour12, uint8_t pm)
{
    // Past Morning
    if(pm)
    {
        // Poprawka na północ
        if(hour12 == 12) return 0;
        return 12 + hour12;
    }

    // Am Morning
    else return hour12; 
}

// Zgaduje rok na podstawie ostatnich cyft
static uint32_t calc_full_year_v1(uint8_t year)
{
    return 2000 + year;
}

// Oblicza pełny rok na podstawie rejestru wieku
static uint32_t calc_full_year_v2(uint8_t year)
{
    uint8_t century = convert_bcd_to_bin(cmos_read_register(century_register));
    uint32_t full_year = century * 100 + year;
    return full_year;
}

// Porównuje dwie struktury czasu
static uint8_t compare_time(struct time_t *t1, struct time_t *t2)
{
    if(t1->seconds != t2->seconds || t1->minutes != t2->minutes || t1->hours != t2->hours ||
    t1->weekday != t2->weekday || t1->day_of_month!= t2->day_of_month || t1->month != t2->month || t1->year != t2->year)
        return 1;

    return 0;
}

// Pobiera z z pamięci CMOS czas w wersji surowej, być może szesnastkowej i bez gwarancji poprawności przez trającą aktualizacje
static inline void __get_time(struct time_t *time)
{
    time->seconds = cmos_read_register(CMOS_REGISTER_SECONDS);
    time->minutes = cmos_read_register(CMOS_REGISTER_MINUTES);
    time->hours = cmos_read_register(CMOS_REGISTER_HOURS);
    time->weekday = cmos_read_register(CMOS_REGISTER_WEEKDAY);
    time->day_of_month = cmos_read_register(CMOS_REGISTER_DAY_OF_MONTH);
    time->month = cmos_read_register(CMOS_REGISTER_MONTH);
    time->year = cmos_read_register(CMOS_REGISTER_YEAR);
}

// Dokonuje konwersji wszystkich pól z BCD do wartości binarnych
static void convert_time_to_bin(struct time_t *time_bcd)
{
    time_bcd->seconds = convert_bcd_to_bin(time_bcd->seconds);
    time_bcd->minutes = convert_bcd_to_bin(time_bcd->minutes);
    time_bcd->hours = convert_bcd_to_bin(time_bcd->hours);
    time_bcd->weekday = convert_bcd_to_bin(time_bcd->weekday);
    time_bcd->day_of_month = convert_bcd_to_bin(time_bcd->day_of_month);
    time_bcd->month = convert_bcd_to_bin(time_bcd->month);
    time_bcd->year = convert_bcd_to_bin(time_bcd->year);
}

// Komenda wyświetlająca aktualny czas
static void time_command(const char* tokens, uint32_t tokens_count)
{
    struct time_t time;
    get_time(&time);
    time_display(&time);
    printf("\n");
}