#include <stddef.h>
#include <stdint.h>
#include "sys.h"
#include "acpi.h"
#include "clib/stdio.h"
#include "clib/string.h"

// Funkcje statyczne
static struct RSDP_descriptor20 *find_RSDP(void);
static int RSDP_veryfi(struct RSDP_descriptor20 *rsdp);
static int RSDT_veryfi(struct ACPISDT_header *header);
static int table_veryfi(struct ACPISDT_header *header);

// RSDP i RSDT
struct RSDP_descriptor20 *rsdp;
struct ACPISDT_header *rsdt;

// Szuka w pamięci struktura RSDP
void acpi_initialize(void)
{
    printf("ACPI Initialization\n");

    // Szukanie struktury RSDP
    rsdp = find_RSDP();
    if(rsdp) printf("RSDP Found\n");
    else kernel_panic("Unable To Find RSDP\n");

    // Sprawdzenie czy struktura jest poprawna
    int check = RSDP_veryfi(rsdp);
    if(check) 
    {
        if(rsdp->first_part.revision == ACPI10 )printf("ACPI 1.0 Verified\n");
        else if(rsdp->first_part.revision == ACPI20 )printf("ACPI 2.0 Verified\n");
    }
    else kernel_panic("RSDP Veryfication Failed\n");

    // W zależności od wersji ACPI wybiera inny adres
    if(rsdp->first_part.revision == ACPI10) rsdt = (struct ACPISDT_header *)rsdp->first_part.rsdt_address;
    else if(rsdp->first_part.revision == ACPI20) rsdt = (struct ACPISDT_header *)(uint32_t)rsdp->xsdt_address;

    check = RSDT_veryfi(rsdt);
    if(check) printf("RSDT Verified\n");
    else kernel_panic("RSDT Verification Failed\n");
}

// Szuka tabeli o podanej sygnaturze w tablicy RSDT
uint32_t find_table(const char *signature)
{
    uint32_t entry_size = rsdp->first_part.revision == ACPI10 ? 4 : 8;
    uint32_t entries = ( rsdt->length - (uint32_t)sizeof(struct ACPISDT_header) ) / entry_size;
    uint32_t first_entry = (uint32_t)rsdt + (uint32_t)sizeof(struct ACPISDT_header);
    
    for(uint32_t i=0; i<entries; i++)
    {
        uint32_t *temp = (uint32_t*)(first_entry + i * entry_size);
        uint32_t address = *temp;
        char *ptr = (char*)address;
        if(strncmp(ptr, signature, 4)==0) 
        {
            // Adres tabeli jest zwracany tylko gdy jest ona poprawna
            struct ACPISDT_header *h = (struct ACPISDT_header*)address;
            if(table_veryfi(h)) return address;
        }
    }
    return 0;
}

// Zwraca wersje ACPI
int get_ACPI_version(void)
{
    if(rsdp->first_part.revision == ACPI10) return ACPI10;
    else return ACPI20;
}

// Szuka struktury RSDP w pamięci RAM
static struct RSDP_descriptor20 *find_RSDP(void)
{
    const char *search = "RSD PTR ";

    // Szuka struktury w pierwszym obszarze
    uint32_t current = SEARCH1_START;

    while(current<SEARCH1_END)
    {
        int ok_flag = 1;

        for(int i=0; i<8; i++)
        {
            char *str = (char*)current;
            if(*(str+i)!=*(search+i)) { ok_flag=0; break; }
        }

        if(ok_flag)
            return (struct RSDP_descriptor20*)current;

        current++;
    }

    // Szuka struktury w drugin obszarze
    current = SEARCH2_START;

    while(current<SEARCH2_END)
    {
        int ok_flag = 1;

        for(int i=0; i<8; i++)
        {
            char *str = (char*)current;
            if(*(str+i)!=*(search+i)) { ok_flag=0; break; }
        }

        if(ok_flag)
            return (struct RSDP_descriptor20*)current;

        current++;
    }

    return NULL;
}

// Sprawdza sumę kontrolną dla struktury RSDP
static int RSDP_veryfi(struct RSDP_descriptor20 *rsdp)
{
    if(rsdp->first_part.revision == ACPI10 || rsdp->first_part.revision == ACPI20)
    {
        uint32_t size = (uint32_t)sizeof(struct RSDP_descriptor);
        uint8_t *byte = (uint8_t*)rsdp;
        uint32_t sum = 0;

        for(uint32_t i=0; i<size; i++) 
        {
            sum += *byte;
            byte++;
        }

        if((sum & 0xFF) != 0 ) return 0;
    }

    else if(rsdp->first_part.revision == ACPI20)
    {
        uint32_t size = (uint32_t)sizeof(struct RSDP_descriptor20) - (uint32_t)sizeof(struct RSDP_descriptor);
        uint8_t *byte = (uint8_t*)&(rsdp->length);
        uint8_t sum = 0;

        for(uint32_t i=0; i<size; i++) sum += *byte++;

        if(sum != 0 ) return 0;
    }

    else 
        return 0;

    return 1;
}

// Sprawdza sumę kontrolną dla struktury RSDT
static int RSDT_veryfi(struct ACPISDT_header *header)
{
    uint32_t size = header->length;
    uint8_t *byte = (uint8_t*)header;
    uint8_t sum = 0;
 
    for (uint32_t i = 0; i < size; i++) sum += *byte++;
    
    return sum == 0;
}

// Sprawdza sumę kontrolną dla tabeli
static int table_veryfi(struct ACPISDT_header *header)
{
    uint32_t size = header->length;
    uint8_t *byte = (uint8_t*)header;
    uint8_t sum = 0;
 
    for (uint32_t i = 0; i < size; i++) sum += *byte++;
    
    return sum % 0x100 == 0;
}