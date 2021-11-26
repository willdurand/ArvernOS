setenv bootargs kshell
load mmc 0:1 0x80008000 uImage
bootm 0x80008000
