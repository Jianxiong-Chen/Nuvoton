KERNELDIR := /home/cjx/workspace/cjx/Nuvoton/Arm9/NUC980/nuc980-src/NUC980-linux-4.4.y/
CURRENT_PATH := $(shell pwd)

obj-m := att7022.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
