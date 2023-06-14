.weak trap_vector
.balign 8,0
.global trap_vector

.text


.option norvc
.globl __mtvec_vector_table
.balign 256,0
__mtvec_vector_table:

IRQ_0:
        j trap_vector
IRQ_1:
        j default_vector_handler
IRQ_2:
        j default_vector_handler
IRQ_3:
        j default_vector_handler #Machine_software_handler
IRQ_4:
        j default_vector_handler        
IRQ_5:
        j default_vector_handler
IRQ_6:
        j default_vector_handler
IRQ_7:
        j Machine_timer_handler
IRQ_8:
        j default_vector_handler
IRQ_9:
        j default_vector_handler
IRQ_10:
        j default_vector_handler
IRQ_11:
        j __Machine_external_handler
IRQ_12:
        j default_vector_handler
IRQ_13:
        j default_vector_handler
IRQ_14:
        j default_vector_handler
IRQ_15:
        j default_vector_handler

.end