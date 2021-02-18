ifneq ($(KERNELRELEASE),)
	obj-m := chardevice.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko .*.cmd .*.swp *.mod.c .tmp_versions Module.symvers modules.order
endif