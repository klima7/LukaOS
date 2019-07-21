.extern interrupt_handler

// Przerwania wewnętrzne
.global int0;  int0:  pushal; pushl $0;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int1;  int1:  pushal; pushl $1;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int2;  int2:  pushal; pushl $2;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int3;  int3:  pushal; pushl $3;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int4;  int4:  pushal; pushl $4;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int5;  int5:  pushal; pushl $5;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int6;  int6:  pushal; pushl $6;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int7;  int7:  pushal; pushl $7;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int8;  int8:  pushal; pushl $8;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int9;  int9:  pushal; pushl $9;  call interrupt_handler; addl $0x04, %esp; popal; iret
.global int10; int10: pushal; pushl $10; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int11; int11: pushal; pushl $11; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int12; int12: pushal; pushl $12; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int13; int13: pushal; pushl $13; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int14; int14: pushal; pushl $14; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int15; int15: pushal; pushl $15; call interrupt_handler; addl $0x04, %esp; popal; iret

.global int16; int16: pushal; pushl $16; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int17; int17: pushal; pushl $17; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int18; int18: pushal; pushl $18; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int19; int19: pushal; pushl $19; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int20; int20: pushal; pushl $20; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int21; int21: pushal; pushl $21; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int22; int22: pushal; pushl $22; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int23; int23: pushal; pushl $23; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int24; int24: pushal; pushl $24; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int25; int25: pushal; pushl $25; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int26; int26: pushal; pushl $26; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int27; int27: pushal; pushl $27; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int28; int28: pushal; pushl $28; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int29; int29: pushal; pushl $29; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int30; int30: pushal; pushl $30; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int31; int31: pushal; pushl $31; call interrupt_handler; addl $0x04, %esp; popal; iret

// Przerwania zewnętrzne - sprzętowe
.global int32;  int32: pushal; pushl $32; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int33;  int33: pushal; pushl $33; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int34;  int34: pushal; pushl $34; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int35;  int35: pushal; pushl $35; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int36;  int36: pushal; pushl $36; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int37;  int37: pushal; pushl $37; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int38;  int38: pushal; pushl $38; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int39;  int39: pushal; pushl $39; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int40;  int40: pushal; pushl $40; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int41;  int41: pushal; pushl $41; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int42;  int42: pushal; pushl $42; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int43;  int43: pushal; pushl $43; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int44;  int44: pushal; pushl $44; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int45;  int45: pushal; pushl $45; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int46;  int46: pushal; pushl $46; call interrupt_handler; addl $0x04, %esp; popal; iret
.global int47;  int47: pushal; pushl $47; call interrupt_handler; addl $0x04, %esp; popal; iret























