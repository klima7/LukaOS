.text

.global check_true_rng
.global generate_true_numer

// Sprawdza, czy procesor obsługuje polecenie rdrand
.type check_true_rng, @function
check_true_rng:
    pushl %ecx

    mov $0x1, %eax
    mov $0x0, %ecx
    cpuid
    shr $0x30, %ecx
    and $0x1, %ecx
    mov %ecx, %eax

    pop %ecx
    ret

// Zwraca prawdziwie losową liczbę
.type generate_true_numer, @function
generate_true_numer:
    rdrand %eax
    ret


