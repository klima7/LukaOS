#include <stddef.h>
#include <stdint.h>
#include "pic.h"
#include "ports.h"

// Inicjuje PIC - przemapowuje
void PIC_remap(int offset1, int offset2)
{ 
    // Zapisuje maski, żeby się nie popsuły
	uint8_t temp1 = inportb(PIC1_DATA);
	uint8_t temp2 = inportb(PIC2_DATA);
 
    // Rozpoczyna proces inicjalizacji
	outportb(PIC1_COMMAND, 0x11); 
	outportb(PIC2_COMMAND, 0x11);

    // Wysyła pierwszy bajt - offsety w tablicy idt
	outportb(PIC1_DATA, offset1);                 
	outportb(PIC2_DATA, offset2);                 

    // Wysyła drugi bajt - informacja o połączeniu
	outportb(PIC1_DATA, 4);                       
	outportb(PIC2_DATA, 2);                       
 
    // Wysyła trzeci bajt - informacja o środowisku
	outportb(PIC1_DATA, ICW4_8086);
	outportb(PIC2_DATA, ICW4_8086);
 
    // Przewraca maski
	outportb(PIC1_DATA, temp1);   
	outportb(PIC2_DATA, temp2);
}

// Ustawia maskowanie przerwania
void PIC_set_mask(uint8_t irq)
{
    uint8_t port = 0;

    if(irq<8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t mask = inportb(port);
    mask = mask | (0x1 << irq);
    outportb(port, mask);
}

// Usówa maskowanie przerwania
void PIC_clear_mask(uint8_t irq)
{
    uint8_t port = 0;

    if(irq<8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t mask = inportb(port);
    mask = mask & ~(0x1 << irq);
    outportb(port, mask);
}

// Powiadamia kontroler/kontrolery o obsłużeniu przerwania
void PIC_end_notify(uint8_t irq)
{
    if(irq<8)
    {
        outportb(PIC1_COMMAND, COMMAND_IRQ_END);
    }
    else
    {
          outportb(PIC2_COMMAND, COMMAND_IRQ_END);
          outportb(PIC1_COMMAND, COMMAND_IRQ_END);
    }
}

// Zwraca zawartość rejestru IST obu kontrolerów, przyda się do wyłapywania fałszywych przerwań
uint16_t PIC_get_isr(void)
{
    outportb(PIC1_COMMAND, COMMAND_READ_ISR);
    outportb(PIC2_COMMAND, COMMAND_READ_ISR);
    uint8_t isr1 = inportb(PIC1_COMMAND);
    uint8_t isr2 = inportb(PIC1_COMMAND);
    uint16_t combined = isr1 | isr2 << 8;
    return combined;
}

// Zwraca zawartość rejestru irr obu kontrolerów - przerwanie które czeka na obsłużenie
uint16_t PIC_get_irr(void)
{
    outportb(PIC1_COMMAND, COMMAND_READ_IRR);
    outportb(PIC2_COMMAND, COMMAND_READ_IRR);
    uint8_t isr1 = inportb(PIC1_COMMAND);
    uint8_t isr2 = inportb(PIC1_COMMAND);
    uint16_t combined = isr1 | isr2 << 8;
    return combined;
}