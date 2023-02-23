all:
	make -C Consola
	make -C Kernel
	make -C CPU
	make -C MemSwap

clean:
	make clean -C Consola
	make clean -C Kernel
	make clean -C CPU
	make clean -C MemSwap