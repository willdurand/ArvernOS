.global get_el

get_el:
	mrs r1, cpsr
	and r0, r1, #0x1f
	bx lr
