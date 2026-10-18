.intel_syntax noprefix

.extern start

.section .notes, "a"

.long 4      # Owner name size ("Xen" + null termination)
.long 4      # Data size (size of the entry point value)
.long 18     # Type: XEN_ELFNOTE_PHYS32_ENTRY (0x12)
.asciz "Xen" # Owner name identifier
.long start  # Kernel 32-bit physical entry point address (defined in
             # `boot.asm`)
