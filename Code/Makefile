include common.mk

SECTIONS = \
	io \
	kernel \
	mem_management \
	task_schedule \
	platform \
	interrupt_and_trap \
	obj \

.DEFAULT_GOAL := all
all :
	@echo "begin compile ALL files for assembly samples ......................."
	$(shell mkdir -p $(TOP_DIR)/bin)
	for dir in $(SECTIONS); do $(MAKE) -C $$dir || exit "$$?"; done
	@echo "compile ALL files finished successfully! ......"

.PHONY : clean
clean:
	for dir in $(SECTIONS); do $(MAKE) -C $$dir clean || exit "$$?"; done


run: $(OUT_PUT)/os.elf
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel $(OUT_PUT)/os.elf

.PHONY : debug
debug: $(OUT_PUT)/os.elf
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel $(OUT_PUT)/os.elf -s -S &
	@${GDB} $(OUT_PUT)/os.elf -q -x gdbinit

.PHONY : code
code: $(OUT_PUT)/os.elf
	@${OBJDUMP} -S $(OUT_PUT)/os.elf | less

.PHONY : clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(OUT_PUT)

