// Stałe niezbędne dla MULTIBOOT 
.set 		ALIGN,    	0x01        
.set 		MEMINFO,  	0x10             
.set 		FLAGS,    	ALIGN | MEMINFO  
.set 		MAGIC,    	0x1BADB002       
.set 		CHECKSUM, 	-(MAGIC + FLAGS) 
 
// Zapisanie w pamięci wcześniej zdefiniowanych stałych.
// Bootloader szuka wartości MAGIC i odczytuje dwie kolejne 
// Musi znajdować się w pierwszych 8kb programu jądra
.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	 
// Tworzy stos o rozmiarze 16kb.
// Wygamane jest aby był wyrówanany do 16 bajtów
// Stos rośnie w dół - rejestr ESP jest dekrementowany
.section .bss
	.align 16
	stack_bottom:
	.skip 16384 # 16 KiB
	stack_top:							

// Rozmiar i adres globalnej tablicy deskryptorów GDT
gdtr:
.word 0
.long 0

// Rozmiar i adres tablicy deskryptorów przerwań IDT
idtr:
.word 0
.long 0

// Adres struktury multiboot
multiboot_info:
.long 0

.text

// Zwraca adres struktury multiboot_info
// Wywoływane z poziomu C - unsigned int get_multibot_info(void)
.global get_multibot_info
.type get_multibot_info, @function
get_multibot_info:
	movl multiboot_info, %eax
	ret

// Ładuje rozmiar i adres globalnej tablicy deskryptorów do rejestru GDTR
// Wywoływane z poziomu C - set_GDT(GDT, sizeof(GDT)-1)
.global set_GDT
.type set_GDT, @function
set_GDT:
   mov   4(%esp), %eax
   mov   %eax, gdtr+2
   mov   8(%esp), %ax
   mov   %ax, gdtr
   lgdt  gdtr
   ret

// Ładuje rozmiar i adres tablicy deskryptorów przerwań do rejestru IDTR
// Wywoływane z poziomu C - set_IDT(IDT, sizeof(IDT)-1)
.global set_IDT
.type set_IDT, @function
set_IDT:
   mov   4(%esp), %eax
   mov   %eax, idtr+2
   mov   8(%esp), %ax
   mov   %ax, idtr
   lidt  idtr
   sti
   ret

// Ładuje selektory do rejestrów segmentowych procesora
// Wywoływanie z poziomu C - reload_segments()
.global reload_segments   
.type reload_segments, @function
reload_segments:
	ljmp   $0x8, $reload_CS
.global reload_CS
reload_CS:
   MOV   $0x10, %ax
   MOV   %ax, %ds
   MOV   %ax, %es
   MOV   %ax, %fs
   MOV   %ax, %gs
   MOV   %ax, %ss
   RET
   
// Zapisuje wartość do danego portu
// Wywoływane z poziomu C - void outportb(unsigned char port, unsigned char value)
// Uwaga! Przed odłożeniem na stos char jest rozszerzany do int!
.global outportb
.type outportb, @function
outportb:
	mov 4(%esp), %dx
	mov 8(%esp), %al
	out %al, %dx
	ret
	
// Odczytuje wartość z podanego rejestru
// Wywoływane z poziomu C - unsigned char inportb(unsigned char port)
.global inportb
.type inportb, @function
inportb:
	mov 4(%esp), %dx
	in %dx, %al
	ret

// Zwraca stan licznika PIT	
// Wywoływane z poziomu C - unsigned short read_pit_count(void)
.global read_PIT_count
.type read_PIT_count, @function
read_PIT_count:
	pushf									// Zrzucamy na stos rejestr EFLAG, 4 bajty - teraz parametry są głębiej
	cli									
	mov $0, %al   
	out %al, $0x43   					// Polecenie Counter Latch      
	in $0x40, %al        			// Odczyt pierwszego bajtu
	mov %al, %ah     			
	in $0x40, %al       			 	// Odczyt drugiego bajtu 
	rol $8, %ax  
	sti       
	popf
	ret
	
// Ustala stan licznika PIT
// Wywoływane z poziomu C - void set_PIT_count(unsigned short count)
.global set_PIT_count
.type set_PIT_count, @function
set_PIT_count:
	pushf
	cli
	mov 8(%esp), %al
	out %al, $0x40        			// Set low byte of reload value
	rol $8, %ax           			// al = high byte, ah = low byte
	mov 9(%esp), %al
	out %al, $0x40        			// Set high byte of reload value
	rol $8, %ax           			// al = low byte, ah = high byte (ax = original reload value)
	sti
	popf
	ret
   
// Tryb pracy procesora jest zmieniany na tryb chroniony
// Ustawiamy bit PE - pierwszy bit w rejestrze CR0
// Wyłącza to adresowanie płaskie i umożliwia mechanizmy takie jak stronicowanie i segmentacja
.type enter_protection_mode, @function
enter_protection_mode:
	mov	%cr0, %eax
	or	$0x01, %eax
	mov	%eax, %cr0
	ret
 
// Wykonywanie rozpoczyna się od tej procedury
.global _start
.type _start, @function			
_start:
	mov %ebx, multiboot_info
	cli 									// Wyłącza przerwania
	call enter_protection_mode		// Procesor przechodzi w tryb chroniony
	mov $stack_top, %esp		  		// Umieszcza adres stosu w rejestrze wskaźnika stosu - ESP
	call kernel_main			   	// Wywołuje funkcje kernel_main()
			
1:	hlt     				  				// Czeka na nadejście przerwania
	jmp 1b								// Nieskończona pętla
 
.size _start, . - _start		

// Zewnętrzn procedury obsługi przerwań
// Umieszczone w pliku idt.c
.extern IRQ00_handler
.extern IRQ01_handler
.extern IRQ02_handler
.extern IRQ03_handler
.extern IRQ04_handler
.extern IRQ05_handler
.extern IRQ06_handler
.extern IRQ07_handler
.extern IRQ08_handler
.extern IRQ09_handler
.extern IRQ10_handler
.extern IRQ11_handler
.extern IRQ12_handler
.extern IRQ13_handler
.extern IRQ14_handler
.extern IRQ15_handler

// Udostępnianie funkcji przerwań dla C
// Ich adresy sa potrzebne do stworzenia tablicy IDT w pliku idt.c
.global IRQ00
.global IRQ01
.global IRQ02
.global IRQ03
.global IRQ04
.global IRQ05
.global IRQ06
.global IRQ07
.global IRQ08
.global IRQ09
.global IRQ10
.global IRQ11
.global IRQ12
.global IRQ13
.global IRQ14
.global IRQ15

// Procedury obsługi przerwań 
// Najpierw umieszczają na stosie zawartości rejestrów ogólnego przeznaczenia
// Wywołują odpowiednią funkcję C i przewracają poprzedni stan rejestrów
IRQ00:
  pusha
  call IRQ00_handler
  popa
  iret
 
IRQ01:
  pusha
  call IRQ01_handler
  popa
  iret
 
IRQ02:
  pusha
  call IRQ02_handler
  popa
  iret
 
IRQ03:
  pusha
  call IRQ03_handler
  popa
  iret
 
IRQ04:
  pusha
  call IRQ04_handler
  popa
  iret
 
IRQ05:
  pusha
  call IRQ05_handler
  popa
  iret
 
IRQ06:
  pusha
  call IRQ06_handler
  popa
  iret
 
IRQ07:
  pusha
  call IRQ07_handler
  popa
  iret
 
IRQ08:
  pusha
  call IRQ08_handler
  popa
  iret
 
IRQ09:
  pusha
  call IRQ09_handler
  popa
  iret
 
IRQ10:
  pusha
  call IRQ10_handler
  popa
  iret
 
IRQ11:
  pusha
  call IRQ11_handler
  popa
  iret
 
IRQ12:
  pusha
  call IRQ12_handler
  popa
  iret
 
IRQ13:
  pusha
  call IRQ13_handler
  popa
  iret
 
IRQ14:
  pusha
  call IRQ14_handler
  popa
  iret
 
IRQ15:
  pusha
  call IRQ15_handler
  popa
  iret







