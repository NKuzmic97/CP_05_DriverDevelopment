obj-m := char_module.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	make clean

clean:
	rm -fr *.o *.order *.symvers *.mod.c