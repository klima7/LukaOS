.text

// Pobiera/zapisuje jeben bajt do portu - byte
.global outportb
.global inportb

// Pobiera/zapisuje dwa bajty do portu - word
.global outportw
.global inportw

// Pobiera/zapisuje cztery bajty do portu - double word
.global outportdw
.global inportdw


.type outportb, @function
outportb:
	mov 4(%esp), %dx
	mov 8(%esp), %al
	out %al, %dx
	ret
	

.type inportb, @function
inportb:
	mov 4(%esp), %dx
	in %dx, %al
	ret


.type outportw, @function
outportw:
	mov 4(%esp), %dx
	mov 8(%esp), %ax
	out %ax, %dx
	ret


.type inportw, @function
inportw:
	mov 4(%esp), %dx
	in %dx, %ax
	ret


.type outportdw, @function
outportdw:
	mov 4(%esp), %dx
	mov 8(%esp), %eax
	out %eax, %dx
	ret


.type inportdw, @function
inportdw:
	mov 4(%esp), %dx
	in %dx, %eax
	ret
