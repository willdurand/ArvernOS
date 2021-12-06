.global get_el

get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret
