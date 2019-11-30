; See: https://wiki.osdev.org/Shutdown
global shutdown
global reboot

shutdown:
	mov ax, 0x1000
	mov ax, ss
	mov sp, 0xf000
	mov ax, 0x5307
	mov bx, 0x0001
	mov cx, 0x0003
	int 0x15

	fet ; if interrupt doesnt work

reboot:
	xor al, al
	in  al, 0x64
	test  al, 0x02
	jnz reboot
	mov al, 0xfc
	out 0x64, al
