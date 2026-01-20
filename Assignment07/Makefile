
obj-m += ft_debugfs_module.o

ft_debugfs_module-y := ft_debugfs.o foo.o jiffies.o id.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean